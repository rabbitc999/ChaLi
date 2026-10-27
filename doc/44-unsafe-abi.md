# 原始存储与 C ABI 指令

[English](en/44-unsafe-abi.md) · **简体中文**

## unsafe 与存储

`unsafe { ... }` 允许调用不安全函数及显式整数／引用转换。`unsafe` 函数要求调用者处于不安全作用域；函数体可以执行这些操作。闭包和其他函数的权限分别检查，不继承创建处的作用域。不安全函数值不能隐式变为普通函数值。

```chali
import std::memory;

void main() {
    std::lang::RawStorage<String> slots = std::lang::RawStorage<String>::allocate(8);
    unsafe {
        std::lang::initAt<String>(slots.pointer(0), "hello");
        String value = std::lang::takeAt<String>(slots.pointer(0));
        assert(value == "hello");
    }
    slots.close();
}
```

`RawStorage<T>` 独占原始容量，实现 `Closeable`，备用槽位不构造元素。`RawPtr<T>` 保存分配身份和元素下标；它不是宿主机器指针。不要改写内部句柄。`pointer(capacity)` 可以形成尾后位置，不能解引用。`__rawSize(T)`、`__rawAlign(T)` 仅在不安全作用域中查询语言存储布局。直接数组和宿主别名不能作为原始元素类型；需要数组时放进具名结构体。

| 操作 | 行为 |
| --- | --- |
| `initAt<T>(pointer, value)` | 仅允许未初始化槽位；把传入值移入目标 |
| `readAt<T>(pointer)` | 读取已初始化槽位，按普通值语义取得独立副本 |
| `borrowAt<T>(pointer)` | 借用已初始化槽位；调用者负责避免清理、移出或释放期间继续使用引用 |
| `takeAt<T>(pointer)` | 转移所有权，源槽位变为未初始化 |
| `dropAt<T>(pointer)` | 清理存活元素，保留容量 |
| `relocateNonOverlapping<T>(to, from, count)` | 先检查完整范围及状态，再搬移；目标必须未初始化、范围不得重叠；零个元素直接返回 |
| `RawStorage<T>::close()` | 只清理已初始化元素，再释放分配；重复关闭无操作 |

类型身份、容量乘法溢出、对齐、槽位状态和分配寿命仍会检查。支持的原始对齐最大为 16 字节。不安全作用域允许程序员承担额外义务，不能把这些接口当成绕过所有权约束的安全引用 API。

### 不检测类型的裸指针与字节操作

`BarePtr<T>` 只保存 ChaLi 逻辑字节地址，不记录运行时类型标签、初始化状态或所有者。`T` 决定解引用的静态布局及算术步长。`p + n`、`n + p`、`p - n` 按 `n * sizeof(T)` 偏移；`p - q` 返回元素距离，要求相差完整元素。调用者应只对同一分配计算距离和解引用有效位置。地址算术本身不访问内存；解引用与字节操作要求 `unsafe`。

```chali
unsafe {
    std::lang::BarePtr<int64> p = std::lang::barePointer<int64>(slots.pointer(0));
    std::lang::BarePtr<int64> q = p + 1;
    assert(q - p == 1);
    std::lang::memset<int64>(p, (uint8)0, 8);
    int64 mut& value = std::lang::dereference<int64>(p);
    *value = 42;
}
```

上述片段的 `slots` 是 `RawStorage<int64>`。`barePointer<T>` 从原始分配取得地址，允许未初始化与尾后位置，不检查实际槽位的类型；`pointerFromAddress<T>` 从数值地址形成裸指针，`dereference<T>` 按静态类型解释地址，均要求 `unsafe`。这些逻辑地址不能直接作为 C 机器指针传给宿主。

`memcpy<T,U>(destination, source, bytes)` 与 `memset<T>(destination, uint8 value, bytes)` 的长度都按字节计算。底层不检测元素类型，不读写原始存储的初始化位图，也不调用复制、移动或清理钩子。非零范围须落在有效分配内；复制的完整源与目标范围均检查成功后才写入，重叠被拒绝。零长度允许空指针。裸指针不携带分配代次，地址被释放并重用后的身份区分需要调用者管理。

这些类型与操作位于 `std::lang`，可以省略命名空间。`memcpy`／`memset` 直接生成专门四元式与 VM 指令，AOT 使用 LLVM intrinsic；不经过语言包装函数或通用内建分派。受检原始存储的必要动态检查通过标量内核执行，小型指针包装优先内联；裸指针解引用直接提取地址。

字节操作适用于字节缓冲区与调用者确认可按位处理的数据。资源值的初始化、转移和清理由 `initAt`、`takeAt`、`dropAt` 等操作承担；按位复制资源句柄不会建立独立所有权。String 实验的裸指针版使用批量复制和填充，旧的逐槽受检版保存在 `bootstrap_string_checked.chl`，测量按各自实际源码区分。

实验实现位于 `benchmarks/perf/raw_containers.chl` 和 `benchmarks/perf/bootstrap_string.chl`。List 删除保留容量并清理元素；Map 使用含键、值和状态的槽位，Set 独立保存键槽位。String 实验按字节保存 UTF-8 内容，提供容量、追加、读取、朴素查找和显式独立复制；它没有实现完整的标准 String Unicode API。拥有资源的类型不实现 `Copy<T>`。

## 裸指针视图与 IEEE754 位模式

`BarePtr<T>.castTo<U>()` 在 `unsafe` 作用域内返回相同地址的 `BarePtr<U>`，只改变元素视图。它不做数值转换、不分配存储，也不检查目标类型；调用者负责目标范围、对齐、初始化和存活。`castTo<uint8>()` 可用于逐字节步进与解引用，指针加减的步长由转换后的元素布局决定。

IEEE754 编解码使用这个方法重解释存储并解引用，不通过整数与浮点数之间的数值强制转换取得位模式。语言的 `float32` 和 `float64` 都使用八字节物理浮点槽；因此 `float32` 的四字节外部编码需要从槽内 binary64 位模式还原，不能把槽的前四字节直接当成 binary32。AOT 将受支持的位模式转换直接降低为 LLVM `bitcast` 及浮点宽度转换，并内联指针视图包装；大小端字节序由编解码接口明确处理。

## 默认标准库的自举实现

默认 List、Map、Set 的增长、查找、插删、复制及清空算法位于 `source/std/lang.chl`。List 删除和清空保留容量；Map 使用密集键值项、有效槽位编码和缓存 HashKey，Set 维护独立槽位。键的 `Equal`／`Hash` 契约保持原样。公开容器仍采用既有不透明存储表示，以便宿主返回的容器与语言容器直接互操作。

链表、栈、队列、双端队列、环形缓冲、二叉搜索树、红黑树、通用树、最小堆、图及并查集由对应的 ChaLi 标准库模块实现。标准列表算法的遍历、函数值回调、归约、二分、极值和稳定索引归并排序也在语言中实现；算法租约在回调期间保持来源存活并阻止结构性移动。String 的拥有值复制与清理、查找、比较、分类、UTF-8 解码、切片、变换、分割、连接和字节转换，以及 IEEE754 编解码和 KMP 均已迁入语言。

保留的宿主入口负责以下边界，不再承担上述容器或文本遍历算法：

- 分配、释放、地址／长度布局读取、存活及范围检查、原始存储初始化状态。
- 不透明容器单槽的预留、交换、读取、拥有值封送、键比较／Hash 回调和迭代租约；它们保持既有宿主 Codec 与记录 ABI。
- I/O、数值显示格式化、操作系统服务、反射、同步回调和 C ABI；大数及 CAS 继续使用既有运行时。

安全容器对资源元素的既有复制限制不变；拥有 `Closeable` 元素的原始存储实验容器另行验证移动与恰好一次清理。上述实验字节 String 也单独保留，性能报告区分真实旧版原生实现、当前默认自举实现和实验实现。

## 导入与导出

```chali
#abi("C", "./native.dll", "calc")
int32 nativeCalc(int32 x);

#export("calc")
int32 calc(int32 x) { return (int32)(x + (int32)1); }
```

导入必须只有声明，三个参数分别是调用约定、库路径、原生符号。目前支持 `"C"`。旧的 `abi "库" { ... }` 形式已删除。导出由 `#export("符号")` 指定原生名字，使用 `chali aot FILE --shared --out OUTPUT.dll` 选择输出库。导出符号必须为有效 C 标识符，不能重复或使用运行时保留前缀。

导入沿用同步回调和返回值所有权适配；释放函数契约及数组长度契约见 [宿主集成](08-host-integration.md)。导入函数值使用语言调用约定的适配入口，真实 C 调用由桥接层执行。运行时函数值不会被强转成机器函数指针。

`#export` 支持标量及不含资源的纯值结构体。生成的 C 头文件描述平台 C ABI；结构体的值参数和返回值经 C++ 编译器适配。String、容器、闭包、引用及需要清理的结构体不能通过这个纯值导出接口直接传递。宿主按生成头文件调用，并遵循 `chali_init`／`chali_finish` 生命周期。

## AOT 按需链接

AOT 根据可达原生调用选择实际标准库宿主模块；模块内部的函数节再由链接器回收。仅写下未使用的 `import` 不会强制把该模块的全部宿主实现链接进去。导出、反射、回调与初始化／清理入口沿用可达根，不以裁掉这些功能换取体积。

外置 LLVM 链路默认执行 O3 和完整 LTO，并在链接阶段消除不可达 LLVM 定义，再进行节区回收与符号裁剪。静态 LLVM 路径配置 O3、全局死代码消除及 LTO；实际验证平台以发行报告为准。用于同版本对照的 `CHALI_AOT_FULL_STDLIB=1` 环境变量可强制选入全部标准库宿主模块。

MinGW 下 CAS 的 64 MiB 零初值后备池位于 BSS，不再把整块零数据写进文件；运行时容量保持原值。文件体积与运行时内存占用分别测量。

AOT 数据映像只保存可达函数中 `addr` 实际引用的数据和函数值所需记录，裁掉不可达条目的初值与布局。零初值由启动时清零恢复，非零片段分段复制，相同片段在文件中共用存储；二进制布局按显式长度读取，支持空表及内嵌零字节。逻辑数据地址、对象身份、数组前置长度头及堆起始边界保持不变，不将文件缩小视为运行时数据区同步缩小。用于调试对照的 `CHALI_AOT_KEEP_DATA=1` 可保留完整映像与布局。

## 复现实验

```text
python tests/chali_unsafe_storage.py --aot
python tests/chali_abi_directives.py
python benchmarks/perf/bootstrap_containers.py --raw-storage --aot-only --operations 1024 --runs 3 --out tmp/raw-containers.json
python benchmarks/perf/bootstrap_string.py --size 512 --runs 3 --out tmp/string-comparison.json
```

String 报告保存编译器和源码 SHA256、正确性结果、原始计时和校验值，三方为旧版原生 String、本版默认自举 String 和实验原始字节串。String 转入实验存储的字节列表转换单列，不把它混入字节串核心操作。计时包括各负载中的构造、分配与清理，编译不计入内部计时；比较时应同时查看各负载的具体算法。
