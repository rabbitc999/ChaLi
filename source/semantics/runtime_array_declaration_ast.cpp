/**
 * @brief 将下标共享前缀解释为数组元素类型，复用既有运行时数组类型。
 * @param typeExpr 类型名称、限定名称或元组表达式节点。
 * @return 运行时数组类型编号；非类型表达式产生解析诊断。
 */
int M_RuntimeArrayType(int typeExpr)
{
    const auto node = g_nodes[static_cast<std::size_t>(typeExpr)];
    int element = -1;
    if (node.kind == MNode::Var)
        element = M_TypeStruct(M_Name(node.s.c_str()));
    else if (node.kind == MNode::EnumCtor)
        element = M_QualifiedType(typeExpr);
    else if (node.kind == MNode::TupleTypeItem)
        element = node.type;
    else if (node.kind == MNode::Tuple)
    {
        const int items = M_List();
        for (const int child : node.kids) M_ListAdd(items, child);
        element = M_TypeTupleSyntax(items);
    }
    else
    {
        PushParseDiagnostic("运行时数组声明必须使用类型名称");
        element = M_TypeVoid();
    }
    return M_TypeRuntimeArray(element);
}
/**
 * @brief 将具名数组声明接入普通局部变量降级，不改变拥有值和引用的语义。
 * @param typeExpr 元素类型语法节点。
 * @param declarations 声明项列表。
 * @param referenceKind 零为拥有数组，一为只读引用，二为可写引用。
 * @return 局部声明节点。
 */
int M_RuntimeArrayDecl(int typeExpr, int declarations, int referenceKind)
{
    int type = M_RuntimeArrayType(typeExpr);
    if (referenceKind == 1) type = M_TypePtr(type);
    else if (referenceKind == 2) type = M_TypePtrMut(type);
    M_SetDeclType(declarations, type, 0);
    return M_LocalDecl(type, declarations, 0);
}
