/**
 * @file aggregate_optimize.hpp
 * @brief 对具有明确新目标存储的聚合调用生成内部目标地址变体。
 */
#pragma once
#include "ast.hpp"

namespace ast
{
/**
 * @brief 优化独立新聚合初始化；保留原函数用于导出、函数值及不适用调用。
 * @param input 已完成类型、借用和清理降低的程序。
 * @return 节点独立的优化副本；CHALI_NO_AGGREGATE_OPT 非空时返回原程序。
 */
Program optimizeAggregateDestinations(const Program &input);
}  // namespace ast
