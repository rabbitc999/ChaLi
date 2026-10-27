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
    ChaliAbiRuntime *runtime;        ///< 本次调用的运行时句柄。
    const int64_t *slots;            ///< 实参原始位模式，长度 `count`。
    const unsigned char *kinds;      ///< 每个实参的值类别，长度 `count`。
    int64_t count;                   ///< 实参数量。
    int64_t result;                  ///< 直通入口写回的返回位模式。
    int resultIsFloat;               ///< 返回值是否为浮点槽。
} ChaliAbiFrame;

/**
 * @brief 直通入口的统一签名：入参出参都在帧上，不分配、不抛异常。
 */
typedef void (*ChaliAbiEntry)(ChaliAbiFrame *frame);

/**
 * @brief 直通入口表项。
 */
typedef struct ChaliAbiRecord
{
    const char *name;   ///< 原生函数完整名称，与注册表登记名一致。
    ChaliAbiEntry entry;///< 直通入口。
    int arity;          ///< 固定实参数量；-1 表示由入口自行检查的变参。
} ChaliAbiRecord;

/**
 * @brief 取直通入口表。
 * @param count 接收表项数量，可为空。
 * @return 静态常量表首地址，按名称升序排列；生命周期与进程相同。
 */
const ChaliAbiRecord *chali_abi_records(size_t *count);

/**
 * @brief 把原生函数名解析成直通入口。
 * @param name 原生函数完整名称，空指针时返回空。
 * @param arity 接收该入口声明的实参数量，可为空。
 * @return 直通入口；名字不在表中返回空指针。
 * @note 解析是纯表查找，不构造任何字符串对象，适合在绑定期批量完成。
 */
ChaliAbiEntry chali_abi_resolve(const char *name, int *arity);

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
typedef const ChaliAbiRecord *(*ChaliAbiModule)(size_t *count);

/**
 * @brief `std::lang` 标准错误流输出：__std_stderr_write / __std_eprint / __std_eprintln。
 */
const ChaliAbiRecord *abi_print_records(size_t *count);

/**
 * @brief `std::math` 标量数学函数（含 `__xxx64` 双精度变体），共 36 项。
 */
const ChaliAbiRecord *abi_math_records(size_t *count);

/**
 * @brief `std::clock` 单调时钟：nowNanos / nowMs。
 */
const ChaliAbiRecord *abi_clock_records(size_t *count);

/**
 * @brief `std::random` 生成器：创建、取值、复制与释放，共 9 项。
 */
const ChaliAbiRecord *abi_random_records(size_t *count);

/**
 * @brief 字符串构造：浮点格式化入口 `__std_str_of_flt`。
 */
const ChaliAbiRecord *abi_string_records(size_t *count);


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
int64_t chali_abi_integer(const ChaliAbiFrame *frame, int64_t index);

/**
 * @brief 读取实参槽的双精度值。
 * @param frame 当前调用帧。
 * @param index 实参下标，越界时返回零。
 * @return 该槽按 IEEE-754 解释的双精度值。
 */
double chali_abi_real(const ChaliAbiFrame *frame, int64_t index);

/**
 * @brief 取得 VM 字符串或字节块的零拷贝视图。
 * @param frame 当前调用帧。
 * @param address 字符串或字节块的 VM 地址，零地址得到空视图。
 * @param data 接收首字节地址，可为空。
 * @param length 接收字节长度，可为空。
 * @return 地址可读且长度有效时为 true，否则为 false 并记录运行时诊断。
 * @note 返回的指针只在本次直通调用期间有效；C 胶水不得保存或释放它。
 */
int chali_abi_bytes(const ChaliAbiFrame *frame, int64_t address, const unsigned char **data, size_t *length);

/**
 * @brief 在当前运行时分配一个 VM 字符串并返回其地址。
 * @param frame 当前调用帧。
 * @param data UTF-8 字节，允许包含零字节。
 * @param length 字节长度。
 * @return 新字符串地址；分配失败返回零并记录运行时诊断。
 */
int64_t chali_abi_string(const ChaliAbiFrame *frame, const char *data, size_t length);

/**
 * @brief 分配随本次运行结束回收的临时字节块。
 * @param frame 当前调用帧。
 * @param bytes 字节数。
 * @return 块地址；失败返回零。
 */
int64_t chali_abi_temporary(const ChaliAbiFrame *frame, size_t bytes);

/**
 * @brief 把文本追加到本次运行的标准输出桶并下沉给宿主。
 * @param frame 当前调用帧。
 * @param data 待输出的字节。
 * @param length 字节长度。
 */
void chali_abi_emit(const ChaliAbiFrame *frame, const char *data, size_t length);

/**
 * @brief 把文本写入标准错误流并立即刷新。
 * @param frame 当前调用帧。
 * @param data 待输出的字节。
 * @param length 字节长度。
 * @param newline 非零时在文本后追加换行。
 * @note 整次写入（含换行）在同一把锁内完成，多线程下不会与其它标准错误输出交错。
 */
void chali_abi_emit_error(const ChaliAbiFrame *frame, const char *data, size_t length, int newline);

/**
 * @brief 报告本次直通调用失败。
 * @param frame 当前调用帧。
 * @param message 需要传播给 VM 的错误文本，空指针按空串处理。
 * @note 只记录诊断，不展开栈；入口随后应尽快返回。
 */
void chali_abi_fail(const ChaliAbiFrame *frame, const char *message);

/**
 * @brief 把双精度值写成返回浮点槽。
 * @param frame 当前调用帧。
 * @param value 返回值。
 */
void chali_abi_return_real(ChaliAbiFrame *frame, double value);

/**
 * @brief 把 64 位整数写成返回整数槽。
 * @param frame 当前调用帧。
 * @param value 返回值。
 */
void chali_abi_return_integer(ChaliAbiFrame *frame, int64_t value);

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
int chali_abi_invoke(const ChaliAbiFrame *frame, int64_t function, const int64_t *arguments,
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
 * @param typeName 记录类型名，用于诊断与借用时的身份校验；须为静态字符串。
 * @param record 记录指针，成功时所有权交给运行时。
 * @param destroy 销毁回调，运行时释放记录时调用；不得为空。
 * @return 新句柄；失败返回零且调用方仍负责销毁记录。
 */
int64_t chali_abi_record_make(const ChaliAbiFrame *frame, const char *typeName, void *record,
                              void (*destroy)(void *));

/**
 * @brief 借用类型匹配的存活宿主记录。
 * @param frame 当前调用帧。
 * @param handle 记录句柄。
 * @param typeName 创建时使用的类型名。
 * @return 记录指针；句柄已释放或类型不匹配返回空指针。
 * @note 指针只在本次直通调用期间有效，不得跨调用保存。
 */
void *chali_abi_record_borrow(const ChaliAbiFrame *frame, int64_t handle, const char *typeName);

/**
 * @brief 用 C 侧复制回调生成独立记录副本。
 * @param frame 当前调用帧。
 * @param handle 待复制的存活句柄。
 * @param typeName 创建时使用的类型名。
 * @param clone 复制回调，返回新记录指针。
 * @return 新句柄；失败返回零。
 */
int64_t chali_abi_record_clone(const ChaliAbiFrame *frame, int64_t handle, const char *typeName,
                               void *(*clone)(const void *));

/**
 * @brief 释放宿主记录及其资源。
 * @param frame 当前调用帧。
 * @param handle 记录句柄。
 * @return 接受释放时为 true；句柄无效时返回 false。
 */
int chali_abi_record_release(const ChaliAbiFrame *frame, int64_t handle);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // CHALI_ABI_H
