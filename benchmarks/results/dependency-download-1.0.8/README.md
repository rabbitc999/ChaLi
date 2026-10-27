# 1.0.8 在线依赖与书稿调整的简单验证

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](README.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

实际下载并校验 Windows x64 的 LLVM/LLD 23.1.4 预编译静态 SDK、匹配的 GCC 16.2 和外置 llvm-mingw；静态 SDK 同时通过 LLVM C API／LLD 静态链接探测。

两种编译器均构建并简单运行。静态编译器在 PATH 不含 SDK／clang 的环境中完成 checked run 和默认 unchecked aot；两种 1.0.8 VSIX 的版本、实际变体和内嵌 EXE 字节核对一致。生成两种 MSI。

Lua 从固定官方 URL 下载并核验 SHA256，构建性能宿主后验证版本和简单运算，没有运行性能套件。Linux x64／ARM64、macOS ARM64 仅核对固定官方发布元数据和平台选择逻辑，本轮未在这些系统执行下载或 AOT 验收。

书稿中英文交换 24／25 章，中文 LaTeX/PDF 同步；PDF 301 页，仅核对目录及两个章首页。第 24 章为不安全代码，第 25 章为 AOT 导出。

具体版本、SHA256、大小与结果见 [verification.json](verification.json)。当前结果不替换此前同版本其他编译器快照的测量，也不构成完整回归或性能结论。
