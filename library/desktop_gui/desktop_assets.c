/**
 * @file desktop_assets.c
 * @brief GUI 资源文件的 C 读取服务。
 */
#ifndef _WIN32
#define _FILE_OFFSET_BITS 64
#define _POSIX_C_SOURCE   200809L
#endif
#include "desktop_assets.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
/**
 * @brief 用 C 文件接口打开资源，Windows 路径按 UTF-8 转换。
 * @param path 调用期间借用的零结尾 UTF-8 路径。
 * @return 由调用方 fclose 的 C 流，失败为空。
 */
static FILE *open_asset(const char *path)
{
#ifdef _WIN32
    int count;
    wchar_t *wide;
    FILE *stream;
    count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, NULL, 0);
    if (count <= 0) return NULL;
    wide = malloc((size_t)count * sizeof(*wide));
    if (!wide) return NULL;
    if (!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, wide, count))
    {
        free(wide);
        return NULL;
    }
    stream = _wfopen(wide, L"rb");
    free(wide);
    return stream;
#else
    return fopen(path, "rb");
#endif
}
/**
 * @brief 读取独立资源字节，长度不超过下游 C 解码接口的 INT_MAX。
 * @param path 调用期间借用的零结尾 UTF-8 路径。
 * @param length 接收资源字节长度。
 * @return 由调用方 free 的独占字节，失败为空。
 */
unsigned char *chali_gui_read_asset(const char *path, size_t *length)
{
    FILE *stream;
    long long size;
    unsigned char *bytes;
    if (length) *length = 0;
    if (!path || !*path) return NULL;
    stream = open_asset(path);
    if (!stream) return NULL;
#ifdef _WIN32
    if (_fseeki64(stream, 0, SEEK_END))
    {
        fclose(stream);
        return NULL;
    }
    size = _ftelli64(stream);
    if (size < 0 || size > INT_MAX || _fseeki64(stream, 0, SEEK_SET))
    {
        fclose(stream);
        return NULL;
    }
#else
    if (fseeko(stream, 0, SEEK_END))
    {
        fclose(stream);
        return NULL;
    }
    size = (long long)ftello(stream);
    if (size < 0 || size > INT_MAX || fseeko(stream, 0, SEEK_SET))
    {
        fclose(stream);
        return NULL;
    }
#endif
    bytes = malloc(size ? (size_t)size : 1);
    if (!bytes)
    {
        fclose(stream);
        return NULL;
    }
    if (fread(bytes, 1, (size_t)size, stream) != (size_t)size || ferror(stream))
    {
        free(bytes);
        fclose(stream);
        return NULL;
    }
    fclose(stream);
    if (length) *length = (size_t)size;
    return bytes;
}
