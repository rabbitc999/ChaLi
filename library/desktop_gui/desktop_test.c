/** @file desktop_test.c @brief 仅测试构建使用的进程内输入和帧捕获，不操作系统指针。 */
#include "desktop.h"
#include "raylib.h"
#include "rlgl.h"
#include <stdint.h>

#ifdef _WIN32
/* 只向本测试创建的隐藏窗口投递字符，不引入 Win32 与 raylib 的重名声明。 */
__declspec(dllimport) int __stdcall PostMessageW(void *window, unsigned int message, uintptr_t word, intptr_t value);
#endif
/**
 * @brief 回放锁定 raylib 5.5 的内部自动化输入，位置只改变 raylib 状态。
 * @param kind 零移动、一按下鼠标、二释放鼠标、三按下键、四释放键。
 * @param a 横坐标、按钮或键码。
 * @param b 纵坐标。
 * @return 输入合法且窗口属于当前线程时为真。
 */
CHALI_GUI_API bool chali_gui_test_input(int32_t kind, int32_t a, int32_t b)
{
    static const unsigned types[] = { 7, 6, 5, 2, 1 };
    AutomationEvent event = { 0 };
    if (!chali_gui_is_owner() || kind < 0 || kind > 4) return false;
    if (kind == 0 && (a < -10000000 || a > 10000000 || b < -10000000 || b > 10000000)) return false;
    if ((kind == 1 || kind == 2) && (a < 0 || a >= 8)) return false;
    if ((kind == 3 || kind == 4) && (a < 0 || a >= 512)) return false;
    event.type = types[kind];
    event.params[0] = a;
    event.params[1] = b;
    PlayAutomationEvent(event);
    return true;
}
/**
 * @brief 向本进程自己的隐藏窗口投递 Unicode 字符，交由真实输入回调处理。
 * @param codepoint 有效 Unicode 标量。
 * @return Windows 投递成功为真，其余平台返回假。
 */
CHALI_GUI_API bool chali_gui_test_character(int32_t codepoint)
{
    if (!chali_gui_is_owner() || codepoint < 0 || codepoint > 0x10ffff || (codepoint >= 0xd800 && codepoint <= 0xdfff)) return false;
#ifdef _WIN32
    if (codepoint <= 0xffff) return PostMessageW(GetWindowHandle(), 0x0102, (uintptr_t)codepoint, 0) != 0;
    codepoint -= 0x10000;
    return PostMessageW(GetWindowHandle(), 0x0102, 0xd800u + ((unsigned)codepoint >> 10), 0) && PostMessageW(GetWindowHandle(), 0x0102, 0xdc00u + ((unsigned)codepoint & 1023u), 0);
#else
    return false;
#endif
}
/**
 * @brief 捕获已绘制的隐藏帧，文件仅由测试调用方指定。
 * @param path PNG 输出路径。
 * @return 写入成功为真。
 */
CHALI_GUI_API bool chali_gui_test_capture(const char *path)
{
    Image image;
    bool result;
    if (!chali_gui_is_drawing() || !path || !*path) return false;
    rlDrawRenderBatchActive();
    image.width = GetRenderWidth();
    image.height = GetRenderHeight();
    image.mipmaps = 1;
    image.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    image.data = rlReadScreenPixels(image.width, image.height);
    result = image.data && ExportImage(image, path);
    UnloadImage(image);
    return result;
}
