# 库包格式与资产检查

<!-- LANGUAGE NAVIGATION -->
[English](en/44-library-packages.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

`.chlib` 是保存语言接口、目标实现及依赖身份的单文件容器。`chali lib template` 生成规范清单，`pack` 封装已有文件，`inspect` 核对格式并输出 JSON。普通 `import` 可加载 `c-v1` 库包接口及原生对象／归档。格式命令不编译语言实现，也不执行库初始化；接口的语言语义在消费者编译时检查。

## 生成与检查

先准备 `public.chl` 和一个现有原生归档。例如接口可以声明最终链接提供的 C 符号：

```chali
module example::math;
#abi("C", "@link", "add")
int64 add(int64 a, int64 b);
```

下面以 Windows x64 为例，`build` 目录须已存在：

```sh
chali ar build/add.obj --out build/libmath.a
chali lib template --module example::math --package-version 1.2.3 --target windows-x64 --license Apache-2.0 --out build/math.manifest.json
chali lib pack --manifest build/math.manifest.json --entry "interface:interface/main.chl=public.chl" --entry "archive:native/x86_64-w64-windows-gnu/library.a=build/libmath.a" --out build/math.chlib
chali lib inspect build/math.chlib --module example::math --package-version 1.2.3 --target windows-x64
```

`template` 不给 `--out` 时输出规范单行 JSON。目标使用规范三元组，清单内的实现名与示例中的归档条目名一致。`pack` 读取这份规范清单；清单的字段顺序、集合排序与 JSON 编码属于格式的一部分，输出末尾允许 CR／LF 换行。资产参数是 `KIND:NAME=FILE`：`NAME` 是包内名字，`FILE` 是真实输入路径，二者分别处理。可重复指定资产，实际封装顺序按名字排序。

`inspect` 的报告包含整包 SHA-256、清单和每条资产的名字、种类、长度、负载 SHA-256。它不打印源文或二进制负载。`--module`、`--package-version` 可核对预期身份，`--target`／`--cpu` 可核对原生选择。不给目标时可以检查含交叉目标的包；检查成功不表示已在目标系统运行。

## 从语言函数构建原生库

`lib build` 将已检查函数直接生成 LLVM 代码并编译成独立对象，再与自动生成的公开接口一起封装。以首批支持的标量函数为例：

```chali
module example::math;
inline int64 adjust(int64 value) {
    return value + 1;
}
int64 add(int64 a, int64 b) {
    return adjust(a) + b - 1;
}
```

`adjust` 的 `inline` 声明使它只在模块内可见；公开接口只包含 `add` 的签名。调用方不会重新编译它的实现。包的目录应按完整模块名排列：

```sh
chali lib build math.chl --module example::math --package-version 1.0.0 --out modules/example/math.chlib
chali lib inspect modules/example/math.chlib
```

输出目录须已存在，入口实际模块必须与 `--module` 一致。包内保存公开接口和当前目标对象，不保存实现源码或独立运行时副本；最终应用提供同一套运行时。公开符号由模块、签名及 ABI 身份计算，内部函数不向消费者公开。`--target`／`--cpu` 选择实现目标，跨目标生成仍须按实际目标验证。

当前构建范围包括普通函数的 void／bool／char、各宽度有符号和无符号整数、float32／float64，以及下文的受支持的普通记录、枚举和标准 String 参数／返回值，函数体支持标量运算、局部变量、控制流、当前单元内直接调用、递归及已导入库的受支持直接调用。C ABI 入口直接由 LLVM 生成位宽和浮点转换。函数入口仍明确拒绝尚未支持的上述 String 及其拥有字段聚合之外的拥有值、全局状态、用户方法、变参、unsafe 导出、一般宿主及地址操作，以及构建期步数预算，不会忽略这些声明或把它们改为标量。String 的标准实现可使用已检查的内部存储操作；泛型定义的消费见下文；初始化与更广的语言内部 ABI 仍按各自支持范围检查。

### 标准 String 的语言库入口

独立语言库支持标准 `std::lang::String` 按值参数／返回及只读／可写借用。例如：

```chali
module example::text;
String copy(String value="默认值") { return value; }
int64 size(String const& value) { return len(value); }
void append(String mut& value) { value=value+"!"; }
#borrow(#ret,[value])
String const& view(String const& value) { return value; }
```

入口采用 `chali-owned-v1` 原生 ABI、`owned-string-value-v1` 必需能力和私有 `ChaLiOwned` 声明。传递的是同一程序共享堆中的逻辑句柄或 String 槽位地址；库不包含独立分配器或运行时。按值形参沿用语言规则建立独立副本，返回值把拥有关系交给调用方，可写借用则修改调用方原槽位。默认参数在定义模块中按调用时求值；借用返回必须显式绑定受支持的借用形参并保留只读／可写约束。UTF-8 内容、内联及独立缓冲区继续使用标准 String 的复制、移动和释放实现。

接口与能力须来自当前目标已验证的包；普通源码不能自行声明 `ChaLiOwned`，旧编译器明确拒绝未知能力。该约定可以与已有标量、非拥有记录／枚举及传递依赖混用，支持原生、bitcode 和适用的 Full／ThinLTO；是否受益取决于负载。String 入口不能同时声明物理 C ABI 的 `#export`、释放函数或数组长度约定。用户同名 String、其他拥有类型、拥有字段、接口／闭包入口和库全局初始化不因该能力而获得支持。带生命周期契约的外部函数仍须直接调用。

### String 字段的拥有记录与枚举

普通记录及枚举还可包含标准 String 字段／载荷，并嵌套同样受支持的记录和枚举。例如：

```chali
module example::messages;
struct Message {String text;int64 sequence;};
enum Event {Empty,Text(String text),MessageValue(Message value)};
Message changed(Message value) { value.text=value.text+"!"; return value; }
#borrow(#ret,[value])
String const& textView(Message const& value) { return &value.text; }
```

此类包额外声明 `owned-record-value-v1`，并要求 `owned-string-value-v1` 及 `shared-type-id-v1`，沿用 `chali-owned-v1`／`ChaLiOwned`。只支持 String 句柄的旧构件会拒绝新能力；移除聚合能力后，接口也不能借用 String 能力传递拥有字段记录。基础字段、String 和嵌套布局以及枚举变体顺序参与完整模块身份摘要，改变布局后旧消费者对象无法链接新实现；同名类型及消费者不同的类型编号分配顺序不会混用。

按值参数沿用语言深复制规则，修改副本不影响调用方；返回值、显式移动及递归转发保持拥有关系。嵌套 String 使用同一个程序的分配器、堆与共享生命周期类型身份；枚举只复制、移动和销毁活动变体。只读／可写借用直接访问调用方存储，借用返回仍须显式来源契约，不能逃逸局部对象。原生对象、bitcode、适用的 Full／ThinLTO 和分离链接使用相同规则，不生成另一套运行时。

当前字段仍限基础值、标准 String 和同样受支持的普通记录／枚举，不包含集合、数组、接口、闭包、File 等其他资源。用户方法／构造器、字段默认初始化器、自定义布局和运算符／析构契约仍拒绝；聚合默认参数和物理 C ABI 的 `#export` 不在此约定内。该能力不启用库全局初始化。String 及其借用作为独立参数时继续使用前一节约定。

### 可选原生调试信息

`lib build --debug` 在对象及可选 bitcode 中携带 O0 DWARF 和独立源码目录；`--debug-optimized` 携带 O3 调试信息。不指定调试选项时不生成这些目录。包仍不携带实现源码，也不包含另一份运行时。

```sh
chali lib build math.chl --module example::math --package-version 1.0.0 --debug --with-bitcode --out modules/example/math.chlib
chali aot main.chl --debug --library-implementation native --out build/application-debug
```

`extensions.nativeDebug` 按目标保存版本、模块目录前缀、优化状态及原生／bitcode 负载 SHA-256。读取包时核对实际负载；消费者只从已验证快照登记所选实现，最终链接从当前快照另行生成只读实现身份对象。调试器核对目录的模块、优化状态及最终实现摘要；旧消费者对象重新链接新库时，身份不符会禁用旧目录的类型化查看并明确提示重新编译，DWARF 和底层反汇编仍保留。符号和实现不能通过修改一个路径而分离；最终映像身份检查继续适用于启动、附加和远程连接。这些摘要建立内容一致性，不提供发行者签名认证。

应用使用 `--debug` 或 `--debug-optimized` 保留最终调试信息。各库按自己的优化状态展示；O0 原生库可用于 O3 应用，O3 库也可用于 O0 应用。O0 调试库的 bitcode 若将由 O3 消费者重新优化，则明确拒绝；选择 `native` 或用 `--debug-optimized` 重建库，不能把重新优化后的存储误当成可写 O0 帧槽。无调试的发布应用仍按原规则剥离符号。

VSCode 的 `sourceFileMap` 对库和应用使用同一套路径前缀映射。源码缺失时保留 DWARF 文件名、行号、实际函数及指令地址；变量以仍可读取的位置为准，反汇编仍可用，源码视图提示缺失并标出模块、版本、所选实现及其摘要。不会编造源码或重建已优化变量。调试目录总读取预算由所有库共享；具体配置和限制见[原生调试](43-native-debugging.md)。

标量函数的默认参数保留定义模块作用域和调用时求值。例如：

```chali
inline int64 defaultValue() { return 7; }
int64 addDefault(int64 x, int64 y=defaultValue()) { return x+y; }
```

导入后 `example::math::addDefault(3)` 在本次调用缺省 `y` 时才求默认值；`addDefault(3,9)` 不求默认值。生产器将已检查表达式编译成独立入口，公开接口只保存该入口的声明和调用，不包含私有函数体，也不会在导入、构建时执行默认表达式。多个缺省参数保持原有求值次序，私有函数和已导入库调用仍在定义模块内解析。默认值支持基础标量及上文的标准 String；全局状态、其他拥有值与未接入操作不会因出现在默认值中而获得支持。生成的默认值入口是实现辅助声明，不作为需要手工调用的标准 API。

公共接口也保留普通记录声明。字段可为基础标量、标准 String，或本模块及已验证预编译依赖中的受支持嵌套普通记录，保留字段名、顺序和完整模块身份。例如提供方可声明：

```chali
struct Point { int64 x; float64 y; };
struct Bounds { Point low; Point high; char marker; };
```

导入后可以声明 `example::math::Bounds bounds;`，通过 `bounds.low.x` 访问嵌套字段，并进行独立值复制。接口使用原始字段结构生成声明，消费者按相同语言版本重建布局，不把摊平字段名写回源语法。默认生成的语言方法由消费者重建；用户方法、构造器、字段默认初始化器、其他资源字段、自定义布局和接口继承契约仍拒绝。当前仍需至少一个受支持的公开函数；参数可为基础标量、普通记录按值参数或下文的借用，返回值可为基础标量、标准 String、void 或受支持的普通记录；String 拥有字段使用上文约定，其他拥有值与完整聚合调用仍待接入。不同模块中同名记录继续是不同类型。

公开枚举支持无载荷、单载荷及多载荷分支；载荷可为基础标量、标准 String 或同样受支持的记录／枚举。例如：

```chali
enum Event {
    Empty,
    Count(int64 value),
    Pair(int64 left, float64 right),
    At(Point position)
};
```

声明保留分支顺序、原始载荷字段名及完整类型名，消费者可以构造值并使用 `match`；无载荷分支用 `Event::Empty`，多载荷分支用 `Event::Pair(3,4.5)`。记录可包含来自预编译依赖的枚举，依赖声明仍由原模块提供。非拥有枚举的库函数入口见下文；String 拥有载荷使用上文约定，其他拥有载荷、用户方法和完整接口契约仍未接入。

非拥有枚举支持按值参数、返回值以及只读／可写借用，包括空载荷、多载荷、嵌套记录和嵌套枚举。限定构造和匹配按实际声明模块解析，消费者中的同名类型不会遮蔽全限定名称：

```chali
Event echo(Event value) { return value; }
void replace(Event mut& value) { value = Event::Count(77); }
```

按值调用沿用 `chali-value-v1`／`ChaLiValue`，借用沿用 `chali-borrow-v1`／`ChaLiBorrow`。这些包额外声明 `plain-enum-value-v1` 必需能力，旧编译器在包检查时明确拒绝未知能力。传递的是 ChaLi 聚合逻辑地址，标签和载荷保持语言布局；按值副本的修改不影响调用方，可写借用的修改则写回原值。返回、递归和跨库转发共用最终程序的分配器及生命周期类型身份。枚举的完整模块身份、字段布局和变体顺序进入符号摘要，改变布局或变体编号后，旧消费者对象不能用原符号链接新包。String 拥有载荷的枚举使用上文约定；其他拥有载荷、枚举默认参数和物理 C ABI 的 `#export` 不在此约定内；借用返回遵循下文的生命周期契约。

普通非拥有记录也可以作为只读或可写借用参数进入已编译语言单元：

```chali
int64 read(Point const& value) { return value.x; }
void bump(Point mut& value, int64 delta) { value.x = value.x + delta; }
```

这些入口直接传递调用方的 ChaLi 逻辑地址，在最终程序的同一内存与检查上下文中访问字段，不转成宿主 C 结构体，也不生成 C++ 参数封送。普通记录返回采用下文的数据重定位约定，函数体限当前已支持的标量控制流、字段访问和直接调用；标准 String 及其拥有字段聚合之外的拥有值、聚合引用默认值和库全局状态尚未接入；借用返回见下文的生命周期契约。只读及可写约定继续由语言检查，已有标量参数可与记录借用混用。含语言借用参数的函数不能同时使用面向物理 C 布局的 `#export`。

这类包使用 `chali-borrow-v1` 原生 ABI 和 `borrowed-record-v1` 必需能力。生成接口的 `ChaLiBorrow` 标记只允许出现在匹配 ABI 的已验证库来源中，不能复制为普通源码声明。符号包含完整记录及嵌套字段布局摘要；独立旧对象遇到布局已改变的新包会因缺少原符号而拒绝链接。普通 C ABI 包继续使用 `c-v1`；引用传递与跨库拥有对象、闭包及动态函数值的完整契约分别验收。

抽象接口声明支持基础标量参数／返回值（含 void 返回）、静态协议方法和继承关系。基接口须来自同一模块或已验证的预编译依赖；生产接口保留原继承关系，跨模块基接口同时登记导入，避免消费者同名声明改变解析。例如：

```chali
interface Error { int64 code(); };
interface DetailedError : Error { bool retry(); };
```

消费者可实现这些接口、通过基接口调用方法，并使用 `IoError(error)` 这样的接口类型匹配。声明保持原方法参数名及静态标记，不把继承的方法重新定义成派生接口的方法。生产器仍拒绝接口默认方法体、关联类型和非标量方法签名；在库内创建接口实例或跨原生函数入口传递接口值仍待共用运行时接入。

跨模块嵌套记录须来自已验证的 `.chlib` 依赖，字段使用原声明模块的完整名称。生产包保留精确依赖，不把外部记录复制成自身模块的新类型；消费者即使只导入外层库，也会恢复原声明包。只来自实现源码的跨模块记录字段会被拒绝，应先将该依赖预编译。

导入的预编译库会以精确版本和内容摘要记入生产包，最终消费者恢复完整依赖闭包。源码 `#export("native_name")` 保留额外的原生入口，普通语言导入仍使用包含模块及完整签名的稳定入口；`inline` 私有函数不对其他模块公开；含泛型或宏的包可保存模块私有 ABI 声明供定义模块使用。原生入口存在不等于最终动态库自动导出它，最终链接器的导出设置仍需单独指定。

`lib build` 也可直接附带交付信息：

```sh
chali lib build math.chl --module example::math --package-version 1.0.0 --out modules/example/math.chlib --license Apache-2.0 --license-asset legal/LICENSE.txt=LICENSE --license-asset legal/NOTICE.txt=NOTICE --runtime-library example-plugin.dll
```

`--license-asset` 可重复，等号左边是包内名字，右边是输入文件；许可证与署名按原始字节随包保存。名字、重复、长度预算及普通文件检查在代码编译前进行，输出不能覆盖这些输入；读取后使用同一快照，失败保留原包。`--system-library NAME` 声明最终链接所需的库，消费者链接时加入；名称必须来自目标 SDK，例如 Windows 的 `kernel32` 或适用 Linux 目标的 `m`；`--runtime-library` 记录应用动态依赖，不自动复制或加载该文件。示例中的插件名只是交付记录，需要按应用实际依赖填写。

`extensions.producer` 记录 AST→LLVM、O3、checked 设置、实际静态／外置 LLVM 后端和由最终消费者提供运行时的约定，不含构建机绝对路径。精确语言库依赖继续使用清单 `dependencies`，不把生产编译器的 LLVM 工具链写成应用动态依赖。运行依赖名称是发布者声明，工具不会据此声称已扫描出所有动态加载路径。

此类包声明 `scalar-unit-v1` 必需能力，缺少该能力的编译器会在加载前拒绝。普通导入既可消费这种语言生成对象，也可消费前节手工声明的 C ABI 归档；两者的接口均不包含运行期函数体。

## 泛型定义与宏上下文

`lib build` 将本模块未实例化的泛型定义放入 `templates/definitions.json`，并声明 `generic-definitions-v1` 必需能力。旧编译器会明确拒绝该能力。普通函数仍编译为目标实现；不再把生产器内部偶然用到的泛型实例作为普通公开函数声明导出。仅含泛型或宏定义的模块也可构建库包。

```chali
module example::generic;
T identity<T>(T value) { return value; }
T defaulted<T = int64>(T value) { return value; }
struct Box<T> { T value; T read() { return self.value; } }
```

消费者移走原始实现文件后，仍可导入该包并使用 `example::generic::identity<String>("text")`、`example::generic::defaulted<>(42)` 和 `example::generic::Box<int64>`。实例化发生在消费者编译会话，保留完整模块身份、形参约束、默认类型及编译期整数实参；递归和所有权检查沿用源码路径。同名模块中的模板不合并为同一名义类型。

格式一的规范 JSON 保存模块、导入、宏定义及泛型声明；每个泛型另存形参、派生请求和运算符开关。宏实参恢复为可重新捕获的原文，不保存编译会话编号或实现文件绝对路径。消费者重新解析这些声明及宏；它们是编译所需的定义，不是整个实现源码附件。未知字段、版本、模块不符、重复定义、非模板普通实现、宏外普通代码及内部片段编号均拒绝。载荷总预算为 32 MiB，每个声明最多 4 MiB，泛型与宏各最多 4096 项。

模板的非标准模块导入必须有精确版本和整包摘要的预编译依赖；标准模块使用当前编译器内嵌资产。不能用生产机上恰好存在的 `.chl` 文件代替依赖身份。需要调用的私有普通入口以模块私有的 ABI 声明保存，调用和取得函数值仍核对原可见性，默认参数保留定义模块语义。

这一能力允许消费者生成模板结构体的方法，不意味着原生函数入口已能传递所有泛型聚合、集合、接口或闭包。生产器已实例化的含用户方法聚合、全局初始化及更广的原生拥有值边界仍按各自支持范围检查。没有 `generic-definitions-v1` 能力的可选 Templates 资产不能作为语言定义消费。

## 普通导入

将上例的包放在 `modules/example/math.chlib`，消费者写：

```chali
import example::math;
void main() {
    int64 answer = example::math::add(20, 22);
    assert(answer == 42);
}
```

`chali --check consumer.chl` 检查公开声明；`chali aot consumer.chl --out build/consumer` 自动链接所选实现。提供方的实现源码不参与消费者编译。搜索依次经过当前单元目录、其 `modules/` 目录和 `-I` 目录；每个位置优先 `.chlib`，其次 `.chl`。已找到的库包若损坏或不兼容，直接报错，不回退到同名源码。标准库仍沿现有内嵌加载路径。

模块头按语句边界识别，`module`、多个 `import` 和后续声明可以位于同一行，例如 `module consumer;import example::math;void main(){assert(example::math::add(20,22)==42);}`。模块路径周围的空白、跨行和注释仍作为词法间隔处理；注释、普通字符串、字符和嵌套插值字符串里的伪导入不会加载包。头部声明必须位于普通声明之前，重复模块、保留模块名、缺失或损坏的包均明确拒绝。工程清单增加导入时只抽取头部，保留同一行的后续代码及原始诊断行列，不要求用户靠换行触发原生包加载。

当前原生接口允许类型与函数声明，函数入口使用显式 C ABI 或受验证的语言调用约定；不允许运行期函数体、全局变量或静态字段。接口内的类型、可见性和借用继续接受正常语言检查。泛型定义仅通过已验证的 `generic-definitions-v1` 载荷恢复；库初始化及上述 String／拥有字段聚合之外的语言内部 ABI 仍按各自支持范围检查。可选源码资产不会自动启用实现编译。

清单内的依赖即使未写在公开接口的 `import` 中也会解析，要求精确版本和整包 SHA-256。已找到的包会补充其完整模块根来搜索传递依赖，也可显式使用 `-I modules`；锁定依赖不接受 `.chl` 回退。重复模块必须具有同一摘要，循环导入拒绝。一轮编译最多加载 128 个包，总有效载荷最多 256 MiB。检查和链接使用同一份已验证的拥有型快照；导入不执行初始化。系统库随目标实现加入链接，运行时动态库仍由部署方提供。

库接口中的非标准模块导入必须出现在清单的精确依赖中，不能借用未锁定的源码模块补齐接口。

## 项目级内容锁与独立链接

为本项目选定的所有包生成独立锁，包含每个包的传递依赖：

```sh
chali lib lock modules/example/math.chlib --out chali.libraries.lock
chali check consumer.chl --library-lock chali.libraries.lock
chali aot consumer.chl --library-lock chali.libraries.lock --out build/consumer
```

有多个包时逐一传入 `lib lock`；清单要求的依赖必须同时提供且版本、摘要匹配。锁文件保存格式、编译器版本以及按模块排序的精确版本和整包 SHA-256，不保存机器上的绝对路径。再次生成同一集合得到相同字节。生成失败保留旧锁，输出禁止覆盖任何输入包。普通检查不会创建或修改锁。

`--library-lock` 对实际加载的每个库核对记录，未列入的包、不同版本和不同摘要拒绝。锁中指定的模块不得回退到 `.chl`；未列入锁的普通项目源码模块仍按正常规则导入。锁采用格式一的规范 JSON，最多 128 个模块、1 MiB，重复键、未知字段及不兼容版本拒绝。它与源码依赖的 `chali.lock` 分开，不改变原有源码包锁格式。

对象与汇编输出不会把库归档嵌进消费者。独立链接时通过 `--package` 提供原始包，链接器直接解析其清单和精确传递依赖：

```sh
chali aot consumer.chl --emit-obj --library-lock chali.libraries.lock --out build/consumer
chali link build/consumer.o --package modules/example/math.chlib --library-lock chali.libraries.lock --out build/consumer.exe
```

`--package` 可重复；`-L` 同时提供额外库根。独立链接不解析或执行接口源文，不启动初始化；原生实现从已验证快照恢复，系统库加入最终链接。包及锁文件不能成为链接输出。当前不自动生成链接依赖回执；调用方显式保留本次使用的包和锁。

## 独立的兼容信息

| 清单信息 | 核对规则 |
| --- | --- |
| 容器格式 | 文件头当前为 1；其他格式拒绝 |
| 接口格式、语言 ABI、运行时 ABI | 分别记录，当前各为 1；不互相代替 |
| 编译器版本 | 当前要求与 `VERSION` 完全相同，不静默加载较旧资产 |
| 模块与包版本 | 完整模块名和精确版本；依赖还包含整个依赖包的 SHA-256 |
| 目标三元组 | 只接受规范的 Windows x64、Linux x64、Linux ARM64 三元组；每个目标至多一个实现 |
| 对象格式、位宽、端序、布局 | COFF／ELF 与目标一致，当前为 64 位小端，数据布局与登记的目标运行时一致 |
| CPU 与特性 | 默认 CPU 实现可供同架构目标使用；专用 CPU 要求匹配，未明确启用的必需特性拒绝 |
| 原生调用约定 | 当前接受 c-v1、chali-borrow-v1 和 chali-value-v1，语言调用约定须带匹配的必需能力标记；与语言接口和运行时协议分开编号 |
| bitcode | 可选 LLVM bitcode 实现；显式选择时核对实际目标、布局及精确 LLVM 版本 |
| 可忽略扩展 | 只放在 `extensions` 对象内；未知语义字段、必需能力或保留标志拒绝 |

`template` 可重复使用 `--dependency "MODULE=VERSION#SHA256"` 记录精确依赖，使用 `--system-library NAME` 记录链接库，使用 `--runtime-library NAME` 记录应用需要的动态库。格式检查核对这些记录的合法性；依赖的文件解析与最终交付需要由使用这些资产的构建流程完成。

许可证名称通过 `--license` 记录。`--license-asset legal/LICENSE.txt` 记录包内许可证资产名，封装时再加入 `--entry "license:legal/LICENSE.txt=LICENSE"`。可包含多个许可证与署名文件。编译器工具链依赖、静态链接输入和应用运行时动态库分别记录，原生应用不因生产端内置 LLVM 就需要携带 LLVM DLL。

显式生命周期契约写入生成接口并参与入口身份；具体能力与借用返回边界见下文“公开生命周期契约与借用返回”。

## 普通记录按值参数

非拥有普通记录可以按值传入库函数。入口使用 chali-value-v1 原生 ABI、plain-record-value-v1 必需能力和私有 ChaLiValue 声明。接收方通过同一共享分配器建立独立副本，修改参数不影响调用方；函数退出时释放副本并清零拥有槽。嵌套普通记录和基础标量字段沿用既有槽布局，记录身份及完整布局摘要进入入口符号。

```chali
struct Point { int64 x; float64 y; };
int64 shifted(Point value, int64 delta) {
    value.x = value.x + delta;
    return value.x;
}
```

消费者传入 x 为 40 的记录并调用 shifted(point, 2) 后，返回 42，point.x 仍为 40。这与可变引用参数会修改调用方的行为分别核对。返回值也可使用下文的普通记录；其他资源字段及完整类型登记仍未接入，不接受物理 C ABI 的 #export。

## 单元数据与普通记录返回

普通非拥有记录可以按值返回，例如：

```chali
Point make(int64 x, float64 y) {
    Point result;
    result.x = x;
    result.y = y;
    return result;
}
```

库包保存本目标的数据映像与逐符号布局。最终程序合并各单元的数据，按 16 字节对齐分配统一偏移；库代码通过带内容身份的基址符号访问自身模板。调用方沿用语言的记录复制规则接收返回值，重复调用不新增长期存活堆块。普通记录参数返回和嵌套普通记录返回均适用；同名类型仍按模块区分。

携带数据的目标额外包含 unitData、unitLayout、unitSymbol 三个字段，并要求 unit-data-v1。映像和布局条目都使用种类 9（unit-data），基址符号摘要覆盖模块、目标三元组、完整布局和映像。清单字段须同时存在；缺失条目、错误摘要、重复名称和布局越界均拒绝。没有数据映像的旧包保持原编码。

当前记录返回只允许来自具备该能力的已验证接口。普通非拥有记录的显式移动、新鲜聚合转移与递归返回使用下文的共享生命周期类型身份；其他未支持的函数体仍明确拒绝。String 拥有字段采用上文约定；其他资源字段、接口／闭包返回和初始化回滚仍保留后续验收。

## 二进制编码

整数明确使用小端编码，不直接写 C++ 结构体、指针或 `size_t`。摘要保存原始 32 字节。SHA-256 的消息规则依据 [NIST FIPS 180-4](https://csrc.nist.gov/pubs/fips/180-4/final)；摘要检查内容变化，来源认证需要独立的签名或可信发布渠道。

| 文件头偏移 | 宽度 | 含义 |
| --- | --- | --- |
| 0 | 8 | ASCII `CHLIB1\r\n` |
| 8 | 4 | 容器版本 1 |
| 12 | 4 | 条目数量 |
| 16 | 8 | 规范清单字节数 |
| 24 | 32 | 清单 SHA-256 |
| 56 | 清单长度 | 无 BOM 的 UTF-8 规范 JSON |

清单后依次保存所有条目。每条是：`u16 名字长度`、`u16 种类`、`u32 标志`、`u64 负载长度`、`32 字节记录摘要`、`名字`、`负载`。记录摘要覆盖前述 16 字节元数据、名字和完整负载，因此名字、种类与长度的变化也会被检测。没有填充字节或隐式宿主对齐。名字必须严格递增；重复、越界段、空路径段和尾部未登记数据均拒绝。

| 种类编号 | 含义 |
| --- | --- |
| 1 | 语言接口 |
| 2 | 未实例化模板资产 |
| 3／4 | 原生对象／自包含归档 |
| 5／6 | bitcode／独立字节码资产 |
| 7／8 | 显式源码资产／许可证与署名 |
| 9 | 单元数据映像或二进制逐符号布局 |

标志位 0 表示未知种类的条目可以忽略；其他位保留。被清单引用的接口、实现和许可证必须存在且种类正确。bitcode／字节码标签只标识容器负载，`inspect` 成功不表示这些字节已经通过相应执行后端的验证。`source` 只记录显式源码资产，原生目标缺失时不会自动使用它。

单包最多 256 MiB，清单最多 1 MiB，条目最多 4096 个，包内名字最多 240 字节；文本资产最多 32 MiB 且不能包含 NUL。目标最多 64 项，依赖与各文本集合最多 1024 项。按实际剩余文件长度核对声明长度后才分配负载。这些是编码及有效载荷预算，不是编译器进程总内存上限。

原生负载在已经摘要核对的拥有型字节上继续检查目标头和归档成员边界，不重新打开包来判定实现身份。符号索引、节、重定位和实际原生签名仍由对应工具链及互调契约核对。薄归档、嵌套归档与错误架构拒绝。

## 原生与 bitcode 实现选择

`lib build --with-bitcode` 同时生成原生对象与 LLVM bitcode，清单记录实际 LLVM 版本。未给此选项时仍只生成原生实现。两份实现共享同一接口和单元数据身份。

```sh
chali lib build provider.chl --module example::math --package-version 1.0.0 --with-bitcode --out modules/example/math.chlib
chali run main.chl --library-implementation bitcode --lto
chali run main.chl --library-implementation bitcode --no-lto
chali link main.o --package modules/example/math.chlib --library-implementation bitcode --out app.exe
```

消费端缺省选择 `native`；`--library-implementation native|bitcode` 对本次全部直接与传递库生效，也可在 `chali.toml` 的 `[aot]` 中设置 `library_implementation = "bitcode"`，命令行优先。缺少所选实现时报告错误，不自动回退。选择原生实现时不会把可选 bitcode 当作对象，也不要求其生产 LLVM 与当前工具相同。

选择 bitcode 时先核对包摘要及目标，再读取载荷内的目标三元组、数据布局、生产 LLVM 版本和格式纪元。载荷、清单及实际静态 LLVM／外置 clang 的精确版本必须一致；这是保守的兼容策略，不承诺跨 LLVM 版本消费。头部检查有字节、记录、字段和嵌套预算，完整模块仍交给实际 LLVM 读取器核验。

`--lto-mode off|full|thin` 明确选择最终链接模式，缺省 `full`。`--lto` 是完整 LTO 的兼容写法，`--no-lto` 关闭 LTO；多个开关按最后一个生效。项目 `[aot]` 的 `lto` 同时接受旧布尔值以及 `"off"`、`"full"`、`"thin"`，显式命令行优先。

```toml
[aot]
library_implementation = "bitcode"
lto = "thin"
```

完整 LTO 把主单元及所选库模块交给最终链接器统一优化。ThinLTO 为模块写入摘要与内容散列，由 LLVM／LLD 执行跨模块导入和优化。消费端为 ThinLTO 重新生成已验证库的实际摘要；完整 LTO 消费携带 ThinLTO 摘要的库时重新编码为完整模块，避免模式由包生产配置隐式决定。库包无需为两种 LTO 保存两份接口或两份数据映像。普通语言包入口使用隐藏可见性参与最终单元链接，显式原生别名保留公开可见性，避免把私有 ABI 入口当成动态导出而阻止删除。

库单元的 LLVM 模块源码身份包含模块名摘要，区分不同库内的同名局部符号。独立库函数使用普通代码节，使 LLVM 可以提升并导入内部函数；主程序继续保留原生调试代码区。生成摘要前按实际 LLVM 的要求分配全局值 GUID，并提供模块的分析上下文。

所有库模块按消费模式重新编码并统一模块标志，Full LTO 沿用工具链要求的拆分标志，ThinLTO 关闭类型单元拆分。外置 LLVM 转换已有模块时，先由同一工具导出文本，仅规范化模块标志表中的模式及拆分标志，再由 LLVM 重新读取；该文本转换最多接受 256 MiB，输出继续核对摘要类别、目标、布局与版本。

关闭 LTO 时，所选 bitcode 先独立编译为原生对象再链接，不会改选包内原生实现；原生库选择仍可与主单元 LTO 同用，但不因此获得库实现的跨模块优化。接口、数据映像及共享运行时约定不因模式改变。开发调试仍使用 O0；LTO 模式不将其自动升级为 O3。当前不提供 ThinLTO 持久缓存或并行度配置，不承诺所有负载获得性能收益。交叉目标生成与实际目标执行分别验收。

`tests/chali_library_lto.py` 比较两层库和消费者在 off／full／thin 下的实际结果与导出函数指令，核对跨库内联、常量合并及未使用入口删除；覆盖 Full／Thin 库包相互消费、独立链接和配置覆盖。结构验收不作为编译耗时或运行性能测量。

### 1.0.12 静态 LLVM 的冻结对照

2026-11-02 在 Windows 10 x64、AMD Ryzen 5 5600X（6 核／12 逻辑处理器）上完成一次冻结对照，使用内置 LLVM 23.1.4，C11 计时壳使用 GCC 11.2.0。被测编译器源码为 `66afd9adc30650649493c4d618638b84d1edbdae`，真实 SHA-256 为 `2bc93306c8b4825bcf1242a3e123b353a0bc76c310ab68fda396d03f20126d68`。以下数据只对应这份冻结字节；文档定稿后重建的编译器与 VSIX 另做功能核对，不重标数据。

[负载、协议和复现方法](../benchmarks/library_lto/README.md)以及[驱动](../tools/benchmark_library_lto.py)保留可重现代码。两类负载都完成 1,048,576 步相同的整数依赖链：`call` 每步跨两层库调用，`bulk` 每次处理 256 步。所有配置先核对五个种子的独立模型结果，再冻结编译器、源码、项目配置、两份库包和运行产物。均使用 unchecked O3、malloc、512 MiB 堆预算；原生选择与 bitcode 选择分别对照，不把未支持的模块全局状态作为负载。

正式记录共 66 个构建样本、50 个运行进程、250 个内核样本。每配置每负载的运行中位数来自五个进程、每进程五个样本；模块初始化、预热、结果归并和结束不计时。116 个计时命令逐次在区间外检查 CPU，启动读数为 1%～29%，均低于 40%；不固定频率、不绑核，不声称全程完全空闲。缓存经完整功能矩阵预热，正式样本之间不清理，配置顺序按固定种子轮换。

| 库选择／LTO | call 内核（ms） | bulk 内核（ms） | AOT DLL（字节） | 独立链接 DLL（字节） |
| --- | ---: | ---: | ---: | ---: |
| native／off | 10.2953 | 6.3899 | 4,886,528 | 4,886,528 |
| native／full | 11.0066 | 6.0879 | 4,877,824 | 4,886,528 |
| bitcode／off | 8.9765 | 6.5387 | 4,886,528 | 4,886,528 |
| bitcode／full | 1.1566 | 1.1423 | 4,876,288 | 4,886,016 |
| bitcode／thin | 1.1462 | 1.1523 | 4,876,288 | 4,886,016 |

两类负载本轮的 DLL 文件大小相同，表中 AOT 大小对应冻结运行副本，独立链接大小对应最后一轮真实产物。与 native／off 相比，bitcode Full／Thin LTO 的 AOT DLL 减少 10,240 字节；这些体积变化不代表通用压缩比例。

原生消费对象的编译中位数为 call 2,140.1 ms、bulk 2,065.1 ms，各取三轮。其余构建时间如下，均为三轮中位数：

| 库选择／LTO | call 独立链接（ms） | call 完整 AOT（ms） | bulk 独立链接（ms） | bulk 完整 AOT（ms） |
| --- | ---: | ---: | ---: | ---: |
| native／off | 590.2 | 2,425.6 | 610.5 | 2,428.4 |
| native／full | 617.8 | 2,585.4 | 585.6 | 2,378.8 |
| bitcode／off | 643.0 | 2,396.9 | 633.5 | 2,494.9 |
| bitcode／full | 611.8 | 2,397.1 | 604.2 | 2,428.6 |
| bitcode／thin | 631.0 | 2,381.5 | 636.7 | 2,520.6 |

编译和链接墙钟包含进程启动；独立链接还包含所选库 bitcode 的重编码／物化。独立链接使用已经生成的原生消费对象，完整 AOT 则可优化主单元，因此不能把对象编译与独立链接之和当作完整 AOT。原生库只开启主单元 LTO 没有稳定改善；ThinLTO 与 Full LTO 也没有统一的时间优势。这里只观察到这两个算术负载在库 bitcode LTO 下的内核收益，不推断其他程序、平台、分配器或 LLVM 变体。

原始命令、逐次 CPU 检查、种子结果、全部样本、摘要与运行二进制仅在本地交付目录 `build/independent-validation/1.0.12-a7-lto-comparison/measurement/` 保存，不提交 Git。`results.json` 的真实 SHA-256 为 `8c572e9c391001ccf5216a46ca6a7a9e2ea02019e073eaca926622177665f770`；独立核对确认 116 个计时命令成功、250 个样本校验一致、冻结文件字节未变。历史 1.0.10／1.0.11 及其他语言数据保持原身份，本轮未重测。

## 失败与复现

库包先写入根 `tmp/` 下的独立工作区，成功后才发布最终文件。Windows 原生产物替换遇到访问拒绝、共享或锁冲突时最多等待五秒；持续失败明确报错，保留旧文件并清理同目录暂存文件。格式或对象检查失败时保留原产物；命令行封装禁止输出覆盖输入清单或任一资产。读取失败不发布半个恢复结果。摘要、版本、目标或必需能力错误不会静默选择另一个实现。

相关回归：

```sh
python tools/run_module_gates.py libraries defaults docs --chali PATH --random-recheck 1
python tests/chali_digest.py
python tests/chali_library_import.py --chali PATH
python tests/chali_library_package.py --chali PATH
python tests/chali_library_package.py --chali STATIC_PATH --other-chali EXTERNAL_PATH
```

测试独立按公开编码和 `hashlib` 交叉核对摘要，覆盖分块与填充边界、规范编码、损坏、长度预算、可忽略条目、三目标选择和事务发布；包内归档恢复后使用独立链接器实际运行。交叉目标只检查生成与格式，宿主执行单独记录。

## 共享生命周期类型身份

涉及原生记录的生命周期操作不沿用库构建时的局部类型编号。库包要求 shared-type-id-v1，LLVM 代码引用由完整模块身份和记录布局摘要生成的类型符号；最终程序为该符号提供本次运行的编号。消费者增加同名或其他类型时，不会让跨库移动标记落到另一个类型上。

生命周期查询、核对和结束直接调用共享 C 原语，沿用同一个对象表。普通整数不会按数值猜测并替换为类型编号。已支持普通记录的新鲜返回值转发、递归及显式移动；重复移动仍按语言规则拒绝。接口装箱、闭包、动态类型查询和拥有资源的完整跨单元登记仍待后续接入。

独立单元也可使用 CHALI_BACKEND_PROFILE=1 编译 AST 所有权计数；计数字符串留在本单元，执行时汇入最终程序的共享计数器，不复制语言运行时。使用 CHALI_BACKEND_PROFILE_OUTPUT 指定本地 JSON；插桩输出不作为正式计时数据。

## 公开生命周期契约与借用返回

普通库函数保留 `#static(形参)`、`#borrow(形参,[来源...])` 和 `#borrow(#ret,[来源...])` 的公开约束。例如：

```chali
module example::views;
struct Point { int64 x; };
#borrow(#ret,[value])
Point const& view(Point const& value) { return value; }
#static(value)
int64 readStatic(Point const& value) { return value.x; }
```

生产器在生成的 `#abi` 声明之前写入生命周期指令，接口不携带实现体。包声明 `lifetime-contract-v1` 必需能力；返回记录或枚举借用时还声明 `borrowed-result-v1`。消费者使用已有生命周期检查器核对静态来源、返回来源和只读／可写约束。同名重载按实际选中的函数查询契约，标量缺省实参不会使契约失效。契约中的形参按下标进入公开符号身份，形参改名不改变这部分身份；改变返回来源等公开约束会改变入口符号，使旧消费者对象不能沿用原入口链接。

当前借用返回支持上述受支持普通记录／枚举及标准 String 的 `const&` 或 `mut&`，来源必须由 `#borrow(#ret,...)` 显式绑定到受支持的借用形参。返回值是调用方存储中的逻辑地址，非拥有聚合沿用 `ChaLiBorrow`／`ChaLiValue`，String 及其拥有字段聚合使用 `ChaLiOwned`，不生成宿主指针封送；调用方仍按原有作用域检查阻止局部引用逃逸或只读引用转为可写。库不能返回按值形参副本、局部对象或临时拥有值的借用。其他拥有载荷、标量引用入口及静态全局返回继续按尚未接入的边界拒绝。

库接口缺失必需能力、契约引用未知形参或返回声明缺少来源时拒绝加载。带生命周期契约的外部函数须直接调用；当前函数值类型不能携带这些契约，取为函数值时明确拒绝。
