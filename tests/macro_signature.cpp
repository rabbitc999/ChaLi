#include "macro/signature.hpp"
#include <iostream>
/**
 * @brief 检查共享宏签名解析器保持旧头部并识别显式结果类别。
 * @return 所有签名及错误类别验证通过时返回零。
 */
int main()
{
    chali_macro::Signature signature;
    Memory::String error;
    for (const auto *category : { "expr", "stmt", "item" })
    {
        const Memory::String source = Memory::String("$macro Make<T>(name:ident) -> ") + category;
        if (!chali_macro::readSignature(source, signature, error) || signature.result != category || signature.name != "Make") return 1;
    }
    if (!chali_macro::readSignature("$macro Old<T:type>(type)", signature, error) || !signature.result.empty()) return 2;
    if (chali_macro::readSignature("$macro Bad() -> unknown", signature, error) || error.empty()) return 3;
    if (!chali_macro::readSignature("$replace Make(name:ident) -> item int64 name(){return 42;}", signature, error) || signature.result != "item") return 4;
    if (!chali_macro::readSignature("$macro Values(...xs:value)", signature, error) || signature.groups.size() != 1 || signature.groups[0].rest != "xs" || signature.groups[0].restCategory != "value") return 5;
    std::cout << "macro result signatures passed\n";
    return 0;
}
