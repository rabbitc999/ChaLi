/**
 * @file compiler_entry.cpp
 * @brief 独立编译器工程的命令入口，复用 C 头导入并转发普通编译。
 */
#include "chali_cli.hpp"
#include "chali_i18n.hpp"
#include "tools/abi_import.hpp"
#include <cstdio>

/**
 * @brief 进入已生成的编译器前端。
 * @param argc 已归一化的参数数量。
 * @param argv 已归一化的参数数组。
 * @return 前端命令的退出状态。
 */
int chali_main_entry(int argc, char **argv);

/**
 * @brief 派发独立工程的 C 头导入或普通前端命令。
 * @param argc 原始参数数量。
 * @param argv 原始参数数组。
 * @return 被调用命令的退出状态；语言选项无效时为二。
 */
int main(int argc, char **argv)
{
    auto original = chali_i18n::processArguments(argc, argv);
    std::vector<char *> values;
    for (auto &argument : original) values.push_back(argument.data());
    auto arguments = chali_cli::normalize(static_cast<int>(values.size()), values.data());
    std::string error;
    if (!chali_i18n::configure(arguments, error))
    {
        std::fprintf(stderr, "chali: %s\n", error.c_str());
        return 2;
    }
    values.clear();
    for (auto &argument : arguments) values.push_back(argument.data());
    values.push_back(nullptr);
    if (arguments.size() > 2 && arguments[1] == "gen" && arguments[2] == "abi")
        return chali_abi_import::run(static_cast<int>(arguments.size()), values.data());
    return chali_main_entry(static_cast<int>(arguments.size()), values.data());
}
