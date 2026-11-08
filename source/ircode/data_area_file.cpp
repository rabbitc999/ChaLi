/**
 * @file data_area_file.cpp
 * @brief 编译工具的数据映像文件读取。
 */
#include "runtime_internal.hpp"

namespace runtime
{
using namespace runtimeDetail;
/**
 * @brief 加载二进制与布局表。
 * @param binaryPath `.data.bin` 路径。
 * @param layoutPath 布局文件：一行 `符号 offset=0 size=8 type=…`。
 * @param diagnostics 出错信息。
 * @return 成功返回 true。
 */
bool DataArea::load(const Memory::String &binaryPath, const Memory::String &layoutPath,
                    Memory::Vector<Memory::String> *diagnostics)
{
#if RUNTIME_WITH_HOST_IO

    // 1. 读二进制。
    std::ifstream binaryStream(Memory::u8path(binaryPath), std::ios::binary);
    if (!binaryStream)
    {
        if (diagnostics) diagnostics->push_back("打不开数据区文件：" + binaryPath);
        return false;
    }
    binary.assign(std::istreambuf_iterator<char>(binaryStream),
                  std::istreambuf_iterator<char>());

    // 2. 读布局：一行 `符号 offset=0 size=8 type=…`。
    symbols.clear();
    std::ifstream layoutStream(Memory::u8path(layoutPath), std::ios::binary);
    if (!layoutStream)
    {
        if (diagnostics) diagnostics->push_back("打不开布局文件：" + layoutPath);
        return false;
    }
    Memory::String layout{ std::istreambuf_iterator<char>(layoutStream), std::istreambuf_iterator<char>() };
    // 内存接口统一处理文本与二进制；先移出映像，避免赋值时自引用失效。
    const auto image = std::move(binary);
    return loadFromBytes(image.data(), image.size(), layout, diagnostics);

#else
    return false;
#endif
}

}
