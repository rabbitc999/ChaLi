/**
 * @file macro_control_flow.cpp
 * @brief 按语法来源选择宏控制出口，独立于展开后新增的物理嵌套。
 */
/**
 * @brief 读取首次展开时固定的控制来源，未展开的普通节点沿用卫生上下文。
 * @param node 源语法节点。
 * @return 当前控制来源的上下文身份。
 */
std::uint32_t MacroControlContext(const MNode &node)
{
    return node.controlContext == UINT32_MAX ? node.syntaxContext : node.controlContext;
}

/**
 * @brief 宏展开后的退出仍受显式 return 控制表达式的原有约束。
 * @param node 返回、传播或层级退出节点。
 * @return 未越过受保护的返回控制表达式时返回真。
 */
bool CheckMacroReturnExpressionControl(const MNode &node)
{
    bool protectedReturn = false;
    for (const auto &context : g_blockStack) protectedReturn = protectedReturn || context.returnExpression;
    if (!protectedReturn) return true;
    if (node.kind == MNode::Return) return Fail("return 表达式内部禁止嵌套 return；请使用 yield 值;");
    if (node.kind == MNode::Try) return Fail("return 表达式内部禁止使用会提前返回的 ?；请显式匹配并使用 yield 值;");
    if (node.kind == MNode::Break && node.i != 0) return Fail("return 表达式内部禁止 break #level 跳转");
    return true;
}

/**
 * @brief 将模板上下文沿所属展开帧退回调用方，只允许语句或旧式宏的模板穿透。
 * @param node 出口语法节点。
 * @param context 当前卫生上下文，成功时改为调用方上下文。
 * @param expressionBoundary 可选输出；遇到表达式边界时置真，不立即报告错误。
 * @return 存在允许穿透的所属展开时返回真。
 */
bool MacroControlCaller(const MNode &node, std::uint32_t &context, bool *expressionBoundary = nullptr)
{
    if (expressionBoundary) *expressionBoundary = false;
    int frameId = node.macroExpansion;
    for (unsigned depth = 0; frameId >= 0 && depth < 256; ++depth)
    {
        if (static_cast<std::size_t>(frameId) >= g_nodes.size()) return Fail("宏控制流来源帧无效");
        const auto frame = N(frameId);
        if (frame.kind != MNode::MacroExpansionFrame || frame.i < 0 || static_cast<std::size_t>(frame.i) >= g_nodes.size())
            return Fail("宏控制流定义来源无效");
        if (frame.bindingContext == context && context != 0)
        {
            if (frame.i >= 0 && N(static_cast<int>(frame.i)).s2 == "expr")
            {
                if (expressionBoundary)
                {
                    *expressionBoundary = true;
                    return false;
                }
                return Fail("expr 宏模板不能把控制流转移到调用方；请使用 stmt 宏，或由调用方显式传入控制片段");
            }
            context = MacroControlContext(frame);
            return true;
        }
        frameId = frame.kids.empty() ? -1 : frame.kids[0];
    }
    return false;
}

/**
 * @brief 在同一语法来源内查找循环或值出口，再按宏契约逐层回到调用方。
 * @param id break、continue 或带值退出节点。
 * @param continuation 是否查找 continue 目标。
 * @return 找到的活动出口；不存在或契约拒绝时为空。
 */
MBlockContext *FindMacroControlTarget(int id, bool continuation)
{
    const auto node = N(id);
    if (!CheckMacroReturnExpressionControl(node)) return nullptr;
    auto context = MacroControlContext(node);
    const bool value = !continuation && !node.kids.empty();
    long long level = continuation ? 0 : node.i;
    MBlockContext *allTarget = nullptr;
    for (unsigned depth = 0; depth < 256; ++depth)
    {
        MBlockContext *outermost = nullptr;
        for (auto current = g_blockStack.rbegin(); current != g_blockStack.rend(); ++current)
        {
            if (node.macroExpansion >= 0 && (current->sourceNode < 0 || MacroControlContext(N(current->sourceNode)) != context)) continue;
            if (continuation)
            {
                if (!current->continueLabel.empty()) return &*current;
            }
            else if (level != 0)
            {
                if (current->continueLabel.empty()) continue;
                outermost = &*current;
                if (level > 0 && --level == 0) return outermost;
            }
            else if (value ? !current->breakValue.empty() : !current->valueOnly)
                return &*current;
        }
        if (level < 0)
        {
            if (outermost) allTarget = outermost;
            bool expressionBoundary = false;
            if (MacroControlCaller(node, context, &expressionBoundary)) continue;
            if (!g_error.empty()) return nullptr;
            if (expressionBoundary && !allTarget)
                return Fail("expr 宏模板不能把控制流转移到调用方；请使用 stmt 宏，或由调用方显式传入控制片段"), nullptr;
            return allTarget;
        }
        if (!MacroControlCaller(node, context)) return nullptr;
    }
    return Fail("宏控制流来源超过展开深度预算"), nullptr;
}

/**
 * @brief 核对 return 或传播运算符的函数归属，禁止片段跨新生成函数返回。
 * @param id 返回或传播节点。
 * @return 来源可到达当前函数边界时返回真。
 */
bool CheckMacroFunctionControl(int id)
{
    const auto node = N(id);
    if (!CheckMacroReturnExpressionControl(node)) return false;
    if (node.macroExpansion < 0)
    {
        g_nodes[id].controlTarget = g_functionControlNode;
        return true;
    }
    auto context = MacroControlContext(node);
    for (unsigned depth = 0; depth < 256; ++depth)
    {
        if (context == g_functionControlContext)
        {
            g_nodes[id].controlTarget = g_functionControlNode;
            return true;
        }
        if (!MacroControlCaller(node, context))
            return g_error.empty() ? Fail("宏控制片段不能跨越新生成的函数边界返回或传播错误") : false;
    }
    return Fail("宏函数控制流来源超过展开深度预算");
}
