# std::macro — 标准宏

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](std-macro.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

[返回库目录](README.md) · [全部符号](symbols.md)

## 引入

```chali
import std::macro;
```

## 公共约定与错误边界

包含选择、区间限制和标准值协议成员宏，见标准库总览。

## 参考代码

聚合入口的具体操作参见对应子模块；本页类型/方法的调用形式列在下方。

## 宏定义

<a id="symbol-334"></a>

### `MIN`

```chali
$macro MIN(left:value, right:value) -> expr
```

返回两个可比较值中较小的值。每个实参求值一次，相等时返回左值。

<a id="symbol-335"></a>

### `MAX`

```chali
$macro MAX(left:value, right:value) -> expr
```

返回两个可比较值中较大的值。每个实参求值一次，相等时返回左值。

<a id="symbol-336"></a>

### `CLAMP`

```chali
$macro CLAMP(value:value, low:value, high:value) -> expr
```

将值限制在闭区间内。每个实参求值一次，上界小于下界时触发断言。

<a id="symbol-337"></a>

### `Equal`

```chali
$macro Equal<T>(method:ident) -> member
```

为类型 T 生成调用方命名的相等方法，按声明顺序比较实例字段或匹配枚举载荷。

<a id="symbol-338"></a>

### `Hash`

```chali
$macro Hash<T>(method:ident) -> member
```

为类型 T 生成调用方命名的哈希方法，按声明顺序混合字段或枚举载荷。

<a id="symbol-339"></a>

### `Clone`

```chali
$macro Clone<T>(method:ident) -> member
```

为类型 T 生成调用方命名的克隆方法。嵌套拥有值沿用其克隆协议。

<a id="symbol-340"></a>

### `Debug`

```chali
$macro Debug<T>(method:ident) -> member
```

为类型 T 生成调用方命名的调试显示方法，保留字段和枚举载荷的声明顺序与文本转义。

## 函数索引

| 名称 | 声明 |
| --- | --- |

## 补充完整示例

[example/samples/std_macro.chl](../../example/samples/std_macro.chl)。

```chali
import std::macro;

int64 calls = 0;
int64 next(int64 value) {
    calls++;
    return value;
}
void main() {
    println("{} {} {}", $std::macro::MIN(next(7), next(3)), $MAX(7, 3), $CLAMP(9, 0, 6));
    println("{}", calls);
}
```

## 补充完整示例

[example/samples/std_macro_value_members.chl](../../example/samples/std_macro_value_members.chl)。

```chali
import std::macro;

struct Leaf {
    int64 number;
    String text;
    $std::macro::Equal<Leaf>(leftEqual);
    $std::macro::Hash<Leaf>(hash);
    $std::macro::Clone<Leaf>(clone);
}
struct Box<T> {
    T value;
    $std::macro::Equal<Box<T>>(leftEqual);
    $std::macro::Hash<Box<T>>(hash);
    $std::macro::Clone<Box<T>>(clone);
}
struct Fraction {
    float64 value;
    $std::macro::Equal<Fraction>(leftEqual);
    $std::macro::Hash<Fraction>(hash);
}
enum Choice {
    Empty, Value(Leaf item, String label, int64 count);
    $std::macro::Equal<Choice>(leftEqual);
    $std::macro::Hash<Choice>(hash);
    $std::macro::Clone<Choice>(clone);
}
void main() {
    Box<Leaf> original;
    original.value.number = 7;
    original.value.text = "value";
    Box<Leaf> duplicate = original.clone();
    assert(original == duplicate);
    assert(original.hash(original) == duplicate.hash(duplicate));
    duplicate.value.text = "different";
    assert(!(original == duplicate));
    Fraction positive; positive.value = 0.0;
    Fraction negative; negative.value = -0.0;
    assert(positive == negative);
    assert(positive.hash(positive) == negative.hash(negative));
    Choice choice = Choice::Value(original.value, "label", 9);
    Choice copy = choice.clone();
    assert(choice == copy && choice.hash(choice) == copy.hash(copy));
    assert(!(choice == Choice::Empty));
    assert(Choice::Empty.clone() == Choice::Empty);
    match (copy) {
        Choice::Value(item, label, count) => { assert(item == original.value && label == "label" && count == 9); }
        _ => { assert(false); }
    }
    println("{} {}", original.value.number, original.value.text);
}
```
