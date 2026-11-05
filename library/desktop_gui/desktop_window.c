/**
 * @file desktop_window.c
 * @brief C 窗口、输入、裁剪与 UI 线程所有权。
 */
#include "desktop.h"
#include "desktop_draw.h"
#include "desktop_assets.h"
#include "raylib.h"
#include "rlgl.h"
#include "raygui.h"
#include <limits.h>
#include <math.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#include <windows.h>
typedef DWORD GuiThread;
#else
#include <pthread.h>
typedef pthread_t GuiThread;
#endif
static atomic_flag gate = ATOMIC_FLAG_INIT;
static GuiThread owner;
static bool owned, drawing, requested, closing_sent, opening_pending, collected, focused;
static int64_t window_id, next_window_id = 1;
static uint64_t frame_id;
static Color background = { 245, 245, 245, 255 };
static Font font;
static char *title_storage, *tooltip_storage;
static int min_width = 1, min_height = 1;
static Rectangle clips[32];
static unsigned clip_count, event_count, event_index;
static ChaliGuiEvent events[256];
static Vector2 last_mouse;
/**
 * @brief 锁定窗口身份。
 */
static void lock_owner(void)
{
    while (atomic_flag_test_and_set_explicit(&gate, memory_order_acquire)) {}
}
/**
 * @brief 释放身份锁。
 */
static void unlock_owner(void) { atomic_flag_clear_explicit(&gate, memory_order_release); }
/**
 * @brief 取得平台线程身份。
 * @return 当前线程。
 */
static GuiThread current_thread(void)
{
#ifdef _WIN32
    return GetCurrentThreadId();
#else
    return pthread_self();
#endif
}
/**
 * @brief 比较平台线程。
 * @param a 左值。
 * @param b 右值。
 * @return 相同为真。
 */
static bool same_thread(GuiThread a, GuiThread b)
{
#ifdef _WIN32
    return a == b;
#else
    return pthread_equal(a, b) != 0;
#endif
}
/**
 * @brief 核对调用线程拥有图形上下文。
 * @return 拥有为一。
 */
int chali_gui_is_owner(void)
{
    bool result;
    lock_owner();
    result = owned && same_thread(owner, current_thread());
    unlock_owner();
    return result;
}
/**
 * @brief 查询本线程是否在帧内。
 * @return 在帧内为一。
 */
int chali_gui_is_drawing(void) { return chali_gui_is_owner() && drawing; }
/**
 * @brief 查询绘制帧身份。
 * @return 帧身份。
 */
uint64_t chali_gui_frame_id(void) { return chali_gui_is_owner() ? frame_id : 0; }
/**
 * @brief 复制不含零的有界文本。
 * @param text 文本。
 * @param length 长度。
 * @return C 堆副本或空。
 */
static char *copy_text(const char *text, int64_t length)
{
    char *result;
    if (length < 0 || length > 1048576 || (length && (!text || memchr(text, 0, (size_t)length)))) return NULL;
    result = malloc((size_t)length + 1);
    if (result)
    {
        if (length) memcpy(result, text, (size_t)length);
        result[length] = 0;
    }
    return result;
}
/**
 * @brief 释放自定义字体并恢复当前默认字体。
 */
static void reset_font(void)
{
    if (drawing) rlDrawRenderBatchActive();
    GuiSetFont(GetFontDefault());
    if (font.texture.id) UnloadFont(font);
    memset(&font, 0, sizeof(font));
}
/**
 * @brief 创建兼容的普通窗口。
 * @param width 宽度。
 * @param height 高度。
 * @param title 标题。
 * @return 成功为真。
 */
bool chali_gui_open(int32_t width, int32_t height, const char *title) { return chali_gui_open_ex(width, height, title, 0); }
/**
 * @brief 创建窗口并捕获 UI 线程。
 * @param width 宽度。
 * @param height 高度。
 * @param title 标题。
 * @param options 选项。
 * @return 成功为真。
 */
bool chali_gui_open_ex(int32_t width, int32_t height, const char *title, int32_t options)
{
    unsigned flags = 0;
    char *copied;
    if (width <= 0 || height <= 0 || options < 0 || (options & ~7)) return false;
    if (!title) title = "";
    copied = copy_text(title, (int64_t)strlen(title));
    if (!copied) return false;
    lock_owner();
    if (owned || next_window_id == INT64_MAX)
    {
        unlock_owner();
        free(copied);
        return false;
    }
    owner = current_thread();
    owned = true;
    unlock_owner();
    if (options & 1) flags |= FLAG_WINDOW_RESIZABLE;
    if (options & 2) flags |= FLAG_WINDOW_HIDDEN;
    if (options & 4) flags |= FLAG_VSYNC_HINT;
    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(flags);
    InitWindow(width, height, copied);
    if (!IsWindowReady())
    {
        free(copied);
        lock_owner();
        owned = false;
        unlock_owner();
        return false;
    }
    title_storage = copied;
    background = RAYWHITE;
    window_id = next_window_id++;
    drawing = requested = closing_sent = collected = false;
    opening_pending = true;
    event_count = event_index = clip_count = 0;
    focused = IsWindowFocused();
    last_mouse = GetMousePosition();
    min_width = min_height = 1;
    GuiLoadStyleDefault();
    GuiSetFont(GetFontDefault());
    GuiEnable();
    GuiUnlock();
    GuiSetAlpha(1);
    SetTargetFPS(60);
    return true;
}
/**
 * @brief 查询当前实例。
 * @return 身份，无窗口为零。
 */
int64_t chali_gui_window_id(void) { return chali_gui_is_owner() && IsWindowReady() ? window_id : 0; }
/**
 * @brief 查询关闭请求。
 * @return 应退出为真。
 */
bool chali_gui_should_close(void) { return !chali_gui_is_owner() || !IsWindowReady() || requested || WindowShouldClose(); }
/**
 * @brief 请求在事件循环安全点退出。
 */
void chali_gui_request_close(void)
{
    if (chali_gui_is_owner()) requested = true;
}
/**
 * @brief 开始唯一绘制帧。
 * @return 成功为真。
 */
bool chali_gui_begin_frame(void)
{
    if (!chali_gui_is_owner() || !IsWindowReady() || drawing) return false;
    BeginDrawing();
    drawing = true;
    ++frame_id;
    ClearBackground(background);
    return true;
}
/**
 * @brief 提交绘制并复位本轮输入队列。
 */
void chali_gui_end_frame(void)
{
    if (!chali_gui_is_drawing()) return;
    if (clip_count)
    {
        EndScissorMode();
        clip_count = 0;
    }
    EndDrawing();
    drawing = false;
    collected = false;
    event_count = event_index = 0;
}
/**
 * @brief 释放窗口拥有的全部控件、字体与 C 文本。
 */
void chali_gui_close(void)
{
    if (!chali_gui_is_owner()) return;
    chali_gui_end_frame();
    chali_gui_widgets_close();
    chali_gui_textures_close();
    GuiSetTooltip(NULL);
    free(tooltip_storage);
    tooltip_storage = NULL;
    if (IsWindowReady())
    {
        reset_font();
        CloseWindow();
    }
    free(title_storage);
    title_storage = NULL;
    window_id = 0;
    lock_owner();
    owned = false;
    unlock_owner();
}
/**
 * @brief 只关闭原实例，避免回调重开后误关新窗口。
 * @param id 原窗口身份。
 */
void chali_gui_close_id(int64_t id)
{
    if (id && chali_gui_window_id() == id) chali_gui_close();
}
/**
 * @brief 释放 C ABI 拥有结果。
 * @param text 文本或空。
 */
void chali_gui_free(void *text) { free(text); }
/**
 * @brief 追加有界标量事件。
 * @param kind 类型。
 * @param code 键或按钮。
 * @param x 横坐标。
 * @param y 纵坐标。
 * @param dx 横向变化。
 * @param dy 纵向变化。
 */
static void push_event(int64_t kind, int64_t code, double x, double y, double dx, double dy)
{
    if (event_count < 256) events[event_count++] = (ChaliGuiEvent){ kind, window_id, code, x, y, dx, dy };
}
/**
 * @brief 每轮采集一次；控件消费后的剩余字符作为窗口文本事件。
 */
static void collect_events(void)
{
    Vector2 mouse = GetMousePosition(), wheel = GetMouseWheelMoveV();
    bool next_focus = IsWindowFocused();
    int code;
    collected = true;
    if (opening_pending)
    {
        push_event(CHALI_GUI_WINDOW_OPENED, 0, GetScreenWidth(), GetScreenHeight(), 0, 0);
        opening_pending = false;
    }
    if (!closing_sent && (requested || WindowShouldClose()))
    {
        push_event(CHALI_GUI_WINDOW_CLOSING, 0, 0, 0, 0, 0);
        closing_sent = true;
    }
    if (IsWindowResized()) push_event(CHALI_GUI_WINDOW_RESIZED, 0, GetScreenWidth(), GetScreenHeight(), 0, 0);
    if (focused != next_focus) push_event(next_focus ? CHALI_GUI_WINDOW_FOCUSED : CHALI_GUI_WINDOW_BLURRED, 0, 0, 0, 0, 0);
    focused = next_focus;
    if (mouse.x != last_mouse.x || mouse.y != last_mouse.y)
        push_event(CHALI_GUI_MOUSE_MOVED, 0, mouse.x, mouse.y, mouse.x - last_mouse.x, mouse.y - last_mouse.y);
    last_mouse = mouse;
    if (wheel.x || wheel.y) push_event(CHALI_GUI_MOUSE_WHEEL, 0, mouse.x, mouse.y, wheel.x, wheel.y);
    for (code = 0; code < 8; ++code)
    {
        if (IsMouseButtonPressed(code)) push_event(CHALI_GUI_MOUSE_PRESSED, code, mouse.x, mouse.y, 0, 0);
        if (IsMouseButtonReleased(code)) push_event(CHALI_GUI_MOUSE_RELEASED, code, mouse.x, mouse.y, 0, 0);
    }
    while ((code = GetKeyPressed())) push_event(CHALI_GUI_KEY_PRESSED, code, mouse.x, mouse.y, 0, 0);
    for (code = 0; code < 512; ++code)
        if (IsKeyReleased(code)) push_event(CHALI_GUI_KEY_RELEASED, code, mouse.x, mouse.y, 0, 0);
    while ((code = GetCharPressed())) push_event(CHALI_GUI_TEXT_ENTERED, code, mouse.x, mouse.y, 0, 0);
}
/**
 * @brief 读取下一条不含借用指针的事件。
 * @return 事件，零类型表示空。
 */
ChaliGuiEvent chali_gui_poll_event(void)
{
    if (!chali_gui_is_owner() || !IsWindowReady()) return (ChaliGuiEvent){ 0 };
    if (!collected) collect_events();
    return event_index < event_count ? events[event_index++] : (ChaliGuiEvent){ 0 };
}
/**
 * @brief 查询窗口、输入和时间标量。
 * @param property 参数。
 * @return 值，无效为零。
 */
double chali_gui_window_value(int32_t property)
{
    if (!chali_gui_is_owner() || !IsWindowReady()) return 0;
    switch (property)
    {
    case 0: return GetScreenWidth();
    case 1: return GetScreenHeight();
    case 2: return GetFrameTime();
    case 3: return GetMouseX();
    case 4: return GetMouseY();
    case 5: return GetMouseWheelMoveV().x;
    case 6: return GetMouseWheelMoveV().y;
    case 7: return GetWindowScaleDPI().x;
    case 8: return GetWindowScaleDPI().y;
    case 9: return IsWindowFocused();
    case 10: return GetTime();
    case 11: return GetFPS();
    default: return 0;
    }
}
/**
 * @brief 设置帧率、退出键、最小尺寸或透明度。
 * @param property 参数。
 * @param value 数值。
 * @return 成功为真。
 */
bool chali_gui_set_window_value(int32_t property, double value)
{
    if (!chali_gui_is_owner() || !IsWindowReady() || !isfinite(value)) return false;
    switch (property)
    {
    case 0:
        if (value < 0 || value > 1000) return false;
        SetTargetFPS((int)value);
        break;
    case 1:
        if (value < 0 || value >= 512) return false;
        SetExitKey((int)value);
        break;
    case 2:
    case 3:
        if (value < 1 || value > INT_MAX) return false;
        if (property == 2)
            min_width = (int)value;
        else
            min_height = (int)value;
        SetWindowMinSize(min_width, min_height);
        break;
    case 4:
        if (value < 0 || value > 1) return false;
        SetWindowOpacity((float)value);
        break;
    default: return false;
    }
    return true;
}
/**
 * @brief 复制并保留窗口标题。
 * @param title 字节。
 * @param length 长度。
 * @return 成功为真。
 */
bool chali_gui_set_title(const char *title, int64_t length)
{
    char *copy;
    if (!chali_gui_is_owner() || !IsWindowReady()) return false;
    copy = copy_text(title, length);
    if (!copy) return false;
    SetWindowTitle(copy);
    free(title_storage);
    title_storage = copy;
    return true;
}
/**
 * @brief 设置后续清屏色。
 * @param color RRGGBBAA。
 */
void chali_gui_background(uint32_t color)
{
    if (chali_gui_is_owner()) background = (Color){ color >> 24, color >> 16, color >> 8, color };
}
/**
 * @brief 开始与父视口相交的裁剪。
 * @param x 左边。
 * @param y 顶部。
 * @param width 宽度。
 * @param height 高度。
 * @return 成功为真。
 */
bool chali_gui_begin_clip(float x, float y, float width, float height)
{
    Rectangle rect = { x, y, width, height };
    if (!chali_gui_is_drawing() || clip_count == 32 || !isfinite(x) || !isfinite(y) || !isfinite(width) || !isfinite(height) || width < 0 || height < 0 || fabsf(x) > 10000000 || fabsf(y) > 10000000 || width > 10000000 || height > 10000000) return false;
    if (clip_count) rect = GetCollisionRec(clips[clip_count - 1], rect);
    clips[clip_count++] = rect;
    BeginScissorMode((int)rect.x, (int)rect.y, (int)rect.width, (int)rect.height);
    return true;
}
/**
 * @brief 恢复上一级裁剪。
 */
void chali_gui_end_clip(void)
{
    if (!chali_gui_is_drawing() || !clip_count) return;
    EndScissorMode();
    if (--clip_count)
    {
        Rectangle r = clips[clip_count - 1];
        BeginScissorMode((int)r.x, (int)r.y, (int)r.width, (int)r.height);
    }
}
/**
 * @brief 设置 GUI 全局状态。
 * @param operation 操作。
 * @param value 值。
 * @return 操作有效为真。
 */
bool chali_gui_configure(int32_t operation, double value)
{
    if (!chali_gui_is_owner() || !IsWindowReady() || !isfinite(value)) return false;
    switch (operation)
    {
    case 0:
        if (value)
            GuiEnable();
        else
            GuiDisable();
        break;
    case 1:
        if (value)
            GuiLock();
        else
            GuiUnlock();
        break;
    case 2:
        if (value < 0 || value > 1) return false;
        GuiSetAlpha((float)value);
        break;
    case 3:
        if (value < 0 || value > 3) return false;
        GuiSetState((int)value);
        break;
    case 4:
        if (value)
            GuiEnableTooltip();
        else
            GuiDisableTooltip();
        break;
    case 5:
        reset_font();
        GuiLoadStyleDefault();
        break;
    case 6:
        if (value < 1 || value > 256) return false;
        GuiSetStyle(DEFAULT, TEXT_SIZE, (int)value);
        break;
    default: return false;
    }
    return true;
}
/**
 * @brief 设置有界样式槽。
 * @param control 样式控件。
 * @param property 属性。
 * @param value 值。
 * @return 成功为真。
 */
bool chali_gui_set_style(int32_t control, int32_t property, int32_t value)
{
    if (!chali_gui_is_owner() || control < 0 || control >= 16 || property < 0 || property >= 24) return false;
    GuiSetStyle(control, property, value);
    return true;
}
/**
 * @brief 查询有界样式槽。
 * @param control 样式控件。
 * @param property 属性。
 * @return 值。
 */
int32_t chali_gui_get_style(int32_t control, int32_t property)
{ return chali_gui_is_owner() && control >= 0 && control < 16 && property >= 0 && property < 24 ? GuiGetStyle(control, property) : 0; }
/**
 * @brief 保留提示文字，避免 ABI 临时字符串悬垂。
 * @param text 字节。
 * @param length 长度。
 * @return 成功为真。
 */
bool chali_gui_tooltip(const char *text, int64_t length)
{
    char *copy;
    if (!chali_gui_is_owner()) return false;
    copy = copy_text(text, length);
    if (!copy) return false;
    GuiSetTooltip(copy);
    free(tooltip_storage);
    tooltip_storage = copy;
    return true;
}
/**
 * @brief 加载拥有型字体及可选 UTF-8 字符集合。
 * @param path 文件。
 * @param size 字号。
 * @param glyphs 字形集合。
 * @return 成功为真。
 */
bool chali_gui_load_font(const char *path, int32_t size, const char *glyphs)
{
    Font loaded;
    int count = 0, *points = NULL;
    unsigned char *bytes;
    size_t length;
    if (!chali_gui_is_owner() || !IsWindowReady() || !path || !*path || size < 1 || size > 256) return false;
    if (glyphs && *glyphs)
    {
        points = LoadCodepoints(glyphs, &count);
        if (!points) return false;
    }
    bytes = chali_gui_read_asset(path, &length);
    if (!bytes)
    {
        if (points) UnloadCodepoints(points);
        return false;
    }
    loaded = LoadFontFromMemory(GetFileExtension(path), bytes, (int)length, size, points, count);
    free(bytes);
    if (points) UnloadCodepoints(points);
    if (!loaded.texture.id || loaded.texture.id == GetFontDefault().texture.id) return false;
    reset_font();
    font = loaded;
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    GuiSetFont(font);
    GuiSetStyle(DEFAULT, TEXT_SIZE, size);
    return true;
}

/**
 * @brief 查询当前裁剪范围，供自定义控件保存交互区域。
 * @return 当前线程窗口的逻辑像素范围，无窗口时为空矩形。
 */
ChaliGuiBounds chali_gui_clip_bounds(void)
{
    Rectangle bounds;
    if (!chali_gui_is_owner()) return (ChaliGuiBounds){ 0, 0, 0, 0 };
    bounds = clip_count ? clips[clip_count - 1] : (Rectangle){ 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() };
    return (ChaliGuiBounds){ bounds.x, bounds.y, bounds.width, bounds.height };
}

/**
 * @brief 查询当前线程是否位于有效窗口的绘制帧。
 * @return 当前帧可绘制时为真。
 */
bool chali_gui_frame_active(void) { return chali_gui_is_drawing() != 0; }
