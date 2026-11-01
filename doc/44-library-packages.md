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

当前构建范围是普通函数的 void／bool／char、各宽度有符号和无符号整数、float32／float64 签名，函数体支持标量运算、局部变量、控制流、当前单元内直接调用、递归及已导入标量库的直接 C ABI 调用。C ABI 入口直接由 LLVM 生成位宽和浮点转换。函数入口仍明确拒绝按值聚合参数／返回值、泛型定义、拥有值、全局状态、方法、变参、unsafe 导出、宿主及地址操作，以及独立调试／构建期步数预算，不会忽略这些声明或把它们改为标量。模板、初始化与更广的语言内部 ABI 继续在后续阶段接入。

标量函数的默认参数保留定义模块作用域和调用时求值。例如：

```chali
inline int64 defaultValue() { return 7; }
int64 addDefault(int64 x, int64 y=defaultValue()) { return x+y; }
```

导入后 `example::math::addDefault(3)` 在本次调用缺省 `y` 时才求默认值；`addDefault(3,9)` 不求默认值。生产器将已检查表达式编译成独立标量入口，公开接口只保存该入口的声明和调用，不包含私有函数体，也不会在导入、构建时执行默认表达式。多个缺省参数保持原有求值次序，私有函数和已导入库调用仍在定义模块内解析。入口仍受标量单元限制；全局状态、拥有值与其他未接入操作不会因出现在默认值中而获得支持。生成的默认值入口是实现辅助声明，不作为需要手工调用的标准 API。

公共接口也保留普通记录声明。字段可为基础标量或本模块或已验证预编译依赖中的嵌套普通记录，保留字段名、顺序和完整模块身份。例如提供方可声明：

```chali
struct Point { int64 x; float64 y; };
struct Bounds { Point low; Point high; char marker; };
```

导入后可以声明 `example::math::Bounds bounds;`，通过 `bounds.low.x` 访问嵌套字段，并进行独立值复制。接口使用原始字段结构生成声明，消费者按相同语言版本重建布局，不把摊平字段名写回源语法。默认生成的语言方法由消费者重建；用户方法、构造器、字段默认初始化器、拥有字段、自定义布局和接口继承契约仍拒绝。当前仍需至少一个受支持的公开函数；参数可为基础标量、普通记录按值参数或下文的借用，返回值可为基础标量、void 或下文的普通记录，拥有字段和完整聚合调用仍待接入。不同模块中同名记录继续是不同类型。

公开枚举支持无载荷、单载荷及多载荷分支；载荷可为基础标量或同样受支持的记录／枚举。例如：

```chali
enum Event {
    Empty,
    Count(int64 value),
    Pair(int64 left, float64 right),
    At(Point position)
};
```

声明保留分支顺序、原始载荷字段名及完整类型名，消费者可以构造值并使用 `match`；无载荷分支用 `Event::Empty`，多载荷分支用 `Event::Pair(3,4.5)`。记录可包含来自预编译依赖的枚举，依赖声明仍由原模块提供。拥有载荷、用户方法、接口契约和跨库按值聚合调用仍未接入，不会把枚举值当作整数原型导出。

普通非拥有记录也可以作为只读或可写借用参数进入已编译语言单元：

```chali
int64 read(Point const& value) { return value.x; }
void bump(Point mut& value, int64 delta) { value.x = value.x + delta; }
```

这些入口直接传递调用方的 ChaLi 逻辑地址，在最终程序的同一内存与检查上下文中访问字段，不转成宿主 C 结构体，也不生成 C++ 参数封送。普通记录返回采用下文的数据重定位约定，函数体限当前已支持的标量控制流、字段访问和直接调用；拥有值、枚举引用、记录引用默认值、引用返回和库全局状态尚未接入。只读及可写约定继续由语言检查，已有标量参数可与记录借用混用。含语言借用参数的函数不能同时使用面向物理 C 布局的 `#export`。

这类包使用 `chali-borrow-v1` 原生 ABI 和 `borrowed-record-v1` 必需能力。生成接口的 `ChaLiBorrow` 标记只允许出现在匹配 ABI 的已验证库来源中，不能复制为普通源码声明。符号包含完整记录及嵌套字段布局摘要；独立旧对象遇到布局已改变的新包会因缺少原符号而拒绝链接。普通 C ABI 包继续使用 `c-v1`；引用传递与跨库拥有对象、闭包及动态函数值的完整契约分别验收。

抽象接口声明支持基础标量参数／返回值（含 void 返回）、静态协议方法和继承关系。基接口须来自同一模块或已验证的预编译依赖；生产接口保留原继承关系，跨模块基接口同时登记导入，避免消费者同名声明改变解析。例如：

```chali
interface Error { int64 code(); };
interface DetailedError : Error { bool retry(); };
```

消费者可实现这些接口、通过基接口调用方法，并使用 `IoError(error)` 这样的接口类型匹配。声明保持原方法参数名及静态标记，不把继承的方法重新定义成派生接口的方法。生产器仍拒绝接口默认方法体、关联类型、泛型定义和非标量方法签名；在库内创建接口实例或跨原生函数入口传递接口值仍待共用运行时接入。

跨模块嵌套记录须来自已验证的 `.chlib` 依赖，字段使用原声明模块的完整名称。生产包保留精确依赖，不把外部记录复制成自身模块的新类型；消费者即使只导入外层库，也会恢复原声明包。只来自实现源码的跨模块记录字段会被拒绝，应先将该依赖预编译。

导入的预编译库会以精确版本和内容摘要记入生产包，最终消费者恢复完整依赖闭包。源码 `#export("native_name")` 保留额外的原生入口，普通语言导入仍使用包含模块及完整签名的稳定入口；`inline` 私有函数不出现在语言公开接口中。原生入口存在不等于最终动态库自动导出它，最终链接器的导出设置仍需单独指定。

`lib build` 也可直接附带交付信息：

```sh
chali lib build math.chl --module example::math --package-version 1.0.0 --out modules/example/math.chlib --license Apache-2.0 --license-asset legal/LICENSE.txt=LICENSE --license-asset legal/NOTICE.txt=NOTICE --runtime-library example-plugin.dll
```

`--license-asset` 可重复，等号左边是包内名字，右边是输入文件；许可证与署名按原始字节随包保存。名字、重复、长度预算及普通文件检查在代码编译前进行，输出不能覆盖这些输入；读取后使用同一快照，失败保留原包。`--system-library NAME` 声明最终链接所需的库，消费者链接时加入；名称必须来自目标 SDK，例如 Windows 的 `kernel32` 或适用 Linux 目标的 `m`；`--runtime-library` 记录应用动态依赖，不自动复制或加载该文件。示例中的插件名只是交付记录，需要按应用实际依赖填写。

`extensions.producer` 记录 AST→LLVM、O3、checked 设置、实际静态／外置 LLVM 后端和由最终消费者提供运行时的约定，不含构建机绝对路径。精确语言库依赖继续使用清单 `dependencies`，不把生产编译器的 LLVM 工具链写成应用动态依赖。运行依赖名称是发布者声明，工具不会据此声称已扫描出所有动态加载路径。

此类包声明 `scalar-unit-v1` 必需能力，缺少该能力的编译器会在加载前拒绝。普通导入既可消费这种语言生成对象，也可消费前节手工声明的 C ABI 归档；两者的接口均不包含运行期函数体。

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

当前原生接口允许类型与函数声明，函数入口使用显式 C ABI 或受验证的语言调用约定；不允许运行期函数体、全局变量或静态字段。接口内的类型、可见性和借用继续接受正常语言检查。模板实例、库初始化及拥有值的语言内部 ABI 尚未开放。库包存在可选源码或模板资产，不会因此自动启用它们。

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
| bitcode | 可记录独立资产和精确 LLVM 版本；原生选择不把它当作机器对象 |
| 可忽略扩展 | 只放在 `extensions` 对象内；未知语义字段、必需能力或保留标志拒绝 |

`template` 可重复使用 `--dependency "MODULE=VERSION#SHA256"` 记录精确依赖，使用 `--system-library NAME` 记录链接库，使用 `--runtime-library NAME` 记录应用需要的动态库。格式检查核对这些记录的合法性；依赖的文件解析与最终交付需要由使用这些资产的构建流程完成。

许可证名称通过 `--license` 记录。`--license-asset legal/LICENSE.txt` 记录包内许可证资产名，封装时再加入 `--entry "license:legal/LICENSE.txt=LICENSE"`。可包含多个许可证与署名文件。编译器工具链依赖、静态链接输入和应用运行时动态库分别记录，原生应用不因生产端内置 LLVM 就需要携带 LLVM DLL。

显式生命周期契约尚未写入库接口；声明了 #static 或 #borrow 的函数目前拒绝导出，不能通过省略契约降低调用约束。

## 普通记录按值参数

非拥有普通记录可以按值传入库函数。入口使用 chali-value-v1 原生 ABI、plain-record-value-v1 必需能力和私有 ChaLiValue 声明。接收方通过同一共享分配器建立独立副本，修改参数不影响调用方；函数退出时释放副本并清零拥有槽。嵌套普通记录和基础标量字段沿用既有槽布局，记录身份及完整布局摘要进入入口符号。

```chali
struct Point { int64 x; float64 y; };
int64 shifted(Point value, int64 delta) {
    value.x = value.x + delta;
    return value.x;
}
```

消费者传入 x 为 40 的记录并调用 shifted(point, 2) 后，返回 42，point.x 仍为 40。这与可变引用参数会修改调用方的行为分别核对。返回值也可使用下文的普通记录；拥有字段、显式生命周期契约及完整类型登记仍未接入，不接受物理 C ABI 的 #export。

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

当前记录返回只允许来自具备该能力的已验证接口。显式移动、跨单元新鲜聚合转移所需的统一类型编号仍待接入；未支持的函数体明确拒绝。拥有字段、接口／闭包返回和初始化回滚也分别保留后续验收。

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

## 失败与复现

库包先写入根 `tmp/` 下的独立工作区，成功后才发布最终文件。格式或对象检查失败时保留原产物；命令行封装禁止输出覆盖输入清单或任一资产。读取失败不发布半个恢复结果。摘要、版本、目标或必需能力错误不会静默选择另一个实现。

相关回归：

```sh
python tools/run_module_gates.py libraries defaults docs --chali PATH --random-recheck 1
python tests/chali_digest.py
python tests/chali_library_import.py --chali PATH
python tests/chali_library_package.py --chali PATH
python tests/chali_library_package.py --chali STATIC_PATH --other-chali EXTERNAL_PATH
```

测试独立按公开编码和 `hashlib` 交叉核对摘要，覆盖分块与填充边界、规范编码、损坏、长度预算、可忽略条目、三目标选择和事务发布；包内归档恢复后使用独立链接器实际运行。交叉目标只检查生成与格式，宿主执行单独记录。
