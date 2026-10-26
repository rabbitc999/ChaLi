# 在线构建依赖

[English](en/45-build-dependencies.md) · **简体中文**

SQLite 与 miniz 的源码不存放在 Git 源码树，也不复制到生成器导出的工程中。构建工具按 [锁定清单](../tools/dependencies.lock.json) 下载固定版本，并验证归档和每个使用文件的 SHA256。SQLite 的原有 SHA3-256 记录也已与官方归档核对。

| 库 | 固定版本 | 上游归档 SHA256 |
| --- | --- | --- |
| SQLite | 3.54.0 | `68e913b0fe8ec6b4e5f391c594a28872884e80d6cd042a1e50c957be46b5aa4d` |
| miniz | 3.1.2 | `e7a0334927a069dc5aeb69616d371542a4376e1569290d7102b88c6f3992d6cb` |

版本、官方下载地址、归档根路径及文件摘要都来自锁定清单，不跟随 `latest` 或浮动分支。下载失败、归档损坏和文件摘要不符均使构建失败，不能降级为不校验下载。

## 使用

仓库的 `make` 自动准备 `build/dependencies/sqlite` 和 `build/dependencies/miniz`。CMake 工程默认使用自己的构建目录下的 `dependencies`，导出的独立编译器也使用同一下载器与锁定清单。首次构建需要 Python 3 和网络连接。

```text
python tools/fetch_dependencies.py
python tools/fetch_dependencies.py --offline
make -j4
```

归档缓存位于项目 `tmp/dependency_downloads/`；源码构建缓存位于 `build/dependencies/`。二者均不提交到 Git。有效构建缓存可离线复用；文件损坏时，可以从摘要正确的归档缓存恢复。`--offline` 禁止网络请求，缺少有效归档时明确失败。CMake 可通过 `-DCHALI_DEPENDENCY_DIR=路径` 指定构建缓存。

共享缓存写入使用锁；下载与成员校验完成后才安装文件。解包只读取清单列出的成员，不展开归档中的任意路径。miniz 的静态导出宏头由项目下载器生成，不改写上游源码。

## 运行与交付

这是构建期下载。编译器及生成的 AOT 程序仍静态链接所需实现，运行 SQLite 程序不需要下载源码或寻找 `sqlite3.dll`。SQLite 开启线程安全并禁用动态扩展加载；miniz 只编译所需的 tinfl 解压内核。

miniz 的 MIT 许可证保存在 [licenses/miniz/LICENSE](../licenses/miniz/LICENSE)，发行包继续携带该许可证与项目 [NOTICE](../NOTICE)。SQLite 保持公共领域身份，来源记录保留在锁定清单及 NOTICE 中。

更新版本时修改锁定清单中的版本、地址与摘要，重新执行缓存／下载验证、完整构建、SQLite 功能、原生依赖解包、生成器和发行包核对。不要把下载得到的第三方源码复制回 `source/` 或 `library/`。
