#ifndef CHALI_MACRO_METADATA_HPP
#define CHALI_MACRO_METADATA_HPP
#include "../allocator/chali_allocator.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>

struct MType;

namespace chali_macro
{
/**
 * @brief 编译期注解实参的精确类型及标量值，不保存语言运行时句柄。
 */
struct MetadataArgument
{
    Memory::String name;
    std::shared_ptr<const MType> type;
    long long integer = 0;
    double floating = 0;
    Memory::String text;
};
/**
 * @brief 注解声明身份及按形参声明顺序归一的实参。
 */
struct MetadataAttribute
{
    Memory::String name;
    std::uint64_t identity = 0;
    Memory::Vector<MetadataArgument> arguments;
};
/**
 * @brief 具体字段的声明身份、精确类型和注解，不包含目标布局偏移。
 */
struct MetadataField
{
    Memory::String name;
    std::uint64_t identity = 0;
    std::size_t index = 0;
    std::shared_ptr<const MType> type;
    Memory::Vector<MetadataAttribute> attributes;
};
/**
 * @brief 已实例化类型的只读字段集合，元素顺序与原声明一致。
 */
struct MetadataFields
{
    Memory::String name;
    std::uint64_t identity = 0;
    std::shared_ptr<const MType> type;
    Memory::Vector<MetadataField> fields;
};
/**
 * @brief 仅供宏展开和编译期查询使用的有类型描述符，禁止转换为运行期值。
 */
struct MetadataValue
{
    /**
     * @brief 描述符的合法查询类别，避免将字段、类型和注解身份混用。
     */
    enum Kind
    {
        Fields,
        Field,
        Type,
        Attributes,
        Attribute,
        Arguments,
        Argument
    } kind = Fields;
    std::shared_ptr<const MetadataFields> owner;
    std::shared_ptr<const MType> type;
    std::size_t field = 0;
    std::size_t attribute = 0;
    std::size_t argument = 0;
};
}  // namespace chali_macro
#endif
