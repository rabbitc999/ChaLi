/** @file abi_structured_data.cpp @brief JSON／TOML 拥有值的纯 C 帧边界与私有解析内核。 */
#include "chali_standard.h"
#include "../json/json.hpp"
#include "../toml/toml.hpp"
#include <cmath>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <type_traits>

namespace
{
/** @brief JSON 解析、节点分类及序列化策略。 */
struct JsonTraits
{
    using Value = json::Value;
    static constexpr const char *name = "std::json::JsonValue";
    /** @brief 解析完整文本。
     * @param text 输入字节。
     * @return 根节点与诊断。
     */
    static auto parse(std::string_view text) { return json::parse(text); }
    /** @brief 序列化节点。
     * @param value 只读节点。
     * @return JSON 文本。
     */
    static std::string dump(const Value &value) { return value.dump(); }
    /** @brief JSON 不具有日期节点。
     * @param value 未使用的节点。
     * @return 空文本。
     */
    static std::string dateTime(const Value &value)
    {
        (void)value;
        return {};
    }
};
/** @brief TOML 解析、节点分类及序列化策略。 */
struct TomlTraits
{
    using Value = toml::Value;
    static constexpr const char *name = "std::toml::TomlValue";
    /** @brief 解析完整文本。
     * @param text 输入字节。
     * @return 根节点与诊断。
     */
    static auto parse(std::string_view text) { return toml::parse(text); }
    /** @brief 按节点类型选择整表或行内序列化。
     * @param value 只读节点。
     * @return TOML 文本。
     */
    static std::string dump(const Value &value) { return value.isTable() ? value.dump() : value.inlineDump(); }
    /** @brief 提取日期时间。
     * @param value 只读节点。
     * @return 日期文本或空文本。
     */
    static std::string dateTime(const Value &value)
    {
        auto date = value.asDateTime();
        return date ? date->toString() : std::string();
    }
};
/** @brief 独立拥有的子树及解析诊断。
 * @tparam Traits 解析格式策略。
 */
template <class Traits>
struct OwnedValue
{
    std::optional<typename Traits::Value> value;
    std::string error;
};
/** @brief 用 C 签名销毁拥有节点。
 * @tparam Traits 格式策略。
 * @param value 独立记录。
 */
template <class Traits>
void CHALI_ABI_CALL destroyValue(void *value)
{ delete static_cast<OwnedValue<Traits> *>(value); }
/** @brief 不跨 C 边界抛出异常的独立快照复制。
 * @tparam Traits 格式策略。
 * @param value 只读源记录。
 * @return 独立副本，失败为空。
 */
template <class Traits>
void *CHALI_ABI_CALL cloneValue(const void *value)
try
{
    return new OwnedValue<Traits>(*static_cast<const OwnedValue<Traits> *>(value));
} catch (...) { return nullptr; }
/** @brief 完整构造记录后才向运行时转移所有权。
 * @tparam Traits 格式策略。
 * @param frame 当前 C 帧。
 * @param value 独立节点或空状态。
 * @param error 解析诊断。
 * @return 拥有句柄，失败为零。
 */
template <class Traits>
int64_t makeValue(const ChaliAbiFrame *frame,
                  std::optional<typename Traits::Value> value = {}, std::string error = {})
{
    auto owned = std::make_unique<OwnedValue<Traits>>();
    owned->value = std::move(value);
    owned->error = std::move(error);
    auto handle = chali_abi_record_make_clonable(frame, Traits::name, owned.get(), destroyValue<Traits>, cloneValue<Traits>);
    if (handle) owned.release();
    return handle;
}
/** @brief 在可能重入或分配前复制完整字符串实参。
 * @param frame 当前帧。
 * @param handle 字符串句柄。
 * @param output 接收独立字节。
 * @return 读取成功为真。
 */
bool readText(const ChaliAbiFrame *frame, int64_t handle, std::string &output)
{
    const unsigned char *data = nullptr;
    size_t length = 0;
    if (!chali_abi_bytes(frame, handle, &data, &length)) return false;
    output.assign(data ? reinterpret_cast<const char *>(data) : "", length);
    return true;
}
/** @brief 结构化节点的固定操作，编译实例选择入口而不建立运行时封送器。 */
enum class Operation
{
    New,
    Copy,
    Drop,
    Parse,
    Valid,
    Error,
    Close,
    Kind,
    Size,
    Int,
    Float,
    Bool,
    TextAvailable,
    String,
    Bytes,
    DateTime,
    Dump,
    Find,
    FindBytes,
    At,
    KeyAvailable,
    Key,
    KeyBytes
};
/** @brief 执行固定格式及操作的 C 帧入口，异常统一转为错误通道。
 * @tparam Traits 格式内核策略。
 * @tparam op 编译期固定操作。
 * @param frame 借用帧及返回槽。
 */
template <class Traits, Operation op>
void CHALI_ABI_CALL valueEntry(ChaliAbiFrame *frame)
try
{
    using Value = typename Traits::Value;
    if constexpr (op == Operation::New)
    {
        chali_abi_return_integer(frame, makeValue<Traits>(frame));
        return;
    }
    int64_t handle = 0;
    if (!chali_abi_argument_integer(frame, 0, &handle)) return;
    if constexpr (op == Operation::Parse)
    {
        std::string text;
        if (!readText(frame, handle, text)) return;
        auto parsed = Traits::parse(text);
        std::string error;
        if (!parsed.value) error = parsed.diagnostics.empty() ? "解析失败" : parsed.diagnostics.front().toString();
        chali_abi_return_integer(frame, makeValue<Traits>(frame, std::move(parsed.value), std::move(error)));
        return;
    }
    if constexpr (op == Operation::Drop)
    {
        if (!chali_abi_record_release(frame, handle))
            chali_abi_fail(frame, "[CHL1205] drop 的对象已经释放或句柄无效");
        chali_abi_return_integer(frame, 0);
        return;
    }
    if constexpr (op == Operation::Copy)
    {
        chali_abi_return_integer(frame, chali_abi_record_clone(frame, handle, Traits::name, cloneValue<Traits>));
        return;
    }
    if constexpr (op == Operation::Valid || op == Operation::Close)
        if (!handle)
        {
            chali_abi_return_integer(frame, 0);
            return;
        }
    std::string key;
    int64_t ignored = 0, index = 0;
    if constexpr (op >= Operation::Kind)
        if (!chali_abi_argument_integer(frame, 1, &ignored)) return;
    if constexpr (op >= Operation::Find)
    {
        if (!chali_abi_argument_integer(frame, 2, &index)) return;
        if constexpr (op == Operation::Find)
        {
            if (!readText(frame, index, key)) return;
        }
        if constexpr (op == Operation::FindBytes)
        {
            size_t length = 0;
            if (!chali_abi_list_bytes(frame, index, nullptr, 0, &length)) return;
            std::vector<unsigned char> bytes(length);
            if (!chali_abi_list_bytes(frame, index, bytes.data(), bytes.size(), &length)) return;
            key.assign(bytes.empty() ? "" : reinterpret_cast<const char *>(bytes.data()), bytes.size());
        }
    }
    auto *owned = static_cast<OwnedValue<Traits> *>(chali_abi_record_borrow(frame, handle, Traits::name));
    if (!owned)
    {
        chali_abi_fail(frame, "结构化数据：值已经释放或类型无效");
        return;
    }
    if constexpr (op == Operation::Valid)
    {
        chali_abi_return_integer(frame, owned->value.has_value());
        return;
    }
    if constexpr (op == Operation::Error)
    {
        chali_abi_return_integer(frame, chali_abi_string(frame, owned->error.c_str(), owned->error.size()));
        return;
    }
    if constexpr (op == Operation::Close)
    {
        bool valid = owned->value.has_value();
        owned->value.reset();
        chali_abi_return_integer(frame, valid);
        return;
    }
    if (!owned->value)
    {
        chali_abi_fail(frame, "结构化数据：值已经关闭或未初始化");
        return;
    }
    const Value &value = *owned->value;
    if constexpr (op == Operation::Kind)
    {
        int64_t kind = value.isBool() ? 1 : value.isInt() ? 2 :
                                        value.isDouble()  ? 3 :
                                        value.isString()  ? 4 :
                                        value.isArray()   ? 5 :
                                        value.isObject()  ? 6 :
                                                            (std::is_same_v<Traits, JsonTraits> ? 0 : 7);
        chali_abi_return_integer(frame, kind);
    }
    if constexpr (op == Operation::Size)
        chali_abi_return_integer(frame, value.asArray() ? value.asArray()->size() : value.asObject() ? value.asObject()->size() :
                                                                                                       0);
    if constexpr (op == Operation::Int) chali_abi_return_integer(frame, value.asInt().value_or(0));
    if constexpr (op == Operation::Float) chali_abi_return_real(frame, value.asDouble().value_or(0.0));
    if constexpr (op == Operation::Bool) chali_abi_return_integer(frame, value.asBool().value_or(false));
    if constexpr (op == Operation::TextAvailable)
    {
        auto text = value.asString();
        chali_abi_return_integer(frame, text && text->find('\0') == std::string_view::npos);
    }
    if constexpr (op == Operation::String || op == Operation::Bytes)
    {
        auto text = value.asString();
        const char *data = text ? text->data() : "";
        size_t size = text ? text->size() : 0;
        if constexpr (op == Operation::String)
            chali_abi_return_integer(frame, chali_abi_string_bytes(frame, data, size));
        else
            chali_abi_return_integer(frame, chali_abi_list_from_bytes(frame, reinterpret_cast<const unsigned char *>(data), size));
    }
    if constexpr (op == Operation::Dump || op == Operation::DateTime)
    {
        auto text = op == Operation::Dump ? Traits::dump(value) : Traits::dateTime(value);
        chali_abi_return_integer(frame, chali_abi_string(frame, text.data(), text.size()));
    }
    if constexpr (op == Operation::Find || op == Operation::FindBytes || op == Operation::At)
    {
        const Value *child = nullptr;
        if constexpr (op == Operation::At)
        {
            if (index >= 0)
            {
                if (auto *array = value.asArray(); array && uint64_t(index) < array->size()) child = &(*array)[size_t(index)];
                if (auto *object = value.asObject(); object && uint64_t(index) < object->size()) child = &(object->begin() + index)->value;
            }
        }
        else
            child = value.find(std::string_view(key.data(), key.size()));
        chali_abi_return_integer(frame, makeValue<Traits>(frame, child ? std::optional<Value>(*child) : std::nullopt));
    }
    if constexpr (op == Operation::Key || op == Operation::KeyBytes || op == Operation::KeyAvailable)
    {
        auto *object = value.asObject();
        const std::string *name = object && index >= 0 && uint64_t(index) < object->size() ? &(object->begin() + index)->key : nullptr;
        if constexpr (op == Operation::KeyAvailable) chali_abi_return_integer(frame, name && name->find('\0') == std::string::npos);
        if constexpr (op == Operation::Key) chali_abi_return_integer(frame, chali_abi_string_bytes(frame, name ? name->data() : "", name ? name->size() : 0));
        if constexpr (op == Operation::KeyBytes) chali_abi_return_integer(frame, chali_abi_list_from_bytes(frame,
                                                                                                           reinterpret_cast<const unsigned char *>(name ? name->data() : ""), name ? name->size() : 0));
    }
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "结构化数据内核失败");
}
/**
 * @brief 线性校验十进制整数或小数，避免正则递归限制大数长度。
 * @param value 不含前后空白的数值文本。
 * @param decimal 是否允许小数部分与十进制指数。
 * @return 文本完整匹配 JSON 数值语法时返回 true。
 * @note 小数指数的绝对值沿用 BigDecimal 后端的一百万上限。
 */
bool numberText(std::string_view value, bool decimal)
{
    std::size_t at = 0;
    const auto digit = [&](std::size_t index) { return index < value.size() && value[index] >= '0' && value[index] <= '9'; };
    if (at < value.size() && value[at] == '-') ++at;
    if (!digit(at)) return false;
    if (value[at] == '0')
        ++at;
    else
        while (digit(at)) ++at;
    if (decimal && at < value.size() && value[at] == '.')
    {
        ++at;
        if (!digit(at)) return false;
        while (digit(at)) ++at;
    }
    if (decimal && at < value.size() && (value[at] == 'e' || value[at] == 'E'))
    {
        ++at;
        if (at < value.size() && (value[at] == '+' || value[at] == '-')) ++at;
        if (!digit(at)) return false;
        std::uint64_t exponent = 0;
        while (digit(at))
        {
            exponent = exponent * 10 + static_cast<unsigned>(value[at++] - '0');
            if (exponent > 1000000) return false;
        }
    }
    return at == value.size();
}

/** @brief 反射序列化的固定 C 操作。 */
enum class Serde
{
    Compiled,
    Quote,
    Float,
    Finite,
    Number
};
/**
 * @brief 执行 JSON 序列化所需的标量内核。
 * @tparam op 编译期操作。
 * @param frame 实参与返回帧。
 */
template <Serde op>
void CHALI_ABI_CALL serdeEntry(ChaliAbiFrame *frame)
try
{
    if constexpr (op == Serde::Compiled)
    {
#if CHALI_WITH_REFLECTION && STACKVM_WITH_REFLECTION
        chali_abi_return_integer(frame, 1);
#else
        chali_abi_return_integer(frame, 0);
#endif
    }
    if constexpr (op == Serde::Quote || op == Serde::Number)
    {
        int64_t handle = 0;
        std::string text;
        if (!chali_abi_argument_integer(frame, 0, &handle) || !readText(frame, handle, text)) return;
        if constexpr (op == Serde::Quote)
        {
            auto output = json::Value(std::move(text)).dump();
            chali_abi_return_integer(frame, chali_abi_string(frame, output.data(), output.size()));
        }
        else
        {
            int64_t decimal = 0;
            if (!chali_abi_argument_integer(frame, 1, &decimal)) return;
            chali_abi_return_integer(frame, numberText(text, decimal != 0));
        }
    }
    if constexpr (op == Serde::Float || op == Serde::Finite)
    {
        double value = 0;
        if (!chali_abi_argument_real(frame, 0, &value)) return;
        if constexpr (op == Serde::Finite)
            chali_abi_return_integer(frame, std::isfinite(value));
        else
        {
            auto output = json::Value(value).dump();
            chali_abi_return_integer(frame, chali_abi_string(frame, output.data(), output.size()));
        }
    }
} catch (const std::exception &error) { chali_abi_fail(frame, error.what()); } catch (...)
{
    chali_abi_fail(frame, "JSON 序列化内核失败");
}
}  // namespace

/**
 * @brief 取得 JSON 拥有值的只读 C 入口表。
 * @param count 接收入口数量，可为空。
 * @return 生命周期覆盖进程的表。
 */
extern "C" const ChaliAbiRecord *CHALI_ABI_CALL abi_json_records(size_t *count)
{
    static const ChaliAbiRecord records[] = {
        { "std::json::__at", valueEntry<JsonTraits, Operation::At>, 3 },
        { "std::json::__bool", valueEntry<JsonTraits, Operation::Bool>, 2 },
        { "std::json::__bytes", valueEntry<JsonTraits, Operation::Bytes>, 2 },
        { "std::json::__close", valueEntry<JsonTraits, Operation::Close>, 1 },
        { "std::json::__dateTime", valueEntry<JsonTraits, Operation::DateTime>, 2 },
        { "std::json::__dump", valueEntry<JsonTraits, Operation::Dump>, 2 },
        { "std::json::__find", valueEntry<JsonTraits, Operation::Find>, 3 },
        { "std::json::__findBytes", valueEntry<JsonTraits, Operation::FindBytes>, 3 },
        { "std::json::__float", valueEntry<JsonTraits, Operation::Float>, 2 },
        { "std::json::__int", valueEntry<JsonTraits, Operation::Int>, 2 },
        { "std::json::__key", valueEntry<JsonTraits, Operation::Key>, 3 },
        { "std::json::__keyAvailable", valueEntry<JsonTraits, Operation::KeyAvailable>, 3 },
        { "std::json::__keyBytes", valueEntry<JsonTraits, Operation::KeyBytes>, 3 },
        { "std::json::__kind", valueEntry<JsonTraits, Operation::Kind>, 2 },
        { "std::json::__parse", valueEntry<JsonTraits, Operation::Parse>, 1 },
        { "std::json::__size", valueEntry<JsonTraits, Operation::Size>, 2 },
        { "std::json::__string", valueEntry<JsonTraits, Operation::String>, 2 },
        { "std::json::__takeError", valueEntry<JsonTraits, Operation::Error>, 1 },
        { "std::json::__textAvailable", valueEntry<JsonTraits, Operation::TextAvailable>, 2 },
        { "std::json::__valid", valueEntry<JsonTraits, Operation::Valid>, 1 },
        { "std::json::__valueCopy", valueEntry<JsonTraits, Operation::Copy>, 1 },
        { "std::json::__valueDrop", valueEntry<JsonTraits, Operation::Drop>, 1 },
        { "std::json::__valueNew", valueEntry<JsonTraits, Operation::New>, 0 },
        { "std::reflect::__compiledReflection", serdeEntry<Serde::Compiled>, 0 },
        { "std::reflect::__serdeFinite", serdeEntry<Serde::Finite>, 1 },
        { "std::reflect::__serdeFloat", serdeEntry<Serde::Float>, 1 },
        { "std::reflect::__serdeNumber", serdeEntry<Serde::Number>, 2 },
        { "std::reflect::__serdeQuote", serdeEntry<Serde::Quote>, 1 },
    };
    if (count) *count = sizeof(records) / sizeof(records[0]);
    return records;
}

/**
 * @brief 取得 TOML 拥有值的只读 C 入口表。
 * @param count 接收入口数量，可为空。
 * @return 生命周期覆盖进程的表。
 */
extern "C" const ChaliAbiRecord *CHALI_ABI_CALL abi_toml_records(size_t *count)
{
    static const ChaliAbiRecord records[] = {
        { "std::toml::__at", valueEntry<TomlTraits, Operation::At>, 3 },
        { "std::toml::__bool", valueEntry<TomlTraits, Operation::Bool>, 2 },
        { "std::toml::__bytes", valueEntry<TomlTraits, Operation::Bytes>, 2 },
        { "std::toml::__close", valueEntry<TomlTraits, Operation::Close>, 1 },
        { "std::toml::__dateTime", valueEntry<TomlTraits, Operation::DateTime>, 2 },
        { "std::toml::__dump", valueEntry<TomlTraits, Operation::Dump>, 2 },
        { "std::toml::__find", valueEntry<TomlTraits, Operation::Find>, 3 },
        { "std::toml::__findBytes", valueEntry<TomlTraits, Operation::FindBytes>, 3 },
        { "std::toml::__float", valueEntry<TomlTraits, Operation::Float>, 2 },
        { "std::toml::__int", valueEntry<TomlTraits, Operation::Int>, 2 },
        { "std::toml::__key", valueEntry<TomlTraits, Operation::Key>, 3 },
        { "std::toml::__keyAvailable", valueEntry<TomlTraits, Operation::KeyAvailable>, 3 },
        { "std::toml::__keyBytes", valueEntry<TomlTraits, Operation::KeyBytes>, 3 },
        { "std::toml::__kind", valueEntry<TomlTraits, Operation::Kind>, 2 },
        { "std::toml::__parse", valueEntry<TomlTraits, Operation::Parse>, 1 },
        { "std::toml::__size", valueEntry<TomlTraits, Operation::Size>, 2 },
        { "std::toml::__string", valueEntry<TomlTraits, Operation::String>, 2 },
        { "std::toml::__takeError", valueEntry<TomlTraits, Operation::Error>, 1 },
        { "std::toml::__textAvailable", valueEntry<TomlTraits, Operation::TextAvailable>, 2 },
        { "std::toml::__valid", valueEntry<TomlTraits, Operation::Valid>, 1 },
        { "std::toml::__valueCopy", valueEntry<TomlTraits, Operation::Copy>, 1 },
        { "std::toml::__valueDrop", valueEntry<TomlTraits, Operation::Drop>, 1 },
        { "std::toml::__valueNew", valueEntry<TomlTraits, Operation::New>, 0 },
    };
    if (count) *count = sizeof(records) / sizeof(records[0]);
    return records;
}
