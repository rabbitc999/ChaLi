"""验证静态 LLVM 的工具链选项边界，并核对外置变体仍接受显式工具。"""
import argparse
import json
import os
from pathlib import Path
import subprocess
from project_temp import tempfile


def main():
    """核对全部原生命令、工程配置、帮助及程序参数边界。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, required=True)
    parser.add_argument('--external', type=Path)
    args = parser.parse_args()
    compiler = args.chali.resolve()
    env = dict(os.environ, CHALI_LANG='zh', CHALI_QUIET='1')
    with tempfile.TemporaryDirectory(prefix='static-llvm-options-') as directory:
        work = Path(directory)
        source = work/'main.chl'
        source.write_text('void main() { println("LLVM options OK"); }\n', encoding='utf-8')
        sentinel = work/'preserved.ll'
        sentinel.write_bytes(b'preserve existing output')
        def run(binary, *arguments):
            """在独占工作区运行指定变体并收集实际诊断。"""
            return subprocess.run([str(binary), *map(str, arguments)], cwd=work, env=env,
                                  capture_output=True, text=True, encoding='utf-8', timeout=180)
        info = run(compiler, '--build-info')
        assert json.loads(info.stdout)['llvm'] == 'static', info
        for command in ('run', 'build', 'asm', 'ar', 'link'):
            for option in (['--llvm', 'missing-clang'], ['--llvm=missing-clang'], ['--llvm']):
                result = run(compiler, command, *option, '--out', sentinel)
                assert result.returncode == 2 and '静态 LLVM 编译器不支持 --llvm' in result.stderr, result
                assert sentinel.read_bytes() == b'preserve existing output'
        for command in ('asm', 'ar', 'link'):
            result = run(compiler, command, '--help')
            assert result.returncode == 0 and '--llvm' not in result.stdout, result
        config = work/'chali.toml'
        config.write_text('[chali]\nmain = "main.chl"\n[build]\nllvm = "missing-clang"\n', encoding='utf-8')
        for command in ('run', 'build'):
            result = run(compiler, command, source, '--emit-llvm', '--out', work/'configured')
            assert result.returncode == 2 and '[build].llvm' in result.stderr, result
        config.unlink()
        result = run(compiler, 'run', source, '--out', work/'program', '--', '--llvm', 'program-argument')
        assert (result.returncode, result.stdout) == (0, 'LLVM options OK\n'), result
        result = run(compiler, source, '--llvm', 'program-argument')
        assert (result.returncode, result.stdout) == (0, 'LLVM options OK\n'), result
        if args.external:
            external = args.external.resolve()
            assert json.loads(run(external, '--build-info').stdout)['llvm'] == 'external'
            for command in ('run', 'build'):
                result = run(external, command, source, '--llvm', 'missing-clang', '--emit-llvm', '--out', work/command)
                assert result.returncode == 0 and (work/(command+'.ll')).is_file(), result
            config.write_text('[chali]\nmain = \"main.chl\"\n[build]\nllvm = \"missing-clang\"\n', encoding='utf-8')
            result = run(external, 'build', '--emit-llvm', '--out', work/'external-configured')
            assert result.returncode == 0, result
            for command in ('asm', 'ar', 'link'):
                result = run(external, command, '--llvm', 'missing-tool', '--help')
                assert result.returncode == 0 and '--llvm' in result.stdout, result
    print('静态 LLVM：五个命令拒绝工具链选项、工程配置、帮助、既有产物及程序参数边界通过')
    if args.external:
        print('外置 LLVM：显式工具路径仍可用于 LLVM 模块导出及原生命令')


if __name__ == '__main__':
    main()
