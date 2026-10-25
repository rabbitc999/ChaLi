#pragma once
#include "allocator/chali_allocator.hpp"

/**
 * @brief 显式生命周期契约：`#borrow(out, [in...])` 与 `#static(a)`。
 *
 * 语法侧只负责把指令原样挂到函数声明上，全部判定留在语义层。
 * 名字一律记为名称池编号，`#ret` 用专用的 `#ret` 文本，与标识符不会撞。
 */
namespace chali_lifetime
{
/** @brief `#ret` 在名称池里的文本；标识符不能含 `#`，所以不会重名。 */
inline const char *RetName() { return "#ret"; }
/** @brief 函数声明节点编号 → 指令列表节点编号。跨翻译单元共享。 */
extern Memory::Map<int, int> g_directiveNodes;
/** @brief 当前函数体里声明过的局部变量名；函数体的作用域在核对时已出栈，靠这里补。 */
extern Memory::Set<Memory::String> g_localNames;
}  // namespace chali_lifetime
