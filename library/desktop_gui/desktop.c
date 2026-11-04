#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <stdint.h>

static bool drawing;

/**
 * @brief 在当前桌面线程创建窗口。
 * @param width 窗口宽度。
 * @param height 窗口高度。
 * @param title UTF-8 标题。
 * @return 窗口就绪时为真。
 */
bool chali_gui_open(int32_t width, int32_t height, const char *title)
{
    if (width <= 0 || height <= 0 || IsWindowReady()) return false;
    InitWindow(width, height, title);
    if (!IsWindowReady()) return false;
    SetTargetFPS(60);
    return true;
}

/**
 * @brief 查询窗体是否应退出。
 * @return 无窗口或收到关闭请求时为真。
 */
bool chali_gui_should_close(void)
{
    return !IsWindowReady() || WindowShouldClose();
}

/**
 * @brief 开始一帧并以默认浅色清屏。
 * @return 成功开始新帧时为真。
 */
bool chali_gui_begin_frame(void)
{
    if (!IsWindowReady() || drawing) return false;
    BeginDrawing();
    drawing = true;
    ClearBackground(RAYWHITE);
    return true;
}

/**
 * @brief 提交已开始的帧，处理窗体事件。
 */
void chali_gui_end_frame(void)
{
    if (!drawing) return;
    EndDrawing();
    drawing = false;
}

/**
 * @brief 关闭当前窗口，并完成尚未提交的帧。
 */
void chali_gui_close(void)
{
    chali_gui_end_frame();
    if (IsWindowReady()) CloseWindow();
}

/**
 * @brief 在当前帧绘制标签。
 * @param x 左边坐标。
 * @param y 顶部坐标。
 * @param width 标签宽度。
 * @param height 标签高度。
 * @param text UTF-8 标签内容。
 */
void chali_gui_label(int32_t x, int32_t y, int32_t width, int32_t height, const char *text)
{
    if (drawing && width > 0 && height > 0) GuiLabel((Rectangle){x, y, width, height}, text);
}

/**
 * @brief 在当前帧绘制按钮并读取点击。
 * @param x 左边坐标。
 * @param y 顶部坐标。
 * @param width 按钮宽度。
 * @param height 按钮高度。
 * @param text UTF-8 按钮文字。
 * @return 当前帧点击时为真。
 */
bool chali_gui_button(int32_t x, int32_t y, int32_t width, int32_t height, const char *text)
{
    return drawing && width > 0 && height > 0 && GuiButton((Rectangle){x, y, width, height}, text);
}
