/**
 * @file desktop_draw.c
 * @brief 独立 C 绘图和纹理接口，资源由窗口生命周期管理。
 */
#include "desktop_draw.h"
#include "raylib.h"
#include "raygui.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief 把 RRGGBBAA 转成 raylib 颜色值。
 * @param value 要检查或转换的数值。
 * @return 逐通道解码的颜色值。
 */
static Color color_value(uint32_t value)
{ return (Color){ (unsigned char)(value >> 24), (unsigned char)(value >> 16), (unsigned char)(value >> 8), (unsigned char)value }; }
/**
 * @brief 检查有限且绝对值不超过一千万的坐标。
 * @param value 要检查或转换的数值。
 * @return 参数、当前线程及操作状态有效时为真，否则为假。
 */
static int coordinate(float value) { return isfinite(value) && fabsf(value) <= 10000000.0f; }
/**
 * @brief 检查有限的正尺寸。
 * @param value 要检查或转换的数值。
 * @return 参数、当前线程及操作状态有效时为真，否则为假。
 */
static int positive(float value) { return isfinite(value) && value > 0 && value <= 10000000.0f; }
/**
 * @brief 检查零填充或有限非负描边宽度。
 * @param value 要检查或转换的数值。
 * @return 参数、当前线程及操作状态有效时为真，否则为假。
 */
static int stroke(float value) { return isfinite(value) && value >= 0 && value <= 100000.0f; }
/**
 * @brief 检查非空指针的零结尾文本，最多一 MiB。
 * @param text 调用期间借用的零结尾 UTF-8 文本。
 * @return 参数、当前线程及操作状态有效时为真，否则为假。
 */
static int text_valid(const char *text)
{
    size_t n;
    if (!text) return 0;
    for (n = 0; n <= 1048576; n++)
        if (!text[n]) return 1;
    return 0;
}
/**
 * @brief 检查字号与字符间距。
 * @param size 正字号。
 * @param spacing 字符间距。
 * @return 参数、当前线程及操作状态有效时为真，否则为假。
 */
static int font_parameters(float size, float spacing)
{ return positive(size) && size <= 100000 && coordinate(spacing) && fabsf(spacing) <= 10000; }
/**
 * @brief 在当前帧绘制逻辑像素。
 * @param x 逻辑像素横坐标。
 * @param y 逻辑像素纵坐标。
 * @param color RRGGBBAA 颜色。
 * @return 参数、当前线程及操作状态有效时为真，否则为假。
 */
bool chali_gui_draw_pixel(float x, float y, uint32_t color)
{
    if (!chali_gui_is_drawing() || !coordinate(x) || !coordinate(y)) return false;
    DrawPixelV((Vector2){ x, y }, color_value(color));
    return true;
}
/**
 * @brief 在当前帧绘制有宽度的线段。
 * @param x1 起点或第一顶点横坐标。
 * @param y1 起点或第一顶点纵坐标。
 * @param x2 终点或第二顶点横坐标。
 * @param y2 终点或第二顶点纵坐标。
 * @param thickness 描边宽度；支持填充的图形以零表示填充。
 * @param color RRGGBBAA 颜色。
 * @return 参数、当前线程及操作状态有效时为真，否则为假。
 */
bool chali_gui_draw_line(float x1, float y1, float x2, float y2, float thickness, uint32_t color)
{
    if (!chali_gui_is_drawing() || !coordinate(x1) || !coordinate(y1) || !coordinate(x2) || !coordinate(y2) || !positive(thickness) || !stroke(thickness)) return false;
    DrawLineEx((Vector2){ x1, y1 }, (Vector2){ x2, y2 }, thickness, color_value(color));
    return true;
}
/**
 * @brief 绘制填充、边框或圆角矩形。
 * @param x 逻辑像素横坐标。
 * @param y 逻辑像素纵坐标。
 * @param width 目标宽度。
 * @param height 目标高度。
 * @param roundness 圆角比例，范围为零到一。
 * @param segments 有界的曲线细分数。
 * @param thickness 描边宽度；支持填充的图形以零表示填充。
 * @param color RRGGBBAA 颜色。
 * @return 参数、当前线程及操作状态有效时为真，否则为假。
 */
bool chali_gui_draw_rectangle(float x, float y, float width, float height, float roundness, int32_t segments, float thickness, uint32_t color)
{
    Rectangle bounds = { x, y, width, height };
    Color rgba = color_value(color);
    if (!chali_gui_is_drawing() || !coordinate(x) || !coordinate(y) || !positive(width) || !positive(height) || !isfinite(roundness) || roundness < 0 || roundness > 1 || segments < 0 || segments > 256 || !stroke(thickness)) return false;
    if (roundness == 0)
    {
        if (thickness == 0)
            DrawRectangleRec(bounds, rgba);
        else
            DrawRectangleLinesEx(bounds, thickness, rgba);
    }
    else
    {
        if (thickness == 0)
            DrawRectangleRounded(bounds, roundness, segments, rgba);
        else
            DrawRectangleRoundedLinesEx(bounds, roundness, segments, thickness, rgba);
    }
    return true;
}
/**
 * @brief 绘制填充圆或环形边框。
 * @param x 逻辑像素横坐标。
 * @param y 逻辑像素纵坐标。
 * @param radius 正半径。
 * @param thickness 描边宽度；支持填充的图形以零表示填充。
 * @param segments 有界的曲线细分数。
 * @param color RRGGBBAA 颜色。
 * @return 参数、当前线程及操作状态有效时为真，否则为假。
 */
bool chali_gui_draw_circle(float x, float y, float radius, float thickness, int32_t segments, uint32_t color)
{
    if (!chali_gui_is_drawing() || !coordinate(x) || !coordinate(y) || !positive(radius) || !stroke(thickness) || segments < 4 || segments > 1024) return false;
    if (thickness == 0)
        DrawCircleV((Vector2){ x, y }, radius, color_value(color));
    else
        DrawRing((Vector2){ x, y }, fmaxf(0, radius - thickness), radius, 0, 360, segments, color_value(color));
    return true;
}
/**
 * @brief 绘制椭圆或一像素椭圆边框。
 * @param x 逻辑像素横坐标。
 * @param y 逻辑像素纵坐标。
 * @param radius_x 正横向半径。
 * @param radius_y 正纵向半径。
 * @param outline 一表示边框，零表示填充。
 * @param color RRGGBBAA 颜色。
 * @return 参数、当前线程及操作状态有效时为真，否则为假。
 */
bool chali_gui_draw_ellipse(float x, float y, float radius_x, float radius_y, int32_t outline, uint32_t color)
{
    if (!chali_gui_is_drawing() || !coordinate(x) || !coordinate(y) || !positive(radius_x) || !positive(radius_y) || (outline != 0 && outline != 1)) return false;
    if (outline)
        DrawEllipseLines((int)x, (int)y, radius_x, radius_y, color_value(color));
    else
        DrawEllipse((int)x, (int)y, radius_x, radius_y, color_value(color));
    return true;
}
/**
 * @brief 绘制三角形，填充时自动统一顶点方向。
 * @param x1 起点或第一顶点横坐标。
 * @param y1 起点或第一顶点纵坐标。
 * @param x2 终点或第二顶点横坐标。
 * @param y2 终点或第二顶点纵坐标。
 * @param x3 第三顶点横坐标。
 * @param y3 第三顶点纵坐标。
 * @param outline 一表示边框，零表示填充。
 * @param color RRGGBBAA 颜色。
 * @return 参数、当前线程及操作状态有效时为真，否则为假。
 */
bool chali_gui_draw_triangle(float x1, float y1, float x2, float y2, float x3, float y3, int32_t outline, uint32_t color)
{
    Vector2 a = { x1, y1 }, b = { x2, y2 }, c = { x3, y3 };
    if (!chali_gui_is_drawing() || !coordinate(x1) || !coordinate(y1) || !coordinate(x2) || !coordinate(y2) || !coordinate(x3) || !coordinate(y3) || (outline != 0 && outline != 1)) return false;
    if (((double)x2 - x1) * ((double)y3 - y1) - ((double)y2 - y1) * ((double)x3 - x1) > 0)
    {
        Vector2 swap = b;
        b = c;
        c = swap;
    }
    if (outline)
        DrawTriangleLines(a, b, c, color_value(color));
    else
        DrawTriangle(a, b, c, color_value(color));
    return true;
}
/**
 * @brief 绘制正多边形填充或边框。
 * @param x 逻辑像素横坐标。
 * @param y 逻辑像素纵坐标。
 * @param sides 多边形边数，范围为三到二百五十六。
 * @param radius 正半径。
 * @param rotation 旋转角度，使用度数。
 * @param thickness 描边宽度；支持填充的图形以零表示填充。
 * @param color RRGGBBAA 颜色。
 * @return 参数、当前线程及操作状态有效时为真，否则为假。
 */
bool chali_gui_draw_polygon(float x, float y, int32_t sides, float radius, float rotation, float thickness, uint32_t color)
{
    if (!chali_gui_is_drawing() || !coordinate(x) || !coordinate(y) || sides < 3 || sides > 256 || !positive(radius) || !coordinate(rotation) || !stroke(thickness)) return false;
    if (thickness == 0)
        DrawPoly((Vector2){ x, y }, sides, radius, rotation, color_value(color));
    else
        DrawPolyLinesEx((Vector2){ x, y }, sides, radius, rotation, thickness, color_value(color));
    return true;
}
/**
 * @brief 绘制具有两个控制点的三次贝塞尔曲线。
 * @param x1 起点或第一顶点横坐标。
 * @param y1 起点或第一顶点纵坐标。
 * @param cx1 第一控制点横坐标。
 * @param cy1 第一控制点纵坐标。
 * @param cx2 第二控制点横坐标。
 * @param cy2 第二控制点纵坐标。
 * @param x2 终点或第二顶点横坐标。
 * @param y2 终点或第二顶点纵坐标。
 * @param thickness 描边宽度；支持填充的图形以零表示填充。
 * @param color RRGGBBAA 颜色。
 * @return 参数、当前线程及操作状态有效时为真，否则为假。
 */
bool chali_gui_draw_bezier(float x1, float y1, float cx1, float cy1, float cx2, float cy2, float x2, float y2, float thickness, uint32_t color)
{
    if (!chali_gui_is_drawing() || !coordinate(x1) || !coordinate(y1) || !coordinate(cx1) || !coordinate(cy1) || !coordinate(cx2) || !coordinate(cy2) || !coordinate(x2) || !coordinate(y2) || !positive(thickness) || !stroke(thickness)) return false;
    DrawSplineSegmentBezierCubic((Vector2){ x1, y1 }, (Vector2){ cx1, cy1 }, (Vector2){ cx2, cy2 }, (Vector2){ x2, y2 }, thickness, color_value(color));
    return true;
}
/**
 * @brief 绘制不超过一周的环带或扇形。
 * @param x 逻辑像素横坐标。
 * @param y 逻辑像素纵坐标。
 * @param inner_radius 非负内半径，小于外半径。
 * @param outer_radius 正外半径。
 * @param start_angle 起始角度，使用度数。
 * @param end_angle 终止角度，与起点之差不超过三百六十度。
 * @param segments 有界的曲线细分数。
 * @param color RRGGBBAA 颜色。
 * @return 参数、当前线程及操作状态有效时为真，否则为假。
 */
bool chali_gui_draw_ring(float x, float y, float inner_radius, float outer_radius, float start_angle, float end_angle, int32_t segments, uint32_t color)
{
    if (!chali_gui_is_drawing() || !coordinate(x) || !coordinate(y) || (!coordinate(inner_radius) || inner_radius < 0) || !positive(outer_radius) || inner_radius >= outer_radius || !coordinate(start_angle) || !coordinate(end_angle) || fabsf(end_angle - start_angle) > 360 || segments < 4 || segments > 1024) return false;
    if (inner_radius == 0)
        DrawCircleSector((Vector2){ x, y }, outer_radius, start_angle, end_angle, segments, color_value(color));
    else
        DrawRing((Vector2){ x, y }, inner_radius, outer_radius, start_angle, end_angle, segments, color_value(color));
    return true;
}
/**
 * @brief 使用当前 GUI 字体绘制 UTF-8 文本，不保留输入借用。
 * @param text 调用期间借用的零结尾 UTF-8 文本。
 * @param x 逻辑像素横坐标。
 * @param y 逻辑像素纵坐标。
 * @param size 正字号。
 * @param spacing 字符间距。
 * @param color RRGGBBAA 颜色。
 * @return 参数、当前线程及操作状态有效时为真，否则为假。
 */
bool chali_gui_draw_text(const char *text, float x, float y, float size, float spacing, uint32_t color)
{
    if (!chali_gui_is_drawing() || !text_valid(text) || !coordinate(x) || !coordinate(y) || !font_parameters(size, spacing)) return false;
    DrawTextEx(GuiGetFont(), text, (Vector2){ x, y }, size, spacing, color_value(color));
    return true;
}
/**
 * @brief 使用当前 GUI 字体测量逻辑像素尺寸，可在帧外查询。
 * @param text 调用期间借用的零结尾 UTF-8 文本。
 * @param size 正字号。
 * @param spacing 字符间距。
 * @return 逻辑文字尺寸或纹理宽高，无效调用为零尺寸。
 */
ChaliGuiPoint chali_gui_measure_text(const char *text, float size, float spacing)
{
    Vector2 measured;
    if (!chali_gui_is_owner() || !text_valid(text) || !font_parameters(size, spacing)) return (ChaliGuiPoint){ 0, 0 };
    measured = MeasureTextEx(GuiGetFont(), text, size, spacing);
    return (ChaliGuiPoint){ measured.x, measured.y };
}
/**
 * @brief 把有效 Unicode 标量编码为独占 UTF-8 文本。
 * @param codepoint Unicode 标量；零、代理和非法值输出空文本。
 * @return 由 chali_gui_free 释放的文本，分配失败为空。
 */
char *chali_gui_codepoint_text(int32_t codepoint)
{
    char *text = malloc(5);
    unsigned p = (unsigned)codepoint;
    size_t n = 0;
    if (!text) return NULL;
    if (codepoint <= 0 || codepoint > 0x10ffff || (codepoint >= 0xd800 && codepoint <= 0xdfff))
    {
        text[0] = 0;
        return text;
    }
    if (p < 0x80)
        text[n++] = (char)p;
    else if (p < 0x800)
    {
        text[n++] = (char)(0xc0 | (p >> 6));
        text[n++] = (char)(0x80 | (p & 63));
    }
    else if (p < 0x10000)
    {
        text[n++] = (char)(0xe0 | (p >> 12));
        text[n++] = (char)(0x80 | ((p >> 6) & 63));
        text[n++] = (char)(0x80 | (p & 63));
    }
    else
    {
        text[n++] = (char)(0xf0 | (p >> 18));
        text[n++] = (char)(0x80 | ((p >> 12) & 63));
        text[n++] = (char)(0x80 | ((p >> 6) & 63));
        text[n++] = (char)(0x80 | (p & 63));
    }
    text[n] = 0;
    return text;
}

/**
 * @brief 查询当前线程的 GUI 是否启用且未锁定输入。
 * @return 参数、当前线程及操作状态有效时为真，否则为假。
 */
bool chali_gui_input_enabled(void) { return chali_gui_is_owner() && GuiGetState() != STATE_DISABLED && !GuiIsLocked(); }
