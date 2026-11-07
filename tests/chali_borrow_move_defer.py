"""宏与普通代码的引用移动、延迟借用及运行时源码位置回归。"""
from pathlib import Path
import argparse
import os
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
CASES = [('branch_local_escape_macro', True, ''),
 ('branch_local_escape_plain', True, ''),
 ('branch_valid_reference_macro', False, 'refs=11,22\n'),
 ('branch_valid_reference_plain', False, 'refs=11,22\n'),
 ('mutable_alias_order_macro', False, 'alias=4\n'),
 ('mutable_alias_order_plain', False, 'alias=4\n'),
 ('move_cleanup_branches_macro', False, 'closed=1\nclosed=2\n'),
 ('move_cleanup_branches_plain', False, 'closed=1\nclosed=2\n'),
 ('branch_move_then_read_macro', True, ''),
 ('branch_move_then_read_plain', True, ''),
 ('alias_after_move_macro', True, ''),
 ('alias_after_move_plain', True, ''),
 ('move_twice_macro', True, ''),
 ('move_twice_plain', True, ''),
 ('loop_move_macro', True, ''),
 ('loop_move_plain', True, ''),
 ('defer_after_move_macro', True, ''),
 ('defer_after_move_plain', True, ''),
 ('resource_value_copy_macro', True, ''),
 ('resource_value_copy_plain', True, ''),
 ('generic_cross_module_macro', False, 'generic=13,14 calls=1 temporary=100 inferred=7\n'),
 ('generic_cross_module_plain', False, 'generic=13,14 calls=1 temporary=100 inferred=7\n'),
 ('generic_generated_records_macro', False, 'records=42,hello\n'),
 ('generic_generated_records_plain', False, 'records=42,hello\n')]


def main():
    """逐项检查用户对照，再运行有效程序及不透明地址的运行时防线。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    env = dict(os.environ, CHALI_LANG='zh', CHALI_QUIET='1')
    with tempfile.TemporaryDirectory(prefix='borrow-move-defer-') as directory:
        work = Path(directory)
        def invoke(mode, source, *options):
            """使用同一编译器收集独立源码的返回值和诊断。"""
            return subprocess.run([str(compiler), mode, str(source), *map(str, options)], cwd=ROOT,
                                  env=env, capture_output=True, text=True, encoding='utf-8', timeout=180)
        for name, rejected, expected in CASES:
            source = (ROOT/'example'/('samples-error' if rejected else 'samples')/('borrow_move_'+name+'.chl'))
            result = invoke('check', source)
            assert bool(result.returncode) == rejected, (name, result)
            if not rejected:
                result = invoke('run', source, '--out', work/name)
                assert (result.returncode, result.stdout, result.stderr) == (0, expected, ''), (name, result)
        for name in ('minimal_move_macro', 'minimal_move_plain', 'minimal_move_function', 'minimal_move_expr',
                     'minimal_double_move', 'minimal_defer_plain', 'minimal_defer_macro'):
            result = invoke('check', ROOT/'example/samples-error'/('borrow_move_'+name+'.chl'))
            assert result.returncode != 0 and 'CHL1205' in result.stderr, (name, result)
        negatives = sorted((ROOT/'example/samples-error').glob('borrow_move_boundary_*.chl'))
        positives = sorted((ROOT/'example/samples').glob('borrow_move_boundary_*.chl'))
        for source in negatives:
            result=invoke('check',source)
            assert result.returncode!=0 and 'CHL1205' in result.stderr,(source,result)
        for source in positives:
            result=invoke('run',source,'--out',work/source.stem)
            expected=(ROOT/'example/expected'/(source.stem+'.txt')).read_text(encoding='utf-8')
            assert (result.returncode,result.stdout,result.stderr)==(0,expected,''),(source,result)
        runtime_source = ROOT/'example/samples-error/borrow_move_runtime_location.chl'
        second = [i for i,line in enumerate(runtime_source.read_text(encoding='utf-8').splitlines(),1) if '$TAKE(ref);' in line][-1]
        for mode, flags in [('run', ['--checked']), ('build', ['--unchecked'])]:
            output = work/('runtime-'+mode)
            result = invoke('build', runtime_source, '--out', output, *flags)
            assert result.returncode == 0, result
            executable = output.with_suffix('.exe') if os.name == 'nt' else output
            result = subprocess.run([str(executable)], cwd=ROOT, env=env, capture_output=True,
                                    text=True, encoding='utf-8', timeout=30)
            assert result.returncode != 0 and 'move 的源对象已经销毁' in result.stderr, result
            assert result.stdout == 'consume=7\nclose=7\n', result
            location = next(line for line in result.stderr.splitlines() if '出错位置：' in line)
            assert f':{second}（main）' in location and 'Handle.close' not in result.stderr, result
    print(f'引用移动与 defer：12 组原始对照、7 个最小复现、{len(negatives)} 个拒绝边界、{len(positives)} 个有效边界及两种运行时定位通过')


if __name__ == '__main__':
    main()
