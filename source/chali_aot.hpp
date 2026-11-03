/**
 * @file chali_aot.hpp
 * @brief 从语义检查后的 AST 直接生成 LLVM 代码并编译原生产物。
 */
#pragma once
#include "stackvm.hpp"
#include "chali_aot_target.hpp"

namespace ast { struct Program; }

namespace chali_aot
{

/**
 * @brief 一个由 `#abi("C", "库", "符号")` 声明、要链接到外部 C 的函数。
 */
struct ExternFunction
{
    Memory::String label;                           ///< 四元式里的调用标签。
    Memory::String nativeAlias;                     ///< 独立语言库保留的显式原生导出名。
    Memory::String symbol;                          ///< C 侧真正的符号名（`= "名字"`）。
    Memory::String library;                         ///< 动态库声明路径或静态链接资产路径；空值直接引用链接符号。
    Memory::String resultType = "long long";        ///< C 返回类型名。
    Memory::Vector<Memory::String> parameterTypes;  ///< C 形参类型名，按序。
    Memory::String abiConvention = "C";             ///< 已验证的外部调用约定。
    bool requiresMarshalling = false;               ///< 非标量签名必须通过 C 帧封送，不按槽位冒充原生布局。
    bool borrowedResult = false;                    ///< 已验证语言入口返回调用方借用的逻辑地址。
    bool hasLifetimeContract = false;               ///< 公开接口保留显式生命周期契约。
};

/** @brief 独立原生单元的数据映像、逐符号布局和可重定位基址名称。 */
struct UnitData
{
    Memory::String symbol;  ///< 含模块及数据身份摘要的 LLVM 基址符号。
    Memory::String bytes;   ///< 原始数据映像，不含宿主指针。
    Memory::String layout;  ///< 相对于单元基址的逐符号布局。
};

/**
 * @brief 判断外部声明是否需要按声明路径动态加载库。
 * @param function 外部函数及其库路径。
 * @return 非空且不是静态库或对象文件时返回真。
 */
bool usesDynamicAbiLibrary(const ExternFunction &function);
/**
 * @brief 判断是否显式声明最终链接阶段解析的 C 符号。
 * @param library ABI 库说明；@link 表示直接链接。
 * @return 是直接链接标记时为真。
 */
bool isDirectLinkLibrary(const Memory::String &library);

/**
 * @brief AOT 输出的形态与开关。
 */
struct Options
{
    bool checked = false;                            ///< 原生导出默认 unchecked，run 驱动显式开启检查。
    bool debug = false;                               ///< 保留原生源码与局部变量调试信息。
    bool debugOptimized = false;                      ///< debug 为真时使用 O3／LTO，不承诺变量或步骤完整保留。
    bool threadsEnabled = false;                     ///< 是否启用与 VM 同源的隔离线程服务。
    bool shared = false;                             ///< 产出动态库而不是可执行文件。
    Memory::String libraryModule;                    ///< 独立原生库单元的完整模块身份；空值沿用完整程序。
    bool libraryBitcode = false;                     ///< 独立库同时保留与原生对象同源的 bitcode。
    bool libraryHasTemplates = false;                ///< 生产器已核对模板载荷，允许没有普通函数的原生单元。
    Memory::String libraryImplementation = "native";  ///< 消费库的明确选择：native 或 bitcode，不作静默回退。
    Memory::String debugLibraries = "[]";             ///< 已验证库实现的调试目录身份，规范 JSON 数组，不重新读取包。
    Memory::Vector<UnitData> unitData;               ///< 从已验证库快照恢复的只初始化一次的数据段。
    Memory::String libraryLicense;                   ///< 生产包声明的许可证名称。
    Memory::Vector<Memory::String> libraryLicenseAssets;     ///< NAME=FILE 形式的许可证及署名输入。
    Memory::Vector<Memory::String> librarySystemLibraries;   ///< 最终消费者链接所需的系统库名。
    Memory::Vector<Memory::String> libraryRuntimeLibraries;  ///< 应用需交付或由目标系统提供的动态库名。
    bool emitLLVMOnly = false;                       ///< `--emit-llvm`：只写 LLVM 模块，不调用目标代码工具。
    bool emitAssemblyOnly = false;                   ///< 只生成目标机器汇编 .s，不生成对象或执行链接。
    bool emitObjectOnly = false;                     ///< 只生成目标机器的对象文件，不执行链接。
    bool emitBitcodeOnly = false;                    ///< 只生成目标的 LLVM bitcode，不执行链接。
    Memory::String target;                           ///< 目标三元组，空值使用实际宿主。
    Memory::String cpu;                              ///< 目标 CPU，空值使用目标的通用 CPU。
    Memory::String sysroot;                          ///< 完整交叉链接使用的目标 sysroot。
    Memory::String runtimeDirectory;                 ///< 目标 libchali_aot_host.a 所在目录。
    Memory::String allocator;                        ///< malloc 或 pool；空值默认系统 malloc。
    Memory::String compiler;                         ///< LLVM clang 驱动；空值优先使用 library/llvm 中的工具。
    Memory::String outputPrefix;                     ///< LLVM 模块、头文件和原生产物的公共前缀。
    Memory::Vector<Memory::String> exports;          ///< 显式导出给 C 的 ChaLi 函数名。
    Memory::Vector<Memory::String> linkLibraries;    ///< 额外的链接库（`abi` 里的库路径）。
    Memory::Vector<Memory::String> assemblySources;  ///< 显式加入最终链接的 .s 输入。
    Memory::Vector<Memory::String> assemblyIncludes;  ///< 汇编 .include 的搜索目录。
    Memory::Vector<Memory::String> linkObjects;      ///< 用户提供及本次编译生成的目标文件。
    Memory::Vector<Memory::String> linkBitcode;      ///< 由库快照核对目标、布局及 LLVM 版本后恢复的 bitcode。
    bool lto = true;                                ///< 最终链接保留 LLVM bitcode，默认启用全量 LTO。
    bool thinLto = false;                           ///< 启用 LTO 时改用带模块摘要的 ThinLTO，默认完整 LTO。
    std::size_t memoryBytes = 512u * 1024u * 1024u;  ///< 数据区之外的堆预算。
    std::size_t stepLimit = 0;                       ///< 运行安全点的逻辑步数上限，零表示不限。
};

/**
 * @brief 一次 AOT 编译的全部输入。
 */
struct Request
{
    const ast::Program *syntax = nullptr;      ///< 语义检查后的 AST；正常运行和 AOT 直接消费此树。
    const stackvm::QuadFile *quads = nullptr;   ///< 独立库调用方的兼容四元式输入；正常编译为空。
    const stackvm::Program *program = nullptr;  ///< 兼容组装结果；AST 路径不需要它。
    Memory::String dataBytes;                   ///< `.data.bin` 原始字节。
    Memory::String dataLayout;                  ///< 布局文本（每行 `名 offset=… size=…`）。
    Memory::Vector<ExternFunction> externs;     ///< `abi` 声明的外部 C 函数。
    Memory::String abiGlue;                     ///< 复杂 C ABI 使用的类型化宿主绑定源码。
    Memory::Vector<ExternFunction> abiExports; ///< 经类型校验的 C ABI 导出，与语言入口独立。
    Memory::String abiExportHeader; ///< 编译器生成的 C 类型声明及原生导出原型。
    Options options;
};

/**
 * @brief 直接启动已生成的原生程序，不经过命令行解释器。
 * @param options 决定产物路径的输出配置。
 * @param arguments 按原样传递的程序参数。
 * @param input 可选的标准输入内容；空指针表示继承当前输入。
 * @return 子进程退出码，启动失败返回一。
 */
int executeNative(const Options &options, const Memory::Vector<Memory::String> &arguments);

/**
 * @brief 取当前编译单元里 `#abi("C", "库", "符号")` 声明的外部 C 函数。
 *
 * 由前端（`source/semantics/artifacts.cpp`）注入，提供函数声明表与 C ABI 类型映射；
 * AOT 这边只消费结果，不反过来依赖前端，这样 `chali_aot` 始终是个干净的独立模块。
 * @return 外部函数清单；未登记提供方时返回空表。
 */
[[nodiscard]] Memory::Vector<ExternFunction> collectAbiExterns();

/**
 * @brief 读取当前编译单元的类型化 C ABI 导出描述。
 * @return 对外符号、语言标签与 C 类型。
 */
Memory::Vector<ExternFunction> collectAbiExports();
/**
 * @brief 注册前端 C ABI 导出描述入口。
 * @param provider 返回导出描述的提供方。
 */
void setAbiExportProvider(Memory::Vector<ExternFunction> (*provider)());
/**
 * @brief 读取生成的纯 C 导出声明。
 * @return 不含语言内部存储地址的 C 声明。
 */
Memory::String collectAbiExportHeader();
/**
 * @brief 登记纯 C 导出声明生成入口。
 * @param provider 前端提供方。
 */
void setAbiExportHeaderProvider(Memory::String (*provider)());

/**
 * @brief 前端注册"怎么取 abi 声明"的办法（进程启动时注入一次即可）。
 * @param provider 返回外部函数清单的函数指针；nullptr 表示清空。
 */
void setAbiExternProvider(Memory::Vector<ExternFunction> (*provider)());

/**
 * @brief 登记前端生成复杂 C ABI 宿主绑定源码的入口。
 * @param provider 返回绑定源码的回调；空指针清除登记。
 */
void setAbiGlueProvider(Memory::String (*provider)());

/**
 * @brief 取得需要结构体、引用、缓冲区或回调封送的宿主绑定源码。
 * @return 没有复杂 ABI 声明时为空串。
 */
[[nodiscard]] Memory::String collectAbiGlue();

/**
 * @brief 生成自包含的 LLVM 模块。
 * @param request 已检查 AST 或显式兼容四元式、数据映像及输出选项。
 * @param diagnostics 不支持的构造等诊断（非空 = 失败）。
 * @param header 给外部 C 用的头文件；不需要可以传 nullptr。
 * @return LLVM 模块文本；失败时返回空串。
 */
[[nodiscard]] Memory::String generateLLVM(const Request &request, Memory::Vector<Memory::String> *diagnostics,
                                          Memory::String *header = nullptr);
/** @brief 生成 LLVM 模块并导出独立单元的数据重定位资产。
 * @param request 已检查的输入。
 * @param diagnostics 接收失败诊断。
 * @param header 可选公开 C 头文件输出。
 * @param unit 可选单元数据输出，每次调用首先清空。
 * @return LLVM 模块文本；失败为空。 */
[[nodiscard]] Memory::String generateLLVM(const Request &request, Memory::Vector<Memory::String> *diagnostics,
                                          Memory::String *header, UnitData *unit);

/**
 * @brief 生成 LLVM 模块、写盘、并调 LLVM clang 驱动产出可执行文件 / 动态库。
 * @param request 输入。
 * @param diagnostics 诊断（含编译器的报错输出）。
 * @return 0 成功，非 0 失败。
 */
[[nodiscard]] int run(const Request &request, Memory::Vector<Memory::String> *diagnostics);
/** @brief 编译原生程序并导出独立单元的数据映像。
 * @param request 输入及输出选项。
 * @param diagnostics 接收失败诊断。
 * @param unit 可选独立数据输出。
 * @return 成功为零，失败非零。 */
[[nodiscard]] int run(const Request &request, Memory::Vector<Memory::String> *diagnostics, UnitData *unit);
/**
 * @brief 查找实际外置 LLVM 驱动，不启动子进程。
 * @param wanted 显式路径；空值使用环境、发行目录或 PATH。
 * @return 驱动路径或 clang 程序名。
 */
Memory::String llvmCompilerPath(const Memory::String &wanted);
/** @brief 核对实际使用的 LLVM 版本，静态后端直接返回编入版本。
 * @param options 静态／显式外置 LLVM 工具选择。
 * @param diagnostics 可选工具失败说明。
 * @return 精确 LLVM 版本；无法核对时为空串。 */
Memory::String llvmVersion(const Options &options, Memory::Vector<Memory::String> *diagnostics = nullptr);
/** @brief 将已验证的 bitcode 单独优化为原生对象，用于关闭跨单元 LTO 的路径。
 * @param bytes 已核对目标、布局及版本的 bitcode 字节。
 * @param options 目标、CPU 与 LLVM 工具选项。
 * @param output 私有工作区中的对象路径。
 * @param diagnostics 接收代码生成或工具失败。
 * @return 成功为零，失败非零。 */
int bitcodeObject(const Memory::String &bytes, const Options &options, const Memory::String &output,
                  Memory::Vector<Memory::String> *diagnostics);
/** @brief 按所选完整／ThinLTO 模式重新编码已验证模块，核对后才用于最终链接。
 * @param bytes 已验证目标、布局及版本的原始 bitcode。
 * @param options 目标、优化及 LTO 模式。
 * @param output 私有工作区中的 bitcode 路径。
 * @param diagnostics 接收 LLVM 读取或生成错误。
 * @return 成功零，失败非零。 */
int bitcodeModule(const Memory::String &bytes, const Options &options, const Memory::String &output,
                  Memory::Vector<Memory::String> *diagnostics);

}  // namespace chali_aot
