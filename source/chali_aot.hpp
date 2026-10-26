/**
 * @file chali_aot.hpp
 * @brief 从 ChaLi 四元式直接生成 LLVM 模块并编译原生产物。
 */
#pragma once
#include "stackvm.hpp"

namespace chali_aot
{

/**
 * @brief 一个由 `#abi("C", "库", "符号")` 声明、要链接到外部 C 的函数。
 */
struct ExternFunction
{
    Memory::String label;                           ///< 四元式里的调用标签。
    Memory::String symbol;                          ///< C 侧真正的符号名（`= "名字"`）。
    Memory::String library;                         ///< 库路径（生成注释与链接参数用，可空）。
    Memory::String resultType = "long long";        ///< C 返回类型名。
    Memory::Vector<Memory::String> parameterTypes;  ///< C 形参类型名，按序。
    Memory::String abiConvention = "C";             ///< 已验证的外部调用约定。
};

/**
 * @brief AOT 输出的形态与开关。
 */
struct Options
{
    bool checked = true;                             ///< 默认生成与 VM 一致的数组边界和指针检查。
    bool threadsEnabled = false;                     ///< 是否启用与 VM 同源的隔离线程服务。
    bool shared = false;                             ///< 产出动态库而不是可执行文件。
    bool emitLLVMOnly = false;                       ///< `--emit-llvm`：只写 LLVM 模块，不调用目标代码工具。
    Memory::String compiler;                         ///< LLVM clang 驱动；空值优先使用 library/llvm 中的工具。
    Memory::String outputPrefix;                     ///< LLVM 模块、头文件和原生产物的公共前缀。
    Memory::Vector<Memory::String> exports;          ///< 显式导出给 C 的 ChaLi 函数名。
    Memory::Vector<Memory::String> linkLibraries;    ///< 额外的链接库（`abi` 里的库路径）。
    Memory::Vector<Memory::String> linkObjects;      ///< 本次编译生成的宿主封送目标文件。
    std::size_t memoryBytes = 512u * 1024u * 1024u;  ///< 数据区之外的堆预算。
};

/**
 * @brief 一次 AOT 编译的全部输入。
 */
struct Request
{
    const stackvm::QuadFile *quads = nullptr;   ///< 已优化的四元式。
    const stackvm::Program *program = nullptr;  ///< 组装结果（取标签、形参与类型）。
    Memory::String dataBytes;                   ///< `.data.bin` 原始字节。
    Memory::String dataLayout;                  ///< 布局文本（每行 `名 offset=… size=…`）。
    Memory::Vector<ExternFunction> externs;     ///< `abi` 声明的外部 C 函数。
    Memory::String abiGlue;                     ///< 复杂 C ABI 使用的类型化宿主绑定源码。
    Memory::Vector<ExternFunction> abiExports; ///< 经类型校验的 C ABI 导出，与语言入口独立。
    Memory::String abiExportHeader; ///< 编译器生成的 C 类型声明及原生导出原型。
    Options options;
};

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
 * @param request 已优化四元式、数据映像、ABI 声明和输出选项。
 * @param diagnostics 不支持的构造等诊断（非空 = 失败）。
 * @param header 给外部 C 用的头文件；不需要可以传 nullptr。
 * @return LLVM 模块文本；失败时返回空串。
 */
[[nodiscard]] Memory::String generateLLVM(const Request &request, Memory::Vector<Memory::String> *diagnostics,
                                          Memory::String *header = nullptr);

/**
 * @brief 生成 LLVM 模块、写盘、并调 LLVM clang 驱动产出可执行文件 / 动态库。
 * @param request 输入。
 * @param diagnostics 诊断（含编译器的报错输出）。
 * @return 0 成功，非 0 失败。
 */
[[nodiscard]] int run(const Request &request, Memory::Vector<Memory::String> *diagnostics);

}  // namespace chali_aot
