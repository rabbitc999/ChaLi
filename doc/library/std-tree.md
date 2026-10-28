# std::tree — 树与堆

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](std-tree.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

[返回库目录](README.md) · [全部符号](symbols.md)

## 引入

```chali
import std::tree;
```

## 公共约定与错误边界

BinarySearchTree 是集合语义的二叉搜索树，重复插入返回 false，中序即升序；删除双孩子节点取中序后继替换，被删节点槽回收复用。RedBlackTree 使用 ChaLi 持久红黑节点，insert/remove 原地旋转及变色修复；重复插入或删除不存在的键返回 false，validate 校验排序、父指针、根颜色、红节点约束及黑高，删除节点槽回收复用。MinHeap 是最小二叉堆，可当优先队列用，相等键的出堆顺序不保证插入序。GeneralTree 用"第一个孩子 / 下一个兄弟"存储，toBinary 与 fromBinary 按"左孩子 = 第一个孩子、右孩子 = 下一个兄弟"互转，转换本身不排序、不保证 BST 有序性。树高按节点数计（单节点 1，空树 0）；树与堆由 ChaLi 结构体独立拥有节点数组，普通复制深复制；snapshot 显式导出独立节点或堆数组，遍历只导出结果。树与堆算法均实现在 std::tree 语言源码中，不注册宿主树算法。

## 参考代码

聚合入口的具体操作参见对应子模块；本页类型/方法的调用形式列在下方。

## 类型、枚举、常量与字段

公开字段按源码原样列出；句柄和游标字段用于包装器状态，通常通过方法操作。枚举的括号表示变体载荷。

| 所属类型 | 声明 | 说明 |
| --- | --- | --- |
| 模块 | <a id="symbol-470"></a>`struct TreeSnapshot` | 独立节点快照；空孩子和空根使用 -1，节点槽号保持稳定。 |
| TreeSnapshot | <a id="symbol-471"></a>`List<int64> keys` | 节点槽键值。 |
| TreeSnapshot | <a id="symbol-472"></a>`List<int64> lefts` | 左孩子下标，空孩子为 -1。 |
| TreeSnapshot | <a id="symbol-473"></a>`List<int64> rights` | 右孩子下标，空孩子为 -1。 |
| TreeSnapshot | <a id="symbol-474"></a>`List<int64> live` | 有效槽标记，1 为有效。 |
| TreeSnapshot | <a id="symbol-475"></a>`int64 root` | 根节点下标，空树为 -1。 |
| TreeSnapshot | <a id="symbol-476"></a>`int64 count` | 有效节点数。 |
| 模块 | <a id="symbol-477"></a>`struct GeneralTreeSnapshot` | 首孩子、次兄弟表示的独立通用树快照。 |
| GeneralTreeSnapshot | <a id="symbol-478"></a>`List<int64> values` | 节点槽值。 |
| GeneralTreeSnapshot | <a id="symbol-479"></a>`List<int64> firstChild` | 首孩子下标，空孩子为 -1。 |
| GeneralTreeSnapshot | <a id="symbol-480"></a>`List<int64> nextSibling` | 次兄弟下标，空兄弟为 -1。 |
| GeneralTreeSnapshot | <a id="symbol-481"></a>`List<int64> live` | 有效槽标记，1 为有效。 |
| GeneralTreeSnapshot | <a id="symbol-482"></a>`int64 root` | 根节点号，空树为 -1。 |
| GeneralTreeSnapshot | <a id="symbol-483"></a>`int64 count` | 有效节点数。 |
| 模块 | <a id="symbol-484"></a>`struct BinarySearchTree: Copy<BinarySearchTree>` | 宿主持久二叉搜索集合，普通复制深复制，snapshot 显式导出节点。 |
| 模块 | <a id="symbol-485"></a>`struct RedBlackTree: Copy<RedBlackTree>` | 宿主持久红黑集合，插入与删除原地修复，普通复制深复制。 |
| 模块 | <a id="symbol-486"></a>`struct MinHeap: Copy<MinHeap>` | 宿主持久最小堆，普通复制深复制，snapshot 显式导出堆数组。 |
| 模块 | <a id="symbol-487"></a>`struct GeneralTree: Copy<GeneralTree>` | 宿主持久首孩子、次兄弟树，支持独立二叉表示转换。 |

## 函数索引

| 名称 | 声明 |
| --- | --- |
| [BinarySearchTree::create](#api-0) | `static BinarySearchTree create()` |
| [BinarySearchTree::copy](#api-1) | `BinarySearchTree copy()` |
| [BinarySearchTree::len](#api-2) | `int64 len()` |
| [BinarySearchTree::clear](#api-3) | `void clear()` |
| [BinarySearchTree::insert](#api-4) | `bool insert(int64 key)` |
| [BinarySearchTree::remove](#api-5) | `bool remove(int64 key)` |
| [BinarySearchTree::contains](#api-6) | `bool contains(int64 key)` |
| [BinarySearchTree::min](#api-7) | `Option<int64> min()` |
| [BinarySearchTree::max](#api-8) | `Option<int64> max()` |
| [BinarySearchTree::height](#api-9) | `int64 height()` |
| [BinarySearchTree::inorder](#api-10) | `List<int64> inorder()` |
| [BinarySearchTree::preorder](#api-11) | `List<int64> preorder()` |
| [BinarySearchTree::postorder](#api-12) | `List<int64> postorder()` |
| [BinarySearchTree::snapshot](#api-13) | `TreeSnapshot snapshot()` |
| [RedBlackTree::create](#api-14) | `static RedBlackTree create()` |
| [RedBlackTree::copy](#api-15) | `RedBlackTree copy()` |
| [RedBlackTree::len](#api-16) | `int64 len()` |
| [RedBlackTree::contains](#api-17) | `bool contains(int64 key)` |
| [RedBlackTree::clear](#api-18) | `void clear()` |
| [RedBlackTree::height](#api-19) | `int64 height()` |
| [RedBlackTree::inorder](#api-20) | `List<int64> inorder()` |
| [RedBlackTree::min](#api-21) | `Option<int64> min()` |
| [RedBlackTree::max](#api-22) | `Option<int64> max()` |
| [RedBlackTree::insert](#api-23) | `bool insert(int64 key)` |
| [RedBlackTree::remove](#api-24) | `bool remove(int64 key)` |
| [RedBlackTree::validate](#api-25) | `bool validate()` |
| [MinHeap::create](#api-26) | `static MinHeap create()` |
| [MinHeap::copy](#api-27) | `MinHeap copy()` |
| [MinHeap::len](#api-28) | `int64 len()` |
| [MinHeap::isEmpty](#api-29) | `bool isEmpty()` |
| [MinHeap::clear](#api-30) | `void clear()` |
| [MinHeap::push](#api-31) | `void push(int64 value)` |
| [MinHeap::peek](#api-32) | `Option<int64> peek()` |
| [MinHeap::pop](#api-33) | `Option<int64> pop()` |
| [MinHeap::snapshot](#api-34) | `List<int64> snapshot()` |
| [GeneralTree::create](#api-35) | `static GeneralTree create()` |
| [GeneralTree::copy](#api-36) | `GeneralTree copy()` |
| [GeneralTree::len](#api-37) | `int64 len()` |
| [GeneralTree::clear](#api-38) | `void clear()` |
| [GeneralTree::addRoot](#api-39) | `int64 addRoot(int64 value)` |
| [GeneralTree::addChild](#api-40) | `int64 addChild(int64 cursor,int64 value)` |
| [GeneralTree::valueAt](#api-41) | `int64 valueAt(int64 cursor)` |
| [GeneralTree::rootNode](#api-42) | `int64 rootNode()` |
| [GeneralTree::children](#api-43) | `List<int64> children(int64 cursor)` |
| [GeneralTree::preorder](#api-44) | `List<int64> preorder()` |
| [GeneralTree::levelOrder](#api-45) | `List<int64> levelOrder()` |
| [GeneralTree::height](#api-46) | `int64 height()` |
| [GeneralTree::toBinary](#api-47) | `BinarySearchTree toBinary()` |
| [GeneralTree::fromBinary](#api-48) | `static GeneralTree fromBinary(BinarySearchTree tree)` |
| [GeneralTree::snapshot](#api-49) | `GeneralTreeSnapshot snapshot()` |

<a id="api-0"></a>

### BinarySearchTree::create（1）

**声明**

```chali
static BinarySearchTree create();
```

**行为**

创建BinarySearchTree，返回可供后续操作的值。

创建空树。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`BinarySearchTree`。创建BinarySearchTree，返回可供后续操作的值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::tree::BinarySearchTree::create();
```

**源码**：[source/std/tree.chl:96](../../source/std/tree.chl#L96)。

<a id="api-1"></a>

### BinarySearchTree::copy（2）

**声明**

```chali
BinarySearchTree copy();
```

**行为**

返回独立的值副本。

取得独立副本。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`BinarySearchTree`。返回独立的值副本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.copy();
```

**源码**：[source/std/tree.chl:98](../../source/std/tree.chl#L98)。

<a id="api-2"></a>

### BinarySearchTree::len（3）

**声明**

```chali
int64 len();
```

**行为**

返回当前元素数量。

返回有效节点数。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回当前元素数量。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.len();
```

**源码**：[source/std/tree.chl:100](../../source/std/tree.chl#L100)。

<a id="api-3"></a>

### BinarySearchTree::clear（4）

**声明**

```chali
void clear();
```

**行为**

移除全部元素。

清空节点。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.clear();
```

**源码**：[source/std/tree.chl:102](../../source/std/tree.chl#L102)。

<a id="api-4"></a>

### BinarySearchTree::insert（5）

**声明**

```chali
bool insert(int64 key);
```

**行为**

在 index 之前插入 value。

插入唯一键，重复时返回 false。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `key` | `int64 key` | 查找键或下标 | 无 |

**返回值**

`bool`。在 index 之前插入 value。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.insert(key);
```

**源码**：[source/std/tree.chl:104](../../source/std/tree.chl#L104)。

<a id="api-5"></a>

### BinarySearchTree::remove（6）

**声明**

```chali
bool remove(int64 key);
```

**行为**

删除指定位置或键的元素。

摘除键；两个孩子时用中序后继替换。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `key` | `int64 key` | 查找键或下标 | 无 |

**返回值**

`bool`。删除指定位置或键的元素。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.remove(key);
```

**源码**：[source/std/tree.chl:111](../../source/std/tree.chl#L111)。

<a id="api-6"></a>

### BinarySearchTree::contains（7）

**声明**

```chali
bool contains(int64 key);
```

**行为**

判断是否包含目标值或匹配文本。

判断是否存在键。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `key` | `int64 key` | 查找键或下标 | 无 |

**返回值**

`bool`。判断是否包含目标值或匹配文本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.contains(key);
```

**源码**：[source/std/tree.chl:118](../../source/std/tree.chl#L118)。

<a id="api-7"></a>

### BinarySearchTree::min（8）

**声明**

```chali
Option<int64> min();
```

**行为**

返回实参中的较小值；泛型入口，接受任意数量实参，相等时取后者。

返回最小键，空树返回 None。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Option<int64>`。返回实参中的较小值；泛型入口，接受任意数量实参，相等时取后者。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.min();
```

**源码**：[source/std/tree.chl:120](../../source/std/tree.chl#L120)。

<a id="api-8"></a>

### BinarySearchTree::max（9）

**声明**

```chali
Option<int64> max();
```

**行为**

返回实参中的较大值；泛型入口，接受任意数量实参，相等时取后者。

返回最大键，空树返回 None。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Option<int64>`。返回实参中的较大值；泛型入口，接受任意数量实参，相等时取后者。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.max();
```

**源码**：[source/std/tree.chl:122](../../source/std/tree.chl#L122)。

<a id="api-9"></a>

### BinarySearchTree::height（10）

**声明**

```chali
int64 height();
```

**行为**

返回最大路径节点数。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回最大路径节点数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.height();
```

**源码**：[source/std/tree.chl:124](../../source/std/tree.chl#L124)。

<a id="api-10"></a>

### BinarySearchTree::inorder（11）

**声明**

```chali
List<int64> inorder();
```

**行为**

返回独立中序键列表。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`List<int64>`。返回独立中序键列表。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.inorder();
```

**源码**：[source/std/tree.chl:126](../../source/std/tree.chl#L126)。

<a id="api-11"></a>

### BinarySearchTree::preorder（12）

**声明**

```chali
List<int64> preorder();
```

**行为**

返回独立先序键列表。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`List<int64>`。返回独立先序键列表。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.preorder();
```

**源码**：[source/std/tree.chl:128](../../source/std/tree.chl#L128)。

<a id="api-12"></a>

### BinarySearchTree::postorder（13）

**声明**

```chali
List<int64> postorder();
```

**行为**

返回独立后序键列表。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`List<int64>`。返回独立后序键列表。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.postorder();
```

**源码**：[source/std/tree.chl:130](../../source/std/tree.chl#L130)。

<a id="api-13"></a>

### BinarySearchTree::snapshot（14）

**声明**

```chali
TreeSnapshot snapshot();
```

**行为**

返回独立节点快照。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`TreeSnapshot`。返回独立节点快照。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.snapshot();
```

**源码**：[source/std/tree.chl:132](../../source/std/tree.chl#L132)。

<a id="api-14"></a>

### RedBlackTree::create（15）

**声明**

```chali
static RedBlackTree create();
```

**行为**

创建RedBlackTree，返回可供后续操作的值。

创建空红黑树。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`RedBlackTree`。创建RedBlackTree，返回可供后续操作的值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::tree::RedBlackTree::create();
```

**源码**：[source/std/tree.chl:139](../../source/std/tree.chl#L139)。

<a id="api-15"></a>

### RedBlackTree::copy（16）

**声明**

```chali
RedBlackTree copy();
```

**行为**

返回独立的值副本。

取得独立副本。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`RedBlackTree`。返回独立的值副本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.copy();
```

**源码**：[source/std/tree.chl:141](../../source/std/tree.chl#L141)。

<a id="api-16"></a>

### RedBlackTree::len（17）

**声明**

```chali
int64 len();
```

**行为**

返回当前元素数量。

返回键数量。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回当前元素数量。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.len();
```

**源码**：[source/std/tree.chl:143](../../source/std/tree.chl#L143)。

<a id="api-17"></a>

### RedBlackTree::contains（18）

**声明**

```chali
bool contains(int64 key);
```

**行为**

判断是否包含目标值或匹配文本。

判断键是否存在。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `key` | `int64 key` | 查找键或下标 | 无 |

**返回值**

`bool`。判断是否包含目标值或匹配文本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.contains(key);
```

**源码**：[source/std/tree.chl:145](../../source/std/tree.chl#L145)。

<a id="api-18"></a>

### RedBlackTree::clear（19）

**声明**

```chali
void clear();
```

**行为**

移除全部元素。

清空所有节点。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.clear();
```

**源码**：[source/std/tree.chl:147](../../source/std/tree.chl#L147)。

<a id="api-19"></a>

### RedBlackTree::height（20）

**声明**

```chali
int64 height();
```

**行为**

返回树高。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回树高。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.height();
```

**源码**：[source/std/tree.chl:149](../../source/std/tree.chl#L149)。

<a id="api-20"></a>

### RedBlackTree::inorder（21）

**声明**

```chali
List<int64> inorder();
```

**行为**

返回独立升序键列表。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`List<int64>`。返回独立升序键列表。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.inorder();
```

**源码**：[source/std/tree.chl:151](../../source/std/tree.chl#L151)。

<a id="api-21"></a>

### RedBlackTree::min（22）

**声明**

```chali
Option<int64> min();
```

**行为**

返回实参中的较小值；泛型入口，接受任意数量实参，相等时取后者。

返回最小键，空树返回 None。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Option<int64>`。返回实参中的较小值；泛型入口，接受任意数量实参，相等时取后者。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.min();
```

**源码**：[source/std/tree.chl:153](../../source/std/tree.chl#L153)。

<a id="api-22"></a>

### RedBlackTree::max（23）

**声明**

```chali
Option<int64> max();
```

**行为**

返回实参中的较大值；泛型入口，接受任意数量实参，相等时取后者。

返回最大键，空树返回 None。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Option<int64>`。返回实参中的较大值；泛型入口，接受任意数量实参，相等时取后者。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.max();
```

**源码**：[source/std/tree.chl:155](../../source/std/tree.chl#L155)。

<a id="api-23"></a>

### RedBlackTree::insert（24）

**声明**

```chali
bool insert(int64 key);
```

**行为**

在 index 之前插入 value。

插入键并修复连续红节点。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `key` | `int64 key` | 查找键或下标 | 无 |

**返回值**

`bool`。在 index 之前插入 value。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.insert(key);
```

**源码**：[source/std/tree.chl:157](../../source/std/tree.chl#L157)。

<a id="api-24"></a>

### RedBlackTree::remove（25）

**声明**

```chali
bool remove(int64 key);
```

**行为**

删除指定位置或键的元素。

删除键，保留中序后继的颜色并修复黑高。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `key` | `int64 key` | 查找键或下标 | 无 |

**返回值**

`bool`。删除指定位置或键的元素。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.remove(key);
```

**源码**：[source/std/tree.chl:202](../../source/std/tree.chl#L202)。

<a id="api-25"></a>

### RedBlackTree::validate（26）

**声明**

```chali
bool validate();
```

**行为**

验证连通节点、父指针、严格顺序、根颜色、红孩子及所有 nil 路径黑高。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。验证连通节点、父指针、严格顺序、根颜色、红孩子及所有 nil 路径黑高。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.validate();
```

**源码**：[source/std/tree.chl:216](../../source/std/tree.chl#L216)。

<a id="api-26"></a>

### MinHeap::create（27）

**声明**

```chali
static MinHeap create();
```

**行为**

创建MinHeap，返回可供后续操作的值。

创建空堆。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`MinHeap`。创建MinHeap，返回可供后续操作的值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::tree::MinHeap::create();
```

**源码**：[source/std/tree.chl:241](../../source/std/tree.chl#L241)。

<a id="api-27"></a>

### MinHeap::copy（28）

**声明**

```chali
MinHeap copy();
```

**行为**

返回独立的值副本。

取得独立副本。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`MinHeap`。返回独立的值副本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.copy();
```

**源码**：[source/std/tree.chl:243](../../source/std/tree.chl#L243)。

<a id="api-28"></a>

### MinHeap::len（29）

**声明**

```chali
int64 len();
```

**行为**

返回当前元素数量。

返回元素数量。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回当前元素数量。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.len();
```

**源码**：[source/std/tree.chl:245](../../source/std/tree.chl#L245)。

<a id="api-29"></a>

### MinHeap::isEmpty（30）

**声明**

```chali
bool isEmpty();
```

**行为**

判断 Any 是否为空盒子，不公开内部执行期标签。

判断是否为空。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。判断 Any 是否为空盒子，不公开内部执行期标签。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isEmpty();
```

**源码**：[source/std/tree.chl:247](../../source/std/tree.chl#L247)。

<a id="api-30"></a>

### MinHeap::clear（31）

**声明**

```chali
void clear();
```

**行为**

移除全部元素。

清空元素。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.clear();
```

**源码**：[source/std/tree.chl:249](../../source/std/tree.chl#L249)。

<a id="api-31"></a>

### MinHeap::push（32）

**声明**

```chali
void push(int64 value);
```

**行为**

将 value 追加到列表尾部。

插入后向上调整。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `int64 value` | 输入值或写入的新值 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.push(value);
```

**源码**：[source/std/tree.chl:251](../../source/std/tree.chl#L251)。

<a id="api-32"></a>

### MinHeap::peek（33）

**声明**

```chali
Option<int64> peek();
```

**行为**

查看下一项并保留该项供后续消费。

返回最小值，空堆返回 None。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Option<int64>`。查看下一项并保留该项供后续消费。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.peek();
```

**源码**：[source/std/tree.chl:253](../../source/std/tree.chl#L253)。

<a id="api-33"></a>

### MinHeap::pop（34）

**声明**

```chali
Option<int64> pop();
```

**行为**

移除列表尾元素并返回值，空列表时报错。

弹出最小值并向下调整。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Option<int64>`。移除列表尾元素并返回值，空列表时报错。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.pop();
```

**源码**：[source/std/tree.chl:255](../../source/std/tree.chl#L255)。

<a id="api-34"></a>

### MinHeap::snapshot（35）

**声明**

```chali
List<int64> snapshot();
```

**行为**

导出独立堆数组。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`List<int64>`。导出独立堆数组。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.snapshot();
```

**源码**：[source/std/tree.chl:257](../../source/std/tree.chl#L257)。

<a id="api-35"></a>

### GeneralTree::create（36）

**声明**

```chali
static GeneralTree create();
```

**行为**

创建GeneralTree，返回可供后续操作的值。

创建空树。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`GeneralTree`。创建GeneralTree，返回可供后续操作的值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::tree::GeneralTree::create();
```

**源码**：[source/std/tree.chl:264](../../source/std/tree.chl#L264)。

<a id="api-36"></a>

### GeneralTree::copy（37）

**声明**

```chali
GeneralTree copy();
```

**行为**

返回独立的值副本。

返回独立副本。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`GeneralTree`。返回独立的值副本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.copy();
```

**源码**：[source/std/tree.chl:266](../../source/std/tree.chl#L266)。

<a id="api-37"></a>

### GeneralTree::len（38）

**声明**

```chali
int64 len();
```

**行为**

返回当前元素数量。

返回有效节点数。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回当前元素数量。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.len();
```

**源码**：[source/std/tree.chl:268](../../source/std/tree.chl#L268)。

<a id="api-38"></a>

### GeneralTree::clear（39）

**声明**

```chali
void clear();
```

**行为**

移除全部元素。

清空节点。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.clear();
```

**源码**：[source/std/tree.chl:270](../../source/std/tree.chl#L270)。

<a id="api-39"></a>

### GeneralTree::addRoot（40）

**声明**

```chali
int64 addRoot(int64 value);
```

**行为**

仅空树能建立根；返回零基节点号，否则返回 -1。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `int64 value` | 输入值或写入的新值 | 无 |

**返回值**

`int64`。仅空树能建立根；返回零基节点号，否则返回 -1。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.addRoot(value);
```

**源码**：[source/std/tree.chl:272](../../source/std/tree.chl#L272)。

<a id="api-40"></a>

### GeneralTree::addChild（41）

**声明**

```chali
int64 addChild(int64 cursor,int64 value);
```

**行为**

向有效父节点的末尾追加孩子。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `cursor` | `int64 cursor` | 仍有效的节点游标 | 无 |
| `value` | `int64 value` | 输入值或写入的新值 | 无 |

**返回值**

`int64`。向有效父节点的末尾追加孩子。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.addChild(cursor, value);
```

**源码**：[source/std/tree.chl:274](../../source/std/tree.chl#L274)。

<a id="api-41"></a>

### GeneralTree::valueAt（42）

**声明**

```chali
int64 valueAt(int64 cursor);
```

**行为**

返回有效节点值，无效下标报错。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `cursor` | `int64 cursor` | 仍有效的节点游标 | 无 |

**返回值**

`int64`。返回有效节点值，无效下标报错。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.valueAt(cursor);
```

**源码**：[source/std/tree.chl:283](../../source/std/tree.chl#L283)。

<a id="api-42"></a>

### GeneralTree::rootNode（43）

**声明**

```chali
int64 rootNode();
```

**行为**

返回零基根节点号，空树为 -1。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回零基根节点号，空树为 -1。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.rootNode();
```

**源码**：[source/std/tree.chl:285](../../source/std/tree.chl#L285)。

<a id="api-43"></a>

### GeneralTree::children（44）

**声明**

```chali
List<int64> children(int64 cursor);
```

**行为**

返回有效节点的独立孩子列表。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `cursor` | `int64 cursor` | 仍有效的节点游标 | 无 |

**返回值**

`List<int64>`。返回有效节点的独立孩子列表。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.children(cursor);
```

**源码**：[source/std/tree.chl:287](../../source/std/tree.chl#L287)。

<a id="api-44"></a>

### GeneralTree::preorder（45）

**声明**

```chali
List<int64> preorder();
```

**行为**

按先序返回独立节点号列表。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`List<int64>`。按先序返回独立节点号列表。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.preorder();
```

**源码**：[source/std/tree.chl:292](../../source/std/tree.chl#L292)。

<a id="api-45"></a>

### GeneralTree::levelOrder（46）

**声明**

```chali
List<int64> levelOrder();
```

**行为**

按层序返回独立节点号列表。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`List<int64>`。按层序返回独立节点号列表。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.levelOrder();
```

**源码**：[source/std/tree.chl:299](../../source/std/tree.chl#L299)。

<a id="api-46"></a>

### GeneralTree::height（47）

**声明**

```chali
int64 height();
```

**行为**

返回最大根叶路径节点数。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回最大根叶路径节点数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.height();
```

**源码**：[source/std/tree.chl:304](../../source/std/tree.chl#L304)。

<a id="api-47"></a>

### GeneralTree::toBinary（48）

**声明**

```chali
BinarySearchTree toBinary();
```

**行为**

独立转换为首孩子作左孩子、次兄弟作右孩子的二叉表示。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`BinarySearchTree`。独立转换为首孩子作左孩子、次兄弟作右孩子的二叉表示。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.toBinary();
```

**源码**：[source/std/tree.chl:309](../../source/std/tree.chl#L309)。

<a id="api-48"></a>

### GeneralTree::fromBinary（49）

**声明**

```chali
static GeneralTree fromBinary(BinarySearchTree tree);
```

**行为**

独立复制二叉表示，保持节点槽号及根。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `tree` | `BinarySearchTree tree` | 独立转换的源二叉树 | 无 |

**返回值**

`GeneralTree`。独立复制二叉表示，保持节点槽号及根。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::tree::GeneralTree::fromBinary(tree);
```

**源码**：[source/std/tree.chl:311](../../source/std/tree.chl#L311)。

<a id="api-49"></a>

### GeneralTree::snapshot（50）

**声明**

```chali
GeneralTreeSnapshot snapshot();
```

**行为**

返回独立首孩子、次兄弟快照。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`GeneralTreeSnapshot`。返回独立首孩子、次兄弟快照。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.snapshot();
```

**源码**：[source/std/tree.chl:313](../../source/std/tree.chl#L313)。
