/** @file chali_library.hpp
 * @brief 带独立接口、目标实现和内容校验的版本化库包。
 */
#pragma once
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>
#include <memory>

namespace chali_library
{
inline constexpr std::uint32_t packageFormat = 1;                           ///< 容器头和条目编码版本。
inline constexpr std::uint32_t interfaceFormat = 1;                         ///< 公开语言接口的表示版本。
inline constexpr std::uint32_t languageAbi = 1;                             ///< 语言声明与类型身份的兼容编号。
inline constexpr std::uint32_t runtimeAbi = 1;                              ///< 必须与公开 C 帧协议版本一致。
inline constexpr std::uint64_t maximumPackageBytes = 256ull * 1024 * 1024;  ///< 单包编码及有效载荷的预算，不代表进程总内存上限。
/** @brief 已登记的独立载荷种类；登记不等于启用相应执行后端。 */
enum class Kind : std::uint16_t
{
    Interface = 1,
    Templates = 2,
    Object = 3,
    Archive = 4,
    Bitcode = 5,
    Bytecode = 6,
    Source = 7,
    License = 8,
    UnitData = 9
};
/** @brief 从 bitcode 自身读取的目标及生产信息，不从清单猜测 LLVM 模块布局。 */
struct BitcodeInfo
{
    std::string triple, dataLayout, producer, llvmVersion;
    std::uint64_t epoch = 0;
    bool summary = false;  ///< 携带 GLOBALVAL_SUMMARY，供 ThinLTO 模式核对，不把完整 LTO 摘要混为一类。
};
/** @brief 在预算内读取单模块 LLVM bitcode 的兼容性头，不执行或优化模块。
 * @param bytes 已通过库包内容摘要核对的载荷。
 * @return 实际目标、布局和生产版本；截断、非法编码或预算耗尽时抛出异常。 */
BitcodeInfo inspectBitcode(std::string_view bytes);
/** @brief 一项以完整模块身份和内容摘要锁定的库依赖。 */
struct Dependency
{
    std::string module;   ///< 完整模块名。
    std::string version;  ///< 精确包版本，不作范围选择。
    std::string sha256;   ///< 整个依赖包的小写 SHA-256。
};
/** @brief 一种规范目标的实现及其可核对 ABI 信息。 */
struct Target
{
    std::string triple;                         ///< LLVM 规范三元组。
    std::string objectFormat;                   ///< coff 或 elf。
    std::string dataLayout;                     ///< 与登记目标运行时一致的 LLVM 数据布局。
    std::string cpu;                            ///< 生产时使用的 CPU。
    std::vector<std::string> features;          ///< 必需的 CPU 特性，不能静默忽略。
    std::string nativeAbi = "c-v1";             ///< 当前原生入口约定；与语言接口版本分开。
    std::string implementation;                 ///< 对象或归档条目名。
    std::string bitcode;                        ///< 可选 bitcode 条目名。
    std::string llvmVersion;                    ///< bitcode 所需的精确 LLVM 版本。
    std::string unitData, unitLayout, unitSymbol;  ///< 可选单元数据、布局条目及基址符号，三者同时存在。
    std::vector<std::string> systemLibraries;   ///< 最终链接需要的系统库名。
    std::vector<std::string> runtimeLibraries;  ///< 应用运行需要交付或由目标系统提供的动态库名。
};
/** @brief 不含指针和宿主路径的库包清单。 */
struct Manifest
{
    std::string module, version, compilerVersion;      ///< 模块、精确包版本和兼容编译器版本。
    std::uint32_t interfaceVersion = interfaceFormat;  ///< 接口表示版本。
    std::uint32_t languageAbiVersion = languageAbi;    ///< 语言接口 ABI。
    std::uint32_t runtimeAbiVersion = runtimeAbi;      ///< 运行时 C 帧 ABI。
    std::string interface = "interface/main.chl";      ///< 必需的语言接口条目。
    std::string templates;                             ///< 可选未实例化模板条目。
    std::string bytecode;                              ///< 可选独立字节码条目，不冒充机器对象。
    std::uint32_t bytecodeVersion = 0;                 ///< 字节码格式编号，未携带时为零。
    std::string source;                                ///< 可选源码回退资产，仅携带不会自动使用。
    std::string license;                               ///< 声明的许可证名称。
    std::vector<std::string> licenseFiles;             ///< 许可证与署名资产。
    std::vector<std::string> requiredFeatures;         ///< 未识别的必需能力必须拒绝。
    std::vector<Dependency> dependencies;              ///< 精确的语言库依赖。
    std::vector<Target> targets;                       ///< 按三元组区分的实现。
    std::string extensions = "{}";                     ///< 可忽略的 JSON 对象元数据，不改变选择语义。
};
/** @brief 已校验的条目；bytes 是拥有型原始负载，不指向文件映射。 */
struct Entry
{
    std::string name;             ///< 可移植的包内相对名字。
    Kind kind = Kind::Interface;  ///< 载荷种类。
    std::uint32_t flags = 0;      ///< 位零允许未知种类作为可忽略条目，其余位保留。
    std::string bytes;            ///< 包含零字节的完整负载。
};
/** @brief 一次事务恢复的完整库包。 */
struct Package
{
    Manifest manifest;           ///< 已校验的清单。
    std::vector<Entry> entries;  ///< 读取后按名字严格递增，写入时归一排序。
    std::string contentSha256;   ///< 从同一读取流计算的整包摘要，不序列化此字段。
    /**
     * @brief 查找精确名字，不进行宿主路径解析。
     * @param name 包内名字。
     * @return 对应条目；不存在时为空。
     */
    const Entry *find(std::string_view name) const;
};
/**
 * @brief 核对泛型元数据并恢复不含普通实现的模板文本。
 * @param manifest 已验证的库清单。
 * @param bytes 带版本、模块及语义形参的规范模板载荷。
 * @return 可以同模块装载的模板声明；非法载荷抛出错误。
 */
std::string templateSource(const Manifest &manifest, std::string_view bytes);
/**
 * @brief 核对可选库调试目录与当前目标实现的字节身份。
 * @param package 已持有全部条目的包快照。
 * @param target 本包的目标描述。
 * @return 规范调试描述；没有可选符号时为空。
 */
std::string debugDescriptor(const Package &package, const Target &target);
/**
 * @brief 序列化已验证的清单为格式一的规范 JSON。
 * @param manifest 模块与目标契约。
 * @return 固定字段顺序的无空白 JSON。
 * @throws std::runtime_error 清单不兼容或非法。
 */
std::string manifestJson(const Manifest &manifest);
/**
 * @brief 解析规范 JSON 并核对版本、目标布局及身份。
 * @param bytes 完整清单文本。
 * @return 已验证的拥有型清单。
 * @throws std::runtime_error 文本不规范、不兼容或超过预算。
 */
Manifest parseManifest(std::string_view bytes);
/**
 * @brief 核对包内条目及清单中的所有引用，不执行实现。
 * @param package 待核对包。
 * @throws std::runtime_error 条目缺失、类型不符或预算超限。
 */
void validate(const Package &package);
/**
 * @brief 从单文件事务读取库包，逐条核对长度和 SHA-256。
 * @param path 库包路径。
 * @param package 成功时才替换的结果。
 * @param error 失败说明。
 * @return 全部检查通过为真。
 */
bool read(const std::filesystem::path &path, Package &package, std::string &error);
/**
 * @brief 按名字排序并确定性写入库包，成功后才发布最终文件。
 * @param path 最终库包路径。
 * @param package 清单与拥有型负载。
 * @param error 失败说明。
 * @return 成功为真，失败保留旧产物。
 */
bool write(const std::filesystem::path &path, const Package &package, std::string &error);
/**
 * @brief 按明确实现选择核对规范目标、布局、CPU 及 bitcode 版本契约。
 * @param package 已验证的库包。
 * @param requested 目标别名或三元组。
 * @param cpu 目标 CPU，空值为登记默认值。
 * @param enabledFeatures 调用方明确支持的 CPU 特性。
 * @param implementation 明确选择 native 或 bitcode。
 * @param llvmVersion 实际 LLVM 读取版本，选择 bitcode 时必须提供。
 * @return 匹配的目标，不进行链接或初始化。
 * @throws std::runtime_error 不兼容、目标缺失或能力不足。
 */
const Target &select(const Package &package, const std::string &requested, const std::string &cpu = {}, const std::vector<std::string> &enabledFeatures = {},
                     const std::string &implementation = "native", const std::string &llvmVersion = {});
/**
 * @brief 处理库包 pack／inspect 命令。
 * @param argc 归一化参数数目。
 * @param argv 完整命令数组。
 * @return 成功零、工具失败一、选项错误二。
 */
int runCli(int argc, char **argv);
}  // namespace chali_library
