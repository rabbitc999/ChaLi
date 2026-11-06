"""发布构建入口回归：检查默认配置、产物格式、项目表和已删除命令。"""
from pathlib import Path
import argparse
import json
import os
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    """使用真实编译器核对命令行边界和发布产物，不执行全量后端回归。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    args = parser.parse_args()
    compiler = args.chali.resolve()
    env = dict(os.environ, CHALI_LANG='en')
    if os.name == 'nt':
        env.setdefault('CHALI_LLVM_CLANG', str(ROOT / 'build/dependencies/llvm/toolchain/bin/clang.exe'))
    with tempfile.TemporaryDirectory(prefix='build-command-') as directory:
        work = Path(directory)
        source = work / '中文 & release.chl'
        source.write_text('void main() { println("release build OK"); }\n', encoding='utf-8')

        def invoke(*values, ok=True):
            """传递独立 argv，失败时输出完整诊断；构建不能隐式运行程序。"""
            result = subprocess.run([str(compiler), *map(str, values)], cwd=work, env=env,
                                    capture_output=True, text=True, encoding='utf-8', timeout=180)
            assert (result.returncode == 0) == ok, (values, result.returncode, result.stdout, result.stderr)
            return result

        help_text = invoke('help').stdout
        assert 'Release build' in help_text and '  aot ' not in help_text
        info = json.loads(invoke('--build-info').stdout)
        assert info['execution'] == 'aot' and 'vm' not in info and 'svmb' not in info
        for words in [('build', source, 'out', work / 'plain'), ('--mode', 'build', source, '--out', work / 'mode')]:
            result = invoke(*words)
            assert not result.stdout, result.stdout
            prefix = work / ('plain' if words[0] == 'build' else 'mode')
            assert 'checks: off' in prefix.with_suffix('.ll').read_text(encoding='utf-8')
            product = Path(str(prefix) + ('.exe' if os.name == 'nt' else ''))
            run = subprocess.run([str(product)], capture_output=True, text=True, timeout=30)
            assert run.returncode == 0 and run.stdout.strip() == 'release build OK', run
        for flag, suffix in [('--emit-llvm', '.ll'), ('--emit-obj', '.o'), ('--emit-asm', '.s')]:
            prefix = work / suffix[1:]
            invoke('build', source, flag, '--checked', '--out', prefix)
            assert prefix.with_suffix(suffix).stat().st_size > 0
            assert 'checks: on' in prefix.with_suffix('.ll').read_text(encoding='utf-8')
        invoke('build', source, '--target', 'linux-arm64', '--emit-llvm', '--out', work / 'arm64')
        assert 'aarch64' in (work / 'arm64.ll').read_text(encoding='utf-8')
        invoke('build', source, '--emit-obj', '--emit-asm', ok=False)
        invoke('aot', source, ok=False)
        invoke('--mode', 'aot', source, ok=False)
        manifest = work / 'chali.toml'
        manifest.write_text('[build]\noutput="llvm"\n', encoding='utf-8')
        invoke('build', source, '--out', work / 'project')
        assert (work / 'project.ll').exists() and not (work / 'project.exe').exists()
        manifest.write_text('[aot]\noutput="llvm"\n', encoding='utf-8')
        failure = invoke('build', source, '--out', work / 'old-table', ok=False)
        assert 'aot' in failure.stderr and 'table' in failure.stderr
        manifest.unlink()
        project = work / 'new-project'
        invoke('new', project)
        readme = (project / 'README.md').read_text(encoding='utf-8')
        assert 'chali build ' in readme and 'VM' not in readme and 'svmb' not in readme
    print('Release build command: execution, defaults, formats, cross-target IR, project table and removed entries passed')


if __name__ == '__main__':
    main()
