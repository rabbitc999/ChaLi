#!/bin/sh
# 在 Linux / WSL 的 GNU 工具链中准备与 LLVM 目标匹配的运行时及可移植链接 SDK。
set -eu
root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
cd "$root"
target=${1:-}
jobs=${2:-2}
case "$target" in
    linux-x64|x86_64-unknown-linux-gnu) name=linux-x64; gcc_target=x86_64-linux-gnu; llvm_target=x86_64-unknown-linux-gnu ;;
    linux-arm64|aarch64-unknown-linux-gnu) name=linux-arm64; gcc_target=aarch64-linux-gnu; llvm_target=aarch64-unknown-linux-gnu ;;
    *) echo '用法：sh tools/build_cross_runtime.sh linux-x64|linux-arm64 [并行数]' >&2; exit 2 ;;
esac
case "$jobs" in ''|0|*[!0-9]*) echo '并行数必须为正整数' >&2; exit 2 ;; esac
case "$(uname -s)" in Linux) ;; *) echo '请在 Linux 或 WSL 中运行本脚本' >&2; exit 2 ;; esac
cxx=${CXX:-$gcc_target-g++}
cc=${CC:-$gcc_target-gcc}
archiver=${AR:-$gcc_target-ar}
python=${PYTHON:-}
if [ -z "$python" ]; then
    if command -v python3.11 >/dev/null 2>&1; then python=python3.11; else python=python3; fi
fi
command -v "$cxx" >/dev/null
command -v "$cc" >/dev/null
command -v "$archiver" >/dev/null
case "$("$cxx" -dumpmachine)" in
    "$gcc_target"|"$llvm_target") ;;
    *) echo 'CXX 的目标与请求目标不同，拒绝混用对象文件' >&2; exit 2 ;;
esac
mkdir -p "$root/tmp"
export TMPDIR="$root/tmp" TMP="$root/tmp" TEMP="$root/tmp"
build="build/cross-runtime-$name"
output="$root/build/cross-sdk/$name"
mkdir -p "$output/lib" "$output/licenses"
make -j"$jobs" OS=Linux PYTHON="$python" CHALI_BUILD_ROOT="$build" CXX="$cxx" CC="$cc" AR="$archiver" \
    "$build/chali-ircode-aot-core/libstackvm.a" "$build/chali-host-records/librecords.a" \
    "$build/chali-aot-host/native_runtime.o" "$build/chali-aot-host/backend_profile.o" \
    "$build/chali-frontend/lztext.o"
directory=$(mktemp -d "$root/tmp/chali-cross-runtime.XXXXXX")
trap 'rm -f "$directory/merge.mri"; rmdir "$directory"' EXIT HUP INT TERM
cat > "$directory/merge.mri" <<EOF
CREATE $output/lib/libchali_aot_host.a
ADDLIB $root/$build/chali-ircode-aot-core/libstackvm.a
ADDLIB $root/$build/chali-host-records/librecords.a
ADDMOD $root/$build/chali-aot-host/native_runtime.o
ADDMOD $root/$build/chali-aot-host/backend_profile.o
ADDMOD $root/$build/chali-frontend/lztext.o
SAVE
END
EOF
"$archiver" -M < "$directory/merge.mri"
gcc_version=$("$cxx" -dumpversion)
gcc_directory="$output/sysroot/usr/lib/gcc/$gcc_target/$gcc_version"
mkdir -p "$gcc_directory"
for file in libstdc++.a libgcc.a libgcc_eh.a crtbegin.o crtbeginS.o crtend.o crtendS.o crtfastmath.o; do
    source=$("$cxx" -print-file-name="$file")
    [ -f "$source" ] || { echo "工具链缺少 $file" >&2; exit 1; }
    cp -L "$source" "$gcc_directory/$file"
    case "$file" in *.a) cp -L "$source" "$output/lib/$file" ;; esac
done
libc=$("$cxx" -print-file-name=libc.so)
system_directory=$(dirname -- "$(readlink -f "$libc")")
# 保留 GNU 链接脚本中的绝对路径，并同时提供 Clang 的标准 multiarch 搜索目录。
for destination in "usr/lib/$gcc_target" "lib/$gcc_target" "usr/$gcc_target/lib"; do
    mkdir -p "$output/sysroot/$destination"
    for file in crt1.o Scrt1.o crti.o crtn.o libc.so libc.so.6 libc_nonshared.a libm.so libm.so.6 libmvec.so.1 libpthread.a libpthread.so.0 libdl.a libdl.so.2 librt.a librt.so.1; do
        if [ -f "$system_directory/$file" ]; then cp -L "$system_directory/$file" "$output/sysroot/$destination/$file"; fi
    done
    for file in libgcc_s.so libgcc_s.so.1; do
        source=$("$cxx" -print-file-name="$file")
        if [ -f "$source" ]; then cp -L "$source" "$output/sysroot/$destination/$file"; fi
    done
done
if [ "$name" = linux-arm64 ]; then
    loader=ld-linux-aarch64.so.1
    loader_directory=lib
else
    loader=ld-linux-x86-64.so.2
    loader_directory=lib64
fi
mkdir -p "$output/sysroot/$loader_directory"
cp -L "$system_directory/$loader" "$output/sysroot/$loader_directory/$loader"
for destination in "lib/$gcc_target" "usr/$gcc_target/lib"; do cp -L "$system_directory/$loader" "$output/sysroot/$destination/$loader"; done
printf '%s\n' "$llvm_target" > "$output/lib/target.txt"
cp VERSION "$output/lib/VERSION"
cp LICENSE NOTICE "$output/"
for package in gcc-11-base libc6 libstdc++6; do
    if [ -f "/usr/share/doc/$package/copyright" ]; then cp -L "/usr/share/doc/$package/copyright" "$output/licenses/$package-copyright"; fi
done
"$cxx" --version > "$output/compiler.txt"
(cd "$output" && sha256sum lib/*.a > SHA256SUMS)
printf '目标 SDK 已准备：%s\n[aot]\ntarget = "%s"\nsysroot = "%s/sysroot"\nruntime_dir = "%s/lib"\n' "$output" "$llvm_target" "$output" "$output"
