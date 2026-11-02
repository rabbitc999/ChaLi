/** @file chali_library_bitcode.cpp
 * @brief 只读提取 LLVM bitstream 的目标兼容性头，静态和外置 LLVM 变体共用。
 */
#include "chali_library.hpp"
#include <limits>
#include <map>
#include <stdexcept>

namespace chali_library
{
namespace
{
constexpr std::uint64_t rootBlock = std::numeric_limits<std::uint64_t>::max();
constexpr std::size_t maximumOperands = 16u * 1048576u;  ///< 头字段总预算，不展开函数体。
/** @brief 一个缩略字段的字面量或固定宽度、VBR、数组、Char6、Blob 编码。 */
struct Operand
{
    bool literal = false;
    unsigned encoding = 0, width = 0;
    std::uint64_t value = 0;
};
using Abbreviation = std::vector<Operand>;
/** @brief 一条已解码的头记录；Blob 借用输入，返回结果不保留该借用。 */
struct Record
{
    std::uint64_t code = 0;
    std::vector<std::uint64_t> values;
    std::string_view blob;
};
/** @brief 带总记录、缩略定义、嵌套及字段预算的只读 bitstream 游标。 */
class Reader
{
    std::string_view bytes_;
    std::size_t position_ = 32, limit_, records_ = 0, definitions_ = 0, modules_ = 0, identifications_ = 0;
    std::size_t blocks_ = 0, inherited_ = 0, operands_ = 0;
    std::map<std::uint64_t, std::vector<Abbreviation>> shared_;
    BitcodeInfo info_;
    bool version_ = false, producer_ = false, epoch_ = false, triple_ = false, layout_ = false;

    /** @brief 读取低位在前的固定宽度字段，不跨越当前块。
     * @param count 位数，零至六十四。
     * @return 无符号字段值；范围非法时抛出异常。 */
    std::uint64_t bits(unsigned count)
    {
        if (count > 64 || position_ > limit_ || count > limit_ - position_) throw std::runtime_error("bitcode 位字段截断或超宽");
        std::uint64_t value = 0;
        for (unsigned bit = 0; bit < count; ++bit, ++position_)
            value |= std::uint64_t((static_cast<unsigned char>(bytes_[position_ / 8]) >> (position_ % 8)) & 1u) << bit;
        return value;
    }
    /** @brief 读取带最高续位的 VBR 整数，拒绝溢出与无限续位。
     * @param width 每组总位数。
     * @return 最多六十四位的字段值。 */
    std::uint64_t variable(unsigned width)
    {
        if (width < 2 || width > 64) throw std::runtime_error("bitcode VBR 宽度非法");
        std::uint64_t value = 0;
        unsigned shift = 0;
        const auto mask = (std::uint64_t(1) << (width - 1)) - 1;
        for (;;)
        {
            const auto group = bits(width), part = group & mask;
            if (part > (std::numeric_limits<std::uint64_t>::max() >> shift)) throw std::runtime_error("bitcode VBR 整数溢出");
            value |= part << shift;
            if (!(group >> (width - 1))) return value;
            shift += width - 1;
            if (shift >= 64) throw std::runtime_error("bitcode VBR 续位过多");
        }
    }
    /** @brief 跳过当前块内的三十二位对齐填充。
     * @return 无，填充超出块时抛出异常。 */
    void align()
    {
        const auto wanted = (position_ + 31u) & ~std::size_t(31u);
        if (wanted > limit_) throw std::runtime_error("bitcode 对齐填充截断");
        position_ = wanted;
    }
    /** @brief 读取一项缩略定义，不展开被跳过的函数或常量块。
     * @return 至多六十四个字段操作的缩略。 */
    Abbreviation abbreviation()
    {
        if (++definitions_ > 65536) throw std::runtime_error("bitcode 缩略定义超过预算");
        const auto count = variable(5);
        if (!count || count > 64) throw std::runtime_error("bitcode 缩略字段数量非法");
        Abbreviation result;
        for (std::uint64_t index = 0; index < count; ++index)
        {
            Operand field;
            field.literal = bits(1) != 0;
            if (field.literal)
                field.value = variable(8);
            else
            {
                field.encoding = static_cast<unsigned>(bits(3));
                if (field.encoding == 1 || field.encoding == 2)
                {
                    const auto width = variable(5);
                    if (width > 64 || (field.encoding == 2 && width == 1)) throw std::runtime_error("bitcode 缩略位宽非法");
                    field.width = static_cast<unsigned>(width);
                    if (!width) field.literal = true;
                }
                else if (field.encoding < 3 || field.encoding > 5)
                    throw std::runtime_error("bitcode 缩略编码未知");
            }
            result.push_back(field);
        }
        return result;
    }
    /** @brief 解码单个标量缩略字段，Char6 使用 LLVM 固定字符表。
     * @param field 已验证的缩略字段。
     * @return 字面量、整数或字符编码。 */
    std::uint64_t scalar(const Operand &field)
    {
        if (field.literal) return field.value;
        if (field.encoding == 1) return bits(field.width);
        if (field.encoding == 2) return variable(field.width);
        if (field.encoding == 4)
        {
            const auto value = bits(6);
            return value < 26 ? 'a' + value : value < 52 ? 'A' + value - 26 :
                                          value < 62     ? '0' + value - 52 :
                                          value == 62    ? '.' :
                                                           '_';
        }
        throw std::runtime_error("bitcode 数组或 Blob 出现在标量位置");
    }
    /** @brief 解码当前头记录，非头子块由块长度直接跳过。
     * @param code 缩略编号或未缩略记录标记。
     * @param abbreviations 当前块可用的缩略目录。
     * @return 记录编号、字段与可选 Blob。 */
    Record record(std::uint64_t code, const std::vector<Abbreviation> &abbreviations)
    {
        if (++records_ > 1048576) throw std::runtime_error("bitcode 头记录超过预算");
        Record result;
        if (code == 3)
        {
            result.code = variable(6);
            const auto count = variable(6);
            if (count > 1048576 || count > maximumOperands - operands_) throw std::runtime_error("bitcode 头字段总量超过预算");
            operands_ += static_cast<std::size_t>(count);
            for (std::uint64_t index = 0; index < count; ++index) result.values.push_back(variable(6));
            return result;
        }
        if (code < 4 || code - 4 >= abbreviations.size()) throw std::runtime_error("bitcode 引用了不存在的缩略");
        const auto &fields = abbreviations[static_cast<std::size_t>(code - 4)];
        result.code = scalar(fields.front());
        for (std::size_t index = 1; index < fields.size(); ++index)
        {
            const auto &field = fields[index];
            if (!field.literal && field.encoding == 3)
            {
                if (index + 2 != fields.size()) throw std::runtime_error("bitcode 数组缩略位置非法");
                const auto count = variable(6);
                if (count > 1048576 || count > maximumOperands - operands_) throw std::runtime_error("bitcode 数组字段总量超过预算");
                operands_ += static_cast<std::size_t>(count);
                const auto &element = fields[++index];
                for (std::uint64_t item = 0; item < count; ++item) result.values.push_back(scalar(element));
            }
            else if (!field.literal && field.encoding == 5)
            {
                if (index + 1 != fields.size()) throw std::runtime_error("bitcode Blob 缩略位置非法");
                const auto length = variable(6);
                align();
                if (length > (limit_ - position_) / 8) throw std::runtime_error("bitcode Blob 截断");
                result.blob = bytes_.substr(position_ / 8, static_cast<std::size_t>(length));
                position_ += static_cast<std::size_t>(length) * 8;
                align();
            }
            else
            {
                if (++operands_ > maximumOperands) throw std::runtime_error("bitcode 头字段总量超过预算");
                result.values.push_back(scalar(field));
            }
        }
        return result;
    }
    /** @brief 将兼容性字段转换为有界 ASCII，不接纳隐藏 NUL 或控制字符。
     * @param source 记录中的字符或 Blob。
     * @return 复制后的目标或生产标识文本。 */
    std::string text(const Record &source)
    {
        if ((!source.blob.empty() && !source.values.empty()) || source.blob.size() > 8192 || source.values.size() > 8192)
            throw std::runtime_error("bitcode 兼容性文本超过预算或编码冲突");
        std::string result(source.blob);
        if (source.blob.empty())
            for (const auto value : source.values)
            {
                if (value < 32 || value > 126) throw std::runtime_error("bitcode 兼容性字段含非法字符");
                result.push_back(static_cast<char>(value));
            }
        for (const unsigned char value : result)
            if (value < 32 || value > 126) throw std::runtime_error("bitcode Blob 含非法兼容性字符");
        return result;
    }
    /** @brief 提取模块和生产者头，同一语义字段只能声明一次。
     * @param block 当前块编号。
     * @param value 解码后的记录。
     * @return 无，重复或未知版本抛出异常。 */
    void metadata(std::uint64_t block, const Record &value)
    {
        if (block == 13 && value.code == 1)
        {
            if (producer_) throw std::runtime_error("bitcode 重复声明生产者");
            producer_ = true;
            info_.producer = text(value);
        }
        else if (block == 13 && value.code == 2)
        {
            if (epoch_ || value.values.size() != 1) throw std::runtime_error("bitcode epoch 声明非法");
            epoch_ = true;
            info_.epoch = value.values.front();
        }
        else if (block == 8 && value.code == 1)
        {
            if (version_ || value.values.size() != 1 || value.values.front() > 2) throw std::runtime_error("bitcode 模块编码版本不受支持");
            version_ = true;
        }
        else if (block == 8 && value.code == 2)
        {
            if (triple_) throw std::runtime_error("bitcode 重复声明目标");
            triple_ = true;
            info_.triple = text(value);
        }
        else if (block == 8 && value.code == 3)
        {
            if (layout_) throw std::runtime_error("bitcode 重复声明数据布局");
            layout_ = true;
            info_.dataLayout = text(value);
        }
    }
    /** @brief 遍历头块并按声明长度跳过函数、常量和摘要内容。
     * @param id 当前块编号，根层使用保留值。
     * @param width 当前缩略编号位宽。
     * @param end 块的位末尾。
     * @param depth 头块嵌套深度。
     * @return 无，块越界或缺少终止时抛出异常。 */
    void block(std::uint64_t id, unsigned width, std::size_t end, unsigned depth)
    {
        if (depth > 64 || width < 2 || width > 32) throw std::runtime_error("bitcode 块嵌套或位宽非法");
        const auto outer = limit_;
        limit_ = end;
        const auto &shared = shared_[id];
        if (shared.size() > 1048576 - inherited_) throw std::runtime_error("bitcode 继承缩略超过预算");
        inherited_ += shared.size();
        auto abbreviations = shared;
        auto target = rootBlock;
        bool closed = id == rootBlock;
        while (position_ < end)
        {
            const auto code = bits(width);
            if (code == 0)
            {
                if (id == rootBlock) throw std::runtime_error("bitcode 根层含块终止码");
                align();
                if (position_ != end) throw std::runtime_error("bitcode 块长度与终止位置不符");
                closed = true;
                break;
            }
            if (code == 1)
            {
                const auto child = variable(8), childWidth = variable(4);
                if (++blocks_ > 1048576 || childWidth < 2 || childWidth > 32) throw std::runtime_error("bitcode 子块数量或位宽非法");
                align();
                const auto words = bits(32);
                if (words > (end - position_) / 32) throw std::runtime_error("bitcode 子块截断");
                const auto childEnd = position_ + static_cast<std::size_t>(words) * 32;
                if (child == 8 && ++modules_ != 1) throw std::runtime_error("库 bitcode 必须只含一个模块");
                if (child == 13 && ++identifications_ != 1) throw std::runtime_error("bitcode 生产头必须唯一");
                if ((child == 8 || child == 13) && id != rootBlock) throw std::runtime_error("bitcode 模块或生产头位置非法");
                if (id == 8 && (child == 20 || child == 24)) info_.summary = true;
                if (child == 0 || child == 8 || child == 13)
                    block(child, static_cast<unsigned>(childWidth), childEnd, depth + 1);
                else
                    position_ = childEnd;
                continue;
            }
            if (code == 2)
            {
                auto value = abbreviation();
                if (id == 0)
                {
                    if (target == rootBlock) throw std::runtime_error("bitcode 块信息缺少目标编号");
                    shared_[target].push_back(std::move(value));
                }
                else
                    abbreviations.push_back(std::move(value));
                continue;
            }
            const auto value = record(code, abbreviations);
            if (id == 0 && value.code == 1)
            {
                if (value.values.size() != 1) throw std::runtime_error("bitcode 块信息目标非法");
                target = value.values.front();
            }
            metadata(id, value);
        }
        if (!closed || position_ != end) throw std::runtime_error("bitcode 块缺少终止码");
        limit_ = outer;
    }

public:
    /** @brief 为已剥离封装头的原始 bitcode 建立游标。
     * @param bytes 含规范四字节魔数的载荷。
     * @return 无。 */
    explicit Reader(std::string_view bytes) : bytes_(bytes), limit_(bytes.size() * 8) {}
    /** @brief 读取一个模块的头，并从 LLVM 生产者字符串核对版本。
     * @return 完全拥有文本的兼容性描述。 */
    BitcodeInfo read()
    {
        block(rootBlock, 2, limit_, 0);
        if (modules_ != 1 || !version_ || !producer_ || !epoch_ || info_.triple.empty() || info_.dataLayout.empty())
            throw std::runtime_error("bitcode 缺少模块、目标、布局或生产版本");
        if (info_.epoch != 0) throw std::runtime_error("bitcode 兼容性 epoch 不受支持");
        if (info_.producer.compare(0, 4, "LLVM") != 0) throw std::runtime_error("bitcode 生产者未提供 LLVM 版本");
        auto start = info_.producer.find_first_not_of(' ', 4);
        if (start == std::string::npos || info_.producer[start] < '0' || info_.producer[start] > '9') throw std::runtime_error("bitcode LLVM 生产版本非法");
        const auto end = info_.producer.find(' ', start);
        info_.llvmVersion = info_.producer.substr(start, end == std::string::npos ? end : end - start);
        return info_;
    }
};
/** @brief 从已核对范围的封装头中读取小端整数。
 * @param bytes 输入封装头。
 * @param offset 四字节字段偏移。
 * @return 小端字段值。 */
std::uint32_t word(std::string_view bytes, std::size_t offset)
{
    std::uint32_t value = 0;
    for (unsigned index = 0; index < 4; ++index) value |= std::uint32_t(static_cast<unsigned char>(bytes[offset + index])) << (index * 8);
    return value;
}
}  // namespace
/** @brief 在预算内检查原始或 LLVM 封装 bitcode 的兼容性头。
 * @param bytes 包快照中的不可变载荷。
 * @return 单模块实际目标、布局、生产版本及摘要标记。 */
BitcodeInfo inspectBitcode(std::string_view bytes)
{
    if (bytes.size() < 4 || bytes.size() > maximumPackageBytes) throw std::runtime_error("bitcode 载荷长度非法");
    if (word(bytes, 0) == 0x0b17c0deu)
    {
        if (bytes.size() < 20 || word(bytes, 4) != 0) throw std::runtime_error("bitcode 封装头截断或版本非法");
        const auto offset = word(bytes, 8), size = word(bytes, 12);
        if (offset < 20 || offset > bytes.size() || size < 4 || size > bytes.size() - offset) throw std::runtime_error("bitcode 封装范围非法");
        bytes = bytes.substr(offset, size);
    }
    if (word(bytes, 0) != 0xdec04342u || bytes.size() % 4) throw std::runtime_error("bitcode 魔数或对齐非法");
    return Reader(bytes).read();
}
}  // namespace chali_library
