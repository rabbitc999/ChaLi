/**
 * @file std_aot_modules.cpp
 * @brief 每个标准库模块的独立 C 入口与仅编译器使用的选择表。
 */
#include "std_native_record.hpp"
#include "std_registry.hpp"
#include "std_collection.hpp"
#include "std_demo.hpp"
#include "std_cas.hpp"

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

#include "std_aot_modules.hpp"
#include "abi/chali_abi_internal.h"
#include "abi/chali_standard.h"
#include <cstdlib>
/**
 * @brief 注册单个已选择模块，独立函数节允许链接器回收未选模块。
 * @param target 接收绑定的注册表。
 * @param failure 调用方的有界错误缓冲。
 * @return 稳定 C ABI 状态码，成功为零。
 */
#define CHALI_AOT_MODULE(name, expression) \
    extern "C" int32_t chali_std_bind_##name(ChaliAbiRegistry *target, ChaliAbiError *failure) try \
    { \
        if (!target || !target->backend) return chali_abi_status(failure, ChaliAbiInvalidArgument, "missing registry"); \
        Memory::AllocationScope allocation(target->backend->allocationOwner()); \
        chali_glue::Registry registry(target); \
        Memory::String text; \
        auto *error = &text; \
        const bool ok = (expression); \
        return chali_abi_status(failure, ok ? ChaliAbiOk : ChaliAbiHostFailure, ok ? nullptr : text.empty() ? "standard module registration failed" : text.c_str()); \
    } \
    catch (const std::exception &error) { return chali_abi_status(failure, ChaliAbiHostFailure, error.what()); } \
    catch (...) { return chali_abi_status(failure, ChaliAbiHostFailure, "standard module registration failed"); \
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
        int32_t (*bind)(ChaliAbiRegistry *, ChaliAbiError *);
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
        char message[1024];
        ChaliAbiError error{message, sizeof(message), 0, ChaliAbiOk};
        if (module.bind(registry.cApi(), &error) != ChaliAbiOk)
        {
            if (diagnostics) diagnostics->push_back(Memory::String("AOT 标准库模块绑定失败：") + message);
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
