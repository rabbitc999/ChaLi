# 当前交付任务

- [ ] 构建并启动验证 Linux ARM64 和 x86 编译器，生成并核对发行包和对应 VSIX。
- [ ] 将 Cortex-M 和 RV32 裸机接入 ChaLi BUILD，并验证启动、链接脚本和最终 ELF。
- [ ] 提供独立的 MCU `std::core`：保留 Result、String、List、Map、Set 和 assert，拒绝反射。
- [ ] 接入板级 `HAL_Chali_*` 和 `RTOS_HAL_Chali_*`，验证打印、分配和释放重定向，以及 RTOS 任务调用。
- [ ] 交叉构建 Windows ARM64 和 x86 编译器，核对 PE 架构并记录启动验收状态。

Windows 两种 PE 已构建并核对机器类型。x86 通过 `--build-info`、LSP 初始化和样例检查，ARM64 仍需 Windows ARM64 环境启动。Linux ARM64 已通过 QEMU 启动并生成 DEB、tar.gz 和 `linux-arm64` VSIX，插件版本和编译器字节一致。

MCU 的 ChaLi 打印样例已在 Cortex-M4 和 RV32 指令模拟器中运行。此次验收通过手工调整生成的 LLVM IR 并链接板级测试端口完成，正常 BUILD 入口、精简核心库及真实 RTOS 调度仍待完成。
