/**
 * @file runtime_i18n.hpp
 * @brief 在编译期选择运行时实际使用的消息，避免链接整张工具链翻译表。
 */
#pragma once
#include "../chali_i18n.hpp"

namespace stackvm::runtimeDetail
{
/**
 * @brief 在编译期查找一个字面量的英文消息。
 * @param original 中文消息字面量。
 * @return 目录中的英文文本，缺失时保留原文。
 */
constexpr const char *runtimeEnglish(const char *original)
{
    for (const auto &message : chali_i18n::messages)
    {
        const char *left = original;
        const char *right = message.zh;
        while (*left && *left == *right)
        {
            ++left;
            ++right;
        }
        if (*left == *right) return message.en;
    }
    return original;
}
}
/** @brief 仅用于运行时消息字面量，语言选择仍遵循进程配置。 */
#define CHALI_RUNTIME_TEXT(original) ([]() { \
    constexpr const char *english = stackvm::runtimeDetail::runtimeEnglish(original); \
    return chali_i18n::language() == chali_i18n::Language::Zh ? original : english; \
}())
