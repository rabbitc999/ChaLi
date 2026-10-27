# ChaLi 文档

<!-- LANGUAGE NAVIGATION -->
[English](en/README.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

已实现功能、默认配置和平台边界统一见[当前能力与实现边界](17-capabilities.md)。

>
> 英文版与中文版内容对应，代码片段两边完全一致、都经过实测运行。中文版优先维护；
> 两处说法有出入时以中文版为准。

**一次编写，处处运行。** 一份逻辑，从嵌入式 Linux 到云端；值语义、非空引用、无 GC。

ChaLi 让业务逻辑只写一次：同一份源码由 LLVM AOT 编译成原生机器码，在桌面、服务器和嵌入式 Linux 上跑出同一套语义，平台差异交给 `#abi` 声明或导出的 C ABI；没有垃圾回收器，资源在离开作用域时确定性释放。

当前语言与工具链版本见 [`VERSION`](../VERSION)，变更记录见 [`CHANGELOG.md`](../CHANGELOG.md)。

让 AI 编写 ChaLi 程序可使用仓库自带的 [ChaLi 编程 Skill](../skills/chali-programming/SKILL.md)，其中包含语法选择、标准库查阅、宏示例与运行验证步骤。

编译与运行的图解见 [编译过程](../README.cn.md) 和 [运行过程](../README.cn.md)。

---

## 你想做什么？

| 你是谁 | 从哪开始 |
| --- | --- |
| 想从头学 ChaLi，一章只讲一件事 | 入门书[《玩转 ChaLi 语言》](book/README.md)（第 1 章装工具链，一路到综合项目） |
| 第一次听说 ChaLi | [使命宣言](00-vision.md) → [5 分钟快速开始](01-quickstart.md) |
| 评估它能不能用在我的项目上 | [使命宣言](00-vision.md)（含"不适合谁"）→ [跨平台部署](06-deployment.md) |
| 开始写代码 | [语言速览](02-language-tour.md) → [语言参考](04-language-reference.md) |
| 想弄明白值语义和引用 | [核心概念](03-core-concepts.md)，显式转移所有权见语言参考的 `move` |
| 要把程序部署到目标宿主 | [交付安全](05-delivery-security.md) → [跨平台部署](06-deployment.md) |
| 安装编译器、生成 MSI/DEB/RPM 或构建其他 CPU 架构 | [安装包与多架构构建](39-installation-packages.md) |
| 想知道标准库有什么 | [标准库概览](07-stdlib-overview.md) → [库函数 API 参考](library/README.md) |
| 想看仓库里有什么能直接跑的 | [示例项目](13-projects.md)（GUI、Web、成绩管理、原生部署……） |
| 要和 C 库、C 宿主打通 | [宿主集成与 C ABI](08-host-integration.md) |
| 查命令行、配置、测试、多诊断与独立借用检查 | [工具链使用](09-toolchain.md) |
| 在终端查 API，或了解编辑器增量缓存 | [命令行手册](man/README.md) → [LSP 增量提示与缓存](man/lsp.md) |
| 使用 VSCode 的工程提示、标准库文档和在线依赖 | [编辑器插件](../vscode/README.md) |
| 想知道产物落在哪、旧格式怎么迁移 | [15 构建产物与中间文件](15-artifacts.md) → [16 原生产物符号与旧格式迁移](16-compact-symbols.md) |
| 用注解 / 反射，或接宿主内存 | [19 注解与反射](19-reflection.md)、[20 内存分配器](20-memory-allocator.md) |
| 查语法的权威写法 | [EBNF 语法参考](14-grammar-ebnf.md) |
| 想看用"方块世界"讲明白的 ChaLi | [99 方块里的 ChaLi 语言](99-chali-in-blocks.md) |
| 要升级版本，怕代码突然不编了 | [21 语义冻结清单](21-semantics-frozen.md)（核心契约、兼容性与实现边界） |
| 想确认哪些写法已经冻住 | [39 语法冻结清单](36-syntax-frozen.md)（词法、关键字、结构形状与改语法同步项） |
| 想知道 ChaLi 这五个字母各承诺了什么 | [40 名称冻结：ChaLi 五个字母](37-name-freeze.md)（C/H/A/L/I 各一条契约与可跑判据） |
| 想参与贡献 / 看路线 | [路线图与生态愿景](10-roadmap.md) |

---

## 文档结构

| 编号 | 文档 | 面向 | 讲什么 |
| --- | --- | --- | --- |
| [00](00-vision.md) | 使命宣言 | 决策者、潜在用户 | 软件交付的三重困境，ChaLi 的回答，适合谁 / 不适合谁 |
| [01](01-quickstart.md) | 5 分钟快速开始 | 第一次用 ChaLi 的人 | 安装、第一个程序、常用命令速查 |
| [02](02-language-tour.md) | 语言速览 | 有经验的开发者 | 用可运行的小片段走一遍 ChaLi 长什么样 |
| [03](03-core-concepts.md) | 核心概念 | 准备深入使用的人 | 值语义、非空引用、无 GC 的资源管理，为什么这样设计 |
| [04](04-language-reference.md) | 语言参考 | 日常写代码的人 | 按主题查阅语法、语义、示例与常见错误（含泛型嵌套、默认类型与单态化边界） |
| [05](05-delivery-security.md) | 交付安全 | 需要对外交付的团队 | 交付形态选择、导出与分发、符号与调试信息、边界声明 |
| [06](06-deployment.md) | 跨平台部署 | 做部署的人 | checked 运行与 AOT 交付、嵌入宿主、桌面 / 嵌入式 Linux、资源与边界 |
| [07](07-stdlib-overview.md) | 标准库概览 | 想知道"能做什么"的人 | 按能力域列出现有模块与核心 API |
| [08](08-host-integration.md) | 宿主集成与 C ABI | 做集成的人 | 胶水绑定、abi 块、类型映射、可空引用 |
| [09](09-toolchain.md) | 工具链使用 | 日常用 chali 命令的人 | 命令行、chali.toml、在线依赖与共享缓存、标准库预编译单元、编译选项、测试、LSP、显示语言 |
| [10](10-roadmap.md) | 路线图与生态愿景 | 贡献者、长期用户 | 短中长期目标与可验证里程碑 |
| [13](13-projects.md) | 示例项目 | 想直接跑东西的人 | 仓库里的应用型与能力型示例：位置、运行命令、验证入口与前置条件 |
| [14](14-grammar-ebnf.md) | EBNF 语法参考 | 查文法细节的人 | 从 `source/chali.gram` 生成的全部产生式与词法登记，权威语法的可读形式 |
| [15](15-artifacts.md) | 构建产物与中间文件 | 想知道文件落在哪、交付带什么的人 | `.chali/` 目录、中间产物清理规则、交付清单 |
| [16](16-compact-symbols.md) | 原生产物符号与旧格式迁移 | 从旧版本升级的人 | `.svmb` 与符号压缩删除后的替代做法 |
| [17](17-capabilities.md) | 当前能力与实现边界 | 评估与集成人员 | 语言、VM、借用、反射、标准库及目标宿主的统一说明 |
| [18](18-panic-trace.md) | panic 与源码行号 | 排查崩溃的人 | 失败输出、函数栈、源码位置与压缩隐藏策略 |
| [19](19-reflection.md) | 注解与反射 | 用注解 / 动态调用的人 | 声明元数据、类型化查询与封闭值动态调用 |
| [20](20-memory-allocator.md) | 内存分配器 | 接宿主内存的人 | 池与容量统计、分配范围检查、外部缓冲区接入 |
| [21](21-semantics-frozen.md) | 语义冻结清单 | 要采用、或要升级的人 | 核心语义、版本规则与仍需完善的实现边界 |
| [22](22-resource-lifetime.md) | 资源生命周期与借用控制流 | 修改运行时的人 | 分配身份、资源回收合并、临时所有权与闭包借用参考 |
| [99](99-chali-in-blocks.md) | 方块里的 ChaLi 语言 | 喜欢比喻、想先建立直觉的人 | 用方块世界讲 MC（Magic-C）的每条规矩：值语义、非空引用、无 GC、迭代器、穷尽 match，以及删掉了什么 |

---

## 配套资源

- [`example/samples/`](../example/samples/) —— 可运行的正式样例，标准输出在 [`example/expected/`](../example/expected/)。
- [`example/samples-error/`](../example/samples-error/) —— 预期报错的样例，诊断文本在 [`example/expected-error/`](../example/expected-error/)。
- [`doc/examples/`](examples/) —— 需要配套工程布局的示例（ABI、SQLite、GUI）。
- [`doc/examples/`](examples/) 里的配套工程总览见 [13 示例项目](13-projects.md)。
- [项目 README](../README.cn.md) —— 构建、快速使用、调试、配置。
- [入门书《玩转 ChaLi 语言》](book/README.md) —— 从零开始的 26 章教程与附录，含分层习题、经典作品改编番外，以及前端编译与 AOT 流程图；完整示例与生成内容按对应回归核验。
- [完整语法参考（EBNF）](14-grammar-ebnf.md) —— 文法的权威形式，由 [`source/chali.gram`](../source/chali.gram) 生成。

## 英文文档

[`doc/en/`](en/README.md) 是英文版，与上面各篇一一对应（含项目 README 的英文版，共 14 篇）：

| 英文版 | 对应这篇中文 |
| --- | --- |
| [../README.en.md](../README.cn.md) | [项目 README](../README.cn.md)（构建、快速使用、调试、配置） |
| [en/README.md](README.md) | [文档入口](README.md) |
| [en/00-vision.md](00-vision.md) | [00 使命宣言](00-vision.md) |
| [en/01-quickstart.md](01-quickstart.md) | [01 快速开始](01-quickstart.md) |
| [en/02-language-tour.md](02-language-tour.md) | [02 语言速览](02-language-tour.md) |
| [en/03-core-concepts.md](03-core-concepts.md) | [03 核心概念](03-core-concepts.md) |
| [en/04-language-reference.md](04-language-reference.md) | [04 语言参考](04-language-reference.md) |
| [en/05-delivery-security.md](05-delivery-security.md) | [05 交付安全](05-delivery-security.md) |
| [en/06-deployment.md](06-deployment.md) | [06 跨平台部署](06-deployment.md) |
| [en/07-stdlib-overview.md](07-stdlib-overview.md) | [07 标准库概览](07-stdlib-overview.md) |
| [en/08-host-integration.md](08-host-integration.md) | [08 宿主集成与 C ABI](08-host-integration.md) |
| [en/09-toolchain.md](09-toolchain.md) | [09 工具链使用](09-toolchain.md) |
| [en/10-roadmap.md](10-roadmap.md) | [10 路线图与生态愿景](10-roadmap.md) |
| [en/13-projects.md](13-projects.md) | [13 示例项目](13-projects.md) |

几点说明：

- **只翻译说明文字，代码片段原样保留**（含片段里的中文注释）。文档承诺"每段代码都实测运行过"，
  校验脚本 `tmp/{tour,core,ref}_check.sh` 是从脚本内的 heredoc 生成片段的；翻了注释两边就对不上，
  这个承诺就弱了。
- **中文版优先维护**。两版说法有出入时以中文版为准。
- **没翻的部分**：底层规格 7 篇（[14](14-grammar-ebnf.md)–[20](20-memory-allocator.md)）、`doc/examples/`（配套工程）、
  `doc/book/`（入门书《玩转 ChaLi 语言》）。
  英文版里指向这几处的链接仍链到中文原文。
- 根目录 README 保持中文，另有一份逐节对应的英文版 [`README.en.md`](../README.en.md)。
- 英文版内的相对路径多一层（指向 `example/` 写 `../../example/`）。

## 其余入口

- [11 借用控制流与算法验证](11-algorithm-validation.md)：独立数据流检查、链表 / BST / DFS / BFS、NOIP2015 运输计划与暴力对照。

- [12 LLVM AOT 跨语言性能](12-performance.md)：五个微负载下 AOT 与 C、C++、Rust、Go、Java、Node、Lua、Python 的中位数对照，checked 与 unchecked 的差距，删除内部四元式优化对编译期与运行期的影响，以及原始样本与计时口径。

- 静态 SQLite 与 MySQL/MariaDB 连接入口：[SQL 接口](../example/sql-chali/doc/01-sql-library.md)、[SQLite 实库回归](../tests/chali_sqlite.py)、[连接 ABI 夹具回归](../tests/chali_sql_connections.py)、[MySQL/PostgreSQL 实库回归](../tests/chali_sql_live.py)。

- [Any、显式泛型与通用数据工具](23-any-generics-data.md)

- [资源所有权移动](24-ownership-move.md)

- [字面量定型、运算符推导与宏](25-literals-operators-macros.md)

- [执行与交付](26-execution-delivery.md)

- [自定义错误传播与 if 表达式](27-error-propagation-if.md)

- [匹配、插值、泛型与作用域清理](28-match-interpolation-scope.md)

- [返回表达式、具名闭包与字段派生](29-declaration-sugars.md)

- 带值退出的默认移动：[语义与清理顺序](04-language-reference.md#带值退出的移动与清理)、[资源身份和析构回归样例](../example/samples/break_move.chl)。

- [线程、通道与显式事件](30-thread-concurrency.md)

- [精确符号计算与高精度数学](31-symbolic-computation.md)
- [标准库存储、复制与字符串方法](32-opcode-values-strings.md)
- [反射序列化与动态 SQL](33-reflection-serialization-sql.md)

- [编译器安全边界与定向回归](34-compiler-safety.md)

- [Opcode 与标准库宿主记录](35-opcode-storage.md)

- [语法冻结清单](36-syntax-frozen.md)

- [名称冻结：ChaLi 五个字母](37-name-freeze.md)

- [AOT 编译入口](38-aot.md)
- [AOT 降低开关与计数](40-backend-ownership.md)
- [原始存储与 C ABI 指令](41-unsafe-abi.md)

- [在线构建依赖](42-build-dependencies.md)：SQLite／miniz 固定版本下载、摘要校验、缓存、离线复用与许可证。

- [原生 AOT 调试](43-native-debugging.md)：GDB 断点、单步、变量与 `--debug` 构建。
