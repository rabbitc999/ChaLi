#!/bin/sh
# 在 Linux / WSL 的 GNU 工具链中准备与 LLVM 目标匹配的运行时及可移植链接 SDK。
set -eu
root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
cd "$root"
target=${1:-}
jobs=${2:-2}
case "$target" in
    linux-x64|x86_64-unknown-linux-gnu) name=linux-x64; gcc_target=x86_64-linux-gnu; llvm_target=x86_64-unknown-linux-gnu ;;
    linux-x86|i686-unknown-linux-gnu) name=linux-x86; gcc_target=i686-linux-gnu; llvm_target=i686-unknown-linux-gnu ;;
    linux-riscv32|riscv32-unknown-linux-gnu) name=linux-riscv32; gcc_target=riscv32-linux-gnu; llvm_target=riscv32-unknown-linux-gnu ;;
    linux-riscv64|riscv64-unknown-linux-gnu) name=linux-riscv64; gcc_target=riscv64-linux-gnu; llvm_target=riscv64-unknown-linux-gnu ;;
    linux-mipsel|mipsel-unknown-linux-gnu) name=linux-mipsel; gcc_target=mipsel-linux-gnu; llvm_target=mipsel-unknown-linux-gnu ;;
    linux-arm32|armv7-unknown-linux-gnueabihf) name=linux-arm32; gcc_target=arm-linux-gnueabihf; llvm_target=armv7-unknown-linux-gnueabihf ;;
    linux-mips64el|mips64el-unknown-linux-gnuabi64) name=linux-mips64el; gcc_target=mips64el-linux-gnuabi64; llvm_target=mips64el-unknown-linux-gnuabi64 ;;
    linux-arm64|aarch64-unknown-linux-gnu) name=linux-arm64; gcc_target=aarch64-linux-gnu; llvm_target=aarch64-unknown-linux-gnu ;;
    *) echo '用法：sh tools/build_cross_runtime.sh linux-x64|linux-x86|linux-arm32|linux-arm64|linux-riscv32|linux-riscv64|linux-mipsel|linux-mips64el [并行数]' >&2; exit 2 ;;
esac
case "$jobs" in ''|0|*[!0-9]*) echo '并行数必须为正整数' >&2; exit 2 ;; esac
case "$(uname -s)" in Linux) ;; *) echo '请在 Linux 或 WSL 中运行本脚本' >&2; exit 2 ;; esac
cxx=${CXX:-$gcc_target-g++}
cc=${CC:-$gcc_target-gcc}
archiver=${AR:-$gcc_target-ar}
if [ "$name" = linux-riscv32 ] && ! command -v "$cxx" >/dev/null 2>&1; then
    cxx=${CXX:-riscv32-linux-g++}
    cc=${CC:-riscv32-linux-gcc}
    archiver=${AR:-riscv32-linux-ar}
fi
python=${PYTHON:-}
if [ -z "$python" ]; then
    if command -v python3.11 >/dev/null 2>&1; then python=python3.11; else python=python3; fi
fi
command -v "$cxx" >/dev/null
command -v "$cc" >/dev/null
command -v "$archiver" >/dev/null
case "$("$cxx" -dumpmachine)" in
    "$gcc_target"|"$llvm_target") ;;
    riscv32-buildroot-linux-gnu) [ "$name" = linux-riscv32 ] || exit 2 ;;
    *) echo 'CXX 的目标与请求目标不同，拒绝混用对象文件' >&2; exit 2 ;;
esac
mkdir -p "$root/tmp"
export TMPDIR="$root/tmp" TMP="$root/tmp" TEMP="$root/tmp"
build="build/cross-runtime-$name"
output="$root/build/cross-sdk/$name"
mkdir -p "$output/lib" "$output/licenses"
make -j"$jobs" OS=Linux CHALI_RELEASE_PIC=1 PYTHON="$python" CHALI_BUILD_ROOT="$build" CXX="$cxx" CC="$cc" AR="$archiver" \
    "$build/chali-ircode-aot-core/libstackvm.a" "$build/chali-host-records/librecords.a" \
    "$build/chali-aot-host/native_runtime.o" "$build/chali-aot-host/backend_profile.o" \
    "$build/chali-aot-host/system_arena.o" \
    "$build/chali-frontend/lztext.o"
directory=$(mktemp -d "$root/tmp/chali-cross-runtime.XXXXXX")
trap 'rm -f "$directory/merge.mri"; rmdir "$directory"' EXIT HUP INT TERM
cat > "$directory/merge.mri" <<EOF
CREATE $output/lib/libchali_aot_host.a
ADDLIB $root/$build/chali-ircode-aot-core/libstackvm.a
ADDLIB $root/$build/chali-host-records/librecords.a
ADDMOD $root/$build/chali-aot-host/native_runtime.o
ADDMOD $root/$build/chali-aot-host/system_arena.o
ADDMOD $root/$build/chali-aot-host/backend_profile.o
ADDMOD $root/$build/chali-frontend/lztext.o
SAVE
END
EOF
"$archiver" -M < "$directory/merge.mri"
gcc_version=$("$cxx" -dumpversion)
gcc_directory="$output/sysroot/usr/lib/gcc/$gcc_target/$gcc_version"
mkdir -p "$gcc_directory"
for file in libstdc++.a libgcc.a libgcc_eh.a crtbegin.o crtbeginS.o crtend.o crtendS.o; do
    source=$("$cxx" -print-file-name="$file")
    [ -f "$source" ] || { echo "工具链缺少 $file" >&2; exit 1; }
    cp -L "$source" "$gcc_directory/$file"
    case "$file" in *.a) cp -L "$source" "$output/lib/$file" ;; esac
done
libc=$("$cxx" -print-file-name=libc.so)
system_directory=$(dirname -- "$(readlink -f "$libc")")
# 部分 MIPS GNU CRT 明确标记需要可执行栈，随 SDK 记录供 LLD 链接使用。
if [ "$name" = linux-mipsel ] || [ "$name" = linux-mips64el ]; then
    readelf=${READELF:-$gcc_target-readelf}
    sections=$("$readelf" -SW "$system_directory/Scrt1.o" "$system_directory/crt1.o" \
        "$system_directory/crti.o" "$system_directory/crtn.o" "$system_directory/libc_nonshared.a")
    if printf '%s\n' "$sections" | grep -Eq '[.]note[.]GNU-stack[[:space:]].*[[:space:]][A-Z]*X[A-Z]*[[:space:]]'; then
        touch "$output/lib/requires-execstack"
    else
        rm -f "$output/lib/requires-execstack"
    fi
fi
# 保留 GNU 链接脚本中的绝对路径，并同时提供 Clang 的标准 multiarch 搜索目录。
set -- "usr/lib/$gcc_target" "lib/$gcc_target" "usr/$gcc_target/lib"
if [ "$name" = linux-riscv32 ]; then set -- "$@" lib lib32 usr/lib; fi
for destination do
    mkdir -p "$output/sysroot/$destination"
    for file in crt1.o Scrt1.o crti.o crtn.o libc.so libc.so.6 libc_nonshared.a libm.so libm.so.6 libmvec.so.1 libpthread.a libpthread.so.0 libdl.a libdl.so.2 librt.a librt.so.1; do
        source="$system_directory/$file"
        if [ ! -f "$source" ] && [ "$name" = linux-riscv32 ]; then source=$("$cxx" -print-file-name="$file"); fi
        if [ -f "$source" ]; then cp -L "$source" "$output/sysroot/$destination/$file"; fi
    done
    for file in libgcc_s.so libgcc_s.so.1; do
        source=$("$cxx" -print-file-name="$file")
        if [ -f "$source" ]; then cp -L "$source" "$output/sysroot/$destination/$file"; fi
    done
    if [ "$name" = linux-mipsel ] || [ "$name" = linux-riscv32 ]; then
        atomic=$("$cxx" -print-file-name=libatomic.so.1)
        cp -L "$atomic" "$output/sysroot/$destination/libatomic.so.1"
    fi
done
if [ "$name" = linux-arm64 ]; then
    loader=ld-linux-aarch64.so.1
    loader_directory=lib
elif [ "$name" = linux-arm32 ]; then
    loader=ld-linux-armhf.so.3
    loader_directory=lib
elif [ "$name" = linux-mips64el ]; then
    loader=ld.so.1
    loader_directory=lib64
elif [ "$name" = linux-x86 ]; then
    loader=ld-linux.so.2
    loader_directory=lib
elif [ "$name" = linux-riscv32 ]; then
    loader=ld-linux-riscv32-ilp32d.so.1
    loader_directory=lib
elif [ "$name" = linux-riscv64 ]; then
    loader=ld-linux-riscv64-lp64d.so.1
    loader_directory=lib
elif [ "$name" = linux-mipsel ]; then
    loader=ld.so.1
    loader_directory=lib
else
    loader=ld-linux-x86-64.so.2
    loader_directory=lib64
fi
mkdir -p "$output/sysroot/$loader_directory"
loader_source="$system_directory/$loader"
if [ ! -f "$loader_source" ]; then loader_source=$("$cxx" -print-file-name="$loader"); fi
cp -L "$loader_source" "$output/sysroot/$loader_directory/$loader"
for destination in "lib/$gcc_target" "usr/$gcc_target/lib"; do cp -L "$loader_source" "$output/sysroot/$destination/$loader"; done
if [ "$name" = linux-mips64el ]; then
    mkdir -p "$output/sysroot/usr/$gcc_target/lib64"
    cp -L "$loader_source" "$output/sysroot/usr/$gcc_target/lib64/$loader"
fi
printf '%s\n' "$llvm_target" > "$output/lib/target.txt"
cp VERSION "$output/lib/VERSION"
cp LICENSE NOTICE "$output/"
for package in gcc-11-base libc6 libstdc++6; do
    if [ -f "/usr/share/doc/$package/copyright" ]; then cp -L "/usr/share/doc/$package/copyright" "$output/licenses/$package-copyright"; fi
done
"$cxx" --version > "$output/compiler.txt"
(cd "$output" && sha256sum lib/*.a > SHA256SUMS)
printf '目标 SDK 已准备：%s\n[build]\ntarget = "%s"\nsysroot = "%s/sysroot"\nruntime_dir = "%s/lib"\n' "$output" "$llvm_target" "$output" "$output"
