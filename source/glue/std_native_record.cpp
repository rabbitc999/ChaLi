#include "std_native_record.hpp"

namespace native_record_glue
{

/**
 * @brief 按通用记录协议复制对象，供 opcode 的 copy 方法绑定。
 * @param state 当前胶水调用状态。
 * @param arguments 包含待复制句柄。
 * @return 独立副本句柄。
 */
Value copy(State &state, const Memory::Vector<Value> &arguments)
{
    return Value::fromInt(state.host.cloneNativeRecord(arguments[0].asInt()));
}

/**
 * @brief 严格释放通用记录，供 opcode 的 drop 方法绑定。
 * @param state 当前胶水调用状态。
 * @param arguments 包含待释放的存活句柄。
 * @return 零占位值。
 */
Value drop(State &state, const Memory::Vector<Value> &arguments)
{
    if (!state.host.releaseNativeRecord(arguments[0].asInt()))
        state.error = "[CHL1205] drop 的对象已经释放或句柄无效";
    return Value::fromInt(0);
}

/**
 * @brief 读取原生参数的整数位模式。
 * @param value 原生参数。
 * @return 整数值或浮点向零截断结果。
 */
std::int64_t nativeInt(const Value &value)
{ return value.asInt(); }

/**
 * @brief 从当前运行时按其文本协议读取字符串。
 * @param state 胶水状态。
 * @param address 文本地址。
 * @param out 接收读取结果。
 * @return 读取成功时为 true。
 */
bool nativeReadCString(State &state, std::size_t address, Memory::String *out)
{
    return state.host.readString(static_cast<std::int64_t>(address), *out);
}

/**
 * @brief 创建交给语言调用方拥有的字符串副本。
 * @param state 胶水状态。
 * @param text 要复制的文本。
 * @return 当前运行时中的文本句柄。
 */
std::int64_t nativeAllocString(State &state, const Memory::String &text)
{
    return state.host.makeString(text);
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
                             const stackvm::NativeRecordType &type)
{
    if (context.nativeRecord(reference, type)) return reference;
    char bytes[8];
    if (!context.readBytes(reference, bytes, sizeof(bytes)))
    {
        context.fail("opcode 引用槽不可读");
        return 0;
    }
    std::uint64_t handle = 0;
    for (unsigned index = 0; index < 8; ++index)
        handle |= static_cast<std::uint64_t>(static_cast<unsigned char>(bytes[index])) << (index * 8);
    return static_cast<std::int64_t>(handle);
}

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
          Value (*operation)(State &, const Memory::Vector<Value> &), Memory::String *error, bool referenceFirst)
{
    return registry.add(name, arity, [operation, referenceFirst](stackvm::NativeCallContext &context, const Memory::Vector<Value> &arguments) {
        State state{ context, {} };
        Memory::Vector<Value> values;
        const auto *input = &arguments;
        if (referenceFirst)
        {
            values = arguments;
            input = &values;
            char bytes[8];
            if (values.empty() || !context.readBytes(values[0].asInt(), bytes, sizeof(bytes)))
            {
                context.fail("胶水对象引用指向的句柄槽无效");
                return Value::fromInt(0);
            }
            std::uint64_t handle = 0;
            for (unsigned index = 0; index < 8; ++index)
                handle |= static_cast<std::uint64_t>(static_cast<unsigned char>(bytes[index])) << (index * 8);
            values[0] = Value::fromInt(static_cast<std::int64_t>(handle));
        }
        auto result = operation(state, *input);
        if (!state.error.empty()) context.fail(state.error);
        return result; }, error);
}

}  // namespace native_record_glue
