# 当前交付任务

- [x] 构建并启动验证 Linux ARM64 和 x86 编译器，生成 DEB、tar.gz 和 ARM64 VSIX。VS Code 无 Linux x86 VSIX 目标。
- [x] 将 Cortex-M 和 RV32 裸机接入 ChaLi BUILD，并验证启动、链接脚本和最终 ELF。
- [ ] 复验 MCU 目标拒绝导入 `std::reflect`。
- [x] 接入板级 `HAL_Chali_*` 和 `RTOS_HAL_Chali_*`，在模拟器验证打印、分配、释放及 Cortex-M4 FreeRTOS 任务调用。
- [x] 交叉构建 Windows ARM64 和 x86 编译器并核对 PE 架构。按用户授权记录 ARM64 未启动验证。

Windows 两种 PE 已构建并核对机器类型。x86 通过 `--build-info`、LSP 初始化和样例检查，ARM64 仍需 Windows ARM64 环境启动。Linux ARM64 已通过 QEMU 启动并生成 DEB、tar.gz 和 `linux-arm64` VSIX，插件版本和编译器字节一致。

MCU 的 ChaLi 打印样例已通过正常 BUILD 入口生成 Cortex-M4 和 RV32 ELF，并在 Unicorn 中执行，板级内存在程序退出时全部释放。Cortex-M4 在 QEMU 中通过 FreeRTOS 任务验证了打印和板级内存的分配、释放。RV32 的模拟验证覆盖裸机模式。
