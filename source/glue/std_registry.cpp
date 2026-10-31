#include "std_native_record.hpp"
#include "std_registry.hpp"
#include "std_collection.hpp"
#include "std_cas.hpp"
#include "abi/chali_standard.h"
#include "abi/chali_abi_internal.h"

/* 统一的"标准胶水"注册入口：编译器 VM（InstallDemoGlue）与独立 VM 宿主（bindChaLiStandardGlue）
 * 共用这一份。每个模块的注册都用 `#ifndef GLUE_NO_XXX` 包住，定义宏即整段不编进来：
 *
 *   - `import std::xxx;` 决定 CHALI 侧符号可见性；
 *   - `GLUE_NO_STD_XXX` 决定宿主侧注册是否编译进来。
 * 两者互补，同时用才是完整裁剪。默认不定义任何宏 = 现有行为。 */

#include "../chali_glue.hpp"

#ifndef GLUE_NO_STD_SQL
#endif
#ifndef GLUE_NO_STD_IMAGE
#endif
#ifndef GLUE_NO_STD_NET
#endif

namespace std_registry
{

/**
 * @brief 为宿主注册 std_registry 模块的标准库接口。
 * @param registry 接收类型化函数绑定的宿主注册表。
 * @param error 绑定失败时接收原始诊断，可为空。
 * @return 全部入口绑定成功时返回 true。
 */
bool bindStandardGlue(chali_glue::Registry &registry, Memory::String *error)
{
    bool ok = true;
    ok = collection_glue::bind(registry.native(), error) && ok;
    ok = registry.bindModule(abi_big_number_records, error) && ok;
    ok = std_cas_glue::bind(registry.native(), error) && ok;
    ok = registry.bindModule(abi_file_records, error) && ok;
#ifndef GLUE_NO_STD_FILESYSTEM
    ok = registry.bindModule(abi_filesystem_records, error) && ok;
#endif
#ifndef GLUE_NO_STD_JSON
    ok = (registry.bindModule(abi_serialization_records, error) && registry.bindModule(abi_json_records, error)) && ok;
#endif
#ifndef GLUE_NO_STD_TOML
    ok = registry.bindModule(abi_toml_records, error) && ok;
#endif
#ifndef GLUE_NO_STD_COMPRESS
    if (!registry.bindModule(abi_compress_records, error)) ok = false;
#endif
#ifndef GLUE_NO_STD_REGEX
    Memory::String regexError;
    if (!registry.bindModule(abi_regex_records, &regexError))
    {
        if (error) *error = regexError;
        ok = false;
    }
#endif
#ifndef GLUE_NO_STD_LINALG
    Memory::String linalgError;
    if (!(registry.bindModule(abi_scalar_records, &linalgError) && registry.bindModule(abi_linalg_records, &linalgError)))
    {
        if (error) *error = linalgError;
        ok = false;
    }
#endif
#ifndef GLUE_NO_STD_PARSE
    Memory::String parseError;
    if (!registry.bindModule(abi_parse_records, &parseError))
    {
        if (error) *error = parseError;
        ok = false;
    }
#endif
#ifndef GLUE_NO_STD_PROCESS
    Memory::String processError;
    if (!registry.bindModule(abi_process_records, &processError))
    {
        if (error) *error = processError;
        ok = false;
    }
#endif
#ifndef GLUE_NO_STD_SQL
    Memory::String sqlError;
    if (!(registry.bindModule(abi_sql_records, &sqlError) && registry.bindModule(abi_sql_ipage_records, &sqlError)))
    {
        if (error) *error = sqlError;
        ok = false;
    }
#endif
#ifndef GLUE_NO_STD_IMAGE
    Memory::String imageError;
    if (!registry.bindModule(abi_image_records, &imageError))
    {
        if (error) *error = imageError;
        ok = false;
    }
#endif
#ifndef GLUE_NO_STD_NET
    Memory::String netError;
    if (!registry.bindModule(abi_net_records, &netError))
    {
        if (error) *error = netError;
        ok = false;
    }
#endif
    /* 已经迁移到 C 直通表的模块（print / math / clock / random）在这里补齐：
     * 它们没有对应的 C++ 胶水文件，只能靠直通表自注册；已由 C++ 绑定的名字保持不变。 */
    registry.bindDirectEntries();
    return ok;
}

}  // namespace std_registry

/**
 * @brief 用不透明 C 句柄登记完整标准库，捕获私有实现异常。
 * @param registry 同一运行时函数表的句柄。
 * @param error 调用方诊断缓冲。
 * @return 稳定状态码，成功为零。
 */
extern "C" int32_t chali_abi_bind_standard(ChaliAbiRegistry *registry, ChaliAbiError *error) try
{
    if (!registry || !registry->backend) return chali_abi_status(error, ChaliAbiInvalidArgument, "missing registry");
    if (registry->backend->sealed())
        return chali_abi_status(error, ChaliAbiSealed, "registry is sealed");
    Memory::AllocationScope allocation(registry->backend->allocationOwner());
    chali_glue::Registry view(registry);
    Memory::String message;
    const bool ok = std_registry::bindStandardGlue(view, &message);
    return chali_abi_status(error, ok ? ChaliAbiOk : ChaliAbiHostFailure,
                           ok ? nullptr : message.empty() ? "standard registration failed" : message.c_str());
}
catch (const std::exception &failure)
{ return chali_abi_status(error, ChaliAbiHostFailure, failure.what()); }
catch (...)
{ return chali_abi_status(error, ChaliAbiHostFailure, "standard registration failed"); }
