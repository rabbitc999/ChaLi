/**
 * @file std_aot_modules.cpp
 * @brief 每个标准库模块的独立 C 入口与仅编译器使用的选择表。
 */
#include "std_aot_modules.hpp"
#include "abi/chali_standard_internal.h"
#include <cstdlib>
#include <memory>

// 模块绑定实现位于各自 ABI 编译单元，编译器这里只引用声明。
#define CHALI_AOT_MODULE(name, ...) \
    extern "C" int32_t chali_std_bind_##name(ChaliAbiRegistry *, ChaliAbiError *);
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
Memory::Vector<Memory::String> buildModulesFor(const Memory::Vector<Memory::String> &names, Memory::Vector<Memory::String> *diagnostics)
{
    /** @brief 选择表中的独立模块绑定入口。 */
    struct Module
    {
        const char *name;
        int32_t (*bind)(ChaliAbiRegistry *, ChaliAbiError *);
    };
    static const Module modules[] = {
#define CHALI_AOT_MODULE(name, ...) { #name, &chali_std_bind_##name },
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
        char message[1024];
        ChaliAbiError error{ message, sizeof(message), 0, ChaliAbiOk };
        std::unique_ptr<ChaliAbiRegistry, decltype(&chali_abi_registry_destroy)> registry(
            chali_abi_registry_create(&error), &chali_abi_registry_destroy);
        if (!registry || module.bind(registry.get(), &error) != ChaliAbiOk)
        {
            if (diagnostics) diagnostics->push_back(Memory::String("AOT 标准库模块绑定失败：") + message);
            continue;
        }
        for (const auto &name : names)
        {
            ChaliAbiEntry entry = nullptr;
            if (chali_abi_registry_resolve(registry.get(), name.c_str(), &entry, nullptr, nullptr) == ChaliAbiOk)
            {
                required.push_back(module.name);
                break;
            }
        }
    }
    return required;
}
}  // namespace std_registry
