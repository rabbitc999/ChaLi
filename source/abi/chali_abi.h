/**
 * @file chali_abi.h
 * @brief ChaLi 宿主胶水的直通 ABI：C 侧唯一需要包含的头文件。
 *
 * 直通 ABI 的目标是把「一次宿主调用」从多层中间结构压成一次函数指针调用：
 *
 *   CHALI 调用点 → chali_host_call(name, count, slots, kinds)
 *                → 直通入口表（编译期常量，二分查找）
 *                → ChaliAbiEntry(frame)
 *
 * 与旧的 NativeRegistry 路径相比，直通路径不构造 `NativeValue` 向量、不整份拷贝
 * `NativeContext`、不经过 `Memory::Function` 的类型擦除间接调用。实参始终是生成
 * 代码已经写好的 64 位槽数组，C 胶水按位模式直接取用。
 *
 * @note 本头文件必须能同时被 C 和 C++ 包含，且不得引入任何 C++ 头。
 */
#ifndef CHALI_ABI_H
#define CHALI_ABI_H

#include <stddef.h>
#include <stdint.h>

#if defined(_WIN32) && defined(__x86_64__) && (defined(__GNUC__) || defined(__clang__))
#define CHALI_ABI_CALL __attribute__((ms_abi))
#elif defined(_WIN32) && (defined(_M_IX86) || defined(__i386__) || defined(_M_X64))
#define CHALI_ABI_CALL __cdecl
#elif defined(__x86_64__) && (defined(__GNUC__) || defined(__clang__))
#define CHALI_ABI_CALL __attribute__((sysv_abi))
#else
#define CHALI_ABI_CALL
#endif

#if defined(_M_X64) || defined(__x86_64__)
#define CHALI_ABI_TARGET_ARCH 2u
#elif defined(_M_IX86) || defined(__i386__)
#define CHALI_ABI_TARGET_ARCH 1u
#elif defined(_M_ARM64) || defined(__aarch64__)
#define CHALI_ABI_TARGET_ARCH 4u
#elif defined(_M_ARM) || defined(__arm__)
#define CHALI_ABI_TARGET_ARCH 3u
#else
#define CHALI_ABI_TARGET_ARCH 0u
#endif

#if defined(_WIN32)
#define CHALI_ABI_TARGET_PLATFORM 1u
#elif defined(__linux__)
#define CHALI_ABI_TARGET_PLATFORM 2u
#elif defined(__APPLE__)
#define CHALI_ABI_TARGET_PLATFORM 3u
#else
#define CHALI_ABI_TARGET_PLATFORM 0u
#endif

#if CHALI_ABI_TARGET_ARCH == 3u && (defined(__ARM_PCS_VFP) || defined(_WIN32))
#define CHALI_ABI_TARGET_CONVENTION 2u
#else
#define CHALI_ABI_TARGET_CONVENTION 1u
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 一次直通调用可见的运行时句柄，布局对 C 侧不透明。
 *
 * 句柄只在本次宿主回调期间有效；C 胶水只能通过本头文件声明的
 * `chali_abi_*` 函数使用它，不得解引用或跨调用保存。
 */
typedef struct ChaliAbiRuntime ChaliAbiRuntime;

/** @brief 不透明宿主函数表；注册期串行使用，封存后可并发查询。 */
typedef struct ChaliAbiRegistry ChaliAbiRegistry;

/**
 * @brief 释放已接管的宿主对象，回调不得抛出异常。
 * @param record 需要销毁的宿主对象。
 */
typedef void(CHALI_ABI_CALL *ChaliAbiDestroy)(void *record);

/**
 * @brief 从借用对象创建独立副本，失败可以返回空指针。
 * @param record 借用的只读宿主对象。
 * @return 独立副本，成功登记后才转移给运行时。
 */
typedef void *(CHALI_ABI_CALL *ChaliAbiClone)(const void *record);

/**
 * @brief 查询或复制宿主记录的文本，回调不得抛出异常、修改记录或调用语言代码。
 * @param record 本次调用期间存活的只读记录。
 * @param data 调用方缓冲；为空且 capacity 为零时仅查询。
 * @param capacity 可写字节数，不含额外终止字节，回调不得越界写入。
 * @param required 接收完整文本字节数，不得为空；容量不足时不改写 data。
 * @return 查询或完整写入成功为一；其他情况为零。
 * @note 记录及回调输出均不转移所有权；两次调用期间文本应保持稳定，允许空文本和内嵌零字节。
 */
typedef int(CHALI_ABI_CALL *ChaliAbiFormat)(const void *record, char *data, size_t capacity, size_t *required);

/**
 * @brief 直通槽的值类别，与生成代码写入的类型数组一一对应。
 */
enum
{
    ChaliAbiSlotInteger = 0,  ///< 按 64 位整数解释。
    ChaliAbiSlotFloat = 1     ///< 按 IEEE-754 双精度解释。
};

/**
 * @brief 一次直通调用的输入输出帧。
 *
 * `slots` 与 `kinds` 由生成代码直接提供，长度均为 `count`；直通入口读参数、
 * 写 `result`，全程不产生任何中间容器。
 */
typedef struct ChaliAbiFrame
{
    ChaliAbiRuntime *runtime;    ///< 本次调用的运行时句柄。
    const int64_t *slots;        ///< 实参原始位模式，长度 `count`。
    const unsigned char *kinds;  ///< 每个实参的值类别，长度 `count`。
    int64_t count;               ///< 实参数量。
    int64_t result;              ///< 直通入口写回的返回位模式。
    int32_t resultIsFloat;       ///< 返回值是否为浮点槽。
} ChaliAbiFrame;

/**
 * @brief 直通入口的统一签名：入参出参都在帧上，不分配、不抛异常。
 */
typedef void(CHALI_ABI_CALL *ChaliAbiEntry)(ChaliAbiFrame *frame);

/**
 * @brief 直通入口表项。
 */
typedef struct ChaliAbiRecord
{
    const char *name;     ///< 原生函数完整名称，与注册表登记名一致。
    ChaliAbiEntry entry;  ///< 直通入口。
    int32_t arity;        ///< 固定实参数量；-1 表示由入口自行检查的变参。
} ChaliAbiRecord;

/** @brief C 宿主边界版本；只有兼容布局与含义完全相同时才接受。 */
#define CHALI_ABI_VERSION           UINT32_C(1)

/** @brief 可选能力位；未知的必需能力会导致模块被拒绝。 */
#define CHALI_ABI_CAP_SYNC_CALLBACK UINT64_C(1)
#define CHALI_ABI_CAP_RECORD        UINT64_C(2)
#define CHALI_ABI_CAP_BYTE_VIEW     UINT64_C(4)
#define CHALI_ABI_CAP_REGISTRY      UINT64_C(8)
#define CHALI_ABI_CAP_MEMORY        UINT64_C(16)
#define CHALI_ABI_CAP_BINARY        UINT64_C(32)  ///< 原始字符串与字节列表服务。
#define CHALI_ABI_CAP_RECORD_CLONE  UINT64_C(64)  ///< 宿主记录自动独立复制服务。
#define CHALI_ABI_CAP_RECORD_PROBE  UINT64_C(128) ///< 不改变错误状态的宿主记录存活与类型探测。
#define CHALI_ABI_CAP_LIST_TEXT     UINT64_C(256) ///< 文本列表直接追加服务。
#define CHALI_ABI_CAP_LIST_NUMBERS  UINT64_C(512) ///< 保留整数及浮点位模式的数值列表服务。
#define CHALI_ABI_CAP_RECORD_TEXT   UINT64_C(1024) ///< 通过 C 回调提供宿主记录默认文本。

/** @brief C 宿主边界的稳定状态码，不使用 errno 或 C++ 异常。 */
enum
{
    ChaliAbiOk = 0,
    ChaliAbiInvalidArgument = 1,
    ChaliAbiIncompatible = 2,
    ChaliAbiDuplicate = 3,
    ChaliAbiHostFailure = 4,
    ChaliAbiNotFound = 5,
    ChaliAbiSealed = 6
};

/**
 * @brief 调用方持有的诊断缓冲；不转移所有权，不分配内存。
 * @note required 包含末尾零字节；容量不足时仍写入可容纳的完整前缀及零字节。
 */
typedef struct ChaliAbiError
{
    char *data;       ///< 可写字节缓冲；容量为零时可为空。
    size_t capacity;  ///< 缓冲区字节容量。
    size_t required;  ///< 完整诊断所需字节数；成功时为零。
    int32_t status;   ///< ChaliAbiOk 或稳定错误码。
} ChaliAbiError;

/**
 * @brief 当前编译目标的 ABI 描述；由模块自己的 C 编译器生成并与宿主比较。
 * @note 不把宿主描述复制为模块描述；不同目标布局不能互相装载。
 */
typedef struct ChaliAbiInfo
{
    uint32_t structSize;         ///< 本描述结构字节数。
    uint32_t version;            ///< CHALI_ABI_VERSION。
    uint32_t pointerBits;        ///< 对象指针位数。
    uint32_t littleEndian;       ///< 小端为一，大端为零。
    uint32_t callingConvention;  ///< 一为默认 C，二为 ARM32 硬浮点 C 调用约定。
    uint32_t architecture;       ///< 一为 x86，二为 x64，三为 ARM，四为 ARM64；零不支持。
    uint32_t platform;           ///< 一为 Windows，二为 Linux，三为 Darwin；零不支持。
    uint32_t frameSize;          ///< ChaliAbiFrame 字节数。
    uint32_t frameAlignment;     ///< ChaliAbiFrame 对齐。
    uint32_t recordSize;         ///< ChaliAbiRecord 字节数。
    uint32_t recordAlignment;    ///< ChaliAbiRecord 对齐。
    uint32_t frameOffsets[6];    ///< runtime、slots、kinds、count、result、resultIsFloat 偏移。
    uint32_t recordOffsets[3];   ///< name、entry、arity 偏移。
    uint64_t capabilities;       ///< 模块要求／宿主支持的能力位。
} ChaliAbiInfo;

/**
 * @brief 用包含本头的编译器生成模块自身的目标布局。
 * @param capabilities 模块实际要求的能力位。
 * @return 不引用外部存储的 ABI 描述副本。
 */
static inline ChaliAbiInfo chali_abi_local_info(uint64_t capabilities)
{
    const uint32_t endian = 1;
    ChaliAbiInfo info;
    info.structSize = (uint32_t)sizeof(ChaliAbiInfo);
    info.version = CHALI_ABI_VERSION;
    info.pointerBits = (uint32_t)(sizeof(void *) * 8u);
    info.littleEndian = *(const unsigned char *)&endian;
    info.callingConvention = CHALI_ABI_TARGET_CONVENTION;
    info.architecture = CHALI_ABI_TARGET_ARCH;
    info.platform = CHALI_ABI_TARGET_PLATFORM;
    info.frameSize = (uint32_t)sizeof(ChaliAbiFrame);
    info.recordSize = (uint32_t)sizeof(ChaliAbiRecord);
#ifdef __cplusplus
    info.frameAlignment = (uint32_t)alignof(ChaliAbiFrame);
    info.recordAlignment = (uint32_t)alignof(ChaliAbiRecord);
#else
    info.frameAlignment = (uint32_t)_Alignof(ChaliAbiFrame);
    info.recordAlignment = (uint32_t)_Alignof(ChaliAbiRecord);
#endif
    info.frameOffsets[0] = (uint32_t)offsetof(ChaliAbiFrame, runtime);
    info.frameOffsets[1] = (uint32_t)offsetof(ChaliAbiFrame, slots);
    info.frameOffsets[2] = (uint32_t)offsetof(ChaliAbiFrame, kinds);
    info.frameOffsets[3] = (uint32_t)offsetof(ChaliAbiFrame, count);
    info.frameOffsets[4] = (uint32_t)offsetof(ChaliAbiFrame, result);
    info.frameOffsets[5] = (uint32_t)offsetof(ChaliAbiFrame, resultIsFloat);
    info.recordOffsets[0] = (uint32_t)offsetof(ChaliAbiRecord, name);
    info.recordOffsets[1] = (uint32_t)offsetof(ChaliAbiRecord, entry);
    info.recordOffsets[2] = (uint32_t)offsetof(ChaliAbiRecord, arity);
    info.capabilities = capabilities;
    return info;
}

/** @brief 模块的只读入口描述，注册成功后宿主必须复制名称和入口。 */
typedef struct ChaliAbiModuleDescriptor
{
    uint32_t structSize;            ///< 本描述结构大小。
    const ChaliAbiInfo *abi;        ///< 模块编译目标描述。
    const ChaliAbiRecord *records;  ///< 入口数组；count 为零时可为空。
    size_t count;                   ///< 入口数量。
} ChaliAbiModuleDescriptor;

/**
 * @brief 获取当前编译目标的只读 ABI 描述。
 * @return 进程存活期间有效的描述，不得修改或释放。
 */
const ChaliAbiInfo *CHALI_ABI_CALL chali_abi_info(void);

/**
 * @brief 检查模块 ABI 是否与当前宿主兼容。
 * @param expected 模块使用自己的编译器生成的 ABI 描述。
 * @param error 可选的调用方诊断缓冲。
 * @return 稳定状态码；成功为 ChaliAbiOk。
 */
int CHALI_ABI_CALL chali_abi_check(const ChaliAbiInfo *expected, ChaliAbiError *error);

/**
 * @brief 无副作用检查整个模块；不注册入口，也不调用用户代码。
 * @param module 模块入口及目标描述。
 * @param error 可选的调用方诊断缓冲。
 * @return 兼容且所有名称、入口、参数数量和模块内重复检查通过时为零。
 */
int CHALI_ABI_CALL chali_abi_check_module(const ChaliAbiModuleDescriptor *module, ChaliAbiError *error);

/**
 * @brief 调用前检查槽数组、参数数量及类别。
 * @param frame 当前帧；函数不修改帧。
 * @param arity 固定参数数量，-1 表示变参。
 * @param error 可选的调用方诊断缓冲。
 * @return 输入合法时为零；内存必须由调用方保证可访问。
 */
int CHALI_ABI_CALL chali_abi_check_frame(const ChaliAbiFrame *frame, int arity, ChaliAbiError *error);

/**
 * @brief 调用后检查返回槽类别。
 * @param frame 调用后的帧。
 * @param error 可选的调用方诊断缓冲。
 * @return resultIsFloat 为零或一时为零。
 */
int CHALI_ABI_CALL chali_abi_check_result(const ChaliAbiFrame *frame, ChaliAbiError *error);

/**
 * @brief 创建拥有运行时函数表的 C 句柄。
 * @param error 可选的调用方诊断缓冲。
 * @return 成功时为句柄，失败时为空；与 destroy 配对。
 */
ChaliAbiRegistry *CHALI_ABI_CALL chali_abi_registry_create(ChaliAbiError *error);

/**
 * @brief 销毁拥有的函数表；调用方须先结束所有查询和调用。
 * @param registry create 返回的句柄，允许为空；不得重复销毁或销毁借用句柄。
 */
void CHALI_ABI_CALL chali_abi_registry_destroy(ChaliAbiRegistry *registry);

/**
 * @brief 原子登记整个模块，失败时不保留该模块的任何入口。
 * @param registry 未封存的函数表。
 * @param module 名称会复制，入口代码须存活到函数表销毁。
 * @param error 可选的调用方诊断缓冲。
 * @return 稳定状态码；已有入口与其回调状态保持原样。
 */
int32_t CHALI_ABI_CALL chali_abi_registry_register(ChaliAbiRegistry *registry, const ChaliAbiModuleDescriptor *module, ChaliAbiError *error);

/**
 * @brief 封存函数表，后续注册与移除均拒绝。
 * @param registry 需要封存的句柄。
 * @param error 可选的调用方诊断缓冲。
 * @return 稳定状态码；重复封存仍成功。
 */
int32_t CHALI_ABI_CALL chali_abi_registry_seal(ChaliAbiRegistry *registry, ChaliAbiError *error);

/**
 * @brief 取得已登记的直接 C 入口，不调用用户代码。
 * @param registry 函数表句柄。
 * @param name 完整名称，不得为空。
 * @param entry 接收入口，失败时清空。
 * @param arity 可选的参数数量接收器，失败时为零。
 * @param error 可选的调用方诊断缓冲。
 * @return 稳定状态码；名称或直接入口不存在时为 ChaliAbiNotFound。
 */
int32_t CHALI_ABI_CALL chali_abi_registry_resolve(const ChaliAbiRegistry *registry, const char *name, ChaliAbiEntry *entry, int32_t *arity, ChaliAbiError *error);

/**
 * @brief 核对帧并调用 C 入口，将异常及入口错误转换成状态。
 * @param registry 函数表句柄。
 * @param name 完整名称。
 * @param frame 调用方持有的帧；纯标量入口允许 runtime 为空。
 * @param error 可选的调用方诊断缓冲。
 * @return 稳定状态码；失败时返回槽重置为整数零。
 */
int32_t CHALI_ABI_CALL chali_abi_registry_call(const ChaliAbiRegistry *registry, const char *name, ChaliAbiFrame *frame, ChaliAbiError *error);

/**
 * @brief 取直通入口表。
 * @param count 接收表项数量，可为空。
 * @return 只读表首地址，按名称升序排列且存活到进程结束；初始化失败为空且数量为零。
 */
const ChaliAbiRecord *CHALI_ABI_CALL chali_abi_records(size_t *count);

/**
 * @brief 把原生函数名解析成直通入口。
 * @param name 原生函数完整名称，空指针时返回空。
 * @param arity 接收该入口声明的实参数量，可为空。
 * @return 直通入口；名字不在表中返回空指针。
 * @note 解析是纯表查找，不构造任何字符串对象，适合在绑定期批量完成。
 */
ChaliAbiEntry CHALI_ABI_CALL chali_abi_resolve(const char *name, int *arity);

/* ---------------------------------------------------------------------------
 *  模块入口表
 *
 * 每个 C 胶水模块导出一张自己的入口表，宿主可以按模块裁剪：只把需要的那几张
 * 表登记进注册表，不必全量绑定。已经全部用 C 实现的模块不再有对应的 C++ 胶水
 * 文件，是否可达完全由这里决定。
 * ------------------------------------------------------------------------- */

/**
 * @brief 模块入口表的取表函数签名。
 * @param count 接收表项数量，可为空。
 * @return 该模块的静态常量表首地址，按名称升序排列。
 */
typedef const ChaliAbiRecord *(CHALI_ABI_CALL *ChaliAbiModule)(size_t *count);

/**
 * @brief `std::lang` 标准错误流输出原语：__std_stderr_write。
 */
const ChaliAbiRecord *CHALI_ABI_CALL abi_print_records(size_t *count);

/**
 * @brief `std::math` 标量数学函数（含 `__xxx64` 双精度变体），共 36 项。
 */
const ChaliAbiRecord *CHALI_ABI_CALL abi_math_records(size_t *count);

/**
 * @brief `std::clock` 单调时钟：nowNanos / nowMs。
 */
const ChaliAbiRecord *CHALI_ABI_CALL abi_clock_records(size_t *count);

/**
 * @brief `std::random` 生成器：创建、取值、复制与释放，共 9 项。
 */
const ChaliAbiRecord *CHALI_ABI_CALL abi_random_records(size_t *count);

/**
 * @brief 字符串构造：浮点格式化入口 `__std_str_of_flt`。
 */
const ChaliAbiRecord *CHALI_ABI_CALL abi_string_records(size_t *count);

/**
 * @brief 取得标准浮点解析的 C 原语入口。
 * @param count 接收入口数量，可为空。
 * @return 程序存活期间有效的只读表。
 */
const ChaliAbiRecord *CHALI_ABI_CALL abi_parse_records(size_t *count);

/**
 * @brief 取得 JSON 与 TOML 格式转换的 C 内核入口。
 * @param count 接收入口数量，可为空。
 * @return 程序存活期间有效的只读表。
 */
const ChaliAbiRecord *CHALI_ABI_CALL abi_serialization_records(size_t *count);

/* ---------------------------------------------------------------------------
 *  C 胶水使用的运行时服务
 *
 * 这些函数由 C++ 运行时提供，全部为 `extern "C"`，C 胶水通过它们访问堆、
 * 数据区、对象表和错误通道，自身不依赖任何 C++ 类型。
 * ------------------------------------------------------------------------- */

/**
 * @brief 读取实参槽的 64 位整数值。
 * @param frame 当前调用帧。
 * @param index 实参下标，越界时返回零。
 * @return 该槽的整数值。
 */
int64_t CHALI_ABI_CALL chali_abi_integer(const ChaliAbiFrame *frame, int64_t index);

/**
 * @brief 检查整数类别并读取参数，失败时输出零并记录错误。
 * @param frame 当前调用帧。
 * @param index 参数下标。
 * @param out 非空的整数接收器。
 * @return 成功为一，类别或下标无效时为零。
 */
int CHALI_ABI_CALL chali_abi_argument_integer(const ChaliAbiFrame *frame, int64_t index, int64_t *out);

/**
 * @brief 检查浮点类别并读取双精度位模式，不隐式转换整数槽。
 * @param frame 当前调用帧。
 * @param index 参数下标。
 * @param out 非空的浮点接收器。
 * @return 成功为一，类别或下标无效时为零。
 */
int CHALI_ABI_CALL chali_abi_argument_real(const ChaliAbiFrame *frame, int64_t index, double *out);

/**
 * @brief 读取实参槽的双精度值。
 * @param frame 当前调用帧。
 * @param index 实参下标，越界时返回零。
 * @return 该槽按 IEEE-754 解释的双精度值。
 */
double CHALI_ABI_CALL chali_abi_real(const ChaliAbiFrame *frame, int64_t index);

/**
 * @brief 取得 VM 字符串或字节块的零拷贝视图。
 * @param frame 当前调用帧。
 * @param address 字符串或字节块的 VM 地址，零地址得到空视图。
 * @param data 接收首字节地址，可为空。
 * @param length 接收字节长度，可为空。
 * @return 地址可读且长度有效时为 true，否则为 false 并记录运行时诊断。
 * @note 视图仅有效到下一次分配、扩容、释放或重入调用之前；不得保存或释放。
 */
int CHALI_ABI_CALL chali_abi_bytes(const ChaliAbiFrame *frame, int64_t address, const unsigned char **data, size_t *length);

/**
 * @brief 在当前运行时分配一个 VM 字符串并返回其地址。
 * @param frame 当前调用帧。
 * @param data UTF-8 字节，不接受内嵌零字节；空文本允许空指针。
 * @param length 字节长度。
 * @return 新字符串地址；分配失败返回零并记录运行时诊断。
 */
int64_t CHALI_ABI_CALL chali_abi_string(const ChaliAbiFrame *frame, const char *data, size_t length);

/**
 * @brief 按精确长度复制原始字符串字节，保留零字节及非 UTF-8 数据。
 * @param frame 当前帧。
 * @param data 输入跨度，非空时不能为零指针。
 * @param length 字节数。
 * @return 拥有型 String 句柄，使用 chali_abi_string_release 配对释放；失败为零。
 */
int64_t CHALI_ABI_CALL chali_abi_string_bytes(const ChaliAbiFrame *frame, const char *data, size_t length);
/**
 * @brief 查询或复制字节列表，短缓冲或无效元素不写入目标。
 * @param frame 当前帧。
 * @param handle 列表句柄，所有元素必须为 0..255 的整数。
 * @param data 可写目标，空指针配合零容量表示查询。
 * @param capacity 目标字节容量。
 * @param required 接收所需字节数，不得为空。
 * @return 成功为一，失败记录帧错误并返回零。
 */
int CHALI_ABI_CALL chali_abi_list_bytes(const ChaliAbiFrame *frame, int64_t handle,
                                       unsigned char *data, size_t capacity, size_t *required);
/**
 * @brief 按原有逐元素增长策略复制字节跨度为独立列表。
 * @param frame 当前帧。
 * @param data 输入跨度，非空时不得为空。
 * @param length 字节数。
 * @return 拥有型 List<uint8> 句柄，交还语言或使用 chali_abi_record_release 释放；失败为零。
 */
int64_t CHALI_ABI_CALL chali_abi_list_from_bytes(const ChaliAbiFrame *frame, const unsigned char *data, size_t length);

/**
 * @brief 查询或复制非拥有数值列表，保留整数及 IEEE-754 双精度位模式，不做数值转换。
 * @param frame 当前调用帧。
 * @param handle 数值列表句柄；文本、拥有值和聚合元素被拒绝。
 * @param bits 接收八字节位模式；与 kinds 同为空且 capacity 为零时仅查询。
 * @param kinds 接收 ChaliAbiSlotInteger 或 ChaliAbiSlotFloat，复制时不得为空。
 * @param capacity 两个输出数组各自可容纳的元素数量。
 * @param required 接收实际元素数，不得为空。
 * @return 成功为一；无效元素或容量不足时为零，两个输出数组均不改写。
 * @note 输出数组由宿主拥有且不得相互重叠；调用方负责将整数位模式缩窄为语言元素类型。
 */
int CHALI_ABI_CALL chali_abi_list_numbers(const ChaliAbiFrame *frame, int64_t handle,
                                        int64_t *bits, unsigned char *kinds, size_t capacity, size_t *required);
/**
 * @brief 从独立数值跨度复制为拥有型列表，直接使用集合数值槽，不构造通用原生值。
 * @param frame 当前调用帧。
 * @param bits 八字节整数或浮点位模式，非空跨度不得为空。
 * @param kinds 各元素为 ChaliAbiSlotInteger 或 ChaliAbiSlotFloat；为空时全部按整数处理。
 * @param count 元素数量。
 * @return 独立拥有型列表句柄；失败为零且不发布部分列表，成功后交还语言或以 record_release 释放。
 * @note bool、单字节 char 和各宽度整数使用整数槽；float32 先扩展为 double，保留扩展后的位模式。
 */
int64_t CHALI_ABI_CALL chali_abi_list_from_numbers(const ChaliAbiFrame *frame,
                                                  const int64_t *bits, const unsigned char *kinds, size_t count);
/**
 * @brief 将独立宿主数值跨度批量追加到现有列表，保留列表身份及当前存储布局。
 * @param frame 当前调用帧。
 * @param handle 可写数值列表句柄，不得存在活动迭代租约或元素视图。
 * @param bits 独立宿主数组，非空跨度不得为空，不得指向运行时借用视图或目标列表缓冲。
 * @param kinds 整数为零、浮点为一；为空表示全部整数。
 * @param count 追加元素数。
 * @return 成功为一；无效输入、借用冲突或分配失败为零，原有元素和长度保持不变。
 * @note 使用 CHALI_ABI_CAP_LIST_NUMBERS；扩容按现有容量逐次倍增，受最大容量约束。
 */
int CHALI_ABI_CALL chali_abi_list_append_numbers(const ChaliAbiFrame *frame, int64_t handle,
                                                const int64_t *bits, const unsigned char *kinds, size_t count);

/**
 * @brief 将借用文本复制为独立的 List<String> 元素，成功构造后才追加。
 * @param frame 当前调用帧。
 * @param handle 存活且可写的 List<String> 句柄；调用方负责匹配元素类型。
 * @param data 借用的文本字节，空文本可为空；允许零字节。
 * @param length 字节长度。
 * @return 成功为一，错误为零；输入缓冲的所有权不转移。
 * @note 调用方须遵守语言借用规则，不得在存在活动迭代租约时改变列表结构。
 */
int CHALI_ABI_CALL chali_abi_list_append_text(const ChaliAbiFrame *frame, int64_t handle, const char *data, size_t length);

/**
 * @brief 分配随本次运行结束回收的临时字节块。
 * @param frame 当前调用帧。
 * @param bytes 字节数。
 * @return 块地址；失败返回零。
 */
int64_t CHALI_ABI_CALL chali_abi_temporary(const ChaliAbiFrame *frame, size_t bytes);

/**
 * @brief 从受检查的逻辑地址复制字节，不把语言地址当宿主指针。
 * @param frame 当前调用帧。
 * @param address 逻辑地址。
 * @param out 调用方的可写缓冲，bytes 为零时可为空。
 * @param bytes 字节数量。
 * @return 完整范围读取成功时为一，否则记录错误并返回零。
 */
int CHALI_ABI_CALL chali_abi_read(const ChaliAbiFrame *frame, int64_t address, void *out, size_t bytes);

/**
 * @brief 将调用方字节写入受检查的逻辑地址。
 * @param frame 当前调用帧。
 * @param address 可写逻辑地址。
 * @param data 调用方的字节缓冲，bytes 为零时可为空。
 * @param bytes 字节数量。
 * @return 完整范围写入成功时为一，否则记录错误并返回零。
 */
int CHALI_ABI_CALL chali_abi_write(const ChaliAbiFrame *frame, int64_t address, const void *data, size_t bytes);

/**
 * @brief 释放本运行时拥有的 String 描述符及最终容量缓冲。
 * @param frame 当前调用帧。
 * @param address 拥有的 String 句柄，零为空串且无需释放。
 * @return 成功为一，失效地址或重复释放会失败；调用方不得释放借用地址。
 */
int CHALI_ABI_CALL chali_abi_string_release(const ChaliAbiFrame *frame, int64_t address);

/**
 * @brief 提前释放 temporary 创建的原始块，不用于 String 或对象句柄。
 * @param frame 当前调用帧。
 * @param address 拥有的原始块地址，零允许忽略。
 * @return 成功为一，无效地址或重复释放时为零。
 */
int CHALI_ABI_CALL chali_abi_temporary_release(const ChaliAbiFrame *frame, int64_t address);

/**
 * @brief 把文本追加到本次运行的标准输出桶并下沉给宿主。
 * @param frame 当前调用帧。
 * @param data 待输出的字节。
 * @param length 字节长度。
 */
void CHALI_ABI_CALL chali_abi_emit(const ChaliAbiFrame *frame, const char *data, size_t length);

/**
 * @brief 把文本写入标准错误流并立即刷新。
 * @param frame 当前调用帧。
 * @param data 待输出的字节。
 * @param length 字节长度。
 * @param newline 非零时在文本后追加换行。
 * @note 整次写入（含换行）在同一把锁内完成，多线程下不会与其它标准错误输出交错。
 */
void CHALI_ABI_CALL chali_abi_emit_error(const ChaliAbiFrame *frame, const char *data, size_t length, int newline);

/**
 * @brief 报告本次直通调用失败。
 * @param frame 当前调用帧。
 * @param message 需要传播给 VM 的错误文本，空指针按空串处理。
 * @note 只记录诊断，不展开栈；入口随后应尽快返回。
 */
void CHALI_ABI_CALL chali_abi_fail(const ChaliAbiFrame *frame, const char *message);

/**
 * @brief 把双精度值写成返回浮点槽。
 * @param frame 当前调用帧。
 * @param value 返回值。
 */
void CHALI_ABI_CALL chali_abi_return_real(ChaliAbiFrame *frame, double value);

/**
 * @brief 把 64 位整数写成返回整数槽。
 * @param frame 当前调用帧。
 * @param value 返回值。
 */
void CHALI_ABI_CALL chali_abi_return_integer(ChaliAbiFrame *frame, int64_t value);

/**
 * @brief 同步调用 CHALI 函数值或闭包。
 * @param frame 当前调用帧。
 * @param function 函数标签地址或闭包句柄。
 * @param arguments 实参数组，可为空。
 * @param kinds 每个实参的值类别，可为空。
 * @param count 实参数量。
 * @param result 接收返回位模式。
 * @param resultIsFloat 接收返回值是否为浮点槽。
 * @return 正常返回时为 true；函数无效、参数数量错误或运行时失败时为 false。
 */
int CHALI_ABI_CALL chali_abi_invoke(const ChaliAbiFrame *frame, int64_t function, const int64_t *arguments,
                                    const unsigned char *kinds, int64_t count, int64_t *result, int *resultIsFloat);

/* ---------------------------------------------------------------------------
 *  宿主记录服务
 *
 * 有状态的 C 胶水（随机数生成器、文件句柄等）用它把宿主对象交给 VM 对象表
 * 托管，复制与释放语义由 C 侧回调提供。
 * ------------------------------------------------------------------------- */

/**
 * @brief 把 C 胶水创建的宿主记录交给当前运行时托管。
 * @param frame 当前调用帧。
 * @param typeName 记录类型名，用于诊断与借用时的身份校验；首次登记时复制，销毁代码须存活到记录释放。
 * @param record 记录指针，成功时所有权交给运行时。
 * @param destroy 销毁回调，运行时释放记录时调用；不得为空，也不得抛出异常。
 * @return 新句柄；失败返回零且调用方仍负责销毁记录。
 */
int64_t CHALI_ABI_CALL chali_abi_record_make(const ChaliAbiFrame *frame, const char *typeName, void *record,
                                             ChaliAbiDestroy destroy);

/**
 * @brief 创建可参与容器深复制及运行时快照的 C 宿主记录。
 * @param frame 当前调用帧。
 * @param typeName 类型身份名称；同名记录必须使用相同销毁及自动复制回调。
 * @param record 成功后交给运行时的独立记录。
 * @param destroy 非抛出销毁回调，不得为空。
 * @param clone 非抛出复制回调，返回独立副本；空回调表示不可自动复制。
 * @return 新句柄；失败为零且不接管 record。
 * @note 回调代码须存活到最后一条记录释放；副本不得引用源运行时的拥有值。
 */
int64_t CHALI_ABI_CALL chali_abi_record_make_clonable(const ChaliAbiFrame *frame, const char *typeName,
    void *record, ChaliAbiDestroy destroy, ChaliAbiClone clone);

/**
 * @brief 创建提供默认文本的 C 记录，可选提供独立复制，复用同一类型表与对象表。
 * @param frame 当前调用帧。
 * @param typeName 类型身份；同名类型的销毁、复制和格式化回调必须完全一致。
 * @param record 成功后转移给运行时的记录。
 * @param destroy 非抛出销毁回调，不得为空。
 * @param clone 非抛出独立复制回调，可为空表示不可复制。
 * @param format 非抛出文本查询／写入回调，不得为空。
 * @return 新句柄；失败为零，调用方继续拥有 record。
 * @note 需要 CHALI_ABI_CAP_RECORD_TEXT；所有回调代码须存活到最后一条记录释放。
 */
int64_t CHALI_ABI_CALL chali_abi_record_make_textual(const ChaliAbiFrame *frame, const char *typeName,
    void *record, ChaliAbiDestroy destroy, ChaliAbiClone clone, ChaliAbiFormat format);

/**
 * @brief 借用类型匹配的存活宿主记录。
 * @param frame 当前调用帧。
 * @param handle 记录句柄。
 * @param typeName 创建时使用的类型名。
 * @return 记录指针；句柄已释放或类型不匹配返回空指针。
 * @note 指针只在本次直通调用期间有效，不得跨调用保存。
 */
void *CHALI_ABI_CALL chali_abi_record_borrow(const ChaliAbiFrame *frame, int64_t handle, const char *typeName);

/**
 * @brief 探测并借用类型匹配的存活记录，不将句柄未找到视为错误。
 * @param frame 当前调用帧；帧无效仍报告服务错误。
 * @param handle 待检查的句柄。
 * @param typeName 记录类型名称。
 * @return 存活且类型匹配的记录，否则为空；指针沿用借用记录的有效期。
 * @note 未知类型、零值、已释放或类型不匹配不改变已有错误状态。
 */
void *CHALI_ABI_CALL chali_abi_record_probe(const ChaliAbiFrame *frame, int64_t handle, const char *typeName);

/**
 * @brief 用 C 侧复制回调生成独立记录副本。
 * @param frame 当前调用帧。
 * @param handle 待复制的存活句柄。
 * @param typeName 创建时使用的类型名。
 * @param clone 复制回调，返回新记录指针。
 * @return 新句柄；失败返回零。
 */
int64_t CHALI_ABI_CALL chali_abi_record_clone(const ChaliAbiFrame *frame, int64_t handle, const char *typeName,
                                              ChaliAbiClone clone);

/**
 * @brief 释放宿主记录及其资源。
 * @param frame 当前调用帧。
 * @param handle 记录句柄。
 * @return 接受释放时为 true；句柄无效时返回 false。
 */
int CHALI_ABI_CALL chali_abi_record_release(const ChaliAbiFrame *frame, int64_t handle);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // CHALI_ABI_H
