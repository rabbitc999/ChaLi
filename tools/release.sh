#!/bin/sh
# tools/release.sh —— Magel 发布打包：构建 + 归档到 dist/（非 root）。
#
# 用法（在 Magel 项目根跑）：
#   sh tools/release.sh              复用已有产物，只打包
#   sh tools/release.sh --build      先按 ci.sh 的口径构建，再打包
#   sh tools/release.sh --out-dir DIR  改输出目录（缺省 dist/）
#
# 产物：
#   dist/magel-<版本>-linux-x64.tar.gz   （<版本> 读 VERSION）
#   dist/magel-<版本>-linux-x64.tar.gz.sha256
#   解包后是 bin/ + doc/ + VERSION / CHANGELOG.md / README.md / LICENSE
#
# 说明：这里只归档**已经构建好**的东西，构建本身交给 ci.sh / build-all.sh，
# 免得发布脚本里再长出一套会漂移的编译逻辑。--build 就是替你先跑一遍构建步骤。

set -u

root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd) || exit 2
cd "$root" || exit 2

do_build=0
out_dir='dist'
while [ $# -gt 0 ]; do
    case "$1" in
    --build) do_build=1 ;;
    --out-dir)
        shift
        [ $# -gt 0 ] || { echo "--out-dir 后面要跟目录" >&2; exit 2; }
        out_dir="$1"
        ;;
    -h | --help)
        awk 'NR > 1 && /^#/ { sub(/^# ?/, ""); print; next } NR > 1 { exit }' "$0"
        exit 0
        ;;
    *) echo "未知参数：$1（用 --help 看用法）" >&2; exit 2 ;;
    esac
    shift
done

version=$(cat VERSION 2>/dev/null | tr -d ' \t\r\n')
[ -n "$version" ] || { echo "读不到 VERSION" >&2; exit 1; }

host=$(uname -s 2>/dev/null | tr 'A-Z' 'a-z')
arch=$(uname -m 2>/dev/null)
[ -n "$host" ] || host='unknown'
[ -n "$arch" ] || arch='unknown'
name="magel-$version-$host-$arch"

echo "== Magel $version（$host-$arch）=="

if [ "$do_build" = 1 ]; then
    echo "-- 先构建（ci.sh 口径）"
    sh ci.sh || { echo "构建/门禁失败，停止打包" >&2; exit 1; }
fi

stage="$out_dir/$name"
[ -d "$stage" ] && find "$stage" -depth -delete
mkdir -p "$stage/bin" "$stage/doc" || exit 1

copy() { # 源 目标目录
    if [ -f "$1" ]; then
        cp "$1" "$2/" || exit 1
        echo "  + ${1}"
    else
        echo "  ! 缺产物：$1（先跑 sh ci.sh）" >&2
    fi
}

echo "-- 归档可执行文件"
copy magel "$stage/bin"
copy build/magel-emit/magel-emit "$stage/bin"
copy build/magel-generator/magel-generator "$stage/bin"
# ParseEngine 文法生成器是外部依赖（由 ParseEngine 仓库构建），发行包里带上它
copy build/bin/ParseEngine "$stage/bin"
copy tmp/ci/stackvm/magel-stackvm "$stage/bin"
copy tmp/ci/stackvm/magel-stackvm-tests "$stage/bin"

echo "-- 归档文档"
for f in VERSION CHANGELOG.md README.md; do
    [ -f "$f" ] && cp "$f" "$stage/" && echo "  + $f"
done
[ -f LICENSE ] && cp LICENSE "$stage/" && echo "  + LICENSE"
[ -d doc ] && cp -r doc "$stage/doc" && echo "  + doc/"

cat >"$stage/README-发行包.txt" <<EOF
Magel $version（$host-$arch）
构建：$(date -u +%Y-%m-%dT%H:%M:%SZ)

bin/magel                     MAGEL 编译器 / 解释器 / 驱动
bin/magel-emit                  四元式 → 自包含 C++ 程序的导出器
bin/magel-generator             统一入口（compiler / vm / clear）
bin/ParseEngine              文法生成器（gen.sh 需要）
bin/magel-stackvm          StackVM 命令行
bin/magel-stackvm-tests    StackVM 单元测试（跑法：magel-stackvm-tests <examples 目录>）

doc/                         用户专题指南（00–16）
VERSION / CHANGELOG.md       版本与变更记录

验收门禁：源码树里跑 sh ci.sh（本包不含测试样例与源码）。
EOF

echo "-- 打包"
archive="$out_dir/$name.tar.gz"
tar -C "$out_dir" -czf "$archive" "$name" || exit 1
( cd "$out_dir" && sha256sum "$(basename "$archive")" >"$(basename "$archive").sha256" ) || exit 1
echo "  完成：$archive"
ls -l "$archive"
