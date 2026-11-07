/**
 * @file chali_aot_debug.hpp
 * @brief 为原生函数生成独立的 DWARF 元数据。
 */
#pragma once
#include <map>
#include <cstdint>
#include <string>
#include <vector>

namespace chali_build
{
/**
 * @brief 源变量的实际帧槽及值类别。
 */
struct DebugVariable
{
    std::string name, pointer;
    bool floating = false;
    std::size_t argument = 0;
    std::string typeName = {};       ///< 用户类型名；空时兼容旧帧槽描述。
    std::size_t bits = 64;           ///< DWARF 展示位宽，窄整数可能小于实际帧槽。
    std::string nativeStorage = {};  ///< 实际 alloca 类型；为空的旧描述不允许调试器写入。
    std::string encoding = {};       ///< DWARF 基础值编码。
    std::size_t scope = 0, line = 0, column = 0;
    std::size_t block = 0;  ///< 声明生效的原生基本块，零表示函数序言。
    std::string file = {};
    bool readOnly = false;
    bool stringOwner = false; ///< 源码降级确认的可变局部 String 拥有关系，不由类型名推断。
    std::string storage = {};  ///< 非空表示原始槽只提供源码停点镜像，写入必须定位真实绑定。
    std::string dwarfName = {}; ///< 私有宏变量的独立调试名，避免遮蔽调用方绑定。
    std::int64_t macro = -1;    ///< 非负值表示只在该宏展开帧内可见。
    bool logicalAddress = false;  ///< 编译器确认槽值直接指向语言数据区或堆，不包括宿主对象编号。
    std::int64_t layout = -1;     ///< 版本化逻辑布局索引；负值表示没有可解码布局。
    std::string storageDwarf = {};  ///< 基础镜像真实逻辑地址的私有 DWARF 名，不能由用户表达式指定。
    std::size_t storageBytes = 0;   ///< 真实语言存储宽度；零表示不允许通过镜像修改。
    std::string availabilityDwarf = {}; ///< 引用绑定移动状态的私有观察槽，不是语言值。
    bool internal = false;          ///< 只生成私有 DWARF 声明，不作为源码变量登记。
};
/**
 * @brief AST 或兼容四元式节点的原始源码位置及词法范围。
 */
struct DebugLine
{
    std::string file;
    std::size_t line = 0;
    std::size_t column = 1, scope = 0;
    std::int64_t macro = -1;
    std::string definitionFile = {};
    std::size_t definitionLine = 0, definitionColumn = 0;
};
/**
 * @brief 与运行时无关的编译期宏来源，父编号严格小于子编号。
 */
struct DebugMacroFrame
{
    std::int64_t id = -1, parent = -1;
    std::string name;
    DebugLine call, invocation, definition;
};
/**
 * @brief 用户块及声明后区间的词法作用域；零号表示函数作用域。
 */
struct DebugScope
{
    std::size_t parent = 0;
    DebugLine origin;
};
/** @brief 源码捕获及其既有存储表示，只用于生成调试目录。 */
struct DebugCapture
{
    std::string name, layout;  ///< 捕获名字及编码布局。
    int mode = 0;              ///< 只读借用、可写借用、copy 或 move。
    bool address = false;      ///< 原捕获值是否为来源存储地址。
};
/**
 * @brief 使用原生帧槽描述源码行和变量，不依赖解释器。
 */
class NativeDebugInfo
{
    /** @brief 单个函数的源码名、参数和局部声明，仅在调试产物中保存。 */
    struct FunctionSource
    {
        std::string name, linkage;
        DebugLine origin;
        std::vector<DebugVariable> variables;
        std::map<std::size_t, DebugScope> scopes;
    };
    /** @brief 原生基本块地址与源码位置之间的只读对应。 */
    struct PositionSource
    {
        std::size_t function = 0, block = 0;
        DebugLine position;
    };
    std::vector<FunctionSource> functions_;
    std::vector<PositionSource> positions_;
    std::map<std::int64_t, DebugMacroFrame> macros_;
    std::vector<std::string> layouts_;              ///< 去重后的逻辑布局文本，索引写进变量描述。
    std::map<std::string, std::size_t> layoutIds_;  ///< 布局文本到公开索引。
    std::map<std::int64_t, std::int64_t> dynamicTypes_;  ///< 运行时实际类型号到只读布局索引。
    /** @brief 已登记捕获的布局索引及原捕获方式。 */
    struct CaptureSource
    {
        std::string name;
        std::int64_t layout = -1;
        int mode = 0;
        bool address = false;
    };
    /** @brief 一个可作为值使用的函数及其有序捕获描述。 */
    struct ClosureSource
    {
        std::string name;
        std::vector<CaptureSource> captures;
    };
    std::map<std::size_t, ClosureSource> closures_;  ///< 不可变函数标签数据地址到捕获描述。
    std::size_t next_ = 1000000, unit_ = 0, empty_ = 0, integer_ = 0, real_ = 0;
    std::string metadata_;
    std::map<std::string, std::size_t> files_;
    std::map<std::string, std::size_t> types_;
    bool optimized_ = false;  ///< 优化调试不通过块地址锚定函数，允许 LLVM 正常内联和删除。
    /**
     * @brief 追加一个 LLVM 元数据节点。
     * @param value 节点正文。
     * @return 唯一节点编号。
     */
    std::size_t node(const std::string &value);
    /**
     * @brief 查询或建立源码文件元数据。
     * @param path UTF-8 源码路径。
     * @return 文件节点编号。
     */
    std::size_t file(const std::string &path);

public:
    /**
     * @brief 选择开发或优化调试的元数据策略。
     * @param optimized 为真时登记优化状态且不锚定基本块地址。
     */
    explicit NativeDebugInfo(bool optimized = false);
    /**
     * @brief 登记同一源码展开的调用与定义，不加入语言执行状态。
     * @param frame 完整且有界的来源。
     * @return 无。
     */
    void recordMacro(DebugMacroFrame frame);
    /**
     * @brief 登记一份逻辑布局文本并返回稳定索引，相同文本只保存一次。
     * @param encoded 已编码的布局文本；空文本表示没有可解码布局。
     * @return 非负索引，空文本返回 -1。
     */
    std::int64_t registerLayout(const std::string &encoded);
    /** @brief 登记运行时装箱类型的静态布局，不生成目标执行代码。
     * @param type 本次程序的实际类型号。
     * @param encoded 不含用户方法的布局文本。
     * @return 无。 */
    void registerDynamicType(std::int64_t type, const std::string &encoded);
    /** @brief 登记函数值的源码名字和有序捕获，不生成执行入口。
     * @param address 数据区函数标签的逻辑地址。
     * @param name 用户可读函数名。
     * @param captures 原捕获顺序和只读布局。
     * @return 无。 */
    void registerClosure(std::size_t address, const std::string &name, const std::vector<DebugCapture> &captures);
    /**
     * @brief 给一个函数附加源码位置和实际变量地址。
     * @param definition 完整 LLVM 函数定义。
     * @param name 源码函数名。
     * @param linkage LLVM 符号名。
     * @param lines 指令下标到源位置的映射。
     * @param variables 用户变量与参数槽。
     * @return 带调试描述的函数。
     */
    std::string annotate(const std::string &definition, const std::string &name, const std::string &linkage,
                         const std::map<std::size_t, DebugLine> &lines, const std::vector<DebugVariable> &variables);
    /**
     * @brief 使用用户词法层级和声明位置标注真实机器帧。
     * @param definition 完整 LLVM 函数定义。
     * @param name 源函数名。
     * @param linkage LLVM 符号。
     * @param lines 指令对应源码位置及作用域。
     * @param variables 变量位置和真实存储编码。
     * @param scopes 用户词法作用域树。
     * @return 带 DWARF 的函数定义。
     */
    std::string annotate(const std::string &definition, const std::string &name, const std::string &linkage,
                         const std::map<std::size_t, DebugLine> &lines, const std::vector<DebugVariable> &variables,
                         const std::map<std::size_t, DebugScope> &scopes);
    /**
     * @brief 将调试编译单元和版本标记加入 LLVM 模块。
     * @param module 完整模块文本。
     * @param prefix 独立库的唯一目录前缀；完整程序使用 chali_debug。
     * @param libraries 已验证依赖的规范 JSON 数组。
     * @param unit 独立库的完整模块身份，完整程序为空。
     */
    void finish(std::string &module, const std::string &prefix = "chali_debug", const std::string &libraries = "[]", const std::string &unit = "");
};
}  // namespace chali_build
