/** @file chali_file_c_test.cpp @brief 对照原文件流的模式、位置和读写行为。 */
#include "../source/abi/chali_file.h"
#include <cassert>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <string>

/**
 * @brief 写入用于对照的固定二进制内容。
 * @param path 测试路径。
 */
static void seed(const char *path)
{
    FILE *file = std::fopen(path, "wb");
    assert(file);
    assert(std::fwrite("abc\r\ndef\0tail", 1, 13, file) == 13);
    assert(std::fclose(file) == 0);
}
/**
 * @brief 原实现的打开标志。
 * @param mode 公开模式。
 * @return 标准流标志。
 */
static std::ios::openmode flags(const std::string &mode)
{
    auto result = std::ios::binary;
    if (mode.find('r') != mode.npos || mode.find('+') != mode.npos) result |= std::ios::in;
    if (mode.find('w') != mode.npos) result |= std::ios::out | std::ios::trunc;
    if (mode.find('a') != mode.npos) result |= std::ios::out | std::ios::app;
    if (mode.find('+') != mode.npos) result |= std::ios::in | std::ios::out;
    return result;
}
/**
 * @brief 读取完整文件以比较实际落盘字节。
 * @param path 路径。
 * @return 文件字节。
 */
static std::string content(const char *path)
{
    std::ifstream input(path, std::ios::binary);
    return std::string(std::istreambuf_iterator<char>(input), {});
}
/**
 * @brief 对照打开、双向相对定位和文件字节，并核对 EOF 与大偏移。
 * @return 通过返回零。
 */
int main()
{
    const char *actualPath = "tmp/chali-file-c.bin", *referencePath = "tmp/chali-file-reference.bin";
    for (const char *mode : {"r", "r+", "w", "w+", "a", "a+", "+", "bad", "rw", "wa", ""})
    {
        seed(actualPath);
        seed(referencePath);
        ChaliFile actual{};
        std::fstream reference(referencePath, flags(mode));
        const bool opened = chali_file_open(&actual, actualPath, mode);
        assert(opened == reference.is_open());
        if (!opened) continue;
        int64_t position = -1;
        reference.clear();
        if (actual.readable) reference.seekg(1, std::ios::cur);
        if (actual.writable) reference.seekp(1, std::ios::cur);
        const bool seekOk = chali_file_seek(&actual, 1, 1, &position);
        assert(seekOk == !reference.fail());
        if (seekOk)
        {
            auto expected = actual.readable ? reference.tellg() : reference.tellp();
            assert(position == static_cast<int64_t>(expected));
        }
        if (actual.writable)
        {
            reference.clear();
            reference.seekp(0);
            if (actual.readable) reference.seekg(0);
            assert(chali_file_seek(&actual, 0, 0, &position));
            reference.write("X\0Y", 3);
            reference.flush();
            assert(chali_file_write(&actual, "X\0Y", 3) == !reference.fail());
        }
        if (actual.readable)
        {
            reference.clear();
            reference.seekg(0);
            assert(chali_file_seek(&actual, 0, 0, &position));
            char expected[64], result[64];
            reference.read(expected, sizeof(expected));
            size_t count = 0;
            assert(chali_file_read(&actual, result, sizeof(result), &count));
            assert(count == static_cast<size_t>(reference.gcount()));
            assert(std::string(result, count) == std::string(expected, count));
            assert(bool(chali_file_eof(&actual)) == reference.eof());
            assert(!chali_file_write(&actual, "z", 1));
            assert(chali_file_seek(&actual, 0, 0, &position));
            assert(!chali_file_eof(&actual));
        }
        reference.clear();
        reference.close();
        assert(chali_file_close(&actual));
        assert(!chali_file_close(&actual));
        assert(content(actualPath) == content(referencePath));
    }
    {
        const char *unicodePath = "tmp/\xe6\x96\x87\xe4\xbb\xb6-c-io.bin";
        ChaliFile file{};
        assert(chali_file_open(&file, unicodePath, "w+"));
        assert(chali_file_write(&file, "abc", 3));
        int64_t position = -1;
        assert(chali_file_seek(&file, INT64_C(5) * 1024 * 1024 * 1024, 0, &position));
        assert(position == INT64_C(5) * 1024 * 1024 * 1024);
        assert(!chali_file_seek(&file, 0, 3, &position));
        assert(chali_file_seek(&file, 0, 0, &position));
        unsigned char byte = 0;
        assert(chali_file_get(&file, &byte) == 1 && byte == 'a');
        assert(chali_file_close(&file));
#ifdef _WIN32
        assert(_wremove(L"tmp/文件-c-io.bin") == 0);
#else
        assert(std::remove(unicodePath) == 0);
#endif
    }
    assert(std::remove(actualPath) == 0);
    assert(std::remove(referencePath) == 0);
}
