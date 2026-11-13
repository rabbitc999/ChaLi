# 当前交付任务

- [ ] 构建并启动验证 Linux ARM64 和 x86 编译器，生成并核对发行包和对应 VSIX。
- [ ] 将 Cortex-M 和 RV32 裸机接入 ChaLi BUILD，并验证启动、链接脚本和最终 ELF。
- [ ] 提供独立的 MCU `std::core`：保留 Result、String、List、Map、Set 和 assert，拒绝反射。
- [ ] 接入板级 `HAL_Chali_*` 和 `RTOS_HAL_Chali_*`，验证打印、分配和释放重定向，以及 RTOS 任务调用。
- [ ] 交叉构建 Windows ARM64 和 x86 编译器，核对 PE 架构并记录启动验收状态。
