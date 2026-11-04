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
 * @brief 不可变描述符在当前源码映射中的声明位置，不包含 AST 池编号。
 */
struct MetadataOrigin
{
    std::size_t line = 0;
    std::size_t column = 0;
};
/**
 * @brief 编译期注解实参的精确类型及标量值，不保存语言运行时句柄。
 */
struct MetadataArgument
{
    MetadataOrigin origin;
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
    MetadataOrigin origin;
    Memory::String name;
    std::uint64_t identity = 0;
    Memory::Vector<MetadataArgument> arguments;
};
/**
 * @brief 具体字段的声明身份、精确类型和注解，不包含目标布局偏移。
 */
struct MetadataField
{
    MetadataOrigin origin;
    Memory::String name;
    std::uint64_t identity = 0;
    std::size_t index = 0;
    std::shared_ptr<const MType> type;
    Memory::Vector<MetadataAttribute> attributes;
    bool variadic = false;
    bool hasDefault = false;
    Memory::String declaringType;
};
/**
 * @brief 枚举变体的声明身份与按声明顺序保存的精确载荷，不暴露合成存储字段。
 */
struct MetadataVariant
{
    MetadataOrigin origin;
    Memory::String name;
    std::uint64_t identity = 0;
    std::size_t index = 0;
    Memory::Vector<MetadataField> payloads;
};
/**
 * @brief 方法签名与接收者属性，不携带可直接执行的机器地址。
 */
struct MetadataMethod
{
    MetadataOrigin origin;
    Memory::String name;
    std::uint64_t identity = 0;
    std::size_t index = 0;
    std::shared_ptr<const MType> result;
    Memory::Vector<MetadataField> parameters;
    bool isStatic = false;
    bool isConstructor = false;
    bool hasBody = false;
    bool inherited = false;
    Memory::String declaringType;
    std::shared_ptr<const MType> receiver;
    bool usesUnsafe = false;
    bool hasPropagation = false;
    bool hasMutableParameters = false;
    bool returnsMutableReference = false;
};
/**
 * @brief 类型实现的一个接口及其直接声明／继承来源。
 */
struct MetadataInterface
{
    MetadataOrigin origin;
    std::shared_ptr<const MType> type;
    std::uint64_t identity = 0;
    std::size_t index = 0;
    bool direct = false;
};
/**
 * @brief 已实例化类型的只读声明快照，集合不包含目标布局或机器地址。
 */
struct MetadataType
{
    MetadataOrigin origin;
    Memory::String name;
    std::uint64_t identity = 0;
    std::shared_ptr<const MType> type;
    Memory::Vector<MetadataField> fields;
    Memory::Vector<MetadataVariant> variants;
    Memory::Vector<MetadataMethod> methods;
    Memory::Vector<MetadataInterface> interfaces;
    Memory::Vector<std::shared_ptr<const MType>> typeArguments;
};
/**
 * @brief 仅供宏展开和编译期查询使用的有类型描述符，禁止转换为运行期值。
 */
struct MetadataValue
{
    MetadataOrigin origin;
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
        Argument,
        Variants,
        Variant,
        Payloads,
        Payload,
        Methods,
        Method,
        Parameters,
        Parameter,
        Interfaces,
        Interface,
        AssociatedTypes,
        AssociatedType,
        TypeArguments
    } kind = Fields;
    std::shared_ptr<const MetadataType> owner;
    std::shared_ptr<const MType> type;
    std::size_t field = 0;
    std::size_t attribute = 0;
    std::size_t argument = 0;
    std::size_t variant = 0;
    std::size_t payload = 0;
    std::size_t method = 0;
    std::size_t parameter = 0;
    std::size_t interfaceIndex = 0;
};
}  // namespace chali_macro
#endif
