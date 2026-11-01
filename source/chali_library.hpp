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

namespace chali_aot
{
struct Options;
struct Request;
}

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
    License = 8
};
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
/** @brief 单次编译持有的已验证库及原生实现快照，不执行初始化。 */
class ImportSession
{
public:
    /** @brief 创建目标明确的会话。
     * @param target 目标别名或三元组。
     * @param cpu 目标 CPU。 */
    ImportSession(std::string target, std::string cpu);
    /** @brief 释放本次会话拥有的原生暂存文件。 */
    ~ImportSession();
    /** @brief 禁止复制快照及工作区所有权。 */
    ImportSession(const ImportSession &) = delete;
    /** @brief 禁止赋值快照及工作区所有权。 */
    ImportSession &operator=(const ImportSession &) = delete;
    /** @brief 加载并锁定模块。
     * @param path 包文件。
     * @param module 预期完整模块名；独立链接传空值时采用清单身份。
     * @param pin 可选精确依赖约束。
     * @return 已验证且会话内地址稳定的库。 */
    const Package &open(const std::filesystem::path &path, const std::string &module, const Dependency *pin = nullptr);
    /** @brief 从同一已验证快照生成链接输入。
     * @param options 接收原生对象及系统库。 */
    void appendTo(chali_aot::Options &options);
    /** @brief 在加载前启用项目级精确锁。
     * @param path 规范 JSON 锁文件；失败抛出诊断。 */
    void loadLock(const std::filesystem::path &path);
    /** @brief 判断模块是否被项目锁指定为库包。
     * @param module 完整模块名。
     * @return 锁中存在时为真，禁止源码回退。 */
    bool requiresPackage(const std::string &module) const;

    /** @brief 为独立链接递归解析包清单依赖。
     * @param path 根包文件。
     * @param searchRoots 额外库根。
     * @return 已验证的根包。 */
    const Package &openRecursive(const std::filesystem::path &path, const std::vector<std::filesystem::path> &searchRoots = {});

    /** @brief 取得用于诊断及接口封装检查的来源标签。
     * @return 已装载接口的稳定标签。 */
    std::vector<std::string> sources() const;
    /** @brief 返回快照来源的真实包路径，供发布保护与依赖追踪。
     * @return 本次成功加载的规范包路径。 */
    std::vector<std::filesystem::path> paths() const;
    /** @brief 导出当前快照的精确依赖记录，不重新打开包。
     * @return 完整模块、版本及内容摘要。 */
    std::vector<Dependency> records() const;

private:
    struct State;
    std::unique_ptr<State> state_;
};
/** @brief 从已检查函数生成不含运行时副本的标量原生库。
 * @param request 完整检查结果和公开原生签名。
 * @param module 完整模块身份。
 * @param version 精确包版本。
 * @param interface 不含实现的公开接口。
 * @param dependencies 已验证的实际依赖。
 * @param output 最终包路径。
 * @return 成功为零，失败为一并输出诊断。 */
int buildNative(const chali_aot::Request &request, const std::string &module, const std::string &version,
                const std::string &interface, const std::vector<Dependency> &dependencies, const std::filesystem::path &output);
/** @brief 按模块排序并生成独立库锁，不保存宿主路径。
 * @param modules 精确版本及整个包摘要。
 * @return 格式一的规范 JSON。 */
std::string lockJson(std::vector<Dependency> modules);
/** @brief 有界读取精确库锁，拒绝不兼容、重复键和非规范编码。
 * @param path 项目锁文件。
 * @return 按模块排序的精确约束。 */
std::vector<Dependency> readLock(const std::filesystem::path &path);
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
 * @brief 选择相同规范目标、布局与 CPU 契约的原生条目。
 * @param package 已验证的库包。
 * @param requested 目标别名或三元组。
 * @param cpu 目标 CPU，空值为登记默认值。
 * @param enabledFeatures 调用方明确支持的 CPU 特性。
 * @return 匹配的目标，不进行链接或初始化。
 * @throws std::runtime_error 不兼容、目标缺失或能力不足。
 */
const Target &select(const Package &package, const std::string &requested, const std::string &cpu = {}, const std::vector<std::string> &enabledFeatures = {});
/**
 * @brief 处理库包 pack／inspect 命令。
 * @param argc 归一化参数数目。
 * @param argv 完整命令数组。
 * @return 成功零、工具失败一、选项错误二。
 */
int runCli(int argc, char **argv);
}  // namespace chali_library
