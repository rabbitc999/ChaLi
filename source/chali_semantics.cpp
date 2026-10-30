#include "chali_lambda.hpp"
#include "chali_temp.hpp"
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include "chali_incremental.hpp"
/* ============================================================
 * 实现：节点池 / 类型池 / 降级（C 语法树 → ast） / 生成与执行
 * ============================================================ */
#include "ast.hpp"
#include "stackvm.hpp"
#include "../source/ircode/value_text.hpp"
#include "chali_glue.hpp"
#include "chali_generic.hpp"
#include "chali_resource_cleanup.hpp"
#include "chali_lambda.hpp"
#include "macro/registry.hpp"
#include "macro/metadata.hpp"
#include "macro/fragments.hpp"
#include "macro/source.hpp"
#include "chali_interpolation.hpp"
#include "literals/string_escape.hpp"
#include "editor/compiler_analysis.hpp"
#include "chali_lifetime_contract.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <fstream>
#include <functional>
#include <filesystem>
#include <chrono>
#include <cerrno>
#if !defined(_WIN32)
#include <unistd.h>
#include <sys/wait.h>
#endif
#include <limits>
#include <climits>
#include <cmath>
#include <set>

namespace
{
/* 语法动作会在读取旧节点的同时创建新节点；deque 保持旧节点引用有效。 */
Memory::Deque<MNode> g_nodes;
Memory::Map<int, std::size_t> g_rawMacroNames;
std::size_t g_macroFragmentFirstLine = 0;
std::size_t g_macroFragmentFirstColumn = 1;
Memory::Vector<std::shared_ptr<MType>> g_types;
Memory::Vector<Memory::String> g_sourceFiles;
Memory::String g_sourceFallback;
Memory::Vector<std::size_t> g_sourceLines;
/* 最近一个 token 的行与列（`M_TokenValue` 里更新）。
 *
 * **行**一直是报源码位置的主力（`M_New` 把它抄进节点的 `sourceLine`），语义检查
 * 再靠 `ReductionSourceScope` 把它来回恢复成"正在降级的那个节点"的行。
 * **列**是后来补上的：以前只有注解保留列，其余诊断一律 `第 N 行` 收尾，
 * 终端上就没有那条"`|  ~~~~`"指着谁的表达；现在照搬行号的套路，
 * `M_New` 也把它抄进节点的 `sourceColumn`。
 * 两个变量必须一起改，否则 node 的位置会一半新一半旧。 */
std::size_t g_reductionLine = 0;
std::size_t g_reductionColumn = 0;
Memory::Map<int, Memory::String> g_stringSources;
Memory::Vector<Memory::String> g_names; /* 标识符池 */
Memory::Vector<int> g_top;              /* 顶层声明（程序） */
int g_globalType = -1;                  /* 当前声明语句的类型（declarator 用它） */
Memory::String g_error;
Memory::Vector<MDiagnostic> g_diagnostics;
Memory::Vector<MDiagnostic> g_warnings;
Memory::Vector<chali_borrow_checker::Fact> g_borrowFacts;
std::size_t g_errorLine = 0;
std::size_t g_errorColumn = 0;
bool g_checkOnly = false;
bool g_sharedLibrary = false;
bool g_abiPrepareOnly = false;
Memory::String g_abiCacheOption, g_abiBridgeOption, g_abiWriteOption;
bool g_abiNoCache = false;
bool g_reflectionEnabled = false;  ///< 本次编译显式启用的注解与反射开关。
std::size_t g_runtimeMemory = Memory::defaultRuntimeCapacity;
std::size_t g_runtimePeak = 0;


/**
 * @brief 将类型描述加入类型池并返回其编号。
 * @param type 类型描述。
 * @retval 类型池中对应类型的编号。
 */
int M_AddType(const std::shared_ptr<MType> &type)
{
    g_types.push_back(type);
    return static_cast<int>(g_types.size()) - 1;
}
/**
 * 解析期的诊断（`ResetState()` 会清 g_error，所以解析期的错先攒在这里，
 * `M_Compile` 一开头报出来）。
 */
/**
 * @brief 一条解析期诊断：文案 + 报它时的源码行列。
 */
struct MParseDiagnostic
{
    Memory::String message;
    std::size_t line = 0, column = 0;
};
/**
 * @brief 解析期攒下的诊断（降级前 `g_error` 会被 ResetState 清掉，所以先挂在这儿）。
 *
 * **行列在入队时抄一份**：这时候 `g_reductionLine` / `g_reductionColumn` 还停在这个
 * token 上；等到 M_Compile 开头再取，Reduce 早就跑完、位置已经飘走（会指到内置
 * 模块里的某一行）。
 */
Memory::Vector<MParseDiagnostic> g_parseDiagnostics;
/**
 * @brief 记一条解析期诊断（顺带抄下当前源码行列）。
 * @param message 文案。
 * @return 无。
 */
void PushParseDiagnostic(const Memory::String &message)
{
    g_parseDiagnostics.push_back({ message, g_reductionLine, g_reductionColumn });
}
Memory::String g_parseModule;

/**
 * @brief 按编号取得类型描述，越界时返回空类型对象。
 * @param id 待查找或操作的对象编号。
 * @retval 对应的类型描述对象。
 */
std::shared_ptr<MType> M_TypeAt(int id)
{
    if (id < 0 || id >= static_cast<int>(g_types.size())) return Memory::makeShared<MType>();
    return g_types[static_cast<std::size_t>(id)];
}

/* `Fail` 在下面才实现（同一个匿名命名空间，先声明就能用）；
 * 语义错误通过解析诊断表保存。 */
bool Fail(const Memory::String &message);

/* 数组类型构造在下面才实现，但类型构造函数（`M_TypeRuntimeArray`）马上要用到。 */
std::shared_ptr<MType> ArrayType(const std::shared_ptr<MType> &elem,
                                 const Memory::Vector<int> &dims);
std::shared_ptr<MType> RuntimeArrayType(const std::shared_ptr<MType> &elem);
/**
 * 是不是运行时长度数组（`T[]`：dims 空）。
 */
bool IsRuntimeArray(const std::shared_ptr<MType> &type);
/**
 * 是不是"对象句柄"类（`bint` / 容器 / 接口引用 / `obj`）：见下面的实现。
 */
bool IsObjectLike(const std::shared_ptr<MType> &type);

}  // namespace

/**
 * @brief 设置当前归约起始行。
 * @param line 预处理后的行号。
 */
void M_SetReductionLine(std::size_t line)
{
    g_reductionLine = g_macroFragmentFirstLine ? (line >= 2 ? g_macroFragmentFirstLine + line - 2 : g_macroFragmentFirstLine) : line;
}
/**
 * @brief 设置源码行映射。
 * @param lines 逐行原始行号。
 */
void M_SetSourceLines(const Memory::Vector<std::size_t> &lines) { g_sourceLines = lines; }

/**
 * @brief 创建语法节点并记录当前模块和源码位置。
 * @param kind 待处理对象的类别。
 * @param s 待处理的文本或节点文本值。
 * @param i 节点保存的整数值。
 * @param d 节点保存的浮点值或待初始化的数据。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_New(int kind, Memory::String s, long long i, double d)
{
    MNode node;
    node.kind = static_cast<MNode::Kind>(kind);
    node.s = std::move(s);
    node.i = i;
    node.d = d;
    node.module = g_parseModule;
    node.sourceLine = g_reductionLine;
    node.sourceColumn = g_reductionColumn;
    g_nodes.push_back(std::move(node));
    return static_cast<int>(g_nodes.size()) - 1;
}

/**
 * @brief 根据类别及名称创建类型描述。
 * @param kind 待处理对象的类别。
 * @param name 名称文本。
 * @retval 类型池中对应类型的编号。
 */
int M_TypeKind(int kind, const Memory::String &name)
{
    auto type = Memory::makeShared<MType>();
    type->kind = static_cast<MType::Kind>(kind);
    type->name = name;
    return M_AddType(type);
}
/**
 * @brief 创建指定位宽及有无符号属性的整数类型。
 * @param bits 数值类型的位宽。
 * @param unsignedFlag 是否创建无符号整数类型。
 * @retval 类型池中对应类型的编号。
 */
int M_TypeIntBits(int bits, int unsignedFlag)
{
    const int id = M_TypeKind(MType::Int);
    g_types[static_cast<std::size_t>(id)]->bits = bits;
    g_types[static_cast<std::size_t>(id)]->isUnsigned = (unsignedFlag != 0);
    return id;
}
/**
 * @brief 构造并登记整数类型。
 * @retval 类型池中对应类型的编号。
 */
int M_TypeInt() { return M_TypeIntBits(64, 0); }
/**
 * @brief 构造并登记布尔类型。
 * @retval 类型池中对应类型的编号。
 */
int M_TypeBool() { return M_TypeKind(MType::Bool); }
/**
 * @brief 构造并登记字符类型。
 * @retval 类型池中对应类型的编号。
 */
int M_TypeChar() { return M_TypeKind(MType::Char); }
/**
 * @brief 构造并登记指定位宽浮点类型。
 * @param bits 数值类型的位宽。
 * @retval 类型池中对应类型的编号。
 */
int M_TypeFloatBits(int bits)
{
    const int id = M_TypeKind(MType::Double);
    g_types[static_cast<std::size_t>(id)]->bits = bits;
    return id;
}
/**
 * @brief 构造并登记双精度浮点类型。
 * @retval 类型池中对应类型的编号。
 */
int M_TypeDouble() { return M_TypeFloatBits(64); }
/**
 * @brief 构造并登记字符串类型。
 * @retval 类型池中对应类型的编号。
 */
int M_TypeString() { return M_TypeKind(MType::Opcode, "std::lang::String"); }
/**
 * @brief 构造并登记无返回值类型。
 * @retval 类型池中对应类型的编号。
 */
int M_TypeVoid() { return M_TypeKind(MType::Void); }
/**
 * @brief 引用 lang 包声明的 BigInteger opcode 类型。
 * @retval 类型池中对应类型的编号。
 */
int M_TypeBigInt() { return M_TypeKind(MType::Opcode, "std::lang::BigInteger"); }
/**
 * @brief 引用 lang 包声明的 BigDecimal opcode 类型。
 * @retval 类型池中对应类型的编号。
 */
int M_TypeBigFloat() { return M_TypeKind(MType::Opcode, "std::lang::BigDecimal"); }
/**
 * @brief `obj`：**装箱格子**（值 = 运行期对象句柄：一个值 + 它的类型号）。
 *
 * 它是"运行时才知道装着什么"的那种类型：`set` 装任意可装箱的值、`get` 按上下文类型
 * 取出来、`name()` 给类型号（和 `__typeTag(类型)` 比就是泛型）。
 * @retval 类型池中对应类型的编号。
 */
int M_TypeObj() { return M_TypeKind(MType::Obj); }
/**
 * @brief 引用 lang 包声明的 File opcode 类型。
 * @retval 类型池中对应类型的编号。
 */
int M_TypeFile() { return M_TypeKind(MType::Opcode, "std::lang::File"); }
/**
 * @brief 引用 lang 包声明的 Process opcode 类型。
 * @retval 类型池中对应类型的编号。
 */
int M_TypeProc() { return M_TypeKind(MType::Opcode, "std::lang::Process"); }

/**
 * @brief 造一个容器类型：`name` 是 "vec" / "set" / "map"。
 * @param name 名称文本。
 * @param firstId 容器元素类型或键类型的编号。
 * @param secondId 映射值类型的编号，不适用时为缺省值。
 * @retval 类型池中对应容器类型的编号。
 */
int M_ContainerType(const char *name, int firstId, int secondId)
{
    auto type = Memory::makeShared<MType>();
    type->kind = MType::Opcode;
    type->name = name;
    type->elem = M_TypeAt(firstId);
    if (secondId >= 0) type->elem2 = M_TypeAt(secondId);
    return M_AddType(type);
}
/**
 * @brief 标准类型名 → 类型节点（`String` / `BigInteger` / `BigDecimal` / `File` / `Process` / `Any`）。
 *
 * `std::lang::X` 只走一条路：通用限定名 `VARIABLE SCOPE VARIABLE` → 这里按名字落回真实类型。
 * 拼写 token（`STD_*`）已删除，所以这是**唯一**入口，不存在两种写法给出不同结果的问题。
 *
 * @param name 类型名（不含 `std::`）。
 * @return 类型节点 id；不是标准名返回 -1。
 */
int StandardTypeByName(const Memory::String &name)
{
    if (name == "String") return M_TypeString();
    if (name == "BigInteger") return M_TypeBigInt();
    if (name == "BigDecimal") return M_TypeBigFloat();
    if (name == "File") return M_TypeFile();
    if (name == "Process") return M_TypeProc();
    if (name == "Any") return M_TypeObj();
    return -1;
}

/**
 * @brief 构造并登记动态列表类型。
 * @param elementId 元素类型在类型池中的编号。
 * @retval 类型池中对应类型的编号。
 */
int M_TypeVec(int elementId) { return M_ContainerType("vec", elementId, -1); }
/**
 * @brief 构造并登记集合类型。
 * @param elementId 元素类型在类型池中的编号。
 * @retval 类型池中对应类型的编号。
 */
int M_TypeSet(int elementId) { return M_ContainerType("set", elementId, -1); }
/**
 * @brief 构造并登记映射类型。
 * @param keyId 映射键类型在类型池中的编号。
 * @param valueId 表示输入值的节点或类型编号。
 * @retval 类型池中对应类型的编号。
 */
int M_TypeMap(int keyId, int valueId) { return M_ContainerType("map", keyId, valueId); }

/**
 * @brief 内建类型的**裸名 / 内部拼写** → 真实类型节点。
 *
 * 取消 `file` / `proc` / `bint` / `bfloat` 这些关键字之后它们成了普通标识符，但
 * **泛型实例化产出的源码里仍然是这些内部拼写**（`Result<File, string>` 实例化出来的结构体
 * 成员就写成 `file`），所以"按名字写出类型"这条路必须照样认它们，否则一用泛型就炸。
 * `String` / `File` / `Process` / `BigInteger` / `BigDecimal` / `Any` 的裸名形式也一并认，
 * 方便 `std::lang::X` 与裸名两种写法过渡（`std::lang::X` 那条路由 `StandardTypeByName` 认）。
 *
 * @param name 类型名（不含 `std::`）。
 * @return 类型节点 id；不是内建名返回 -1（调用方按普通结构体处理）。
 */
int BuiltinTypeByName(const Memory::String &name)
{
    if (name == "file") return M_TypeFile();
    if (name == "proc") return M_TypeProc();
    if (name == "bint") return M_TypeBigInt();
    if (name == "bfloat") return M_TypeBigFloat();
    if (name == "String") return M_TypeString();
    if (name == "File") return M_TypeFile();
    if (name == "Process") return M_TypeProc();
    if (name == "BigInteger") return M_TypeBigInt();
    if (name == "BigDecimal") return M_TypeBigFloat();
    if (name == "Any") return M_TypeObj();
    return -1;
}

/**
 * @brief 构造并登记命名结构体类型。
 * @param nameId 名称池中的名称编号。
 * @retval 类型池中对应类型的编号。
 */
int M_TypeStruct(int nameId)
{
    const Memory::String &name = g_names[static_cast<std::size_t>(nameId)];
    if (name == "file" || name == "proc" || name == "bint" || name == "bfloat")
        return BuiltinTypeByName(name);
    const int id = M_TypeKind(MType::Struct, name);
    M_TypeAt(id)->lookupModule = g_parseModule;
    M_TypeAt(id)->unresolvedName = true;
    return id;
}

/**
 * @brief 保存载荷模式的限定类型及绑定名称。
 * @param type 类型池编号，负一表示通配绑定。
 * @param nameId 绑定名称编号。
 * @return 模式节点编号。
 */
int M_TypedPattern(int type, int nameId)
{
    const int id = M_Var(nameId);
    g_nodes[id].type = type;
    g_nodes[id].s2 = "__chali_typed_pattern";
    return id;
}

/** @brief 建立不绑定名称的通配模式，返回节点编号。 */
int M_WildcardPattern()
{
    return M_Var(M_Name("_"));
}

namespace
{
/**
 * @brief 生成解析期元组元素类型的去重键。
 * @param type 类型描述。
 * @retval 生成或读取的结果文本。
 */
Memory::String TupleParseKey(const std::shared_ptr<MType> &type)
{
    if (!type) return "void";
    Memory::String key = Memory::toString(static_cast<int>(type->kind)) + ":" + Memory::toString(type->bits) + ":" + (type->isUnsigned ? "u:" : "s:") + (type->kind == MType::Func ? Memory::String() : type->name);
    if (type->elem) key += "<" + TupleParseKey(type->elem) + ">";
    if (type->elem2) key += "<" + TupleParseKey(type->elem2) + ">";
    for (const std::shared_ptr<MType> &param : type->paramTypes)
        key += "(" + TupleParseKey(param) + ")";
    for (const int dim : type->dims) key += "[" + Memory::toString(dim) + "]";
    return key;
}

/**
 * @brief 根据元素类型生成元组的内部名称。
 * @param elements 元素类型或元素值列表。
 * @retval 生成或读取的结果文本。
 */
Memory::String TupleInternalName(const Memory::Vector<std::shared_ptr<MType>> &elements)
{
    std::uint64_t hash = 1469598103934665603ull;
    Memory::String key;
    for (const std::shared_ptr<MType> &element : elements)
        key += "|" + TupleParseKey(element);
    for (const unsigned char ch : key)
    {
        hash ^= ch;
        hash *= 1099511628211ull;
    }
    Memory::OutputStringStream name;
    name << "__chali_tuple_" << elements.size() << "_" << std::hex << hash;
    return name.str();
}

/**
 * @brief 创建元组类型并复用已有定义。
 * @param elements 元素类型或元素值列表。
 * @retval 对应的类型描述对象。
 */
std::shared_ptr<MType> MakeTupleType(
    const Memory::Vector<std::shared_ptr<MType>> &elements)
{
    auto type = Memory::makeShared<MType>();
    type->kind = MType::Struct;
    type->paramTypes = elements; /* 非空 = 编译器匿名元组；普通 struct 不使用此字段。 */
    type->name = TupleInternalName(elements);
    return type;
}
}  // namespace

/**
 * @brief 构造并登记元组类型。
 * @param typeList 元组元素类型列表的编号。
 * @retval 类型池中对应类型的编号。
 */
int M_TypeTuple(int typeList)
{
    Memory::Vector<std::shared_ptr<MType>> elements;
    if (typeList >= 0)
        for (const int typeId : g_nodes[static_cast<std::size_t>(typeList)].kids)
            elements.push_back(M_TypeAt(typeId));
    return M_AddType(MakeTupleType(elements));
}

/**
 * @brief 将显式类型包装为共享元组语法项。
 * @param type 类型。
 * @return 节点下标。
 */
int M_TupleTypeItem(int type)
{
    const int id = M_New(MNode::TupleTypeItem);
    g_nodes[id].type = type;
    return id;
}
/**
 * @brief 在声明上下文将名称与嵌套元组解释为类型。
 * @param items 项列表。
 * @return 类型下标。
 */
int M_TypeTupleSyntax(int items)
{
    Memory::Function<int(int)> convert = [&](int id) -> int {
        const auto &node = g_nodes[id];
        if (node.kind == MNode::TupleTypeItem) return node.type;
        if (node.kind == MNode::Var) return M_TypeStruct(M_Name(node.s.c_str()));
        if (node.kind == MNode::EnumCtor && node.kids.empty()) return M_QualifiedType(id);
        if (node.kind == MNode::Tuple)
        {
            const int list = M_List();
            for (int child : node.kids) M_ListAdd(list, convert(child));
            return M_TypeTuple(list);
        }
        PushParseDiagnostic("元组类型项必须是类型名称");
        return M_TypeVoid();
    };
    const int types = M_List();
    for (int item : g_nodes[items].kids) M_ListAdd(types, convert(item));
    return M_TypeTuple(types);
}

/**
 * @brief 构造并登记只读指针类型。
 * @param baseId 指针目标类型在类型池中的编号。
 * @retval 类型池中对应类型的编号。
 */
int M_TypePtr(int baseId)
{
    auto type = Memory::makeShared<MType>();
    type->kind = MType::Ptr;
    type->elem = M_TypeAt(baseId);
    if (type->elem && type->elem->kind == MType::Ptr)
        PushParseDiagnostic("不支持复合引用类型；引用只能直接引用元素");
    type->readOnly = true; /* `T*` / `T const&` ≡ `T const&`：只能读 */
    return M_AddType(type);
}
/**
 * @brief 构造并登记可变指针类型。
 * @param baseId 指针目标类型在类型池中的编号。
 * @retval 类型池中对应类型的编号。
 */
int M_TypePtrMut(int baseId)
{
    /* `T mut&`：可写引用 —— 能写、能做引用算术、有所有权（`new T` 出来的归它）。 */
    auto type = Memory::makeShared<MType>();
    type->kind = MType::Ptr;
    type->elem = M_TypeAt(baseId);
    if (type->elem && type->elem->kind == MType::Ptr)
        PushParseDiagnostic("不支持复合引用类型；引用只能直接引用元素");
    type->readOnly = false;
    return M_AddType(type);
}
/**
 * @brief `T[]`：**运行时长度数组**（`T[] a = new T[N];`）。
 *
 * 与 `T[N]`（编译期就把维度写死在数据区里）的区别：长度跟着对象走、下标检查
 * 读的是运行期存下来的元素个数，所以 `N` 可以是变量 —— 这就是"运行时可变的分配"。
 * @param elementId 元素类型。
 * @retval 类型池中对应类型的编号。
 */
int M_TypeRuntimeArray(int elementId)
{
    /* `T[]`：运行时长度数组（dims 空 = 长度跟着对象走）。 */
    return M_AddType(RuntimeArrayType(M_TypeAt(elementId)));
}
/**
 * @brief 从类型列表构建匿名函数类型，保留变长参数约束。
 * @param paramsId 形参节点列表。
 * @param returnTypeId 返回类型下标。
 * @return 新增的函数类型下标。
 */
int M_FunctionType(int paramsId, int returnTypeId)
{
    auto type = Memory::makeShared<MType>();
    type->kind = MType::Func;
    type->elem = M_TypeAt(returnTypeId);
    const auto &params = g_nodes[static_cast<std::size_t>(paramsId)].kids;
    for (std::size_t index = 0; index < params.size(); ++index)
    {
        const auto &param = g_nodes[static_cast<std::size_t>(params[index])];
        type->paramTypes.push_back(M_TypeAt(param.type));
        if (param.i != 0)
        {
            type->variadic = true;
            if (index + 1u != params.size())
                PushParseDiagnostic("函数类型的变长参数必须是最后一个形参");
        }
    }
    return M_AddType(type);
}
/**
 * @brief 保存函数类型的一项形参；变长参数按列表传递。
 * @param typeId 形参类型下标。
 * @param variadic 是否为变长参数。
 * @return 形参节点下标。
 */
int M_FunctionParameter(int typeId, int variadic)
{
    const int id = M_New(MNode::List);
    g_nodes[static_cast<std::size_t>(id)].type = variadic ? M_TypeVec(typeId) : typeId;
    g_nodes[static_cast<std::size_t>(id)].i = variadic;
    return id;
}
/**
 * @brief 保存共用 fn 签名，延后到函数体或类型上下文确定其用途。
 * @param result 返回类型编号。
 * @param params 形参列表编号。
 * @return 签名节点编号。
 */
int M_FnSignature(int result, int params)
{
    const int id = M_New(MNode::List);
    g_nodes[id].type = result;
    g_nodes[id].kids = { params };
    return id;
}
/**
 * @brief 将签名转换为函数类型，拒绝具名参数和默认值。
 * @param signature 签名节点编号。
 * @return 函数类型编号。
 */
int M_FnSignatureType(int signature)
{
    const auto node = g_nodes[signature];
    for (const int param : g_nodes[node.kids[0]].kids)
        if (!g_nodes[param].s.empty() || !g_nodes[param].kids.empty())
            PushParseDiagnostic("函数类型的形参只写类型，不写名称或默认值");
    return M_FunctionType(node.kids[0], node.type);
}
/**
 * @brief 把后置函数类型的形参名擦掉，只留类型后转换为函数类型。
 *
 * 后置写法 `-> fn(T v) -> T` 里的形参名只是注解，擦掉不进入类型身份；
 * 默认值不是注解，仍按 M_FnSignatureType 的口径报诊断。
 *
 * @param signature 签名节点编号。
 * @return 函数类型编号。
 */
int M_FnSignatureTypeErased(int signature)
{
    const int result = g_nodes[static_cast<std::size_t>(signature)].type;
    const int parameters = g_nodes[static_cast<std::size_t>(signature)].kids[0];
    const int list = M_List();
    /* 逐个按下标取：M_New 会搬动 g_nodes，不能把容器引用或迭代器留在循环外。 */
    const std::size_t count = g_nodes[static_cast<std::size_t>(parameters)].kids.size();
    for (std::size_t index = 0; index < count; ++index) {
        const int parameter = g_nodes[static_cast<std::size_t>(parameters)].kids[index];
        const int type = g_nodes[static_cast<std::size_t>(parameter)].type;
        if (!g_nodes[static_cast<std::size_t>(parameter)].kids.empty())
            PushParseDiagnostic("函数类型的形参不写默认值");
        M_ListAdd(list, M_FunctionParameter(type, 0));
    }
    return M_FunctionType(list, result);
}
/**
 * @brief 将共用签名和函数体组成默认只读捕获的匿名函数。
 * @param signature 签名节点编号。
 * @param body 函数体节点编号。
 * @return 匿名函数节点编号。
 */
int M_FnSignatureLambda(int signature, int body)
{
    const auto node = g_nodes[signature];
    for (const int param : g_nodes[node.kids[0]].kids)
        if (g_nodes[param].s.empty())
            PushParseDiagnostic("fn 匿名函数的形参必须具有名称");
    return M_Lambda(node.type, node.kids[0], body, 0);
}
/**
 * @brief 保存解析阶段发现的语义错误，交由检查阶段报告。
 */
int M_ParseBad(const Memory::String &message)
{
    const int id = M_New(MNode::ParseBad);
    g_nodes[static_cast<std::size_t>(id)].s = message;
    return id;
}

/* 维度表：kids 里放各维长度的**表达式节点**（降级时要求是整数字面量）。 */
/**
 * @brief 创建包含一个数组维度的节点列表。
 * @param a 第一个操作数或节点编号。
 * @retval 包含指定维度的列表节点编号。
 */
int M_Dims1(int a)
{
    const int id = M_New(MNode::List, "dims");
    g_nodes[static_cast<std::size_t>(id)].kids = { a };
    return id;
}
/**
 * @brief 创建包含两个数组维度的节点列表。
 * @param a 第一个操作数或节点编号。
 * @param b 第二个操作数或节点编号。
 * @retval 包含两个维度的列表节点编号。
 */
int M_Dims2(int a, int b)
{
    const int id = M_New(MNode::List, "dims");
    g_nodes[static_cast<std::size_t>(id)].kids = { a, b };
    return id;
}
/**
 * @brief 按声明顺序合并两个数组维度列表。
 * @param dims 已有维度列表编号。
 * @param next 待追加维度列表编号。
 * @return 合并后的列表编号。
 */
int M_DimsAppend(int dims, int next)
{
    const auto values = g_nodes[static_cast<std::size_t>(next)].kids;
    for (const int value : values) M_ListAdd(dims, value);
    return dims;
}
/**
 * @brief 将多维下标展开为现有连续索引节点，保留逐维检查。
 * @param base 被索引的表达式编号。
 * @param dims 下标表达式列表编号。
 * @return 最后一个索引节点编号。
 */
int M_IndexDims(int base, int dims)
{
    const auto values = g_nodes[static_cast<std::size_t>(dims)].kids;
    for (const int value : values) base = M_Index(base, value);
    return base;
}
/**
 * @brief 创建空的语法节点列表。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_List() { return M_New(MNode::List); }
/**
 * @brief 创建包含一个元素的语法节点列表。
 * @param item 待追加或处理的节点编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_List1(int item)
{
    const int id = M_New(MNode::List);
    g_nodes[static_cast<std::size_t>(id)].kids.push_back(item);
    return id;
}
/**
 * @brief 创建包含两个元素的语法节点列表。
 * @param first 首个节点或待检查的文本起点。
 * @param second 第二个节点或类型编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_List2(int first, int second)
{
    const int id = M_New(MNode::List);
    g_nodes[static_cast<std::size_t>(id)].kids = { first, second };
    return id;
}
/**
 * @brief 将语法节点追加到已有列表。
 * @param list 接收新增节点的列表编号。
 * @param item 待追加或处理的节点编号。
 */
void M_ListAdd(int list, int item)
{
    if (list >= 0) g_nodes[static_cast<std::size_t>(list)].kids.push_back(item);
}
/**
 * @brief 根据运算符类别创建二元运算节点。
 * @param kind 待处理对象的类别。
 * @param left 左操作数的节点编号。
 * @param right 右操作数的节点编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_BinOp(int kind, int left, int right)
{
    static const char *const table[] = { "||", "&&", "==", "!=", "<", "<=", ">",
                                         ">=", "+", "-", "*", "/", "%",
                                         "&", "|", "^", "<<", ">>" };
    const int count = static_cast<int>(sizeof(table) / sizeof(table[0]));
    const int id = M_New(MNode::Bin, (kind >= 0 && kind < count) ? table[kind] : "+");
    g_nodes[static_cast<std::size_t>(id)].kids = { left, right };
    return id;
}
/**
 * @brief 构造 `move 表达式` 节点。
 *
 * 移动是运算符而不是函数：这里把它包成内部调用 `std::lang::__move(x)`，借用流、生命周期与
 * 降级只认这一种形状。`__move` 是编译器内部拼写，和 `__fileTryOpen` 那批内建一样不在文档里公开。
 * @param operand 被移动的位置表达式。
 * @retval 内部移动调用节点的编号。
 */
int M_Move(int operand)
{
    if (operand >= 0 && g_nodes[static_cast<std::size_t>(operand)].kind == MNode::Un && g_nodes[static_cast<std::size_t>(operand)].s == "&")
        PushParseDiagnostic("move 不接受取地址表达式；请直接移动变量或引用");
    const int callee = M_New(MNode::EnumCtor, "std::lang");
    g_nodes[static_cast<std::size_t>(callee)].s2 = "__move";
    return M_CallOn(callee, M_List1(operand));
}
/**
 * @brief 根据运算符类别创建一元运算节点。
 * @param kind 待处理对象的类别。
 * @param operand 一元运算的操作数。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_UnOp(int kind, int operand)
{
    /* 0 = `!`，1 = 一元 `-`，2 = 解引用 `*`（降级成 `p[0]`），3 = 按位取反 `~`，
     * 4 = 取地址 `&`，5 = 一元 `+`（只对用户类型的重载有意义，数值上恒等）。 */
    static const char *const table[] = { "!", "-", "*", "~", "&", "+" };
    const int count = static_cast<int>(sizeof(table) / sizeof(table[0]));
    return M_Un((kind >= 0 && kind < count) ? table[kind] : "!", operand);
}
/**
 * @brief 将标识符文本保存到名称池并返回编号。
 * @param text 待处理的文本。
 * @retval 名称池中对应文本的编号。
 */
int M_Name(const char *text)
{
    g_names.push_back(text != nullptr ? text : "");
    return static_cast<int>(g_names.size()) - 1;
}
/**
 * @brief 名称池的反向查询：取回文本已有的编号，没有才登记。
 * @param text 已存在于节点上的标识符文本。
 * @retval 名称池中对应文本的编号。
 * @note 节点只保存文本，文法动作之间要靠编号传递；倒序扫描让最近登记的名字先命中。
 */
int M_NameIdOf(const Memory::String &text)
{
    for (std::size_t i = g_names.size(); i-- > 0;)
        if (g_names[i] == text)
            return static_cast<int>(i);
    return M_Name(text.c_str());
}

/**
 * @brief 返回保留字 `yield` 对应的名称池编号，供 break 语法糖在限定名 / 声明名位置复用。
 * @retval 名称池中 "yield" 的编号。
 * @note 文法动作不支持字符串字面量，故单独抽出此无参辅助函数。
 */
int M_YieldName()
{
    return M_Name("yield");
}
/**
 * @brief 将运算符序号转换为名称池编号。
 * @param which 运算符或关键字查找表的序号。
 * @retval 名称池中对应文本的编号。
 */
int M_Op(int which)
{
    static const char *const table[] = { "+", "-", "*", "/", "%", "==",
                                         "!=", "<", "<=", ">", ">=",
                                         "&", "|", "^", "<<", ">>" };
    const int count = static_cast<int>(sizeof(table) / sizeof(table[0]));
    return M_Name((which >= 0 && which < count) ? table[which] : "+");
}
/**
 * @brief **关键字当名字用**时的名字池下标（同样用序号取，动作里不写字符串字面量）。
 *
 * 用于 drop、opcode、type 等保留字成员选择。**关键字 Token 的词法路径不填 `text` / `pName`**（固定字面量不碰
 * 词法器的 scratch 缓冲），所以在动作里拿 `$n` 只会得到零值 —— 必须走这张表。
 * @param which 序号：0 为 set，1 为 drop，2 为 new，3 为 copy，4 为 move，5 为 opcode，6 为 type。
 * @return 名字池下标。
 */
int M_KeywordName(int which)
{
    static const char *const table[] = { "set", "drop", "new", "copy", "move", "opcode", "type" };
    const int count = static_cast<int>(sizeof(table) / sizeof(table[0]));
    return M_Name((which >= 0 && which < count) ? table[which] : "set");
}
/**
 * @brief 构造并登记整数字面量。
 * @param value 值表达式或字面量的语义值。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Int(long long value) { return M_New(MNode::Int, "", value); }
/**
 * @brief 构造并登记布尔字面量。
 * @param value 值表达式或字面量的语义值。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Bool(int value) { return M_New(MNode::Bool, "", value); }
/**
 * @brief 构造并登记空值字面量。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Null() { return M_New(MNode::Null); }
/**
 * @brief 构造并登记浮点字面量。
 * @param value 值表达式或字面量的语义值。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Float(double value) { return M_New(MNode::Float, "", 0, value); }
/**
 * @brief 构造并登记字符串字面量。
 * @param stringId 字符串在名称池中的编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Str(int stringId) { return M_New(MNode::Str, g_names[static_cast<std::size_t>(stringId)]); }
/**
 * @brief 拼接相邻字符串字面量并保存到名称池。
 * @param leftId 左侧文本或表达式的编号。
 * @param rightId 右侧文本或表达式的编号。
 * @retval 名称池中对应文本的编号。
 */
int M_ConcatStringLiterals(int leftId, int rightId)
{
    g_names.push_back(g_names[static_cast<std::size_t>(leftId)] + g_names[static_cast<std::size_t>(rightId)]);
    return static_cast<int>(g_names.size()) - 1;
}
/**
 * @brief 字符字面量 `'A'` / `'\n'`：一个独立的 `char` 字节值。
 *
 * `char` 与整数之间必须显式转换；输出使用 `std::putChar`。
 * @param textId 词法拿到的原文（带两侧单引号）在名字池里的下标。
 * @return Int 节点。
 */
int M_Char(int textId)
{
    /* 原样存着（带引号），降级时再剥：见 MNode::Char 的注释。 */
    return M_New(MNode::Char, g_names[static_cast<std::size_t>(textId)]);
}
/**
 * @brief `s[起:止]`：字符串切片（半开区间，越界自动夹到范围内；止 < 0 = 到末尾）。
 *
 * 值是**新造出来的字符串**（堆上），所以语言侧不用写逐字节的循环 —— 拼接 / 切片 /
 * 格式化都交给虚拟机里那份 C++ 运行时（`__std_str_*`）。
 * @param base 被切的字符串。
 * @param begin 起始下标表达式。
 * @param end 结束下标表达式；-1 = 到末尾。
 * @return Slice 节点。
 */
int M_Slice(int base, int begin, int end)
{
    const int id = M_New(MNode::Slice);
    g_nodes[static_cast<std::size_t>(id)].kids = { base, begin, end };
    return id;
}
/**
 * @brief 构造并登记变量引用。
 * @param nameId 名称池中的名称编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Var(int nameId) { return M_New(MNode::Var, g_names[static_cast<std::size_t>(nameId)]); }
/**
 * @brief 构造并登记二元运算表达式。
 * @param op 运算符文本或类别。
 * @param left 左操作数的节点编号。
 * @param right 右操作数的节点编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Bin(const char *op, int left, int right)
{
    const int id = M_New(MNode::Bin, op);
    g_nodes[static_cast<std::size_t>(id)].kids = { left, right };
    return id;
}
/**
 * @brief 构造并登记一元运算表达式。
 * @param op 运算符文本或类别。
 * @param operand 一元运算的操作数。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Un(const char *op, int operand)
{
    if (op != nullptr && Memory::String(op) == "-")
    {
        int literal = operand;
        while (literal >= 0 && g_nodes[literal].kind == MNode::Un && g_nodes[literal].s == "+" && !g_nodes[literal].kids.empty())
            literal = g_nodes[literal].kids[0];
        if (literal >= 0 && g_nodes[literal].kind == MNode::Int && g_nodes[literal].unsignedLiteral)
            PushParseDiagnostic("u 后缀整数字面量不能使用一元负号；需要有符号负数时请去掉 u 后缀");
        /* `-9223372036854775808`：幅值 2^63 本身超出 int64，直接取负才落回最小值。
         * 字面量节点在词法阶段按"待取负"登记，这里折成真正的负数并清掉标记。 */
        if (literal >= 0 && g_nodes[literal].kind == MNode::Int && g_nodes[literal].intMinMagnitude)
        {
            /* 先把要用的字段抄出来：M_Int 会往节点池里追加，引用随即失效。 */
            const Memory::String magnitude = g_nodes[static_cast<std::size_t>(literal)].s;
            const std::size_t line = g_nodes[static_cast<std::size_t>(literal)].sourceLine;
            const std::size_t column = g_nodes[static_cast<std::size_t>(literal)].sourceColumn;
            g_nodes[static_cast<std::size_t>(literal)].intMinMagnitude = false;
            const int folded = M_Int(LLONG_MIN);
            g_nodes[static_cast<std::size_t>(folded)].s = "-" + magnitude;
            g_nodes[static_cast<std::size_t>(folded)].sourceLine = line;
            g_nodes[static_cast<std::size_t>(folded)].sourceColumn = column;
            return folded;
        }
    }
    const int id = M_New(MNode::Un, op);
    g_nodes[static_cast<std::size_t>(id)].kids = { operand };
    return id;
}
/**
 * @brief 构造并登记显式类型转换表达式。
 * @param typeId 类型池中的类型编号。
 * @param value 值表达式或字面量的语义值。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Cast(int typeId, int value)
{
    const int id = M_New(MNode::Cast);
    g_nodes[static_cast<std::size_t>(id)].type = typeId;
    g_nodes[static_cast<std::size_t>(id)].kids = { value };
    return id;
}
/**
 * @brief 构造并登记下标访问表达式。
 * @param base 被访问表达式的节点编号。
 * @param position 目标位置或源码位置。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Index(int base, int position)
{
    const int id = M_New(MNode::Index);
    g_nodes[static_cast<std::size_t>(id)].kids = { base, position };
    return id;
}
/**
 * @brief 构造并登记成员访问表达式。
 * @param base 被访问表达式的节点编号。
 * @param fieldName 字段名称在名称池中的编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Member(int base, int fieldName)
{
    const int id = M_New(MNode::Member, g_names[static_cast<std::size_t>(fieldName)]);
    g_nodes[static_cast<std::size_t>(id)].kids = { base };
    return id;
}
/**
 * @brief 构造并登记元组字段访问表达式。
 * @param base 被访问表达式的节点编号。
 * @param numberNode 元组字段序号的字面量节点编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_TupleMember(int base, int numberNode)
{
    const long long index = (numberNode >= 0) ? g_nodes[static_cast<std::size_t>(numberNode)].i : -1;
    const int id = M_New(MNode::Member, "__" + Memory::toString(index), index);
    g_nodes[static_cast<std::size_t>(id)].kids = { base };
    return id;
}
/**
 * @brief 构造并登记元组表达式。
 * @param values 输入值列表或接收读取值的输出列表。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Tuple(int values)
{
    const int id = M_New(MNode::Tuple);
    if (values >= 0) g_nodes[static_cast<std::size_t>(id)].kids =
                         g_nodes[static_cast<std::size_t>(values)].kids;
    return id;
}
/**
 * @brief 构造 List、Set、Map 或等待上下文推导的空花括号集合节点。
 * @param kind 0 为空花括号，1 为 Set 元素表，2 为 Map 键值对表，3 为 List 元素表。
 * @param values 元素或交替键值的列表节点。
 * @return 集合表达式节点编号。
 *
 * 方括号只构造 List，花括号元素表只构造 Set，键值对表构造 Map。
 */
int M_CollectionLiteral(int kind, int values)
{
    const int id = M_New(MNode::CollectionLiteral, kind == 3 ? "vec" : kind == 2 ? "map" :
                                                                   kind == 1     ? "set" :
                                                                                   "empty");
    if (values >= 0) g_nodes[static_cast<std::size_t>(id)].kids =
                         g_nodes[static_cast<std::size_t>(values)].kids;
    return id;
}
/**
 * @brief 构造并登记指针成员访问表达式。
 * @param base 被访问表达式的节点编号。
 * @param fieldName 字段名称在名称池中的编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Arrow(int base, int fieldName)
{
    /* `p->f` 与 `p.f` 分开记：基类型是 `T*` 时两者都只读，但报错文案要说清是箭头。 */
    const int id = M_New(MNode::Arrow, g_names[static_cast<std::size_t>(fieldName)]);
    g_nodes[static_cast<std::size_t>(id)].kids = { base };
    return id;
}
/**
 * @brief `bint(表达式)`：构造一个大整数（十进制文本 / 整数 / 浮点）。
 *
 * `bint` 是**类型关键字**（不是普通函数名），所以它有自己一条表达式规则 +
 * 一个专用节点，而不是走"按函数名解析"那条路（那样还得把关键字塞进名字池）。
 * @param value 实参。
 * @return BigIntCtor 节点。
 */
int M_BigIntCtor(int value)
{
    const int id = M_New(MNode::BigIntCtor);
    g_nodes[static_cast<std::size_t>(id)].kids = { value };
    return id;
}
/**
 * @brief 构造并登记大浮点数构造表达式。
 * @param value 值表达式或字面量的语义值。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_BigFloatCtor(int value)
{
    const int id = M_New(MNode::BigFloatCtor);
    g_nodes[static_cast<std::size_t>(id)].kids = { value };
    return id;
}
/**
 * @brief 构造并登记函数调用表达式。
 * @param calleeName 被调用函数的名称编号。
 * @param args 按接口约定顺序传入的实参列表。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Call(int calleeName, int args)
{
    const int id = M_New(MNode::Call, g_names[static_cast<std::size_t>(calleeName)]);
    g_nodes[static_cast<std::size_t>(id)].kids = { args };
    return id;
}
bool IsStandardModule(const Memory::String &name);

/**
 * @brief 构造后缀调用，显式 Index 调用生成缓冲区索引左值。
 * @param callee 被调用的表达式节点。
 * @param args 实参列表节点。
 * @return 调用、构造器或显式缓冲区索引节点。
 */
int M_CallOn(int callee, int args)
{
    MNode &target = g_nodes[static_cast<std::size_t>(callee)];
    if (target.kind == MNode::TypeQualifiedRef)
    {
        target.type2 = 1;
        target.kids = g_nodes[args].kids;
        return callee;
    }
    const bool bufferIndex = (target.kind == MNode::Var && target.s == "index") || (target.kind == MNode::EnumCtor && target.s == "std::lang" && target.s2 == "index");
    if (bufferIndex && g_nodes[static_cast<std::size_t>(args)].kids.size() == 2u)
    {
        // 显式缓冲区元素访问直接构造成左值，复用索引的读写与取引用流程。
        const bool bareIndex = target.kind == MNode::Var;
        const auto values = g_nodes[static_cast<std::size_t>(args)].kids;
        const int id = M_Index(values[0], values[1]);
        g_nodes[static_cast<std::size_t>(id)].s = "__chali_buffer_index";
        if (bareIndex)
        {
            // 声明收集完成后再判断裸调用是否被用户函数遮蔽。
            g_nodes[static_cast<std::size_t>(id)].s2 = "__chali_bare_index";
            g_nodes[static_cast<std::size_t>(id)].i = callee;
        }
        return id;
    }
    /* 已声明模块按模块调用处理，模块限定调用不应被误判为类型构造。 */
    const std::size_t qualifier = target.s.rfind("::");
    const std::size_t typeStart = qualifier == Memory::String::npos ? 0u : qualifier + 2u;
    std::size_t typeLetter = typeStart;
    while (typeLetter < target.s.size() && target.s[typeLetter] == '_') ++typeLetter;
    const bool standardModule = target.s.compare(0u, 11u, "std::lang::") == 0 && IsStandardModule(target.s.substr(11u));
    const bool moduleQualifier = standardModule || target.s == "program";
    if (target.kind == MNode::EnumCtor && !moduleQualifier && typeLetter < target.s.size() && std::isupper(static_cast<unsigned char>(target.s[typeLetter])))
    {
        if (target.s.compare(0u, 11u, "std::lang::") == 0) target.s.erase(0u, 11u);
        target.kids = g_nodes[static_cast<std::size_t>(args)].kids;
        target.type2 = 1; /* `Type::name(...)`，与无括号的 static 字段访问区分。 */
        return callee;
    }
    const int id = M_New(MNode::Call);
    g_nodes[static_cast<std::size_t>(id)].kids = { callee, args };
    return id;
}

/**
 * @brief 构造并登记模块声明。
 * @param nameId 名称池中的名称编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Module(int nameId)
{
    const Memory::String name = g_names[static_cast<std::size_t>(nameId)];
    const int id = M_New(MNode::ModuleDecl, name);
    g_parseModule = name == "__root" ? Memory::String() : name;
    return id;
}

/**
 * @brief 构造并登记模块导入声明。
 * @param nameId 名称池中的名称编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Import(int nameId)
{
    return M_New(MNode::ImportDecl, g_names[static_cast<std::size_t>(nameId)]);
}

/**
 * @brief 构造并登记限定名称。
 * @param leftNameId 左侧限定名称的编号。
 * @param rightNameId 右侧名称的编号。
 * @retval 名称池中对应文本的编号。
 */
int M_QualifiedName(int leftNameId, int rightNameId)
{
    g_names.push_back(g_names[static_cast<std::size_t>(leftNameId)] + "::" + g_names[static_cast<std::size_t>(rightNameId)]);
    return static_cast<int>(g_names.size()) - 1;
}

/**
 * @brief `std::lang::X` 里的 X 是不是标准库**模块**（表在 `chali.gram` 的 `%code` 里）。
 *
 * 判定办法是查文法声明的那张表，而不是在编译器里写死 —— 新加标准模块只要改文法。
 * @param name `std::` 后面的那一段（如 `Math` / `Clock`）。
 * @return 是模块返回 true。
 */
bool IsStandardModule(const Memory::String &name)
{
    static const Memory::Set<Memory::String> modules = []() {
        Memory::Set<Memory::String> result;
        for (const char *item : kChaLiStdModules)
            if (item) result.insert(item);
        return result;
    }();
    return modules.count("std::lang::" + name) != 0u;
}

/**
 * @brief 构造并登记限定符号引用。
 * @param leftRefId 左侧限定引用的编号。
 * @param rightNameId 右侧名称的编号。
 * @retval 限定引用节点在节点池中的编号。
 */
int M_QualifiedRef(int leftRefId, int rightNameId)
{
    const MNode &left = g_nodes[static_cast<std::size_t>(leftRefId)];
    if (left.kind == MNode::TypeQualifiedRef)
    {
        const int id = M_AstTypeQualifiedRef(left.type, rightNameId);
        g_nodes[id].s2 = left.s2 + "::" + g_names[rightNameId];
        return id;
    }
    const bool standardType = left.s == "std::lang" && !left.s2.empty() && std::isupper(static_cast<unsigned char>(left.s2[0]));
    /* `std::lang::X::y` 里的 X 是不是**模块**，看 chali.gram 的 `%code` 里那张
     * kChaLiStdModules 表（新加标准模块时改文法，别在编译器里硬编码）。
     * 表外的按标准**类型**处理（`std::lang::List::of` 只留 `List::of`）。 */
    const Memory::String qualified = standardType ? (IsStandardModule(left.s2) ? "std::lang::" + left.s2 : left.s2) : left.s + "::" + left.s2;
    const int nameId = M_Name(qualified.c_str());
    const int emptyArgs = M_List();
    return M_EnumCtor(nameId, rightNameId, emptyArgs);
}

/**
 * @brief 构造并登记限定名称类型。
 * @param refId 限定类型引用的节点编号。
 * @retval 类型池中对应类型的编号。
 */
int M_QualifiedType(int refId)
{
    const MNode &ref = g_nodes[static_cast<std::size_t>(refId)];
    if (ref.kind == MNode::TypeQualifiedRef)
    {
        auto type = Memory::makeShared<MType>(*M_TypeAt(ref.type));
        type->genericMember = type->genericMember.empty() ? ref.s2 : type->genericMember + "::" + ref.s2;
        return M_AddType(type);
    }
    const Memory::String module = ref.s;
    const Memory::String typeName = ref.s2;
    return M_StdNamedType(M_Name(module.c_str()), M_Name(typeName.c_str()));
}

/**
 * @brief 将多段限定容器名交给标准类型检查。
 * @param refId 限定名节点。
 * @param first 元素或键类型。
 * @param second 值类型或 -1。
 * @return 容器类型。
 */
int M_QualifiedContainer(int refId, int first, int second)
{
    const MNode &ref = g_nodes[static_cast<std::size_t>(refId)];
    const Memory::String module = ref.s;
    const Memory::String name = ref.s2;
    return M_StdContainerNamed(M_Name(module.c_str()), M_Name(name.c_str()), first, second);
}

/**
 * @brief 构造并登记标准库内建类型。
 * @param moduleNameId 标准库模块名在名称池中的编号。
 * @param kind 待处理对象的类别。
 * @retval 类型池中对应类型的编号。
 */
int M_StdType(int moduleNameId, int kind)
{
    if (g_names[static_cast<std::size_t>(moduleNameId)] != "std::lang")
        PushParseDiagnostic("标准类型必须使用 std::lang:: 前缀");
    switch (kind)
    {
    case 0: return M_TypeString();
    case 1: return M_TypeBigInt();
    case 2: return M_TypeBigFloat();
    case 3: return M_TypeFile();
    case 4: return M_TypeProc();
    case 5: return M_TypeObj();
    default: return M_TypeVoid();
    }
}

/**
 * @brief 构造并登记标准库命名类型。
 * @param moduleNameId 标准库模块名在名称池中的编号。
 * @param typeNameId 类型名在名称池中的编号。
 * @retval 类型池中对应类型的编号。
 */
int M_StdNamedType(int moduleNameId, int typeNameId)
{
    const Memory::String &module = g_names[static_cast<std::size_t>(moduleNameId)];
    const Memory::String &name = g_names[static_cast<std::size_t>(typeNameId)];
    /* `std::lang::String` / `std::lang::BigInteger` / `std::lang::File` … 落回真实类型。 */
    if (module == "std::lang")
    {
        const int standard = StandardTypeByName(name);
        if (standard >= 0) return standard;
    }
    /*
     * 其它限定名——包括**用户模块里的泛型类型**（`custom::Box<int>` 会被泛型前处理
     * 改写成 `custom::Box__int`）——按具名类型处理。
     * 保留写出限定名，完整声明收集后再归一到实际类型键，避免不同模块同名类型混淆。
     */
    const int id = M_TypeStruct(typeNameId);
    if (M_TypeAt(id)->kind == MType::Struct) M_TypeAt(id)->name = module + "::" + name;
    return id;
}

/**
 * @brief `std::lang::List<T>` / `std::lang::Set<T>` / `std::lang::Map<K,V>`：**按名字**认容器。
 *
 * 容器的裸写法和限定写法统一走这条语义路径。
 * @param moduleNameId 模块名（必须是 `lang`）。
 * @param typeNameId 容器名（`List` / `Set` / `Map`）。
 * @param firstType 元素 / 键类型。
 * @param secondType `Map` 的值类型；其它容器传 -1。
 * @return 类型节点。
 */
int M_StdContainerNamed(int moduleNameId, int typeNameId, int firstType, int secondType)
{
    if (g_names[static_cast<std::size_t>(moduleNameId)] != "std::lang")
        PushParseDiagnostic("标准容器必须使用 std::lang:: 前缀");
    const Memory::String &name = g_names[static_cast<std::size_t>(typeNameId)];
    if (name == "List") return M_TypeVec(firstType);
    if (name == "Set") return M_TypeSet(firstType);
    if (name == "Map") return M_TypeMap(firstType, secondType);
    PushParseDiagnostic("不是标准容器：" + name + "（只有 std::lang::List / std::lang::Set / std::lang::Map）");
    return M_TypeVoid();
}

/**
 * @brief 解析省略核心模块的容器类型。
 * @param name 容器名。
 * @param first 元素或键。
 * @param second 值或 -1。
 * @return 类型编号。
 */
int M_ImplicitContainer(int name, int first, int second)
{
    return M_StdContainerNamed(M_Name("std::lang"), name, first, second);
}

/**
 * @brief 构造并登记标准库容器类型。
 * @param moduleNameId 标准库模块名在名称池中的编号。
 * @param kind 待处理对象的类别。
 * @param firstType 第一个容器类型参数的编号。
 * @param secondType 第二个容器类型参数的编号。
 * @retval 类型池中对应类型的编号。
 */
int M_StdContainer(int moduleNameId, int kind, int firstType, int secondType)
{
    if (g_names[static_cast<std::size_t>(moduleNameId)] != "std::lang")
        PushParseDiagnostic("标准容器必须使用 std::lang:: 前缀");
    if (kind == 0) return M_TypeVec(firstType);
    if (kind == 1) return M_TypeSet(firstType);
    return M_TypeMap(firstType, secondType);
}

/**
 * @brief 构造并登记标准库数值构造表达式。
 * @param moduleNameId 标准库模块名在名称池中的编号。
 * @param kind 待处理对象的类别。
 * @param value 值表达式或字面量的语义值。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_StdNumberCtor(int moduleNameId, int kind, int value)
{
    if (g_names[static_cast<std::size_t>(moduleNameId)] != "std::lang")
        PushParseDiagnostic("标准数值构造器必须使用 std::lang:: 前缀");
    return kind == 0 ? M_BigIntCtor(value) : M_BigFloatCtor(value);
}
/**
 * @brief 构造并登记赋值表达式。
 * @param target 赋值目标的节点编号。
 * @param value 值表达式或字面量的语义值。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Assign(int target, int value)
{
    if (value >= 0 && g_nodes[value].kind == MNode::Assign && g_nodes[value].type2 == 1)
        PushParseDiagnostic("复合赋值不能参与链式赋值");
    const int id = M_New(MNode::Assign);
    g_nodes[static_cast<std::size_t>(id)].kids = { target, value };
    return id;
}
/**
 * @brief 将复合赋值 A op= B 展开为普通赋值 A = A op B。
 * @param kind 对应 M_BinOp 的二元运算编号。
 * @param target 赋值目标，同时作为二元运算的左操作数。
 * @param value 二元运算的右操作数。
 * @return 带复合标记的赋值节点；降级时保存目标位置并只求值一次。
 */
int M_CompoundAssign(int kind, int target, int value)
{
    if (value >= 0 && g_nodes[value].kind == MNode::Assign)
        PushParseDiagnostic("复合赋值不能参与链式赋值");
    const int expression = M_BinOp(kind, target, value);
    const int assignment = M_Assign(target, expression);
    g_nodes[assignment].type2 = 1;
    return assignment;
}
/**
 * @brief 构造前后置自增减。
 * @param delta 正负一步。
 * @param prefix 前置标记。
 * @param target 可写目标。
 * @return 新节点。
 */
int M_Update(int delta, int prefix, int target)
{
    const int id = M_New(MNode::Update, "", delta);
    g_nodes[id].kids = { target };
    g_nodes[id].type2 = prefix;
    return id;
}
/**
 * @brief 标记完整表达式，副作用冲突在变量绑定后统一收集。
 * @param expression 表达式。
 * @return 原节点。
 */
int M_CheckUpdates(int expression)
{
    if (expression >= 0) g_nodes[static_cast<std::size_t>(expression)].checkedExpression = true;
    return expression;
}

/**
 * @brief 构造并登记语句块。
 * @param statements 待处理的语句列表。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Block(int statements) { return M_New(MNode::Block, "", statements); }
/**
 * @brief 构造并登记条件分支语句。
 * @param cond 条件表达式的节点编号。
 * @param thenBlock 条件成立时执行的语句块编号。
 * @param elifs 后续条件分支的节点列表编号。
 * @param elseBlock 条件不成立时执行的语句块编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_If(int cond, int thenBlock, int elifs, int elseBlock)
{
    const int id = M_New(MNode::If);
    g_nodes[static_cast<std::size_t>(id)].kids = { cond, thenBlock, elifs, elseBlock };
    return id;
}
/**
 * @brief 将单表达式分支转成带值出口。
 * @param value 分支值。
 * @return 分支块。
 */
int M_ValueBlock(int value)
{
    const int exit = M_Break(value);
    g_nodes[exit].type2 = 1;
    return M_Block(M_List1(exit));
}
/**
 * @brief 条件表达式复用已有的带值控制流出口。
 * @param statement 条件节点。
 * @return 有值节点。
 */
int M_IfValue(int statement)
{
    g_nodes[statement].type2 = 1;
    const int id = M_Loop(M_Block(M_List1(statement)));
    g_nodes[id].s = "if";
    return id;
}
/**
 * @brief 将普通块转换为只执行一次的带值表达式，保留词法作用域清理。
 * @param block 块节点编号。
 * @return 复用带值控制流出口的表达式节点编号。
 */
int M_BlockValue(int block)
{
    const int id = M_Loop(block);
    g_nodes[id].s = "block";
    return id;
}
/**
 * @brief 匹配表达式复用带值出口，并保留语句匹配的控制行为。
 * @param statement 匹配节点。
 * @return 表达式节点。
 */
int M_MatchValue(int statement)
{
    g_nodes[statement].type2 = 1;
    const int id = M_Loop(M_Block(M_List1(statement)));
    g_nodes[id].s = "match";
    return id;
}
/**
 * @brief 提取分支块的语句表。
 * @param patterns 模式。
 * @param guard 守卫。
 * @param block 正文。
 * @return 分支节点。
 */
int M_CaseBlock(int patterns, int guard, int block)
{
    return M_CaseGuard(patterns, guard, static_cast<int>(g_nodes[block].i));
}
/**
 * @brief 插值使用核心 format 的精确语义。
 * @param args 格式串与实参。
 * @return 调用节点。
 */
int M_FString(int args)
{
    return M_CallOn(M_EnumCtor(M_Name("std::lang"), M_Name("format"), M_List()), args);
}
/**
 * @brief 延迟正文提升为借用当前作用域的闭包。
 * @param statement 正文。
 * @return 登记节点。
 */
int M_Defer(int statement)
{
    const int body = g_nodes[statement].kind == MNode::Block ? statement : M_Block(M_List1(statement));
    const int callback = M_Lambda(M_TypeVoid(), M_List(), body, 1);
    const int function = g_nodes[callback].kids[0];
    g_nodes[callback].s += "_defer";
    g_nodes[function].s = g_nodes[callback].s;
    const int id = M_New(MNode::Defer);
    g_nodes[id].kids = { callback };
    return id;
}
/**
 * @brief 块式退出回调只接受 exit。
 * @param mode 模式。
 * @param block 正文。
 * @return 登记节点。
 */
int M_ScopeExit(int mode, int block)
{
    if (g_names[mode] != "exit") return M_New(MNode::ParseBad, "scope 只支持 scope(exit)");
    return M_Defer(block);
}
/**
 * @brief 接口关联类型占位声明。
 * @param name 名称。
 * @return 节点。
 */
int M_AssociatedDecl(int name) { return M_New(MNode::AssociatedDecl, g_names[name]); }
/**
 * @brief 实现关联类型，不产生运行时字段。
 * @param name 名称。
 * @param type 具体类型。
 * @return 节点。
 */
int M_AssociatedDef(int name, int type)
{
    const int id = M_New(MNode::AssociatedDef, g_names[name]);
    g_nodes[id].type = type;
    return id;
}
/**
 * @brief 类型检查使用赋值兼容规则，不执行显式强制转换。
 * @param type 目标。
 * @param value 值。
 * @return 检查节点。
 */
int M_CheckType(int type, int value)
{
    const int id = M_Cast(type, value);
    g_nodes[id].type2 = 2;
    return id;
}
/**
 * @brief 将类型体中的关联类型引用绑定到所属类型，避免同名 Item 混用。
 * @param owner 所属类型。
 * @param members 成员表。
 */
void M_BindAssociatedTypes(const Memory::String &owner, int members)
{
    const auto qualifiedOwner = g_parseModule.empty() ? owner : g_parseModule + "::" + owner;
    Memory::Set<Memory::String> names;
    for (const auto id : g_nodes[members].kids)
        if (g_nodes[id].kind == MNode::AssociatedDecl || g_nodes[id].kind == MNode::AssociatedDef) names.insert(g_nodes[id].s);
    if (names.empty()) return;
    std::function<std::shared_ptr<MType>(const std::shared_ptr<MType> &)> bind;
    bind = [&](const std::shared_ptr<MType> &type) -> std::shared_ptr<MType> {
        if (!type) return nullptr;
        auto result = Memory::makeShared<MType>(*type);
        if (result->kind == MType::Struct && names.count(result->name)) result->name = qualifiedOwner + "::" + result->name;
        result->elem = bind(result->elem);
        result->elem2 = bind(result->elem2);
        for (auto &param : result->paramTypes) param = bind(param);
        return result;
    };
    std::function<void(int)> visit = [&](int id) {
        auto &node = g_nodes[id];
        if (node.type >= 0) node.type = M_AddType(bind(M_TypeAt(node.type)));
        if (node.kind == MNode::Block && node.i >= 0) visit(static_cast<int>(node.i));
        for (const auto child : Memory::Vector<int>(node.kids))
            if (child >= 0) visit(child);
    };
    visit(members);
}
/**
 * @brief 构造并登记后续条件分支。
 * @param cond 条件表达式的节点编号。
 * @param block 语句块编号或待释放的内存块地址。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Elif(int cond, int block)
{
    const int id = M_New(MNode::If);
    g_nodes[static_cast<std::size_t>(id)].kids = { cond, block, M_List(), -1 };
    return id;
}
/**
 * @brief 构造并登记前置条件循环。
 * @param cond 条件表达式的节点编号。
 * @param body 待处理的函数体或循环体。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_While(int cond, int body)
{
    const int id = M_New(MNode::While);
    g_nodes[static_cast<std::size_t>(id)].kids = { cond, body };
    return id;
}
/**
 * @brief 构造并登记后置条件循环。
 * @param body 待处理的函数体或循环体。
 * @param cond 条件表达式的节点编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_DoWhile(int body, int cond)
{
    const int id = M_New(MNode::DoWhile);
    g_nodes[static_cast<std::size_t>(id)].kids = { body, cond };
    return id;
}
/**
 * @brief 构造并登记多路分支语句。
 * @param subject 待匹配表达式的节点编号。
 * @param cases 匹配分支的节点列表编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Switch(int subject, int cases)
{
    const int id = M_New(MNode::Switch);
    g_nodes[static_cast<std::size_t>(id)].kids = { subject, cases };
    return id;
}
/**
 * @brief 构造并登记模式匹配表达式。
 * @param subject 待匹配表达式的节点编号。
 * @param cases 匹配分支的节点列表编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Match(int subject, int cases)
{
    // 同一分支的备选模式展开成共享语句表的 Case；type2 保留分组用于载荷核验。
    const int expanded = M_List();
    const auto original = g_nodes[static_cast<std::size_t>(cases)].kids;
    for (const int caseId : original)
    {
        const MNode item = g_nodes[static_cast<std::size_t>(caseId)];
        if (item.i >= 0 && g_nodes[static_cast<std::size_t>(item.i)].kind == MNode::List)
        {
            const auto alternatives = g_nodes[static_cast<std::size_t>(item.i)].kids;
            for (const int pattern : alternatives)
            {
                const int branch = M_Case(pattern, item.kids[0]);
                g_nodes[static_cast<std::size_t>(branch)].type2 = caseId + 1;
                g_nodes[static_cast<std::size_t>(branch)].guard = item.guard;
                M_ListAdd(expanded, branch);
            }
        }
        else
        {
            M_ListAdd(expanded, caseId);
        }
    }
    const int id = M_Switch(subject, expanded);
    g_nodes[static_cast<std::size_t>(id)].s = "match";
    return id;
}
/**
 * @brief 构造并登记匹配分支。
 * @param value 值表达式或字面量的语义值。
 * @param stmts 语句列表的节点编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Case(int value, int stmts)
{
    const int id = M_New(MNode::Case, "", value);
    g_nodes[static_cast<std::size_t>(id)].kids = { stmts };
    return id;
}
/**
 * @brief 构造守卫分支。
 * @param patterns 模式。
 * @param guard bool 条件。
 * @param statements 正文。
 * @return 节点编号。
 */
int M_CaseGuard(int patterns, int guard, int statements)
{
    const int id = M_Case(patterns, statements);
    g_nodes[id].guard = guard;
    return id;
}
/**
 * @brief 构造忽略绑定。
 * @return 占位节点编号。
 */
int M_DestructureIgnore() { return M_New(MNode::Var, "_"); }
/**
 * @brief 构造局部解构声明。
 * @param bindings 嵌套绑定树。
 * @param value 元组来源。
 * @return 节点编号。
 */
int M_Destructure(int bindings, int value)
{
    const int id = M_New(MNode::Destructure);
    g_nodes[id].kids = { bindings, value };
    return id;
}
/**
 * @brief 普通语句保留原表达式，括号或逗号多目标赋值统一构造解构节点。
 * @param expressions 与集合字面量共用的表达式列表。
 * @return 普通表达式语句或解构赋值节点编号。
 */
int M_AssignTuple(int expressions)
{
    const auto items = g_nodes[static_cast<std::size_t>(expressions)].kids;
    if (items.size() == 1)
    {
        const auto expression = g_nodes[static_cast<std::size_t>(items.front())];
        if (expression.kind == MNode::Assign && expression.type2 != 1 && g_nodes[static_cast<std::size_t>(expression.kids[0])].kind == MNode::Tuple)
        {
            const int id = M_Destructure(expression.kids[0], expression.kids[1]);
            g_nodes[id].i = 1;
            return id;
        }
        return M_ExprStmt(items.front());
    }
    const auto last = g_nodes[static_cast<std::size_t>(items.back())];
    if (last.kind != MNode::Assign || last.type2 == 1)
    {
        PushParseDiagnostic("多目标解构赋值必须使用 = 和一个元组来源");
        return M_New(MNode::ParseBad);
    }
    const int targets = M_List();
    for (std::size_t i = 0; i + 1 < items.size(); ++i) M_ListAdd(targets, items[i]);
    M_ListAdd(targets, last.kids[0]);
    const int id = M_Destructure(targets, last.kids[1]);
    g_nodes[id].i = 1;
    return id;
}
/**
 * @brief 建立左闭区间模式。
 * @param lo 下界。
 * @param hi 上界。
 * @param loClosed 必须为真。
 * @param hiClosed 是否包含上界。
 * @return 区间节点或语义错误。
 */
int M_Range(int lo, int hi, int loClosed, int hiClosed)
{
    if (!loClosed) return M_ParseBad("match 区间仅支持 [a,b) 和 [a,b]，不支持左开区间");
    const int id = M_New(MNode::Range, "", loClosed * 2 + hiClosed);
    g_nodes[static_cast<std::size_t>(id)].kids = { lo, hi };
    return id;
}
/**
 * @brief 构造并登记计数循环。
 * @param init 初始化表达式或语句的节点编号。
 * @param cond 条件表达式的节点编号。
 * @param inc 循环迭代表达式的节点编号。
 * @param body 待处理的函数体或循环体。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_For(int init, int cond, int inc, int body)
{
    /* 循环**本身就是**一个节点（不展开成 while）：`continue` 要落在自增那一步上。
     * kids = {init, cond, inc, body}，前三项 -1 表示"那一段没写"。 */
    const int id = M_New(MNode::For);
    g_nodes[static_cast<std::size_t>(id)].kids = { init, cond, inc, body };
    return id;
}
/**
 * @brief 构造并登记迭代循环。
 * @param typeId 类型池中的类型编号。
 * @param nameId 名称池中的名称编号。
 * @param source 提供输入数据的来源。
 * @param body 待处理的函数体或循环体。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Foreach(int typeId, int nameId, int, int source, int body)
{
    const int id = M_New(MNode::Foreach,
                         g_names[static_cast<std::size_t>(nameId)]);
    g_nodes[static_cast<std::size_t>(id)].type = typeId;
    g_nodes[static_cast<std::size_t>(id)].kids = { source, body };
    return id;
}
/**
 * @brief 构造并登记无条件循环。
 * @param body 待处理的函数体或循环体。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Loop(int body)
{
    const int id = M_New(MNode::Loop);
    g_nodes[static_cast<std::size_t>(id)].kids = { body };
    return id;
}
/**
 * @brief 构造并登记返回语句。
 * @param value 值表达式或字面量的语义值。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Return(int value)
{
    M_CheckUpdates(value);
    const int id = M_New(MNode::Return);
    if (value >= 0) g_nodes[static_cast<std::size_t>(id)].kids.push_back(value);
    return id;
}
#include "semantics/return_expression_ast.cpp"
/**
 * @brief 构造并登记表达式语句。
 * @param value 值表达式或字面量的语义值。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_ExprStmt(int value)
{
    const int id = M_New(MNode::ExprStmt);
    g_nodes[static_cast<std::size_t>(id)].kids = { value };
    return id;
}
/**
 * @brief 将与位与共享的前缀转换为只读引用类型。
 * @param typeExpr 类型名称节点。
 * @return 引用类型编号。
 */
int M_ReadOnlyReferenceType(int typeExpr)
{
    const auto node = g_nodes[static_cast<std::size_t>(typeExpr)];
    int type = -1;
    if (node.kind == MNode::Var)
        type = M_TypeStruct(M_Name(node.s.c_str()));
    else if (node.kind == MNode::EnumCtor)
        type = M_QualifiedType(typeExpr);
    else
    {
        PushParseDiagnostic("引用声明必须使用类型名称");
        type = M_TypeVoid();
    }
    return M_TypePtr(type);
}
/**
 * @brief 构造具名类型的只读引用局部声明。
 * @param typeExpr 类型名称。
 * @param nameId 首个名称。
 * @param value 首个初值。
 * @param rest 后续声明列表，负数表示无后续项。
 * @return 局部声明节点。
 */
int M_ReadOnlyReferenceDecl(int typeExpr, int nameId, int value, int rest)
{
    const int type = M_ReadOnlyReferenceType(typeExpr);
    const int declarations = M_List1(M_Decl(nameId, -1, value));
    if (rest >= 0)
        for (const int child : g_nodes[static_cast<std::size_t>(rest)].kids) M_ListAdd(declarations, child);
    M_SetDeclType(declarations, type, 0);
    return M_LocalDecl(type, declarations, 0);
}
/**
 * @brief 构造并登记变量声明项。
 * @param nameId 名称池中的名称编号。
 * @param arrayDims 数组维度节点列表的编号。
 * @param init 初始化表达式或语句的节点编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Decl(int nameId, int arrayDims, int init)
{
    const int id = M_New(MNode::Declare, g_names[static_cast<std::size_t>(nameId)],
                         arrayDims < 0 ? -1 : arrayDims);
    g_nodes[static_cast<std::size_t>(id)].type = g_globalType; /* 由 decl_head 设好 */
    g_nodes[static_cast<std::size_t>(id)].i = arrayDims;
    g_nodes[static_cast<std::size_t>(id)].kids.push_back(init);
    return id;
}
/**
 * @brief 构造并登记多维数组变量声明项。
 * @param nameId 名称池中的名称编号。
 * @param dims 数组维度节点列表的编号。
 * @param init 初始化表达式或语句的节点编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Decl2(int nameId, int dims, int init)
{
    const int id = M_Decl(nameId, dims, init);
    g_nodes[static_cast<std::size_t>(id)].i = dims;
    return id;
}
/**
 * @brief `new`（count = -1）或 `new [count]`。
 *
 * **元素类型不写在 new 后面**：降级时从"被赋值的变量 / 字段的类型"取
 * （`T mut& p = new;` / `T a[] = new [n];`），所以 `new` 只能出现在声明或赋值里。
 *
 * 0.20.7 起 `new(实参)` 已删除：需要初值就写 `T v = T::create(实参);`（`static T create(...)` 工厂）。
 * @param count 元素个数表达式；-1 表示单个对象。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_NewPtr(int count)
{
    /* `new` / `new [n]`：i = 元素个数表达式（-1 = 单个）。
     * 元素类型不在这儿定 —— 降级时从"被赋值的那个东西的类型"取。
     * `new(实参)` 那条路 0.20.7 起已删除（构造走 `static T create(...)` 工厂）。 */
    const int id = M_New(MNode::NewPtr);
    g_nodes[static_cast<std::size_t>(id)].i = count;
    return id;
}

/**
 * @brief 构造调用节点：按**目标类型**展开一个结构体的构造函数体。
 *
 * 两个来源共用这一个节点：
 *   * 用户语法 `T v = new(实参);` → `M_Construct(-1, $3, -1)`，类型从声明目标推，
 *     构造函数在降级时按名字 + 实参重载挑选；
 *   * `static T create(形参)` 工厂的内部展开 → `M_Construct(类型, 实参表, 工厂修饰名)`，
 *     `skipName` 用来把正在展开的这条工厂排除掉，免得工厂体递归调自己。
 *
 * @param typeId   目标类型（-1 = 由降级阶段的声明目标决定）。
 * @param args     实参列表节点。
 * @param skipName 要排除的重载名（-1 = 不排除）。
 * @return 节点 id。
 */
int M_Construct(int typeId, int args, int skipName)
{
    const int id = M_New(MNode::Construct);
    g_nodes[static_cast<std::size_t>(id)].type = typeId;
    if (skipName >= 0)
        g_nodes[static_cast<std::size_t>(id)].s = g_names[static_cast<std::size_t>(skipName)];
    g_nodes[static_cast<std::size_t>(id)].kids = { args };
    return id;
}
/**
 * @brief 将退出值中的直接变量转换为标准库移动调用，元组逐项应用相同规则。
 * @param value 原始退出表达式编号。
 * @retval 转换后的退出表达式编号；临时表达式保持原样。
 */
int M_BreakValue(int value)
{
    if (value < 0) return value;
    const MNode expression = g_nodes[static_cast<std::size_t>(value)];
    if (expression.kind == MNode::Tuple)
    {
        Memory::Vector<int> items;
        for (const int child : expression.kids) items.push_back(M_BreakValue(child));
        g_nodes[static_cast<std::size_t>(value)].kids = std::move(items);
        return value;
    }
    if (expression.kind != MNode::Var) return value;
    return M_Move(value);
}
/**
 * @brief 构造带移动退出值的循环或块退出语句。
 * @param value 值表达式或字面量的语义值。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Break(int value)
{
    const int id = M_New(MNode::Break);
    if (value >= 0) g_nodes[static_cast<std::size_t>(id)].kids = { value };
    return id;
}
/**
 * @brief `continue;`：直接进入最内层循环的下一轮。
 *
 * `match` 不截获 `break`，也不影响 `continue` 的目标（就是外面那层循环）。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Continue() { return M_New(MNode::Continue); }

/**
 * @brief 构造指定循环层数的退出语句并校验正整数层数。
 * @param level 整数字面量节点编号；负一表示退出全部循环。
 * @retval 返回保存层数的退出节点编号；非法层数登记语义错误。
 */
int M_BreakLevel(int level)
{
    const int id = M_Break(-1);
    if (level < 0)
        g_nodes[id].i = -1;
    else
    {
        const MNode value = g_nodes[level];
        if (value.kind != MNode::Int || value.i <= 0 || value.unsignedLiteral)
            return M_ParseBad("break #level(N) 的 N 必须是正整数字面量");
        else
            g_nodes[id].i = value.i;
    }
    return id;
}

/**
 * @brief 语法糖辅助：大小写无关地判定名称池中的名字是否等于给定文本。
 * @param nameId 名称池编号。
 * @param text 目标文本（ASCII）。
 * @retval 名字与 text 大小写无关相等时返回 true。
 * @note 不引入关键字，保留 level / all 作为普通标识符。
 */
bool M_NameIs(int nameId, const char *text)
{
    if (nameId < 0 || nameId >= static_cast<int>(g_names.size()) || text == nullptr) return false;
    const std::string &nm = g_names[static_cast<std::size_t>(nameId)];
    for (size_t i = 0; text[i] != '\0'; ++i)
    {
        char a = nm[i], b = text[i];
        if (a >= 'a' && a <= 'z') a -= static_cast<char>('a' - 'A');
        if (b >= 'a' && b <= 'z') b -= static_cast<char>('a' - 'A');
        if (a != b) return false;
    }
    return nm.size() == std::strlen(text);
}

/**
 * @brief `break level N;` / `break level all;` 语法糖的层级名判定辅助（文法动作不支持字符串字面量）。
 * @param nameId 名称池编号。
 * @retval 名字大小写无关等于 level / all 时返回 true。
 */
bool M_NameIsLevel(int nameId) { return M_NameIs(nameId, "level"); }
bool M_NameIsAll(int nameId) { return M_NameIs(nameId, "all"); }

/**
 * @brief `break level N;` 语法糖的参数错误节点（文法动作不支持字符串字面量）。
 */
int M_ParseBadLevelSugar() { return M_ParseBad("break level 后必须跟正整数字面量层数，形如 break level 2;"); }
int M_ParseBadLevelAllSugar() { return M_ParseBad("break level 后只能跟 all，形如 break level all;"); }
int M_ParseBadBreakAllSugar() { return M_ParseBad("break 仅支持 break all 退出全部循环，带值退出请写 yield 值"); }

/**
 * @brief 构造并登记实例字段声明。
 * @param typeId 类型池中的类型编号。
 * @param nameId 名称池中的名称编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Field(int typeId, int nameId)
{
    const int id = M_New(MNode::Field, g_names[static_cast<std::size_t>(nameId)]);
    g_nodes[static_cast<std::size_t>(id)].type = typeId;
    g_nodes[id].i = -1;
    return id;
}
/**
 * @brief 保存泛型闭包类型参数的名称、接口约束与默认类型。
 * @param nameId 名称池中的类型参数编号。
 * @param constraint 接口约束类型编号，负数表示无约束。
 * @param fallback 默认类型编号，负数表示无默认值。
 * @return 参数语法节点编号。
 */
int M_GenericClosureParameter(int nameId, int constraint, int fallback)
{
    const int id = M_New(MNode::Params, g_names[nameId]);
    g_nodes[id].type = fallback;
    g_nodes[id].type2 = constraint;
    return id;
}

/**
 * @brief 构造具有默认闭包表达式的自动类型字段。
 * @param nameId 字段名称编号。
 * @param initializer 初始化表达式编号。
 * @return 字段声明节点。
 */
int M_ClosureField(int nameId, int initializer)
{
    const int id = M_Field(-1, nameId);
    g_nodes[id].kids = { initializer };
    return id;
}

/**
 * @brief 构造泛型闭包的显式类型选择，保留原闭包环境表达式。
 * @param value 闭包值表达式。
 * @param arguments 类型参数节点列表。
 * @return 选择表达式编号。
 */
int M_ClosureSelect(int value, int arguments)
{
    const int id = M_New(MNode::GenericClosureSelect);
    g_nodes[id].kids = { value, arguments };
    return id;
}

/**
 * @brief 数组字段的尺寸在类型登记时检查并内联铺开。
 * @param type 元素。
 * @param name 名称。
 * @param dims 维度。
 * @return 字段节点。
 */
int M_ArrayField(int type, int name, int dims)
{
    const int id = M_Field(type, name);
    g_nodes[id].i = dims;
    return id;
}
/**
 * @brief 构造并登记静态字段声明。
 * @param typeId 类型池中的类型编号。
 * @param nameId 名称池中的名称编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_StaticField(int typeId, int nameId)
{
    const int id = M_Field(typeId, nameId);
    g_nodes[static_cast<std::size_t>(id)].type2 = 1;
    return id;
}
/**
 * @brief 构造并登记实例方法声明。
 * @param resultType 函数或表达式的结果类型。
 * @param nameId 名称池中的名称编号。
 * @param opName 运算符名称的编号。
 * @param params 形参或类型参数列表。
 * @param body 待处理的函数体或循环体。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Method(int resultType, int nameId, int opName, int params, int body)
{
    const int id = M_New(MNode::MethodDef, g_names[static_cast<std::size_t>(nameId)]);
    /* `s2` 放运算符文本（普通方法为空）：`loperator` + `+` 合起来才是方法名。 */
    g_nodes[static_cast<std::size_t>(id)].s2 =
        (opName >= 0) ? g_names[static_cast<std::size_t>(opName)] : Memory::String();
    g_nodes[static_cast<std::size_t>(id)].type = resultType;
    g_nodes[static_cast<std::size_t>(id)].kids = { params, body };
    return id;
}
/**
 * @brief 构造并登记静态方法声明。
 * @param resultType 函数或表达式的结果类型。
 * @param nameId 名称池中的名称编号。
 * @param params 形参或类型参数列表。
 * @param body 待处理的函数体或循环体。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_StaticMethod(int resultType, int nameId, int params, int body)
{
    const int id = M_Method(resultType, nameId, -1, params, body);
    g_nodes[static_cast<std::size_t>(id)].type2 = 1;
    return id;
}
/**
 * @brief 构造并登记构造函数声明。
 * @param nameId 名称池中的名称编号。
 * @param params 形参或类型参数列表。
 * @param body 待处理的函数体或循环体。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Constructor(int nameId, int params, int body)
{
    const int id = M_Method(M_TypeVoid(), nameId, -1, params, body);
    g_nodes[static_cast<std::size_t>(id)].type2 = 2;
    return id;
}
/**
 * @brief 构造并登记运算符声明。
 * @param resultType 函数或表达式的结果类型。
 * @param nameId 名称池中的名称编号。
 * @param opName 运算符名称的编号。
 * @param params 形参或类型参数列表。
 * @param body 待处理的函数体或循环体。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_OperatorDef(int resultType, int nameId, int opName, int params, int body)
{
    /* s = 名字（应当就是 `operator`），s2 = 运算符文本，type = 返回类型 C，
     * kids = {形参表（左、右两个）, 函数体}。 */
    const int id = M_New(MNode::OperatorDef, g_names[static_cast<std::size_t>(nameId)]);
    g_nodes[static_cast<std::size_t>(id)].s2 = g_names[static_cast<std::size_t>(opName)];
    g_nodes[static_cast<std::size_t>(id)].type = resultType;
    g_nodes[static_cast<std::size_t>(id)].kids = { params, body };
    return id;
}
/**
 * @brief 将普通函数声明标记为动态库导出入口。
 * @param declaration 需要公开的函数声明节点编号。
 * @return 原函数声明节点编号。
 */
int M_Export(int declaration)
{
    g_nodes[static_cast<std::size_t>(declaration)].exported = true;
    return declaration;
}
/**
 * @brief 登记函数的返回类型、名称、形参与函数体。
 * @param resultType 返回类型编号，负值表示推导。
 * @param nameId 名称池编号。
 * @param params 形参列表节点。
 * @param body 函数体节点。
 * @return 函数声明节点编号。
 */
int M_Func(int resultType, int nameId, int params, int body)
{
    const int id = M_New(MNode::FuncDef, g_names[static_cast<std::size_t>(nameId)]);
    g_nodes[static_cast<std::size_t>(id)].type = resultType;
    g_nodes[static_cast<std::size_t>(id)].kids = { params, body };
    return id;
}
/** @brief 生命周期契约的跨翻译单元登记表：函数声明节点编号 → 指令列表节点编号。 */
Memory::Map<int, int> chali_lifetime::g_directiveNodes;
/** @brief 当前函数体里声明过的局部变量名：函数体的作用域在核对时已出栈，靠这里补。 */
Memory::Set<Memory::String> chali_lifetime::g_localNames;
/**
 * @brief 取 `#ret` 在名称池里的编号。
 * @retval `#ret` 名称编号。
 */
int M_LifetimeRet()
{
    return M_Name(chali_lifetime::RetName());
}
/**
 * @brief 建立一条 `#borrow(out, [in...])` 指令。
 * @param target out 的名称编号（可能是 `#ret`）。
 * @param sources in 的名称编号列表。
 * @return 指令节点编号。
 */
int M_BorrowDirective(int target, int sources)
{
    const int id = M_New(MNode::List, "#borrow");
    g_nodes[static_cast<std::size_t>(id)].i = target;
    g_nodes[static_cast<std::size_t>(id)].kids = g_nodes[static_cast<std::size_t>(sources)].kids;
    return id;
}
/**
 * @brief 建立一条 `#static(a...)` 指令。
 * @param names 声明为 'static 的来源名称编号列表。
 * @return 指令节点编号。
 */
int M_LifetimeStatic(int names)
{
    const int id = M_New(MNode::List, "#static");
    g_nodes[static_cast<std::size_t>(id)].kids = g_nodes[static_cast<std::size_t>(names)].kids;
    return id;
}
/**
 * @brief 把生命周期指令挂到函数声明上，原样返回函数节点。
 * @param directives 指令列表节点编号。
 * @param function 函数声明节点编号。
 * @return 函数声明节点编号。
 */
int M_LifetimeContract(int directives, int function)
{
    chali_lifetime::g_directiveNodes[function] = directives;
    return function;
}
/**
 * @brief 建立 lambda。
 * @param resultType 返回类型。
 * @param params 形参。
 * @param body 函数体。
 * @param captureMode 默认捕获方式。
 * @return 表达式节点。
 */
int M_Lambda(int resultType, int params, int body, int captureMode)
{
    for (const int param : g_nodes[params].kids)
        if (g_nodes[param].type < 0)
            return M_New(MNode::ParseBad, "lambda 形参需要可确定的类型上下文");
    if (g_nodes[body].kind != MNode::Block)
    {
        const bool isVoid = resultType >= 0 && M_TypeAt(resultType)->kind == MType::Void;
        body = M_Block(M_List1(isVoid ? M_ExprStmt(body) : M_Return(body)));
    }
    const Memory::String name = Memory::String("__chali_lambda_ast_") + (captureMode == 1 ? "mut_" : captureMode == 2 ? "copy_" :
                                                                                                 captureMode == 3     ? "move_" :
                                                                                                                        "const_")
                                + Memory::toString(g_nodes.size());
    const int function = M_New(MNode::FuncDef, name);
    g_nodes[function].type = resultType;
    g_nodes[function].kids = { params, body };
    const int id = M_New(MNode::Lambda, name);
    g_nodes[id].kids = { function };
    return id;
}
/**
 * @brief 建立捕获分组。
 * @param mode 捕获方式。
 * @param names 变量名称列表。
 * @return 分组节点。
 */
int M_CaptureGroup(int mode, int names)
{
    const int id = M_List();
    g_nodes[id].i = mode;
    g_nodes[id].kids = g_nodes[names].kids;
    return id;
}
/**
 * @brief 建立列表式 lambda。
 * @param resultType 返回类型。
 * @param params 形参。
 * @param body 函数体。
 * @param groups 捕获列表。
 * @param defaultMode 默认捕获方式。
 * @return 表达式节点。
 */
int M_LambdaListed(int resultType, int params, int body, int groups, int defaultMode)
{
    Memory::Map<Memory::String, int> specifications;
    for (const int group : g_nodes[groups].kids)
        for (const int name : g_nodes[group].kids)
            if (!specifications.emplace(g_names[name], static_cast<int>(g_nodes[group].i)).second)
                return M_New(MNode::ParseBad, "lambda 捕获列表重复声明变量：" + g_names[name]);
    const int id = M_Lambda(resultType, params, body, defaultMode);
    if (g_nodes[id].kind == MNode::Lambda)
    {
        chali_lambda::captureSpecifications[g_nodes[id].s] = std::move(specifications);
        const int captures = M_List();
        for (const int group : g_nodes[groups].kids)
            for (const int name : g_nodes[group].kids)
            {
                const int reference = M_Var(name);
                const int capture = M_List1(reference);
                g_nodes[capture].i = g_nodes[group].i;
                M_ListAdd(captures, capture);
            }
        g_nodes[id].kids.push_back(captures);
    }
    return id;
}
#include "semantics/named_function_ast.cpp"
/**
 * @brief 为函数声明添加内联标记。
 * @param declarationId 待标记的声明节点编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Inline(int declarationId)
{
    MNode &declaration = g_nodes[static_cast<std::size_t>(declarationId)];
    declaration.moduleLocal = true;
    return declarationId;
}
/**
 * @brief 标记显式 unsafe 块或具有不安全调用契约的函数。
 * @param id 块或函数节点。
 * @return 标记后的原节点。
 */
int M_Unsafe(int id)
{
    auto &node = g_nodes[id];
    if (node.kind == MNode::Block) node.unsafeScope = true;
    else {
        node.unsafeRequired = true;
        if (node.kind == MNode::FuncDef && node.kids.size() > 1u)
            g_nodes[node.kids[1]].unsafeScope = true;
    }
    return id;
}
/**
 * @brief 将 C ABI 编译器指令附着于库声明或导出函数。
 * @param convention ABI 字符串编号。
 * @param declaration 声明节点。
 * @return 声明或明确的诊断节点。
 */
int M_AbiDeclaration(int convention, int declaration)
{
    const auto mode = g_names[convention];
    if (mode != "C") return M_ParseBad("#abi 当前仅支持 C 调用约定");
    auto &node = g_nodes[declaration];
    node.abiConvention = "C";
    if (node.kind == MNode::AbiBlock)
        for (const auto child : node.kids) g_nodes[child].abiConvention = "C";
    return declaration;
}
/**
 * @brief 将逐函数 ABI 指令绑定到一个外部声明，保留库路径的源码基准。
 * @param convention ABI 调用约定字符串编号。
 * @param library 动态库路径字符串编号。
 * @param symbol 动态库原生符号字符串编号。
 * @param declaration 仅包含签名的外部函数声明。
 * @return 已标记的声明或调用约定诊断节点。
 */
int M_AbiImport(int convention, int library, int symbol, int declaration)
{
    const int result = M_AbiDeclaration(convention, declaration);
    if (g_nodes[result].kind == MNode::ParseBad) return result;
    auto &node = g_nodes[result];
    node.abiLibrary = g_names[library];
    node.s2 = g_names[symbol];
    const auto source = g_stringSources.find(library);
    if (source != g_stringSources.end()) node.abiSourceFile = source->second;
    return result;
}
/**
 * @brief 标记 C ABI 导出实现并记录由指令指定的原生符号。
 * @param symbol 符号字符串编号。
 * @param declaration 函数实现节点。
 * @return 已标记节点。
 */
int M_ExportSymbol(int symbol, int declaration)
{
    auto &node = g_nodes[declaration];
    if (g_names[symbol].empty()) return M_ParseBad("#export 原生符号名不能为空");
    node.exported = true;
    node.exportSymbol = g_names[symbol];
    node.abiConvention = "C";
    return declaration;
}
/**
 * @brief 构造并登记外部函数声明。
 * @param resultType 函数或表达式的结果类型。
 * @param nameId 名称池中的名称编号。
 * @param params 形参或类型参数列表。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_ExternFunc(int resultType, int nameId, int params)
{
    const int id = M_New(MNode::ExternFuncDecl, g_names[static_cast<std::size_t>(nameId)]);
    g_nodes[static_cast<std::size_t>(id)].type = resultType;
    g_nodes[static_cast<std::size_t>(id)].kids = { params };
    return id;
}
/**
 * @brief 构造并登记C ABI 函数声明。
 * @param resultType 函数或表达式的结果类型。
 * @param nameId 名称池中的名称编号。
 * @param params 形参或类型参数列表。
 * @param symbolId 外部 ABI 符号名的编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_AbiFunc(int resultType, int nameId, int params, int symbolId)
{
    const int id = M_ExternFunc(resultType, nameId, params);
    if (symbolId >= 0)
        g_nodes[static_cast<std::size_t>(id)].s2 =
            g_names[static_cast<std::size_t>(symbolId)];
    return id;
}
/**
 * @brief 构造并登记C ABI 缓冲区返回函数声明。
 * @param resultType 函数或表达式的结果类型。
 * @param nameId 名称池中的名称编号。
 * @param params 形参或类型参数列表。
 * @param symbolId 外部 ABI 符号名的编号。
 * @param lengthId 外部缓冲区长度描述的编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_AbiBufferFunc(int resultType, int nameId, int params, int symbolId, int lengthId)
{
    const int id = M_AbiFunc(resultType, nameId, params, symbolId);
    g_nodes[static_cast<std::size_t>(id)].abiLength = g_names[static_cast<std::size_t>(lengthId)];
    return id;
}
/**
 * @brief 将复制后释放约定附着到 ABI 函数声明。
 * @param function ABI 函数节点编号。
 * @param release 释放符号字符串编号。
 * @return 原函数节点编号。
 */
int M_AbiOwned(int function, int release)
{
    g_nodes[function].abiRelease = g_names[release];
    return function;
}
/**
 * @brief 构造并登记外部 ABI 声明块。
 * @param libraryId 外部动态库路径的编号。
 * @param functionsId 外部函数声明列表的编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_AbiBlock(int libraryId, int functionsId)
{
    const int id = M_New(MNode::AbiBlock);
    MNode &block = g_nodes[static_cast<std::size_t>(id)];
    block.s = g_names[static_cast<std::size_t>(libraryId)];
    block.kids = g_nodes[static_cast<std::size_t>(functionsId)].kids;
    for (const int functionId : block.kids)
    {
        auto &function = g_nodes[static_cast<std::size_t>(functionId)];
        function.abiLibrary = block.s;
        const auto source = g_stringSources.find(libraryId);
        if (source != g_stringSources.end()) function.abiSourceFile = source->second;
    }
    return id;
}
/**
 * @brief 构造不透明胶水类型及其方法绑定列表。
 * @param nameId 类型名称编号。
 * @param methods 生命周期方法列表编号。
 * @return opcode 声明节点编号。
 */
int M_Opcode(int nameId, int methods)
{
    const int id = M_New(MNode::OpcodeDecl, g_names[nameId]);
    g_nodes[id].kids.push_back(methods);
    return id;
}
/**
 * @brief 记录一个 opcode 生命周期胶水函数。
 * @param operation 创建、销毁或复制的方法种类。
 * @param symbol 绑定函数名称编号。
 * @return 方法节点编号。
 */
int M_OpcodeMethod(int operation, int symbol)
{
    const int id = M_New(MNode::OpcodeMethod, g_names[symbol]);
    g_nodes[id].i = operation;
    return id;
}
/**
 * @brief 标记 opcode 类型禁止普通值复制。
 * @param declaration 待标记的声明节点。
 * @return 原声明节点编号。
 */
int M_NonCopyableOpcode(int declaration)
{
    g_nodes[declaration].nonCopyable = true;
    return declaration;
}

/**
 * @brief 构造并登记宿主类型声明。
 * @param nameId 名称池中的名称编号。
 * @param abiTypeId 宿主底层 ABI 类型的编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_HostType(int nameId, int abiTypeId)
{
    const int id = M_New(MNode::HostTypeDecl, g_names[static_cast<std::size_t>(nameId)]);
    g_nodes[static_cast<std::size_t>(id)].type = abiTypeId;
    return id;
}
/**
 * @brief 构造并登记宿主接口实现声明。
 * @param ownerTypeId 宿主类型的编号。
 * @param interfaceNameId 接口名称的编号。
 * @param functionNameId 实现函数名称的编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_HostImpl(int ownerTypeId, int interfaceNameId, int functionNameId)
{
    const int id = M_New(MNode::HostImplDecl);
    g_nodes[static_cast<std::size_t>(id)].type = ownerTypeId;
    g_nodes[static_cast<std::size_t>(id)].s2 =
        g_names[static_cast<std::size_t>(interfaceNameId)];
    g_nodes[static_cast<std::size_t>(id)].i = functionNameId;
    return id;
}
/**
 * @brief 解析期登记顶层类型时顺手查保留名（`Fail` 在解析期会被 ResetState 抹掉，所以走诊断表）。
 * @param name 名称文本。
 */
void NoteReservedCoreType(const Memory::String &name)
{
    if (g_parseModule == "std::lang") return;
    static const Memory::Set<Memory::String> reserved{
        "List", "Set", "Map", "Option", "Result", "Any", "range",
        "Index", "Iterator"
    };
    if (reserved.count(name) != 0u)
        PushParseDiagnostic("`" + name + "` 是核心 prelude 的保留名，不能声明为顶层类型；"
                                         "标准库请写 `import std::xxx;` 再用 `std::xxx::"
                            + name + "`");
}

/**
 * @brief `struct 名字 : 接口1, 接口2 { … };`。
 * @param nameId 结构体名。
 * @param membersId 成员表。
 * @param interfacesId 继承的接口名列表（`iface_list` 的产物）；-1 = 没有继承。
 * @return StructDecl 节点。
 * @param members 类型成员声明列表的编号。
 */
int M_Struct(int nameId, int members, int interfacesId)
{
    M_BindAssociatedTypes(g_names[nameId], members);
    NoteReservedCoreType(g_names[static_cast<std::size_t>(nameId)]);
    const int id = M_New(MNode::StructDecl, g_names[static_cast<std::size_t>(nameId)]);
    /* `i` 记"继承的接口名列表"（-1 = 没有继承）：kids[0] 仍是成员表。 */
    g_nodes[static_cast<std::size_t>(id)].i = interfacesId;
    g_nodes[static_cast<std::size_t>(id)].kids.push_back(members);
    return id;
}
/**
 * @brief 构造并登记紧凑布局结构体声明。
 * @param nameId 名称池中的名称编号。
 * @param qualifierId 结构体布局限定信息的编号。
 * @param members 类型成员声明列表的编号。
 * @param interfacesId 实现接口列表的编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_PackedStruct(int nameId, int qualifierId, int members, int interfacesId)
{
    const int id = M_Struct(nameId, members, interfacesId);
    if (g_names[static_cast<std::size_t>(qualifierId)] != "packed")
        PushParseDiagnostic("struct ABI qualifier must be packed");
    g_nodes[static_cast<std::size_t>(id)].abiPacked = true;
    return id;
}
/**
 * @brief 构造并登记枚举声明。
 * @param nameId 名称池中的名称编号。
 * @param variants 枚举变体声明列表。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Enum(int nameId, int variants)
{
    /* 注意：这里**不加**保留名检查 —— `M_EnumDecl` 也被内部合成类型（接口、元组）用到，
     * 那些名字不受用户面约束。保留名由 M_Struct 那条路（用户显式写 `struct X`）兜。 */
    const int id = M_New(MNode::EnumDecl, g_names[static_cast<std::size_t>(nameId)]);
    g_nodes[static_cast<std::size_t>(id)].kids = { variants };
    return id;
}
/**
 * @brief 构造并登记带方法的枚举声明。
 * @param nameId 名称池中的名称编号。
 * @param variants 枚举变体声明列表。
 * @param members 类型成员声明列表的编号。
 * @param interfacesId 实现接口列表的编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_EnumWithMethods(int nameId, int variants, int members, int interfacesId)
{
    const int id = M_Enum(nameId, variants);
    g_nodes[static_cast<std::size_t>(id)].kids.push_back(members);
    g_nodes[static_cast<std::size_t>(id)].i = interfacesId;
    return id;
}
#include "semantics/derive_ast.cpp"
/**
 * @brief 构造并登记枚举变体声明。
 * @param nameId 名称池中的名称编号。
 * @param paramsId 枚举载荷形参列表的编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_EnumVariant(int nameId, int paramsId)
{
    const int id = M_New(MNode::EnumVariant, g_names[static_cast<std::size_t>(nameId)]);
    if (paramsId >= 0) g_nodes[static_cast<std::size_t>(id)].kids = { paramsId };
    return id;
}
/**
 * @brief 构造并登记枚举变体构造表达式。
 * @param enumNameId 枚举类型名称的编号。
 * @param variantNameId 枚举变体名称的编号。
 * @param argsId 枚举载荷实参列表的编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_EnumCtor(int enumNameId, int variantNameId, int argsId)
{
    /* 限定构造器：`模块::名字`、`模块::子模块::名字`，或者 `模块::类型::成员`。
     * 类型与它的成员在表里都按**裸名**登记，所以"三段及以上、最后一段是大驼峰"的写法
     * 一律归一到最后一段（`std::io::SeekOrigin::Begin` == `SeekOrigin::Begin`）；
     * 只有一段限定（`std::numtheory::gcd`）按模块函数保留全名。 */
    Memory::String owner = g_names[static_cast<std::size_t>(enumNameId)];
    const std::size_t firstScope = owner.find("::");
    const std::size_t lastScope = owner.rfind("::");
    if (firstScope != Memory::String::npos && lastScope != firstScope)
    {
        const Memory::String tail = owner.substr(lastScope + 2u);
        if (!tail.empty() && std::isupper(static_cast<unsigned char>(tail[0])))
            owner = tail;
    }
    const int id = M_New(MNode::EnumCtor, owner);
    g_nodes[static_cast<std::size_t>(id)].s2 =
        g_names[static_cast<std::size_t>(variantNameId)];
    g_nodes[static_cast<std::size_t>(id)].kids =
        g_nodes[static_cast<std::size_t>(argsId)].kids;
    return id;
}
/**
 * @brief 构造并登记错误传播表达式。
 * @param value 值表达式或字面量的语义值。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Try(int value)
{
    const int id = M_New(MNode::Try);
    g_nodes[static_cast<std::size_t>(id)].kids = { value };
    return id;
}
/**
 * @brief `interface 名字 { 函数声明… };`：接口**只声明函数**（没有字段、没有函数体）。
 *
 * 接口是**编译期契约**：继承它的结构体必须把每个函数按同样的签名实现出来
 * （登记时逐个核对，漏了直接报错）。接口可以继承别的接口（见 `M_InterfaceInherit`）。
 * @param nameId 接口名。
 * @param membersId 函数声明表（`iface_member_list` 的产物）。
 * @return InterfaceDecl 节点。
 */
int M_Interface(int nameId, int membersId)
{
    M_BindAssociatedTypes(g_names[nameId], membersId);
    const int id = M_New(MNode::InterfaceDecl, g_names[static_cast<std::size_t>(nameId)]);
    /* `i` 记"继承的基接口名列表"（-1 = 不继承）：与 `struct X : A, B` 同一套记法。 */
    g_nodes[static_cast<std::size_t>(id)].i = -1;
    g_nodes[static_cast<std::size_t>(id)].kids = { membersId };
    return id;
}
/**
 * 接口方法：body 为 -1 时要求实现者提供方法，否则可由实现者直接继承。
 */
/**
 * @brief 构造仅声明的静态协议方法。
 * @param resultType 返回类型。
 * @param nameId 名称。
 * @param params 参数。
 * @return 方法节点。
 */
int M_StaticIfaceMethod(int resultType, int nameId, int params)
{
    const int id = M_IfaceMethod(resultType, nameId, params, -1);
    g_nodes[id].type2 = 1;
    return id;
}
/**
 * @brief 构造并登记接口方法声明。
 * @param resultType 函数或表达式的结果类型。
 * @param nameId 名称池中的名称编号。
 * @param params 形参或类型参数列表。
 * @param body 待处理的函数体或循环体。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_IfaceMethod(int resultType, int nameId, int params, int body)
{
    const int id = M_New(MNode::MethodDef, g_names[static_cast<std::size_t>(nameId)]);
    g_nodes[static_cast<std::size_t>(id)].type = resultType;
    g_nodes[static_cast<std::size_t>(id)].kids = { params, body };
    return id;
}
/**
 * @brief 拒绝接口中的字段声明并记录诊断。
 * @param nameId 名称池中的名称编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_InterfaceFieldRejected(int nameId)
{
    return M_ParseBad("接口不能定义字段：" + g_names[static_cast<std::size_t>(nameId)]);
}
/**
 * @brief `interface A : B, C { … };` —— 带基接口的接口声明。
 * @param nameId 接口名。
 * @param basesId 基接口名列表（`iface_list` 的产物）。
 * @param membersId 函数声明表（`iface_member_list` 的产物）。
 * @return InterfaceDecl 节点（`i` 记基接口名列表）。
 */
int M_InterfaceInherit(int nameId, int basesId, int membersId)
{
    const int id = M_Interface(nameId, membersId);
    g_nodes[static_cast<std::size_t>(id)].i = basesId;
    return id;
}
/**
 * @brief `__typeTag(类型)`：这个类型的**编号**（编译期就折成一个整数）。
 * @param typeId 类型池下标。
 * @return NameOf 节点。
 */
int M_TypeTagType(int typeId)
{
    const int id = M_New(MNode::TypeTag);
    g_nodes[static_cast<std::size_t>(id)].type = typeId;
    g_nodes[static_cast<std::size_t>(id)].reflectionBridge = true;
    return id;
}
/**
 * @brief 构造 unsafe 原始布局查询，不公开 C ABI 的布局作为语言存储布局。
 * @param typeId 语言类型编号。
 * @param alignment 非零查询对齐。
 * @return 查询节点。
 */
int M_RawLayout(int typeId, int alignment)
{
    const int id = M_New(MNode::RawLayout);
    g_nodes[id].type = typeId;
    g_nodes[id].i = alignment;
    return id;
}

/**
 * @brief 构造并登记形参声明。
 * @param typeId 类型池中的类型编号。
 * @param nameId 名称池中的名称编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Param(int typeId, int nameId)
{
    const int id = M_New(MNode::Params, g_names[static_cast<std::size_t>(nameId)]);
    g_nodes[static_cast<std::size_t>(id)].type = typeId;
    return id;
}
/**
 * @brief 保存形参默认表达式。
 * @param paramId 形参节点。
 * @param exprId 默认值表达式。
 * @return 形参节点。
 */
int M_DefaultParam(int paramId, int exprId)
{
    g_nodes[static_cast<std::size_t>(paramId)].kids = { exprId };
    return paramId;
}
/**
 * @brief 构造并登记可变形参声明。
 * @param typeId 类型池中的类型编号。
 * @param nameId 名称池中的名称编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_VariadicParam(int typeId, int nameId)
{
    const int id = M_Param(M_TypeVec(typeId), nameId);
    g_nodes[static_cast<std::size_t>(id)].i = 1;
    return id;
}
/**
 * @brief 构造并登记全局变量声明。
 * @param typeId 类型池中的类型编号。
 * @param declarators 变量声明项列表的编号。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_Global(int typeId, int declarators)
{
    const int id = M_New(MNode::Declare);
    g_nodes[static_cast<std::size_t>(id)].type = typeId;
    g_nodes[static_cast<std::size_t>(id)].kids = { declarators };
    return id;
}
/**
 * @brief 构造并登记带限定符的全局变量声明。
 * @param typeId 类型池中的类型编号。
 * @param declarators 变量声明项列表的编号。
 * @param constFlag 是否使用只读声明限定。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_GlobalDecl(int typeId, int declarators, int constFlag)
{
    g_globalType = typeId;
    const int id = M_Global(typeId, declarators);
    g_nodes[static_cast<std::size_t>(id)].type2 = constFlag; /* 整组声明是不是 const */
    return id;
}
/**
 * @brief 构造并登记带限定符的局部变量声明。
 * @param typeId 类型池中的类型编号。
 * @param declarators 变量声明项列表的编号。
 * @param constFlag 是否使用只读声明限定。
 * @retval 生成的语法节点在节点池中的编号。
 */
int M_LocalDecl(int typeId, int declarators, int constFlag)
{
    const int id = M_Global(typeId, declarators);
    g_nodes[static_cast<std::size_t>(id)].s = "local";
    g_nodes[static_cast<std::size_t>(id)].type2 = constFlag;
    return id;
}
/**
 * @brief 保存本次解析的顶层声明列表。
 * @param topList 顶层声明列表的编号。
 */
void M_SetProgram(int topList)
{
    chali_macro::sourceFragments.finishParsing();
    g_top.clear();
    for (const int topId : g_nodes[static_cast<std::size_t>(topList)].kids)
    {
        const MNode &top = g_nodes[static_cast<std::size_t>(topId)];
        if (top.kind == MNode::AbiBlock)
            g_top.insert(g_top.end(), top.kids.begin(), top.kids.end());
        else
            g_top.push_back(topId);
    }
}
/**
 * @brief 设置后续声明项继承的待定类型。
 * @param typeId 类型池中的类型编号。
 */
void M_SetPending(int typeId) { g_globalType = typeId; }
/**
 * @brief 取得当前声明项继承的待定类型编号。
 * @retval 类型池中对应类型的编号。
 */
int M_PendingType() { return g_globalType; }
/**
 * @brief 为声明列表中的各项补充类型与常量限定。
 * @param declaratorList 待补充类型的变量声明列表编号。
 * @param typeId 类型池中的类型编号。
 * @param constFlag 是否使用只读声明限定。
 */
void M_SetDeclType(int declaratorList, int typeId, int constFlag)
{
    if (declaratorList < 0) return;
    for (const int child : g_nodes[static_cast<std::size_t>(declaratorList)].kids)
    {
        if (child < 0) continue;
        g_nodes[static_cast<std::size_t>(child)].type = typeId;
        g_nodes[static_cast<std::size_t>(child)].type2 = constFlag; /* 这个声明不可变 */
    }
}
/**
 * @brief 设置仅检查模式。
 * @param enabled 是否禁用产物与执行。
 * @return 无。
 */
void M_SetCheckOnly(bool enabled) { g_checkOnly = enabled; }
/**
 * @brief 配置动态库编译所用的入口与生命周期策略。
 * @param enabled 为 true 时允许没有 main 并生成库生命周期函数。
 */
void M_SetSharedLibrary(bool enabled) { g_sharedLibrary = enabled; }
/**
 * @brief 控制是否仅准备 ABI 调用桥而不运行程序。
 * @param enabled 为 true 时跳过 VM 入口。
 */
void M_SetAbiPrepareOnly(bool enabled) { g_abiPrepareOnly = enabled; }
/**
 * @brief 设置本次编译使用的 ABI 桥位置与缓存选项。
 * @param cache 缓存目录，空值沿用环境默认。
 * @param bridge 预编译桥路径，空值沿用环境默认。
 * @param output 保存桥的路径，空值不显式导出。
 * @param noCache 是否禁用缓存。
 */
void M_SetAbiOptions(const Memory::String &cache, const Memory::String &bridge, const Memory::String &output, bool noCache)
{
    g_abiCacheOption = cache;
    g_abiBridgeOption = bridge;
    g_abiWriteOption = output;
    g_abiNoCache = noCache;
}

/**
 * @brief 设置单次编译开关，检查实际链接的 VM 能力。
 * @param enabled 是否开启。
 * @return 配置有效时返回 true。
 */
bool M_SetReflectionEnabled(bool enabled)
{
    g_reflectionEnabled = false;
    if (!enabled) return true;
#if CHALI_WITH_REFLECTION
    if ((stackvm::supportedProgramFeatures() & stackvm::kFeatureReflection) == 0) return false;
    g_reflectionEnabled = true;
    return true;
#else
    return false;
#endif
}
/**
 * @brief 获取首条语义错误的预处理行号。
 * @return 一基行号。
 */
std::size_t M_LastErrorLine() { return g_errorLine; }
/**
 * @brief 获取注解语义错误列。
 * @return 一基列，未知为零。
 */
std::size_t M_LastErrorColumn() { return g_errorColumn; }
/**
 * @brief 取得最近一次编译错误的文本。
 * @retval 最近一次编译错误的零结尾文本。
 */
const char *M_LastError() { return g_error.c_str(); }
/**
 * @brief 获取全部诊断，保留单条运行错误追加的调用栈。
 * @return 本次编译的诊断列表。
 */
const Memory::Vector<MDiagnostic> &M_Diagnostics()
{
    if (g_diagnostics.size() == 1u && !g_error.empty())
    {
        g_diagnostics.front().message = g_error;
        g_diagnostics.front().line = g_errorLine;
        g_diagnostics.front().column = g_errorColumn;
    }
    return g_diagnostics;
}

/**
 * @brief 读取编译期警告；警告不改变编译返回状态。
 * @return 本次编译保存的警告列表。
 */
const Memory::Vector<MDiagnostic> &M_Warnings() { return g_warnings; }

/**
 * @brief 设置源码文件映射与缺省文件名。
 * @param files 源码位置对应的文件名称表。
 * @param fallback 无法取得目标值时使用的默认值。
 */
void M_SetSourceFiles(const Memory::Vector<Memory::String> &files, const Memory::String &fallback)
{
    g_sourceFiles = files;
    g_sourceFallback = fallback;
    g_stringSources.clear();
}

/**
 * @brief 将词法值转换为语义值并更新源码位置。
 * @param tokenType 当前词法单元的类型编号。
 * @param number 当前词法单元的数值。
 * @param pName 词法单元对应的名称文本。
 * @param text 待处理的文本。
 * @param line 源码行号。
 * @param column 源码列号。
 * @retval 词法单元对应的语义值或名称池编号。
 */
int M_TokenValue(int tokenType, double number, const char *pName, Memory::String text,
                 std::size_t line, std::size_t column)
{
    if (g_macroFragmentFirstLine)
    {
        if (line == 2) column += g_macroFragmentFirstColumn - 1;
        line = line >= 2 ? g_macroFragmentFirstLine + line - 2 : g_macroFragmentFirstLine;
    }
    g_reductionLine = line;
    g_reductionColumn = column;
    if (tokenType == TOKEN_AT || tokenType == TOKEN_ANNOTATION)
    {
        const int id = M_New(MNode::AnnotationMarker);
        g_nodes[id].sourceColumn = column;
        return id;
    }
    /* 字符串：去引号后进名字池（M_Str 按池下标取）；标识符：直接进名字池；
     * 数字 / 浮点：直接发字面量节点（**不能**把原文当名字登记，否则 `42` 会变成名字下标）。
     * 浮点必须看词素原文：Token::value 是 int32，`2.0` 在里面就是 0。 */
    if (tokenType == TOKEN_STRING)
    {
        if (!text.empty() && text.front() == '"') text.erase(text.begin());
        if (!text.empty() && text.back() == '"') text.pop_back();
        g_names.push_back(text);
        const int id = static_cast<int>(g_names.size()) - 1;
        g_stringSources[id] = line > 0 && line <= g_sourceFiles.size() && !g_sourceFiles[line - 1].empty() ? g_sourceFiles[line - 1] : g_sourceFallback;
        // 宏标签用于诊断，不作为 ABI 相对路径解析的文件名。
        const auto macroTag = g_stringSources[id].rfind(" [宏 ");
        if (macroTag != Memory::String::npos && g_stringSources[id].back() == ']')
            g_stringSources[id].erase(macroTag);
        return id;
    }
    if (tokenType == TOKEN_NUMBER)
    {
        if (!text.empty())
        {
            const char *begin = text.c_str();
            char *end = nullptr;
            errno = 0;
            const long long parsed = std::strtoll(begin, &end, 10);
            // 超出 int64 时 strtoll 静默返回饱和值，源码与结果会不符；这里必须报错。
            // 需要表示更大的正整数时使用 u 后缀，由 ULIT 分支按 64 位位模式保留。
            if (errno == ERANGE || end != begin + text.size())
            {
                /* 唯一例外是 `-9223372036854775808`：幅值 2^63 本身放不进 int64，
                 * 取负后正好是 int64 最小值。这里先按"待取负"登记这个幅值，
                 * 由一元负号在 M_Un 里折成真正的负数；没被负号消费的字面量
                 * 仍在 M_Compile 开头按越界报错。 */
                char *magnitudeEnd = nullptr;
                errno = 0;
                const unsigned long long magnitude = std::strtoull(begin, &magnitudeEnd, 10);
                const bool digitsOnly = magnitudeEnd == begin + text.size() && text.find_first_not_of("0123456789") == Memory::String::npos;
                if (!digitsOnly || errno == ERANGE || magnitude != static_cast<unsigned long long>(LLONG_MAX) + 1ull)
                {
                    PushParseDiagnostic("十进制整数超出 int64 范围：" + text);
                    return M_Int(0);
                }
                const int id = M_Int(LLONG_MAX);
                g_nodes[static_cast<std::size_t>(id)].s = text;
                g_nodes[static_cast<std::size_t>(id)].intMinMagnitude = true;
                return id;
            }
            const int id = M_Int(parsed);
            g_nodes[id].s = text;
            return id;
        }
        return M_Int(static_cast<long long>(number));
    }
    if (tokenType == TOKEN_ULIT || tokenType == TOKEN_HEXLIT || tokenType == TOKEN_BINLIT)
    {
        const bool unsignedSuffix = !text.empty() && text.back() == 'u';
        const std::size_t end = text.size() - (unsignedSuffix ? 1u : 0u);
        const unsigned base = tokenType == TOKEN_ULIT ? 10u : tokenType == TOKEN_HEXLIT ? 16u :
                                                                                          2u;
        std::size_t start = base == 10u ? 0u : base == 16u ? 2u :
                                                             1u;
        if (base == 2u && start < text.size() && text[start] == '_') ++start;
        std::uint64_t value = 0;
        bool valid = start < end;
        bool previousDigit = false;
        for (std::size_t i = start; i < end; ++i)
        {
            const char ch = text[i];
            if (ch == '_')
            {
                if (!previousDigit || i + 1u == end) valid = false;
                previousDigit = false;
                continue;
            }
            const unsigned digit = ch >= '0' && ch <= '9' ? static_cast<unsigned>(ch - '0') :
                                   ch >= 'a' && ch <= 'f' ? static_cast<unsigned>(ch - 'a' + 10) :
                                   ch >= 'A' && ch <= 'F' ? static_cast<unsigned>(ch - 'A' + 10) :
                                                            base;
            if (digit >= base)
            {
                valid = false;
                break;
            }
            previousDigit = true;
            if (value > (std::numeric_limits<std::uint64_t>::max() - digit) / base)
            {
                PushParseDiagnostic("整数位数超出 64 位：" + text);
                return M_Int(0);
            }
            value = value * base + digit;
        }
        if (!valid)
        {
            PushParseDiagnostic("整数进制字面量无效：" + text);
            return M_Int(0);
        }
        long long bits = 0;
        static_assert(sizeof(bits) == sizeof(value), "整数槽必须是 64 位");
        std::memcpy(&bits, &value, sizeof(bits));
        const int id = M_Int(bits);
        g_nodes[id].s = text;
        g_nodes[id].unsignedLiteral = unsignedSuffix;
        return id;
    }
    if (tokenType == TOKEN_FLIT)
    {
        if (!text.empty())
        {
            const bool single = text.back() == 'f';
            const double value = single ? static_cast<double>(std::strtof(text.c_str(), nullptr)) : std::strtod(text.c_str(), nullptr);
            // 溢出成 inf 后会被当成标识符参与解析并给出无关诊断；这里直接拒绝。
            if (!std::isfinite(value))
            {
                PushParseDiagnostic("浮点字面量超出可表示范围：" + text);
                return M_Float(0);
            }
            const int id = M_Float(value);
            if (single) g_nodes[id].type = M_TypeFloatBits(32);
            return id;
        }
        return M_Float(number);
    }
    if (!text.empty())
    {
        auto rawMacro = static_cast<std::size_t>(-1);
        if (tokenType == TOKEN_MACRO_NAME && text.find("@!") != Memory::String::npos)
        {
            rawMacro = chali_macro::sourceFragments.resolveMarker(std::string_view(text.data(), text.size()));
            text.erase(text.find("@!"));
        }
        const int id = M_Name(text.c_str());
        if (tokenType == TOKEN_MACRO_NAME)
        {
            g_rawMacroNames.erase(id);
            const auto raw = rawMacro != static_cast<std::size_t>(-1) ? rawMacro : chali_macro::sourceFragments.find(line, column);
            if (raw != static_cast<std::size_t>(-1) && chali_macro::sourceFragments.at(raw).name == std::string(text.data() + 1, text.size() - 1))
                g_rawMacroNames[id] = raw;
        }
        return id;
    }
    if (pName != nullptr && pName[0] != '\0') return M_Name(pName);
    return M_Int(static_cast<long long>(number));
}

#include "semantics/annotation_ast.cpp"
#include "semantics/macro_ast.cpp"

/* ============================================================
 * 降级：C 语法树 → ast::Program → 四元式 / .data → stackvm 执行
 *
 * 分两遍：
 *   ① 登记：struct 的字段 / 方法 / 运算符重载、函数签名、全局变量；
 *   ② 降级：函数体、方法体、全局初始化 → ast 的表达式树。
 *
 * 前端不另造 IR：自己的 C 语法树直接映射到 ast，四元式与 `.data` 由它落盘。
 * ============================================================ */
namespace
{
#include "semantics/symbol_table.cpp"
#include "semantics/type_ids.cpp"
#include "semantics/type_ids_shared.cpp"
#include "semantics/lifetime.cpp"
#include "semantics/aggregate_ownership.cpp"
#include "semantics/scope.cpp"
#include "semantics/types.cpp"
#include "semantics/associated_types.cpp"
#include "semantics/interfaces.cpp"
#include "semantics/formatted_output.cpp"
#include "semantics/builtins.cpp"
#include "semantics/generic_inference.cpp"
#include "semantics/editor_analysis.cpp"
#include "semantics/constants.cpp"
#include "semantics/lowering.cpp"
#include "semantics/interface_dispatch.cpp"
#include "semantics/value_text.cpp"
#include "semantics/boxing.cpp"
#include "semantics/annotations.cpp"
#include "semantics/symbol_registration.cpp"
#include "semantics/program_lowering.cpp"
#include "semantics/artifacts.cpp"
#include "semantics/reflection_metadata.cpp"
#include "semantics/reflection_queries.cpp"
#include "semantics/reflection_calls.cpp"
#include "semantics/reflection_serialization.cpp"
#include "semantics/derive.cpp"
#include "semantics/macro_fragments.cpp"
#include "semantics/ast_generic_closures.cpp"
#include "semantics/macro_metadata.cpp"
#include "semantics/macro_controls.cpp"
#include "semantics/macro_patterns.cpp"
#include "semantics/macro_expansion.cpp"
}  // namespace

/**
 * @brief 从最终声明表收集源码显式公开的函数标签。
 * @return 保留重载和模块限定身份的有效导出标签。
 */
Memory::Vector<Memory::String> M_AotExports()
{
    Memory::Vector<Memory::String> labels;
    for (const auto &declaration : g_functionDecls)
        if (declaration.exported && !declaration.superseded && !declaration.external)
            labels.push_back(declaration.label);
    return labels;
}

/**
 * @brief 把攒下的生命周期契约违规交给统一诊断队列。
 * @return 无。
 */
void M_ReportLifetimeDiagnostics()
{
    ReportLifetimeDiagnostics();
}

/**
 * @brief 最近一次成功语义检查后的 AST，由编译器保留到下一次编译。
 */
static std::unique_ptr<ast::Program> g_aotSyntax;

/**
 * @brief 返回最近一次语义降级完成的 AST，不构造四元式。
 * @return 下次编译前有效的 AST 指针。
 */
const ast::Program *M_AotSyntax() { return g_aotSyntax.get(); }

/**
 * @brief 完成语义检查和所有权展开，保留供 LLVM 直接访问的 AST。
 * @param moduleName 仅用于显式兼容导出的模块名称。
 * @param outPrefix 显式兼容导出的文件前缀；正常 AOT 不使用它生成四元式。
 * @param run 兼容调用方的语义绑定开关，不在此处执行程序。
 * @param emitBinary 非零时额外生成独立 AST 库的兼容四元式产物。
 * @param inputText 已废弃，标准输入由原生程序读取。
 * @return 成功返回零，失败由 M_LastError 提供诊断。
 */
int M_Compile(const char *moduleName, const char *outPrefix, int run, int emitBinary,
              const char *inputText)
{
    g_aotSyntax.reset();
    ResetState();
    g_executeNow = run != 0;
    /* 解析期攒下的错（复合引用等）：ResetState 会清 g_error，所以在这里补报。 */
    if (!g_parseDiagnostics.empty())
    {
        for (const auto &item : g_parseDiagnostics) AddDiagnostic(item.message, item.line, item.column);
        g_parseDiagnostics.clear();
        RestoreFirstDiagnostic();
        return 1;
    }
    /* 幅值 2^63 的字面量只有被一元负号消费才成立；剩下的仍按越界报错。 */
    for (const auto &node : g_nodes)
    {
        if (node.kind != MNode::Int || !node.intMinMagnitude) continue;
        AddDiagnostic("十进制整数超出 int64 范围：" + node.s, node.sourceLine, node.sourceColumn);
        RestoreFirstDiagnostic();
        return 1;
    }
    if (g_top.empty())
    {
        Fail("没有解析到任何顶层声明");
        return 1;
    }
    /* 1. 降级成 ast 的程序。 */
    ast::Program program;
    if (!ExpandAstMacros())
    {
        RestoreFirstDiagnostic();
        return 1;
    }
    if (!LiftReachableAstLambdas()) return 1;
    if (!ExpandDerivedDeclarations()) return 1;
    if (!PrepareGenericSignatures()) return 1;
    auto parsedNodes = g_nodes;
    auto parsedTypes = g_types;
    auto parsedNames = g_names;
    auto parsedTop = g_top;
    bool reflectionQueriesReady = false;
    for (std::size_t pass = 0;; ++pass)
    {
        if (pass >= 256) return Fail("泛型实例化超过 256 轮，请检查递归类型展开"), 1;
        try
        {
            ResetState();
            g_inferringExpressions.clear();
            g_freshSink = nullptr;
            program = ast::Program{};
            // 第一轮只发现具体签名，不执行生成前的占位宿主；最终重放恢复全部绑定检查。
            g_executeNow = run != 0 && !(!reflectionQueriesReady && HasReflectionQueries());
            if (!RegisterTop())
            {
                RestoreFirstDiagnostic();
                return 1;
            }
            if (!ValidateDerivedFields()) return 1;
            const bool lowered = LowerProgram(program);
            // 独立借用检查阶段：语义仅记录事实，全部单态化完成后才执行规则。
            for (const auto &diagnostic : chali_borrow_checker::check(g_borrowFacts))
                AddDiagnostic(diagnostic.message, diagnostic.line);
            for (const auto &flow : g_borrowFlows)
                for (const auto &diagnostic : chali_borrow_checker::checkFlow(flow))
                    AddDiagnostic(diagnostic.message, diagnostic.line);
            M_ReportLifetimeDiagnostics();
            if (!lowered || !g_diagnostics.empty())
            {
                RestoreFirstDiagnostic();
                return 1;
            }
            g_executeNow = run != 0;
            if (!reflectionQueriesReady && HasReflectionQueries())
            {
                MReflectionBuilder table;
                if (g_reflectionEnabled && !table.build()) return 1;
                MReflectionQueries queries{ table };
                auto generated = queries.build();
                MReflectionCalls calls{ queries, table };
                calls.build();
                MReflectionSerialization serialization{ queries, table };
                serialization.build();
                generated = "module std::reflect;\n" + queries.helpers + queries.functions;
                if (!g_error.empty()) return 1;
                g_nodes = parsedNodes;
                g_types = parsedTypes;
                g_names = parsedNames;
                g_top = parsedTop;
                Memory::String error;
                if (!chali_generic::rewrite(generated, chali_generic::session.generics,
                                            chali_generic::session.implemented, chali_generic::session.made,
                                            chali_generic::session.pending, error)) return Fail(error), 1;
                Memory::String additional;
                if (!chali_generic::instantiate("Option", { "int64" }, additional, error)) return Fail(error), 1;
                chali::Engine parser;
                int value = 0;
                g_parseModule.clear();
                const auto firstBridgeNode = g_nodes.size();
                chali_generic::markContainerTypeArguments(generated);
                if (parser.runText(generated, &value) != chali::Status::Ok)
                {
                    const auto *error = parser.lastError();
                    const auto detail = g_error.empty() ? (error ? Memory::String(error->message().c_str()) : Memory::String()) : g_error;
                    if (const char *dump = std::getenv("CHALI_DUMP_REFLECTION"))
                    {
                        std::ofstream output(std::filesystem::u8path(dump), std::ios::binary);
                        output.write(generated.data(), static_cast<std::streamsize>(generated.size()));
                    }
                    return Fail("编译器生成的反射查询无法解析" + (detail.empty() ? Memory::String() : Memory::String(": ") + detail)), 1;
                }
                for (std::size_t i = firstBridgeNode; i < g_nodes.size(); ++i) g_nodes[i].reflectionBridge = true;
                auto generatedTop = g_top;
                if (!additional.empty())
                {
                    g_parseModule.clear();
                    if (parser.runText(additional, &value) != chali::Status::Ok)
                        return Fail("反射桥接的具体泛型声明无法解析"), 1;
                    generatedTop.insert(generatedTop.end(), g_top.begin(), g_top.end());
                }
                g_top = std::move(generatedTop);
                Memory::Vector<int> all;
                for (int id : parsedTop)
                    if (!(N(id).module == "std::reflect" && N(id).kind == MNode::ExternFuncDecl && queries.replaced.count(N(id).s))) all.push_back(id);
                all.insert(all.end(), g_top.begin(), g_top.end());
                g_top = std::move(all);
                if (!LiftReachableAstLambdas()) return 1;
                parsedNodes = g_nodes;
                parsedTypes = g_types;
                parsedNames = g_names;
                parsedTop = g_top;
                reflectionQueriesReady = true;
                continue;
            }
            break;
        } catch (const MMacroSemanticReplay &)
        {
            parsedNodes = g_nodes;
            parsedTypes = g_types;
            parsedNames = g_names;
            parsedTop = g_top;
        } catch (const MInstantiationRequest &request)
        {
            g_nodes = parsedNodes;
            g_types = parsedTypes;
            g_names = parsedNames;
            g_top = parsedTop;
            if (!LiftReachableAstLambdas()) return 1;
            Memory::String source, error;
            if (!chali_generic::instantiate(request.name, request.args, source, error)) return Fail(error), 1;
            if (source.empty()) return Fail("泛型实例化未产生新声明：" + request.name), 1;
            chali::Engine parser;
            int value = 0;
            g_parseModule.clear();
            if (parser.runText(source, &value) != chali::Status::Ok)
                return Fail("推导出的泛型声明无法解析：" + request.name), 1;
            if (!g_parseDiagnostics.empty())
            {
                for (const auto &item : g_parseDiagnostics) AddDiagnostic(item.message, item.line, item.column);
                g_parseDiagnostics.clear();
                RestoreFirstDiagnostic();
                return 1;
            }
            if (!ExpandAdditionalAstMacros()) return 1;
            // 自动实例化也可能带来普通闭包；重放前登记其函数，避免类型查询退回整数占位。
            if (!LiftReachableAstLambdas()) return 1;
            parsedTop.insert(parsedTop.end(), g_top.begin(), g_top.end());
            g_top = parsedTop;
            if (!ExpandDerivedDeclarations()) return 1;
            parsedTop = g_top;
            parsedNodes = g_nodes;
            parsedTypes = g_types;
            parsedNames = g_names;
        }
    }
    // 单态化及反射桥重放已结束，后续只读取全局最终 AST，不再保留整份解析快照。
    // 在四元式生成前归还这些副本，避免它们与生成结果争用同一个编译池。
    decltype(parsedNodes)().swap(parsedNodes);
    decltype(parsedTypes)().swap(parsedTypes);
    decltype(parsedNames)().swap(parsedNames);
    decltype(parsedTop)().swap(parsedTop);
    /* 降级期错误统一收集到诊断列表，这里必须再确认一次：
     * 少了它，出错的地方会带着占位节点一路生成下去，看起来"能跑但结果不对"。 */
    if (!g_diagnostics.empty())
    {
        RestoreFirstDiagnostic();
        return 1;
    }
    MReflectionBuilder reflection;
    if (g_reflectionEnabled && !reflection.build()) return 1;
    if (g_checkOnly) return 0;

    AddRuntimeHelpers(program);
    if (!FinalizeBootstrapCalls(program))
    {
        RestoreFirstDiagnostic();
        return 1;
    }

    g_aotSyntax = std::make_unique<ast::Program>(std::move(program));
    /* 仅显式请求兼容四元式产物时生成；正常编译直接把 AST 交给 LLVM 后端。 */
    if (!emitBinary) return 0;
    program = *g_aotSyntax;
    ast::CodegenResult result = ast::generate(program, moduleName);
    if (g_reflectionEnabled) result.quads = "# chali-features: reflection\n# chali-reflection: " + *mr::toHex(reflection.table) + "\n" + result.quads;
    if (!result.ok())
    {
        Fail("降级失败：" + JoinMessages(result.diagnostics));
        return 1;
    }
    Memory::String binaryPath;
    Memory::String layoutPath;
    stackvm::QuadFile file;
    if (!WriteArtifacts(outPrefix, result, &binaryPath, &layoutPath, &file)) return 1;
    (void)run;
    (void)emitBinary;
    (void)inputText;
    if (g_abiPrepareOnly)
    {
        chali_glue::Registry registry;
        return BindRuntimeAbi(&registry) ? 0 : 1;
    }
    return 0;
}

/**
 * @brief 设置程序运行时可用的内存预算。
 * @param bytes 内存区域或数据的字节数。
 */
void M_SetRuntimeMemory(std::size_t bytes) { g_runtimeMemory = bytes; }
/**
 * @brief 查询最近一次执行的运行时内存峰值。
 * @retval 以字节为单位的内存容量或占用量。
 */
std::size_t M_RuntimePeakBytes() { return g_runtimePeak; }

/**
 * @brief 设置程序执行使用的宿主原生函数注册表。
 * @param registry 接收函数绑定的宿主注册表。
 */
void M_SetNativeRegistry(const stackvm::NativeRegistry *registry) { g_nativeRegistry = registry; }

/**
 * @brief 设置本次程序执行是否启用线程调度。
 * @param enabled 是否启用对应功能。
 */
void M_SetThreadsEnabled(int enabled) { g_threadsEnabled = enabled != 0; }

/**
 * @brief 设置是否输出编译及执行调试信息。
 * @param enabled 是否启用对应功能。
 */
void M_SetDebugEnabled(int enabled) { g_debugEnabled = enabled != 0; }

/**
 * @brief 设置虚拟机允许执行的最大指令步数。
 * @param limit 索引、步数或资源使用的上限。
 */
void M_SetStepLimit(std::size_t limit) { g_stepLimit = limit; }

/**
 * @brief 设置是否从编译产物中裁剪调试信息。
 * @param enabled 是否启用对应功能。
 */


/**
 * @brief 设置传递给 ChaLi 程序的命令行参数。
 * @param argc 命令行参数数量。
 * @param argv 命令行参数指针数组。
 */
void M_SetProgramArguments(int argc, const char *const *argv)
{
    g_programArguments.clear();
    if (argc <= 0 || argv == nullptr) return;
    g_programArguments.reserve(static_cast<std::size_t>(argc));
    for (int index = 0; index < argc; ++index)
        g_programArguments.emplace_back(argv[index] == nullptr ? "" : argv[index]);
}

/* ============================================================
 * 标准库预编译单元：解析产物的落盘与装载
 *
 * 标准库核心的节点、类型池、标识符池、顶层表及解析游标在构建阶段序列化，
 * 运行编译器时由内嵌二进制恢复为竞技场前缀；用户导入和用户节点不进入该资产。
 *
 * 单元格式在此版本化；chali_precompiled 包装层验证核心内容、编译器版本身份和完整性。
 * 文件与原始行的重复映射在写出时归一，避免用户全文长度进入二进制。
 * ============================================================ */
namespace
{
constexpr std::uint64_t kStandardUnitMagic = 0x4d47535444494e54ull; /* "MGSTDINT" */
constexpr std::uint64_t kStandardUnitVersion = 3;

/**
 * @brief 一个类型的引用字段在单元里的原始下标。
 */
struct UnitTypeRefs
{
    Memory::Vector<std::uint64_t> arguments;   ///< 泛型实参类型。
    Memory::Vector<std::uint64_t> parameters;  ///< 函数指针形参类型。
    std::uint64_t elem = 0;                    ///< 元素类型 / 引用目标 / 函数返回类型。
    std::uint64_t elem2 = 0;                   ///< `Map<K,V>` 的 V 等第二个类型参数。
};

/**
 * @brief 写出一组节点编号（加一后写成无符号，`-1` 落为 0）。
 * @param out 输出流。
 * @param values 节点编号。
 */
void WriteUnitInts(std::ostream &out, const Memory::Vector<int> &values)
{
    chali_incremental::writeNumber(out, values.size());
    for (const int value : values)
        /* 按补码直写：`kids` / `annotations` 里 `-1` 是合法值（表示"没有"），
         * 不能靠"加一"编码，否则 -1 会落成一个看起来非法的 0。 */
        chali_incremental::writeNumber(out, static_cast<std::uint64_t>(static_cast<std::int64_t>(value)));
}
/**
 * @brief 读入一组节点编号。
 * @param in 输入流。
 * @param values 输出编号。
 * @retval 结构完整时返回 true。
 */
bool ReadUnitInts(std::istream &in, Memory::Vector<int> &values)
{
    std::uint64_t count = 0;
    if (!chali_incremental::readNumber(in, count) || count > (1ull << 26)) return false;
    values.resize(static_cast<std::size_t>(count));
    for (auto &value : values)
    {
        std::uint64_t stored = 0;
        if (!chali_incremental::readNumber(in, stored)) return false;
        value = static_cast<int>(static_cast<std::int64_t>(stored));
    }
    return true;
}
/**
 * @brief 写出一个双精度值（按位搬运，不做文本转换）。
 * @param out 输出流。
 * @param value 待写出的值。
 */
void WriteUnitDouble(std::ostream &out, double value)
{
    std::uint64_t bits = 0;
    std::memcpy(&bits, &value, sizeof(bits));
    chali_incremental::writeNumber(out, bits);
}
/**
 * @brief 读入一个双精度值。
 * @param in 输入流。
 * @param value 输出值。
 * @retval 读够八字节时返回 true。
 */
bool ReadUnitDouble(std::istream &in, double &value)
{
    std::uint64_t bits = 0;
    if (!chali_incremental::readNumber(in, bits)) return false;
    std::memcpy(&value, &bits, sizeof(value));
    return true;
}
/**
 * @brief 读入一个引用下标。
 * @param in 输入流。
 * @param value 输出下标；`0` 表示空引用，其余为「池下标 + 1」。
 * @retval 读够八字节时返回 true。
 */
bool ReadUnitRef(std::istream &in, std::uint64_t &value)
{
    return chali_incremental::readNumber(in, value);
}
/**
 * @brief 写出一个语法节点，将重复来源映射到同一文件行的首次出现位置。
 * @param out 输出流。
 * @param node 待写出的节点。
 * @param canonical 当前来源映射的规范行号表，消除用户输入长度带来的附加映射偏移。
 */
void WriteUnitNode(std::ostream &out, const MNode &node, const Memory::Vector<std::size_t> &canonical)
{
    if (node.compileMetadata) { out.setstate(std::ios::failbit); return; }
    chali_incremental::writeNumber(out, static_cast<std::uint64_t>(node.kind));
    chali_incremental::writeNumber(out, node.sourceLine < canonical.size() ? canonical[node.sourceLine] : node.sourceLine);
    chali_incremental::writeNumber(out, node.sourceColumn);
    chali_incremental::writeNumber(out, node.syntaxContext);
    chali_incremental::writeNumber(out, node.bindingIdentity);
    chali_incremental::writeNumber(out, node.nameParameter);
    chali_incremental::writeNumber(out, node.bindingContext);
    chali_incremental::writeNumber(out, node.expansionLine < canonical.size() ? canonical[node.expansionLine] : node.expansionLine);
    chali_incremental::writeNumber(out, node.expansionColumn);
    chali_incremental::writeString(out, node.expansionModule);
    chali_incremental::writeNumber(out, static_cast<std::uint64_t>(static_cast<std::int64_t>(node.macroExpansion)));
    WriteUnitInts(out, node.annotations);
    chali_incremental::writeStrings(out, node.derives);
    chali_incremental::writeStrings(out, node.noReplace);
    chali_incremental::writeStrings(out, node.noRight);
    chali_incremental::writeNumber(out, node.derivesExpanded ? 1u : 0u);
    chali_incremental::writeNumber(out, node.annotationValues.size());
    for (const auto &value : node.annotationValues)
    {
        chali_incremental::writeNumber(out, static_cast<std::uint64_t>(static_cast<std::int64_t>(value.type)));
        chali_incremental::writeNumber(out, static_cast<std::uint64_t>(value.integer));
        WriteUnitDouble(out, value.floating);
        chali_incremental::writeString(out, value.text);
    }
    chali_incremental::writeString(out, node.s);
    chali_incremental::writeString(out, node.s2);
    chali_incremental::writeString(out, node.abiLibrary);
    chali_incremental::writeString(out, node.abiSourceFile);
    chali_incremental::writeString(out, node.abiLength);
    chali_incremental::writeString(out, node.abiRelease);
    chali_incremental::writeString(out, node.abiConvention);
    chali_incremental::writeString(out, node.exportSymbol);
    chali_incremental::writeNumber(out, node.abiPacked ? 1u : 0u);
    chali_incremental::writeString(out, node.module);
    std::uint64_t flags = 0;
    if (node.moduleLocal) flags |= 1u << 0;
    if (node.reflectionBridge) flags |= 1u << 1;
    if (node.generatedDeclaration) flags |= 1u << 2;
    if (node.nonCopyable) flags |= 1u << 3;
    if (node.unsignedLiteral) flags |= 1u << 4;
    if (node.intMinMagnitude) flags |= 1u << 5;
    if (node.checkedExpression) flags |= 1u << 6;
    if (node.returnExpression) flags |= 1u << 7;
    if (node.implicitExitMove) flags |= 1u << 8;
    if (node.exported) flags |= 1u << 9;
    if (node.unsafeRequired) flags |= 1u << 10;
    if (node.unsafeScope) flags |= 1u << 11;
    if (node.formattedOutputBridge) flags |= 1u << 12;
    chali_incremental::writeNumber(out, flags);
    chali_incremental::writeNumber(out, static_cast<std::uint64_t>(node.i));
    WriteUnitDouble(out, node.d);
    chali_incremental::writeNumber(out, static_cast<std::uint64_t>(static_cast<std::int64_t>(node.guard)));
    chali_incremental::writeNumber(out, static_cast<std::uint64_t>(static_cast<std::int64_t>(node.type)));
    chali_incremental::writeNumber(out, static_cast<std::uint64_t>(static_cast<std::int64_t>(node.type2)));
    WriteUnitInts(out, node.kids);
}
/**
 * @brief 读入一个语法节点。
 * @param in 输入流。
 * @param node 输出节点。
 * @retval 结构完整时返回 true。
 */
bool ReadUnitNode(std::istream &in, MNode &node)
{
    std::uint64_t kind = 0, flags = 0, count = 0;
    if (!chali_incremental::readNumber(in, kind) || kind > static_cast<std::uint64_t>(MNode::MacroPatternFragment)) return false;
    node = MNode{};
    node.kind = static_cast<MNode::Kind>(kind);
    if (!chali_incremental::readNumber(in, node.sourceLine)) return false;
    if (!chali_incremental::readNumber(in, node.sourceColumn)) return false;
    std::uint64_t context = 0, identity = 0, parameter = 0, binding = 0;
    if (!chali_incremental::readNumber(in, context)) return false;
    if (!chali_incremental::readNumber(in, identity)) return false;
    if (!chali_incremental::readNumber(in, parameter)) return false;
    if (!chali_incremental::readNumber(in, binding)) return false;
    node.syntaxContext = static_cast<std::uint32_t>(context);
    node.bindingIdentity = static_cast<std::uint32_t>(identity);
    node.nameParameter = static_cast<std::uint32_t>(parameter);
    node.bindingContext = static_cast<std::uint32_t>(binding);
    if (!chali_incremental::readNumber(in, node.expansionLine)) return false;
    if (!chali_incremental::readNumber(in, node.expansionColumn)) return false;
    if (!chali_incremental::readString(in, node.expansionModule)) return false;
    std::uint64_t frame = 0;
    if (!chali_incremental::readNumber(in, frame)) return false;
    node.macroExpansion = static_cast<int>(static_cast<std::int64_t>(frame));
    if (!ReadUnitInts(in, node.annotations)) return false;
    if (!chali_incremental::readStrings(in, node.derives)) return false;
    if (!chali_incremental::readStrings(in, node.noReplace)) return false;
    if (!chali_incremental::readStrings(in, node.noRight)) return false;
    std::uint64_t derived = 0;
    if (!chali_incremental::readNumber(in, derived)) return false;
    node.derivesExpanded = derived != 0;
    if (!chali_incremental::readNumber(in, count) || count > (1ull << 20)) return false;
    node.annotationValues.resize(static_cast<std::size_t>(count));
    for (auto &value : node.annotationValues)
    {
        std::uint64_t type = 0, integer = 0;
        if (!chali_incremental::readNumber(in, type)) return false;
        if (!chali_incremental::readNumber(in, integer)) return false;
        value.type = static_cast<int>(static_cast<std::int64_t>(type));
        value.integer = static_cast<long long>(integer);
        if (!ReadUnitDouble(in, value.floating)) return false;
        if (!chali_incremental::readString(in, value.text)) return false;
    }
    if (!chali_incremental::readString(in, node.s)) return false;
    if (!chali_incremental::readString(in, node.s2)) return false;
    if (!chali_incremental::readString(in, node.abiLibrary)) return false;
    if (!chali_incremental::readString(in, node.abiSourceFile)) return false;
    if (!chali_incremental::readString(in, node.abiLength)) return false;
    if (!chali_incremental::readString(in, node.abiRelease)) return false;
    if (!chali_incremental::readString(in, node.abiConvention)) return false;
    if (!chali_incremental::readString(in, node.exportSymbol)) return false;
    std::uint64_t packed = 0;
    if (!chali_incremental::readNumber(in, packed)) return false;
    node.abiPacked = packed != 0;
    if (!chali_incremental::readString(in, node.module)) return false;
    if (!chali_incremental::readNumber(in, flags)) return false;
    node.moduleLocal = (flags & (1u << 0)) != 0;
    node.reflectionBridge = (flags & (1u << 1)) != 0;
    node.generatedDeclaration = (flags & (1u << 2)) != 0;
    node.nonCopyable = (flags & (1u << 3)) != 0;
    node.unsignedLiteral = (flags & (1u << 4)) != 0;
    node.intMinMagnitude = (flags & (1u << 5)) != 0;
    node.checkedExpression = (flags & (1u << 6)) != 0;
    node.returnExpression = (flags & (1u << 7)) != 0;
    node.implicitExitMove = (flags & (1u << 8)) != 0;
    node.exported = (flags & (1u << 9)) != 0;
    node.unsafeRequired = (flags & (1u << 10)) != 0;
    node.unsafeScope = (flags & (1u << 11)) != 0;
    node.formattedOutputBridge = (flags & (1u << 12)) != 0;
    std::uint64_t integerValue = 0, guard = 0, type = 0, type2 = 0;
    if (!chali_incremental::readNumber(in, integerValue)) return false;
    node.i = static_cast<long long>(integerValue);
    if (!ReadUnitDouble(in, node.d)) return false;
    if (!chali_incremental::readNumber(in, guard)) return false;
    node.guard = static_cast<int>(static_cast<std::int64_t>(guard));
    if (!chali_incremental::readNumber(in, type)) return false;
    node.type = static_cast<int>(static_cast<std::int64_t>(type));
    if (!chali_incremental::readNumber(in, type2)) return false;
    node.type2 = static_cast<int>(static_cast<std::int64_t>(type2));
    return ReadUnitInts(in, node.kids);
}
/**
 * @brief 写出一个类型的标量字段。
 * @param out 输出流。
 * @param type 待写出的类型。
 */
void WriteUnitTypeBody(std::ostream &out, const MType &type)
{
    chali_incremental::writeNumber(out, static_cast<std::uint64_t>(type.kind));
    chali_incremental::writeString(out, type.name);
    chali_incremental::writeString(out, type.lookupModule);
    chali_incremental::writeNumber(out, type.unresolvedName ? 1u : 0u);
    chali_incremental::writeString(out, type.genericBase);
    chali_incremental::writeString(out, type.genericMember);
    chali_incremental::writeNumber(out, type.macroBinding);
    chali_incremental::writeNumber(out, static_cast<std::uint64_t>(static_cast<std::int64_t>(type.bits)));
    chali_incremental::writeNumber(out, type.isUnsigned ? 1u : 0u);
    WriteUnitInts(out, type.dims);
    chali_incremental::writeNumber(out, type.readOnly ? 1u : 0u);
    chali_incremental::writeNumber(out, type.variadic ? 1u : 0u);
}
/**
 * @brief 写出一个类型的引用字段（写成「池下标 + 1」，空引用写 0）。
 * @param out 输出流。
 * @param type 待写出的类型。
 * @param index 类型指针到池下标的映射。
 */
void WriteUnitTypeRefs(std::ostream &out, const MType &type, const Memory::Map<const MType *, std::size_t> &index)
{
    const auto write = [&](const std::shared_ptr<MType> &target) {
        if (!target) return chali_incremental::writeNumber(out, std::uint64_t{ 0 });
        const auto found = index.find(target.get());
        chali_incremental::writeNumber(out, found == index.end() ? std::uint64_t{ 0 } : static_cast<std::uint64_t>(found->second) + 1);
    };
    chali_incremental::writeNumber(out, type.genericArguments.size());
    for (const auto &argument : type.genericArguments) write(argument);
    write(type.elem);
    chali_incremental::writeNumber(out, type.paramTypes.size());
    for (const auto &parameter : type.paramTypes) write(parameter);
    write(type.elem2);
}
/**
 * @brief 读入一个类型的标量字段与引用下标。
 * @param in 输入流。
 * @param type 输出类型对象（引用字段此时还是空的）。
 * @param refs 输出引用下标。
 * @retval 结构完整时返回 true。
 */
bool ReadUnitTypeBody(std::istream &in, std::shared_ptr<MType> &type, UnitTypeRefs &refs)
{
    std::uint64_t kind = 0, flag = 0, bits = 0, count = 0;
    if (!chali_incremental::readNumber(in, kind) || kind > static_cast<std::uint64_t>(MType::Obj)) return false;
    type = std::make_shared<MType>();
    type->kind = static_cast<MType::Kind>(kind);
    if (!chali_incremental::readString(in, type->name)) return false;
    if (!chali_incremental::readString(in, type->lookupModule)) return false;
    if (!chali_incremental::readNumber(in, flag)) return false;
    type->unresolvedName = flag != 0;
    if (!chali_incremental::readString(in, type->genericBase)) return false;
    if (!chali_incremental::readString(in, type->genericMember)) return false;
    std::uint64_t binding = 0;
    if (!chali_incremental::readNumber(in, binding)) return false;
    type->macroBinding = static_cast<std::uint32_t>(binding);
    if (!chali_incremental::readNumber(in, bits)) return false;
    type->bits = static_cast<int>(static_cast<std::int64_t>(bits));
    if (!chali_incremental::readNumber(in, flag)) return false;
    type->isUnsigned = flag != 0;
    if (!ReadUnitInts(in, type->dims)) return false;
    if (!chali_incremental::readNumber(in, flag)) return false;
    type->readOnly = flag != 0;
    if (!chali_incremental::readNumber(in, flag)) return false;
    type->variadic = flag != 0;
    if (!chali_incremental::readNumber(in, count) || count > (1ull << 20)) return false;
    refs.arguments.resize(static_cast<std::size_t>(count));
    for (auto &value : refs.arguments)
        if (!ReadUnitRef(in, value)) return false;
    if (!ReadUnitRef(in, refs.elem)) return false;
    std::uint64_t parameterCount = 0;
    if (!chali_incremental::readNumber(in, parameterCount) || parameterCount > (1ull << 20)) return false;
    refs.parameters.resize(static_cast<std::size_t>(parameterCount));
    for (auto &value : refs.parameters)
        if (!ReadUnitRef(in, value)) return false;
    return ReadUnitRef(in, refs.elem2);
}
}  // namespace

Memory::Vector<int> M_TopLevelIds()
{
    return g_top;
}

void M_PrefixTopLevel(const Memory::Vector<int> &prefix)
{
    if (prefix.empty()) return;
    Memory::Vector<int> merged = prefix;
    merged.insert(merged.end(), g_top.begin(), g_top.end());
    g_top = std::move(merged);
}

std::size_t M_NodeCount()
{
    return g_nodes.size();
}

bool M_WriteStandardUnit(std::ostream &out, std::size_t nodeLimit)
{
    /* 解析期已经攒下诊断的标准库段不固化：固化下来等于把一次带错的解析结果留到以后。 */
    if (!g_parseDiagnostics.empty() || !g_error.empty()) return false;
    if (nodeLimit == 0 || nodeLimit > g_nodes.size()) return false;
    Memory::Map<const MType *, std::size_t> index;
    for (std::size_t i = 0; i < g_types.size(); ++i)
        if (g_types[i]) index[g_types[i].get()] = i;
    // 原始宏来源被附加在全文映射之后；按文件及原始行归一，避免把用户全文长度写进核心包。
    Memory::Map<std::pair<Memory::String, std::size_t>, std::size_t> firstOrigins;
    Memory::Vector<std::size_t> canonical(g_sourceFiles.size() + 1, 0);
    for (std::size_t i = 0; i < g_sourceFiles.size(); ++i)
    {
        const auto line = i < g_sourceLines.size() ? g_sourceLines[i] : i + 1;
        canonical[i + 1] = firstOrigins.emplace(std::make_pair(g_sourceFiles[i], line), i + 1).first->second;
    }
    chali_incremental::writeNumber(out, kStandardUnitMagic);
    chali_incremental::writeNumber(out, kStandardUnitVersion);
    chali_incremental::writeNumber(out, nodeLimit);
    for (std::size_t i = 0; i < nodeLimit; ++i) WriteUnitNode(out, g_nodes[i], canonical);
    chali_incremental::writeNumber(out, g_types.size());
    for (const auto &type : g_types)
    {
        WriteUnitTypeBody(out, type ? *type : MType{});
        WriteUnitTypeRefs(out, type ? *type : MType{}, index);
    }
    chali_incremental::writeStrings(out, g_names);
    WriteUnitInts(out, g_top);
    Memory::Vector<int> rawNames, stringNodes;
    Memory::Vector<std::size_t> rawFragments;
    Memory::Vector<Memory::String> stringValues;
    for (const auto &entry : g_rawMacroNames)
        if (static_cast<std::size_t>(entry.first) < nodeLimit)
        {
            rawNames.push_back(entry.first);
            rawFragments.push_back(entry.second);
        }
    for (const auto &entry : g_stringSources)
        if (static_cast<std::size_t>(entry.first) < nodeLimit)
        {
            stringNodes.push_back(entry.first);
            stringValues.push_back(entry.second);
        }
    WriteUnitInts(out, rawNames);
    chali_incremental::writeNumber(out, rawFragments.size());
    for (const auto value : rawFragments) chali_incremental::writeNumber(out, value);
    WriteUnitInts(out, stringNodes);
    chali_incremental::writeStrings(out, stringValues);
    chali_incremental::writeNumber(out, static_cast<std::uint64_t>(static_cast<std::int64_t>(g_globalType)));
    chali_incremental::writeNumber(out, g_reductionLine < canonical.size() ? canonical[g_reductionLine] : g_reductionLine);
    chali_incremental::writeNumber(out, g_reductionColumn);
    chali_incremental::writeNumber(out, g_macroFragmentFirstLine < canonical.size() ? canonical[g_macroFragmentFirstLine] : g_macroFragmentFirstLine);
    chali_incremental::writeNumber(out, g_macroFragmentFirstColumn);
    chali_incremental::writeString(out, g_parseModule);
    return true;
}

bool M_ReadStandardUnit(std::istream &in)
{
    std::uint64_t magic = 0, version = 0, nodeLimit = 0, typeCount = 0, globalType = 0;
    if (!chali_incremental::readNumber(in, magic) || magic != kStandardUnitMagic) return false;
    if (!chali_incremental::readNumber(in, version) || version != kStandardUnitVersion) return false;
    if (!chali_incremental::readNumber(in, nodeLimit) || nodeLimit == 0 || nodeLimit > (1ull << 26)) return false;
    Memory::Deque<MNode> nodes;
    nodes.resize(static_cast<std::size_t>(nodeLimit));
    for (auto &node : nodes)
        if (!ReadUnitNode(in, node)) return false;
    if (!chali_incremental::readNumber(in, typeCount) || typeCount > (1ull << 24)) return false;
    Memory::Vector<std::shared_ptr<MType>> types;
    Memory::Vector<UnitTypeRefs> references(static_cast<std::size_t>(typeCount));
    types.resize(static_cast<std::size_t>(typeCount));
    for (std::size_t i = 0; i < types.size(); ++i)
        if (!ReadUnitTypeBody(in, types[i], references[i])) return false;
    /* 第二趟接引用：先建好全部对象，再按「池下标 + 1」回填，共享与自指都不丢。 */
    const auto resolve = [&](std::uint64_t value) -> std::shared_ptr<MType> {
        if (value == 0 || value > types.size()) return nullptr;
        return types[static_cast<std::size_t>(value) - 1u];
    };
    for (std::size_t i = 0; i < types.size(); ++i)
    {
        for (const auto value : references[i].arguments) types[i]->genericArguments.push_back(resolve(value));
        types[i]->elem = resolve(references[i].elem);
        for (const auto value : references[i].parameters) types[i]->paramTypes.push_back(resolve(value));
        types[i]->elem2 = resolve(references[i].elem2);
    }
    Memory::Vector<Memory::String> names;
    if (!chali_incremental::readStrings(in, names)) return false;
    Memory::Vector<int> top;
    if (!ReadUnitInts(in, top)) return false;
    Memory::Vector<int> rawNames, stringNodes;
    if (!ReadUnitInts(in, rawNames)) return false;
    std::uint64_t fragmentCount = 0;
    if (!chali_incremental::readNumber(in, fragmentCount) || fragmentCount != rawNames.size()) return false;
    Memory::Vector<std::size_t> rawFragments(fragmentCount);
    for (std::size_t i = 0; i < rawFragments.size(); ++i)
    {
        std::uint64_t value = 0;
        if (!chali_incremental::readNumber(in, value)) return false;
        rawFragments[i] = static_cast<std::size_t>(value);
    }
    if (!ReadUnitInts(in, stringNodes)) return false;
    Memory::Vector<Memory::String> stringValues;
    if (!chali_incremental::readStrings(in, stringValues) || stringValues.size() != stringNodes.size()) return false;
    if (!chali_incremental::readNumber(in, globalType)) return false;
    std::size_t reductionLine = 0, reductionColumn = 0, fragmentLine = 0, fragmentColumn = 0;
    if (!chali_incremental::readNumber(in, reductionLine)) return false;
    if (!chali_incremental::readNumber(in, reductionColumn)) return false;
    if (!chali_incremental::readNumber(in, fragmentLine)) return false;
    if (!chali_incremental::readNumber(in, fragmentColumn)) return false;
    Memory::String parseModule;
    if (!chali_incremental::readString(in, parseModule)) return false;
    /* 装载前先自检：只挡明显离谱的编号（结构错位才会读出天文数字）。
     * **不能**把"暂时越界"当单元损坏：标准库段解析里存在回滚（`nodes_.resize`）收回节点后
     * 没清掉的旧子节点编号，它当下确实超出竞技场末尾 —— 但这与一次性解析整段源码时的
     * 情形完全一致（等用户段把节点补上来，编号又落回范围内）。挡下它等于改了行为。 */
    for (const auto &node : nodes)
        for (const int kid : node.kids)
            if (kid >= (1 << 26)) return false;
    for (const int id : top)
        if (id < 0 || static_cast<std::size_t>(id) >= nodeLimit) return false;
    g_nodes = std::move(nodes);
    g_types = std::move(types);
    g_names = std::move(names);
    g_top = std::move(top);
    g_rawMacroNames.clear();
    for (std::size_t i = 0; i < rawNames.size(); ++i) g_rawMacroNames[rawNames[i]] = rawFragments[i];
    g_stringSources.clear();
    for (std::size_t i = 0; i < stringNodes.size(); ++i) g_stringSources[stringNodes[i]] = stringValues[i];
    g_globalType = static_cast<int>(static_cast<std::int64_t>(globalType));
    g_reductionLine = reductionLine;
    g_reductionColumn = reductionColumn;
    g_macroFragmentFirstLine = fragmentLine;
    g_macroFragmentFirstColumn = fragmentColumn;
    g_parseModule = parseModule;
    return true;
}

void M_ShiftNodeLines(std::size_t from, std::size_t lines)
{
    if (lines == 0 || from >= g_nodes.size()) return;
    for (std::size_t i = from; i < g_nodes.size(); ++i)
    {
        g_nodes[i].sourceLine += lines;
        if (g_nodes[i].expansionLine != 0) g_nodes[i].expansionLine += lines;
    }
}

std::size_t M_ParseDiagnosticCount() { return g_parseDiagnostics.size(); }

void M_ShiftParseDiagnostics(std::size_t from, std::size_t lines)
{
    if (lines == 0) return;
    for (std::size_t i = from; i < g_parseDiagnostics.size(); ++i)
        if (g_parseDiagnostics[i].line != 0) g_parseDiagnostics[i].line += lines;
    /* 解析期 `Fail` 落下的行号同样是按用户段重新数出来的（标准库段解析失败会提前退出，
     * 所以走到这里时 `g_error` 只可能是用户段留下的），一起平移。 */
    if (g_errorLine != 0) g_errorLine += lines;
}
