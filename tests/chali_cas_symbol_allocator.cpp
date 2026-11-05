/** @file chali_cas_symbol_allocator.cpp @brief 禁止 C++ 堆时核对 CAS 符号驻留与地址稳定性。 */
#include "cas/detail/node.h"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <new>
#include <string>

static bool rejectCppAllocation = false;
/** @brief 在被测区间禁止默认 C++ 分配。 @param bytes 大小。 @return 初始化用存储。 */
void *operator new(std::size_t bytes)
{
    if (rejectCppAllocation) std::abort();
    if (void *pointer = std::malloc(bytes ? bytes : 1)) return pointer;
    throw std::bad_alloc();
}
/** @brief 释放初始化用存储。 @param pointer 地址。 */
void operator delete(void *pointer) noexcept { std::free(pointer); }
/** @brief 支持带大小释放。 @param pointer 地址。 @param bytes 大小。 */
void operator delete(void *pointer, std::size_t bytes) noexcept { (void)bytes; std::free(pointer); }
/**
 * @brief 核对长名称、重复驻留、查询与扩容后的旧地址。
 * @return 成功返回零。
 */
int main()
{
    const std::string longName(1024, 'q');
    rejectCppAllocation = true;
    const auto first = Cas::Detail::internSymbol(longName);
    const char *stable = Cas::Detail::symbolName(first);
    assert(std::strcmp(stable, longName.c_str()) == 0);
    for (int index = 0; index < 2048; ++index)
    {
        const auto name = std::to_string(index);
        const auto id = Cas::Detail::internSymbol(name);
        assert(Cas::Detail::lookupSymbol(name) == id);
        assert(Cas::Detail::internSymbol(name) == id);
    }
    assert(Cas::Detail::symbolName(first) == stable);
    assert(Cas::Detail::lookupSymbol(longName) == first);
    assert(Cas::Detail::allSymbols().size() == 2049);
    rejectCppAllocation = false;
}
