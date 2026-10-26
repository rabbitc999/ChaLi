# std::graph — 图论与并查集

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](std-graph.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

[返回库目录](README.md) · [全部符号](symbols.md)

## 引入

```chali
import std::graph;
```

## 公共约定与错误边界

Graph 使用链式前向星存储：head 是每顶点首条边，edgeTo/edgeNext/edgeWeight 是边终点、同起点下一条边与边权，顶点编号 0..vertexCount-1，重边与自环都允许。有向 / 无向由 create 的 directed 决定，无向图加边时自动镜像登记。bfs/dfs 输出可达顶点的访问序；topologicalSort 用 Kahn 算法，有环时输出空表；dijkstra 要求边权非负，不可达顶点距离为 -1；hasCycle 有向图用 Kahn 计数、无向图用并查集；minimumSpanningTreeWeight 用 Kruskal，图不连通返回 -1。Graph 与 UnionFind 使用 ChaLi 结构体及独立数组，普通复制深复制；Graph.snapshot 和 UnionFind.parents/sizes 显式导出独立快照。UnionFind 路径压缩与按大小合并均原地执行，不随每次查询复制容量数组。BFS、DFS、Kahn、Dijkstra、判环、Kruskal 及并查集均由 ChaLi 实现；Kruskal 使用边权最小堆，不调用宿主图算法。

## 参考代码

聚合入口的具体操作参见对应子模块；本页类型/方法的调用形式列在下方。

## 类型、枚举、常量与字段

公开字段按源码原样列出；句柄和游标字段用于包装器状态，通常通过方法操作。枚举的括号表示变体载荷。

| 所属类型 | 声明 | 说明 |
| --- | --- | --- |
| 模块 | <a id="symbol-69"></a>`struct GraphReply` | 图算法的兼容回执类型；算法直接返回语言值，不再依赖宿主填写槽。 |
| GraphReply | <a id="symbol-70"></a>`String error` | 本次胶水调用的错误说明；成功时为空串。 |
| GraphReply | <a id="symbol-71"></a>`int64 value` | 标量结果：连通分量个数或生成树总权值。 |
| GraphReply | <a id="symbol-72"></a>`int64 flag` | 布尔结果：按入口语义表示无环 / 有环。 |
| GraphReply | <a id="symbol-73"></a>`List<int64> order` | 访问序或拓扑序。 |
| GraphReply | <a id="symbol-74"></a>`List<int64> dist` | 单源最短路距离表，不可达顶点为 -1。 |
| 模块 | <a id="symbol-75"></a>`struct GraphSnapshot` | 完整、独立的链式前向星邻接快照。 |
| GraphSnapshot | <a id="symbol-76"></a>`int64 vertexCount` | 顶点数量。 |
| GraphSnapshot | <a id="symbol-77"></a>`bool directed` | 是否为有向图。 |
| GraphSnapshot | <a id="symbol-78"></a>`int64 edgeCount` | 逻辑边数，不计无向镜像。 |
| GraphSnapshot | <a id="symbol-79"></a>`List<int64> head` | 各顶点第一条邻接记录。 |
| GraphSnapshot | <a id="symbol-80"></a>`List<int64> edgeTo` | 邻接终点数组。 |
| GraphSnapshot | <a id="symbol-81"></a>`List<int64> edgeNext` | 同起点下一条邻接记录。 |
| GraphSnapshot | <a id="symbol-82"></a>`List<int64> edgeWeight` | 邻接记录权重。 |
| 模块 | <a id="symbol-83"></a>`struct UnionFind: Copy<UnionFind>` | 持久父指针并查集，压缩和合并原地执行，普通复制深复制。 |
| 模块 | <a id="symbol-84"></a>`struct Graph: Copy<Graph>` | 持久邻接图，算法原地读取，普通复制深复制，snapshot 显式导出。 |

## 函数索引

| 名称 | 声明 |
| --- | --- |
| [UnionFind::create](#api-0) | `static UnionFind create(int64 n)` |
| [UnionFind::copy](#api-1) | `UnionFind copy()` |
| [UnionFind::find](#api-2) | `int64 find(int64 x)` |
| [UnionFind::unionSets](#api-3) | `bool unionSets(int64 a,int64 b)` |
| [UnionFind::connected](#api-4) | `bool connected(int64 a,int64 b)` |
| [UnionFind::sizeOf](#api-5) | `int64 sizeOf(int64 x)` |
| [UnionFind::count](#api-6) | `int64 count()` |
| [UnionFind::len](#api-7) | `int64 len()` |
| [UnionFind::parents](#api-8) | `List<int64> parents()` |
| [UnionFind::sizes](#api-9) | `List<int64> sizes()` |
| [Graph::create](#api-10) | `static Graph create(int64 vertexCount,bool directed)` |
| [Graph::copy](#api-11) | `Graph copy()` |
| [Graph::vertexCount](#api-12) | `int64 vertexCount()` |
| [Graph::isDirected](#api-13) | `bool isDirected()` |
| [Graph::len](#api-14) | `int64 len()` |
| [Graph::addEdge](#api-15) | `void addEdge(int64 from,int64 to,int64 weight)` |
| [Graph::bfs](#api-16) | `List<int64> bfs(int64 start)` |
| [Graph::dfs](#api-17) | `List<int64> dfs(int64 start)` |
| [Graph::connectedComponents](#api-18) | `int64 connectedComponents()` |
| [Graph::topologicalSort](#api-19) | `List<int64> topologicalSort()` |
| [Graph::dijkstra](#api-20) | `List<int64> dijkstra(int64 start)` |
| [Graph::hasCycle](#api-21) | `bool hasCycle()` |
| [Graph::minimumSpanningTreeWeight](#api-22) | `int64 minimumSpanningTreeWeight()` |
| [Graph::snapshot](#api-23) | `GraphSnapshot snapshot()` |

<a id="api-0"></a>

### UnionFind::create（1）

**声明**

```chali
static UnionFind create(int64 n);
```

**行为**

创建UnionFind，返回可供后续操作的值。

创建 n 个独立集合，保持原容量边界。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `n` | `int64 n` | 向量长度或矩阵阶数 | 无 |

**返回值**

`UnionFind`。创建UnionFind，返回可供后续操作的值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::graph::UnionFind::create(n);
```

**源码**：[source/std/graph.chl:12](../../source/std/graph.chl#L12)。

<a id="api-1"></a>

### UnionFind::copy（2）

**声明**

```chali
UnionFind copy();
```

**行为**

返回独立的值副本。

返回独立数组副本。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`UnionFind`。返回独立的值副本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.copy();
```

**源码**：[source/std/graph.chl:14](../../source/std/graph.chl#L14)。

<a id="api-2"></a>

### UnionFind::find（3）

**声明**

```chali
int64 find(int64 x);
```

**行为**

查找首个匹配项，返回下标或可选载荷；具体未找到表示见返回类型及模块约定。

先找到根，再原地压缩路径。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `x` | `int64 x` | 输入横坐标/标量 | 无 |

**返回值**

`int64`。查找首个匹配项，返回下标或可选载荷；具体未找到表示见返回类型及模块约定。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.find(x);
```

**源码**：[source/std/graph.chl:16](../../source/std/graph.chl#L16)。

<a id="api-3"></a>

### UnionFind::unionSets（4）

**声明**

```chali
bool unionSets(int64 a,int64 b);
```

**行为**

按集合大小合并，已连通时返回 false。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `a` | `int64 a` | 第一个元素或顶点下标 | 无 |
| `b` | `int64 b` | 第二个元素或顶点下标 | 无 |

**返回值**

`bool`。按集合大小合并，已连通时返回 false。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.unionSets(a, b);
```

**源码**：[source/std/graph.chl:18](../../source/std/graph.chl#L18)。

<a id="api-4"></a>

### UnionFind::connected（5）

**声明**

```chali
bool connected(int64 a,int64 b);
```

**行为**

判断是否连通。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `a` | `int64 a` | 第一个元素或顶点下标 | 无 |
| `b` | `int64 b` | 第二个元素或顶点下标 | 无 |

**返回值**

`bool`。判断是否连通。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.connected(a, b);
```

**源码**：[source/std/graph.chl:20](../../source/std/graph.chl#L20)。

<a id="api-5"></a>

### UnionFind::sizeOf（6）

**声明**

```chali
int64 sizeOf(int64 x);
```

**行为**

返回元素所在集合大小。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `x` | `int64 x` | 输入横坐标/标量 | 无 |

**返回值**

`int64`。返回元素所在集合大小。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.sizeOf(x);
```

**源码**：[source/std/graph.chl:22](../../source/std/graph.chl#L22)。

<a id="api-6"></a>

### UnionFind::count（7）

**声明**

```chali
int64 count();
```

**行为**

读取结果首行首列的整数表示。

返回连通分量数。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。读取结果首行首列的整数表示。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.count();
```

**源码**：[source/std/graph.chl:24](../../source/std/graph.chl#L24)。

<a id="api-7"></a>

### UnionFind::len（8）

**声明**

```chali
int64 len();
```

**行为**

返回当前元素数量。

返回元素总数。

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

**源码**：[source/std/graph.chl:26](../../source/std/graph.chl#L26)。

<a id="api-8"></a>

### UnionFind::parents（9）

**声明**

```chali
List<int64> parents();
```

**行为**

返回独立父指针列表。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`List<int64>`。返回独立父指针列表。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.parents();
```

**源码**：[source/std/graph.chl:28](../../source/std/graph.chl#L28)。

<a id="api-9"></a>

### UnionFind::sizes（10）

**声明**

```chali
List<int64> sizes();
```

**行为**

返回独立集合大小列表。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`List<int64>`。返回独立集合大小列表。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.sizes();
```

**源码**：[source/std/graph.chl:30](../../source/std/graph.chl#L30)。

<a id="api-10"></a>

### Graph::create（11）

**声明**

```chali
static Graph create(int64 vertexCount,bool directed);
```

**行为**

创建Graph，返回可供后续操作的值。

创建指定规模的空图。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `vertexCount` | `int64 vertexCount` | 非负顶点数量 | 无 |
| `directed` | `bool directed` | 是否创建有向图 | 无 |

**返回值**

`Graph`。创建Graph，返回可供后续操作的值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::graph::Graph::create(vertexCount, directed);
```

**源码**：[source/std/graph.chl:60](../../source/std/graph.chl#L60)。

<a id="api-11"></a>

### Graph::copy（12）

**声明**

```chali
Graph copy();
```

**行为**

返回独立的值副本。

返回完整邻接状态的独立副本。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Graph`。返回独立的值副本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.copy();
```

**源码**：[source/std/graph.chl:62](../../source/std/graph.chl#L62)。

<a id="api-12"></a>

### Graph::vertexCount（13）

**声明**

```chali
int64 vertexCount();
```

**行为**

返回顶点数。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回顶点数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.vertexCount();
```

**源码**：[source/std/graph.chl:64](../../source/std/graph.chl#L64)。

<a id="api-13"></a>

### Graph::isDirected（14）

**声明**

```chali
bool isDirected();
```

**行为**

返回是否为有向图。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。返回是否为有向图。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isDirected();
```

**源码**：[source/std/graph.chl:66](../../source/std/graph.chl#L66)。

<a id="api-14"></a>

### Graph::len（15）

**声明**

```chali
int64 len();
```

**行为**

返回当前元素数量。

返回逻辑边数，不计无向镜像记录。

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

**源码**：[source/std/graph.chl:68](../../source/std/graph.chl#L68)。

<a id="api-15"></a>

### Graph::addEdge（16）

**声明**

```chali
void addEdge(int64 from,int64 to,int64 weight);
```

**行为**

添加逻辑边，无向且不是自环时追加反向记录。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `from` | `int64 from` | 被替换的子串 | 无 |
| `to` | `int64 to` | 替换文本 | 无 |
| `weight` | `int64 weight` | 边权值 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.addEdge(from, to, weight);
```

**源码**：[source/std/graph.chl:72](../../source/std/graph.chl#L72)。

<a id="api-16"></a>

### Graph::bfs（17）

**声明**

```chali
List<int64> bfs(int64 start);
```

**行为**

广度优先访问；无效起点返回空列表。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `start` | `int64 start` | 起始值 | 无 |

**返回值**

`List<int64>`。广度优先访问；无效起点返回空列表。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.bfs(start);
```

**源码**：[source/std/graph.chl:76](../../source/std/graph.chl#L76)。

<a id="api-17"></a>

### Graph::dfs（18）

**声明**

```chali
List<int64> dfs(int64 start);
```

**行为**

显式栈深度优先访问，保持原邻接访问顺序。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `start` | `int64 start` | 起始值 | 无 |

**返回值**

`List<int64>`。显式栈深度优先访问，保持原邻接访问顺序。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.dfs(start);
```

**源码**：[source/std/graph.chl:81](../../source/std/graph.chl#L81)。

<a id="api-18"></a>

### Graph::connectedComponents（19）

**声明**

```chali
int64 connectedComponents();
```

**行为**

忽略方向，用并查集计算弱连通分量。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。忽略方向，用并查集计算弱连通分量。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.connectedComponents();
```

**源码**：[source/std/graph.chl:87](../../source/std/graph.chl#L87)。

<a id="api-19"></a>

### Graph::topologicalSort（20）

**声明**

```chali
List<int64> topologicalSort();
```

**行为**

Kahn 拓扑排序；有环返回空列表。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`List<int64>`。Kahn 拓扑排序；有环返回空列表。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.topologicalSort();
```

**源码**：[source/std/graph.chl:89](../../source/std/graph.chl#L89)。

<a id="api-20"></a>

### Graph::dijkstra（21）

**声明**

```chali
List<int64> dijkstra(int64 start);
```

**行为**

O(V²+E) Dijkstra；先拒绝所有负权边，不可达距离为 -1。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `start` | `int64 start` | 起始值 | 无 |

**返回值**

`List<int64>`。O(V²+E) Dijkstra；先拒绝所有负权边，不可达距离为 -1。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.dijkstra(start);
```

**源码**：[source/std/graph.chl:96](../../source/std/graph.chl#L96)。

<a id="api-21"></a>

### Graph::hasCycle（22）

**声明**

```chali
bool hasCycle();
```

**行为**

有向图按拓扑计数判环，无向图按逻辑边并查集判环，包含自环和重边。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。有向图按拓扑计数判环，无向图按逻辑边并查集判环，包含自环和重边。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.hasCycle();
```

**源码**：[source/std/graph.chl:109](../../source/std/graph.chl#L109)。

<a id="api-22"></a>

### Graph::minimumSpanningTreeWeight（23）

**声明**

```chali
int64 minimumSpanningTreeWeight();
```

**行为**

以边权堆执行 Kruskal；无向镜像只取一次，不连通返回 -1。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。以边权堆执行 Kruskal；无向镜像只取一次，不连通返回 -1。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.minimumSpanningTreeWeight();
```

**源码**：[source/std/graph.chl:115](../../source/std/graph.chl#L115)。

<a id="api-23"></a>

### Graph::snapshot（24）

**声明**

```chali
GraphSnapshot snapshot();
```

**行为**

导出完整独立邻接快照。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`GraphSnapshot`。导出完整独立邻接快照。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.snapshot();
```

**源码**：[source/std/graph.chl:123](../../source/std/graph.chl#L123)。
