# 当前已授权任务与验收状态

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](TASK.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

本轮目标版本与性能基线为 0.31.0；启动时仓库 VERSION 为 0.30.0，现已构建 0.31.0 的优化前编译器；历史测量保留其真实版本。用户已授权继续完成本文件全部任务、验收、文档与交付；本文件保留尚未完成的项目与已知问题，不把局部测试视为最终验收完成。

## 本轮追加：安装包、多架构与书稿维护

最新交付版本统一为 1.0.0，项目许可证 Apache License 2.0，作者小兔糖糖_OvO；0.31.0 保留为真实性能测量基线，不把旧测量改标为 1.0.0。用户授权将 Git 历史中的本人旧署名统一，并在最终交付时使用带租约的强制推送更新 origin；不推送其他远端。最终发行版已在书稿核实和内嵌资产更新后重建；Windows 两种 LLVM 变体及 Ubuntu/CentOS 的受支持目标均已生成。

用户要求本轮不运行测试门禁；此前完整门禁在第 90 项执行期间停止，前 89 项通过，日志保留在 tmp/limits031-ci-full-acceptance.log，不将其记录为完整通过。当前执行必要构建、打包、产物核对及用户明确要求的文档定向核实。

- [x] 自动探测 WSL；无可用 WSL 时只构建本机架构。x86 编译后启动失败，用户于 2026-10-25 明确允许不发布，默认构建和打包入口均已排除该目标。
- [x] 实际构建 Linux amd64、x86 与 ARM64，分别记录成功产物或明确阻碍；CentOS 使用真实发行版用户空间，不能把 Ubuntu 二进制重命名为 CentOS 版本。
- [x] Windows MSI、Linux DEB/RPM/tar.gz 快速安装，核对 PE/ELF 机器类型、版本、发行版与 SHA256。
- [x] 按最新命名统一为 chali-1.0.0-llvm-external-windows-x64，静态变体为 llvm-static；Linux 在相同位置填写实际发行版。
- [x] doc/book/en 提供英文技术书稿与附录，不翻译番外，不生成英文 LaTeX/PDF；man book 内嵌、选择与导出英文版。
- [x] 根据历史变更审阅全部维护中文档的当前语法；修正函数类型、入口返回类型、资源写法与错误协议等残留，同步相关生成资产。
- [x] 过长的历史变更按 0.x.x 补丁版本拆分，保留实际测量版本与既有 tag，不改标历史二进制。
- [x] 中文各章番外参考《孔乙己与闭包》的原作改写方式，加强幽默，技术对照留在章末；同步中文 Markdown、LaTeX、PDF。
- [x] 简化根目录及 ipage 的 AGENTS.md；根目录 51 行，默认维护后端，用户未提出前端需求时尽量不动，删除前端文法与解析器生成的强制要求。
- [x] README.md 改用英文，原中文逐字节保存为 README.cn.md；README.en.md 保留。独立提交 b33f538，已推送 origin/master。


## 当前优先任务：0.31.0 缺陷发现、修复与默认容器优化

本轮严格分阶段执行。发现阶段只增加测试、复现、测量与记录，不混入编译器、VM、AOT 或标准库的缺陷修复和性能优化；完成全部代表用例并冻结问题清单后才进入修复阶段。此前未完成任务保留，涉及同一问题时合并验收，不以本轮任务替代或隐藏历史欠项。

### 一、冻结基线并先找 BUG

- [x] 建立优化前 0.31.0 基线（已保存 tmp/chali-0.31.0-before-container-optimization.exe，LSP 自报 0.31.0，SHA256 a7138cfa20ba4558c14622b4cfda206cb4cbe68489661b7aaf04e5bc6f945897；宿主、LLVM 与构建配置已保存 tmp/limits031-baseline.json 并纳入冻结报告）：记录源码提交、实际程序版本、编译器 SHA256、构建选项、宿主/LLVM 版本、默认反射与最高优化；保存编译器和后续复测所需输入。历史 0.30.0 或更早数据不得改标为 0.31.0。
- [x] 常见业务代码约 25 个代表用例：覆盖默认 Map/List/Set、增删查改、字符串、结构体、错误传播、文件/数据转换、回调及模块组合；检查结果、诊断、退出与资源清理。
- [x] 炫技写法约 25 个代表用例：覆盖高阶函数、嵌套函数返回类型、泛型推导、闭包捕获/返回、容器内函数值、引用/所有权、组合表达式等。
- [x] 压力写法约 25 个代表用例：分开检查编译规模与运行规模，涵盖深层类型/嵌套、泛型实例、长表达式、大容器、扩容/冲突/删除、递归与内存预算；逐项设置超时和输入规模，禁止无界耗尽系统。
- [x] 三类各约 25 项均分别检查 VM 与 AOT，保留源码、期望、编译/运行命令、超时、退出码及 stdout/stderr。合法拒绝与崩溃、错算、卡死、诊断错误分别记录，不能把后端一致但共同错误判为通过。
- [x] 专项复现 `T fn(T) f<T>(T fn(T) a, T fn(T) b) { ... }` 的返回类型检测限制；覆盖显式/隐式类型实参、重载、返回闭包、嵌套函数类型、泛型结构/容器中的函数值以及不兼容返回值的拒绝，检查类似限制。
- [x] 完成整批发现后冻结缺陷清单和证据，再开始修复；不得发现一个问题就立即夹带修复。

### 本轮发现阶段冻结结果

- 冻结源码基线提交 29c6150，优化前二进制 SHA256 见上；原始命令、输出字节、源码、期望、宿主/LLVM 和构建配置保存于 `tests/fixtures/compiler_limits/discovery_0_31_0.json`。
- 业务 25 项：VM/AOT 各 25 项通过。文件字符串用例初次因隔离目录缺少参数指定的 tmp 子目录失败，补齐夹具后两端通过，不计为编译器缺陷。
- 高级 25 项：两端均为 23 项通过、1 项非法整数返回值被正确拒绝、1 项返回组合闭包给出错误的类型诊断。初始用例的默认借用还有生命周期限制，进一步使用合法复制捕获确认待修 F001：`T fn(T) compose<T>(T fn(T) a, T fn(T) b) { return fn copy(T x) { a(b(x)) }; }` 自动推导失败，而显式指定 int64 通过；补充发现证据见 `tests/fixtures/compiler_limits/return_type_followup_0_31_0.json`；直接返回已有函数和嵌套函数值调用已通过。
- 压力 25 项：VM 22 项通过、3 项有诊断的预算拒绝；AOT 23 项通过、2 项有诊断的预算拒绝，无崩溃或挂起。512 项普通加法表达式触发片段规模预算；64 MiB 执行预算下申请约 128 MiB 动态数组被拒绝；8192 元素固定数组在 VM 的 64 MiB 预算内无法完成执行准备，AOT 通过。这些是当前配置下的测量点，不是所有宿主的绝对极限。
- 压力夹具中的 C 式 for、String.len 和文件父目录假设已修正并重测，只修测试夹具，未混入编译器/VM/AOT/标准库修复。发现阶段结束后才允许改实现。
- 冻结报告 SHA256：96604daddf6cff5295bcd2cd80e48df09928122cf3849194065e8bbb35948297。补充类型推导变体、定位根因和修复回归在后续阶段继续记录，不能将这一轮发现当作全部任务完成。

### 二、按分类修复（VM 与 AOT）

发现阶段已独立提交 2b35c7e；补充合法复制捕获也在优化前二进制上复现。F001 已补齐自动实例化后新增闭包的提升和登记，tests/chali_generic_function_returns.py 的 12 项 VM/AOT 正反回归通过（tmp/limits031-f001-regression.log）。覆盖自动/显式类型实参、数值和浮点、闭包复制、List/Map 存储、嵌套组合；非法整数返回与返回局部借用继续被正确拒绝。整批 75 项已复测（tests/fixtures/compiler_limits/verification_0_31_0.json）：业务两端各 25 项通过，高级两端各 24 项通过及 1 项非法返回正确拒绝，压力两端仅保留已记录的预算拒绝，无新增失败。全部冒烟仍在后续验收。

- [x] 常见业务代码发现的问题必须修复并保留最小复现回归，验证两个后端的结果、错误传播、生命周期和检查时机。
- [x] 炫技写法按修复难度处理；成本合理则修复，成本高则提供明确的编译器警告或错误和可定位原因，不允许静默错算、崩溃或挂起。
- [x] 压力写法优先保持有界失败和明确诊断，可通过编译器警告或报错界定规模/预算，不优先扩大能力；记录触发阈值与宿主配置，不把单机测量写成普遍极限。
- [x] 尝试修复上述泛型高阶函数返回类型检测及同类问题；补齐合法代码和非法返回类型的正反回归，必要时同步权威文法、语义文档及 LSP。
- [x] 每个问题明确标记已修复、以诊断限制、未完成及原因；修复后复测整批三类用例，不能只复测单个最小复现。

- [x] F002：按函数形状和其他实参已绑定的类型筛选回调重载，保留目标函数签名；没有唯一候选时给出诊断。扩展 18 项 VM/AOT 正反回归全部通过，包含显式泛型实参、同名不同参数数量/类型、字符串返回、嵌套工厂和歧义拒绝（tmp/limits031-f002-complete-regression.log，tmp/generic-function-returns-vn26z5x8/report.json）。发现记录仍保留；第一次完整冒烟已暂停，最终完整门禁另行执行。

### 三、默认 Map、List、Set 调用优化与基准

- [x] 缺陷修复后分析默认容器调用热点，分别尝试减少 VM 与 AOT 的调度、参数封送、重复查询、复制与分配开销，保留边界/存活检查、值语义、迭代与错误行为。
- [x] 为 Map/List/Set 分别准备代表性的创建、查询、写入/更新、插入/删除与遍历负载，避免只测单一理想情况；固定输入、操作次数、结果校验和分配策略说明。
- [x] 使用最终编译器测量 VM 与 LLVM AOT，并与其他语言作同轮对照，至少保留 C/C++、Python、Lua；记录解释器/编译器版本和选项，明确不同语言容器与分配策略差异。
- [x] 使用优化前基线与优化后版本交错多次测量，保留预热、原始样本、统计与校验值，区分编译时间、启动时间和执行耗时；如无改善或存在退化，记录实测结果，不承诺全部优化必然提速。
- [x] 更新 doc/12-vm-performance.md 及英文页与相关容器说明，使用本轮真实版本/哈希，不重标旧测量。

容器测量已完成：三次进程各五轮，12 种负载，VM/AOT 优化前后及 C++/Python/Lua 共七种配置。主测几何平均耗时下降：List VM 3.19% / AOT 12.67%，Map VM 3.55% / AOT 15.92%，Set VM 5.94% / AOT 11.79%。VM List 删除主测约慢 1.5%，七进程复核约快 1.3%，两份样本均保存，不宣称这一小幅变化稳定提速。原始样本和双语表格已同步，完整冒烟另验。

- [x] 按用户补充口径明确性能比较：中英文性能页保留 VM、LLVM AOT、C/C++、Python、Lua 的执行耗时，新增 12 项容器的 AOT/VM 提速及 VM 相对其他语言耗时倍数，并单列小函数、固定数组读写和列表热点的 AOT/VM 提速。全部比例由原始中位数生成；语言能力与预算边界另见对应文档，不替代性能对比。文档链接检查 125 页、缺失 0 处。

### 四、全部冒烟、文档与发布

- [ ] 修复及优化完成后执行全部冒烟：完整正常/错误样例、前端/四元式/VM、AOT、标准库、LSP/VS Code、独立工程导出及相关宿主服务；发版前运行 `sh ci.sh --full`，保留实际结果和失败项复验。
- [ ] 完成历史欠项中仍适用的验收，包括学生系统最终 AOT 尝试；相关文档仅描述实测能力，书稿变更同步 Markdown、LaTeX、PDF。
- [x] 程序版本及 chaliCompilerVersion 按追加要求同步为 1.0.0，插件保持 1.0.0；同步内嵌资产、LLVM 静态/外置两种编译器及 VSIX，核对程序版本、命名和内嵌字节。
- [ ] 发布前逐项核对全部要求、结果、文档及产物；仅在全部完成后删除 TASK.md，删除随最终交付提交；未完成则继续保留清单，不提前发布。
- [ ] 中文提交并推送 origin/master，不推送 github 远端。

## 已实施并有局部验证

- [x] 保存优化前编译器：提交 17debd4，SHA-256 044d6e2c52cc9b4291df42b3babc9774f395311271cecb4b325996d67975b0e7。
- [x] 接通寄存器复制和同循环整数声明路径，优化默认堆范围查询，添加四元式完整整数写后读转发；尚有下述运行崩溃需要修复。
- [x] 堆构造使用 heap T / heap T(args)，拒绝 heap new T 与 move(&a)，同步文法、示例、诊断和书稿。
- [x] 添加 export 函数标记，AOT 动态库允许无 main，全局初始化与清理由 chali_init / chali_finish 承接。
- [x] 动态库 C 调用及全局 opcode 连续两轮初始化、销毁验证通过；VM 单测 350 项断言、20 用例 × 5 后端的先期差分通过。
- [x] 修复 VS Code 宏树单子节点靠左造成的断线；浏览器检查 24 种宽度、Tab、缩放组合及折叠与明暗主题通过。
- [x] 同步中英文功能文档、书稿 Markdown / LaTeX；本机 PDF 已生成，284 页。
- [x] 尝试学生信息管理系统 AOT：实际入口编译失败，首个阻碍为 std::json::__valueCopy。用户明确要求留待下一版本处理，书稿继续保留该系统的 VM 运行说明。

## 未完成项

- [x] 修复 example/samples/chain_expression_cleanup.chl 与 compound_assignment.chl 的寄存器执行核崩溃：操作数去重可让 Declare 漏过帧槽验证，导致无效槽号写入。补齐所有声明映射的绑定检查和服务边界保护后，两条样例默认 VM 均通过；证据 tmp/task-compound_assignment.log、tmp/task-chain_expression_cleanup.log，完整门禁另验。
- [x] 复验两条解析诊断期望：新增 export 后，extern_cast_forbidden 的候选数 106 → 107、macro_end_for_orphan 的候选数 44 → 45；tmp/task7-check.log 验证均通过。
- [x] 完成完整门禁及失败项定向复验。原始 ci.sh --full 已结束：134 项中 128 项通过、6 项失败（tmp/task10-ci-full.log）。278 个正常样例、312 个错误样例、VM 350 项断言与双核差分通过。静态库解压依赖、闭包签名计数、反射名称限定三项已修复并定向复验；内嵌手册使用新编译器复验通过（tmp/task16-man.log）。书稿 46 个完整程序及全部 LaTeX 同步复验通过（tmp/task16-book.log）；工程导出的独立构建、VM/TOML、AOT 入口及所选标准库样例复验通过（tmp/task16-generator.log）。原始六项失败均已修复并逐项复验，未重新宣称原始运行零失败；后续功能与最终产物仍按各条继续验收。
- [x] 在修复及验收完成后，用最终编译器交错测量小函数、固定数组读取、固定数组读写，更新 doc/12-vm-performance.md 及英文页，并重跑 VM / AOT / Lua 等对照。当前 doc/12-vm-performance.md 及英文页已更新为 0.31.0 实测；保留 0.30.0 历史编译器的实际身份和交错结果，详见 benchmarks/results/hotspots-0310/、array-rw-0310/ 和 containers-0310/。
- [x] 同步最终内嵌资产、内置 LLVM / 外置 LLVM 两种编译器及 VSIX，核对版本和字节；两种编译器及 VSIX 已重新打包，逐字节核对通过；默认 VSIX 和 Windows x64 MSI 使用静态 LLVM。
- [ ] 完成上述必要验收和产物同步后，逐项核对证据，再删除本文件并提交。

## 本机会话证据（tmp 不入库）

- tmp/three-hotspots-ci-full.log、tmp/ci/logs/028.log：完整样例回归的 4 项失败。
- tmp/three-hotspots-crash-probe.log、tmp/three-hotspots-crash-gdb.log：两处崩溃的模式对照与调试堆栈。
- tmp/three-hotspots-export-aot.log：动态库与全局资源生命周期验证。
- tmp/three-hotspots-students-aot.log：学生系统 AOT 退出码 1。
- tmp/check_macro_connector_geometry.py：修复前单子节点中心偏移 14.625px；修复后几何检查通过。

## 本轮任务：标准库 Error 接口统一

最初授权仅修改标准库及必要使用适配，不回归 AOT。用户随后要求完成本文件全部任务，已将 AOT 修复与回归一并纳入当前范围。

本项与下述默认模块/LSP、AOT 两项一起登记并持续跟踪；已有实施改动尚未完成全部验收，不视为已交付。

- [x] 新增公共 Error 协议，11 种标准错误实现可读诊断；tmp/ChaLi-task7.exe 的 tests/chali_std_errors.py 全部通过，证据 tmp/task7-errors.log。
- [x] 标准库 Result 的文本错误改用实现 Error 的错误对象，包括反射序列化内部桥接；原始诊断、枚举和位置数据及 ? 传播均有独立测试。
- [x] 适配受影响的标准库调用、示例和测试；全面检索并更新仓库内所有与标准库有关的 Markdown 文档，包括 README、doc/ 中英文功能文档、API 索引、命令手册、书稿、skills 和 VS Code 说明，核对错误类型、接口签名及示例，不只更新 API 生成页。
- [x] 标准库相关书稿改动同步生成 LaTeX 和 PDF，验证链接、代码示例与生成资产一致。
- [x] 重建内嵌资产和编译器，执行标准库 VM 验证，并纳入下述 AOT 能力对齐验收；既有 VM 问题单独记录。
- [x] 排查新增标准错误协议回归的 arena 耗尽失败，确认测试覆盖错误接口调用、具体错误数据保留及 Result 错误传播。综合 12 模块样例再次触发编译池用尽（tmp/task11-all-errors-1g.log）；--memory-size 配置执行堆，不改变前端默认 512 MiB 编译池。已按模块隔离回归，并逐模块验证具体错误、Error 泛型约束与接口借用分发（tmp/task13-errors.log）；已复验综合样例的编译池大块分配边界并修复未捕获异常：当前两端退出 1，报告容量、活动量和请求量；综合样例仍受默认 512 MiB 编译预算约束，未宣称其已正常执行。证据 tmp/limits031-f004-budget.log 与 tmp/compiler-budget-9qpbk52k/report.json。
- [ ] 同步本轮交付并提交推送，未完成的历史验收继续保留。

## 构建增量与中文输出

- [x] 所有项目临时文件夹统一在 ChaLi/tmp，构建与测试子进程继承统一路径；原生 AOT、LSP、ABI 工具临时目录不使用同级 tmp 或系统目录。Python、Node 和原生工具已接入；定向测试验证外部目录覆盖、上级启动与子进程环境均受约束（tmp/task10-temp-test.log），工程生成器同级路径及 check.sh 外部覆盖已修复，tmp/ChaLi-task10.exe 已重建成功，书稿工具临时目录亦已统一并有定向回归；依赖下载和 Git 检出的中转目录也迁入统一 tmp。最新路径策略回归通过（tmp/task17-temporary-paths.log），Python 在线依赖回归通过（tmp/task17-online-unit.log），原生依赖工具已随 tmp/ChaLi-task17.exe 重建，在线依赖完整回归通过（tmp/task17-online-native.log）；全量门禁在独立条目跟踪。

- [x] 内嵌资产生成器维护输入、生成脚本和产物的 SHA256 文件，内容相同直接跳过压缩与生成；输出缺失、损坏、索引改变或输入改变时重新生成。摘要写入 build/embedded-sha256/，回归证据 tmp/task8-asset-cache-test.log。
- [x] gen.sh 入口统一 Python UTF-8 编码，验证在继承 cp936 输出编码时仍产生完整 UTF-8 中文，证据同上。
- [x] 使用真实完整资产核对首次生成与再次命中的耗时及产物摘要，纳入最终构建和门禁。完整资产已生成；连续两次 gen.sh 命中约 6.43 / 6.05 秒，产物 SHA256 和 mtime 保持不变（tmp/task10-gen-cache.json），真实完整资产已用独立 SHA 缓存计时：冷缓存 92.19 秒，命中 0.293 秒；压缩工具预先构建，生成源 SHA256 与 mtime 均不变（tmp/limits031-asset-generation.json），默认缓存未被替换。
- [x] Windows x64 静态 LLVM 编译器按追加要求命名为 chali-1.0.0-llvm-static-windows-x64.exe；Make 和发布脚本按 VERSION 生成命名，外置变体使用 llvm-external；待核对实际静态构建、发布与 VSIX 字节。

## 新增任务一：默认 program 模块与 LSP 宏补全

用户已要求实施并完成此任务，按以下范围验收：

- [x] 用户源码未显式声明 module 时，归属小写 program 模块；显式 module 声明继续按指定名称归属。新构建 tmp/ChaLi-task7.exe 已验证，证据 tmp/task7-default-module.log。
- [x] 同步模块加载、限定函数调用与 LSP 默认身份；tests/chali_default_module.py 验证 program:: 限定调用、import program、$module 及显式 custom / customCase 模块名称保留。LSP 宏补全使用同一构建复验通过，证据 tmp/task7-macro-completion.log。
- [x] LSP 宏补全包含已导入外部模块宏，修正 $ 限定模块名称识别；tests/chali_lsp_macro_completion.py 验证通过（tmp/ChaLi-task3.exe）。
- [x] 输入 $ 后只提示宏，过滤普通符号和模块名；$、$ext、$helpers:: 三种位置验证通过，检查插入文本不重复 $。
- [x] 同步说明与对应编译器、LSP、VS Code 回归及交付。

### 函数与宏的重载详情提示

用户追加要求：参考 Visual Studio 的交互方式，此项纳入本轮实施及验收。

- [x] 函数详情和签名帮助只展示当前选中重载的签名、参数、返回值和说明，不把其他重载详情混在同一项中。tests/chali_lsp_overloads.py 使用 tmp/ChaLi-task8.exe 验证通过，证据 tmp/task8-overloads.log；tmp/ChaLi-task9.exe 复验见 tmp/task9-overloads.log，VS Code 过期结果拒绝测试通过。
- [x] 输入尚不能确定唯一重载时，提供可切换的候选；以“函数 [1/4]”一类形式显示当前候选序号与筛选后的候选总数。
- [x] 根据已输入的参数数量、可推断类型及实际重载匹配规则动态筛选候选；兼顾默认参数、可变参数、泛型和尚未输入完整的实参。
- [x] 编辑参数、移动光标或切换候选时，正确更新当前参数和选中重载；不展示过期请求的结果。
- [x] 宏采用相同的候选选择和详情展示方式，以“宏 [1/4]”一类形式显示；按实际宏匹配规则处理参数分组、参数数量、片段类别及可用类型信息。 宏重载按实际分组与数量匹配，片段及类型条件由宏体匹配分支处理，不虚构类型重载；见中英文工具链说明。
- [x] 同步原生 LSP、必要的 VS Code 展示及中英文说明；添加函数和宏的单候选、多候选、自动缩减、候选切换与不完整输入回归。

## 新增任务二：修复 AOT 全部已知问题并与 VM 能力对齐

用户已要求实施并完成此任务；此条扩展此前“留待下一版本”的 AOT 范围，以下项目须以实际验收为准。

- [x] 汇总并修复全部已知 AOT 问题，包括学生系统当前遇到的 std::json::__valueCopy 调用失败。
- [x] 支持 AOT 调用闭包，覆盖捕获、函数值传递、返回值、所有权与清理，并与 VM 行为对照。
- [x] 补齐尚不能由 AOT 调用的标准库模块与运行时服务，覆盖模块间调用及所需宿主依赖。
- [ ] AOT 的语言语义、可用能力与限制和 VM 保持一致；逐项核对入口、错误、检查、生命周期及宿主平台边界，不保留额外的 AOT 特有能力缺口。
- [x] 最后尝试学生信息管理系统 AOT：原始工程与仅替换数据库名的副本均编译成功，服务启动与 18 项 HTTP 业务检查通过，独立库已删除；命令和结果见 tests/fixtures/compiler_limits/student_aot_acceptance_0_31_0.json，使用实际 0.31.0 编译器，未改标为 1.0.0。中英文书稿与示例 README 已补 AOT 启动及依赖说明，最终中文 LaTeX/PDF 与两种 Windows、三种 Linux 编译器的内嵌书稿已同步。未运行 CI 门禁。
- [ ] 完成闭包与标准库的 VM/AOT 差分、性能对照、内置 LLVM / 外置 LLVM 两种编译器及 VSIX 验收，同步文档和产物。

### 补充发现 F003：复杂 C ABI

- [x] 业务级 ABI 结构体/引用/缓冲区差分：同一现有 abi_buffers 示例在 VM 输出 abi-values-ok，AOT 编译成功但补齐 DLL 搜索路径后退出 0xC0000005。发现证据单独冻结于 tests/fixtures/compiler_limits/abi_followup_0_31_0.json；根因是复杂 ABI 类型被兜底映射为整数，以及语言堆偏移未经封送直接当作 C 值。先冻结发现，再修复，不纳入已通过的 75 项伪装覆盖。

F003 已接入类型化宿主封送，修正窄标量存储位宽及返回指针引用实参副本的生命周期。VM/AOT 值、引用、缓冲区及错误分支通过（tmp/limits031-f003-values-vm.log、tmp/limits031-f003-values-lifetime.log），AOT 所有权、闭包及嵌套 C 回调通过（tmp/limits031-f003-contracts-lifetime.log）。复杂 ABI 导出需要兼容 GNU C++17 宿主编译器；语言函数仍直接降低到 LLVM。新增两项完整门禁，静态 LLVM、独立导出工程和最终全量验收仍待执行。

### 补充发现 F004：前端编译预算异常退出

- [x] 现有 12 模块标准错误综合样例在 VM/AOT 编译时都因前端池耗尽抛出未捕获 std::bad_alloc，退出码 3；不能当作正常预算拒绝。已冻结 tests/fixtures/compiler_limits/budget_followup_0_31_0.json，分别在约 97.6 / 86.9 秒复现。第二次完整门禁在无 panic 构建阶段暂停，通过项与日志保存 tmp/limits031-ci-attempt2-logs；先修复诊断并核对不再使用的编译快照占用，再重启完整门禁。

F004 复验：VM/AOT 均为有诊断的预算拒绝，退出 1；请求 80 MiB 连续块，池容量 512 MiB，失败时活动量分别为 421378384 / 323844320 字节。及时释放已完成重放的解析快照，未提高默认池预算，也不把池内连续分配边界写为总内存已全部使用。18 项高阶函数两端复验通过（tmp/limits031-f004-functions.log）。

### 补充发现 F005：Windows AOT 命令行编码

- [x] 同一组六个参数（中文姓名、非 BMP 字符、空串、空格引号、JSON 及中文路径），VM 保留 UTF-8，AOT 主入口直接使用窄 argv，中文按本机代码页进入程序，非 BMP 字符丢失。先冻结 tests/fixtures/compiler_limits/arguments_followup_0_31_0.json，再修复入口的宽字符到 UTF-8 转换；该问题会影响学生 CLI 的中文 JSON 业务参数，需在最终学生系统验收前解决。

F005 已通过编译器同源的 CRT 宽参数转换保留 Windows 进程入口 UTF-8。三组 VM/AOT 输入（含空列表、空串、引号、反斜线、中文、非 BMP 字符、JSON），以及中文和空格产物路径均通过；证据 tmp/limits031-f005-arguments.log、tmp/aot-arguments-b3ui1avu/report.json。原 UTF-8 宿主参数数组接口保持原协议。

### Markdown 语言切换

- [x] 全部 236 篇受维护 Markdown 添加当前语言标记及可用语言链接；正文优先链接同语言文档，缺少译文保留中文。指定日文口号已删除。
- [x] API 手册、性能页与 CHANGELOG 生成器统一使用 tools/documentation_navigation.py，重复生成无漂移；中文 LaTeX/PDF 已重新生成，共 289 页，封面及 AOT 章节已检查。
- [x] 最终内嵌资产生成完成后，构建两种 LLVM 变体及多架构发行包；本轮不执行测试门禁。

- [x] 书稿离线链接默认使用 https://github.com/rabbitc999/ChaLi，Markdown 继续采用可离线浏览的相对路径；LaTeX/PDF 从转换源统一生成，不向 GitHub 推送。

### 书稿与其他文档的内容核实

- [x] 检查全部 236 篇受维护 Markdown；printfn、被误转义的占位符、未闭合代码围栏均未发现。四处 0b1010 均明确作为错误写法说明。
- [x] 逐块核对中文书稿 589 个代码块，Markdown 到 LaTeX 的代码内容、行和相对缩进一致。
- [x] 修复行内代码反斜杠的重复 LaTeX 转义，修复 SQL 示例中的反引号围栏及转换支持；重新生成 289 页 PDF，异常可见转义从 6 页降为 0 页，渲染检查 Hello、空容器、字符串与 SQL 等 8 页。
- [x] 用实际编译器核对 b1010、空 List、空 Set，输出 10 0 0；0b1010 被语法诊断拒绝。书稿两处表格及正文的词法错误分类改为语法错误。未执行测试门禁，未提供的外部 EPUB 不纳入核实。
- [x] 将核实后的书稿重新同步到全部内嵌资产与发行包，避免交付旧书稿。

### 最终产物证据与用户授权的交付范围

- [x] Windows x64 静态 LLVM 和外置 LLVM：EXE、MSI、VSIX、ZIP 均已生成，默认安装入口为静态版；默认 VSIX 与静态变体字节一致。MSI 作者为小兔糖糖_OvO，版本 1.0.0，保留第三方许可。
- [x] Ubuntu 22.04 amd64/ARM64 和真实 CentOS Stream 9 amd64：提供 DEB/RPM/tar.gz 中适用的格式，PE/ELF 机器类型、版本、包内编译器、权限及 SHA256 已核对。最终仅保留八个 Linux 安装归档。
- [x] 两种 Windows 编译器、Ubuntu x64、QEMU 下的 Ubuntu ARM64 和 CentOS x64 均成功导出 56 篇中英文 Markdown 与图片，共 63 文件，逐篇匹配当前原稿；Windows ZIP 的 PDF 匹配当前 289 页 PDF，130 个书稿仓库链接指向 GitHub，Gitee 链接为零。
- [x] x86 不发布：编译和打包曾完成，但读取手册与 help 均在启动时发生 std::bad_alloc。用户已允许放弃；失败产物移至 tmp/unreleased-ubuntu-22.04-x86，日志 tmp/linux-x86-startup.log 与 tmp/linux-x86-book-startup.log 保留。未把它标为可用版本，也未修复其运行时。
- [x] Git 本人作者、提交者及标签署名已在本地规范化，备份位于 tmp/before-author-rewrite*.bundle；旧提交到新提交的映射维护在 tools/git_author_commit_map.json。远端推送只允许 origin/Gitee，最终推送结果另行记录。

本文件继续保留尚未完成的全量冒烟、AOT/VM 全能力与平台对齐、静态 LLVM 复杂 ABI 独立导出工程验收等项目。本轮用户要求不执行测试门禁，因此不把历史部分通过或产物检查当成这些全量验收已完成；ARM64 当前交付 VM 编译器，原生 AOT 的宿主布局仍为 x64。未满足删除条件，不以结束会话删除任务清单。

- [x] 署名历史已原子强制推送 origin/Gitee：master b33f538 -> 9acd1a6，六个既有标签仅沿用等价历史重写；未推送 GitHub。最终产物为两类 Windows x64 MSI、八个 Linux 安装归档、双变体 VSIX/ZIP；x86 不发布。

## 本轮追加：1.0.1 的 fn 前缀与返回类型后置

本轮目标版本 1.0.1，启动时仓库 VERSION 为 1.0.0。用户已授权实现下列语法糖、补齐示例与文档、最后提交双仓库（Gitee 与 GitHub）。本轮验收只跑 VM，**不跑 AOT**（用户明确要求）。1.0.0 及更早的测量与产物保留其真实版本，不因改版本号重标。

技术前提（已核对，决定实现方式）：泛型声明的 `<T>` 由文本层接管，`normalizeGenericSyntax()`（`source/chali_generic.cpp:473`）在进入文法前把 `T calc<T>(T x)` 改写成 `__chali_template<T> T calc(T x)`，随后 `collect()`（`709`）抹空原文并登记，`substitute()`（`1333`）单态化后才重新进入解析器；真正的 `<T>` 从不到达文法层。因此**不在 `source/chali.gram` 加产生式**：那样要先穿越 `normalizeGenericSyntax`、`markClosureTypeArguments`、`markContainerTypeArguments` 三处文本层拦截，且 `chali.gram:25` 的 `%expect 0` 被突破会让 `gen.sh` 直接失败。改在 `chali_generic::run`（`1648`，调用点 `1699`）的最前面加一道**文本脱糖 pass**，把新写法归一化成现有写法，泛型单态化链路原样复用。

### 一、语法范围

- [x] 顶层函数定义省返回类型：`fn main() { }` 等价于 `void main() { }`。
- [x] 顶层函数定义返回类型后置：`fn calc(int64 x) -> int64 { }` 等价于 `int64 calc(int64 x) { }`。
- [x] 泛型函数返回类型后置：`fn calc<T>(T x) -> T { }` 等价于 `T calc<T>(T x) { }`。
- [x] 返回闭包类型（裸写法）：`fn calc<T>(T x) -> (T fn(T b)) { }`。
- [x] 返回闭包类型（后置嵌套）：`fn calc<T>(T x) -> fn(T b) -> T { }`。
- [x] 返回闭包类型（前置括号）：`(fn(T b) -> T) calc<T>(T x) { }`。
- [x] 函数类型位置的后置写法：`fn(int64 a, int64 b) -> int64` 等价于 `int64 fn(int64 a, int64 b)`。
- [x] 高阶闭包的前后置混写：形参、变量声明、返回位置的函数类型可任意混用上述写法。
- [x] 局部具名函数同样支持：`fn localTwice(int64 v) -> int64 { }` 与语句级声明共用同一条路径。

### 二、脱糖 pass 的实现约束

- [x] 新 pass 在 `chali_lambda::run` 与 `normalizeGenericSyntax()` 之前执行；否则 `fn calc<T>` 会被当作旧泛型声明吞掉并整段抹空，lambda 关卡也会先拦下箭头。实现落在 `source/chali_fn_sugar.cpp`，接入点 `source/chali_driver.cpp:1169`。
- [x] 必须跳过字符串、字符字面量、行注释与块注释，不能改写其中的 `fn` 与 `->`；复用 `chali_lambda::codeMask` 的掩码。
- [x] 不得误伤 lambda（`fn(...)` 后接函数体）、泛型闭包头（`fn<T>(...)`）与成员访问的 `->`（`a->b`）。判定依据：`fn` 后接标识符才是声明；`fn(...)` 后紧跟 `->` 才是函数类型；`fn` 后紧跟 `<` 是泛型闭包。
- [x] 嵌套由内向外递归归一化，`fn(T b) -> T` 先变成 `T fn(T b)`，外层再补括号。
- [x] 改写只插入/替换文本，保留换行数，不破坏 `lineMap` 与行列映射。
- [x] 函数类型位置的形参名按既有规则剔除（`chali_semantics.cpp:641` 要求函数类型的形参只写类型），`fn(int64 a, int64 b) -> int64` 归一化为 `int64 fn(int64, int64)`。

### 三、已知限制

- [x] 用户给出的 `fn calc<T>(T x) -> fn(T b) -> b` 中最内层 `->` 后是形参名 `b`，不是类型。类型位置不接受形参名，本轮按 `-> T` 实现；写法可用，其中的 `b` 只作文档用途，不进入类型身份。
- [x] lambda 的箭头返回类型保持 0.12.0 的移除决定：`fn(int64 x) -> int64 { ... }` 仍被拒绝，脱糖 pass 不改写它，交由 `chali_lambda.cpp:372` 报错。
- [x] --raw 模式不接入脱糖：该分支不处理泛型声明，接入会让残留的 `<T>` 被闭包标点化。

### 四、文档

- [x] `doc/28-declaration-sugars.md` 与 `doc/en/28-declaration-sugars.md` 新增「fn 前缀与返回类型后置」章节，写明等价关系与脱糖规则。
- [x] `doc/39-syntax-frozen.md` 登记该写法（函数与函数类型两行）。
- [x] 中文书稿第 15 章新增 15.18「fn 前缀与返回类型后置」，后续小节顺延为 15.19 至 15.24；`doc/book/latex/chapters/ch15.tex` 同步插入该节（LaTeX 自动编号，无需改写其余节号）。
- [x] `doc/14-grammar-ebnf.md` 无需改动：脱糖不新增产生式，EBNF 由文法导出且文法本轮未变，`tools/export_chali_ebnf.py` 无漂移。
- [x] 中文书稿 PDF 重新生成：290 页（原 289 页 + 新增节），由 `doc/book/latex` 下 `latexmk -xelatex book.tex` 生成。
- [x] 英文书稿 `doc/book/en/15-functions-closures.md` 同步，不翻译番外、不生成英文 LaTeX/PDF。
- [x] 三份 README 与 `doc/README.md` 本轮无需改动：均未逐条列举函数声明写法。

### 五、示例与验收

- [x] `example/samples/fn_trailing_return.chl` 新增后置写法样例并配 `example/expected/fn_trailing_return.txt`；覆盖 void 入口、普通函数、泛型、返回闭包的三种写法、函数类型变量与局部具名函数。
- [x] 只测 VM，不测 AOT（用户明确）。
- [x] 与既有前置写法的样例逐一对照：`tmp/fn_sugar_legacy.chl` 用纯前置写法表达同一语义，确认行为一致，返回借用局部的闭包同样报 CHL1204，不是脱糖引入的问题。
- [x] 反向核对：lambda、泛型闭包、成员访问 `->`、字符串与注释里的 `fn` 均不受影响；lambda 箭头返回类型仍按 0.12.0 的决定被拒绝。
- [x] 全量 `sh check.sh`（VM）回归：279 个正常样例 + 312 个错误样例全部通过，既有样例未被文本层改动影响。

### 六、版本与交付

- [x] VERSION 由 1.0.0 改为 1.0.1；`source/chali_package.hpp` 的 `compilerVersion`、语言服务 `kServerVersion` 与 `chali-lsp` 帮助/版本输出、插件 `chaliCompilerVersion` 同步。插件自身 `version` 仍独立保持 1.0.0。`tools/release.py` 三处核对通过：VERSION 1.0.1、扩展包 1.0.0、运行中编译器 LSP 自报 1.0.1。
- [x] 产物名相关文档随 VERSION 同步为 1.0.1：`doc/41-aot.md`、`doc/42-installation-packages.md`、中英文书稿三处 Windows 构建产物注释、`doc/book/latex/preamble.tex` 封面（`ChaLi 1.0.1 · 2026年10月25日`）；`doc/12-vm-performance.md` 与英文版由 `tools/update_performance_docs.py` 按 VERSION 重新生成，数值未变。ABI 身份 `chali-abi-v1.0.0-collections-2` 是独立于程序版本的 ABI 标签，本次未改。
- [x] `tools/changelog_data.json` 追加 1.0.1 条目并重新生成 CHANGELOG.md（`tools/changelog.py --build` 无漂移）。
- [x] 书稿 LaTeX 由 `doc/book/latex/tools/md2tex.py` 重新导出（ch01/preface/ch15），PDF 重新生成仍为 290 页。
- [ ] 提交并推送 origin/master（Gitee）与 GitHub 两个远端（用户已明确授权）。
