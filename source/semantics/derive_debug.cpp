/**
 * @file derive_debug.cpp
 * @brief 生成调试显示的普通 ChaLi 方法与 UTF-8 安全转义辅助函数。
 */

/**
 * @brief 读取标准 Debug 接口经模块消歧后的实际声明名。
 * @return 已解析接口名；未加载标准库时保留限定名以产生缺失声明诊断。
 */
Memory::String DerivedDebugInterfaceName()
{
    for (const auto id : g_top)
        if (N(id).kind == MNode::InterfaceDecl && N(id).module == "std::lang" && BareTypeName(N(id).s) == "Debug") return N(id).s;
    return "std::lang::Debug";
}

/**
 * @brief 生成单个值的调试表达式，聚合必须通过 Debug 协议。
 * @param type 已解析字段类型。
 * @param value 读取表达式。
 * @return 普通 ChaLi 表达式；不执行动态字段查询。
 */
Memory::String DerivedDebugExpression(const std::shared_ptr<MType> &type, const Memory::String &value)
{
    if (IsStandardOpcode(type, "String") || (type->kind == MType::Struct && type->name == "std::lang::String"))
        return "__chali_derive_debug_quote(" + value + ",false)";
    if (type->kind == MType::Char)
        return "__chali_derive_debug_quote(std::lang::format(\"{}\"," + value + "),true)";
    if (type->kind == MType::Bool) return "__chali_derive_debug_bool(" + value + ")";
    if (type->kind == MType::Int || type->kind == MType::Double)
        return "std::lang::format(\"{}\"," + value + ")";
    return value + ".debug()";
}

/**
 * @brief 为结构体或枚举生成按声明顺序显示字段的实例方法。
 * @param declaration 具体类型声明。
 * @param output 接收生成源码。
 * @return 类型布局可用于调试派生时返回真。
 */
bool DerivedDebugMethodSource(const MNode &declaration, Memory::String &output)
{
    MReflectionBuilder metadata;
    metadata.indexDeclarations();
    auto display = metadata.named(declaration.s);
    if (display.rfind("program::", 0) == 0) display = display.substr(9);
    const auto quote = MReflectionQueries::quote;
    output += "std::lang::String debug(){";
    if (declaration.kind == MNode::StructDecl)
    {
        output += "std::lang::String __chali_debug=" + quote(display + " {") + ";";
        bool first = true;
        for (const auto id : N(declaration.kids[0]).kids)
        {
            const auto &field = N(id);
            if (field.kind != MNode::Field || field.type2 == 1) continue;
            if (field.i >= 0) return Fail("#derive(Debug) 尚无数组字段的接口契约：" + declaration.s + "." + field.s);
            output += "__chali_debug.append(" + quote((first ? " " : ", ") + field.s + ": ") + ");";
            output += "__chali_debug.append(" + DerivedDebugExpression(T(field.type), "self." + field.s) + ");";
            first = false;
        }
        output += "__chali_debug.append(" + quote(first ? "}" : " }") + ");return __chali_debug;}\n";
        return true;
    }
    output += "match(self){";
    for (const auto id : N(declaration.kids[0]).kids)
    {
        const auto &variant = N(id);
        Memory::Vector<Memory::String> bindings;
        output += DerivedVariantPattern(declaration.s, variant, "chaliDebugPayload", bindings) + "=>{";
        output += "std::lang::String __chali_debug=" + quote(display + "::" + variant.s + (bindings.empty() ? "" : "(")) + ";";
        for (std::size_t index = 0; index < bindings.size(); ++index)
        {
            const auto &payload = N(N(variant.kids[0]).kids[index]);
            output += "__chali_debug.append(" + quote((index ? ", " : "") + payload.s + ": ") + ");";
            output += "__chali_debug.append(" + DerivedDebugExpression(T(payload.type), bindings[index]) + ");";
        }
        if (!bindings.empty()) output += "__chali_debug.append(\")\");";
        output += "return __chali_debug;}";
    }
    output += "}}\n";
    return true;
}

/**
 * @brief 生成按字节识别 ASCII 转义、按完整 UTF-8 段复制文本的辅助函数。
 * @return ChaLi 源码；所有缓冲区仍按普通值语义清理。
 */
Memory::String DerivedDebugHelpers()
{
    return R"CHL(
std::lang::String __chali_derive_debug_bool(bool value){
    if(value){return "true";}
    return "false";
}
std::lang::String __chali_derive_debug_quote(std::lang::String value,bool character){
    std::lang::String quote="\"";
    if(character){quote="'";}
    std::lang::String result=quote;
    std::lang::String hex="0123456789abcdef";
    int64 begin=0;
    int64 index=0;
    while(index<value.length()){
        uint8 byte=value.byteAt(index).unwrap();
        if(byte<32||byte==127||byte==92||(!character&&byte==34)||(character&&byte==39)){
            result.append(value.substring(begin,index-begin));
            if(byte==10){result.append("\\n");}
            elif(byte==13){result.append("\\r");}
            elif(byte==9){result.append("\\t");}
            elif(byte==92){result.append("\\\\");}
            elif(byte==34){result.append("\\\"");}
            elif(byte==39){result.append("\\'");}
            else{
                result.append("\\x");
                result.append(hex.substring(cast<int64>(byte)/16,1));
                result.append(hex.substring(cast<int64>(byte)%16,1));
            }
            begin=index+1;
        }
        index=index+1;
    }
    result.append(value.substring(begin,value.length()-begin));
    result.append(quote);
    return result;
}
)CHL";
}
