#ifndef PARSE_ENGINE_STD_NUMTHEORY_HPP
#define PARSE_ENGINE_STD_NUMTHEORY_HPP

#include "../magel_glue.hpp"
#include <cstdint>

namespace std_numtheory_glue
{

inline std::uint64_t gcd(std::uint64_t left, std::uint64_t right)
{
    while (right != 0)
    {
        const std::uint64_t next = left % right;
        left = right;
        right = next;
    }
    return left;
}

inline std::uint64_t addMod(std::uint64_t left, std::uint64_t right, std::uint64_t modulus)
{
    left %= modulus;
    right %= modulus;
    return (left >= modulus - right) ? left - (modulus - right) : left + right;
}

inline std::uint64_t mulMod(std::uint64_t left, std::uint64_t right, std::uint64_t modulus)
{
    std::uint64_t result = 0;
    left %= modulus;
    while (right != 0)
    {
        if ((right & 1u) != 0) result = addMod(result, left, modulus);
        right >>= 1;
        if (right != 0) left = addMod(left, left, modulus);
    }
    return result;
}

inline std::uint64_t powMod(std::uint64_t base, std::uint64_t exponent, std::uint64_t modulus)
{
    std::uint64_t result = 1u % modulus;
    base %= modulus;
    while (exponent != 0)
    {
        if ((exponent & 1u) != 0) result = mulMod(result, base, modulus);
        exponent >>= 1;
        if (exponent != 0) base = mulMod(base, base, modulus);
    }
    return result;
}

inline bool isPrime(std::uint64_t value)
{
    if (value < 2) return false;
    if (value == 2) return true;
    if ((value & 1u) == 0) return false;
    std::uint64_t odd = value - 1;
    int powers = 0;
    while ((odd & 1u) == 0)
    {
        odd >>= 1;
        ++powers;
    }
    /* 这七个底数对完整 uint64 范围构成确定性 Miller-Rabin 检验。 */
    static const std::uint64_t bases[] = { 2, 325, 9375, 28178, 450775, 9780504, 1795265022 };
    for (const std::uint64_t base0 : bases)
    {
        const std::uint64_t base = base0 % value;
        if (base == 0) continue;
        std::uint64_t residue = powMod(base, odd, value);
        if (residue != 1 && residue != value - 1)
        {
            bool reachedMinusOne = false;
            for (int j = 1; j < powers; ++j)
            {
                residue = mulMod(residue, residue, value);
                if (residue == value - 1)
                {
                    reachedMinusOne = true;
                    break;
                }
            }
            if (!reachedMinusOne) return false;
        }
    }
    return true;
}

inline bool bind(magel_glue::Registry &registry, Memory::String *error = nullptr)
{
    return registry.bindTyped<std::uint64_t, std::uint64_t, std::uint64_t>(
               "std::numtheory::numtheoryGcd",
               [](std::uint64_t left, std::uint64_t right) { return gcd(left, right); }, error)
           && registry.bindTypedWithContext<std::uint64_t, std::uint64_t, std::uint64_t>(
               "std::numtheory::numtheoryLcm",
               [](magel_glue::Context &ctx, std::uint64_t left, std::uint64_t right) {
                   if (left == 0 || right == 0) return std::uint64_t{ 0 };
                   const std::uint64_t quotient = left / gcd(left, right);
                   const std::uint64_t product = quotient * right;
                   if (product / right != quotient)
                   {
                       ctx.fail("lcm 超出 uint64 范围");
                       return std::uint64_t{ 0 };
                   }
                   return product;
               },
               error)
           && registry.bindTypedWithContext<std::uint64_t, std::uint64_t, std::uint64_t, std::uint64_t>(
               "std::numtheory::numtheoryAddMod",
               [](magel_glue::Context &ctx, std::uint64_t left, std::uint64_t right, std::uint64_t modulus) {
                   if (!(modulus > 0))
                   {
                       ctx.fail("模数必须为正");
                       return std::uint64_t{ 0 };
                   }
                   return addMod(left, right, modulus);
               },
               error)
           && registry.bindTypedWithContext<std::uint64_t, std::uint64_t, std::uint64_t, std::uint64_t>(
               "std::numtheory::numtheoryMulMod",
               [](magel_glue::Context &ctx, std::uint64_t left, std::uint64_t right, std::uint64_t modulus) {
                   if (!(modulus > 0))
                   {
                       ctx.fail("模数必须为正");
                       return std::uint64_t{ 0 };
                   }
                   return mulMod(left, right, modulus);
               },
               error)
           && registry.bindTypedWithContext<std::uint64_t, std::uint64_t, std::uint64_t, std::uint64_t>(
               "std::numtheory::numtheoryPowMod",
               [](magel_glue::Context &ctx, std::uint64_t base, std::uint64_t exponent, std::uint64_t modulus) {
                   if (!(modulus > 0))
                   {
                       ctx.fail("模数必须为正");
                       return std::uint64_t{ 0 };
                   }
                   return powMod(base, exponent, modulus);
               },
               error)
           && registry.bindTyped<bool, std::uint64_t>(
               "std::numtheory::numtheoryIsPrime",
               [](std::uint64_t value) { return isPrime(value); }, error)
           && registry.bindTypedWithContext<std::int64_t, std::int64_t, std::int64_t>(
               "std::numtheory::numtheoryModInverse",
               [](magel_glue::Context &ctx, std::int64_t value, std::int64_t modulus) {
                   if (!(modulus > 1))
                   {
                       ctx.fail("modInverse 模数必须大于 1");
                       return std::int64_t{ 0 };
                   }
                   std::int64_t oldR = modulus, r = value % modulus;
                   if (r < 0) r += modulus;
                   std::int64_t oldT = 0, t = 1;
                   while (r != 0)
                   {
                       const std::int64_t quotient = oldR / r;
                       const std::int64_t nextR = oldR - quotient * r;
                       const std::int64_t nextT = oldT - quotient * t;
                       oldR = r;
                       r = nextR;
                       oldT = t;
                       t = nextT;
                   }
                   if (!(oldR == 1))
                   {
                       ctx.fail("modInverse 仅适用于互素整数");
                       return std::int64_t{ 0 };
                   }
                   if (oldT < 0) oldT += modulus;
                   return oldT;
               },
               error);
}
}  // namespace std_numtheory_glue
#endif
