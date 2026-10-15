#ifndef PARSE_ENGINE_STD_MATH_HPP
#define PARSE_ENGINE_STD_MATH_HPP

#include "../magel_glue.hpp"

#include <chrono>
#include <cmath>
#include <cstdint>
#include <string>
#include <limits>

namespace std_math
{

/**
 * 单调时钟读数（纳秒）。`steady_clock` 不受系统时间调整影响。
 */
inline std::int64_t steadyClockNanos()
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               std::chrono::steady_clock::now().time_since_epoch())
        .count();
}

/**
 * 绑定 `std::math` 与 `std::lang::Clock` 的桌面默认实现。它们是宿主胶水，不是 StackVM
 * 指令集的一部分。嵌入式宿主可不引入本文件，改为绑定自己的数学库与时钟。
 */
inline bool bind(magel_glue::Registry &registry, Memory::String *error = nullptr)
{
    // 保留 MAGEL 重载的整数回绕、NaN 比较和负零语义。
    return registry.bindTyped<std::int64_t, std::int64_t>(
               "std::math::__absInt64",
               [](std::int64_t value) { return value == std::numeric_limits<std::int64_t>::min() ? value : (value < 0 ? -value : value); }, error)
           && registry.bindTyped<std::int64_t, std::int64_t, std::int64_t>(
               "std::math::__minInt64",
               [](std::int64_t left, std::int64_t right) { return left < right ? left : right; }, error)
           && registry.bindTyped<std::int64_t, std::int64_t, std::int64_t>(
               "std::math::__maxInt64",
               [](std::int64_t left, std::int64_t right) { return left > right ? left : right; }, error)
           && registry.bindTypedWithContext<std::int64_t, std::int64_t, std::int64_t, std::int64_t>(
               "std::math::__clampInt64",
               [](magel_glue::Context &context, std::int64_t value, std::int64_t low, std::int64_t high) { if (!(low <= high)) { context.fail("std::math::clamp 的 low 不能大于 high"); return value; }
                   return value < low ? low : (value > high ? high : value); }, error)
           && registry.bindTyped<double, double>(
               "std::math::__absFloat64",
               [](double value) { return value < 0.0 ? 0.0 - value : value; }, error)
           && registry.bindTyped<double, double, double>(
               "std::math::__minFloat64",
               [](double left, double right) { return left < right ? left : right; }, error)
           && registry.bindTyped<double, double, double>(
               "std::math::__maxFloat64",
               [](double left, double right) { return left > right ? left : right; }, error)
           && registry.bindTypedWithContext<double, double, double, double>(
               "std::math::__clampFloat64",
               [](magel_glue::Context &context, double value, double low, double high) { if (!(low <= high)) { context.fail("std::math::clamp 的 low 不能大于 high"); return value; }
                   return value < low ? low : (value > high ? high : value); }, error)
           && registry.bindTyped<double, double>("std::math::sin", [](double x) { return std::sin(x); }, error) && registry.bindTyped<double, double>("std::math::cos", [](double x) { return std::cos(x); }, error) && registry.bindTyped<double, double>("std::math::tan", [](double x) { return std::tan(x); }, error) && registry.bindTyped<double, double>("std::math::asin", [](double x) { return std::asin(x); }, error) && registry.bindTyped<double, double>("std::math::acos", [](double x) { return std::acos(x); }, error) && registry.bindTyped<double, double>("std::math::atan", [](double x) { return std::atan(x); }, error) && registry.bindTyped<double, double>("std::math::exp", [](double x) { return std::exp(x); }, error) && registry.bindTyped<double, double>("std::math::ln", [](double x) { return std::log(x); }, error) && registry.bindTyped<double, double>("std::math::log10", [](double x) { return std::log10(x); }, error) && registry.bindTyped<double, double, double>("std::math::pow", [](double x, double y) { return std::pow(x, y); }, error) && registry.bindTyped<double, double>("std::math::sqrt", [](double x) { return std::sqrt(x); }, error) && registry.bindTyped<double, double>("std::math::abs", [](double x) { return std::fabs(x); }, error) && registry.bindTyped<double, double>("std::math::floor", [](double x) { return std::floor(x); }, error) && registry.bindTyped<double, double>("std::math::ceil", [](double x) { return std::ceil(x); }, error) && registry.bindTyped<double, double>("std::math::round", [](double x) { return std::round(x); }, error) && registry.bindTyped<double, double>("std::math::trunc", [](double x) { return std::trunc(x); }, error) && registry.bindTyped<double, double, double>("std::math::fmod", [](double x, double y) { return std::fmod(x, y); }, error) && registry.bindTyped<bool, double>("std::math::isNaN", [](double x) { return std::isnan(x) != 0; }, error) && registry.bindTyped<bool, double>("std::math::isInf", [](double x) { return std::isinf(x) != 0; }, error) && registry.bindTyped<std::int64_t>("std::clock::nowNanos", []() { return steadyClockNanos(); }, error) && registry.bindTyped<std::int64_t>("std::clock::nowMs", []() { return steadyClockNanos() / 1000000; }, error);
}

}  // namespace std_math

#endif  // PARSE_ENGINE_STD_MATH_HPP
