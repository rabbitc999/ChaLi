/**
 * @brief 将元组下标映射到既有字段布局，不把异构元组当作同型数组。
 * @param id 下标表达式编号；嵌套数组下标从内向外按实际类型处理。
 * @return 元组下标为范围内整数常量时为真；其它普通下标保持原状。
 */
bool NormalizeTupleIndex(int id)
{
    if (id < 0 || N(id).kind != MNode::Index || N(id).s == "__chali_buffer_index") return true;
    const auto node = N(id);
    const auto type = ExprType(node.kids[0]);
    if (!IsTupleType(type)) return true;
    MConstant index;
    if (!EvaluateConstant(node.kids[1], &index)) return false;
    if (index.kind != MConstant::Integer)
        return Fail(CHALI_TEXT("元组下标必须是整数常量表达式"));
    if (index.integer < 0 || static_cast<unsigned long long>(index.integer) >= type->paramTypes.size())
        return Fail(CHALI_TEXT("元组下标超出范围：") + Memory::toString(index.integer));
    auto &normalized = g_nodes[static_cast<std::size_t>(id)];
    normalized.kind = MNode::Member;
    normalized.s = "__" + Memory::toString(index.integer);
    normalized.s2.clear();
    normalized.i = index.integer;
    normalized.kids = { node.kids[0] };
    return true;
}
