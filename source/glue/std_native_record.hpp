#ifndef CHALI_GLUE_NATIVE_RECORD_HPP
#define CHALI_GLUE_NATIVE_RECORD_HPP

#include "stackvm.hpp"

/**
 * @brief 基于公开运行时接口访问不透明宿主记录的胶水辅助层。
 */
namespace native_record_glue
{
using Value = stackvm::NativeValue;

/**
 * @brief 在一次胶水操作内累积诊断，绑定入口统一交回运行时。
 */
struct State
{
    stackvm::NativeCallContext &host;  ///< 仅在本次调用内有效的宿主上下文。
    Memory::String error;              ///< 当前操作的原始诊断。
};

/**
 * @brief 按通用记录协议复制对象，供 opcode 的 copy 方法绑定。
 * @param state 当前胶水调用状态。
 * @param arguments 包含待复制句柄。
 * @return 独立副本句柄。
 */
Value copy(State &state, const Memory::Vector<Value> &arguments);

/**
 * @brief 严格释放通用记录，供 opcode 的 drop 方法绑定。
 * @param state 当前胶水调用状态。
 * @param arguments 包含待释放的存活句柄。
 * @return 零占位值。
 */
Value drop(State &state, const Memory::Vector<Value> &arguments);

/**
 * @brief 读取原生参数的整数位模式。
 * @param value 原生参数。
 * @return 整数值或浮点向零截断结果。
 */
std::int64_t nativeInt(const Value &value);

/**
 * @brief 从当前运行时按其文本协议读取字符串。
 * @param state 胶水状态。
 * @param address 文本地址。
 * @param out 接收读取结果。
 * @return 读取成功时为 true。
 */
bool nativeReadCString(State &state, std::size_t address, Memory::String *out);

/**
 * @brief 创建交给语言调用方拥有的字符串副本。
 * @param state 胶水状态。
 * @param text 要复制的文本。
 * @return 当前运行时中的文本句柄。
 */
std::int64_t nativeAllocString(State &state, const Memory::String &text);

/**
 * @brief 校验记录类型并借用宿主记录。
 * @tparam T 胶水自己的记录布局。
 * @param state 胶水状态。
 * @param handle 原生参数中的对象句柄。
 * @param type 预期宿主描述符。
 * @param who 诊断中的操作名称。
 * @return 有效记录，失败返回空并保存诊断。
 */
template <class T>
T *record(State &state, const Value &handle, const stackvm::NativeRecordType &type, const char *who)
{
    auto *result = static_cast<T *>(state.host.nativeRecord(handle.asInt(), type));
    if (!result)
        state.error = Memory::String(who) + " 用的 " + type.name + " 已经释放过了（句柄 " + Memory::toString(handle.asInt()) + "）";
    return result;
}

/**
 * @brief 从 opcode 接收者或语言引用槽取得宿主记录句柄。
 * @param context 当前胶水调用上下文。
 * @param reference 直接传递的 opcode 接收者，或保存该值的语言引用槽地址。
 * @param type 目标记录的生命周期描述符。
 * @return 记录句柄；引用不可读时报告错误并返回零。
 * @note opcode 方法的 self 使用直接句柄，普通字段引用使用内存槽；两者都保留前端的可写性检查。
 */
std::int64_t referenceHandle(stackvm::NativeCallContext &context, std::int64_t reference,
                             const stackvm::NativeRecordType &type);

/**
 * @brief 注册只通过公开上下文访问宿主记录的原生操作。
 * @param registry 目标胶水注册表。
 * @param name 语言侧绑定名称。
 * @param arity 实参数量。
 * @param operation 操作实现。
 * @param error 注册失败的诊断输出。
 * @param referenceFirst 首个参数是否为保存对象句柄的语言引用槽。
 * @return 注册成功时为 true。
 */
bool bind(stackvm::NativeRegistry &registry, const char *name, std::size_t arity,
          Value (*operation)(State &, const Memory::Vector<Value> &), Memory::String *error, bool referenceFirst = false);

/**
 * @brief 浮点 → 文本：默认保留 3 位小数（语言的默认），去掉尾随的 0。
 *
 * 与 `%f` 的默认精度保持一致：`3.5` 打成 `3.5`（不是 `3.500`），`2.0` 打成 `2.0`。
 * @param value 值。
 * @param precision 小数位数。
 * @return 文本。
 */
Memory::String nativeFloatText(double value, int precision);

/**
 * @brief `__std_str_of_flt(值) -> 新串地址`（默认 3 位小数，`精度` 可覆盖）。
 * @param ctx 原生调用上下文，提供内存访问与错误报告。
 * @param args 按接口约定顺序传入的实参列表。
 * @retval 封装为原生值的运行时结果。
 */
Value strOfFlt(State &ctx, const Memory::Vector<Value> &args);

/**
 * @brief 注册浮点数的宿主文本格式边界。
 * @param registry 目标原生函数表。
 * @param error 注册失败的原因。
 * @return 浮点格式入口绑定成功时返回 true。
 */
bool bindString(stackvm::NativeRegistry &registry, Memory::String *error = nullptr);
}  // namespace native_record_glue

#endif
