# 附录 C 语法与操作符

<!-- LANGUAGE NAVIGATION -->
[English](en/C-syntax-operators.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

这一附录供读完入门章节后回查。先按想做的事情找写法，再回正文看完整例子；不需要把表格当作第一遍阅读的顺序。

## C.1 名字、字面量与基础类型

标识符区分大小写。// 注释到行尾，/* ... */ 注释一段。字符串使用双引号，char 使用单引号；转义包括 \n、\t、\" 和 \\。数字的具体类型既受后缀影响，也受所在位置的期望类型影响，见第 3 章。

| 类型 | 常见用途 |
| --- | --- |
| int8、int16、int32、int64 | 有符号整数，数字表示位宽 |
| uint8、uint16、uint32、uint64 | 无符号整数 |
| float32、float64 | 浮点数 |
| bool、char、void | 逻辑值、字符、无结果 |
| String | 拥有 UTF-8 字节的字符串 |

true 与 false 是布尔字面量。普通格式化可能将它们显示为 1、0；JSON 需要 true、false 时显式生成相应文字。

## C.2 声明、函数与类型形状

T name = value 声明有明确类型的变量，let name = value 推导类型，const T name = value 声明常量。顶层声明属于模块，块内声明属于所在作用域。type 声明宿主不透明类型，不是任意基础类型间的透明别名。

| 写法 | 阅读方法 | 正文 |
| --- | --- | --- |
| int64 add(int64 a, int64 b) | 接收两个整数并返回整数 | 第 4 章 |
| fn add(int64 a, int64 b) -> int64 | 同一签名的返回类型后置形式 | 第 19 章 |
| static T create(...) | 类型级具名工厂，可用 return self | 第 6、18 章 |
| R fn(A, B) | 函数值类型 | 第 19 章 |
| T mut&、T const&、T & | 可变引用、只读引用、只读引用简写 | 第 8 章 |
| (A, B) 与 value.0 | 元组类型与按位置访问 | 本附录 |
| T values[2][3] | 两行三列定长数组 | 第 13 章 |
| T values[] = new[3] | 运行时长度数组 | 第 13 章 |

默认参数放在函数声明中，调用省略时使用默认值。普通变长尾参写 T... values；宏的 ...values 是语法捕获，不能与之混为一谈。main 返回 void，接收零参数或 List<String> 程序参数。

## C.3 操作符按用途查找

| 用途 | 操作符 |
| --- | --- |
| 算术 | +、-、*、/、% |
| 比较 | ==、!=、<、<=、>、>= |
| 逻辑与短路 | !、&&、\|\| |
| 位运算 | &、\|、^、~、<<、>> |
| 赋值 | =、+=、-=、*=、/=、%=、&=、\|=、^=、<<=、>>= |
| 自增与自减 | ++、-- |
| 成员与调用 | .、::、(...)、[...] |
| 所有权与传播 | move value、result? |
| 显式转换 | cast<T>(value)、(T)value |

优先级从高到低可按一元、乘除取模、加减、移位、关系、相等、位与、位异或、位或、逻辑与、逻辑或记忆。不确定时给表达式加括号。&& 与 || 短路，不是可重载的普通二元接口。

同一个完整表达式中，自增减目标不能再被其他操作读取或写入，例如 i = i++ 会被拒绝。复合赋值的目标位置只计算一次，a[index()] += 1 不会因此调用两次 index。

结构体的算术和比较通过 Add、Less 等接口实现。RightAdd 等接口处理显式反向操作，#NoRightAdd 与 #NoAddReplace 控制推导，详见第 17、24 章及语言参考。不要按 C 的隐式规则猜枚举或结构体比较。

## C.4 循环、匹配与退出

if、elif、else 选择分支，else if 与 elif 等价。while(condition) 在每轮前检查条件；loop 直接开始下一轮，适合没有条件的重复。习题里的无限循环优先写 loop，而不必写 while(true)。

for (T value in sequence) 通过迭代协议取值。range 的上界不包含在内，rangeEq 包含。match 按枚举变体、常量、区间、接口具体类型或联合模式选择分支，通配写 _，需要保留值时可绑定变量。

break 与 continue 默认影响最近循环；break 另外支持 level 指定层数和 all 退出全部循环，continue 不提供层数标注。循环的 else 为未取得结果的路径提供值；带值退出与 yield 要按所在结构区分。return 总是针对当前函数，不能把块的 yield 当作函数返回。

下面把较少使用的控制表达式放在完整函数中，避免在初学函数时同时引入所有结构：

<!-- verify-book -->
```chali
int64 fromBlock()
{
    return
    {
        int64 local = 3;
        yield local;
    };
}

int64 choose(bool first)
{
    return if (first)
    {
        yield 5;
    }
    else
    {
        yield 6;
    };
}

int64 fromMatch(int64 item)
{
    return match (item)
    {
        0 => { yield 7; }
        _ => { yield 8; }
    };
}

int64 fromLoop()
{
    return loop
    {
        yield 9;
    };
}

void main()
{
    println("{} {} {} {}", fromBlock(), choose(false), fromMatch(0), fromLoop());
}
```

```text
3 6 7 9
```

## C.5 更深的函数类型

掌握一层 R fn(A) 以后，再把 A 换成另一个函数类型。下面的例子不是常用 API 的推荐形状，而是用来练习从内向外读类型。

先看 level1 做 +1，level2 把 10 交给回调。接着 level3、level4、level5 逐层传递函数值。hello 最后把得到的 11 交给 finish；finish 读取 factor=2，输出 22。传递函数名本身不调用它，只有括号调用才执行。

<!-- verify-book -->
```chali
int64 level1(int64 value)
{
    return value + 1;
}

int64 level2(int64 fn(int64) callback)
{
    return callback(10);
}

int64 level3(int64 fn(int64 fn(int64)) callback)
{
    return callback(level1);
}

int64 level4(int64 fn(int64 fn(int64 fn(int64))) callback)
{
    return callback(level2);
}

int64 level5(int64 fn(int64 fn(int64 fn(int64 fn(int64)))) callback)
{
    return callback(level3);
}

int64 hello(
    int64 fn(int64 fn(int64 fn(int64 fn(int64 fn(int64))))) pipeline,
    int64 fn(int64) finish)
{
    return finish(pipeline(level4));
}

void main()
{
    int64 factor = 2;
    int64 finish(int64 value)
    {
        return value * factor;
    }
    println("{}", hello(level5, finish));
}
```

```text
22
```

## C.6 泛型、接口与匹配的组合

类型参数写在尖括号内，常量参数可以决定数组形状，约束说明所需接口，关联类型由实现提供。泛型闭包保留自己的捕获环境，特化不等于运行时替换函数。

Result<A,B> 的两个标签独立匹配。Ok(AType a) 筛选成功载荷的具体实现，Err(E e) 筛选错误载荷；Err(_) 不绑定剩余值。单载荷时支持 Err::IoError(e)，解析唯一时也可写 IoError(e)。多载荷必须保留变体与括号内的类型标注，如 Failed(code, IoError e)。第 14 章介绍基本传播，第 17 章解释接口关系。

? 从成功分支取得值，否则按传播协议提前返回。具体错误可提升到已实现的 Error 接口，Option 和自定义 Try 仍需满足各自协议；载荷叫 Error 不代表外层一定处于 Err。

## C.7 宏、注解与编译器指令

$macro ... $end_macro 与 $replace 定义宏；调用带 $ 前缀。value、expr、ident、type、stmt、block、pattern 约束输入类别，-> expr、-> stmt、-> item、-> member 约束输出位置。<T:#res> 从结果上下文推导，<U:b> 从实参静态类型推导。

$for、$if、$match、$regex 选择或重复模板；$fields 与 $member 静态生成字段操作，$compile_assert 与 $compile_error 报告编译时条件。完整例程及逐段解释见第 22 章。

%define、%symbol 及 %if、%ifdef、%ifndef、%elif、%else、%endif 在预处理阶段决定保留哪些源码；%undefine、%unsymbol 撤销定义，%warning、%error 给出消息。它们不等于运行期 if，也不等于类型检查后的宏字段选择。

@annotation 声明注解，@Name 标注声明；@Reflect 选择运行期反射信息。#derive、#borrow、#static、#abi、#export、#NonCopyable 等编译器指令各有具体约束，不以 # 前缀自动成为用户宏。

## C.8 不安全操作与宿主边界

unsafe 只允许使用指定的原始操作，不取消所有权与类型规则。address、rawSize、memcpy、memset 处理的地址、大小、对齐和存活范围需要逐项明确。原始指针不是能随意解引用的安全引用。

#abi("C", library, symbol) 声明 C ABI 入口。指针、句柄、字符串与结构体的表示必须按 ABI 约定准备，语言函数值不能直接当作机器函数指针。#export 为原生产物暴露接口，部署时仍需准备应用自己的动态库依赖。

完整限制见第 25、26、27 章。遇到具体签名时回到参考文档核对，不把某个平台验证过的动态库名称当作跨平台名称。
