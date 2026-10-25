#include "allocator/chali_allocator.hpp"
#ifndef CHALI_FN_SUGAR_HPP
#define CHALI_FN_SUGAR_HPP

#include "preprocess.hpp"

namespace chali_fn_sugar
{

/**
 * @brief 将 fn 前缀与返回类型后置的写法改写成等价的前置写法。
 * @param source 原地规范化的源码，保留行数以映射诊断。
 * @return 无。
 * @note 归一化四类写法：`fn main()` 补 void、`fn calc(int64) -> int64` 前置返回类型、
 *       `fn(int64 a, int64 b) -> int64` 转成既有的 `int64 fn(int64 a, int64 b)`，
 *       以及返回闭包类型的前后置混写与嵌套。必须在泛型单态化之前执行，
 *       否则泛型声明会被当作旧语法摘走。
 */
void normalize(Memory::String &source);

}  // namespace chali_fn_sugar

#endif
