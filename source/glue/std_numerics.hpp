#ifndef PARSE_ENGINE_STD_NUMERICS_HPP
#define PARSE_ENGINE_STD_NUMERICS_HPP

#include "../magel_glue.hpp"
#include <cmath>

namespace std_numerics_glue
{
using Context = magel_glue::Context;
using Fn = magel_glue::Function<double(double)>;
using Fn2 = magel_glue::Function<double(double, double)>;

inline double derivative(const Fn &function, double x, double step)
{
    return (function(x + step) - function(x - step)) / (2.0 * step);
}

inline double integrateSimpson(const Fn &function, double begin, double end, int intervals)
{
    const double step = (end - begin) / static_cast<double>(intervals);
    double total = function(begin) + function(end);
    for (int i = 1; i < intervals; ++i)
        total += (i % 2 == 0 ? 2.0 : 4.0) * function(begin + step * static_cast<double>(i));
    return total * step / 3.0;
}

inline double bisect(const Fn &function, double left, double right, double tolerance, int maxIterations)
{
    double low = function(left), high = function(right);
    if (low == 0.0) return left;
    if (high == 0.0) return right;
    for (int i = 0; i < maxIterations; ++i)
    {
        const double middle = left + (right - left) / 2.0;
        const double value = function(middle);
        if (std::abs(value) <= tolerance || (right - left) / 2.0 <= tolerance) return middle;
        if ((value < 0.0 && low < 0.0) || (value > 0.0 && low > 0.0))
        {
            left = middle;
            low = value;
        }
        else
        {
            right = middle;
            high = value;
        }
    }
    throw std::runtime_error("bisect 在迭代上限内未收敛");
}

inline double rk4Step(const Fn2 &derivative, double time, double value, double step)
{
    const double k1 = derivative(time, value);
    const double k2 = derivative(time + step / 2.0, value + step * k1 / 2.0);
    const double k3 = derivative(time + step / 2.0, value + step * k2 / 2.0);
    const double k4 = derivative(time + step, value + step * k3);
    return value + step * (k1 + 2.0 * k2 + 2.0 * k3 + k4) / 6.0;
}

inline double newton(const Fn &function, double initial, double tolerance, int maxIterations)
{
    double current = initial;
    for (int i = 0; i < maxIterations; ++i)
    {
        const double value = function(current);
        if (std::abs(value) <= tolerance) return current;
        double scale = std::abs(current);
        if (scale < 1.0) scale = 1.0;
        const double slope = derivative(function, current, 0.000001 * scale);
        if (!(std::abs(slope) > 0.000000000001)) throw std::runtime_error("newton 导数太接近零");
        current = current - value / slope;
    }
    throw std::runtime_error("newton 在迭代上限内未收敛");
}

inline bool bind(magel_glue::Registry &registry, Memory::String *error = nullptr)
{
    return registry.bindTypedWithContext<double, Fn, double, double>(
               "std::numerics::numericsDerivative",
               [](Context &ctx, const Fn &function, double x, double step) {
                   if (!(step > 0.0))
                   {
                       ctx.fail("derivative 步长必须为正");
                       return 0.0;
                   }
                   return derivative(function, x, step);
               },
               error)
           && registry.bindTypedWithContext<double, Fn, double, double, std::int64_t>(
               "std::numerics::numericsIntegrateSimpson",
               [](Context &ctx, const Fn &function, double begin, double end, std::int64_t intervals) {
                   if (!(intervals > 0 && intervals % 2 == 0))
                   {
                       ctx.fail("Simpson 分段数必须为正偶数");
                       return 0.0;
                   }
                   return integrateSimpson(function, begin, end, static_cast<int>(intervals));
               },
               error)
           && registry.bindTypedWithContext<double, Fn, double, double, double, std::int64_t>(
               "std::numerics::numericsBisect",
               [](Context &ctx, const Fn &function, double left, double right, double tolerance, std::int64_t maxIterations) {
                   if (!(left < right && tolerance > 0.0 && maxIterations > 0))
                   {
                       ctx.fail("bisect 区间、精度或迭代次数不合法");
                       return 0.0;
                   }
                   const double low = function(left), high = function(right);
                   if (low == 0.0) return left;
                   if (high == 0.0) return right;
                   if (!((low < 0.0 && high > 0.0) || (low > 0.0 && high < 0.0)))
                   {
                       ctx.fail("bisect 端点必须异号");
                       return 0.0;
                   }
                   return bisect(function, left, right, tolerance, static_cast<int>(maxIterations));
               },
               error)
           && registry.bindTypedWithContext<double, Fn2, double, double, double>(
               "std::numerics::numericsRk4Step",
               [](Context &ctx, const Fn2 &derivative, double time, double value, double step) {
                   if (!(step > 0.0))
                   {
                       ctx.fail("rk4Step 步长必须为正");
                       return 0.0;
                   }
                   return rk4Step(derivative, time, value, step);
               },
               error)
           && registry.bindTypedWithContext<double, Fn, double, double, std::int64_t>(
               "std::numerics::numericsNewton",
               [](Context &ctx, const Fn &function, double initial, double tolerance, std::int64_t maxIterations) {
                   if (!(tolerance > 0.0 && maxIterations > 0))
                   {
                       ctx.fail("newton 精度和迭代次数必须为正");
                       return 0.0;
                   }
                   try
                   {
                       return newton(function, initial, tolerance, static_cast<int>(maxIterations));
                   } catch (const std::runtime_error &e)
                   {
                       ctx.fail(e.what());
                       return 0.0;
                   }
               },
               error)
           && registry.bindTypedWithContext<Memory::Vector<double>, Fn2, double, double, double, std::int64_t>(
               "std::numerics::numericsSolveOdeEuler",
               [](Context &ctx, const Fn2 &rate, double initialTime, double initialValue, double step, std::int64_t steps) {
                   if (!(step > 0.0 && steps >= 0))
                   {
                       ctx.fail("Euler 步长或步数不合法");
                       return Memory::Vector<double>{};
                   }
                   Memory::Vector<double> values;
                   values.push_back(initialValue);
                   double time = initialTime, value = initialValue;
                   for (std::int64_t i = 0; i < steps; ++i)
                   {
                       value += step * rate(time, value);
                       time += step;
                       values.push_back(value);
                   }
                   return values;
               },
               error)
           && registry.bindTypedWithContext<Memory::Vector<double>, Fn2, double, double, double, std::int64_t>(
               "std::numerics::numericsSolveOdeRK4",
               [](Context &ctx, const Fn2 &rate, double initialTime, double initialValue, double step, std::int64_t steps) {
                   if (!(step > 0.0 && steps >= 0))
                   {
                       ctx.fail("RK4 步长或步数不合法");
                       return Memory::Vector<double>{};
                   }
                   Memory::Vector<double> values;
                   values.push_back(initialValue);
                   double time = initialTime, value = initialValue;
                   for (std::int64_t i = 0; i < steps; ++i)
                   {
                       value = rk4Step(rate, time, value, step);
                       time += step;
                       values.push_back(value);
                   }
                   return values;
               },
               error);
}
}  // namespace std_numerics_glue
#endif
