
/* ---- ChaLi AOT 运行时（产物自带，不依赖虚拟机） ---- */
#ifdef _WIN32
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#else
typedef unsigned long size_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;
typedef int int32_t;
typedef short int16_t;
typedef struct ChaliFile FILE;
extern FILE *stdin, *stdout, *stderr;
extern void *malloc(size_t), *calloc(size_t, size_t), *memcpy(void *, const void *, size_t), *memmove(void *, const void *, size_t), *memset(void *, int, size_t);
extern void free(void *);
extern void *realloc(void *, size_t);
extern void exit(int);
extern int fflush(FILE *), fprintf(FILE *, const char *, ...), printf(const char *, ...), snprintf(char *, size_t, const char *, ...), sscanf(const char *, const char *, ...), fputc(int, FILE *);
extern size_t fwrite(const void *, size_t, size_t, FILE *), strlen(const char *);
extern char *fgets(char *, int, FILE *), *strchr(const char *, int), *strstr(const char *, const char *), *strncat(char *, const char *, size_t);
extern int memcmp(const void *, const void *, size_t);
extern int strcmp(const char *, const char *);
extern int fgetc(FILE *);
extern int feof(FILE *);
#define NULL ((void *)0)
#endif
extern const unsigned char chali_data_image[];
extern const size_t chali_data_size, chali_heap_size;

typedef int64_t chali_i64;
typedef uint64_t chali_u64;

/**
 * @brief 取得当前目标平台的 C 标准流，供 LLVM 直接生成的流调用使用。
 * @param index 零为标准输入，一为标准输出，二为标准错误。
 * @return 当前平台的 FILE 指针，不经过语言宿主记录或胶水注册表。
 */
FILE *chali_standard_stream(int index)
{
    return index == 0 ? stdin : index == 1 ? stdout : stderr;
}

extern void chali_host_init(void), chali_host_finish(void), chali_host_drop(chali_i64);
extern unsigned char *chali_host_address(chali_i64, size_t);
extern chali_i64 chali_host_allocate(size_t), chali_host_function(chali_i64, int);
extern chali_i64 chali_host_call(const char *, chali_i64, const chali_i64 *, const unsigned char *);
extern void chali_host_trace(void);

/**
 * @brief 刷新输出、报告运行时错误并终止进程。
 * @param message 错误或断言消息；句柄值为零时省略消息。
 */
void chali_panic(const char *message)
{
    fflush(stdout);
    fprintf(stderr, "chali aot: %s\n", message);
    chali_host_trace();
    exit(1);
}

/**
 * @brief 按 VM 的六十四位槽规则拒绝越界移位。
 * @param count 移位位数。
 */
void chali_check_shift(chali_i64 count)
{
    if (count < 0 || count >= 64) chali_panic("移位位数超出整数位宽（有效范围 0..63）");
}

/**
 * @brief 在执行浮点除法或取余前保留 VM 的除零陷阱。
 * @param divisor 浮点除数。
 */
void chali_check_real_divisor(double divisor)
{
    if (divisor == 0.0) chali_panic("浮点除零");
}

/**
 * @brief 使用 VM 同源的整数除零、溢出和最小负数取余规则。
 * @param left 被除数。
 * @param right 除数。
 * @param unsignedValue 非零时执行无符号除法。
 * @param remainder 非零时返回余数，否则返回商。
 * @return 计算得到的六十四位槽值。
 */
chali_i64 chali_integer_divide(chali_i64 left, chali_i64 right, int unsignedValue, int remainder)
{
    if (!right) chali_panic("整数除零");
    if (unsignedValue)
        return (chali_i64)(remainder ? (chali_u64)left % (chali_u64)right : (chali_u64)left / (chali_u64)right);
    if (left == (-9223372036854775807LL - 1) && right == -1)
    {
        if (remainder) return 0;
        chali_panic("整数除法溢出：最小负数除以 -1 的结果超出 int64 范围");
    }
    return remainder ? left % right : left / right;
}

/**
 * @brief 按编译模块的数据和堆预算创建运行时内存并复制初始化数据。
 */
void chali_init(void)
{
    chali_host_init();
}

/**
 * @brief 刷新输出并释放 AOT 运行时的整块内存。
 */
void chali_finish(void)
{
    fflush(stdout);
    chali_host_finish();
}

/* 堆由共享原生服务管理，释放撤销逻辑身份，读取保留存活和范围检查。 */
/**
 * @brief 从本次运行的共享堆预算中分配字节。
 * @param bytes 待复制的源字节或待分配的字节数。
 * @return 新分配范围的字节偏移。
 */
chali_i64 chali_alloc(size_t bytes)
{
    return chali_host_allocate(bytes);
}

/**
 * @brief 释放活跃堆块或拥有型对象。
 * @param address 运行时内存中的字节偏移。
 */
void chali_drop(chali_i64 address) { chali_host_drop(address); }

/* 读写：地址是内存块内的偏移 */
/**
 * @brief 从运行时偏移读取 i64 标量，使用复制避免未对齐访问。
 * @param address 运行时内存中的字节偏移。
 * @return 读取并扩展到入口返回类型的标量值。
 */
chali_i64 chali_load_i64(chali_i64 address)
{
    chali_i64 v = 0;
    memcpy(&v, chali_host_address(address, 8), 8);
    return v;
}
/**
 * @brief 将 i64 标量写入运行时偏移，使用复制避免未对齐访问。
 * @param address 运行时内存中的字节偏移。
 * @param value 待读取、写入、格式化或输出的值。
 */
void chali_store_i64(chali_i64 address, chali_i64 value) { memcpy(chali_host_address(address, 8), &value, 8); }
/**
 * @brief 从运行时偏移读取 f64 标量，使用复制避免未对齐访问。
 * @param address 运行时内存中的字节偏移。
 * @return 读取并扩展到入口返回类型的标量值。
 */
double chali_load_f64(chali_i64 address)
{
    chali_i64 v = chali_load_i64(address);
    double d = 0.0;
    memcpy(&d, &v, 8);
    return d;
}
/**
 * @brief 将 f64 标量写入运行时偏移，使用复制避免未对齐访问。
 * @param address 运行时内存中的字节偏移。
 * @param value 待读取、写入、格式化或输出的值。
 */
void chali_store_f64(chali_i64 address, double value)
{
    chali_i64 v = 0;
    memcpy(&v, &value, 8);
    chali_store_i64(address, v);
}
/**
 * @brief 从运行时偏移读取 u8 标量，使用复制避免未对齐访问。
 * @param address 运行时内存中的字节偏移。
 * @return 读取并扩展到入口返回类型的标量值。
 */
chali_i64 chali_load_u8(chali_i64 address) { return (chali_i64)*chali_host_address(address, 1); }
/**
 * @brief 将 u8 标量写入运行时偏移，使用复制避免未对齐访问。
 * @param address 运行时内存中的字节偏移。
 * @param value 待读取、写入、格式化或输出的值。
 */
void chali_store_u8(chali_i64 address, chali_i64 value) { *chali_host_address(address, 1) = (unsigned char)(value & 0xFF); }
/**
 * @brief 从运行时偏移读取 i32 标量，使用复制避免未对齐访问。
 * @param address 运行时内存中的字节偏移。
 * @return 读取并扩展到入口返回类型的标量值。
 */
chali_i64 chali_load_i32(chali_i64 address)
{
    int32_t v = 0;
    memcpy(&v, chali_host_address(address, 4), 4);
    return (chali_i64)v;
}
/**
 * @brief 将 i32 标量写入运行时偏移，使用复制避免未对齐访问。
 * @param address 运行时内存中的字节偏移。
 * @param value 待读取、写入、格式化或输出的值。
 */
void chali_store_i32(chali_i64 address, chali_i64 value)
{
    int32_t v = (int32_t)value;
    memcpy(chali_host_address(address, 4), &v, 4);
}
/**
 * @brief 从运行时偏移读取 i16 标量，使用复制避免未对齐访问。
 * @param address 运行时内存中的字节偏移。
 * @return 读取并扩展到入口返回类型的标量值。
 */
chali_i64 chali_load_i16(chali_i64 address)
{
    int16_t v = 0;
    memcpy(&v, chali_host_address(address, 2), 2);
    return (chali_i64)v;
}
/**
 * @brief 将 i16 标量写入运行时偏移，使用复制避免未对齐访问。
 * @param address 运行时内存中的字节偏移。
 * @param value 待读取、写入、格式化或输出的值。
 */
void chali_store_i16(chali_i64 address, chali_i64 value)
{
    int16_t v = (int16_t)value;
    memcpy(chali_host_address(address, 2), &v, 2);
}

/* 字符串：[8 字节长度][字节…]，值就是块起点（与 VM 的堆串布局一致） */
/**
 * @brief 将指定字节复制为长度前缀字符串，保留嵌入的零字节。
 * @param bytes 待复制的源字节或待分配的字节数。
 * @param length 字节数或数组元素数量。
 * @return 新构造的字符串句柄；空字符串仍具有有效的长度前缀。
 */
chali_i64 chali_string_new(const char *bytes, size_t length)
{
    char *copy = (char *)malloc(length ? length : 1);
    if (!copy) chali_panic("复制字符串时内存不足");
    if (length) memcpy(copy, bytes, length);
    const chali_i64 at = chali_alloc(length + 9u);
    const chali_i64 n = (chali_i64)length;
    memcpy(chali_host_address(at, 8), &n, 8);
    if (length != 0u) memcpy(chali_host_address(at + 8, length), copy, length);
    *chali_host_address(at + 8 + length, 1) = 0;
    free(copy);
    return at;
}
/* 两种串共存：数据映像里的字面量是 NUL 结尾的裸字节（地址 < chali_data_size），
 * 堆上的串是「8 字节长度 + 字节」。VM 那侧靠 `heap->contains` 区分，这里靠地址分界线。 */
/**
 * @brief 根据初始化数据边界区分堆句柄和数据区地址。
 * @param address 运行时内存中的字节偏移。
 * @return 地址位于堆区时返回非零。
 */
int chali_on_heap(chali_i64 address) { return address >= (chali_i64)chali_data_size; }
/**
 * @brief 读取堆字符串的长度或计算数据区字面量长度。
 * @param s 源字符串句柄；零表示空字符串。
 * @return 字符串的字节长度。
 */
chali_i64 chali_string_length(chali_i64 s)
{
    if (s == 0) return 0;
    return chali_on_heap(s) ? chali_load_i64(s) : (chali_i64)strlen((const char *)chali_host_address(s, 1));
}
/**
 * @brief 取得堆字符串或数据区字面量的字节起点。
 * @param s 源字符串句柄；零表示空字符串。
 * @return 指向字符串首字节的宿主指针。
 */
const char *chali_string_bytes(chali_i64 s)
{
    return (const char *)chali_host_address(s + (chali_on_heap(s) ? 8 : 0), 0);
}
/**
 * @brief 复制数据区的零结尾字面量为拥有型字符串。
 * @param address 运行时内存中的字节偏移。
 * @return 新构造的字符串句柄；空字符串仍具有有效的长度前缀。
 */
chali_i64 chali_string_literal(chali_i64 address)
{
    const char *text = (const char *)chali_host_address(address, 1);
    return chali_string_new(text, strlen(text));
}
/**
 * @brief 按实际长度复制字符串句柄对应的全部字节。
 * @param s 源字符串句柄；零表示空字符串。
 * @return 新构造的字符串句柄；空字符串仍具有有效的长度前缀。
 */
chali_i64 chali_string_copy(chali_i64 s)
{
    return s == 0 ? 0 : chali_string_new(chali_string_bytes(s), (size_t)chali_string_length(s));
}
/**
 * @brief 释放字符串拥有的共享堆存储。
 * @param s 源字符串句柄；零表示空字符串。
 */
void chali_string_drop(chali_i64 s) { chali_host_drop(s); }

/* 数组：块头 8 字节是元素个数，返回值指向元素区，块起点 = 返回值 - 8
 * （四元式里释放数组写的是 drop(a - 8)） */
/**
 * @brief 分配含元素数量前缀的数组，返回元素区偏移。
 * @param length 字节数或数组元素数量。
 * @param stride 单个数组元素的字节大小。
 * @return 新数组元素区的字节偏移。
 */
chali_i64 chali_new_array(chali_i64 length, chali_i64 stride)
{
    const chali_i64 count = length < 0 ? 0 : length;
    const chali_i64 base = chali_alloc((size_t)count * (size_t)stride + 8u);
    memcpy(chali_host_address(base, 8), &count, 8);
    return base + 8;
}

/**
 * @brief 检查数组下标是否落在零至长度之间，越界时报错退出。
 * @param index 从零开始的字节或元素下标。
 * @param limit 数组元素数量，不包含该上界。
 * @param what 用于错误消息的访问类型。
 */
void chali_bounds(chali_i64 index, chali_i64 limit, const char *what)
{
    if (index < 0 || index >= limit)
    {
        char message[128];
        snprintf(message, sizeof(message), "%s越界：下标 %lld，长度 %lld", what, (long long)index, (long long)limit);
        chali_panic(message);
    }
}
/**
 * @brief 检查偏移是否为空指针，空指针时报错退出。
 * @param address 运行时内存中的字节偏移。
 * @param what 用于错误消息的访问类型。
 */
void chali_null(chali_i64 address, const char *what)
{
    if (address == 0)
    {
        char message[128];
        snprintf(message, sizeof(message), "%s：空指针", what);
        chali_panic(message);
    }
}

/* 输出：格式串一个个替换。println 的换行已经由前端写进格式串字面量，
 * 所以这里不需要区分 print 与 println（VM 那一侧也是这么做的）。
 * 前端既可能留 `{}`，也可能已经翻成 printf 的 `%d` / `%g` / `%s`，两种都认。 */
static chali_i64 chali_fmt_at = 0;
static size_t chali_fmt_pos = 0u;
/**
 * @brief 选择本次格式化输出的格式串并重置扫描位置。
 * @param format 格式串在运行时内存中的偏移。
 */
void chali_fmt_begin(chali_i64 format)
{
    chali_fmt_at = format;
    chali_fmt_pos = 0u;
}

/* 吐出到下一个占位符之前的普通字符，返回占位符的转换字符：
 * `'?'` = `{}`（按值的种类自己挑格式）、`'%'` = 转义的百分号、0 = 后面没有了。
 * 宽度 / 精度 / 标志留在 prefix 里（`%` 开头，不含长度修饰符与转换字符）。 */
/**
 * @brief 输出普通字符并解析下一处占位符的转换字符和格式前缀。
 * @param prefix 接收或提供格式化标志、宽度与精度的文本。
 * @param prefixSize 格式前缀缓冲区的字节容量。
 * @return 转换字符；没有剩余占位符时返回零。
 */
int chali_fmt_scan(char *prefix, size_t prefixSize)
{
    const char *text = chali_string_bytes(chali_fmt_at);
    size_t at = chali_fmt_pos;
    while (text[at] != '\0' && text[at] != '%' && !(text[at] == '{' && text[at + 1u] == '}')) at++;
    if (at > chali_fmt_pos)
    {
        fwrite(text + chali_fmt_pos, 1u, at - chali_fmt_pos, stdout);
        chali_fmt_pos = at;
    }
    if (text[at] == '\0')
    {
        prefix[0] = '\0';
        return 0;
    }
    if (text[at] == '{')
    {
        size_t close = at + 1u;
        while (text[close] != '\0' && text[close] != '}') close++;
        chali_fmt_pos = (text[close] == '}') ? close + 1u : close;
        prefix[0] = '%';
        prefix[1] = '\0';
        size_t inner = at + 1u;
        if (text[inner] == ':') inner++;
        const size_t last = close;
        if (last > inner) /* `{:3F}` / `{:x}`：冒号后面就是 printf 的规格，末字符是转换字符 */
        {
            size_t n = 1u;
            for (size_t scan = inner; scan + 1u < last && n + 2u < prefixSize; ++scan) prefix[n++] = text[scan];
            prefix[n] = '\0';
            return (int)(unsigned char)text[last - 1u];
        }
        return '?'; /* 光秃秃的 `{}`：按值的种类自己挑 */
    }
    size_t scan = at + 1u;
    if (text[scan] == '%')
    {
        fputc('%', stdout);
        chali_fmt_pos = scan + 1u;
        prefix[0] = '\0';
        return '%';
    }
    size_t n = 0u;
    prefix[n++] = '%';
    while (text[scan] != '\0' && strchr("-+ #0123456789.", text[scan]) != NULL && n + 2u < prefixSize) prefix[n++] = text[scan++];
    while (text[scan] != '\0' && strchr("hlLqjzt", text[scan]) != NULL) scan++; /* 长度修饰符丢掉，自己按 64 位补 */
    prefix[n] = '\0';
    if (text[scan] == '\0')
    {
        chali_fmt_pos = scan;
        return 0;
    }
    chali_fmt_pos = scan + 1u;
    return (int)(unsigned char)text[scan];
}

/* 怎么打印由**格式串里的转换字符**说了算，不看翻译器猜的种类：
 * `%s` 就把这个 64 位当地址去读串，`%f` / `%g` 就把它按 double 的位模式解释。
 * 猜错种类（`float64 fromInt = 2` 那条路上的老坑）不会再把 `%.3f` 喂给整数打印。 */
/**
 * @brief 按格式转换字符输出整数位模式、浮点值或字符串。
 * @param conv 占位符指定的转换字符。
 * @param prefix 接收或提供格式化标志、宽度与精度的文本。
 * @param bits 待格式化值的六十四位位模式。
 * @param asFloat 值是否来源于浮点类型。
 */
void chali_fmt_emit(int conv, const char *prefix, chali_i64 bits, int asFloat)
{
    char spec[64], padded[40];
    if (conv == 's')
    {
        if (bits == 0) return;
        snprintf(spec, sizeof spec, "%s.*s", prefix);
        printf(spec, (int)chali_string_length(bits), chali_string_bytes(bits));
        return;
    }
    if (conv == 'c')
    {
        fputc((int)(bits & 0xFF), stdout);
        return;
    }
    if (conv == '?') conv = asFloat ? 'g' : 'd';
    if (strchr("gGeEfFaA", conv) != NULL)
    {
        double value = 0.0;
        memcpy(&value, &bits, 8);
        /* 语言的默认精度是三位小数（`native_format.cpp` 里写死的 `.3`）：
         * `%g` 打 3.625 出来的是 `3.62`，不是 C 默认的 `3.625`。 */
        snprintf(padded, sizeof padded, "%s", prefix);
        if (strchr(padded, '.') == NULL) strncat(padded, ".3", sizeof padded - strlen(padded) - 1u);
        snprintf(spec, sizeof spec, "%s%c", padded, (char)conv);
        printf(spec, value);
        return;
    }
    if (conv == 'u' || conv == 'x' || conv == 'X' || conv == 'o')
    {
        snprintf(spec, sizeof spec, "%sll%c", prefix, (char)conv);
        printf(spec, (unsigned long long)(chali_u64)bits);
        return;
    }
    snprintf(spec, sizeof spec, "%slld", prefix);
    printf(spec, (long long)bits);
}
/**
 * @brief 读取下一个占位符并输出整数。
 * @param value 待读取、写入、格式化或输出的值。
 */
void chali_fmt_int(chali_i64 value)
{
    char prefix[32];
    const int conv = chali_fmt_scan(prefix, sizeof prefix);
    if (conv == 0 || conv == '%') return;
    chali_fmt_emit(conv, prefix, value, 0);
}
/**
 * @brief 复用整数格式化入口输出无符号整数位模式。
 * @param value 待读取、写入、格式化或输出的值。
 */
void chali_fmt_uint(chali_i64 value) { chali_fmt_int(value); }
/**
 * @brief 读取下一个占位符并输出字符对应的整数值。
 * @param value 待读取、写入、格式化或输出的值。
 */
void chali_fmt_char(chali_i64 value)
{
    char prefix[32];
    const int conv = chali_fmt_scan(prefix, sizeof prefix);
    if (conv == 0 || conv == '%') return;
    chali_fmt_emit(conv, prefix, value, 0);
}
/**
 * @brief 将浮点位模式交给格式转换器输出。
 * @param value 待读取、写入、格式化或输出的值。
 */
void chali_fmt_double(double value)
{
    char prefix[32];
    chali_i64 bits = 0;
    const int conv = chali_fmt_scan(prefix, sizeof prefix);
    if (conv == 0 || conv == '%') return;
    memcpy(&bits, &value, 8);
    chali_fmt_emit(conv, prefix, bits, 1);
}
/**
 * @brief 按实际长度输出字符串，避免读取结尾之外的字节。
 * @param value 待读取、写入、格式化或输出的值。
 */
void chali_fmt_string(chali_i64 value)
{
    char prefix[32], spec[48];
    const int conv = chali_fmt_scan(prefix, sizeof prefix);
    if (conv == 0 || conv == '%' || value == 0) return;
    const char *bytes = chali_string_bytes(value);
    const int length = (int)chali_string_length(value);
    /* 堆串是「8 字节长度 + 字节」，不一定带结尾 0，所以一律按长度打印。 */
    snprintf(spec, sizeof spec, "%s.*s", prefix);
    printf(spec, length, bytes);
}
/**
 * @brief 输出格式串剩余字符并还原转义百分号。
 */
void chali_fmt_finish(void)
{
    const char *text = chali_string_bytes(chali_fmt_at);
    size_t at = chali_fmt_pos;
    /* 没有占位符的格式串也可能带 `%%`，照样要还原成一个 `%`（VM 那侧也是这个语义）。 */
    while (text[at] != '\0')
    {
        if (text[at] == '%' && text[at + 1u] == '%')
        {
            fputc('%', stdout);
            at += 2u;
            continue;
        }
        fputc(text[at], stdout);
        at++;
    }
    chali_fmt_pos = at;
}

/* 字符串比较 / 子串 / 查找：与 VM 那侧一样按字节做，串是「8 字节长度 + 字节」。 */
/**
 * @brief 按字节序比较字符串，再比较公共前缀后的长度。
 * @param left 左侧源字符串句柄。
 * @param right 右侧源字符串句柄。
 * @return 左串较小、相同或较大分别返回 -1、0、1。
 */
chali_i64 chali_string_cmp(chali_i64 left, chali_i64 right)
{
    const size_t a = (size_t)(left == 0 ? 0 : chali_string_length(left));
    const size_t b = (size_t)(right == 0 ? 0 : chali_string_length(right));
    const size_t n = a < b ? a : b;
    const int order = n == 0u ? 0 : memcmp(chali_string_bytes(left), chali_string_bytes(right), n);
    if (order != 0) return order < 0 ? -1 : 1;
    return a == b ? 0 : (a < b ? -1 : 1);
}
/**
 * @brief 截断起点和长度后复制指定字节子串。
 * @param value 待读取、写入、格式化或输出的值。
 * @param from 源字节偏移或切片起点。
 * @param count 请求复制的子串字节数。
 * @return 新构造的字符串句柄；空字符串仍具有有效的长度前缀。
 */
chali_i64 chali_string_sub(chali_i64 value, chali_i64 from, chali_i64 count)
{
    if (value == 0) return chali_string_new("", 0u);
    const chali_i64 length = chali_string_length(value);
    chali_i64 start = from < 0 ? 0 : from;
    chali_i64 take = count < 0 ? 0 : count;
    if (start > length) start = length;
    if (take > length - start) take = length - start;
    return chali_string_new(chali_string_bytes(value) + start, (size_t)take);
}
/**
 * @brief 查找子串并返回首次匹配的字节位置。
 * @param value 待读取、写入、格式化或输出的值。
 * @param needle 待查找的子字符串句柄。
 * @return 首次匹配的字节下标；未找到时返回 -1。
 */
chali_i64 chali_string_pos(chali_i64 value, chali_i64 needle)
{
    if (value == 0 || needle == 0) return -1;
    const chali_i64 length = chali_string_length(value);
    const chali_i64 wanted = chali_string_length(needle);
    if (wanted == 0) return 0;
    for (chali_i64 at = 0; at + wanted <= length; ++at)
        if (memcmp(chali_string_bytes(value) + at, chali_string_bytes(needle), (size_t)wanted) == 0) return at;
    return -1;
}
/**
 * @brief 从标准输入逐字节读取完整行，仅移除 LF 及其前面的一个 CR。
 * @return 新字符串句柄；EOF 返回空串，零字节及内存不足终止执行。
 */
chali_i64 chali_read_line(void)
{
    size_t capacity = 256, length = 0;
    char *buffer = (char *)malloc(capacity);
    if (!buffer) chali_panic("读取标准输入时内存不足");
    int byte;
    while ((byte = fgetc(stdin)) != -1 && byte != '\n')
    {
        if (byte == 0)
        {
            free(buffer);
            chali_panic("readLine 不接受零字节，String 不能包含内嵌零字节");
        }
        if (length == capacity)
        {
            if (capacity > (size_t)-1 / 2) chali_panic("标准输入行过长");
            capacity *= 2;
            char *grown = (char *)realloc(buffer, capacity);
            if (!grown) { free(buffer); chali_panic("读取标准输入时内存不足"); }
            buffer = grown;
        }
        buffer[length++] = (char)byte;
    }
    if (byte == '\n' && length && buffer[length - 1] == '\r') --length;
    const chali_i64 result = chali_string_new(buffer, length);
    free(buffer);
    return result;
}

/**
 * @brief 调用 libc 将浮点数格式化为固定小数位文本，不处理尾零。
 * @param value 浮点值。
 * @param precision 小数位数，负数使用三位。
 * @return 新字符串句柄；保留原有 255 字节截断规则，格式化失败终止执行。
 */
chali_i64 chali_string_fixed(double value, chali_i64 precision)
{
    char buffer[256];
    if (precision < 0) precision = 3;
    const int written = snprintf(buffer, sizeof(buffer), "%.*f", (int)precision, value);
    if (written < 0) chali_panic("浮点数格式化失败");
    const size_t length = (size_t)written < sizeof(buffer) ? (size_t)written : sizeof(buffer) - 1;
    return chali_string_new(buffer, length);
}
/**
 * @brief 读取一行并解析整数或浮点位模式。
 * @param wantFloat 非零时解析浮点并返回其位模式。
 * @return 解析的整数或双精度浮点值的原始位模式。
 */
chali_i64 chali_read_number(int wantFloat)
{
    char buffer[256];
    if (fgets(buffer, (int)sizeof buffer, stdin) == NULL) return 0;
    if (wantFloat != 0)
    {
        double value = 0.0;
        if (sscanf(buffer, "%lf", &value) != 1) value = 0.0;
        chali_i64 bits = 0;
        double copy = value;
        memcpy(&bits, &copy, 8);
        return bits;
    }
    long long value = 0;
    if (sscanf(buffer, "%lld", &value) != 1) value = 0;
    return (chali_i64)value;
}

/**
 * @brief 查询标准输入在最近一次读取后是否已经到达 EOF。
 * @return 到达 EOF 返回一，否则返回零。
 */
chali_i64 chali_stdin_eof(void) { return feof(stdin) != 0; }

/**
 * @brief 将整数格式化为十进制字符串。
 * @param value 待读取、写入、格式化或输出的值。
 * @return 新构造的字符串句柄；空字符串仍具有有效的长度前缀。
 */
chali_i64 chali_string_of_int(chali_i64 value)
{
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%lld", (long long)value);
    return chali_string_new(buffer, strlen(buffer));
}
/**
 * @brief 将双精度浮点格式化为字符串。
 * @param value 待读取、写入、格式化或输出的值。
 * @return 新构造的字符串句柄；空字符串仍具有有效的长度前缀。
 */
chali_i64 chali_string_of_double(double value)
{
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%g", value);
    return chali_string_new(buffer, strlen(buffer));
}

/**
 * @brief 为 LLVM 生成器提供不含宿主指针参数的下标检查入口。
 * @param index 从零开始的字节或元素下标。
 * @param limit 数组元素数量，不包含该上界。
 */
void chali_check(chali_i64 index, chali_i64 limit) { chali_bounds(index, limit, "下标"); }
/**
 * @brief 为 LLVM 生成器提供偏移指针检查入口。
 * @param address 运行时内存中的字节偏移。
 */
void chali_check_pointer(chali_i64 address) { chali_null(address, "访问"); }
/**
 * @brief 输出整数低八位对应的字节。
 * @param value 待读取、写入、格式化或输出的值。
 */
void chali_put_char(chali_i64 value) { fputc((int)(value & 255), stdout); }
/**
 * @brief 输出语言错误消息后报告 panic 并退出。
 * @param message 错误或断言消息；句柄值为零时省略消息。
 */
void chali_abort(chali_i64 message)
{
    if (message)
    {
        chali_fmt_begin(message);
        chali_fmt_finish();
    }
    chali_panic("panic");
}
/**
 * @brief 条件为零时输出错误消息并终止。
 * @param condition 断言条件；零表示失败。
 * @param message 错误或断言消息；句柄值为零时省略消息。
 */
void chali_assert(chali_i64 condition, chali_i64 message)
{
    if (!condition) chali_abort(message);
}
/**
 * @brief 在运行时内存中按字节复制允许重叠的范围。
 * @param to 目标范围的字节偏移。
 * @param from 源字节偏移或切片起点。
 * @param size 内存操作的字节数量。
 * @return 目标范围的字节偏移。
 */
chali_i64 chali_copy_bytes(chali_i64 to, chali_i64 from, chali_i64 size)
{
    memmove(chali_host_address(to, (size_t)size), chali_host_address(from, (size_t)size), (size_t)size);
    return to;
}
/**
 * @brief 将运行时内存范围填充为指定字节。
 * @param to 目标范围的字节偏移。
 * @param byte 要写入的字节值，使用低八位。
 * @param size 内存操作的字节数量。
 * @return 目标范围的字节偏移。
 */
chali_i64 chali_set_bytes(chali_i64 to, chali_i64 byte, chali_i64 size)
{
    memset(chali_host_address(to, (size_t)size), (int)byte, (size_t)size);
    return to;
}
/**
 * @brief 读取字符串指定位置的无符号字节。
 * @param value 待读取、写入、格式化或输出的值。
 * @param index 从零开始的字节或元素下标。
 * @return 指定位置的无符号字节值。
 */
chali_i64 chali_string_at(chali_i64 value, chali_i64 index) { return (unsigned char)chali_string_bytes(value)[index]; }
/**
 * @brief 写入堆字符串指定位置的一个字节。
 * @param value 待读取、写入、格式化或输出的值。
 * @param index 从零开始的字节或元素下标。
 * @param byte 要写入的字节值，使用低八位。
 */
void chali_string_set(chali_i64 value, chali_i64 index, chali_i64 byte) { chali_store_u8(value + 8 + index, byte); }
/**
 * @brief 按长度拼接两个字符串，保留嵌入的零字节。
 * @param left 左侧源字符串句柄。
 * @param right 右侧源字符串句柄。
 * @return 新构造的字符串句柄；空字符串仍具有有效的长度前缀。
 */
chali_i64 chali_string_concat(chali_i64 left, chali_i64 right)
{
    const size_t a = (size_t)chali_string_length(left), b = (size_t)chali_string_length(right);
    const chali_i64 at = chali_alloc(a + b + 8);
    chali_store_i64(at, (chali_i64)(a + b));
    memcpy(chali_host_address(at + 8, a), chali_string_bytes(left), a);
    memcpy(chali_host_address(at + 8 + a, b), chali_string_bytes(right), b);
    return at;
}

/**
 * @brief 按共享生命周期协议释放一个拥有值。
 * @param value 活跃句柄或内存地址。
 */
void chali_owned_drop(chali_i64 value) { chali_host_drop(value); }
/**
 * @brief 解析函数值的原生标签。
 * @param function 函数地址或闭包句柄。
 * @return 静态标签地址。
 */
chali_i64 chali_function_target(chali_i64 function) { return chali_host_function(function, 0); }
/**
 * @brief 取得函数值的隐藏捕获环境。
 * @param function 函数地址或闭包句柄。
 * @return 环境句柄，没有环境时为零。
 */
chali_i64 chali_function_environment(chali_i64 function) { return chali_host_function(function, 1); }
/**
 * @brief 比较实际函数标签与生成的原生入口名称。
 * @param address 静态标签地址。
 * @param name 完整函数名称。
 * @return 相同返回一，否则返回零。
 */
chali_i64 chali_function_match(chali_i64 address, const char *name)
{
    if (address <= 0 || (size_t)address >= chali_data_size) return 0;
    const size_t length = strlen(name);
    if (length >= chali_data_size - (size_t)address) return 0;
    return memcmp(chali_host_address(address, length + 1), name, length + 1) == 0;
}
