# ChaLi for VS Code

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文入口](../README.cn.md)
<!-- END LANGUAGE NAVIGATION -->

ChaLi（`.chl`）的编辑器支持：**语言服务器就是编译器本体**（`chali lsp`），所以只用
一个可执行文件 —— 不需要另外编 `library/lsp` 的 `lsp-server`，开发工具和语言服务器原生内建，无需安装 Python。

## 能力

| 能力 | 说明 |
| --- | --- |
| 诊断 | 真实前端检查（`chali --check --diagnostics=json`）：语法错误带列、语义错误覆盖整行；码是编译器自己的稳定错误码 |
| 悬停 | 函数、工程类型与字段声明、重载及文档注释；let 变量及 let 关键字显示完整推断类型、泛型实参与高阶 fn 参数/返回层次 |
| 参数提示 | `(` / `,` 及宏的 `<` / `[` / `{` 触发；显示泛型、默认参数、重载与当前参数；多括号宏按组高亮、统计独立尾参 |
| 类型内联提示 | 显示 let 变量、for-in 元素和上下文匿名函数参数的推断类型，可关闭 |
| 引用与重命名 | 按原始绑定区分同名遮蔽，支持跨模块和未保存文档；歧义、冲突及无法确认的宏模板引用拒绝重命名，应用前检查文档版本 |
| 关联诊断 | 移动、借用及返回表达式清理冲突可关联原始声明、显式捕获或 defer 位置 |
| 宏展开预览 | 复用真实预处理器；同一页面用多个 Tab 展示各宏调用的 AST 树形图，按宏名、文件和调用行号搜索，双击图头跳回调用处 |
| 宏体独立高亮 | `$macro` 到 `$end_macro` 使用独立 TextMate 区域；指令、形参、`$for` 循环变量和局部声明分别着色，字符串与注释保持原有角色；语义着色同步区分声明和使用 |
| 工程符号 | 在工作区其他 `.chl` 文件中搜索函数、方法、结构体、枚举、接口和字段 |
| 补全 | 当前作用域、标准库、工程模块和成员（`:` / `.` 触发）；选择尚未导入模块的函数时补入 `import` |
| 语义着色 | 区分类型、泛型参数、变量、参数、函数和成员；颜色由当前主题决定 |
| 跳转定义 | 名字引用 → 当前文件或导入模块的声明处 |
| 文档大纲 | 当前文档显式声明的名字，区分函数、变量和类型 |
| 格式化 | 保守：只规范缩进与行尾空白，注释 / 字符串 / 宏指令逐字保留 |
| 折叠 | `$macro` / `$for` 宏块成对折叠（客户端计算，不需要服务器） |
| 运行 | 标题栏 ▶ / 右键 / 命令面板跑 `chali run <当前文件>`：先存盘，再在集成终端里执行，工作目录是文件所在目录，标准输入可直接交互 |
| 构建 | 标题栏构建按钮、命令面板或 `.chl` 右键菜单生成 LLVM AOT 可执行文件（exe），保存源码并显示原始构建诊断 |
| 断点调试 | 运行与调试面板里选「ChaLi」，在 `.chl` 上打断点即可：编译器本体就是调试适配器（`chali --dap <文件>`），支持断点、继续、步进 / 步过 / 步出与调用栈；程序输出显示在调试控制台 |

## 内嵌的编译器

首次开发或打包插件，先在 `vscode/` 执行 `npm ci` 安装锁定的测试依赖。`npm test` 和 `npm run package` 都运行真实 TextMate/Oniguruma 引擎的宏体高亮回归；测试依赖不进入安装包。

插件包自带编译器：`bin/chali[.exe]` —— 打包前由 `node tools/embed-compiler.js` 放进去，
`vsce package` 会自动跑这一步（`vscode:prepublish`）。装完即用：不需要用户先构建工具链，
语言服务器和依赖工具均由原生 C++ 实现，无需安装 Python。

- 刷新内嵌副本：在项目根目录 `make` 出 `./chali[.exe]`，然后在 `vscode/` 里 `npm run embed`；
- 查找顺序：设置 `chali.path` → 内嵌 `bin/` → 工作区（`build/work/chali/chali`、`chali/chali`…）→ PATH；
  输出面板会打印实际用的是哪一份。

`bin/` 不进版本库，只在打包时生成。插件提供 `llvm-static` 与 `llvm-external` 两种替代变体：静态变体把 LLVM 与 LLD 实际链接进编译器，构建 exe 无需另外安装 LLVM；外置变体需要可用的 LLVM clang。两种变体版本均与编译器一致；.svmb 已删除。

## 设置

| 设置 | 说明 |
| --- | --- |
| `chali.path` | `chali` 编译器路径；留空时依次用**插件自带的 `bin/chali`**、`build/work/chali/chali`、`build/chali/chali`、`chali/chali`、`PATH`（Windows 带 `.exe`） |
| `chali.traceServer` | 设为 `messages` 把与 `chali lsp` 的协议消息写进输出面板 |
| `chali.inlayHints.enabled` | 是否显示推断类型内联提示，默认开启；也受 VS Code 的内联提示设置控制 |
| `chali.build.outputDirectory` | 构建输出目录，默认 `build`；相对最近的 ChaLi 工程目录，没有工程清单时相对源文件目录 |
| `chali.build.llvmPath` | 外置 LLVM clang 路径；留空使用编译器内置后端或默认工具链 |
| `chali.build.checked` | 为 AOT 构建生成显式边界和空指针检查，默认关闭 |

## 命令

- **ChaLi: 重启语言服务器**
- **ChaLi: 显示语言服务器日志**
- **ChaLi: 打开语言文档**（打开 `chali/doc/02-language-tour.md`）
- **ChaLi: 运行当前文件**（标题栏的 ▶ 与右键菜单里的同一项）
- **ChaLi: 构建当前文件**（LLVM AOT）
- **ChaLi: 构建可执行文件（LLVM AOT）**

## 构建程序

打开已保存的 `.chl` 文件，点击标题栏的构建按钮并选择格式，也可直接使用右键菜单中的对应命令。构建使用 `chali.path` 或当前插件内嵌的编译器，执行前保存目标文档；成功后在“ChaLi 构建”输出中显示完整产物路径，并提供在资源管理器中定位的入口。失败时保留编译器诊断和退出码，不报告成功。构建期间不自动运行产物。

例如 `src/main.chl` 在工程默认配置下生成 `build/main.exe`；exe 使用本机 Windows x64 的 LLVM AOT 后端，.svmb 已删除，不再交付 VM 字节码。AOT 支持范围、静态依赖准备和检查模式见 [AOT 文档](../doc/en/41-aot.md)。源路径和输出路径作为独立参数传递，支持空格与中文。未受信任的工作区不启动编译器。

## 打包

```sh
npm run package            # 内嵌编译器 → 清单自检 → 冒烟测试 → 产出 dist/*.vsix
npm run package -- --skip-tests   # 跳过自检，只打包
npm run package -- --llvm-variant static --compiler ../dist/llvm-static/ChaLi.exe
npm run package -- --llvm-variant external --compiler ../dist/llvm-external/ChaLi.exe
```

两个变体的插件版本均为 1.0.0，文件名分别为 `chali-editor-1.0.0-llvm-static.vsix` 与 `chali-editor-1.0.0-llvm-external.vsix`，编译器程序版本从根目录 VERSION 同步。仓库根的 `python tools/release_variants.py` 可顺序构建、验收并打包两个程序和插件；LLVM 静态开发库首次自动下载固定源码并校验 SHA-256。许可证随 VSIX 与发布 ZIP 提供。

`tools/package.js` 会按顺序做四件事：`tools/embed-compiler.js` 把编译器放进 `bin/`、
`tools/check-manifest.js` 自检清单、`node test/smoke.js` 跑 23 项冒烟（起真 `chali lsp`），
最后调 `vsce package` 产出 `dist/chali-editor-<版本>.vsix`。`vsce` 优先用 PATH 上的那份，
没有就退回 `npx --yes @vscode/vsce`；加 `--dry-run` 可以只打印要跑的命令。

## 本地自检

```sh
node chali/vscode/test/smoke.js                 # 折叠 + LSP 往返（自动找 chali）
node chali/vscode/test/smoke.js --chali <路径>  # 指定编译器
node chali/vscode/tools/check-manifest.js       # 清单自检（语言 / 高亮 / 图标注册齐了没）
```

## 作者与发布者（清单字段）

| 字段 | 值 | 说明 |
| --- | --- | --- |
| `publisher` | `rabbitc999` | 发布者**标识**：必须是 ASCII，且与 marketplace 上的 publisher 账号一致。插件全名是 `<发布者>.<名字>`（本插件即 `rabbitc999.chali-editor`）——换标识等于换身份，装过旧标识的要卸掉重装。 |
| `author` | 小兔糖糖_OvO `<rabbitc999@outlook.com>` | 详情页里显示的**作者**（人类可读名）。`publisher` 不收中文（vsce 会报 *Expected the identifier of a publisher, not its human-friendly name*），所以名字写在这里。 |
| `license` | `Apache-2.0` | 许可见 项目根的 `LICENSE`。 |
| `icon` | `icon.png` | 插件图标（魔术棒 + `C` + `Value`，由项目根的 `tools/make_icon.py` 生成）。 |

发布：先在 marketplace 建好 `rabbitc999` 这个 publisher 并配好 PAT，然后 `npx @vscode/vsce publish`。


## 在线模块依赖

在 `chali.toml` 的右键菜单或命令面板中执行 **ChaLi: 下载并锁定模块依赖**、**ChaLi: 离线校验模块依赖** 和 **ChaLi: 打开在线依赖缓存** 命令。安装调用 `chali deps lock` 并递归下载到编译器旁的 `libraries/`，生成 `chali.lock` 后自动刷新语言服务器。多工程会提供选择；详细日志见依赖输出通道。下载只在受信任的工作区执行。缓存不会打入插件包。

补全提示保留完整名称以说明来源；没有名称冲突时插入简写，有跨模块同名或当前作用域遮蔽时保留限定名，同模块函数重载不视为冲突。

工程类型和字段提示会递归读取导入模块，并优先使用已打开的未保存内容。成员提示目前以显式声明的接收者类型为依据，复杂表达式的返回类型推断仍有局限。插件未提供完整的工程级重命名与查找引用功能。

## 宏提示与展开预览

普通语言服务器同时提供 `$macro` / `$replace` 定义的补全、参数提示、文档悬停和定义跳转，索引 `import` 引入模块中的宏。当前文件的 `%define` 符号也进入补全。命令面板或源码右键选择 **ChaLi: 预览宏展开**，编译器返回卫生 AST，在同一预览页内按宏调用展示多个 Tab，标签显示宏名与调用行号。图中按节点类型、名称、成员、类型与字面值着色，实参来源节点单独高亮，节点带子项计数；双击图头回到源码调用处。

宏展开在一个预览面板内使用多个 Tab，每处调用仍有独立树形图；再次预览复用同一面板。搜索框按宏名、文件和调用行号筛选 Tab，显示匹配数量，无结果时明确提示。Tab 支持方向键、Home/End，切换时保留每张树的折叠状态。

AST 使用从根到叶的节点卡片和连接线展示层级；宏定义节点与调用实参使用不同配色。点击节点展开或折叠，也可全部展开、全部折叠、调节缩放或适应宽度；横向超出时在图内滚动。双击图头回到对应调用位置。

中文编辑器中，界面和节点种类尽量用“模板、调用、声明、标识符、整数、字符串”等中文表示；宏名、变量名、类型名及字面值保留源码写法。英语编辑器使用英文命令、构建提示和预览界面。

页面为自包含 HTML：样式内联，颜色有兜底值，适配浅色与深色主题，脱离编辑器仍可在浏览器中使用 Tab、搜索和树形操作；单图静态导出也保留。预览使用未保存的当前源码及已打开的模块文件，调用真实编译器；遇到错误保留诊断。修改后再次运行命令即可更新同一个预览页面。

扩展与编译器统一使用 1.0.8 版本，分别打包为 chali-editor-1.0.8-llvm-static.vsix 和 chali-editor-1.0.8-llvm-external.vsix；打包检查真实 --build-info。原生调试使用 GDB/MI，详见[原生调试](../doc/46-native-debugging.md)。
