"""构建、验证和打包静态内置 LLVM 与外置 LLVM 两种程序及 VSIX 交付。"""
import argparse
import os
import shutil
from pathlib import Path
import sys
import release

ROOT = Path(__file__).resolve().parents[1]


def main():
    """顺序构建两个隔离产物，逐一核对插件字节和发布包元数据，不执行远端发布。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out-dir', type=Path, default=ROOT / 'dist')
    parser.add_argument('--jobs', type=int, default=4)
    parser.add_argument('--package-only', action='store_true')
    parser.add_argument('--skip-tests', action='store_true', help='按授权跳过 AOT 与 VS Code 回归，仅构建并核对产物')
    args = parser.parse_args()
    if os.name != 'nt' or args.jobs < 1:
        parser.error('当前双变体发布已验收目标为 Windows x64；jobs 必须大于零')
    version = release.version_of(ROOT)
    extension = release.editor_version_of(ROOT)
    revision = release.run(['git', 'rev-parse', 'HEAD'], capture=True)
    dirty = bool(release.run(['git', 'status', '--porcelain'], capture=True))
    if not args.package_only:
        release.run([sys.executable, 'library/llvm/setup.py'])
        release.run([sys.executable, 'library/llvm/setup_static.py', '--jobs', str(args.jobs)])
        if not (ROOT / 'source/chali_main.cpp').is_file():
            release.run(['sh', 'gen.sh'])
        if not (ROOT / 'vscode/node_modules/vscode-textmate/package.json').is_file():
            release.run(['npm.cmd', 'ci', '--no-audit', '--no-fund'], cwd=ROOT / 'vscode')
        if not (ROOT / 'doc/book/latex/book.pdf').is_file():
            release.run([sys.executable, 'doc/book/latex/tools/md2tex.py'])
            for _ in range(3):
                release.run(['xelatex', '-interaction=nonstopmode', '-halt-on-error', 'book.tex'], cwd=ROOT / 'doc/book/latex')
    for variant in ('external', 'static'):
        output = args.out_dir.resolve() / ('llvm-' + variant)
        output.mkdir(parents=True, exist_ok=True)
        binary = output / f'chali-{version}-llvm-{variant}-windows-x64.exe'
        env = dict(os.environ, CHALI_LANG='zh', PYTHONUTF8='1', CHALI_BIN=str(binary))
        if not args.package_only:
            release.run(['sh', '-c', 'exec "$@"', 'chali-variant', 'make', '-j' + str(args.jobs),
                         'CHALI_STATIC_LLVM=' + ('1' if variant == 'static' else '0'),
                         'TARGET=' + binary.relative_to(ROOT).as_posix()], env=env)
            if not args.skip_tests:
                release.run([sys.executable, 'tests/chali_aot.py', '--chali', binary], env=env)
                if variant == 'static':
                    release.run([sys.executable, 'tests/chali_aot_static.py', '--chali', binary], env=env)
            release.run(['npm.cmd', 'run', 'package', '--', '--llvm-variant', variant,
                         '--compiler', str(binary), *(['--skip-tests'] if args.skip_tests else [])], cwd=ROOT / 'vscode', env=env)
        vsix = ROOT / f'vscode/dist/chali-editor-{extension}-llvm-{variant}.vsix'
        release.verify_assets(ROOT, binary, vsix, version, True, env)
        if variant == 'static':
            shutil.copy2(vsix, ROOT / f'vscode/dist/chali-editor-{extension}.vsix')
        archive = release.package_assets(ROOT, output, binary, vsix, version, 'windows-x64', revision, dirty, variant)
        print('已生成：' + str(archive), flush=True)


if __name__ == '__main__':
    raise SystemExit(main())
