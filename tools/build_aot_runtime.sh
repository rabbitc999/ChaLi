#!/bin/sh
# 使用固定 LLVM 直接生成 AOT 运行时模块；无需 Python。
set -eu
root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
cd "$root"
check=0
clang=${CHALI_LLVM_CLANG:-}
while [ "$#" -gt 0 ]; do
    case "$1" in
        --check) check=1; shift ;;
        --clang) [ "$#" -ge 2 ] || { echo '--clang 缺少路径' >&2; exit 2; }; clang=$2; shift 2 ;;
        *) echo "未知选项：$1" >&2; exit 2 ;;
    esac
done
if [ -z "$clang" ]; then
    if [ -f build/dependencies/llvm/toolchain/bin/clang.exe ]; then
        clang="$root/build/dependencies/llvm/toolchain/bin/clang.exe"
    else
        clang=clang
    fi
fi
case "$("$clang" --version)" in
    *'version 23.1.4'*) ;;
    *) echo '运行时资产必须用固定版本 LLVM 23.1.4 生成' >&2; exit 2 ;;
esac
mkdir -p "$root/tmp"
temporary_root="$root/tmp"
if command -v cygpath >/dev/null 2>&1; then temporary_root=$(cygpath -m "$temporary_root"); fi
export TMPDIR="$temporary_root" TMP="$temporary_root" TEMP="$temporary_root"
directory=$(mktemp -d "$root/tmp/chali-aot-runtime.XXXXXX")
trap 'rm -f "$directory/runtime-windows.ll" "$directory/runtime-linux.ll" "$directory/runtime-linux_arm64.ll"; rmdir "$directory"' EXIT HUP INT TERM
for platform in windows linux linux_arm64; do
    case "$platform" in
        windows) triple=x86_64-w64-windows-gnu ;;
        linux) triple=x86_64-unknown-linux-gnu ;;
        linux_arm64) triple=aarch64-unknown-linux-gnu ;;
    esac
    output="$directory/runtime-$platform.ll"
    if [ "$platform" = windows ]; then
        "$clang" --target="$triple" -S -emit-llvm -O1 source/aot/runtime.c -o "$output" -isystem build/dependencies/llvm/toolchain/include
    else
        "$clang" --target="$triple" -S -emit-llvm -O1 source/aot/runtime.c -o "$output"
    fi
    target="source/aot/runtime_$platform.ll"
    if [ "$check" -eq 1 ]; then
        # 文本比较只忽略 CRLF/LF；不忽略实际指令和常量字节差异。
        diff --strip-trailing-cr "$target" "$output"
    else
        cp "$output" "$target"
    fi
done
if [ "$check" -eq 1 ]; then echo 'AOT 运行时 LLVM 模块已核对'; else echo 'AOT 运行时 LLVM 模块已生成'; fi
