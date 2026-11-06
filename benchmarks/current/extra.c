/** @file extra.c
 * @brief C 的显式载荷、函数指针、嵌套结构及 UTF-8 字节任务。
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#define MASK 2147483647LL
/** @brief 保存三个整数分量。 */
typedef struct
{
    int64_t x, y, z;
} Vec;
/** @brief 保存嵌套坐标、速度和权重。 */
typedef struct
{
    Vec position, velocity;
    int64_t weights[4];
} Transform;
/** @brief 保存结构体负载和标记。 */
typedef struct
{
    Transform transform;
    int64_t payload[8], flags;
} Row;
/** @brief 以标签和字段表达四种载荷。 */
typedef struct
{
    int tag;
    int64_t x, y;
} Event;
static Event events[256];
static Row rows[256];
static int64_t matrix[64][64], cube[16][16][16];
static char text[16384], first[128], second[128];
static size_t width, length;
/** @brief 构造所有只读输入；本函数位于计时区间之外。 */
void feature_setup(void)
{
    const char *unit = "ChaLi-字符串-🙂-0123456789|";
    size_t n = strlen(unit);
    width = n + 1;
    length = width * 256;
    memcpy(first, unit, n);
    first[n] = 'a';
    memcpy(second, unit, n);
    second[n] = 'B';
    for (int64_t i = 0; i < 256; i++)
    {
        memcpy(text + i * width, unit, n);
        text[i * width + n] = (char)('A' + i % 26);
        events[i].tag = (int)(i % 4);
        events[i].x = i % 4 == 1 ? i * 3 + 7 : i % 4 == 2 ? i * 5 :
                                           i % 4 == 3     ? i * 7 :
                                                            0;
        events[i].y = i % 4 == 2 ? i + 11 : i % 4 == 3 ? i + 13 :
                                                         0;
        Row *r = &rows[i];
        r->transform.position = (Vec){ i * 3 + 1, i * 5 + 2, i * 7 + 3 };
        r->transform.velocity = (Vec){ i + 4, i + 5, i + 6 };
        r->flags = i * 11 + 9;
        for (int k = 0; k < 4; k++) r->transform.weights[k] = i * 13 + k;
        for (int k = 0; k < 8; k++) r->payload[k] = i * 17 + k * 3;
    }
    text[length] = 0;
    for (int r = 0; r < 64; r++)
        for (int c = 0; c < 64; c++) matrix[r][c] = r * 131 + c * 17 + 3;
    for (int x = 0; x < 16; x++)
        for (int y = 0; y < 16; y++)
            for (int z = 0; z < 16; z++) cube[x][y][z] = x * 257 + y * 31 + z * 7 + 3;
}
/**
 * @brief 单态和多态操作的乘加实现。
 * @param v 输入。
 * @return 掩码结果。
 */
static int64_t multiply(int64_t v) { return (v * 3 + 7) & MASK; }
/**
 * @brief 多态操作的乘减实现。
 * @param v 输入。
 * @return 掩码结果。
 */
static int64_t subtract(int64_t v) { return (v * 5 - 11) & MASK; }
/**
 * @brief 多态操作的异或实现。
 * @param v 输入。
 * @return 掩码结果。
 */
static int64_t exclusive_or(int64_t v) { return ((v * 7) ^ 13) & MASK; }
static int64_t (*operations[4])(int64_t) = { multiply, subtract, exclusive_or, multiply };
/**
 * @brief 执行编译期选择的负载。
 * @param seed 初值。
 * @return 独立核对的结果。
 */
int64_t feature_work(int64_t seed)
{
    int64_t a = seed;
#if SELECT == 0
    for (int64_t i = 0; i < 262144; i++)
    {
        Event *e = &events[(i * 13 + ((a >> 7) & 255)) & 255];
        a = (a * 33 + (e->tag == 0 ? 17 : e->x + e->y)) & MASK;
    }
#elif SELECT == 1
    for (int i = 0; i < 262144; i++) a = operations[0](a);
#elif SELECT == 2
    for (int64_t i = 0; i < 262144; i++) a = operations[(i + seed) & 3](a);
#elif SELECT == 3
    for (int64_t i = 0; i < 262144; i++)
    {
        Row *r = &rows[(i * 13 + (a & 255)) & 255];
        Transform *t = &r->transform;
        int64_t v = t->position.x + t->position.z + t->velocity.y + t->weights[i & 3] + r->payload[(i + seed) & 7] + r->flags;
        a = (a * 33 + v) & MASK;
    }
#elif SELECT == 4
    for (int64_t i = 0; i < 262144; i++) a = (a * 33 + matrix[(a >> 6) & 63][(i + a) & 63]) & MASK;
#elif SELECT == 5
    for (int64_t i = 0; i < 262144; i++) a = (a * 33 + cube[(a >> 4) & 15][(a >> 8) & 15][(i + seed) & 15]) & MASK;
#elif SELECT == 6
    for (int64_t i = 0; i < 131072; i++) a = (a * 33 + (unsigned char)text[(i * 13 + ((a >> 7) & 65535)) % length]) & MASK;
#elif SELECT == 7
    char *v = NULL;
    size_t used = 0;
    for (int64_t i = 0; i < 2048; i++)
    {
        char *next = malloc(used + width);
        if (!next) abort();
        if (used) memcpy(next, v, used);
        memcpy(next + used, ((i + seed) & 1) ? second : first, width);
        free(v);
        v = next;
        used += width;
    }
    for (size_t i = 0; i < used; i++) a = (a * 33 + (unsigned char)v[i]) & MASK;
    free(v);
#elif SELECT == 8
    for (int64_t i = 0; i < 1024; i++)
    {
        int64_t block = (i * 13 + (a & 127)) & 127;
        size_t start = block * width, n = (((i + seed) & 3) + 1) * width, other = (i & 1) ? ((block + 13) & 127) * width : start;
        char *p = malloc(n), *q = malloc(n);
        if (!p || !q) abort();
        memcpy(p, text + start, n);
        memcpy(q, text + other, n);
        a = (a * 33 + (unsigned char)p[0] + (unsigned char)p[n - 1] + (memcmp(p, q, n) == 0)) & MASK;
        free(p);
        free(q);
    }
#elif SELECT == 9
    const char *patterns[4] = { "字符串", "🙂", "不存在!", "012345" };
    for (int64_t i = 0; i < 256; i++)
    {
        size_t start = (((a >> 7) + i) & 255) * width;
        char *found = strstr(text + start, patterns[(i + seed) & 3]);
        int64_t at = found ? found - text : -1;
        a = (a * 33 + at + 1) & MASK;
    }
#endif
    return a;
}
