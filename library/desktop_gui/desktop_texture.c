/**
 * @file desktop_texture.c
 * @brief 独立 C 绘图和纹理接口，资源由窗口生命周期管理。
 */
#include "desktop_draw.h"
#include "desktop_assets.h"
#include "raylib.h"
#include "rlgl.h"
#include <stdlib.h>
#include <math.h>
#include <limits.h>
/** @brief 当前窗口独占的 GPU 纹理及不复用的身份。 */
typedef struct TextureEntry
{
    int64_t id;
    Texture2D texture;
    struct TextureEntry *next;
} TextureEntry;
static TextureEntry *textures;
static int64_t next_id = 1;
/**
 * @brief 在当前窗口线程查找纹理身份。
 * @param id 单调分配的纹理身份。
 * @return 窗口拥有的内部记录，未找到时为空。
 */
static TextureEntry *find_texture(int64_t id)
{
    TextureEntry *at;
    if (!chali_gui_is_owner() || id <= 0) return NULL;
    for (at = textures; at; at = at->next)
        if (at->id == id) return at;
    return NULL;
}
/**
 * @brief 读取 UTF-8 路径资源并创建由当前窗口拥有的纹理。
 * @param path 调用期间借用的零结尾 UTF-8 路径。
 * @return 非零纹理身份；读取、解码或分配失败时为零。
 */
int64_t chali_gui_texture_load(const char *path)
{
    Texture2D texture;
    TextureEntry *entry;
    Image image;
    unsigned char *bytes;
    size_t length;
    if (!chali_gui_is_owner() || !path || !*path || next_id == INT64_MAX) return 0;
    bytes = chali_gui_read_asset(path, &length);
    if (!bytes) return 0;
    image = LoadImageFromMemory(GetFileExtension(path), bytes, (int)length);
    free(bytes);
    if (!IsImageValid(image)) return 0;
    texture = LoadTextureFromImage(image);
    UnloadImage(image);
    if (!IsTextureValid(texture)) return 0;
    entry = malloc(sizeof(*entry));
    if (!entry)
    {
        UnloadTexture(texture);
        return 0;
    }
    entry->id = next_id++;
    entry->texture = texture;
    entry->next = textures;
    textures = entry;
    return entry->id;
}
/**
 * @brief 核对纹理是否仍属于当前线程的窗口。
 * @param id 单调分配的纹理身份。
 * @return 参数、当前线程及操作状态有效时为真，否则为假。
 */
bool chali_gui_texture_valid(int64_t id) { return find_texture(id) != NULL; }
/**
 * @brief 幂等关闭纹理，绘制中先提交使用旧纹理的批次。
 * @param id 单调分配的纹理身份。
 */
void chali_gui_texture_close(int64_t id)
{
    TextureEntry **link;
    if (!chali_gui_is_owner()) return;
    for (link = &textures; *link; link = &(*link)->next)
        if ((*link)->id == id)
        {
            TextureEntry *entry = *link;
            if (chali_gui_is_drawing()) rlDrawRenderBatchActive();
            *link = entry->next;
            UnloadTexture(entry->texture);
            free(entry);
            return;
        }
}
/**
 * @brief 在窗口销毁图形上下文前释放全部纹理。
 */
void chali_gui_textures_close(void)
{
    if (!chali_gui_is_owner()) return;
    while (textures) chali_gui_texture_close(textures->id);
}
/**
 * @brief 读取纹理的像素宽高，无效身份返回零。
 * @param id 单调分配的纹理身份。
 * @return 逻辑文字尺寸或纹理宽高，无效调用为零尺寸。
 */
ChaliGuiPoint chali_gui_texture_size(int64_t id)
{
    TextureEntry *entry = find_texture(id);
    return entry ? (ChaliGuiPoint){ entry->texture.width, entry->texture.height } : (ChaliGuiPoint){ 0, 0 };
}
/**
 * @brief 检查有限且绝对值不超过一千万的坐标。
 * @param value 要检查或转换的数值。
 * @return 参数、当前线程及操作状态有效时为真，否则为假。
 */
static int coordinate(float value) { return isfinite(value) && fabsf(value) <= 10000000.0f; }
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
bool chali_gui_texture_draw(int64_t id, float sx, float sy, float sw, float sh, float x, float y, float width, float height, float origin_x, float origin_y, float rotation, uint32_t tint)
{
    TextureEntry *entry = find_texture(id);
    if (!entry || !chali_gui_is_drawing() || !coordinate(sx) || !coordinate(sy) || !coordinate(sw) || !coordinate(sh) || sx < 0 || sy < 0 || sw <= 0 || sh <= 0 || (double)sx + sw > entry->texture.width || (double)sy + sh > entry->texture.height || !coordinate(x) || !coordinate(y) || !coordinate(width) || !coordinate(height) || width <= 0 || height <= 0 || !coordinate(origin_x) || !coordinate(origin_y) || !coordinate(rotation)) return false;
    DrawTexturePro(entry->texture, (Rectangle){ sx, sy, sw, sh }, (Rectangle){ x, y, width, height }, (Vector2){ origin_x, origin_y }, rotation, (Color){ (unsigned char)(tint >> 24), (unsigned char)(tint >> 16), (unsigned char)(tint >> 8), (unsigned char)tint });
    return true;
}
