#include "std_native_record.hpp"
#include "std_registry.hpp"
#include "std_reference.hpp"
#include "std_collection.hpp"
#include "std_file.hpp"
#include "std_big_number.hpp"
#include "std_cas.hpp"

/* 统一的"标准胶水"注册入口：编译器 VM（InstallDemoGlue）与独立 VM 宿主（bindChaLiStandardGlue）
 * 共用这一份。每个模块的注册都用 `#ifndef GLUE_NO_XXX` 包住，定义宏即整段不编进来：
 *
 *   - `import std::xxx;` 决定 CHALI 侧符号可见性；
 *   - `GLUE_NO_STD_XXX` 决定宿主侧注册是否编译进来。
 * 两者互补，同时用才是完整裁剪。默认不定义任何宏 = 现有行为。 */

#include "../chali_glue.hpp"
#ifndef GLUE_NO_STD_FILESYSTEM
#include "std_filesystem.hpp"
#endif

#ifndef GLUE_NO_STD_REGEX
#include "std_regex.hpp"
#endif
#ifndef GLUE_NO_STD_SIGNAL
#include "std_signal.hpp"
#endif
#ifndef GLUE_NO_STD_JSON
#ifndef PARSE_ENGINE_STD_JSON_HPP
#include "std_json.hpp"
#endif
#endif
#ifndef GLUE_NO_STD_TOML
#ifndef PARSE_ENGINE_STD_TOML_HPP
#include "std_toml.hpp"
#endif
#endif
#ifndef GLUE_NO_STD_BINARY
#include "std_binary.hpp"
#endif
#ifndef GLUE_NO_STD_COMPRESS
#include "std_compress.hpp"
#endif
#ifndef GLUE_NO_STD_LINALG
#include "std_linalg.hpp"
#endif
#ifndef GLUE_NO_STD_PARSE
#include "std_parse.hpp"
#endif
#ifndef GLUE_NO_STD_PROCESS
#include "std_process.hpp"
#endif
#ifndef GLUE_NO_STD_SQL
#include "std_sql.hpp"
#endif
#ifndef GLUE_NO_STD_IMAGE
#include "std_image.hpp"
#endif
#ifndef GLUE_NO_STD_NET
#include "std_net.hpp"
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
    ok = std_reference_glue::bind(registry, error) && ok;
    ok = collection_glue::bind(registry.native(), error) && ok;
    ok = big_number_glue::bind(registry.native(), error) && ok;
    ok = std_cas_glue::bind(registry.native(), error) && ok;
    ok = file_native_glue::bind(registry.native(), error) && ok;
#ifndef GLUE_NO_STD_FILESYSTEM
    ok = std_filesystem_glue::bind(registry, error) && ok;
#endif
#ifndef GLUE_NO_STD_JSON
    ok = std_json_glue::bind(registry, error) && ok;
#endif
#ifndef GLUE_NO_STD_TOML
    ok = std_toml_glue::bind(registry, error) && ok;
#endif
#ifndef GLUE_NO_STD_BINARY
    if (!std_binary_glue::bind(registry, error)) ok = false;
#endif
#ifndef GLUE_NO_STD_COMPRESS
    if (!std_compress_glue::bind(registry, error)) ok = false;
#endif
#ifndef GLUE_NO_STD_REGEX
    Memory::String regexError;
    if (!std_regex::bind(registry, &regexError))
    {
        if (error) *error = regexError;
        ok = false;
    }
#endif
#ifndef GLUE_NO_STD_SIGNAL
    Memory::String signalError;
    if (!std_signal_glue::bind(registry, &signalError))
    {
        if (error) *error = signalError;
        ok = false;
    }
#endif
#ifndef GLUE_NO_STD_LINALG
    Memory::String linalgError;
    if (!std_linalg_glue::bind(registry, &linalgError))
    {
        if (error) *error = linalgError;
        ok = false;
    }
#endif
#ifndef GLUE_NO_STD_PARSE
    Memory::String parseError;
    if (!std_parse_glue::bind(registry, &parseError))
    {
        if (error) *error = parseError;
        ok = false;
    }
#endif
#ifndef GLUE_NO_STD_PROCESS
    Memory::String processError;
    if (!std_process_glue::bind(registry, &processError))
    {
        if (error) *error = processError;
        ok = false;
    }
#endif
#ifndef GLUE_NO_STD_SQL
    Memory::String sqlError;
    if (!std_sql_glue::bind(registry, &sqlError))
    {
        if (error) *error = sqlError;
        ok = false;
    }
#endif
#ifndef GLUE_NO_STD_IMAGE
    Memory::String imageError;
    if (!std_image_glue::bind(registry, &imageError))
    {
        if (error) *error = imageError;
        ok = false;
    }
#endif
#ifndef GLUE_NO_STD_NET
    Memory::String netError;
    if (!std_net_glue::bind(registry, &netError))
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
