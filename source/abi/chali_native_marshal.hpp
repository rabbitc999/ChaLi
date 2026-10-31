/**
 * @file chali_native_marshal.hpp
 * @brief 生成 C 入口使用的私有封送实现，只通过 C 帧访问语言运行时。
 */
#pragma once
#include "chali_abi.h"
#include <array>
#include <cstring>
#include <cstdint>
#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

extern "C" {
/**
 * @brief 解析并保留原生库，失败以 C 状态及文本报告。
 * @param path UTF-8 库路径。
 * @param name 精确符号名。
 * @param error 可选的有界诊断。
 * @return 原生地址，失败为空。
 */
void *chali_host_abi_lookup(const char *path, const char *name, ChaliAbiError *error);
/**
 * @brief 取得已验证符号，失败进入运行时错误路径。
 * @param path UTF-8 库路径。
 * @param name 精确符号名。
 * @return 原生机器地址。
 */
void *chali_host_abi_symbol(const char *path, const char *name);
}

namespace chali_native_marshal
{
inline constexpr std::size_t maximumBytes = 64u * 1024u * 1024u;

/**
 * @brief 校验 ABI 元素数量及语言跨度。
 * @param length 元素数量。
 * @param stride 单个元素的语言跨度。
 * @return 有界元素数，非法时抛出私有异常。
 */
inline std::size_t checkedCount(std::int64_t length, std::size_t stride)
{
    if (length < 0 || !stride || static_cast<std::uint64_t>(length) > maximumBytes / stride)
        throw std::runtime_error("ABI buffer length outside supported range");
    return static_cast<std::size_t>(length);
}

/** @brief 从 C 帧借用服务，不拥有函数表或运行时对象。 */
struct Frame
{
    ChaliAbiFrame *raw;
    /**
     * @brief 读取精确整数种类参数。
     * @param index 参数下标。
     * @return 整数位模式；失败抛出私有异常。
     */
    std::int64_t integer(std::size_t index) const
    {
        std::int64_t value = 0;
        if (!chali_abi_argument_integer(raw, index, &value)) throw std::runtime_error("ABI integer argument invalid");
        return value;
    }
    /**
     * @brief 读取精确浮点种类参数。
     * @param index 参数下标。
     * @return binary64 值。
     */
    double real(std::size_t index) const
    {
        double value = 0;
        if (!chali_abi_argument_real(raw, index, &value)) throw std::runtime_error("ABI real argument invalid");
        return value;
    }
    /**
     * @brief 创建沿用调用方临时清理队列的语言存储。
     * @param bytes 存储字节数。
     * @return 逻辑地址，失败抛出私有异常。
     */
    std::int64_t temporary(std::size_t bytes) const
    {
        const auto address = chali_abi_tracked_temporary(raw, bytes);
        if (!address) throw std::runtime_error("ABI temporary allocation failed");
        return address;
    }
};

/**
 * @brief 把标量编码为语言八字节槽，不构造通用值对象。
 * @tparam T 原生标量类型。
 * @param value 原生值。
 * @return 语言位模式。
 */
template <typename T>
std::int64_t scalarBits(T value)
{
    if constexpr (std::is_floating_point_v<T>)
    {
        const double real = value;
        std::int64_t bits;
        std::memcpy(&bits, &real, sizeof(bits));
        return bits;
    }
    else
        return static_cast<std::int64_t>(value);
}

/**
 * @brief 把语言槽转换为原生标量。
 * @tparam T 原生标量类型。
 * @param bits 语言位模式。
 * @return 已按目标位宽转换的值。
 */
template <typename T>
T scalarValue(std::int64_t bits)
{
    if constexpr (std::is_floating_point_v<T>)
    {
        double real;
        std::memcpy(&real, &bits, sizeof(real));
        return static_cast<T>(real);
    }
    else
        return static_cast<T>(bits);
}

/**
 * @brief 标量逻辑存储；结构体由生成源按字段槽特化。
 * @tparam T 原生数值或纯值结构体类型。
 */
template <typename T>
struct Storage
{
    static constexpr std::size_t slots = 1;
    /**
     * @brief 从逻辑地址读取原生标量。
     * @param frame 当前 C 帧。
     * @param address 语言逻辑地址。
     * @return 读取值。
     */
    static T read(Frame frame, std::int64_t address)
    {
        if constexpr (std::is_same_v<T, std::uint8_t>)
        {
            T value{};
            if (!chali_abi_read(frame.raw, address, &value, sizeof(value))) throw std::runtime_error("ABI read outside VM storage");
            return value;
        }
        else
        {
            std::int64_t bits = 0;
            if (!chali_abi_read(frame.raw, address, &bits, sizeof(bits))) throw std::runtime_error("ABI read outside VM storage");
            return scalarValue<T>(bits);
        }
    }
    /**
     * @brief 写入标量；失败保留 C 帧错误，析构清理可安全调用。
     * @param frame 当前 C 帧。
     * @param address 语言逻辑地址。
     * @param value 原生值。
     * @return 写入成功时为 true。
     */
    static bool write(Frame frame, std::int64_t address, T value) noexcept
    {
        if constexpr (std::is_same_v<T, std::uint8_t>)
            return chali_abi_write(frame.raw, address, &value, sizeof(value)) != 0;
        else
        {
            const auto bits = scalarBits(value);
            return chali_abi_write(frame.raw, address, &bits, sizeof(bits)) != 0;
        }
    }
};

/**
 * @brief 在标量与纯值聚合的返回槽之间转换。
 * @tparam T 原生标量或聚合。
 * @param frame 当前 C 帧。
 * @param value 原生值。
 * @return 语言槽位模式或聚合地址。
 */
template <typename T>
std::int64_t encode(Frame frame, const T &value)
{
    if constexpr (std::is_arithmetic_v<T>)
        return scalarBits(value);
    else
    {
        const auto address = frame.temporary(Storage<T>::slots * 8);
        if (!Storage<T>::write(frame, address, value))
        {
            chali_abi_temporary_release(frame.raw, address);
            throw std::runtime_error("ABI write outside VM storage");
        }
        return address;
    }
}

/**
 * @brief 解码标量或纯值聚合的返回槽。
 * @tparam T 原生类型。
 * @param frame 当前 C 帧。
 * @param bits 位模式或逻辑地址。
 * @return 已复制的原生值。
 */
template <typename T>
T decode(Frame frame, std::int64_t bits)
{
    if constexpr (std::is_arithmetic_v<T>)
        return scalarValue<T>(bits);
    else
        return Storage<T>::read(frame, bits);
}

/**
 * @brief 单对象引用的共享原生副本，相同帧同类型地址保持别名。
 * @tparam T 不带 const 的元素类型。
 */
template <typename T>
struct PointerState
{
    Frame frame;
    std::int64_t address;
    T value;
    bool writable;
    PointerState *next = nullptr;
    std::weak_ptr<PointerState> owner;
    inline static thread_local PointerState *head = nullptr;
    /**
     * @brief 构造未链接的引用副本，失败不留下活动节点。
     * @param current 当前帧。
     * @param at 语言地址。
     * @param write 是否需要回写。
     */
    PointerState(Frame current, std::int64_t at, bool write) : frame(current), address(at), value(Storage<T>::read(current, at)), writable(write) {}
    /** @brief 回写共享副本并移除线程局部借用节点。 */
    ~PointerState()
    {
        if (writable) Storage<T>::write(frame, address, value);
        auto **entry = &head;
        while (*entry && *entry != this) entry = &(*entry)->next;
        if (*entry) *entry = next;
    }
};

/**
 * @brief 查找或构造本帧的共享引用副本。
 * @tparam T 不带 const 的元素类型。
 * @param frame 当前 C 帧。
 * @param address 非零逻辑地址。
 * @param writable 是否需要回写。
 * @return 原生副本的共同所有者。
 */
template <typename T>
std::shared_ptr<PointerState<T>> pointer(Frame frame, std::int64_t address, bool writable)
{
    if (!address) throw std::runtime_error("ABI reference argument is null");
    using State = PointerState<T>;
    for (auto *at = State::head; at; at = at->next)
        if (at->frame.raw == frame.raw && at->address == address)
        {
            auto result = at->owner.lock();
            result->writable = result->writable || writable;
            return result;
        }
    auto result = std::make_shared<State>(frame, address, writable);
    result->owner = result;
    result->next = State::head;
    State::head = result.get();
    return result;
}

/** @brief 生成源用来表示动态数组形参的私有标记。 */
template <typename T>
struct Array
{
};
/** @brief 生成源用来表示可空引用的私有标记。 */
template <typename T>
struct OptionalReference
{
};
/** @brief 生成源用来表示同步 C 回调签名的私有标记。 */
template <typename T>
struct Callback
{
};

/** @brief 按参数的原生类型持有一次封送结果。 */
template <typename T>
struct Argument
{
    T value;
    /**
     * @brief 从参数槽解码标量或聚合。
     * @param frame 当前 C 帧。
     * @param index 参数序号。
     */
    Argument(Frame frame, std::size_t index)
    {
        if constexpr (std::is_floating_point_v<T>)
            value = static_cast<T>(frame.real(index));
        else
            value = decode<T>(frame, frame.integer(index));
    }
    /** @brief 返回本次原生调用的值副本。
     * @return 原生参数。
     */
    T get() const { return value; }
    /** @brief 普通参数没有延迟回调错误。 */
    void check() const {}
};

/** @brief 引用参数持有共享原生对象副本。 */
template <typename T>
struct Argument<T *>
{
    std::shared_ptr<PointerState<std::remove_const_t<T>>> state;
    /**
     * @brief 解码必需的非零引用参数。
     * @param frame 当前 C 帧。
     * @param index 参数序号。
     */
    Argument(Frame frame, std::size_t index) : state(pointer<std::remove_const_t<T>>(frame, frame.integer(index), !std::is_const_v<T>)) {}
    /** @brief 取得原生引用地址。
     * @return 有效至本参数析构的机器指针。
     */
    T *get() const { return state ? &state->value : nullptr; }
    /** @brief 引用参数没有延迟回调错误。 */
    void check() const {}
};

/** @brief C 文本参数持有独立副本，允许同步重入导致语言缓冲重定位。 */
template <>
struct Argument<const char *>
{
    std::string value;
    /**
     * @brief 复制文本并拒绝内嵌零字节。
     * @param frame 当前 C 帧。
     * @param index 参数序号。
     */
    Argument(Frame frame, std::size_t index)
    {
        const unsigned char *data = nullptr;
        std::size_t length = 0;
        if (!chali_abi_bytes(frame.raw, frame.integer(index), &data, &length)) throw std::runtime_error("ABI string argument invalid");
        if (length && std::memchr(data, 0, length)) throw std::runtime_error("ABI C string contains NUL; use a byte buffer");
        if (length) value.assign(reinterpret_cast<const char *>(data), length);
    }
    /** @brief 取得以零结尾的独立副本。
     * @return 在本参数析构前有效的文本地址。
     */
    const char *get() const { return value.c_str(); }
    /** @brief 文本参数没有延迟回调错误。 */
    void check() const {}
};

/** @brief 可空引用参数保留 None 与 Value 的原有标签及回写语义。 */
template <typename T>
struct Argument<OptionalReference<T>>
{
    std::shared_ptr<PointerState<std::remove_const_t<T>>> state;
    /**
     * @brief 检查枚举标签，再建立非空引用的共享副本。
     * @param frame 当前 C 帧。
     * @param index 参数序号。
     */
    Argument(Frame frame, std::size_t index)
    {
        const auto base = frame.integer(index);
        if (!base) throw std::runtime_error("Option 引用缺少枚举存储");
        const auto tag = Storage<std::int64_t>::read(frame, base);
        if (tag == 1) return;
        if (tag != 0) throw std::runtime_error("Option 引用标签无效");
        const auto address = Storage<std::int64_t>::read(frame, base + 8);
        if (!address) throw std::runtime_error("Option::Value 不能包含零地址引用");
        state = pointer<std::remove_const_t<T>>(frame, address, !std::is_const_v<T>);
    }
    /** @brief 取得可空原生指针。
     * @return None 为 nullptr，Value 为共享副本地址。
     */
    T *get() const { return state ? &state->value : nullptr; }
    /** @brief 可空引用没有延迟回调错误。 */
    void check() const {}
};

/** @brief 动态数组参数共享原生副本和最终回写。 */
template <typename T>
struct Argument<Array<T>>
{
    /** @brief 一个帧内同地址数组的共享状态。 */
    struct State
    {
        Frame frame;
        std::int64_t address;
        std::unique_ptr<T[]> values;
        std::size_t count = 0;
        State *next = nullptr;
        std::weak_ptr<State> owner;
        inline static thread_local State *head = nullptr;
        /**
         * @brief 按语言长度头复制全部元素。
         * @param current 当前 C 帧。
         * @param at 非空数组元素首地址。
         */
        State(Frame current, std::int64_t at) : frame(current), address(at)
        {
            count = checkedCount(Storage<std::int64_t>::read(frame, address - 8), Storage<T>::slots * 8);
            if (count) values = std::make_unique<T[]>(count);
            for (std::size_t i = 0; i < count; ++i) values[i] = Storage<T>::read(frame, address + i * Storage<T>::slots * 8);
        }
        /** @brief 回写全部数组元素并移除共享节点。 */
        ~State()
        {
            for (std::size_t i = 0; i < count; ++i) Storage<T>::write(frame, address + i * Storage<T>::slots * 8, values[i]);
            auto **entry = &head;
            while (*entry && *entry != this) entry = &(*entry)->next;
            if (*entry) *entry = next;
        }
    };
    std::shared_ptr<State> state;
    /**
     * @brief 建立数组参数并保留相同参数间的别名。
     * @param frame 当前 C 帧。
     * @param index 参数序号。
     */
    Argument(Frame frame, std::size_t index)
    {
        const auto address = frame.integer(index);
        if (!address) throw std::runtime_error("ABI array argument is null");
        for (auto *at = State::head; at; at = at->next)
            if (at->frame.raw == frame.raw && at->address == address)
            {
                state = at->owner.lock();
                return;
            }
        state = std::make_shared<State>(frame, address);
        state->owner = state;
        state->next = State::head;
        State::head = state.get();
    }
    /** @brief 返回数组的原生连续副本。
     * @return 空数组为 nullptr，否则为元素首地址。
     */
    T *get() const { return state->values.get(); }
    /** @brief 数组参数没有延迟回调错误。 */
    void check() const {}
};

/** @brief 每个标量签名提供三十二个线程局部同步回调槽。 */
template <typename R, typename... A>
class Argument<Callback<R(A...)>>
{
    /** @brief 当前原生调用期间借用的语言函数值。 */
    struct State
    {
        Frame frame;
        std::int64_t function;
        std::exception_ptr error;
    } state;
    std::size_t slot = 32;
    inline static thread_local std::array<State *, 32> slots{};
    /**
     * @brief 调用指定槽内的语言函数值，并隔离异常。
     * @tparam I 跳板槽号。
     * @param values 原生标量参数。
     * @return 原生返回值；失败时为零值。
     */
    template <std::size_t I>
    static R invoke(A... values) noexcept
    {
        auto *current = slots[I];
        if (current && !current->error)
            try
            {
                const std::array<std::int64_t, sizeof...(A)> bits{ scalarBits(values)... };
                const std::array<unsigned char, sizeof...(A)> kinds{ static_cast<unsigned char>(std::is_floating_point_v<A>)... };
                std::int64_t result = 0;
                int real = 0;
                if (!chali_abi_invoke(current->frame.raw, current->function, bits.data(), kinds.data(), bits.size(), &result, &real))
                    throw std::runtime_error("ABI callback invocation failed");
                if constexpr (!std::is_void_v<R>)
                    return scalarValue<R>(result);
                else
                    return;
            } catch (...) { current->error = std::current_exception(); }
        if constexpr (!std::is_void_v<R>) return R{};
    }
    /**
     * @brief 为固定槽序列生成原生函数指针表。
     * @tparam I 槽号序列。
     * @return 与原生签名一致的跳板数组。
     */
    template <std::size_t... I>
    static auto table(std::index_sequence<I...>)
    { return std::array<R (*)(A...), sizeof...(I)>{ &invoke<I>... }; }

public:
    /**
     * @brief 保留一个同步回调槽，语言函数值不转换为机器地址。
     * @param frame 当前 C 帧。
     * @param index 参数序号。
     */
    Argument(Frame frame, std::size_t index) : state{ frame, frame.integer(index), {} }
    {
        if (!state.function) throw std::runtime_error("ABI callback is null");
        for (std::size_t i = 0; i < slots.size(); ++i)
            if (!slots[i])
            {
                slot = i;
                slots[i] = &state;
                return;
            }
        throw std::runtime_error("ABI synchronous callback capacity exhausted");
    }
    /** @brief 解除同步回调槽，调用结束后跳板不再访问语言值。 */
    ~Argument()
    {
        if (slot < slots.size()) slots[slot] = nullptr;
    }
    Argument(const Argument &) = delete;
    Argument &operator=(const Argument &) = delete;
    /** @brief 取得真正的原生跳板地址。
     * @return 当前签名及当前槽对应的 C 可调用函数地址。
     */
    auto get() const
    {
        static const auto functions = table(std::make_index_sequence<32>{});
        return functions[slot];
    }
    /** @brief 将原生调用期间延迟的回调错误交还入口错误边界。 */
    void check() const
    {
        if (state.error) std::rethrow_exception(state.error);
    }
};

/** @brief 自定义拥有返回值的释放守卫，不跨 C 边界传播析构异常。 */
struct OwnedReturn
{
    const void *pointer = nullptr;
    void (*release)(void *) = nullptr;
    /** @brief 参数及返回值封送结束后释放拥有返回值。 */
    ~OwnedReturn()
    {
        if (pointer && release) release(const_cast<void *>(pointer));
    }
};

/** @brief 导出参数的固定大小清理清单，不为登记再分配内存。 */
template <std::size_t N>
struct ExportArguments
{
    Frame frame;
    std::array<std::int64_t, N> addresses{};
    std::size_t count = 0;
    /**
     * @brief 记录已成功编码的聚合参数。
     * @param address 本帧拥有的参数存储。
     */
    void add(std::int64_t address) { addresses[count++] = address; }
    /** @brief 释放本帧参数副本，保留语言返回值的既有寿命。 */
    ~ExportArguments()
    {
        for (std::size_t i = 0; i < count; ++i) chali_abi_temporary_release(frame.raw, addresses[i]);
    }
};

/**
 * @brief 把原生单对象或数组复制到语言存储。
 * @tparam T 不带 const 的元素类型。
 * @param frame 当前 C 帧。
 * @param value 原生元素首地址。
 * @param length 元素数量。
 * @param array 是否写入动态数组的长度头。
 * @return 语言元素首地址。
 */
template <typename T>
std::int64_t copyPointer(Frame frame, const T *value, std::size_t length, bool array)
{
    const auto count = checkedCount(static_cast<std::int64_t>(length), Storage<T>::slots * 8);
    if (!array && !value) throw std::runtime_error("ABI returned null for nonnull reference");
    if (!value && count) throw std::runtime_error("ABI returned null for nonempty buffer");
    const auto base = frame.temporary(count * Storage<T>::slots * 8 + (array ? 8 : 0));
    if (array && !Storage<std::int64_t>::write(frame, base, count)) throw std::runtime_error("ABI write outside VM storage");
    const auto address = base + (array ? 8 : 0);
    for (std::size_t i = 0; i < count; ++i)
        if (!Storage<T>::write(frame, address + i * Storage<T>::slots * 8, value[i])) throw std::runtime_error("ABI write outside VM storage");
    return address;
}

/** @brief 标量及纯值聚合的结果槽写入。 */
template <typename T>
struct Result
{
    using Native = T;
    static constexpr std::size_t stride = 1;
    /**
     * @brief 写入原生调用结果。
     * @param frame 当前 C 帧。
     * @param value 原生结果。
     * @param count 已校验的结果计数，普通值忽略。
     */
    static void store(Frame frame, const T &value, std::size_t count)
    {
        (void)count;
        if constexpr (std::is_floating_point_v<T>)
            chali_abi_return_real(frame.raw, value);
        else
            chali_abi_return_integer(frame.raw, encode(frame, value));
    }
};

/** @brief void 原生调用没有返回封送。 */
template <>
struct Result<void>
{
    using Native = void;
    static constexpr std::size_t stride = 1;
};

/** @brief 非空单对象引用结果。 */
template <typename T>
struct Result<T *>
{
    using Native = T *;
    static constexpr std::size_t stride = 1;
    /**
     * @brief 复制单个原生对象后返回逻辑地址。
     * @param frame 当前 C 帧。
     * @param value 非空原生对象。
     * @param count 已校验的计数，单对象固定为一。
     */
    static void store(Frame frame, T *value, std::size_t count)
    {
        (void)count;
        chali_abi_return_integer(frame.raw, copyPointer<std::remove_const_t<T>>(frame, value, 1, false));
    }
};

/** @brief 原生文本结果复制后再归还自定义拥有指针。 */
template <>
struct Result<const char *>
{
    using Native = const char *;
    static constexpr std::size_t stride = 1;
    /**
     * @brief 按原契约把空指针作为空文本，限制非空文本长度。
     * @param frame 当前 C 帧。
     * @param value 以零结尾的文本或 nullptr。
     * @param count 普通文本忽略计数。
     */
    static void store(Frame frame, const char *value, std::size_t count)
    {
        (void)count;
        std::size_t length = 0;
        if (value)
            while (length < maximumBytes && value[length]) ++length;
        if (length == maximumBytes) throw std::runtime_error("ABI C string exceeds maximum length");
        const auto address = chali_abi_string(frame.raw, value, length);
        if (!address) throw std::runtime_error("ABI string allocation failed");
        chali_abi_return_integer(frame.raw, address);
    }
};

/** @brief 动态数组结果保留长度头和逐元素位宽转换。 */
template <typename T>
struct Result<Array<T>>
{
    using Native = T *;
    static constexpr std::size_t stride = Storage<T>::slots * 8;
    /**
     * @brief 复制给定数量的原生数组元素。
     * @param frame 当前 C 帧。
     * @param value 原生元素首地址，空数组允许 nullptr。
     * @param count 已校验的元素数量。
     */
    static void store(Frame frame, T *value, std::size_t count)
    { chali_abi_return_integer(frame.raw, copyPointer<T>(frame, value, count, true)); }
};

/** @brief 可空引用结果复制元素后构造语言枚举存储。 */
template <typename T>
struct Result<OptionalReference<T>>
{
    using Native = T *;
    static constexpr std::size_t stride = 1;
    /**
     * @brief 构造 Value 或 None 引用结果。
     * @param frame 当前 C 帧。
     * @param value 可空原生指针。
     * @param count 单对象引用忽略计数。
     */
    static void store(Frame frame, T *value, std::size_t count)
    {
        (void)count;
        const auto address = value ? copyPointer<std::remove_const_t<T>>(frame, value, 1, false) : 0;
        const auto base = frame.temporary(16);
        if (!Storage<std::int64_t>::write(frame, base, value ? 0 : 1) || !Storage<std::int64_t>::write(frame, base + 8, address))
            throw std::runtime_error("ABI write outside VM storage");
        chali_abi_return_integer(frame.raw, base);
    }
};
}  // namespace chali_native_marshal
