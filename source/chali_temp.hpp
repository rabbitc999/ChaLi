/**
 * @file chali_temp.hpp
 * @brief 编译器内部临时文件夹统一定位到项目 tmp。
 */
#pragma once
#include <filesystem>

namespace chali_temp
{
/**
 * @brief 从当前目录和编译器所在目录查找项目，只在其 tmp 内创建临时子目录。
 * @param error 接收目录定位、创建或路径越界错误。
 * @return 项目 tmp 的规范绝对路径；失败返回空路径。
 * @note 独立安装且不处于源码仓库时使用编译器所在目录的 tmp，不使用上级或系统临时目录。
 */
std::filesystem::path root(std::error_code &error);
/**
 * @brief 取得项目临时目录，目录不可用时抛出文件系统异常。
 * @return 已创建的项目 tmp 规范绝对路径。
 */
std::filesystem::path root();
}  // namespace chali_temp
