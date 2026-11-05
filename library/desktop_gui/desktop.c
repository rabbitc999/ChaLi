/**
 * @file desktop.c
 * @brief raygui 实现与兼容的即时标签、按钮。
 */
#include "desktop.h"
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
/**
 * @brief 在本线程当前帧绘制标签。
 * @param x 左边。
 * @param y 顶部。
 * @param width 宽度。
 * @param height 高度。
 * @param text UTF-8 文本。
 */
void chali_gui_label(int32_t x, int32_t y, int32_t width, int32_t height, const char *text)
{
    if (chali_gui_is_drawing() && width > 0 && height > 0)
        GuiLabel((Rectangle){ x, y, width, height }, text ? text : "");
}
/**
 * @brief 在本线程当前帧绘制按钮，兼容原接口。
 * @param x 左边。
 * @param y 顶部。
 * @param width 宽度。
 * @param height 高度。
 * @param text UTF-8 文本。
 * @return 点击时为真。
 */
bool chali_gui_button(int32_t x, int32_t y, int32_t width, int32_t height, const char *text)
{
    return chali_gui_is_drawing() && width > 0 && height > 0 && GuiButton((Rectangle){ x, y, width, height }, text ? text : "");
}
