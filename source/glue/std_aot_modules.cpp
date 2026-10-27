/**
 * @file std_aot_modules.cpp
 * @brief 每个标准库模块的独立 C 入口与仅编译器使用的选择表。
 */
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

#include "std_aot_modules.hpp"
#include <cstdlib>
/**
 * @brief 注册单个已选择模块，独立函数节允许链接器回收未选模块。
 * @param target 接收绑定的注册表。
 * @param error 接收绑定错误。
 * @return 成功时为一，失败时为零。
 */
#define CHALI_AOT_MODULE(name, expression)                                                             \
    extern "C" std::int32_t chali_std_bind_##name(chali_glue::Registry *target, Memory::String *error) \
    {                                                                                                  \
        auto &registry = *target;                                                                      \
        return (expression) ? 1 : 0;                                                                   \
    }
#include "aot_modules.inc"
#undef CHALI_AOT_MODULE

namespace std_registry
{
/**
 * @brief 从每个模块实际绑定的名称判定可达依赖，不猜测模块前缀。
 * @param names 可达原生调用。
 * @param diagnostics 绑定错误。
 * @return 需要链接的模块名称。
 */
Memory::Vector<Memory::String> aotModulesFor(const Memory::Vector<Memory::String> &names, Memory::Vector<Memory::String> *diagnostics)
{
    /** @brief 选择表中的独立模块绑定入口。 */
    struct Module
    {
        const char *name;
        std::int32_t (*bind)(chali_glue::Registry *, Memory::String *);
    };
    static const Module modules[] = {
#define CHALI_AOT_MODULE(name, expression) { #name, &chali_std_bind_##name },
#include "aot_modules.inc"
#undef CHALI_AOT_MODULE
    };
    Memory::Vector<Memory::String> required;
    const auto *full = std::getenv("CHALI_AOT_FULL_STDLIB");
    for (const auto &module : modules)
    {
        if (full && *full)
        {
            required.push_back(module.name);
            continue;
        }
        chali_glue::Registry registry;
        Memory::String error;
        if (!module.bind(&registry, &error))
        {
            if (diagnostics) diagnostics->push_back("AOT 标准库模块绑定失败：" + error);
            continue;
        }
        for (const auto &name : names)
            if (registry.native().find(name))
            {
                required.push_back(module.name);
                break;
            }
    }
    return required;
}
}  // namespace std_registry
