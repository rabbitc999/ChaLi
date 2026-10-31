/**
 * @file chali_abi_table.cpp
 * @brief 直通入口表的自检：排序、解析一致性与越界查询。
 *
 * 链接实际 C API 入口及其私有格式内核，不依赖运行时：验证的是「名字 → 入口」这张表本身，
 * 与具体胶水函数的语义无关。
 */
#include "abi/chali_abi_internal.h"

#include <cstdio>
#include <cstring>

/* 本测试只验证表结构，不运行任何胶水函数。这里给运行时服务提供空实现，
 * 使测试可以脱离运行时核心与桥接层单独编译。 */
extern "C" {
int chali_abi_bytes(const ChaliAbiFrame *, int64_t, const unsigned char **data, size_t *length)
{
    if (data != nullptr) *data = nullptr;
    if (length != nullptr) *length = 0;
    return 0;
}
int64_t chali_abi_string(const ChaliAbiFrame *, const char *, size_t) { return 0; }
/**
 * @brief 表结构测试不运行原始字符串分配。
 * @param frame 未使用的帧。
 * @param data 未使用的原始字节。
 * @param length 未使用的长度。
 * @return 固定零占位值。
 */
int64_t CHALI_ABI_CALL chali_abi_string_bytes(const ChaliAbiFrame *frame, const char *data, size_t length)
{ (void)frame; (void)data; (void)length; return 0; }
/**
 * @brief 表结构测试不读取实际集合。
 * @param frame 未使用的帧。
 * @param handle 未使用的列表句柄。
 * @param data 未使用的目标。
 * @param capacity 未使用的容量。
 * @param required 可选的长度接收器，清零。
 * @return 固定失败占位值。
 */
int CHALI_ABI_CALL chali_abi_list_bytes(const ChaliAbiFrame *frame, int64_t handle, unsigned char *data, size_t capacity, size_t *required)
{ (void)frame; (void)handle; (void)data; (void)capacity; if(required) *required=0; return 0; }
/**
 * @brief 表结构测试不创建实际集合。
 * @param frame 未使用的帧。
 * @param data 未使用的输入。
 * @param length 未使用的长度。
 * @return 固定零占位值。
 */
int64_t CHALI_ABI_CALL chali_abi_list_from_bytes(const ChaliAbiFrame *frame, const unsigned char *data, size_t length)
{ (void)frame; (void)data; (void)length; return 0; }
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
/**
 * @brief 表结构测试不创建拥有记录。
 * @param frame 未使用的帧。
 * @param name 未使用的类型名。
 * @param value 未使用的记录。
 * @param destroy 未使用的销毁回调。
 * @param clone 未使用的复制回调。
 * @return 固定零占位值。
 */
int64_t CHALI_ABI_CALL chali_abi_record_make_clonable(const ChaliAbiFrame *frame, const char *name,
    void *value, ChaliAbiDestroy destroy, ChaliAbiClone clone)
{ (void)frame; (void)name; (void)value; (void)destroy; (void)clone; return 0; }
void *chali_abi_record_borrow(const ChaliAbiFrame *, int64_t, const char *) { return nullptr; }
/**
 * @brief 表结构测试不探测实际记录。
 * @param frame 未使用的帧。
 * @param handle 未使用的句柄。
 * @param name 未使用的类型名。
 * @return 空占位指针。
 */
void *CHALI_ABI_CALL chali_abi_record_probe(const ChaliAbiFrame *frame, int64_t handle, const char *name)
{ (void)frame; (void)handle; (void)name; return nullptr; }
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
    const auto local = chali_abi_local_info(0);
    const ChaliAbiModuleDescriptor module{sizeof(ChaliAbiModuleDescriptor), &local, records, count};
    char diagnostic[128];
    ChaliAbiError error{diagnostic, sizeof(diagnostic), 0, 0};
    if (chali_abi_check_module(&module, &error) != ChaliAbiOk)
        report("聚合 C 模块不满足公开协议", diagnostic);
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
