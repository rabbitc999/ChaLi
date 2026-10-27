# 1.0.8 在线依赖、书稿与历史清理收尾

- [x] 下载锁定的预编译 LLVM/LLD 23.1.4、匹配 GCC 16.2 与 LLD，校验 26 个包及安装文件；完整静态编译器构建、DLL 路径隔离下的简单 checked run／unchecked aot 通过。
- [x] 外置 LLVM 改为在线下载，更新默认查找路径；external 编译器构建及简单 run 通过。
- [x] 外置工具链下载支持 Windows x64、Linux x64／ARM64、macOS ARM64 固定分发。仅 Windows x64 实际执行，其他平台没有声称 AOT 验收。
- [x] 删除入库 SDK 分片及 Lua 归档；Lua 性能宿主按需在线获取、校验源码并构建，版本与简单运算通过。
- [x] 交换书稿 24／25 章的中文／英文文件、目录、编号、引用；LaTeX／PDF 同步，目录和两个章首页抽查通过。
- [x] 生成 static／external 两种 1.0.8 编译器、VSIX 与 MSI，VSIX 版本／变体／编译器字节逐一核对一致。
- [x] 简单验证证据转存 benchmarks/results/dependency-download-1.0.8 和 dist/validation/dependency-download-1.0.8。历史性能记录未重标；没有运行全量 CI／回归／性能套件。
- [ ] Git 历史重写前保存并验证完整 bundle、原始引用和大对象清单；不删除文档 API 索引。
- [ ] 清理已确认的生成缓存、SDK 分片及已在线获取的依赖副本历史；提交标题中文化并去冒号前缀，保留贡献者身份和时间，保存提交映射。
- [ ] 清理无引用 Git 对象、重建索引，核对历史和源码树；使用原始对象号的 force-with-lease 更新 Gitee master 与现有标签，不推 GitHub。
- [ ] 结束相关进程、转存证据后清空 tmp，全部完成后随交付删除 TASK.md。
