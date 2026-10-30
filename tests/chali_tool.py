"""验证统一入口的命令选择、四元式与原生导出产物、未知文件诊断，以及 .svmb 的删除提示。"""
from compiler_path import compiler_path
from pathlib import Path
import os
import re
import subprocess
from project_temp import tempfile
ROOT = Path(__file__).resolve().parents[1]
COMPILER = compiler_path()
SOURCE = 'void main() { println("VM command OK"); }\n'


def run(*arguments, cwd=ROOT, success=True, language='zh'):
    """运行真实编译器，并保留失败时的退出状态和诊断。"""
    result = subprocess.run([str(COMPILER), *map(str, arguments)], cwd=cwd,
                            env=dict(os.environ, CHALI_LANG=language), capture_output=True,
                            encoding='utf-8', errors='replace', timeout=120)
    assert (result.returncode == 0) == success, (arguments, result.stdout, result.stderr)
    return result


def main():
    """检查运行方式别名、导出产物、已删除入口的诊断与未知文件提示。"""
    with tempfile.TemporaryDirectory(prefix='chali-vm-tool-') as directory:
        work = Path(directory)
        source = work / 'main.chl'
        source.write_text(SOURCE, encoding='utf-8')
        # 运行方式的四种写法都要落到同一个原生执行路径，显式 --out 保留中间产物。
        for options in [('run', source), ('interpret', source), ('--mode', 'interpret', source),
                        ('--run-mode=interpret', source)]:
            prefix = work / 'run'
            result = run(*options, '--out', prefix)
            assert result.stdout == 'VM command OK\n' and not result.stderr, result
            assert prefix.with_suffix('.ll').is_file(), '显式 --out 应保留 LLVM 模块'
            assert not prefix.with_suffix('.quad').exists(), '正常运行应从 AST 直达 LLVM'
        # 未给 --out 时中间产物落在隐藏的 .chali/ 下，运行后不保留标准库磁盘缓存。
        result = run('run', source, cwd=work)
        assert result.stdout == 'VM command OK\n', result
        leftovers = sorted(entry.name for entry in (work / '.chali').iterdir())
        assert leftovers == [], leftovers
        assert not list(work.glob('chali_out*')), '缺省前缀不应落在工作目录'
        # 已删除的三种 emit 写法都拒绝执行和生成产物。
        for arguments in [('emit', source), ('--mode', 'emit', source), ('run', source, '--emit')]:
            prefix = work / 'removed'
            result = run(*arguments, '--out', prefix, success=False)
            assert result.returncode == 2 and 'run 或 aot' in result.stderr, result
            assert not list(work.glob('removed.*'))
        # LLVM IR 从 AST 直接生成，不执行源程序，也不生成四元式中间文件。
        prefix = work / 'llvm'
        result = run('aot', source, '--emit-llvm', 'out', prefix, '--debug')
        assert not result.stdout and not result.stderr, result
        assert prefix.with_suffix('.ll').is_file()
        assert all(not prefix.with_suffix(suffix).exists() for suffix in ['.quad', '.data.bin', '.layout'])
        # .svmb 已在 1.0.8 删除：命令与 --svmb 选项都要给出明确提示而不是静默忽略。
        for arguments in [('svmb', source, '--out', work / 'svmb'), ('run', source, '--svmb', '--out', work / 'svmb')]:
            result = run(*arguments, success=False)
            assert '已删除' in result.stderr, result.stderr
            assert not (work / 'svmb.svmb').exists(), '不应该再写出 .svmb'
        for unknown in ['unknown-command', 'missing.chl']:
            result = run(unknown, cwd=work, success=False)
            assert '不认识的文件' in result.stderr, result.stderr
        result = run('unknown-command', cwd=work, success=False, language='en')
        assert 'Unknown file' in result.stderr, result.stderr
        help_text = run('--help').stdout
        assert re.search(r'\baot\b', help_text), help_text
        for options in [('aot', source), ('--mode', 'aot', source)]:
            prefix = work / 'aot'
            result = run(*options, '--emit-llvm', '--out', prefix)
            assert not result.stdout and not result.stderr, result
            assert prefix.with_suffix('.ll').is_file()
            assert not prefix.with_suffix('.exe').exists()
    print('Unified commands: run aliases, LLVM artifacts and removed emit, .chali cleanup, removed .svmb, unknown files and aot all passed')


if __name__ == '__main__':
    main()
