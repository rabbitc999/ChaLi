/**
 * @file desktop_draw.h
 * @brief 独立 C 绘图和纹理接口，资源由窗口生命周期管理。
 */
#ifndef CHALI_DESKTOP_DRAW_H
#define CHALI_DESKTOP_DRAW_H
#include "desktop.h"
#ifdef __cplusplus
extern "C" {
#endif
/** @brief 两个 double 的值类型，表示逻辑尺寸或纹理像素尺寸。 */
typedef struct ChaliGuiPoint
{
    double x, y;
} ChaliGuiPoint;
/** @brief 四个 double 的逻辑裁剪矩形，不包含宿主指针。 */
typedef struct ChaliGuiBounds
{
    double x, y, width, height;
} ChaliGuiBounds;
/**
 * @brief 查询当前线程是否位于可绘制帧。
 * @return 参数、当前线程及操作状态有效时为真，否则为假。
 */
CHALI_GUI_API bool chali_gui_frame_active(void);
/**
 * @brief 查询当前线程的 GUI 是否启用且未锁定输入。
 * @return 参数、当前线程及操作状态有效时为真，否则为假。
 */
CHALI_GUI_API bool chali_gui_input_enabled(void);
/**
 * @brief 在当前帧绘制逻辑像素。
 * @param x 逻辑像素横坐标。
 * @param y 逻辑像素纵坐标。
 * @param color RRGGBBAA 颜色。
 * @return 参数、当前线程及操作状态有效时为真，否则为假。
 */
CHALI_GUI_API bool chali_gui_draw_pixel(float x, float y, uint32_t color);
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
CHALI_GUI_API bool chali_gui_draw_line(float x1, float y1, float x2, float y2, float thickness, uint32_t color);
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
CHALI_GUI_API bool chali_gui_draw_rectangle(float x, float y, float width, float height, float roundness, int32_t segments, float thickness, uint32_t color);
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
CHALI_GUI_API bool chali_gui_draw_circle(float x, float y, float radius, float thickness, int32_t segments, uint32_t color);
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
CHALI_GUI_API bool chali_gui_draw_ellipse(float x, float y, float radius_x, float radius_y, int32_t outline, uint32_t color);
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
CHALI_GUI_API bool chali_gui_draw_triangle(float x1, float y1, float x2, float y2, float x3, float y3, int32_t outline, uint32_t color);
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
CHALI_GUI_API bool chali_gui_draw_polygon(float x, float y, int32_t sides, float radius, float rotation, float thickness, uint32_t color);
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
CHALI_GUI_API bool chali_gui_draw_bezier(float x1, float y1, float cx1, float cy1, float cx2, float cy2, float x2, float y2, float thickness, uint32_t color);
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
CHALI_GUI_API bool chali_gui_draw_ring(float x, float y, float inner_radius, float outer_radius, float start_angle, float end_angle, int32_t segments, uint32_t color);
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
CHALI_GUI_API bool chali_gui_draw_text(const char *text, float x, float y, float size, float spacing, uint32_t color);
/**
 * @brief 使用当前 GUI 字体测量逻辑像素尺寸，可在帧外查询。
 * @param text 调用期间借用的零结尾 UTF-8 文本。
 * @param size 正字号。
 * @param spacing 字符间距。
 * @return 逻辑文字尺寸或纹理宽高，无效调用为零尺寸。
 */
CHALI_GUI_API ChaliGuiPoint chali_gui_measure_text(const char *text, float size, float spacing);
/**
 * @brief 查询当前裁剪区，无嵌套裁剪时为完整窗口。
 * @return 当前逻辑裁剪区，无窗口时为空矩形。
 */
CHALI_GUI_API ChaliGuiBounds chali_gui_clip_bounds(void);
/**
 * @brief 把有效 Unicode 标量编码为独占 UTF-8 文本。
 * @param codepoint Unicode 标量；零、代理和非法值输出空文本。
 * @return 由 chali_gui_free 释放的文本，分配失败为空。
 */
CHALI_GUI_API char *chali_gui_codepoint_text(int32_t codepoint);
/**
 * @brief 读取 UTF-8 路径资源并创建由当前窗口拥有的纹理。
 * @param path 调用期间借用的零结尾 UTF-8 路径。
 * @return 非零纹理身份；读取、解码或分配失败时为零。
 */
CHALI_GUI_API int64_t chali_gui_texture_load(const char *path);
/**
 * @brief 核对纹理是否仍属于当前线程的窗口。
 * @param id 单调分配的纹理身份。
 * @return 参数、当前线程及操作状态有效时为真，否则为假。
 */
CHALI_GUI_API bool chali_gui_texture_valid(int64_t id);
/**
 * @brief 幂等关闭纹理，绘制中先提交使用旧纹理的批次。
 * @param id 单调分配的纹理身份。
 */
CHALI_GUI_API void chali_gui_texture_close(int64_t id);
/**
 * @brief 读取纹理的像素宽高，无效身份返回零。
 * @param id 单调分配的纹理身份。
 * @return 逻辑文字尺寸或纹理宽高，无效调用为零尺寸。
 */
CHALI_GUI_API ChaliGuiPoint chali_gui_texture_size(int64_t id);
/**
 * @brief 把纹理区域绘制到目标矩形，可指定原点、旋转与调色。
 * @param id 单调分配的纹理身份。
 * @param sx 源区域横坐标。
 * @param sy 源区域纵坐标。
 * @param sw 正源区域宽度。
 * @param sh 正源区域高度。
 * @param x 逻辑像素横坐标。
 * @param y 逻辑像素纵坐标。
 * @param width 目标宽度。
 * @param height 目标高度。
 * @param origin_x 目标矩形内的旋转原点横坐标。
 * @param origin_y 目标矩形内的旋转原点纵坐标。
 * @param rotation 旋转角度，使用度数。
 * @param tint RRGGBBAA 调色值，白色保持原色。
 * @return 参数、当前线程及操作状态有效时为真，否则为假。
 */
CHALI_GUI_API bool chali_gui_texture_draw(int64_t id, float sx, float sy, float sw, float sh, float x, float y, float width, float height, float origin_x, float origin_y, float rotation, uint32_t tint);
/**
 * @brief 在窗口销毁图形上下文前释放全部纹理。
 */
void chali_gui_textures_close(void);
#ifdef __cplusplus
}
#endif
#endif
