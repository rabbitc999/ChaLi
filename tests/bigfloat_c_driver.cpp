/** @file bigfloat_c_driver.cpp @brief 大浮点 C 迁移的逐行差分驱动。 */
#include "bigfloat.hpp"
#include "bigfloat_math.hpp"
#include <iostream>
#include <stdexcept>
#include <string>
/**
 * @brief 执行单个无副作用数值请求，输出值和规范化内部表示。
 * @param operation 操作名。
 * @param digits 有效位数或整数根次/幂指数。
 * @param first 左输入文本。
 * @param second 右输入文本。
 */
static void evaluate(const std::string &operation, int digits, const std::string &first, const std::string &second)
{
    bigfloat::BigFloat a{ Memory::String(first.c_str()) }, b{ Memory::String(second.c_str()) }, result;
    if (operation == "parse")
    {
        bool valid;
        result = bigfloat::BigFloat::fromString(Memory::String(first.c_str()), &valid);
        std::cout << valid << ':';
    }
    else if (operation == "round") result = a.round(digits);
    else if (operation == "add") result = a + b;
    else if (operation == "sub") result = a - b;
    else if (operation == "mul") result = a * b;
    else if (operation == "div") result = a.divide(b, digits);
    else if (operation == "sqrt") result = a.sqrt(digits);
    else if (operation == "root") result = a.root(std::stoull(second), digits);
    else if (operation == "pow") result = a.pow(std::stoull(second));
    else if (operation == "abs") result = a.abs();
    else if (operation == "neg") result = -a;
    else if (operation == "double") result = bigfloat::BigFloat::fromDouble(std::stod(first), digits);
    else if (operation == "compare") { std::cout << a.compare(b) << '\n'; return; }
    else if (operation == "integer") { std::cout << a.isInteger() << '\n'; return; }
    else if (operation == "trunc") { std::cout << a.trunc().toString() << '\n'; return; }
    else if (operation == "floor") { std::cout << a.floor().toString() << '\n'; return; }
    else if (operation == "ceil") { std::cout << a.ceil().toString() << '\n'; return; }
    else if (operation == "format") { std::cout << a.toString(digits) << '|' << a.toPlainString(digits) << '\n'; return; }
    else throw std::invalid_argument("unknown operation");
    std::cout << result.toString(0) << '|' << result.mantissa().toString() << '|' << result.exponent10() << '\n';
}
/**
 * @brief 从标准输入逐行读取操作并保留原有异常种类与诊断。
 * @return 协议读写正常时返回零。
 */
int main()
{
    std::string operation, first, second;
    int digits;
    while (std::cin >> operation >> digits >> first >> second)
    {
        try { evaluate(operation, digits, first, second); }
        catch (const std::out_of_range &error) { std::cout << "range:" << error.what() << '\n'; }
        catch (const std::overflow_error &error) { std::cout << "overflow:" << error.what() << '\n'; }
        catch (const std::domain_error &error) { std::cout << "domain:" << error.what() << '\n'; }
        catch (const std::invalid_argument &error) { std::cout << "invalid:" << error.what() << '\n'; }
        catch (const std::exception &error) { std::cout << "error:" << error.what() << '\n'; }
    }
}
