# 05 交付安全

<!-- LANGUAGE NAVIGATION -->
[English](en/05-delivery-security.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

原生可执行文件不提供加密或混淆：`--encrypt` 与 `[chali] encrypt` 不受支持，交付物始终由源码经 AOT 生成。

## 1. 交付形态

| 形态 | 接收方需要什么 | 适用场景 |
| --- | --- | --- |
| `.chl` 源码 | ChaLi 编译器 | 开放源码、联合开发 |
| `aot` 可执行文件 | 目标平台；程序用到的动态库须与产物同目录 | 交付闭源程序、离线运行 |
| `--shared --export` 动态库 + 头文件 | C/C++ 宿主工程与链接器 | 嵌入既有宿主程序 |
| 四元式三件套（`emit`） | 具备自行装配能力的工具链 | 检查中间表示、定制装配 |

## 2. 导出与分发

可执行文件：

```sh
chali aot src/main.chl --out build/app
```

动态库与头文件：

```sh
chali aot src/lib.chl --shared --export add --out build/lib
```

第二条命令生成 `build/lib.dll`（Linux 为 `.so`）与 `build/lib.h`，宿主按 `chali_init()` → `f_add(...)` → `chali_finish()` 调用。Windows 加载器按可执行文件所在目录和 `PATH` 查库，因此 `#abi` 引用的动态库要和产物放在一起。

只想检查中间表示时用 `chali emit src/main.chl --out build/app`，它只出 `.quad` / `.data.bin` / `.layout`，不链接也不执行。

## 3. 能力边界

可执行文件不提供身份认证。需要控制发布来源或设备访问权限时，应由部署系统、系统签名或包管理实现对应策略。

默认构建会丢弃符号与调试信息（`--strip-all`）；需要保留函数名与源码位置时显式使用 `--debug`，该模式以 `O0` 编译并保留调试段。字符串常量、类型名与[反射表](19-reflection.md)仍会出现在产物中——想彻底不暴露内部结构，就不要把对应源码交付出去。

## 4. 产物管理

| 文件 | 保存方 | 用途 |
| --- | --- | --- |
| `.chl` | 开发方或协作方 | 源码与重新构建 |
| 可执行文件 / 动态库 + 头文件 | 运行方 | 直接运行或嵌入宿主 |
| `.chali/` 中间产物 | 构建机 | 默认构建后即删除，仅供排查 |
| 调试构建与构建记录 | 开发方 | 复现和定位问题 |

升级时从源码重新构建，并核对目标平台的架构、动态库依赖与系统服务。项目配置里的旧键（如 `chali.encrypt`）会直接报错，不会静默忽略。

## 5. 下一步

- [工具链](09-toolchain.md)：命令与构建方式。
- [构建产物与中间文件](15-artifacts.md)：产物布局、`.chali/` 目录与清理规则。
- [宿主集成与 C ABI](08-host-integration.md)：两个方向的调用约定。
- [panic 调用栈](18-panic-trace.md)：错误诊断与定位。
