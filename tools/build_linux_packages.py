"""在独立源码副本中构建 Linux amd64、ARM64 和 x86 发行包。"""
import argparse
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import platform
import hashlib
import re

ROOT = Path(__file__).resolve().parents[1]
TARGETS = {'amd64': ('x86_64-linux-gnu-gcc', 'x86_64-linux-gnu-g++', 'x86_64-linux-gnu-ar'),
           'x86': ('i686-linux-gnu-gcc', 'i686-linux-gnu-g++', 'i686-linux-gnu-ar'),
           'arm64': ('aarch64-linux-gnu-gcc', 'aarch64-linux-gnu-g++', 'aarch64-linux-gnu-ar')}


def run(command, cwd=ROOT, env=None):
    """直接执行参数列表，构建失败立即停止。"""
    print('+ ' + ' '.join(map(str, command)), flush=True)
    subprocess.run(list(map(str, command)), cwd=cwd, env=env, check=True)


def snapshot(work):
    """复制受版本管理的源码与当前生成资产，保留目标独立的增量对象。"""
    files = subprocess.check_output(['git', 'ls-files', '-z', '--cached', '--others', '--exclude-standard'], cwd=ROOT).decode().split('\0')
    generated = ['source/chali_parser.cpp', 'source/chali_parser.hpp', 'source/chali_parser_user.h', 'source/chali_main.cpp',
                 'source/ircode/core_assets.hpp', 'library/caspp/core_assets.hpp', 'tools/chali_manual.json']
    for name in generated:
        if not (ROOT / name).is_file():
            raise ValueError('先在宿主运行 sh gen.sh，缺少生成资产：' + name)
    work.mkdir(parents=True, exist_ok=True)
    for name in dict.fromkeys(files + generated):
        if not name or not (ROOT / name).is_file():
            continue
        target = work / name
        target.parent.mkdir(parents=True, exist_ok=True)
        if not target.exists() or (ROOT / name).read_bytes() != target.read_bytes():
            shutil.copy2(ROOT / name, target)
    return work


def main():
    """隔离目标对象、记录来源，并为成功的目标生成安装包。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--arch', nargs='+', choices=TARGETS, default=['amd64', 'arm64', 'x86'])
    parser.add_argument('--jobs', type=int, default=4)
    parser.add_argument('--available', action='store_true', help='仅构建宿主或已安装交叉工具链可编译链接的目标')
    parser.add_argument('--formats', nargs='+', choices=['deb', 'rpm', 'tar'])
    args = parser.parse_args()
    if sys.platform != 'linux' or args.jobs < 1:
        parser.error('请在 Linux 或 WSL 中运行，jobs 必须大于零')
    if sys.version_info < (3, 11):
        parser.error('内嵌资产构建需要 Python 3.11 或更新版本')
    version = (ROOT / 'VERSION').read_text().strip()
    declared = re.search(r'compilerVersion\s*=\s*"([^"]+)"', (ROOT / 'source/chali_package.hpp').read_text(encoding='utf-8'))
    if not declared or declared[1] != version:
        raise ValueError('源码内嵌编译器版本与 VERSION 不符')
    release = dict(line.split('=', 1) for line in Path('/etc/os-release').read_text().splitlines() if '=' in line)
    distribution = release['ID'].strip('"') + '-' + release.get('VERSION_ID', 'rolling').strip('"')
    native = {'x86_64': 'amd64', 'aarch64': 'arm64', 'i686': 'x86', 'i386': 'x86'}.get(platform.machine())
    formats = args.formats or [kind for kind, tool in [('deb', 'dpkg-deb'), ('rpm', 'rpmbuild'), ('tar', 'tar')] if shutil.which(tool)]
    for arch in args.arch:
        cc, cxx, ar = ('gcc', 'g++', 'ar') if native == arch else TARGETS[arch]
        missing = [tool for tool in (cc, cxx, ar, 'make') if not shutil.which(tool)]
        if missing:
            if args.available:
                print('跳过 ' + arch + '，缺少工具链：' + ', '.join(missing), flush=True)
                continue
            parser.error('缺少目标工具链：' + ', '.join(missing))
        work = ROOT / 'build' / ('release-' + distribution + '-' + arch)
        snapshot(work)
        temporary = ROOT / 'tmp'
        temporary.mkdir(exist_ok=True)
        env = dict(os.environ, PYTHONUTF8='1', TMPDIR=str(temporary), TMP=str(temporary), TEMP=str(temporary))
        env.pop('OS', None)
        probe = work / 'architecture_probe.cpp'
        probe.write_text('int main() { return 0; }\n')
        result = subprocess.run([cxx, str(probe), '-o', str(work / 'architecture_probe')], env=env, capture_output=True)
        if result.returncode:
            if args.available:
                print('跳过 ' + arch + '，工具链无法完成编译和链接：' + result.stderr.decode(errors='replace'), flush=True)
                continue
            raise ValueError('目标工具链无法完成编译和链接：' + result.stderr.decode(errors='replace'))
        from package_installers import validate
        validate(work / 'architecture_probe', 'linux', arch)
        runner = []
        make_runner = []
        if native != arch:
            emulator = shutil.which({'arm64': 'qemu-aarch64', 'x86': 'qemu-i386', 'amd64': 'qemu-x86_64'}[arch])
            if emulator is None:
                message = '缺少执行交叉编译阶段编译器的 QEMU：' + arch
                if args.available:
                    print('跳过 ' + arch + '，' + message, flush=True)
                    continue
                parser.error(message)
            libc = Path(subprocess.check_output([cc, '-print-file-name=libc.so.6'], text=True).strip()).resolve()
            env['QEMU_LD_PREFIX'] = str(libc.parent.parent)
            run([emulator, probe.with_name('architecture_probe')], work, env)
            runner.append(emulator)
            make_runner.append('CHALI_STDLIB_RUNNER=' + emulator)
        binary = work / ('chali-' + version + '-llvm-external-' + distribution + '-' + ('x64' if arch == 'amd64' else arch))
        if native == arch:
            run([sys.executable, ROOT / 'tools/setup_native_extensions.py', '--build-root', work / 'build/release-pic',
                 '--cc', cc, '--jobs', args.jobs], work, env)
        else:
            print('交叉编译器不嵌入原宿主的 GUI、音频、XML 和密码学归档：' + arch, flush=True)
        run(['make', '-j' + str(args.jobs), 'PYTHON=' + sys.executable, 'CC=' + cc, 'CXX=' + cxx, 'AR=' + ar,
             'CHALI_STATIC_LLVM=0', 'CHALI_RELEASE_PIC=1', 'CHALI_BUILD_ROOT=build/release-pic',
             'LDFLAGS=-Wl,--gc-sections -s -static-libgcc -static-libstdc++', 'TARGET=' + binary.name,
             'TMPDIR=' + str(temporary), 'TMP=' + str(temporary), 'TEMP=' + str(temporary), *make_runner], work, env)
        validate(binary, 'linux', arch)
        from release import compiler_version
        if compiler_version(binary, runner=runner, env=env) != version:
            raise ValueError('目标编译器的实际 LSP 版本与 VERSION 不符：' + arch)
        # 平台与架构已包含在文件名中，发行附件统一平铺，直接供 GitHub Release 上传。
        output = ROOT / 'dist/installers'
        output.mkdir(parents=True, exist_ok=True)
        shutil.copy2(binary, output / binary.name)
        receipt = output / (binary.name + '.build-source.json')
        receipt.write_text(json.dumps({'version': version, 'architecture': arch,
            'binarySha256': hashlib.sha256(binary.read_bytes()).hexdigest(),
            'compiler': subprocess.check_output([cxx, '--version'], text=True).splitlines()[0],
            'sourceCommit': subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip(),
            'workingTreeDirty': bool(subprocess.check_output(['git', 'status', '--porcelain'], cwd=ROOT)),
            'gatesRun': False, 'generatedParser': 'copied from the current host generation'}, indent=2) + '\n')
        run([sys.executable, ROOT / 'tools/package_installers.py', '--binary', binary, '--arch', arch,
             '--platform', 'linux', '--build-metadata', receipt, '--formats', *formats, '--out', output], ROOT, env)


if __name__ == '__main__':
    main()
