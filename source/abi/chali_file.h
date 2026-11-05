/** @file chali_file.h @brief 使用 C 标准 I/O 的跨平台文件服务。 */
#ifndef CHALI_FILE_H
#define CHALI_FILE_H
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif
/** @brief 调用方拥有的文件状态，关闭后可重复初始化。 */
typedef struct ChaliFile
{
    FILE *stream;
    int readable;
    int writable;
    int last_read;
} ChaliFile;
/**
 * @brief 根据既有 r/w/a/+ 规则打开二进制文件，Windows 路径按 UTF-8 解码。
 * @param file 接收状态，必须尚未打开。
 * @param path 以零结束的路径。
 * @param mode 以零结束的模式。
 * @return 成功为一，失败为零。
 */
int chali_file_open(ChaliFile *file, const char *path, const char *mode);
/**
 * @brief 获取一个字节。
 * @param file 打开的文件。
 * @param byte 接收字节。
 * @return 一为成功，零为 EOF，负一为错误。
 */
int chali_file_get(ChaliFile *file, unsigned char *byte);
/**
 * @brief 读取字节，EOF 允许成功短读取。
 * @param file 打开的文件。
 * @param bytes 输出缓冲。
 * @param capacity 缓冲容量。
 * @param count 实际读取数量。
 * @return 无 I/O 错误为一。
 */
int chali_file_read(ChaliFile *file, void *bytes, size_t capacity, size_t *count);
/**
 * @brief 写入全部字节并刷新 C 缓冲。
 * @param file 打开的文件。
 * @param bytes 输入缓冲。
 * @param count 输入长度。
 * @return 成功为一。
 */
int chali_file_write(ChaliFile *file, const void *bytes, size_t count);
/**
 * @brief 刷新输出缓冲。
 * @param file 打开的文件。
 * @return 成功为一。
 */
int chali_file_flush(ChaliFile *file);
/**
 * @brief 按旧读写流次序定位，更新模式保持两次定位行为。
 * @param file 打开的文件。
 * @param offset 有符号偏移。
 * @param origin 零开头、一当前位置、二结尾。
 * @param position 接收最终绝对位置。
 * @return 成功为一。
 */
int chali_file_seek(ChaliFile *file, int64_t offset, int origin, int64_t *position);
/**
 * @brief 查询 EOF 标记。
 * @param file 文件状态。
 * @return 已到达 EOF 为一。
 */
int chali_file_eof(const ChaliFile *file);
/**
 * @brief 关闭并清空句柄，即使关闭报错也不会重复关闭。
 * @param file 文件状态。
 * @return 成功为一，未打开或关闭出错为零。
 */
int chali_file_close(ChaliFile *file);
#ifdef __cplusplus
}
#endif
#endif
