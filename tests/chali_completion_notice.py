"""验证 run／aot 的双语完成提示、独立输出流及显式静默行为。"""
from pathlib import Path
import argparse
import json
import os
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile


def main():
    """清除业务回归的静默环境，直接核对真实编译及运行失败的消息边界。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--llvm', type=Path)
    options = parser.parse_args()
    compiler = options.chali.resolve()
    backend = ['--llvm', str(options.llvm.resolve())] if options.llvm else []
    with tempfile.TemporaryDirectory(prefix='completion-notice-') as directory:
        work = Path(directory)
        source = work / 'notice 中文.chl'
        valid = 'void main(){println("PROGRAM OUTPUT");}\n'
        source.write_text(valid, encoding='utf-8')
        product = source.with_suffix('.exe' if os.name == 'nt' else '')

        def invoke(command, extra=(), language='zh', quiet=None, success=True):
            """按指定语言和静默状态运行，保留标准输出与标准错误的区别。"""
            environment = dict(os.environ, CHALI_LANG=language)
            environment.pop('CHALI_QUIET', None)
            if quiet is not None:
                environment['CHALI_QUIET'] = quiet
            result = subprocess.run([str(compiler), command, str(source), *backend, *map(str, extra)],
                                    cwd=work, env=environment, capture_output=True, text=True,
                                    encoding='utf-8', errors='replace', timeout=180)
            assert (result.returncode == 0) == success, result
            return result

        for language, label in [('zh', '编译完成：'), ('en', 'Compilation completed: ')]:
            expected = label + str(product) + '\n'
            result = invoke('aot', language=language)
            assert result.stdout == '' and result.stderr == expected, result
            result = invoke('run', language=language)
            assert result.stdout == 'PROGRAM OUTPUT\n' and result.stderr == expected, result
        for flag, suffix in [('--emit-llvm', '.ll'), ('--emit-obj', '.o'), ('--emit-asm', '.s')]:
            result = invoke('aot', [flag])
            assert result.stdout == '' and result.stderr == '编译完成：' + str(source.with_suffix(suffix)) + '\n', result
        result = invoke('run', ['--quiet'])
        assert result.stdout == 'PROGRAM OUTPUT\n' and result.stderr == '', result
        result = invoke('aot', quiet='1')
        assert result.stdout == result.stderr == '', result
        assert invoke('run', quiet='0').stderr.startswith('编译完成：')
        result = invoke('run', ['--quiet', '--stats'])
        assert result.stdout == 'PROGRAM OUTPUT\n' and result.stderr and '编译完成' not in result.stderr, result
        source.write_text('void main(){assert(false);}\n', encoding='utf-8')
        result = invoke('run', success=False)
        assert result.stderr.startswith('编译完成：' + str(product) + '\n'), result
        result = invoke('run', ['--quiet'], success=False)
        assert result.stderr and '编译完成' not in result.stderr, result
        previous = product.read_bytes()
        source.write_text('void main(){missingName();}\n', encoding='utf-8')
        for command in ('run', 'aot'):
            result = invoke(command, success=False)
            assert '编译完成' not in result.stderr and 'missingName' in result.stderr, result
            assert product.read_bytes() == previous
        result = invoke('check', ['--diagnostics=json'], success=False)
        assert json.loads(result.stdout)['diagnostics'] and not result.stderr, result
        source.write_text(valid, encoding='utf-8')
        result = invoke('check')
        assert result.stdout == result.stderr == '', result
        result = invoke('run', ['--help'], language='en')
        assert '--quiet' in result.stdout and 'beside the source' in result.stdout and not result.stderr, result
    print('completion notice: zh/en, publication success, run output, quiet/errors/stats and structured diagnostics passed')


if __name__ == '__main__':
    main()
