/**
 * @file chali_abi_table.cpp
 * @brief 直通入口表的自检：排序、解析一致性与越界查询。
 *
 * 只链接 C 侧胶水单元，不依赖运行时：验证的是「名字 → 入口」这张表本身，
 * 与具体胶水函数的语义无关。
 */
#include "abi/chali_abi_internal.h"

#include <cstdio>
#include <cstring>

/* 本测试只验证表结构，不运行任何胶水函数。这里给运行时服务提供空实现，
 * 使测试可以脱离 VM 核心与桥接层单独编译。 */
extern "C" {
int chali_abi_bytes(const ChaliAbiFrame *, int64_t, const unsigned char **data, size_t *length)
{
    if (data != nullptr) *data = nullptr;
    if (length != nullptr) *length = 0;
    return 0;
}
int64_t chali_abi_string(const ChaliAbiFrame *, const char *, size_t) { return 0; }
int64_t chali_abi_temporary(const ChaliAbiFrame *, size_t) { return 0; }
void chali_abi_emit(const ChaliAbiFrame *, const char *, size_t) {}
void chali_abi_emit_error(const ChaliAbiFrame *, const char *, size_t, int) {}
void chali_abi_fail(const ChaliAbiFrame *, const char *) {}
int chali_abi_invoke(const ChaliAbiFrame *, int64_t, const int64_t *, const unsigned char *, int64_t,
                     int64_t *, int *)
{
    return 0;
}
int64_t chali_abi_record_make(const ChaliAbiFrame *, const char *, void *, void (*)(void *)) { return 0; }
void *chali_abi_record_borrow(const ChaliAbiFrame *, int64_t, const char *) { return nullptr; }
int64_t chali_abi_record_clone(const ChaliAbiFrame *, int64_t, const char *, void *(*)(const void *))
{
    return 0;
}
int chali_abi_record_release(const ChaliAbiFrame *, int64_t) { return 0; }
}

namespace
{
int failures = 0;

/**
 * @brief 记录一条失败并继续检查其余表项。
 * @param message 失败说明。
 * @param name 相关入口名称，可为空。
 */
void report(const char *message, const char *name)
{
    std::fprintf(stderr, "失败：%s", message);
    if (name != nullptr) std::fprintf(stderr, "（%s）", name);
    std::fprintf(stderr, "\n");
    ++failures;
}
}  // namespace

/**
 * @brief 逐项检查直通表的排序与解析一致性。
 * @return 全部通过时为 0，否则为失败项数。
 */
int main()
{
    chali_abi_prepare();

    std::size_t count = 0;
    const ChaliAbiRecord *records = chali_abi_records(&count);
    if (records == nullptr || count == 0u)
    {
        std::fprintf(stderr, "失败：直通表为空\n");
        return 1;
    }

    for (std::size_t at = 0; at < count; ++at)
    {
        const ChaliAbiRecord &record = records[at];
        if (record.name == nullptr || record.name[0] == '\0')
        {
            report("表项名称为空", nullptr);
            continue;
        }
        if (record.entry == nullptr) report("表项没有入口", record.name);
        if (at > 0u && std::strcmp(records[at - 1u].name, record.name) >= 0)
            report("表未按名称严格升序排列", record.name);

        int arity = 0;
        const ChaliAbiEntry resolved = chali_abi_resolve(record.name, &arity);
        if (resolved != record.entry) report("按名称解析得到的入口与表项不一致", record.name);
        if (arity != record.arity) report("按名称解析得到的实参数量与表项不一致", record.name);
    }

    if (chali_abi_resolve("std::math::__chali_abi_missing", nullptr) != nullptr)
        report("未知名称不该解析出入口", "std::math::__chali_abi_missing");
    if (chali_abi_resolve("", nullptr) != nullptr) report("空名称不该解析出入口", nullptr);
    if (chali_abi_resolve(nullptr, nullptr) != nullptr) report("空指针不该解析出入口", nullptr);

    if (failures == 0)
        std::printf("直通入口表自检通过：%zu 项\n", count);
    return failures;
}
