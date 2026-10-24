"""自动检测本机与 WSL，构建可用架构及快速安装包，不运行测试门禁。"""
import argparse
import json
import os
from pathlib import Path
import platform
import shutil
import subprocess
import sys
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def execute(command, capture=False):
    """执行构建或工具链探测，保留失败的退出状态。"""
    print('+ ' + subprocess.list2cmdline(list(map(str, command))), flush=True)
    return subprocess.run(list(map(str, command)), cwd=ROOT, check=True, capture_output=capture)


def wsl_distributions():
    """只返回可以执行 Linux 命令的 WSL 发行版，不把工具存在当作已安装系统。"""
    if os.name != 'nt' or not shutil.which('wsl'):
        return []
    try:
        result = subprocess.run(['wsl', '--list', '--quiet'], capture_output=True, timeout=30)
    except (OSError, subprocess.TimeoutExpired):
        return []
    if result.returncode:
        return []
    text = result.stdout.decode('utf-16-le' if b'\0' in result.stdout else 'utf-8', errors='replace')
    names = [name.strip('\ufeff\r\n ') for name in text.splitlines() if name.strip('\ufeff\r\n ')]
    available = []
    for name in names:
        if name.lower().startswith('docker-desktop'):
            continue
        try:
            probe = subprocess.run(['wsl', '-d', name, '--', 'uname', '-s'], capture_output=True, timeout=30)
            if probe.returncode == 0 and probe.stdout.strip() == b'Linux':
                available.append(name)
        except (OSError, subprocess.TimeoutExpired):
            continue
    return available


def native_windows(jobs, variant):
    """构建当前受支持的 Windows 编译器及 MSI，不发布启动验证失败的 x86 版本。"""
    machine = platform.machine().lower()
    arch = {'amd64': 'amd64', 'x86_64': 'amd64', 'x86': 'x86', 'i386': 'x86', 'arm64': 'arm64'}.get(machine)
    if arch is None:
        raise ValueError('未登记的 Windows 架构：' + machine)
    if arch == 'x86':
        raise ValueError('当前不提供 x86 安装包：32 位运行时未通过启动验证')
    version = (ROOT / 'VERSION').read_text().strip()
    label = 'x64' if arch == 'amd64' else arch
    binary = ROOT / f'chali-{version}-llvm-{variant}-windows-{label}.exe'
    execute(['sh', '-c', 'exec "$@"', 'chali-build', 'make', '-j' + str(jobs),
             'CHALI_STATIC_LLVM=' + ('1' if variant == 'static' else '0'), 'CHALI_TARGET_ARCH=' + label, 'TARGET=' + binary.name])
    execute([sys.executable, 'tools/package_installers.py', '--binary', binary, '--platform', 'windows',
             '--arch', arch, '--variant', variant, '--formats', 'msi'])
    print('当前不发布 x86 安装包。', flush=True)


def main():
    """按宿主能力选择构建目标，WSL 缺失时正常完成本机交付。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--jobs', type=int, default=4)
    parser.add_argument('--wsl', choices=['auto', 'never'], default='auto')
    parser.add_argument('--variant', choices=['external', 'static'],
                        default='static' if os.name == 'nt' and platform.machine().lower() in ('amd64', 'x86_64') else 'external',
                        help='Windows x64 默认静态内置 LLVM；external 构建外置工具链版本')
    parser.add_argument('--linux-only', action='store_true', help='Windows 上仅调度 WSL，不重复构建本机')
    args = parser.parse_args()
    if args.jobs < 1:
        parser.error('jobs 必须大于零')
    if os.name == 'nt' or not (ROOT / 'source/chali_main.cpp').is_file():
        execute(['sh', 'gen.sh'])
    else:
        execute([sys.executable, 'tools/embed_manual.py'])
        execute([sys.executable, 'tools/stackvm_sources.py', '--build-assets'])
        execute([sys.executable, 'tools/cas_sources.py', '--build-assets'])
    if os.name == 'nt' and not args.linux_only:
        if args.variant == 'static':
            execute([sys.executable, 'library/llvm/setup_static.py', '--jobs', args.jobs])
        native_windows(args.jobs, args.variant)
    if sys.platform == 'linux':
        execute([sys.executable, 'tools/build_linux_packages.py', '--available', '--jobs', args.jobs])
        return
    distributions = wsl_distributions() if args.wsl == 'auto' else []
    if not distributions:
        print('没有可用 WSL：只保留本机可构建架构及安装包。', flush=True)
        return
    root = execute(['wsl', '--exec', 'wslpath', '-a', str(ROOT)], capture=True).stdout.decode().strip()
    for distribution in distributions:
        execute(['wsl', '-d', distribution, '--', 'sh', '-c',
                 'cd "$1" && if command -v python3.11 >/dev/null; then py=python3.11; else py=python3; fi; exec "$py" tools/build_linux_packages.py --available --jobs "$2"',
                 'chali-wsl-build', root, args.jobs])


if __name__ == '__main__':
    main()
