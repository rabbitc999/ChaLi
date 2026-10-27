# 26 执行方式与部署边界

<!-- LANGUAGE NAVIGATION -->
[English](en/26-execution-delivery.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

四元式只走一条路：交给 LLVM AOT 生成原生机器码。编译器里没有解释执行核，`.svmb` 字节码导出已删除。

| 场景 | 入口 | 结果 |
| --- | --- | --- |
| 直接运行 | `chali run app.chl` | checked AOT 编译并执行，返回程序退出状态 |
| 检查 | `chali check app.chl` | 语法、类型、借用与语义诊断，不执行、不产出 |
| 原生导出 | `chali aot app.chl` | 与 `app.chl` 同级的原生可执行文件（Windows 带 `.exe`） |
| 四元式 | `chali emit app.chl` | 与 `app.chl` 同级的 `.quad`、`.data.bin`、`.layout` |
| 宿主嵌入 | `#export` / `--shared` | 动态库与 C 头文件，由宿主直接调用 |

`run` 保留边界与存活检查，`aot` 默认 unchecked；两者都能用 `--checked` / `--unchecked` 改过来。反射是语言的固定组成，不可裁剪。

## 中间产物放哪

编译过程中间产物（`.quad`、`.data.bin`、`.layout`、`.ll`、ABI 头、汇编器日志）统一落在当前目录的隐藏目录 `.chali/` 下，Windows 上带隐藏属性：

- 运行或导出完成后只保留标准库预编译缓存 `.chali/chali-stdlib-cache/`，其余删除；
- `--keep-artifacts` 保留全部中间产物，用于对照与排错；
- `--out 前缀` 显式给路径时按用户要求全部保留，不做清理；`--incremental` 与 `--emit-llvm` 同样保留；
- `.chali/` 已由 `chali new` 生成的 `.gitignore` 排除，不进版本库。

**交付物不放隐藏目录**：`aot` 的可执行文件与 `emit` 的四元式文件都与源文件同级，Linux 等平台的可执行文件没有后缀。需要别的位置时用 `--out`。

```powershell
chali run app.chl                     # .chali/ 里只剩标准库缓存
chali aot app.chl                     # 产出 ./app.exe
chali aot app.chl --out build/app     # 产出 build/app.exe 与 build/app.ll 等，全部保留
chali emit app.chl                    # 产出 ./app.quad 等
```

标准库内嵌在编译器中，外部数据库、动态库及外设仍由部署环境提供。详见[工具链](09-toolchain.md)、[部署](06-deployment.md)、[宿主集成](08-host-integration.md)与[当前能力](17-capabilities.md)。
