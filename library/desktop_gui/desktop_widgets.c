/**
 * @file desktop_widgets.c
 * @brief 全部 raygui 4.0 控件的 C 模型、所有权与变化事件。
 */
#include "desktop.h"
#include "raylib.h"
#include "raygui.h"
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief 单窗口拥有的控件；全部动态存储使用 C 分配接口。
 */
typedef struct Widget
{
    struct Widget *next;
    int64_t id, pending;
    uint64_t last_frame;
    int kind, active, integer, scroll, focus, result, subdivisions, integer_min, integer_max;
    bool checked, editing, secret, visible, enabled;
    float real, minimum, maximum, spacing;
    Vector2 offset;
    Rectangle content, view;
    Vector3 hsv;
    Color color;
    char *text[5];
    char **items;
    int count, capacity;
} Widget;
static Widget *widgets;
static int64_t next_widget_id = 1;

/**
 * @brief 判断输入控件。
 * @param kind 类型。
 * @return 需要可写文本为真。
 */
static bool text_input(int kind) { return kind == CHALI_GUI_text_box || kind == CHALI_GUI_text_input_box; }
/**
 * @brief 判断多项选择控件。
 * @param kind 类型。
 * @return 需要项目清单为真。
 */
static bool item_control(int kind)
{ return kind == CHALI_GUI_tab_bar || kind == CHALI_GUI_toggle_group || kind == CHALI_GUI_toggle_slider || kind == CHALI_GUI_combo_box || kind == CHALI_GUI_dropdown_box || kind == CHALI_GUI_list_view || kind == CHALI_GUI_list_view_ex; }
/**
 * @brief 查找本线程拥有的有效身份。
 * @param id 身份。
 * @return 模型或空。
 */
static Widget *find_widget(int64_t id)
{
    Widget *at;
    if (!chali_gui_is_owner() || id <= 0) return NULL;
    for (at = widgets; at; at = at->next)
        if (at->id == id) return at;
    return NULL;
}
/**
 * @brief 读取不为空的模型文字。
 * @param widget 模型。
 * @param property 属性。
 * @return 借用文字。
 */
static const char *text_at(const Widget *widget, int property) { return widget->text[property] ? widget->text[property] : ""; }
/**
 * @brief 分割清单，字符串与指针表存放在同一个 C 块。
 * @param text 清单。
 * @param length 长度。
 * @param count 数量输出。
 * @return 清单，空清单或失败为空。
 */
static char **split_items(const char *text, size_t length, int *count)
{
    size_t index;
    int total = length ? 1 : 0, at = 0;
    char **items;
    char *storage;
    for (index = 0; index < length; ++index)
        if (text[index] == ';' || text[index] == '\n') ++total;
    *count = total;
    if (!total) return NULL;
    if (total > 128)
    {
        *count = -1;
        return NULL;
    }
    items = malloc((size_t)total * sizeof(*items) + length + 1);
    if (!items)
    {
        *count = -1;
        return NULL;
    }
    storage = (char *)(items + total);
    memcpy(storage, text, length);
    storage[length] = 0;
    items[at++] = storage;
    for (index = 0; index < length; ++index)
        if (storage[index] == ';' || storage[index] == '\n')
        {
            storage[index] = 0;
            items[at++] = storage + index + 1;
        }
    return items;
}
/**
 * @brief 原子替换文字及清单，输入模型保留固定容量。
 * @param widget 模型。
 * @param property 属性。
 * @param text 字节。
 * @param length 长度。
 * @return 成功为真。
 */
static bool replace_text(Widget *widget, int property, const char *text, int64_t length)
{
    char *copy;
    char **items = NULL;
    int count = 0;
    if (property < 0 || property >= 5 || length < 0 || length > 1048576 || (length && (!text || memchr(text, 0, (size_t)length)))) return false;
    if (property == 0 && text_input(widget->kind))
    {
        if (length >= widget->capacity) return false;
        if (length) memcpy(widget->text[0], text, (size_t)length);
        widget->text[0][length] = 0;
        return true;
    }
    if (property == 0 && item_control(widget->kind))
    {
        items = split_items(text, (size_t)length, &count);
        if (count < 0) return false;
    }
    copy = malloc((size_t)length + 1);
    if (!copy)
    {
        free(items);
        return false;
    }
    if (length) memcpy(copy, text, (size_t)length);
    copy[length] = 0;
    free(widget->text[property]);
    widget->text[property] = copy;
    if (property == 0 && item_control(widget->kind))
    {
        free(widget->items);
        widget->items = items;
        widget->count = count;
        if (!count)
            widget->active = -1;
        else if (widget->active >= count)
            widget->active = count - 1;
        if (widget->scroll >= count) widget->scroll = count ? count - 1 : 0;
        if (widget->focus >= count) widget->focus = -1;
    }
    return true;
}
/**
 * @brief 释放模型及所有独占文字。
 * @param widget 模型。
 */
static void destroy_widget(Widget *widget)
{
    int index;
    for (index = 0; index < 5; ++index) free(widget->text[index]);
    free(widget->items);
    free(widget);
}
/**
 * @brief 创建稳定且不复用的控件身份。
 * @param kind 类型。
 * @param text 初始文字。
 * @param length 长度。
 * @return 身份，失败为零。
 */
int64_t chali_gui_widget_create(int32_t kind, const char *text, int64_t length)
{
    Widget *widget;
    if (!chali_gui_is_owner() || kind < 0 || kind >= CHALI_GUI_CONTROL_COUNT || next_widget_id == INT64_MAX || length < 0 || length >= 1048576) return 0;
    widget = calloc(1, sizeof(*widget));
    if (!widget) return 0;
    widget->kind = kind;
    widget->id = next_widget_id;
    widget->last_frame = UINT64_MAX;
    widget->visible = widget->enabled = true;
    widget->maximum = 1;
    widget->integer_max = 100;
    widget->subdivisions = 4;
    widget->spacing = 20;
    widget->focus = widget->result = -1;
    widget->active = kind == CHALI_GUI_list_view || kind == CHALI_GUI_list_view_ex ? -1 : 0;
    widget->content = (Rectangle){ 0, 0, 640, 480 };
    widget->color = (Color){ 255, 0, 0, 255 };
    widget->hsv = (Vector3){ 0, 1, 1 };
    if (kind == CHALI_GUI_color_bar_alpha) widget->real = 1;
    if (kind == CHALI_GUI_color_bar_hue) widget->maximum = 360;
    if (text_input(kind))
    {
        widget->capacity = length < 256 ? 256 : (int)length + 1;
        widget->text[0] = calloc((size_t)widget->capacity * 2, 1);
        if (!widget->text[0])
        {
            destroy_widget(widget);
            return 0;
        }
    }
    if (!replace_text(widget, 0, text, length))
    {
        destroy_widget(widget);
        return 0;
    }
    widget->next = widgets;
    widgets = widget;
    ++next_widget_id;
    return widget->id;
}
/**
 * @brief 查询身份是否有效。
 * @param id 身份。
 * @return 有效为真。
 */
bool chali_gui_widget_valid(int64_t id) { return find_widget(id) != NULL; }
/**
 * @brief 按稳定身份幂等销毁。
 * @param id 身份。
 */
void chali_gui_widget_close(int64_t id)
{
    Widget **at;
    if (!chali_gui_is_owner()) return;
    for (at = &widgets; *at; at = &(*at)->next)
        if ((*at)->id == id)
        {
            Widget *widget = *at;
            *at = widget->next;
            destroy_widget(widget);
            return;
        }
}
/**
 * @brief 关闭窗口时释放全部模型，旧身份不会复用。
 */
void chali_gui_widgets_close(void)
{
    if (!chali_gui_is_owner()) return;
    while (widgets)
    {
        Widget *next = widgets->next;
        destroy_widget(widgets);
        widgets = next;
    }
}
/**
 * @brief 让文本编辑焦点互斥。
 * @param widget 新焦点。
 * @param notify 是否通知旧焦点。
 */
static void focus_editor(Widget *widget, bool notify)
{
    Widget *at;
    for (at = widgets; at; at = at->next)
        if (at != widget && at->editing)
        {
            at->editing = false;
            if (notify) at->pending |= CHALI_GUI_EDIT_TOGGLED | CHALI_GUI_FOCUSED;
        }
}
/**
 * @brief 读取整数、布尔及颜色属性。
 * @param id 身份。
 * @param property 属性。
 * @return 值。
 */
int64_t chali_gui_widget_integer(int64_t id, int32_t property)
{
    Widget *w = find_widget(id);
    if (!w) return 0;
    switch (property)
    {
    case CHALI_GUI_ACTIVE: return w->active;
    case CHALI_GUI_INTEGER: return w->integer;
    case CHALI_GUI_SCROLL: return w->scroll;
    case CHALI_GUI_FOCUS: return w->focus;
    case CHALI_GUI_CHECKED: return w->checked;
    case CHALI_GUI_EDITING: return w->editing;
    case CHALI_GUI_SECRET: return w->secret;
    case CHALI_GUI_VISIBLE: return w->visible;
    case CHALI_GUI_ENABLED: return w->enabled;
    case CHALI_GUI_RESULT: return w->result;
    case CHALI_GUI_SUBDIVISIONS: return w->subdivisions;
    case CHALI_GUI_KIND: return w->kind;
    case CHALI_GUI_COLOR: return (uint32_t)w->color.r << 24 | (uint32_t)w->color.g << 16 | (uint32_t)w->color.b << 8 | w->color.a;
    case CHALI_GUI_INTEGER_MIN: return w->integer_min;
    case CHALI_GUI_INTEGER_MAX: return w->integer_max;
    default: return 0;
    }
}
/**
 * @brief 验证后设置整数或布尔属性。
 * @param id 身份。
 * @param property 属性。
 * @param value 值。
 * @return 成功为真。
 */
bool chali_gui_widget_set_integer(int64_t id, int32_t property, int64_t value)
{
    Widget *w = find_widget(id);
    if (!w) return false;
    if (property == CHALI_GUI_COLOR)
    {
        uint32_t color;
        if (value < 0 || (uint64_t)value > UINT32_MAX) return false;
        color = (uint32_t)value;
        w->color = (Color){ color >> 24, color >> 16, color >> 8, color };
        return true;
    }
    if (value < INT_MIN || value > INT_MAX) return false;
    switch (property)
    {
    case CHALI_GUI_ACTIVE:
        if (value < -1 || (item_control(w->kind) && value >= w->count)) return false;
        w->active = (int)value;
        break;
    case CHALI_GUI_INTEGER:
        if (value < w->integer_min || value > w->integer_max) return false;
        w->integer = (int)value;
        break;
    case CHALI_GUI_SCROLL:
        if (value < 0) return false;
        w->scroll = (int)value;
        break;
    case CHALI_GUI_FOCUS:
        if (value < -1 || value >= w->count) return false;
        w->focus = (int)value;
        break;
    case CHALI_GUI_SUBDIVISIONS:
        if (value < 1 || value > 64) return false;
        w->subdivisions = (int)value;
        break;
    case CHALI_GUI_INTEGER_MIN:
        if (value > w->integer_max) return false;
        w->integer_min = (int)value;
        if (w->integer < value) w->integer = (int)value;
        break;
    case CHALI_GUI_INTEGER_MAX:
        if (value < w->integer_min) return false;
        w->integer_max = (int)value;
        if (w->integer > value) w->integer = (int)value;
        break;
    case CHALI_GUI_CHECKED:
    case CHALI_GUI_EDITING:
    case CHALI_GUI_SECRET:
    case CHALI_GUI_VISIBLE:
    case CHALI_GUI_ENABLED:
        if (value != 0 && value != 1) return false;
        if (property == CHALI_GUI_CHECKED) w->checked = value != 0;
        if (property == CHALI_GUI_EDITING)
        {
            w->editing = value != 0;
            if (w->editing) focus_editor(w, true);
        }
        if (property == CHALI_GUI_SECRET) w->secret = value != 0;
        if (property == CHALI_GUI_VISIBLE) w->visible = value != 0;
        if (property == CHALI_GUI_ENABLED) w->enabled = value != 0;
        break;
    default: return false;
    }
    return true;
}
/**
 * @brief 读取数值、滚动、内容或视口。
 * @param id 身份。
 * @param property 属性。
 * @return 值。
 */
double chali_gui_widget_real(int64_t id, int32_t property)
{
    Widget *w = find_widget(id);
    if (!w) return 0;
    switch (property)
    {
    case CHALI_GUI_REAL: return w->real;
    case CHALI_GUI_REAL_MIN: return w->minimum;
    case CHALI_GUI_REAL_MAX: return w->maximum;
    case CHALI_GUI_SCROLL_X: return w->offset.x;
    case CHALI_GUI_SCROLL_Y: return w->offset.y;
    case CHALI_GUI_VIEW_X: return w->view.x;
    case CHALI_GUI_VIEW_Y: return w->view.y;
    case CHALI_GUI_VIEW_WIDTH: return w->view.width;
    case CHALI_GUI_VIEW_HEIGHT: return w->view.height;
    case CHALI_GUI_CONTENT_X: return w->content.x;
    case CHALI_GUI_CONTENT_Y: return w->content.y;
    case CHALI_GUI_CONTENT_WIDTH: return w->content.width;
    case CHALI_GUI_CONTENT_HEIGHT: return w->content.height;
    case CHALI_GUI_HUE: return w->hsv.x;
    case CHALI_GUI_SATURATION: return w->hsv.y;
    case CHALI_GUI_BRIGHTNESS: return w->hsv.z;
    case CHALI_GUI_SPACING: return w->spacing;
    default: return 0;
    }
}
/**
 * @brief 原子配置连续或整数范围。
 * @param id 身份。
 * @param minimum 下限。
 * @param maximum 上限。
 * @return 成功为真。
 */
bool chali_gui_widget_range(int64_t id, double minimum, double maximum)
{
    Widget *w = find_widget(id);
    if (!w || !isfinite(minimum) || !isfinite(maximum) || fabs(minimum) > FLT_MAX || fabs(maximum) > FLT_MAX) return false;
    if (w->kind == CHALI_GUI_spinner || w->kind == CHALI_GUI_value_box)
    {
        if (minimum < INT_MIN || maximum > INT_MAX || minimum > maximum || floor(minimum) != minimum || floor(maximum) != maximum) return false;
        w->integer_min = (int)minimum;
        w->integer_max = (int)maximum;
        if (w->integer < w->integer_min) w->integer = w->integer_min;
        if (w->integer > w->integer_max) w->integer = w->integer_max;
    }
    else
    {
        if (minimum >= maximum || (float)minimum >= (float)maximum) return false;
        w->minimum = (float)minimum;
        w->maximum = (float)maximum;
        if (w->real < w->minimum) w->real = w->minimum;
        if (w->real > w->maximum) w->real = w->maximum;
    }
    return true;
}
/**
 * @brief 设置合法浮点属性，不接受 NaN 或无穷。
 * @param id 身份。
 * @param property 属性。
 * @param value 值。
 * @return 成功为真。
 */
bool chali_gui_widget_set_real(int64_t id, int32_t property, double value)
{
    Widget *w = find_widget(id);
    float v;
    if (!w || !isfinite(value) || fabs(value) > FLT_MAX) return false;
    v = (float)value;
    switch (property)
    {
    case CHALI_GUI_REAL:
        if (v < w->minimum || v > w->maximum) return false;
        w->real = v;
        break;
    case CHALI_GUI_REAL_MIN: return chali_gui_widget_range(id, value, w->maximum);
    case CHALI_GUI_REAL_MAX: return chali_gui_widget_range(id, w->minimum, value);
    case CHALI_GUI_SCROLL_X:
    case CHALI_GUI_SCROLL_Y:
    case CHALI_GUI_CONTENT_X:
    case CHALI_GUI_CONTENT_Y:
    case CHALI_GUI_CONTENT_WIDTH:
    case CHALI_GUI_CONTENT_HEIGHT:
        if (fabs(value) > 10000000 || ((property == CHALI_GUI_CONTENT_WIDTH || property == CHALI_GUI_CONTENT_HEIGHT) && value <= 0)) return false;
        if (property == CHALI_GUI_SCROLL_X) w->offset.x = v;
        if (property == CHALI_GUI_SCROLL_Y) w->offset.y = v;
        if (property == CHALI_GUI_CONTENT_X) w->content.x = v;
        if (property == CHALI_GUI_CONTENT_Y) w->content.y = v;
        if (property == CHALI_GUI_CONTENT_WIDTH) w->content.width = v;
        if (property == CHALI_GUI_CONTENT_HEIGHT) w->content.height = v;
        break;
    case CHALI_GUI_HUE:
        if (v < 0 || v > 360) return false;
        w->hsv.x = v;
        break;
    case CHALI_GUI_SATURATION:
    case CHALI_GUI_BRIGHTNESS:
        if (v < 0 || v > 1) return false;
        if (property == CHALI_GUI_SATURATION)
            w->hsv.y = v;
        else
            w->hsv.z = v;
        break;
    case CHALI_GUI_SPACING:
        if (v <= 0 || v > 10000000) return false;
        w->spacing = v;
        break;
    default: return false;
    }
    return true;
}
/**
 * @brief 设置拥有型文字。
 * @param id 身份。
 * @param property 属性。
 * @param text 字节。
 * @param length 长度。
 * @return 成功为真。
 */
bool chali_gui_widget_set_text(int64_t id, int32_t property, const char *text, int64_t length)
{
    Widget *w = find_widget(id);
    return w && replace_text(w, property, text, length);
}
/**
 * @brief 返回独立文字副本。
 * @param id 身份。
 * @param property 属性。
 * @return C 堆文本或空。
 */
char *chali_gui_widget_text(int64_t id, int32_t property)
{
    Widget *w = find_widget(id);
    const char *text;
    size_t length;
    char *copy;
    if (!w || property < 0 || property >= 5) return NULL;
    text = text_at(w, property);
    length = strlen(text);
    copy = malloc(length + 1);
    if (copy) memcpy(copy, text, length + 1);
    return copy;
}
/**
 * @brief 调整输入容量，过小或分配失败时保留原数据。
 * @param id 身份。
 * @param capacity 字节容量。
 * @return 成功为真。
 */
bool chali_gui_widget_capacity(int64_t id, int32_t capacity)
{
    Widget *w = find_widget(id);
    char *copy;
    size_t length;
    if (!w || !text_input(w->kind) || capacity < 2 || capacity > 1048576) return false;
    length = strlen(w->text[0]);
    if (length >= (size_t)capacity) return false;
    copy = calloc((size_t)capacity * 2, 1);
    if (!copy) return false;
    memcpy(copy, w->text[0], length + 1);
    free(w->text[0]);
    w->text[0] = copy;
    w->capacity = capacity;
    return true;
}
/**
 * @brief 绘制上游全部 34 种控件，一帧内相同身份只处理一次。
 * @param id 身份。
 * @param x 左边。
 * @param y 顶部。
 * @param width 宽度。
 * @param height 高度。
 * @return 组合变化事件。
 */
int64_t chali_gui_widget_draw(int64_t id, float x, float y, float width, float height)
{
    Widget *w = find_widget(id);
    Rectangle bounds = { x, y, width, height };
    int result = 0, state;
    int active, number, scroll, focus;
    bool checked, editing, secret;
    float real;
    Vector2 offset;
    Vector3 hsv;
    Color color;
    int64_t flags;
    const char *text, *left, *right;
    if (!w || !chali_gui_is_drawing() || !w->visible || w->last_frame == chali_gui_frame_id() || !isfinite(x) || !isfinite(y) || !isfinite(width) || !isfinite(height) || width <= 0 || height <= 0 || fabsf(x) > 10000000 || fabsf(y) > 10000000 || width > 10000000 || height > 10000000) return 0;
    if (item_control(w->kind) && !w->count) return 0;
    w->last_frame = chali_gui_frame_id();
    flags = w->pending;
    w->pending = 0;
    active = w->active;
    number = w->integer;
    scroll = w->scroll;
    focus = w->focus;
    checked = w->checked;
    editing = w->editing;
    secret = w->secret;
    real = w->real;
    offset = w->offset;
    hsv = w->hsv;
    color = w->color;
    text = text_at(w, 0);
    left = text_at(w, 1);
    right = text_at(w, 2);
    if (text_input(w->kind)) memcpy(w->text[0] + w->capacity, w->text[0], strlen(w->text[0]) + 1);
    state = GuiGetState();
    if (!w->enabled) GuiSetState(STATE_DISABLED);
    switch (w->kind)
    {
    case CHALI_GUI_window_box:
        result = GuiWindowBox(bounds, text);
        if (result)
        {
            flags |= CHALI_GUI_CLOSED | CHALI_GUI_CLICKED;
            w->visible = false;
        }
        break;
    case CHALI_GUI_group_box: GuiGroupBox(bounds, text); break;
    case CHALI_GUI_line: GuiLine(bounds, text); break;
    case CHALI_GUI_panel: GuiPanel(bounds, text); break;
    case CHALI_GUI_tab_bar:
        result = GuiTabBar(bounds, (const char **)w->items, w->count, &w->active);
        if (result >= 0) flags |= CHALI_GUI_CLOSED | CHALI_GUI_CLICKED;
        break;
    case CHALI_GUI_scroll_panel: GuiScrollPanel(bounds, text, w->content, &w->offset, &w->view); break;
    case CHALI_GUI_label: GuiLabel(bounds, text); break;
    case CHALI_GUI_button:
        result = GuiButton(bounds, text);
        if (result) flags |= CHALI_GUI_CLICKED;
        break;
    case CHALI_GUI_label_button:
        result = GuiLabelButton(bounds, text);
        if (result) flags |= CHALI_GUI_CLICKED;
        break;
    case CHALI_GUI_toggle: GuiToggle(bounds, text, &w->checked); break;
    case CHALI_GUI_toggle_group: GuiToggleGroup(bounds, text, &w->active); break;
    case CHALI_GUI_toggle_slider:
        result = GuiToggleSlider(bounds, text, &w->active);
        if (result) flags |= CHALI_GUI_CLICKED;
        break;
    case CHALI_GUI_check_box: GuiCheckBox(bounds, text, &w->checked); break;
    case CHALI_GUI_combo_box: GuiComboBox(bounds, text, &w->active); break;
    case CHALI_GUI_dropdown_box:
        result = GuiDropdownBox(bounds, text, &w->active, w->editing);
        if (result) w->editing = !w->editing;
        break;
    case CHALI_GUI_spinner:
        result = GuiSpinner(bounds, text, &w->integer, w->integer_min, w->integer_max, w->editing);
        if (result) w->editing = !w->editing;
        break;
    case CHALI_GUI_value_box:
        result = GuiValueBox(bounds, text, &w->integer, w->integer_min, w->integer_max, w->editing);
        if (result) w->editing = !w->editing;
        break;
    case CHALI_GUI_text_box:
        result = GuiTextBox(bounds, w->text[0], w->capacity, w->editing);
        if (result) w->editing = !w->editing;
        break;
    case CHALI_GUI_slider: GuiSlider(bounds, left, right, &w->real, w->minimum, w->maximum); break;
    case CHALI_GUI_slider_bar: GuiSliderBar(bounds, left, right, &w->real, w->minimum, w->maximum); break;
    case CHALI_GUI_progress_bar: GuiProgressBar(bounds, left, right, &w->real, w->minimum, w->maximum); break;
    case CHALI_GUI_status_bar: GuiStatusBar(bounds, text); break;
    case CHALI_GUI_dummy_rec: GuiDummyRec(bounds, text); break;
    case CHALI_GUI_grid: GuiGrid(bounds, text, w->spacing, w->subdivisions, &w->offset); break;
    case CHALI_GUI_list_view: GuiListView(bounds, text, &w->scroll, &w->active); break;
    case CHALI_GUI_list_view_ex: GuiListViewEx(bounds, (const char **)w->items, w->count, &w->scroll, &w->active, &w->focus); break;
    case CHALI_GUI_message_box:
        result = GuiMessageBox(bounds, text, text_at(w, 3), w->text[4] ? w->text[4] : "OK");
        if (result >= 0)
        {
            flags |= CHALI_GUI_CLICKED | (result ? CHALI_GUI_SUBMITTED : CHALI_GUI_CLOSED);
            w->visible = false;
        }
        break;
    case CHALI_GUI_text_input_box:
        result = GuiTextInputBox(bounds, *left ? left : "Input", text_at(w, 3), w->text[4] ? w->text[4] : "OK", w->text[0], w->capacity, &w->secret);
        if (result >= 0)
        {
            flags |= CHALI_GUI_CLICKED | (result ? CHALI_GUI_SUBMITTED : CHALI_GUI_CLOSED);
            w->visible = false;
        }
        break;
    case CHALI_GUI_color_picker: GuiColorPicker(bounds, text, &w->color); break;
    case CHALI_GUI_color_panel: GuiColorPanel(bounds, text, &w->color); break;
    case CHALI_GUI_color_bar_alpha: GuiColorBarAlpha(bounds, text, &w->real); break;
    case CHALI_GUI_color_bar_hue: GuiColorBarHue(bounds, text, &w->real); break;
    case CHALI_GUI_color_picker_hsv: GuiColorPickerHSV(bounds, text, &w->hsv); break;
    case CHALI_GUI_color_panel_hsv: GuiColorPanelHSV(bounds, text, &w->hsv); break;
    default: break;
    }
    GuiSetState(state);
    w->result = result;
    if (active != w->active) flags |= CHALI_GUI_CHANGED | CHALI_GUI_SELECTED;
    if (number != w->integer || real != w->real || secret != w->secret || memcmp(&color, &w->color, sizeof(color)) || memcmp(&hsv, &w->hsv, sizeof(hsv))) flags |= CHALI_GUI_CHANGED;
    if (checked != w->checked) flags |= CHALI_GUI_CHANGED | CHALI_GUI_CLICKED;
    if (scroll != w->scroll || offset.x != w->offset.x || offset.y != w->offset.y) flags |= w->kind == CHALI_GUI_grid ? CHALI_GUI_HOVERED : CHALI_GUI_SCROLLED;
    if (focus != w->focus) flags |= CHALI_GUI_FOCUSED;
    if (editing != w->editing)
    {
        flags |= CHALI_GUI_EDIT_TOGGLED | CHALI_GUI_FOCUSED;
        if (w->editing && (text_input(w->kind) || w->kind == CHALI_GUI_spinner || w->kind == CHALI_GUI_value_box)) focus_editor(w, true);
        if (editing && !w->editing && IsKeyPressed(KEY_ENTER)) flags |= CHALI_GUI_SUBMITTED;
    }
    if (text_input(w->kind) && strcmp(w->text[0], w->text[0] + w->capacity)) flags |= CHALI_GUI_CHANGED;
    return flags;
}
