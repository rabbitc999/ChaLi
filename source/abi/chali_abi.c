/**
 * @file chali_abi.c
 * @brief 直通入口表的聚合与解析。
 *
 * 各 C 胶水模块各自导出一张入口表，本文件把它们聚合成一张全局表，排序一次后
 * 供二分查找使用。排序在运行时完成，因此各模块内部的表项顺序没有约束。
 *
 * 新增直通入口：在对应模块的 `abi_*.c` 中实现入口并加入该模块的表，
 * 无需改动本文件；若新增模块，则把它的登记表函数加进 `kModules`。
 */
#include "chali_abi_internal.h"
#include "chali_standard.h"

#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>

/* ---------------------------------------------------------------------------
 *  模块登记表
 * ------------------------------------------------------------------------- */

/**
 * @brief 一个 C 胶水模块的入口表访问器。
 */
typedef const ChaliAbiRecord *(*ModuleRecords)(size_t *count);

/**
 * @brief 参与聚合的模块；新增模块时在此登记。
 * @note 裁剪宏与旧 C++ 胶水保持同一套名字：定义 `GLUE_NO_STD_XXX` 即不把该模块
 *       编进直通表，宿主因此可以按需只带需要的胶水。
 */
static const ModuleRecords kModules[] = {
    abi_print_records,
    abi_string_records,
#ifndef GLUE_NO_STD_COMPRESS
    abi_compress_records,
#endif
#ifndef GLUE_NO_STD_PROCESS
    abi_process_records,
#endif
#ifndef GLUE_NO_STD_JSON
    abi_serialization_records,
    abi_json_records,
#endif
#ifndef GLUE_NO_STD_TOML
    abi_toml_records,
#endif
#ifndef GLUE_NO_STD_PARSE
    abi_parse_records,
#endif
#ifndef GLUE_NO_STD_MATH
    abi_math_records,
    /* 时钟原先与数学函数同在一个 C++ 胶水文件里，沿用同一个裁剪宏。 */
    abi_clock_records,
#endif
#ifndef GLUE_NO_STD_RANDOM
    abi_random_records,
#endif
};

/**
 * @brief 聚合后的入口表容量上限，须不小于各模块表项之和。
 */
#define CHALI_ABI_MAX_RECORDS 1024

static ChaliAbiRecord gTable[CHALI_ABI_MAX_RECORDS];
static size_t gCount = 0;
static atomic_int gPrepared = ATOMIC_VAR_INIT(0);  ///< 零未开始，一构建中，二成功，三失败。

/**
 * @brief 按入口名称比较两个表项，供 `qsort` 使用。
 * @param left 左侧表项。
 * @param right 右侧表项。
 * @return 名称的字典序比较结果。
 */
static int compareRecord(const void *left, const void *right)
{
    const ChaliAbiRecord *a = (const ChaliAbiRecord *)left;
    const ChaliAbiRecord *b = (const ChaliAbiRecord *)right;
    return strcmp(a->name, b->name);
}

/**
 * @brief 聚合各模块入口表并排序；可重复调用，只准备一次。
 * @note 原子状态发布完整只读表，不允许其他线程读取构建中的数据。
 */
void chali_abi_prepare(void)
{
    if (atomic_load_explicit(&gPrepared, memory_order_acquire) >= 2) return;
    int expected = 0;
    if (!atomic_compare_exchange_strong_explicit(&gPrepared, &expected, 1, memory_order_acq_rel, memory_order_acquire))
    {
        while (atomic_load_explicit(&gPrepared, memory_order_acquire) == 1) {}
        return;
    }
    gCount = 0;
    int valid = 1;
    for (size_t module = 0; module < sizeof(kModules) / sizeof(kModules[0]); ++module)
    {
        size_t count = 0;
        const ChaliAbiRecord *records = kModules[module](&count);
        if ((count && records == NULL) || count > CHALI_ABI_MAX_RECORDS - gCount)
        {
            valid = 0;
            break;
        }
        for (size_t at = 0; at < count; ++at)
        {
            if (!records[at].name || !records[at].name[0] || !records[at].entry || records[at].arity < -1)
            {
                valid = 0;
                break;
            }
            gTable[gCount++] = records[at];
        }
        if (!valid) break;
    }
    if (gCount > 1u) qsort(gTable, gCount, sizeof(gTable[0]), compareRecord);
    for (size_t at = 1; at < gCount && valid; ++at)
        if (strcmp(gTable[at - 1].name, gTable[at].name) == 0) valid = 0;
    if (!valid) gCount = 0;
    atomic_store_explicit(&gPrepared, valid ? 2 : 3, memory_order_release);
}

const ChaliAbiRecord *chali_abi_records(size_t *count)
{
    chali_abi_prepare();
    if (count != NULL) *count = gCount;
    return atomic_load_explicit(&gPrepared, memory_order_acquire) == 2 ? gTable : NULL;
}

ChaliAbiEntry chali_abi_resolve(const char *name, int *arity)
{
    if (name == NULL || *name == '\0') return NULL;
    chali_abi_prepare();
    if (atomic_load_explicit(&gPrepared, memory_order_acquire) != 2) return NULL;
    size_t low = 0;
    size_t high = gCount;
    while (low < high)
    {
        const size_t middle = low + (high - low) / 2u;
        const int order = strcmp(name, gTable[middle].name);
        if (order == 0)
        {
            if (arity != NULL) *arity = gTable[middle].arity;
            return gTable[middle].entry;
        }
        if (order < 0)
            high = middle;
        else
            low = middle + 1u;
    }
    return NULL;
}

/* ---------------------------------------------------------------------------
 *  帧访问辅助
 * ------------------------------------------------------------------------- */

int chali_abi_argument_integer(const ChaliAbiFrame *frame, int64_t index, int64_t *out)
{
    if (out == NULL)
    {
        chali_abi_fail(frame, "missing integer argument output");
        return 0;
    }
    *out = 0;
    if (frame == NULL || frame->slots == NULL || frame->kinds == NULL || index < 0 || index >= frame->count || frame->kinds[index] != ChaliAbiSlotInteger)
    {
        chali_abi_fail(frame, "invalid integer argument slot");
        return 0;
    }
    *out = frame->slots[index];
    return 1;
}

int chali_abi_argument_real(const ChaliAbiFrame *frame, int64_t index, double *out)
{
    if (out == NULL)
    {
        chali_abi_fail(frame, "missing floating argument output");
        return 0;
    }
    *out = 0.0;
    if (frame == NULL || frame->slots == NULL || frame->kinds == NULL || index < 0 || index >= frame->count || frame->kinds[index] != ChaliAbiSlotFloat)
    {
        chali_abi_fail(frame, "invalid floating argument slot");
        return 0;
    }
    const int64_t bits = frame->slots[index];
    double value = 0.0;
    memcpy(&value, &bits, sizeof(value));
    *out = value;
    return 1;
}

int chali_abi_argument_kind(const ChaliAbiFrame *frame, int64_t index)
{
    if (frame == NULL || frame->kinds == NULL || index < 0 || index >= frame->count || frame->kinds[index] > ChaliAbiSlotFloat)
    {
        chali_abi_fail(frame, "invalid argument slot kind");
        return ChaliAbiSlotInteger;
    }
    return frame->kinds[index];
}

void chali_abi_return_integer(ChaliAbiFrame *frame, int64_t value)
{
    if (frame == NULL) return;
    frame->result = value;
    frame->resultIsFloat = 0;
}

int64_t chali_abi_slot_from_real(double value)
{
    int64_t bits = 0;
    memcpy(&bits, &value, sizeof(bits));
    return bits;
}

void chali_abi_return_real(ChaliAbiFrame *frame, double value)
{
    if (frame == NULL) return;
    frame->result = chali_abi_slot_from_real(value);
    frame->resultIsFloat = 1;
}

/* ---------------------------------------------------------------------------
 *  帧基础访问
 * ------------------------------------------------------------------------- */

int64_t chali_abi_integer(const ChaliAbiFrame *frame, int64_t index)
{
    int64_t value = 0;
    chali_abi_argument_integer(frame, index, &value);
    return value;
}

double chali_abi_real(const ChaliAbiFrame *frame, int64_t index)
{
    double value = 0.0;
    chali_abi_argument_real(frame, index, &value);
    return value;
}
