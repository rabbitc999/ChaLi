/**
 * @file desktop_assets.h
 * @brief GUI 资源文件的 C 读取服务。
 */
#ifndef CHALI_GUI_ASSETS_H
#define CHALI_GUI_ASSETS_H
#include <stddef.h>
/**
 * @brief 读取独立资源字节，长度不超过下游 C 解码接口的 INT_MAX。
 * @param path 调用期间借用的零结尾 UTF-8 路径。
 * @param length 接收资源字节长度。
 * @return 由调用方 free 的独占字节，失败为空。
 */
unsigned char *chali_gui_read_asset(const char *path, size_t *length);
#endif
