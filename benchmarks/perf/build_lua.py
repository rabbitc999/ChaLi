"""按需下载官方 Lua 源码构建带单调时钟的性能宿主。"""
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
MANIFEST = ROOT / 'library/lua/manifest.json'
VERSION = json.loads(MANIFEST.read_text(encoding='utf-8'))['version']
HOST = HERE / 'build-lua' / ('luabench.exe' if os.name == 'nt' else 'luabench')


def main():
    """按依赖时间戳复用宿主，构建后核对它报告的实际 Lua 版本。"""
    dependencies = [Path(__file__), HERE / 'luabench.c', MANIFEST,
                    ROOT / 'tools/fetch_lua.py']
    if not HOST.is_file() or HOST.stat().st_mtime < max(path.stat().st_mtime for path in dependencies):
        subprocess.run([sys.executable, str(ROOT / 'library/lua/setup.py')], cwd=ROOT, check=True)
        source = ROOT / f'build/dependencies/lua/lua-{VERSION}/src'
        files = [path for path in sorted(source.glob('*.c')) if path.name not in ('lua.c', 'luac.c')]
        cc = shutil.which(os.environ.get('CC', 'gcc'))
        if not cc:
            raise SystemExit('Lua 性能宿主需要 GCC')
        HOST.parent.mkdir(parents=True, exist_ok=True)
        command = [cc, '-O2', '-DNDEBUG', '-I', str(source),
                   str(HERE / 'luabench.c'), *map(str, files), '-lm', '-o', str(HOST)]
        if os.name != 'nt':
            command.extend(['-DLUA_USE_LINUX', '-ldl'])
        subprocess.run(command, cwd=ROOT, check=True)
    version = subprocess.check_output([str(HOST), '--version'], text=True).strip()
    if not version.startswith('Lua ' + VERSION + ' '):
        raise SystemExit('Lua 性能宿主实际版本与依赖清单不同：' + version)
    print(version)


if __name__ == '__main__':
    main()
