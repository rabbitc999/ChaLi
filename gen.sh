#!/bin/sh
# 重新生成解析器 / 驱动 / 工程文件（产物落在本目录，--out-dir 缺省就是 `.`）。
#
#   sh gen.sh
#
# 文法生成器（ParseEngine）是外部依赖，按下面顺序找（也可用 ENGINE=<路径> 指定）：
#   ./ParseEngine.exe、./ParseEngine             —— 放到本项目根目录（最省事）
#   ../ParseEngine/build/bin/ParseEngine[.exe]   —— 与 ParseEngine 仓库并排时
#   ../build/bin/ParseEngine[.exe]               —— 还住在 ParseEngine 仓库里时
set -e
cd "$(dirname "$0")"

# 文法生成器（ParseEngine）在哪：ENGINE=<路径> 显式指定；否则先看**本项目根目录**（默认就
# 找它），再退回同级的 ParseEngine 仓库 build/bin/。生成器是外部依赖：本项目只有 .gram。
if [ -z "${ENGINE:-}" ]; then
    # 两条讲究：① 候选要写成带 `./` 的路径 —— 裸名会被 bash 当成 PATH 查找，
    # 当前目录不在 PATH 里，于是报 `ParseEngine.exe: command not found`；
    # ② Windows 上 `.exe` 排在前面 —— MSYS 的 `test -x ParseEngine` 会因 .exe 后缀假命中。
    for candidate in "./ParseEngine.exe" "./ParseEngine" \
                     "../ParseEngine/build/bin/ParseEngine.exe" "../ParseEngine/build/bin/ParseEngine" \
                     "../build/bin/ParseEngine.exe" "../build/bin/ParseEngine"; do
        if [ -x "$candidate" ]; then
            ENGINE="$candidate"
            break
        fi
    done
fi
if [ -z "${ENGINE:-}" ] || [ ! -x "$ENGINE" ]; then
    echo "找不到 ParseEngine 文法生成器（本项目只有 .gram，生成器是外部依赖）。任选一种：" >&2
    echo "  1) 从旁边的 ParseEngine 仓库构建：make -C ../ParseEngine/source -j4" >&2
    echo "     产出 ../ParseEngine/build/bin/ParseEngine[.exe]，之后本脚本会自动找到它" >&2
    echo "  2) 复制到本项目根目录：cp ../ParseEngine/build/bin/ParseEngine.exe ." >&2
    echo "     （PowerShell：Copy-Item ..\ParseEngine\build\bin\ParseEngine.exe .）" >&2
    echo "  3) 显式指路径：ENGINE=/path/to/ParseEngine.exe sh gen.sh" >&2
    exit 2
fi

echo "文法生成器：$ENGINE"

# 先把标准库 .mgl 源码内嵌进编译器（magel_stdlib_embedded.cpp），
# 这样 exe 自包含，import std::xxx 不依赖磁盘上的 .mgl 文件。
python tools/sync_tool_console.py
python tools/embed_i18n.py
python tools/embed_stdlib.py

# --no-build-files：本目录的 Makefile 是手写、纳入版本管理的「生成代码 → 构建」
# 工程文件（见 Makefile / local.mk），不能让生成器覆盖它，也别删它。
# 解析器 / 驱动源码（magel_parser.* / magel_main.cpp）照旧生成到仓库根目录：
# --out-dir 缺省就是当前目录（gen.sh 已 cd 到仓库根），文法放在 source/ 不影响产物落点。
# 词法包含由本项目展开，兼容旧版外置生成器；权威定义仍为 source/magel.gram。
expanded=source/.magel-build.gram
trap 'rm -f "$expanded"' EXIT HUP INT TERM
python tools/expand_grammar.py source/magel.gram "$expanded"
"$ENGINE" "$expanded" --mode ielr --no-build-files
