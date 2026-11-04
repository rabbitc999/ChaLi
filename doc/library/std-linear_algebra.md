# std::linear_algebra — 向量与矩阵

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](std-linear_algebra.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

[返回库目录](README.md) · [全部符号](symbols.md)

## 引入

```chali
import std::linear_algebra;
```

## 公共约定与错误边界

Vector<T>、Matrix<T> 保留泛型聚合外观，values 使用 ScalarArray<T>，底层 ScalarStorage opcode 拥有连续数值缓冲，不再保存通用 List。支持内建有符号/无符号整数及 float32/float64，复制得到独立缓冲。下标从 0 开始；加减、点积和乘法必须维度兼容，非法维度触发断言或宿主错误。fromArray 复制，toArray 返回独立数组；裸引用重载须提供有效 count。normalized 不接受零向量。浮点分解和 solve 使用宿主胶水，返回分解结构/解向量；奇异、非法形状或不满足算法条件时报告运行错误，不返回 Result。

## 参考代码

完整程序：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

```powershell
chali run example/samples/std_linalg_solve_glue.chl
```

<details>
<summary>展开完整参考代码</summary>

```chali
import std::linear_algebra;
import std::math;

void checkSolve<T>() {
    std::linear_algebra::Matrix<T> a = std::linear_algebra::Matrix<T>::zeros(3, 3);
    a.set(0, 0, (T)0); a.set(0, 1, (T)2); a.set(0, 2, (T)1);
    a.set(1, 0, (T)2); a.set(1, 1, (T)1); a.set(1, 2, (T)0);
    a.set(2, 0, (T)1); a.set(2, 1, (T)0); a.set(2, 2, (T)3);
    std::linear_algebra::Vector<T> b = std::linear_algebra::Vector<T>::zeros(3);
    b.set(0, (T)7); b.set(1, (T)4); b.set(2, (T)10);
    std::linear_algebra::Vector<T> answer = a.solve(b);
    assert(std::math::abs((float64)answer.at(0) - 1.0) < 0.00001);
    assert(std::math::abs((float64)answer.at(1) - 2.0) < 0.00001);
    assert(std::math::abs((float64)answer.at(2) - 3.0) < 0.00001);
    assert(a.at(0, 0) == (T)0 && a.at(2, 2) == (T)3 && b.at(0) == (T)7);
    std::linear_algebra::Matrix<T> empty = std::linear_algebra::Matrix<T>::zeros(0, 0);
    std::linear_algebra::Vector<T> emptyRhs = std::linear_algebra::Vector<T>::zeros(0);
    std::linear_algebra::Vector<T> emptyAnswer = empty.solve(emptyRhs);
    assert(emptyAnswer.len() == 0);
    std::linear_algebra::Matrix<T> one = std::linear_algebra::Matrix<T>::identity(1);
    std::linear_algebra::Vector<T> rhs = std::linear_algebra::Vector<T>::zeros(1);
    rhs.set(0, (T)7);
    std::linear_algebra::Vector<T> single = one.solve(rhs);
    assert(single.at(0) == (T)7);
}
void checkInteger<T>() {
    std::linear_algebra::Matrix<T> a = std::linear_algebra::Matrix<T>::identity(1);
    std::linear_algebra::Vector<T> rhs = std::linear_algebra::Vector<T>::zeros(1);
    rhs.set(0, (T)7);
    std::linear_algebra::Vector<T> answer = a.solve(rhs);
    assert(answer.at(0) == (T)7);
}
float64[] makeArray() {
    float64 values[] = new[1];
    values[0] = 4.0;
    return values;
}
struct ArrayFactory {
    static float64[] make() {
        float64 values[] = new[1];
        values[0] = 5.0;
        return values;
    }
};
std::linear_algebra::Vector<float64> firstColumn() {
    for (col in std::linear_algebra::Matrix<float64>::identity(2)) { return col; }
    return std::linear_algebra::Vector<float64>::zeros(0);
}
struct ArrayItem { int64 x; int64 y; };
void checkArrays() {
    float64 first[] = makeArray();
    float64 second[] = ArrayFactory::make();
    assert(first[0] == 4.0 && second[0] == 5.0);
    float64 raw[6];
    raw[0] = 1.0; raw[1] = 2.0; raw[2] = 3.0;
    raw[3] = 4.0; raw[4] = 5.0; raw[5] = 6.0;
    std::linear_algebra::Vector<float64> vector = std::linear_algebra::Vector<float64>::fromArray(raw, len(raw));
    assert(vector[2] == 3.0);
    float64 vectorArray[] = vector.toArray();
    vectorArray[0] = 99.0;
    assert(vector[0] == 1.0);
    std::linear_algebra::Vector<float64> restored = std::linear_algebra::Vector<float64>::fromArray(vectorArray);
    assert(restored[0] == 99.0);
    std::linear_algebra::Matrix<float64> matrix = std::linear_algebra::Matrix<float64>::fromArray(raw, len(raw), 2, 3);
    std::linear_algebra::Vector<float64> column = matrix[1];
    assert(column.len() == 2 && column[0] == 2.0 && column[1] == 5.0);
    float64 sum = 0.0;
    for (col in matrix) {
        for (number in col) { sum = sum + number; }
    }
    assert(sum == 21.0);
    float64 arraySum = 0.0;
    for (number in raw) { arraySum = arraySum + number; }
    assert(arraySum == sum);
    int64 visited = 0;
    for (col in matrix) {
        visited = visited + 1;
        if (visited == 1) { continue; }
        break;
    }
    assert(visited == 2);
    std::linear_algebra::Vector<float64> returned = firstColumn();
    assert(returned[0] == 1.0 && returned[1] == 0.0);
    float64 temporarySum = 0.0;
    for (number in makeArray()) { temporarySum = temporarySum + number; }
    assert(temporarySum == 4.0);
    ArrayItem records[2];
    records[0].x = 1; records[0].y = 2;
    records[1].x = 3; records[1].y = 4;
    int64 recordSum = 0;
    for (entry in records) { recordSum = recordSum + entry.x + entry.y; }
    assert(recordSum == 10);
    int64 grid[2][2];
    grid[0][0] = 1; grid[0][1] = 2; grid[1][0] = 3; grid[1][1] = 4;
    int64 gridSum = 0;
    for (number in grid) { gridSum = gridSum + number; }
    assert(gridSum == 10);
    column.set(0, 88.0);
    assert(matrix.at(0, 1) == 2.0);
    float64 flat[] = matrix.toArray();
    assert(len(flat) == 6 && flat[4] == 5.0);
    std::linear_algebra::Matrix<float64> copy = std::linear_algebra::Matrix<float64>::fromArray(flat, 2, 3);
    flat[0] = 77.0;
    assert(copy.at(0, 0) == 1.0 && raw[0] == 1.0);
    std::linear_algebra::Vector<float64> last = copy.column(2);
    assert(last[0] == 3.0 && last[1] == 6.0);
    std::linear_algebra::Matrix<float64> empty = std::linear_algebra::Matrix<float64>::zeros(0, 2);
    std::linear_algebra::Vector<float64> emptyColumn = empty[1];
    float64 emptyArray[] = emptyColumn.toArray();
    assert(len(emptyArray) == 0);
}
void main() {
    checkArrays();
    checkSolve<float32>();
    checkSolve<float64>();
    checkInteger<int8>();
    checkInteger<int16>();
    checkInteger<int32>();
    checkInteger<int64>();
    checkInteger<uint8>();
    checkInteger<uint16>();
    checkInteger<uint32>();
    checkInteger<uint64>();

    std::linear_algebra::Matrix<int64> ints = std::linear_algebra::Matrix<int64>::identity(2);
    std::linear_algebra::Vector<int64> rhs = std::linear_algebra::Vector<int64>::zeros(2);
    rhs.set(0, -5); rhs.set(1, 7);
    std::linear_algebra::Vector<int64> answer = ints.solve(rhs);
    assert(answer.at(0) == -5 && answer.at(1) == 7);
    println("LU glue solve OK");
}
```

</details>

<details>
<summary>示例输出</summary>

```text
LU glue solve OK
```

</details>

聚合入口的具体操作参见对应子模块；本页类型/方法的调用形式列在下方。

## 类型、枚举、常量与字段

公开字段按源码原样列出；句柄和游标字段用于包装器状态，通常通过方法操作。枚举的括号表示变体载荷。

| 所属类型 | 声明 | 说明 |
| --- | --- | --- |
| 模块 | <a id="symbol-309"></a>`opcode ScalarStorage` | 线性代数专用数值存储，由 C 原语管理连续数值槽；复制独立，clear 保留容量。 |
| 模块 | <a id="symbol-310"></a>`struct ScalarArray<T>: Index<T, int64>, Iterator<T>` | 为 bool、char、全部整数与浮点类型提供泛型索引外观，元素保存在 opcode 中。 |
| ScalarArray | <a id="symbol-311"></a>`ScalarStorage storage` | 独占此数组数值缓冲的 opcode 记录；复制时深复制。 |
| 模块 | <a id="symbol-312"></a>`struct Vector<T>: Add<Vector<T>, Vector<T>>, Subtract<Vector<T>, Vector<T>>, Multiply<Vector<T>, T>, Index<T, int64>, Iterator<T>, Drop` | 稠密数值向量，元素按顺序存储。 |
| Vector | <a id="symbol-313"></a>`ScalarArray<T> values` | 按声明顺序保存的元素存储。 |
| 模块 | <a id="symbol-314"></a>`struct Matrix<T>: Add<Matrix<T>, Matrix<T>>, Subtract<Matrix<T>, Matrix<T>>, Multiply<Matrix<T>, Matrix<T>>, Multiply<Vector<T>, Vector<T>>, Multiply<Matrix<T>, T>, Index<Vector<T>, int64>, Iterator<Vector<T>>, Drop` | 行主元 LU 满足 P*A=L*U；矩阵求解的主元判断适用于 float32/float64。 |
| Matrix | <a id="symbol-315"></a>`int64 rows` | 矩阵的行数。 |
| Matrix | <a id="symbol-316"></a>`int64 cols` | 矩阵的列数。 |
| Matrix | <a id="symbol-317"></a>`ScalarArray<T> values` | 按声明顺序保存的元素存储。 |
| 模块 | <a id="symbol-318"></a>`struct LuFactorization` | LU 分解结果，包含下三角、上三角及置换矩阵。 |
| LuFactorization | <a id="symbol-319"></a>`Matrix<float64> lower` | LU 分解的下三角矩阵。 |
| LuFactorization | <a id="symbol-320"></a>`Matrix<float64> upper` | LU 分解的上三角矩阵。 |
| LuFactorization | <a id="symbol-321"></a>`Matrix<float64> permutation` | LU 分解的置换矩阵。 |
| 模块 | <a id="symbol-322"></a>`struct EigenDecomposition` | 特征分解结果，包含特征值与对应特征向量。 |
| EigenDecomposition | <a id="symbol-323"></a>`Vector<float64> eigenvalues` | 分解得到的特征值。 |
| EigenDecomposition | <a id="symbol-324"></a>`Matrix<float64> eigenvectors` | 与特征值对应的特征向量。 |
| 模块 | <a id="symbol-325"></a>`struct SingularValueDecomposition` | 奇异值分解结果，包含 U、奇异值及 V 的转置。 |
| SingularValueDecomposition | <a id="symbol-326"></a>`Matrix<float64> u` | 奇异值分解的左奇异向量矩阵。 |
| SingularValueDecomposition | <a id="symbol-327"></a>`Vector<float64> singularValues` | 分解得到的奇异值。 |
| SingularValueDecomposition | <a id="symbol-328"></a>`Matrix<float64> vTranspose` | 右奇异向量矩阵 V 的转置。 |
| 模块 | <a id="symbol-329"></a>`struct JordanDecomposition` | Jordan 分解结果，包含 Jordan 矩阵与变换基。 |
| JordanDecomposition | <a id="symbol-330"></a>`Matrix<float64> basis` | 将原矩阵转换到分解形式的基矩阵。 |
| JordanDecomposition | <a id="symbol-331"></a>`Matrix<float64> jordan` | Jordan 标准形矩阵。 |

## 函数索引

| 名称 | 声明 |
| --- | --- |
| [ScalarArray::len](#api-0) | `int64 len()` |
| [ScalarArray::clear](#api-1) | `void clear()` |
| [ScalarArray::at](#api-2) | `T at(int64 index)` |
| [ScalarArray::index](#api-3) | `T index(int64 key)` |
| [ScalarArray::set](#api-4) | `void set(int64 index, T value)` |
| [ScalarArray::push](#api-5) | `void push(T value)` |
| [ScalarArray::copy](#api-6) | `ScalarArray<T> copy()` |
| [ScalarArray::iterator](#api-7) | `Option<T> iterator(int64 position)` |
| [Vector::drop](#api-8) | `void drop()` |
| [Vector::zeros](#api-9) | `static Vector<T> zeros(int64 n)` |
| [Vector::fromArray](#api-10) | `static Vector<T> fromArray(T const& data, int64 count)` |
| [Vector::fromArray](#api-11) | `static Vector<T> fromArray(T data[])` |
| [Vector::toArray](#api-12) | `T[] toArray()` |
| [Vector::index](#api-13) | `T index(int64 key)` |
| [Vector::iterator](#api-14) | `Option<T> iterator(int64 position)` |
| [Vector::len](#api-15) | `int64 len()` |
| [Vector::at](#api-16) | `T at(int64 i)` |
| [Vector::set](#api-17) | `void set(int64 i, T value)` |
| [Vector::dot](#api-18) | `T dot(Vector<T> other)` |
| [Vector::norm](#api-19) | `float64 norm()` |
| [Vector::normalized](#api-20) | `Vector<T> normalized()` |
| [Vector::add](#api-21) | `Vector<T> add(Vector<T> other)` |
| [Vector::subtract](#api-22) | `Vector<T> subtract(Vector<T> other)` |
| [Vector::scale](#api-23) | `Vector<T> scale(T amount)` |
| [Vector::leftAdd](#api-24) | `Vector<T> leftAdd(Vector<T> right)` |
| [Vector::leftSub](#api-25) | `Vector<T> leftSub(Vector<T> right)` |
| [Vector::leftMul](#api-26) | `Vector<T> leftMul(T right)` |
| [Matrix::drop](#api-27) | `void drop()` |
| [Matrix::zeros](#api-28) | `static Matrix<T> zeros(int64 rows, int64 cols)` |
| [Matrix::fromArray](#api-29) | `static Matrix<T> fromArray(T const& data, int64 count, int64 rows, int64 cols)` |
| [Matrix::fromArray](#api-30) | `static Matrix<T> fromArray(T data[], int64 rows, int64 cols)` |
| [Matrix::toArray](#api-31) | `T[] toArray()` |
| [Matrix::column](#api-32) | `Vector<T> column(int64 key)` |
| [Matrix::index](#api-33) | `Vector<T> index(int64 key)` |
| [Matrix::iterator](#api-34) | `Option<Vector<T>> iterator(int64 position)` |
| [Matrix::identity](#api-35) | `static Matrix<T> identity(int64 n)` |
| [Matrix::at](#api-36) | `T at(int64 row, int64 col)` |
| [Matrix::set](#api-37) | `void set(int64 row, int64 col, T value)` |
| [Matrix::transpose](#api-38) | `Matrix<T> transpose()` |
| [Matrix::add](#api-39) | `Matrix<T> add(Matrix<T> other)` |
| [Matrix::subtract](#api-40) | `Matrix<T> subtract(Matrix<T> other)` |
| [Matrix::scale](#api-41) | `Matrix<T> scale(T factor)` |
| [Matrix::multiply](#api-42) | `Matrix<T> multiply(Matrix<T> other)` |
| [Matrix::apply](#api-43) | `Vector<T> apply(Vector<T> vector)` |
| [Matrix::solve](#api-44) | `Vector<T> solve(Vector<T> rhs)` |
| [Matrix::leftAdd](#api-45) | `Matrix<T> leftAdd(Matrix<T> right)` |
| [Matrix::leftSub](#api-46) | `Matrix<T> leftSub(Matrix<T> right)` |
| [Matrix::leftMul](#api-47) | `Matrix<T> leftMul(Matrix<T> right)` |
| [Matrix::leftMul](#api-48) | `Vector<T> leftMul(Vector<T> right)` |
| [Matrix::leftMul](#api-49) | `Matrix<T> leftMul(T right)` |
| [lu_decompose](#api-50) | `LuFactorization lu_decompose(Matrix<float64> input)` |
| [lur_decompose](#api-51) | `LuFactorization lur_decompose(Matrix<float64> input)` |
| [symmetric_eigen](#api-52) | `EigenDecomposition symmetric_eigen(Matrix<float64> input)` |
| [svd](#api-53) | `SingularValueDecomposition svd(Matrix<float64> input)` |
| [jordan2](#api-54) | `JordanDecomposition jordan2(Matrix<float64> input)` |
| [diagonalize2](#api-55) | `JordanDecomposition diagonalize2(Matrix<float64> input)` |

<a id="api-0"></a>

### ScalarArray::len（1）

**声明**

```chali
int64 len();
```

**行为**

返回当前元素数量。

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

**源码**：[source/std/linear_algebra.chl:23](../../source/std/linear_algebra.chl#L23)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-1"></a>

### ScalarArray::clear（2）

**声明**

```chali
void clear();
```

**行为**

移除全部元素。

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

**源码**：[source/std/linear_algebra.chl:27](../../source/std/linear_algebra.chl#L27)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-2"></a>

### ScalarArray::at（3）

**声明**

```chali
T at(int64 index);
```

**行为**

读取从 0 开始的指定位置。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `index` | `int64 index` | 从 0 开始的位置 | 无 |

**返回值**

`T`。读取从 0 开始的指定位置。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.at(index);
```

**源码**：[source/std/linear_algebra.chl:28](../../source/std/linear_algebra.chl#L28)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-3"></a>

### ScalarArray::index（4）

**声明**

```chali
T index(int64 key);
```

**行为**

读取 key 对应的元素，供下标语法调用。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `key` | `int64 key` | 查找键或下标 | 无 |

**返回值**

`T`。读取 key 对应的元素，供下标语法调用。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.index(key);
```

**源码**：[source/std/linear_algebra.chl:29](../../source/std/linear_algebra.chl#L29)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-4"></a>

### ScalarArray::set（5）

**声明**

```chali
void set(int64 index, T value);
```

**行为**

将指定位置或共享槽替换为 value。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `index` | `int64 index` | 从 0 开始的位置 | 无 |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.set(index, value);
```

**源码**：[source/std/linear_algebra.chl:30](../../source/std/linear_algebra.chl#L30)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-5"></a>

### ScalarArray::push（6）

**声明**

```chali
void push(T value);
```

**行为**

将 value 追加到列表尾部。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.push(value);
```

**源码**：[source/std/linear_algebra.chl:31](../../source/std/linear_algebra.chl#L31)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-6"></a>

### ScalarArray::copy（7）

**声明**

```chali
ScalarArray<T> copy();
```

**行为**

返回独立的值副本。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`ScalarArray<T>`。返回独立的值副本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.copy();
```

**源码**：[source/std/linear_algebra.chl:32](../../source/std/linear_algebra.chl#L32)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-7"></a>

### ScalarArray::iterator（8）

**声明**

```chali
Option<T> iterator(int64 position);
```

**行为**

读取指定迭代位置；无元素时返回 None。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `position` | `int64 position` | 目标绝对位置 | 无 |

**返回值**

`Option<T>`。读取指定迭代位置；无元素时返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.iterator(position);
```

**源码**：[source/std/linear_algebra.chl:33](../../source/std/linear_algebra.chl#L33)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-8"></a>

### Vector::drop（9）

**声明**

```chali
void drop();
```

**行为**

释放对象拥有的资源，调用后遵守本模块失效规则。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.drop();
```

**源码**：[source/std/linear_algebra.chl:43](../../source/std/linear_algebra.chl#L43)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-9"></a>

### Vector::zeros（10）

**声明**

```chali
static Vector<T> zeros(int64 n);
```

**行为**

创建指定大小并以 T 的默认值填充。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `n` | `int64 n` | 向量长度或矩阵阶数 | 无 |

**返回值**

`Vector<T>`。创建指定大小并以 T 的默认值填充。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`n >= 0, "Vector 长度必须非负"`。

**调用形式**

```chali
std::linear_algebra::Vector<T>::zeros(n);
```

**源码**：[source/std/linear_algebra.chl:45](../../source/std/linear_algebra.chl#L45)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-10"></a>

### Vector::fromArray（11）

**声明**

```chali
static Vector<T> fromArray(T const& data, int64 count);
```

**行为**

从连续数组复制元素；带 count 的重载由调用者保证有效范围。

从数组复制数据；指针形式同时接受固定数组与运行时数组。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `data` | `T const& data` | 连续存储的首元素引用，调用者保证范围有效 | 无 |
| `count` | `int64 count` | 元素或字节数量，具体单位见行为说明 | 无 |

**返回值**

`Vector<T>`。从连续数组复制元素；带 count 的重载由调用者保证有效范围。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`count >= 0, "Vector.fromArray 长度必须非负"`。

**调用形式**

```chali
std::linear_algebra::Vector<T>::fromArray(data, count);
```

**源码**：[source/std/linear_algebra.chl:56](../../source/std/linear_algebra.chl#L56)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-11"></a>

### Vector::fromArray（12）

**声明**

```chali
static Vector<T> fromArray(T data[]);
```

**行为**

从连续数组复制元素；带 count 的重载由调用者保证有效范围。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `data` | `T data[]` | 连续存储的首元素引用，调用者保证范围有效 | 无 |

**返回值**

`Vector<T>`。从连续数组复制元素；带 count 的重载由调用者保证有效范围。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::linear_algebra::Vector<T>::fromArray(data);
```

**源码**：[source/std/linear_algebra.chl:63](../../source/std/linear_algebra.chl#L63)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-12"></a>

### Vector::toArray（13）

**声明**

```chali
T[] toArray();
```

**行为**

复制成由调用者作用域管理的独立运行时数组。

返回独立的运行时数组，由调用者作用域管理。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`T[]`。复制成由调用者作用域管理的独立运行时数组。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.toArray();
```

**源码**：[source/std/linear_algebra.chl:65](../../source/std/linear_algebra.chl#L65)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-13"></a>

### Vector::index（14）

**声明**

```chali
T index(int64 key);
```

**行为**

读取 key 对应的元素，供下标语法调用。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `key` | `int64 key` | 查找键或下标 | 无 |

**返回值**

`T`。读取 key 对应的元素，供下标语法调用。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.index(key);
```

**源码**：[source/std/linear_algebra.chl:71](../../source/std/linear_algebra.chl#L71)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-14"></a>

### Vector::iterator（15）

**声明**

```chali
Option<T> iterator(int64 position);
```

**行为**

读取指定迭代位置；无元素时返回 None。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `position` | `int64 position` | 目标绝对位置 | 无 |

**返回值**

`Option<T>`。读取指定迭代位置；无元素时返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.iterator(position);
```

**源码**：[source/std/linear_algebra.chl:72](../../source/std/linear_algebra.chl#L72)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-15"></a>

### Vector::len（16）

**声明**

```chali
int64 len();
```

**行为**

返回当前元素数量。

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

**源码**：[source/std/linear_algebra.chl:76](../../source/std/linear_algebra.chl#L76)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-16"></a>

### Vector::at（17）

**声明**

```chali
T at(int64 i);
```

**行为**

读取从 0 开始的指定位置。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `i` | `int64 i` | 从 0 开始的元素下标 | 无 |

**返回值**

`T`。读取从 0 开始的指定位置。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.at(i);
```

**源码**：[source/std/linear_algebra.chl:77](../../source/std/linear_algebra.chl#L77)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-17"></a>

### Vector::set（18）

**声明**

```chali
void set(int64 i, T value);
```

**行为**

将指定位置或共享槽替换为 value。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `i` | `int64 i` | 从 0 开始的元素下标 | 无 |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.set(i, value);
```

**源码**：[source/std/linear_algebra.chl:78](../../source/std/linear_algebra.chl#L78)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-18"></a>

### Vector::dot（19）

**声明**

```chali
T dot(Vector<T> other);
```

**行为**

计算等长向量的点积。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `other` | `Vector<T> other` | 另一操作数 | 无 |

**返回值**

`T`。计算等长向量的点积。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`self.len() == other.len(), "Vector.dot 维度不匹配"`。

**调用形式**

```chali
obj.dot(other);
```

**源码**：[source/std/linear_algebra.chl:79](../../source/std/linear_algebra.chl#L79)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-19"></a>

### Vector::norm（20）

**声明**

```chali
float64 norm();
```

**行为**

返回向量欧氏范数。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`float64`。返回向量欧氏范数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.norm();
```

**源码**：[source/std/linear_algebra.chl:86](../../source/std/linear_algebra.chl#L86)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-20"></a>

### Vector::normalized（21）

**声明**

```chali
Vector<T> normalized();
```

**行为**

返回单位向量；零向量报错。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Vector<T>`。返回单位向量；零向量报错。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`length > 0.000000000001, "零向量不能归一化"`。

**调用形式**

```chali
obj.normalized();
```

**源码**：[source/std/linear_algebra.chl:87](../../source/std/linear_algebra.chl#L87)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-21"></a>

### Vector::add（22）

**声明**

```chali
Vector<T> add(Vector<T> other);
```

**行为**

返回同维向量/矩阵逐项相加的新值。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `other` | `Vector<T> other` | 另一操作数 | 无 |

**返回值**

`Vector<T>`。返回同维向量/矩阵逐项相加的新值。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`self.len() == other.len(), "Vector.add 维度不匹配"`。

**调用形式**

```chali
obj.add(other);
```

**源码**：[source/std/linear_algebra.chl:92](../../source/std/linear_algebra.chl#L92)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-22"></a>

### Vector::subtract（23）

**声明**

```chali
Vector<T> subtract(Vector<T> other);
```

**行为**

返回等维数据逐项相减的结果。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `other` | `Vector<T> other` | 另一操作数 | 无 |

**返回值**

`Vector<T>`。返回等维数据逐项相减的结果。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`self.len() == other.len(), "Vector.subtract 维度不匹配"`。

**调用形式**

```chali
obj.subtract(other);
```

**源码**：[source/std/linear_algebra.chl:99](../../source/std/linear_algebra.chl#L99)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-23"></a>

### Vector::scale（24）

**声明**

```chali
Vector<T> scale(T amount);
```

**行为**

返回各分量乘以 factor 的新值。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `amount` | `T amount` | 需要跳过或取得的元素数量 | 无 |

**返回值**

`Vector<T>`。返回各分量乘以 factor 的新值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.scale(amount);
```

**源码**：[source/std/linear_algebra.chl:106](../../source/std/linear_algebra.chl#L106)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-24"></a>

### Vector::leftAdd（25）

**声明**

```chali
Vector<T> leftAdd(Vector<T> right);
```

**行为**

实现左操作数加法接口。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `right` | `Vector<T> right` | 右操作数或区间右端 | 无 |

**返回值**

`Vector<T>`。实现左操作数加法接口。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.leftAdd(right);
```

**源码**：[source/std/linear_algebra.chl:112](../../source/std/linear_algebra.chl#L112)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-25"></a>

### Vector::leftSub（26）

**声明**

```chali
Vector<T> leftSub(Vector<T> right);
```

**行为**

实现左操作数减法接口。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `right` | `Vector<T> right` | 右操作数或区间右端 | 无 |

**返回值**

`Vector<T>`。实现左操作数减法接口。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.leftSub(right);
```

**源码**：[source/std/linear_algebra.chl:113](../../source/std/linear_algebra.chl#L113)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-26"></a>

### Vector::leftMul（27）

**声明**

```chali
Vector<T> leftMul(T right);
```

**行为**

实现左操作数乘法接口。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `right` | `T right` | 右操作数或区间右端 | 无 |

**返回值**

`Vector<T>`。实现左操作数乘法接口。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.leftMul(right);
```

**源码**：[source/std/linear_algebra.chl:114](../../source/std/linear_algebra.chl#L114)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-27"></a>

### Matrix::drop（28）

**声明**

```chali
void drop();
```

**行为**

释放对象拥有的资源，调用后遵守本模块失效规则。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.drop();
```

**源码**：[source/std/linear_algebra.chl:126](../../source/std/linear_algebra.chl#L126)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-28"></a>

### Matrix::zeros（29）

**声明**

```chali
static Matrix<T> zeros(int64 rows, int64 cols);
```

**行为**

创建指定大小并以 T 的默认值填充。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `rows` | `int64 rows` | 矩阵行数 | 无 |
| `cols` | `int64 cols` | 矩阵列数 | 无 |

**返回值**

`Matrix<T>`。创建指定大小并以 T 的默认值填充。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`rows >= 0 && cols >= 0, "Matrix 维度必须非负"`。

**调用形式**

```chali
std::linear_algebra::Matrix<T>::zeros(rows, cols);
```

**源码**：[source/std/linear_algebra.chl:128](../../source/std/linear_algebra.chl#L128)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-29"></a>

### Matrix::fromArray（30）

**声明**

```chali
static Matrix<T> fromArray(T const& data, int64 count, int64 rows, int64 cols);
```

**行为**

从连续数组复制元素；带 count 的重载由调用者保证有效范围。

按行优先顺序从一维数组复制，元素数必须等于 rows * cols。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `data` | `T const& data` | 连续存储的首元素引用，调用者保证范围有效 | 无 |
| `count` | `int64 count` | 元素或字节数量，具体单位见行为说明 | 无 |
| `rows` | `int64 rows` | 矩阵行数 | 无 |
| `cols` | `int64 cols` | 矩阵列数 | 无 |

**返回值**

`Matrix<T>`。从连续数组复制元素；带 count 的重载由调用者保证有效范围。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`rows >= 0 && cols >= 0 && count >= 0, "Matrix.fromArray 维度不匹配"`。
显式前置检查：`count == 0, "Matrix.fromArray 维度不匹配"`。
显式前置检查：`count / rows == cols && count % rows == 0, "Matrix.fromArray 维度不匹配"`。

**调用形式**

```chali
std::linear_algebra::Matrix<T>::fromArray(data, count, rows, cols);
```

**源码**：[source/std/linear_algebra.chl:140](../../source/std/linear_algebra.chl#L140)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-30"></a>

### Matrix::fromArray（31）

**声明**

```chali
static Matrix<T> fromArray(T data[], int64 rows, int64 cols);
```

**行为**

从连续数组复制元素；带 count 的重载由调用者保证有效范围。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `data` | `T data[]` | 连续存储的首元素引用，调用者保证范围有效 | 无 |
| `rows` | `int64 rows` | 矩阵行数 | 无 |
| `cols` | `int64 cols` | 矩阵列数 | 无 |

**返回值**

`Matrix<T>`。从连续数组复制元素；带 count 的重载由调用者保证有效范围。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::linear_algebra::Matrix<T>::fromArray(data, rows, cols);
```

**源码**：[source/std/linear_algebra.chl:149](../../source/std/linear_algebra.chl#L149)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-31"></a>

### Matrix::toArray（32）

**声明**

```chali
T[] toArray();
```

**行为**

复制成由调用者作用域管理的独立运行时数组。

返回独立的行优先运行时数组。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`T[]`。复制成由调用者作用域管理的独立运行时数组。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.toArray();
```

**源码**：[source/std/linear_algebra.chl:153](../../source/std/linear_algebra.chl#L153)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-32"></a>

### Matrix::column（33）

**声明**

```chali
Vector<T> column(int64 key);
```

**行为**

复制指定列为向量。

按零起始列索引返回独立列向量。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `key` | `int64 key` | 查找键或下标 | 无 |

**返回值**

`Vector<T>`。复制指定列为向量。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.column(key);
```

**源码**：[source/std/linear_algebra.chl:160](../../source/std/linear_algebra.chl#L160)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-33"></a>

### Matrix::index（34）

**声明**

```chali
Vector<T> index(int64 key);
```

**行为**

读取 key 对应的元素，供下标语法调用。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `key` | `int64 key` | 查找键或下标 | 无 |

**返回值**

`Vector<T>`。读取 key 对应的元素，供下标语法调用。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.index(key);
```

**源码**：[source/std/linear_algebra.chl:161](../../source/std/linear_algebra.chl#L161)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-34"></a>

### Matrix::iterator（35）

**声明**

```chali
Option<Vector<T>> iterator(int64 position);
```

**行为**

读取指定迭代位置；无元素时返回 None。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `position` | `int64 position` | 目标绝对位置 | 无 |

**返回值**

`Option<Vector<T>>`。读取指定迭代位置；无元素时返回 None。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.iterator(position);
```

**源码**：[source/std/linear_algebra.chl:162](../../source/std/linear_algebra.chl#L162)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-35"></a>

### Matrix::identity（36）

**声明**

```chali
static Matrix<T> identity(int64 n);
```

**行为**

创建单位矩阵。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `n` | `int64 n` | 向量长度或矩阵阶数 | 无 |

**返回值**

`Matrix<T>`。创建单位矩阵。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::linear_algebra::Matrix<T>::identity(n);
```

**源码**：[source/std/linear_algebra.chl:168](../../source/std/linear_algebra.chl#L168)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-36"></a>

### Matrix::at（37）

**声明**

```chali
T at(int64 row, int64 col);
```

**行为**

读取从 0 开始的指定位置。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `row` | `int64 row` | 从 0 开始的行下标 | 无 |
| `col` | `int64 col` | 从 0 开始的列下标 | 无 |

**返回值**

`T`。读取从 0 开始的指定位置。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`row >= 0 && row < self.rows && col >= 0 && col < self.cols, "Matrix.at 越界"`。

**调用形式**

```chali
obj.at(row, col);
```

**源码**：[source/std/linear_algebra.chl:174](../../source/std/linear_algebra.chl#L174)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-37"></a>

### Matrix::set（38）

**声明**

```chali
void set(int64 row, int64 col, T value);
```

**行为**

将指定位置或共享槽替换为 value。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `row` | `int64 row` | 从 0 开始的行下标 | 无 |
| `col` | `int64 col` | 从 0 开始的列下标 | 无 |
| `value` | `T value` | 输入值或写入的新值 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`row >= 0 && row < self.rows && col >= 0 && col < self.cols, "Matrix.set 越界"`。

**调用形式**

```chali
obj.set(row, col, value);
```

**源码**：[source/std/linear_algebra.chl:178](../../source/std/linear_algebra.chl#L178)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-38"></a>

### Matrix::transpose（39）

**声明**

```chali
Matrix<T> transpose();
```

**行为**

返回转置矩阵。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Matrix<T>`。返回转置矩阵。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.transpose();
```

**源码**：[source/std/linear_algebra.chl:182](../../source/std/linear_algebra.chl#L182)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-39"></a>

### Matrix::add（40）

**声明**

```chali
Matrix<T> add(Matrix<T> other);
```

**行为**

返回同维向量/矩阵逐项相加的新值。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `other` | `Matrix<T> other` | 另一操作数 | 无 |

**返回值**

`Matrix<T>`。返回同维向量/矩阵逐项相加的新值。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`self.rows == other.rows && self.cols == other.cols, "Matrix.add 维度不匹配"`。

**调用形式**

```chali
obj.add(other);
```

**源码**：[source/std/linear_algebra.chl:192](../../source/std/linear_algebra.chl#L192)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-40"></a>

### Matrix::subtract（41）

**声明**

```chali
Matrix<T> subtract(Matrix<T> other);
```

**行为**

返回等维数据逐项相减的结果。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `other` | `Matrix<T> other` | 另一操作数 | 无 |

**返回值**

`Matrix<T>`。返回等维数据逐项相减的结果。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`self.rows == other.rows && self.cols == other.cols, "Matrix.subtract 维度不匹配"`。

**调用形式**

```chali
obj.subtract(other);
```

**源码**：[source/std/linear_algebra.chl:203](../../source/std/linear_algebra.chl#L203)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-41"></a>

### Matrix::scale（42）

**声明**

```chali
Matrix<T> scale(T factor);
```

**行为**

返回各分量乘以 factor 的新值。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `factor` | `T factor` | 缩放因子 | 无 |

**返回值**

`Matrix<T>`。返回各分量乘以 factor 的新值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.scale(factor);
```

**源码**：[source/std/linear_algebra.chl:214](../../source/std/linear_algebra.chl#L214)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-42"></a>

### Matrix::multiply（43）

**声明**

```chali
Matrix<T> multiply(Matrix<T> other);
```

**行为**

进行矩阵乘法。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `other` | `Matrix<T> other` | 另一操作数 | 无 |

**返回值**

`Matrix<T>`。进行矩阵乘法。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`self.cols == other.rows, "Matrix.multiply 维度不匹配"`。

**调用形式**

```chali
obj.multiply(other);
```

**源码**：[source/std/linear_algebra.chl:224](../../source/std/linear_algebra.chl#L224)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-43"></a>

### Matrix::apply（44）

**声明**

```chali
Vector<T> apply(Vector<T> vector);
```

**行为**

计算矩阵与向量的乘积。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `vector` | `Vector<T> vector` | 输入向量 | 无 |

**返回值**

`Vector<T>`。计算矩阵与向量的乘积。

**错误与边界**

遵循本页“公共约定与错误边界”。
显式前置检查：`self.cols == vector.len(), "Matrix.apply 维度不匹配"`。

**调用形式**

```chali
obj.apply(vector);
```

**源码**：[source/std/linear_algebra.chl:240](../../source/std/linear_algebra.chl#L240)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-44"></a>

### Matrix::solve（45）

**声明**

```chali
Vector<T> solve(Vector<T> rhs);
```

**行为**

解线性方程组，返回独立解向量；奇异或维度不符时报错。

带部分主元的 LU 求解；复制系数矩阵，不修改调用者。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `rhs` | `Vector<T> rhs` | 线性方程右端 | 无 |

**返回值**

`Vector<T>`。解线性方程组，返回独立解向量；奇异或维度不符时报错。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.solve(rhs);
```

**源码**：[source/std/linear_algebra.chl:253](../../source/std/linear_algebra.chl#L253)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-45"></a>

### Matrix::leftAdd（46）

**声明**

```chali
Matrix<T> leftAdd(Matrix<T> right);
```

**行为**

实现左操作数加法接口。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `right` | `Matrix<T> right` | 右操作数或区间右端 | 无 |

**返回值**

`Matrix<T>`。实现左操作数加法接口。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.leftAdd(right);
```

**源码**：[source/std/linear_algebra.chl:258](../../source/std/linear_algebra.chl#L258)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-46"></a>

### Matrix::leftSub（47）

**声明**

```chali
Matrix<T> leftSub(Matrix<T> right);
```

**行为**

实现左操作数减法接口。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `right` | `Matrix<T> right` | 右操作数或区间右端 | 无 |

**返回值**

`Matrix<T>`。实现左操作数减法接口。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.leftSub(right);
```

**源码**：[source/std/linear_algebra.chl:259](../../source/std/linear_algebra.chl#L259)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-47"></a>

### Matrix::leftMul（48）

**声明**

```chali
Matrix<T> leftMul(Matrix<T> right);
```

**行为**

实现左操作数乘法接口。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `right` | `Matrix<T> right` | 右操作数或区间右端 | 无 |

**返回值**

`Matrix<T>`。实现左操作数乘法接口。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.leftMul(right);
```

**源码**：[source/std/linear_algebra.chl:260](../../source/std/linear_algebra.chl#L260)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-48"></a>

### Matrix::leftMul（49）

**声明**

```chali
Vector<T> leftMul(Vector<T> right);
```

**行为**

实现左操作数乘法接口。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `right` | `Vector<T> right` | 右操作数或区间右端 | 无 |

**返回值**

`Vector<T>`。实现左操作数乘法接口。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.leftMul(right);
```

**源码**：[source/std/linear_algebra.chl:261](../../source/std/linear_algebra.chl#L261)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-49"></a>

### Matrix::leftMul（50）

**声明**

```chali
Matrix<T> leftMul(T right);
```

**行为**

实现左操作数乘法接口。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `right` | `T right` | 右操作数或区间右端 | 无 |

**返回值**

`Matrix<T>`。实现左操作数乘法接口。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.leftMul(right);
```

**源码**：[source/std/linear_algebra.chl:262](../../source/std/linear_algebra.chl#L262)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-50"></a>

### lu_decompose（51）

**声明**

```chali
LuFactorization lu_decompose(Matrix<float64> input);
```

**行为**

计算带主元置换的 LU 分解。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `input` | `Matrix<float64> input` | 输入矩阵、信号或数据值 | 无 |

**返回值**

`LuFactorization`。计算带主元置换的 LU 分解。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::linear_algebra::lu_decompose(input);
```

**源码**：[source/std/linear_algebra.chl:293](../../source/std/linear_algebra.chl#L293)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-51"></a>

### lur_decompose（52）

**声明**

```chali
LuFactorization lur_decompose(Matrix<float64> input);
```

**行为**

计算 LUR 分解结果。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `input` | `Matrix<float64> input` | 输入矩阵、信号或数据值 | 无 |

**返回值**

`LuFactorization`。计算 LUR 分解结果。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::linear_algebra::lur_decompose(input);
```

**源码**：[source/std/linear_algebra.chl:301](../../source/std/linear_algebra.chl#L301)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-52"></a>

### symmetric_eigen（53）

**声明**

```chali
EigenDecomposition symmetric_eigen(Matrix<float64> input);
```

**行为**

计算实对称矩阵的特征分解。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `input` | `Matrix<float64> input` | 输入矩阵、信号或数据值 | 无 |

**返回值**

`EigenDecomposition`。计算实对称矩阵的特征分解。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::linear_algebra::symmetric_eigen(input);
```

**源码**：[source/std/linear_algebra.chl:314](../../source/std/linear_algebra.chl#L314)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-53"></a>

### svd（54）

**声明**

```chali
SingularValueDecomposition svd(Matrix<float64> input);
```

**行为**

计算奇异值分解。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `input` | `Matrix<float64> input` | 输入矩阵、信号或数据值 | 无 |

**返回值**

`SingularValueDecomposition`。计算奇异值分解。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::linear_algebra::svd(input);
```

**源码**：[source/std/linear_algebra.chl:328](../../source/std/linear_algebra.chl#L328)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-54"></a>

### jordan2（55）

**声明**

```chali
JordanDecomposition jordan2(Matrix<float64> input);
```

**行为**

计算 2×2 矩阵的 Jordan 分解。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `input` | `Matrix<float64> input` | 输入矩阵、信号或数据值 | 无 |

**返回值**

`JordanDecomposition`。计算 2×2 矩阵的 Jordan 分解。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::linear_algebra::jordan2(input);
```

**源码**：[source/std/linear_algebra.chl:341](../../source/std/linear_algebra.chl#L341)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。

<a id="api-55"></a>

### diagonalize2（56）

**声明**

```chali
JordanDecomposition diagonalize2(Matrix<float64> input);
```

**行为**

将可对角化的实数 2×2 矩阵对角化；不满足条件时报错。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `input` | `Matrix<float64> input` | 输入矩阵、信号或数据值 | 无 |

**返回值**

`JordanDecomposition`。将可对角化的实数 2×2 矩阵对角化；不满足条件时报错。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::linear_algebra::diagonalize2(input);
```

**源码**：[source/std/linear_algebra.chl:348](../../source/std/linear_algebra.chl#L348)。

**完整示例**：[example/samples/std_linalg_solve_glue.chl](../../example/samples/std_linalg_solve_glue.chl)。
