/** @file chali_file.c @brief 不依赖 C++ 流和文件系统的文件内核。 */
#if !defined(_WIN32)
#define _FILE_OFFSET_BITS 64
#define _POSIX_C_SOURCE 200809L
#endif
#include "chali_file.h"
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/types.h>
#endif

/**
 * @brief 使用当前平台的 64 位文件定位接口。
 * @param stream C 文件流。
 * @param offset 偏移。
 * @param origin C 定位基准。
 * @return 成功为零。
 */
static int seek_stream(FILE *stream, int64_t offset, int origin)
{
#ifdef _WIN32
    return _fseeki64(stream, offset, origin);
#else
    return fseeko(stream, (off_t)offset, origin);
#endif
}
/**
 * @brief 读取当前平台的 64 位绝对位置。
 * @param stream C 文件流。
 * @return 位置，失败为负一。
 */
static int64_t tell_stream(FILE *stream)
{
#ifdef _WIN32
    return _ftelli64(stream);
#else
    return (int64_t)ftello(stream);
#endif
}
/**
 * @brief 打开 UTF-8 路径，不依赖 C++ locale 或文件系统。
 * @param path UTF-8 路径。
 * @param mode 已规范化的二进制模式。
 * @return C 文件流或空指针。
 */
static FILE *open_stream(const char *path, const char *mode)
{
#ifdef _WIN32
    int length = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, NULL, 0);
    wchar_t wide_mode[4];
    size_t index;
    FILE *stream;
    wchar_t *wide_path;
    if (length <= 0 || (size_t)length > SIZE_MAX / sizeof(wchar_t)) return NULL;
    wide_path = (wchar_t *)malloc((size_t)length * sizeof(wchar_t));
    if (!wide_path) return NULL;
    if (!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, wide_path, length))
    {
        free(wide_path);
        return NULL;
    }
    for (index = 0; index < sizeof(wide_mode) / sizeof(wide_mode[0]); ++index)
    {
        wide_mode[index] = (wchar_t)(unsigned char)mode[index];
        if (!mode[index]) break;
    }
    stream = _wfopen(wide_path, wide_mode);
    free(wide_path);
    return stream;
#else
    return fopen(path, mode);
#endif
}
/** @brief 打开文件。 @param file 状态。 @param path 路径。 @param mode 模式。 @return 成功为一。 */
int chali_file_open(ChaliFile *file, const char *path, const char *mode)
{
    int truncate, append;
    const char *normalized;
    if (!file || !path || !mode) return 0;
    memset(file, 0, sizeof(*file));
    truncate = strchr(mode, 'w') != NULL;
    append = strchr(mode, 'a') != NULL;
    file->readable = strchr(mode, 'r') != NULL || strchr(mode, '+') != NULL;
    file->writable = truncate || append || strchr(mode, '+') != NULL;
    if ((truncate && append) || (!file->readable && !file->writable)) return 0;
    if (truncate) normalized = file->readable ? "w+b" : "wb";
    else if (append) normalized = file->readable ? "a+b" : "ab";
    else normalized = file->writable ? "r+b" : "rb";
    file->stream = open_stream(path, normalized);
    if (file->stream && append && file->readable && seek_stream(file->stream, 0, SEEK_SET) != 0)
    {
        fclose(file->stream);
        file->stream = NULL;
    }
    return file->stream != NULL;
}
/** @brief 获取字节。 @param file 文件。 @param byte 输出。 @return 一成功、零 EOF、负一错误。 */
int chali_file_get(ChaliFile *file, unsigned char *byte)
{
    int value;
    if (!file || !file->stream || !byte || !file->readable) return -1;
    value = fgetc(file->stream);
    file->last_read = 1;
    if (value == EOF) return ferror(file->stream) ? -1 : 0;
    *byte = (unsigned char)value;
    return 1;
}
/** @brief 读取字节。 @param file 文件。 @param bytes 输出。 @param capacity 容量。 @param count 数量。 @return 成功为一。 */
int chali_file_read(ChaliFile *file, void *bytes, size_t capacity, size_t *count)
{
    if (count) *count = 0;
    if (!file || !file->stream || !file->readable || !count || (capacity && !bytes)) return 0;
    *count = capacity ? fread(bytes, 1, capacity, file->stream) : 0;
    file->last_read = 1;
    return !ferror(file->stream);
}
/** @brief 写入并刷新。 @param file 文件。 @param bytes 输入。 @param count 长度。 @return 成功为一。 */
int chali_file_write(ChaliFile *file, const void *bytes, size_t count)
{
    if (!file || !file->stream || !file->writable || (count && !bytes)) return 0;
    if (ferror(file->stream) || feof(file->stream)) return 0;
    if (file->last_read && seek_stream(file->stream, 0, SEEK_CUR) != 0) return 0;
    file->last_read = 0;
    if (count && fwrite(bytes, 1, count, file->stream) != count) return 0;
    return fflush(file->stream) == 0;
}
/** @brief 刷新。 @param file 文件。 @return 成功为一。 */
int chali_file_flush(ChaliFile *file)
{
    if (!file || !file->stream || ferror(file->stream) || feof(file->stream)) return 0;
    /* 输入流不调用 fflush，避免 ISO C 未定义的输入刷新行为。 */
    return file->last_read || !file->writable || fflush(file->stream) == 0;
}
/** @brief 定位。 @param file 文件。 @param offset 偏移。 @param origin 基准。 @param position 结果。 @return 成功为一。 */
int chali_file_seek(ChaliFile *file, int64_t offset, int origin, int64_t *position)
{
    int direction;
    int64_t result;
    if (!file || !file->stream || !position || origin < 0 || origin > 2) return 0;
    direction = origin == 0 ? SEEK_SET : origin == 1 ? SEEK_CUR : SEEK_END;
    clearerr(file->stream);
    if (file->readable && seek_stream(file->stream, offset, direction) != 0) return 0;
    if (file->writable && seek_stream(file->stream, offset, direction) != 0) return 0;
    result = tell_stream(file->stream);
    if (result < 0) return 0;
    *position = result;
    file->last_read = 0;
    return 1;
}
/** @brief 查询末尾。 @param file 文件。 @return 已到末尾为一。 */
int chali_file_eof(const ChaliFile *file) { return file && file->stream && feof(file->stream); }
/** @brief 关闭文件。 @param file 状态。 @return 成功为一。 */
int chali_file_close(ChaliFile *file)
{
    FILE *stream;
    if (!file || !file->stream) return 0;
    stream = file->stream;
    file->stream = NULL;
    return fclose(stream) == 0;
}
