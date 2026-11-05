/**
 * @file desktop.h
 * @brief 桌面宿主的 C 控件状态和事件协议。
 */
#ifndef CHALI_DESKTOP_GUI_H
#define CHALI_DESKTOP_GUI_H
#include <stdbool.h>
#include <stdint.h>
#if defined(CHALI_GUI_STATIC)
#define CHALI_GUI_API
#elif defined(_WIN32)
#if defined(CHALI_GUI_BUILD)
#define CHALI_GUI_API __declspec(dllexport)
#else
#define CHALI_GUI_API __declspec(dllimport)
#endif
#elif defined(__GNUC__) || defined(__clang__)
#define CHALI_GUI_API __attribute__((visibility("default")))
#else
#define CHALI_GUI_API
#endif
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 与 controls.def 对应的完整控件类型。
 */
typedef enum ChaliGuiControlKind
{
#define CHALI_GUI_CONTROL(name, upstream) CHALI_GUI_##name,
#include "controls.def"
#undef CHALI_GUI_CONTROL
    CHALI_GUI_CONTROL_COUNT
} ChaliGuiControlKind;
/**
 * @brief 控件状态变化，可在一帧中组合。
 */
enum ChaliGuiControlEvent
{
    CHALI_GUI_CHANGED = 1,
    CHALI_GUI_CLICKED = 2,
    CHALI_GUI_SUBMITTED = 4,
    CHALI_GUI_EDIT_TOGGLED = 8,
    CHALI_GUI_CLOSED = 16,
    CHALI_GUI_SELECTED = 32,
    CHALI_GUI_SCROLLED = 64,
    CHALI_GUI_HOVERED = 128,
    CHALI_GUI_FOCUSED = 256
};
/**
 * @brief 可读写的整数、布尔及颜色状态槽。
 */
enum ChaliGuiIntegerProperty
{
    CHALI_GUI_ACTIVE,
    CHALI_GUI_INTEGER,
    CHALI_GUI_SCROLL,
    CHALI_GUI_FOCUS,
    CHALI_GUI_CHECKED,
    CHALI_GUI_EDITING,
    CHALI_GUI_SECRET,
    CHALI_GUI_VISIBLE,
    CHALI_GUI_ENABLED,
    CHALI_GUI_RESULT,
    CHALI_GUI_SUBDIVISIONS,
    CHALI_GUI_KIND,
    CHALI_GUI_COLOR,
    CHALI_GUI_INTEGER_MIN,
    CHALI_GUI_INTEGER_MAX
};
/**
 * @brief 可读写的浮点状态和只读视口坐标。
 */
enum ChaliGuiRealProperty
{
    CHALI_GUI_REAL,
    CHALI_GUI_REAL_MIN,
    CHALI_GUI_REAL_MAX,
    CHALI_GUI_SCROLL_X,
    CHALI_GUI_SCROLL_Y,
    CHALI_GUI_VIEW_X,
    CHALI_GUI_VIEW_Y,
    CHALI_GUI_VIEW_WIDTH,
    CHALI_GUI_VIEW_HEIGHT,
    CHALI_GUI_CONTENT_X,
    CHALI_GUI_CONTENT_Y,
    CHALI_GUI_CONTENT_WIDTH,
    CHALI_GUI_CONTENT_HEIGHT,
    CHALI_GUI_HUE,
    CHALI_GUI_SATURATION,
    CHALI_GUI_BRIGHTNESS,
    CHALI_GUI_SPACING
};
/**
 * @brief 控件文字、左右标签、消息和对话框按钮。
 */
enum ChaliGuiTextProperty
{
    CHALI_GUI_TEXT,
    CHALI_GUI_LEFT_TEXT,
    CHALI_GUI_RIGHT_TEXT,
    CHALI_GUI_MESSAGE,
    CHALI_GUI_BUTTONS
};
/**
 * @brief 窗口和输入事件类型，零为没有事件。
 */
enum ChaliGuiEventKind
{
    CHALI_GUI_NO_EVENT,
    CHALI_GUI_WINDOW_OPENED,
    CHALI_GUI_WINDOW_CLOSING,
    CHALI_GUI_WINDOW_RESIZED,
    CHALI_GUI_WINDOW_FOCUSED,
    CHALI_GUI_WINDOW_BLURRED,
    CHALI_GUI_KEY_PRESSED,
    CHALI_GUI_KEY_RELEASED,
    CHALI_GUI_TEXT_ENTERED,
    CHALI_GUI_MOUSE_PRESSED,
    CHALI_GUI_MOUSE_RELEASED,
    CHALI_GUI_MOUSE_MOVED,
    CHALI_GUI_MOUSE_WHEEL,
    CHALI_GUI_CONTROL_EVENT
};
/**
 * @brief 只含基本标量的跨 C ABI 事件值，不携带借用指针。
 */
typedef struct ChaliGuiEvent
{
    int64_t kind, source, code;
    double x, y, delta_x, delta_y;
} ChaliGuiEvent;

/**
 * @brief 查询是否由调用线程拥有窗口。
 * @return 拥有时为一。
 */
int chali_gui_is_owner(void);
/**
 * @brief 查询当前线程是否正在绘制。
 * @return 正在绘制为一。
 */
int chali_gui_is_drawing(void);
/**
 * @brief 返回当前帧的唯一序号。
 * @return 帧序号。
 */
CHALI_GUI_API uint64_t chali_gui_frame_id(void);
/**
 * @brief 清理窗口拥有的所有控件。
 */
void chali_gui_widgets_close(void);
/**
 * @brief 创建窗口。
 * @param width 宽度。
 * @param height 高度。
 * @param title 标题。
 * @return 成功为真。
 */
CHALI_GUI_API bool chali_gui_open(int32_t width, int32_t height, const char *title);
/**
 * @brief 创建带选项的窗口。
 * @param width 宽度。
 * @param height 高度。
 * @param title 标题。
 * @param options 位一可缩放、位二隐藏、位四垂直同步。
 * @return 成功为真。
 */
CHALI_GUI_API bool chali_gui_open_ex(int32_t width, int32_t height, const char *title, int32_t options);
/**
 * @brief 当前窗口的稳定身份。
 * @return 窗口身份，无窗口为零。
 */
CHALI_GUI_API int64_t chali_gui_window_id(void);
/**
 * @brief 结束尚未提交的帧并关闭当前窗口。
 */
CHALI_GUI_API void chali_gui_close(void);
/**
 * @brief 只关闭指定窗口实例，避免误关回调中重开的窗口。
 * @param id 窗口身份。
 */
CHALI_GUI_API void chali_gui_close_id(int64_t id);
/**
 * @brief 请求在事件循环安全点关闭。
 */
CHALI_GUI_API void chali_gui_request_close(void);
/**
 * @brief 查询关闭请求。
 * @return 应退出或线程无窗口为真。
 */
CHALI_GUI_API bool chali_gui_should_close(void);
/**
 * @brief 开始新帧。
 * @return 开始成功为真。
 */
CHALI_GUI_API bool chali_gui_begin_frame(void);
/**
 * @brief 提交当前帧，恢复裁剪并更新输入。
 */
CHALI_GUI_API void chali_gui_end_frame(void);
/**
 * @brief 在本线程当前帧绘制兼容的即时标签。
 * @param x 左边。
 * @param y 顶部。
 * @param width 宽度。
 * @param height 高度。
 * @param text UTF-8 文本。
 */
CHALI_GUI_API void chali_gui_label(int32_t x, int32_t y, int32_t width, int32_t height, const char *text);
/**
 * @brief 在本线程当前帧绘制兼容的即时按钮。
 * @param x 左边。
 * @param y 顶部。
 * @param width 宽度。
 * @param height 高度。
 * @param text UTF-8 文本。
 * @return 点击时为真。
 */
CHALI_GUI_API bool chali_gui_button(int32_t x, int32_t y, int32_t width, int32_t height, const char *text);
/**
 * @brief 取下一条未消费的事件。
 * @return 独立标量事件，没有事件时 kind 为零。
 */
CHALI_GUI_API ChaliGuiEvent chali_gui_poll_event(void);
/**
 * @brief 读取窗口或输入参数。
 * @param property 参数编号。
 * @return 参数值。
 */
CHALI_GUI_API double chali_gui_window_value(int32_t property);
/**
 * @brief 设置窗口参数。
 * @param property 参数编号。
 * @param value 参数值。
 * @return 有效时为真。
 */
CHALI_GUI_API bool chali_gui_set_window_value(int32_t property, double value);
/**
 * @brief 修改窗口标题。
 * @param title 标题。
 * @param length 字节数。
 * @return 成功为真。
 */
CHALI_GUI_API bool chali_gui_set_title(const char *title, int64_t length);
/**
 * @brief 设置默认清屏色。
 * @param color RRGGBBAA。
 */
CHALI_GUI_API void chali_gui_background(uint32_t color);
/**
 * @brief 开始可嵌套的裁剪。
 * @param x 左边。
 * @param y 顶部。
 * @param width 宽度。
 * @param height 高度。
 * @return 成功为真。
 */
CHALI_GUI_API bool chali_gui_begin_clip(float x, float y, float width, float height);
/**
 * @brief 恢复上一级裁剪。
 */
CHALI_GUI_API void chali_gui_end_clip(void);
/**
 * @brief 配置 GUI 全局状态。
 * @param operation 操作编号。
 * @param value 数值。
 * @return 操作有效为真。
 */
CHALI_GUI_API bool chali_gui_configure(int32_t operation, double value);
/**
 * @brief 设置样式。
 * @param control 控件样式编号。
 * @param property 属性。
 * @param value 值。
 * @return 有效为真。
 */
CHALI_GUI_API bool chali_gui_set_style(int32_t control, int32_t property, int32_t value);
/**
 * @brief 查询样式。
 * @param control 控件样式编号。
 * @param property 属性。
 * @return 值。
 */
CHALI_GUI_API int32_t chali_gui_get_style(int32_t control, int32_t property);
/**
 * @brief 复制并保留提示文字。
 * @param text 文字。
 * @param length 字节数。
 * @return 成功为真。
 */
CHALI_GUI_API bool chali_gui_tooltip(const char *text, int64_t length);
/**
 * @brief 加载拥有型字体。
 * @param path 文件路径。
 * @param size 字号。
 * @param glyphs 字符集合，空串使用默认字形。
 * @return 成功为真。
 */
CHALI_GUI_API bool chali_gui_load_font(const char *path, int32_t size, const char *glyphs);
/**
 * @brief 创建控件模型。
 * @param kind 控件类型。
 * @param text 文本。
 * @param length 字节数。
 * @return 唯一身份，失败为零。
 */
CHALI_GUI_API int64_t chali_gui_widget_create(int32_t kind, const char *text, int64_t length);
/**
 * @brief 核对控件仍然有效。
 * @param id 控件身份。
 * @return 有效为真。
 */
CHALI_GUI_API bool chali_gui_widget_valid(int64_t id);
/**
 * @brief 幂等销毁模型。
 * @param id 控件身份。
 */
CHALI_GUI_API void chali_gui_widget_close(int64_t id);
/**
 * @brief 设置整数或布尔属性。
 * @param id 控件身份。
 * @param property 属性编号。
 * @param value 值。
 * @return 成功为真。
 */
CHALI_GUI_API bool chali_gui_widget_set_integer(int64_t id, int32_t property, int64_t value);
/**
 * @brief 查询整数或布尔属性。
 * @param id 控件身份。
 * @param property 属性编号。
 * @return 值，无效为零。
 */
CHALI_GUI_API int64_t chali_gui_widget_integer(int64_t id, int32_t property);
/**
 * @brief 设置浮点属性。
 * @param id 控件身份。
 * @param property 属性编号。
 * @param value 值。
 * @return 成功为真。
 */
CHALI_GUI_API bool chali_gui_widget_set_real(int64_t id, int32_t property, double value);
/**
 * @brief 原子设置滑块或数值输入范围。
 * @param id 控件。
 * @param minimum 下限。
 * @param maximum 上限。
 * @return 成功为真。
 */
CHALI_GUI_API bool chali_gui_widget_range(int64_t id, double minimum, double maximum);
/**
 * @brief 查询浮点属性。
 * @param id 控件身份。
 * @param property 属性编号。
 * @return 值，无效为零。
 */
CHALI_GUI_API double chali_gui_widget_real(int64_t id, int32_t property);
/**
 * @brief 原子替换控件文本。
 * @param id 控件身份。
 * @param property 文本属性。
 * @param text UTF-8 字节。
 * @param length 字节数。
 * @return 成功为真。
 */
CHALI_GUI_API bool chali_gui_widget_set_text(int64_t id, int32_t property, const char *text, int64_t length);
/**
 * @brief 复制拥有型文本。
 * @param id 控件身份。
 * @param property 文本属性。
 * @return 由 chali_gui_free 释放的文本。
 */
CHALI_GUI_API char *chali_gui_widget_text(int64_t id, int32_t property);
/**
 * @brief 调整输入容量，失败保留原文本。
 * @param id 控件身份。
 * @param capacity 含终止零的字节容量。
 * @return 成功为真。
 */
CHALI_GUI_API bool chali_gui_widget_capacity(int64_t id, int32_t capacity);
/**
 * @brief 绘制一个模型并生成本帧状态事件。
 * @param id 控件身份。
 * @param x 左边。
 * @param y 顶部。
 * @param width 宽度。
 * @param height 高度。
 * @return 组合事件标记。
 */
CHALI_GUI_API int64_t chali_gui_widget_draw(int64_t id, float x, float y, float width, float height);
/**
 * @brief 释放宿主交付的文本。
 * @param text 文本或空指针。
 */
CHALI_GUI_API void chali_gui_free(void *text);
#ifdef __cplusplus
}
#endif
#endif
