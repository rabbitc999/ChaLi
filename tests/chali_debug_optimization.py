"""核对开发／O3 调试选项、最终优先级、项目调试配置及 LLVM IR 中的真实模式。"""
import argparse
import os
from pathlib import Path
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile


def main():
    """比较实际生成的调试描述和块地址，不以命令成功代替模式核对。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    args = parser.parse_args()
    compiler = args.chali.resolve()
    with tempfile.TemporaryDirectory(prefix='debug-optimization-') as temporary:
        work = Path(temporary)
        source = work / 'main.chl'
        source.write_text('void main(){\n int64 value=7;\n println("{}",value);\n}\n', encoding='utf-8')
        cases = [(['--debug'], False), (['--debug-optimized'], True),
                 (['--debug-optimized', '--debug'], False), (['--debug', '--debug-optimized'], True),
                 (['--debug-optimized', '--no-debug'], None), (['--no-debug', '--debug-optimized'], True)]
        for index, (flags, optimized) in enumerate(cases):
            prefix = work / str(index)
            result = subprocess.run([str(compiler), 'aot', str(source), '--emit-llvm', *flags, '--out', str(prefix)],
                                    cwd=work, capture_output=True, text=True, encoding='utf-8', errors='replace', timeout=180)
            assert result.returncode == 0, (flags, result.stdout, result.stderr)
            llvm = prefix.with_suffix('.ll').read_text(encoding='utf-8')
            if optimized is None:
                assert 'producer: "ChaLi"' not in llvm and '@chali_debug_catalog' not in llvm
            else:
                assert ('producer: "ChaLi", isOptimized: ' + str(optimized).lower()) in llvm
                assert ('ptr blockaddress(' in llvm) == (not optimized)
                assert ('DISPFlagOptimized' in llvm) == optimized
        (work / 'chali.toml').write_text('[chali]\nmain="main.chl"\ndebug=true\n', encoding='utf-8')
        result = subprocess.run([str(compiler), 'aot', str(source), '--debug-optimized', '--emit-llvm', '--out', 'project'],
                                cwd=work, capture_output=True, text=True, encoding='utf-8', errors='replace', timeout=180)
        assert result.returncode == 0, (result.stdout, result.stderr)
        assert 'producer: "ChaLi", isOptimized: true' in (work / 'project.ll').read_text(encoding='utf-8')
        help_result = subprocess.run([str(compiler), '--help'], cwd=work, env=dict(os.environ, CHALI_LANG='en'),
                                     capture_output=True, text=True, encoding='utf-8', errors='replace', timeout=30)
        assert help_result.returncode == 0
        assert '--debug-optimized      Keep debug info with O3/LTO' in help_result.stdout
    print('调试优化选项：O0/O3、参数顺序、显式关闭、工程 debug 兼容、无地址锚定及英文帮助通过')


if __name__ == '__main__':
    main()
