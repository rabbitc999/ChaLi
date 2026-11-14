# dsh-chali

DeepSeek Harness 插件：让 DeepSeek 直接编写、检查、运行和构建 ChaLi 程序。插件内嵌 ChaLi 编程 Skill、四个工具，以及**真实的 ChaLi 编译器原程序**（`bin/chali.exe`），安装后不依赖仓库路径或系统 `PATH`。

## 包含什么

| 组成 | 说明 |
| --- | --- |
| `chali-programming` Skill | 来自仓库 `skills/chali-programming/SKILL.md`，写作前自动进入模型上下文；构建时附加 DSH 工具使用说明。 |
| `chali_write` | 写入完整 `.chl` 源码，默认立即 `chali check` 并返回诊断。 |
| `chali_run` | 对已有文件执行 `check` / `run` / `build`；捕获标准输出、错误和构建产物。 |
| `chali_man` | 查询编译器内嵌手册（关键字、标准库 API、书稿）。 |
| `chali_new` | 新建 ChaLi 应用工程。 |
| `bin/chali.exe` | 当前版本的 ChaLi 编译器本体，由 `npm run package` 复制进来；`bin/compiler.json` 记录版本、大小和 SHA-256。 |

插件以 host-plane 行注册：工具进入全局工具层，Skill 进入全局 Skill 层，所有 agent preset 都能继承，不需要修改某个预设。

## 安装

插件包必须是“已打包”状态：先运行打包脚本，把 ChaLi 编译器复制到 `bin/`。

```powershell
cd plugins/dsh-chali
node scripts/package.mjs
```

`bin/chali.exe` 约 168 MiB，按 `.gitignore` 不进入 Git；每次需要安装自包含插件时重新运行打包脚本。打包脚本按以下顺序找编译器：`CHALI_COMPILER` / `CHALI_BIN` → `vscode/bin/chali.exe` → 仓库根目录的正式版本产物 → `PATH`。

然后在 DeepSeek Harness 中任选一种方式安装：

1. **界面（推荐）**：侧边栏 **Plugins → Add plugin → Install a third-party plugin**，填入本目录的绝对路径，例如
   `D:\Desktop\Workspace\ChaLi\plugins\dsh-chali`。界面会执行 pnpm 安装、把 `dsh-chali` 加入 profile bundles，并在 HMR 可用时立即装载。
2. **命令行**：

   ```powershell
   dsh plugin --profile desktop add "link:D:\Desktop\Workspace\ChaLi\plugins\dsh-chali"
   ```

   当前 CLI 在安装带 `dsh.bundle.patch` 的包时会把 `dsh-chali` 追加到 profile 的 `dsh.profile.bundles`。如果目标版本没有自动追加，手工在 `~/.dsh/profiles/<profile>/package.json` 的 `dsh.profile.bundles` 末尾加入 `"dsh-chali"`，配置 HMR 会重新组合。

安装后新开一个会话（或让 HMR 完成重组），Skill 目录会出现 `chali-programming`，工具列表会出现 `chali_write` / `chali_run` / `chali_man` / `chali_new`。

> 桌面版 profile 由 Electron 应用管理。安装包操作最好在应用内 Plugins 页面完成；使用 `dsh plugin` 时不要同时让应用执行其他插件操作。

## 配置

profile 的 `cordis.patch.yml` 可以覆盖该行配置，字段都是可选的：

```yaml
- id: chali
  config:
    compiler: 'D:\tools\chali.exe'   # 默认使用插件自带 bin/chali.exe
    workspaceRoot: 'D:\work\demo'    # 没有 agent 工作目录时的兜底
    allowOutsideWorkspace: false     # 默认只允许写会话工作目录内
    checkOnWrite: true               # chali_write 默认立即检查
    timeoutMs: 120000                # 默认检查/运行超时
    maxOutputChars: 12000            # 工具结果截断上限
```

## 开发与验证

```powershell
cd plugins/dsh-chali
node scripts/build.mjs          # 只重新生成 lib/skill-content.js
node scripts/package.mjs        # 重新生成 Skill 并打包编译器
node --test test/plugin.test.mjs
```

测试覆盖：插件注册、编译器原程序确实在包内、`chali_write` 写入并检查、`chali_run` 运行并核对输出、`chali_man` 读取内嵌手册。

修改 `skills/chali-programming/SKILL.md` 后必须重新运行 `node scripts/build.mjs`（或 `package.mjs`），并提交生成后的 `lib/skill-content.js`；`bin/` 下的编译器与元数据不提交。

## 设计边界

- 插件只调用 ChaLi 官方工具链，不重新实现语法、类型或借用检查。
- 工具结果按 `maxOutputChars` 截断，保留头尾；完整输出不落盘。
- 写入默认被限制在会话工作目录内；确需写外部路径时显式配置 `allowOutsideWorkspace: true`。
- 打包后的插件是平台相关的：`bin/chali.exe` 来自当前 Windows x64 构建。其他平台需要在对应 ChaLi 构建上重新运行 `package.mjs`。
