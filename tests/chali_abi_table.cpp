/**
 * @file chali_abi_table.cpp
 * @brief 直通入口表的自检：排序、解析一致性与越界查询。
 *
 * 链接实际 C API 入口及其私有格式内核，不依赖运行时：验证的是「名字 → 入口」这张表本身，
 * 与具体胶水函数的语义无关。
 */
#include "abi/chali_abi_internal.h"
#include "abi/chali_standard.h"
#include <fstream>
#include <sstream>
#include <string>

#include <cstdio>
#include <cstring>

/* 本测试只验证表结构，不运行任何胶水函数。这里给运行时服务提供空实现，
 * 使测试可以脱离运行时核心与桥接层单独编译。 */
extern "C" {
/**
 * @brief 表结构测试不提供实际注册表；误用注册入口时明确失败。
 * @param registry 未使用的注册表。
 * @param module 未使用的模块描述符。
 * @param error 未使用的错误缓冲。
 * @return 固定返回参数无效，实际注册语义由注册表专项验证。
 */
int32_t CHALI_ABI_CALL chali_abi_registry_register(ChaliAbiRegistry *registry,
    const ChaliAbiModuleDescriptor *module, ChaliAbiError *error)
{
    (void)registry;
    (void)module;
    (void)error;
    return ChaliAbiInvalidArgument;
}
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
/** @brief 入口表测试不消费实际输入。
 * @param frame 未使用帧。
 * @param position 可选输出位置。
 * @return 固定 EOF。
 */
int64_t chali_abi_input_token(const ChaliAbiFrame *frame,uint64_t *position) {(void)frame;if(position)*position=0;return 0;}
/** @brief 入口表测试不构造具有文本协议的实际对象。
 * @param frame 未使用帧。
 * @param name 未使用类型名。
 * @param value 未使用对象。
 * @param destroy 未使用销毁回调。
 * @param clone 未使用复制回调。
 * @param format 未使用文本回调。
 * @return 固定零句柄。
 */
int64_t chali_abi_record_make_textual(const ChaliAbiFrame *frame,const char *name,void *value,ChaliAbiDestroy destroy,ChaliAbiClone clone,ChaliAbiFormat format) {(void)frame;(void)name;(void)value;(void)destroy;(void)clone;(void)format;return 0;}
/**
 * @brief 入口表测试不调用私有集合内核。
 * @param frame 未使用帧。
 * @param operation 未使用编号。
 */
void chali_abi_collection_apply(ChaliAbiFrame *frame,size_t operation) {(void)frame;(void)operation;}
/**
 * @brief 入口表测试不构造运行期聚合暂存。
 * @param frame 未使用帧。
 * @param bytes 未使用大小。
 * @return 固定零句柄。
 */
int64_t chali_abi_tracked_temporary(const ChaliAbiFrame *frame,size_t bytes) {(void)frame;(void)bytes;return 0;}
/**
 * @brief 表结构验证不释放实际临时块。
 * @param frame 未使用帧。
 * @param address 未使用地址。
 * @return 固定成功占位值。
 */
int chali_abi_temporary_release(const ChaliAbiFrame *frame,int64_t address) {(void)frame;(void)address;return 1;}
int64_t chali_abi_record_make(const ChaliAbiFrame *, const char *, void *, void (*)(void *)) { return 0; }
/**
 * @brief 表结构检查不登记实际记录描述符。
 * @param name 未使用的类型名。
 * @param destroy 未使用的销毁回调。
 * @param clone 未使用的复制回调。
 * @return 空占位描述符。
 */
const void *chali_abi_record_type(const char *name, void (*destroy)(void *), ChaliAbiClone clone)
{ (void)name; (void)destroy; (void)clone; return nullptr; }
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
/** @brief 表结构测试不读运行时内存。
 * @param frame 未使用的帧。
 * @param address 未使用的地址。
 * @param output 未使用的缓冲。
 * @param length 未使用的长度。
 * @return 固定失败占位值。
 */
int chali_abi_read(const ChaliAbiFrame *frame, int64_t address, void *output, size_t length)
{ (void)frame; (void)address; (void)output; (void)length; return 0; }
/** @brief 表结构测试不写运行时内存。
 * @param frame 未使用的帧。
 * @param address 未使用的地址。
 * @param data 未使用的输入。
 * @param length 未使用的长度。
 * @return 固定失败占位值。
 */
int chali_abi_write(const ChaliAbiFrame *frame, int64_t address, const void *data, size_t length)
{ (void)frame; (void)address; (void)data; (void)length; return 0; }
/** @brief 表结构测试不释放实际字符串。
 * @param frame 未使用的帧。
 * @param address 未使用的地址。
 * @return 固定失败占位值。
 */
int chali_abi_string_release(const ChaliAbiFrame *frame, int64_t address)
{ (void)frame; (void)address; return 0; }
/** @brief 表结构测试不向实际列表追加文本。
 * @param frame 未使用的帧。
 * @param handle 未使用的句柄。
 * @param data 未使用的输入。
 * @param length 未使用的长度。
 * @return 固定失败占位值。
 */
int chali_abi_list_append_text(const ChaliAbiFrame *frame, int64_t handle, const char *data, size_t length)
{ (void)frame; (void)handle; (void)data; (void)length; return 0; }
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
 * @param argc 参数个数；可额外提供演示声明的冻结夹具。
 * @param argv 程序名及可选夹具路径。
 * @return 全部通过时为 0，否则为失败项数。
 */
int main(int argc,char **argv)
{
    size_t length=0;const auto *source=abi_demo_declarations(&length);
    if(!source||length!=std::strlen(source))return 10;
    if(!abi_demo_declarations_used("GlueDemoPoint",13)||abi_demo_declarations_used("prefixGlueDemoPoint",19)||abi_demo_declarations_used("GlueDemoPointSuffix",19))return 11;
    if(argc==2){
        std::ifstream file(argv[1],std::ios::binary);std::ostringstream expected;expected<<file.rdbuf();
        auto text=expected.str();for(size_t at=0;(at=text.find("\r\n",at))!=std::string::npos;)text.erase(at,1);
        if(!file||text!=std::string(source,length))return 12;
    }

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

/** @brief 表结构测试的数值列表查询占位。
 * @param frame 未使用的帧。
 * @param handle 未使用的句柄。
 * @param bits 未使用的输出。
 * @param kinds 未使用的类别。
 * @param capacity 未使用的容量。
 * @param required 清零的长度。
 * @return 固定失败值。
 */
int chali_abi_list_numbers(const ChaliAbiFrame *frame,int64_t handle,int64_t *bits,unsigned char *kinds,size_t capacity,size_t *required)
{(void)frame;(void)handle;(void)bits;(void)kinds;(void)capacity;if(required)*required=0;return 0;}
/** @brief 表结构测试的数值追加占位。
 * @param frame 未使用的帧。
 * @param handle 未使用的句柄。
 * @param bits 未使用的输入。
 * @param kinds 未使用的类别。
 * @param count 未使用的数量。
 * @return 固定失败值。
 */
int chali_abi_list_append_numbers(const ChaliAbiFrame *frame,int64_t handle,const int64_t *bits,const unsigned char *kinds,size_t count)
{(void)frame;(void)handle;(void)bits;(void)kinds;(void)count;return 0;}
