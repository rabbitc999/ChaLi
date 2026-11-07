"""验证泛型单态化、嵌套组合、类型身份、推导与非法实例诊断。"""
from compiler_path import compiler_path
from pathlib import Path
import argparse
import os
import subprocess
import shutil
from project_temp import tempfile
ROOT = Path(__file__).resolve().parents[1]

def main():
    """运行底层展开单测、原生类型行为压力样例及非法实例回归。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    args = parser.parse_args()
    compiler = args.chali.resolve()
    if os.name == 'nt' and (not compiler.is_file()):
        compiler = compiler.with_suffix('.exe')
    env = dict(os.environ, CHALI_LANG='zh')

    def run(*command, cwd=ROOT):
        return subprocess.run([str(x) for x in command], cwd=cwd, env=env, capture_output=True, encoding='utf-8', errors='replace', timeout=240)
    with tempfile.TemporaryDirectory(prefix='chali-generic-monomorphization-') as folder:
        work = Path(folder)
        unit = work / ('unit.exe' if os.name == 'nt' else 'unit')
        result = run('g++', '-std=c++17', '-I' + str(ROOT / 'source'), '-I' + str(ROOT / 'library'), '-I' + str(ROOT / 'source/preprocess'), '-I' + str(ROOT / 'source/ircode'), ROOT / 'tests/chali_generic_monomorphization_test.cpp', *[ROOT / 'source' / name for name in ('chali_generic.cpp', 'chali_lambda.cpp', 'chali_generic_closure.cpp', 'syntax/source_text.cpp', 'syntax/constant_expression.cpp')], '-o', unit)
        assert result.returncode == 0, result.stderr
        result = run(unit)
        assert (result.returncode, result.stdout, result.stderr) == (0, 'generic monomorphization unit tests OK\n', ''), result
        standalone = work / ('ChaLi.exe' if os.name == 'nt' else 'chali')
        shutil.copy2(compiler, standalone)
        source = ROOT / 'example/samples/generic_type_behavior.chl'
        for backend in ('run',):
            output = work / (backend + ('.exe' if os.name == 'nt' else ''))
            result = run(compiler, backend, source, 'out', output, cwd=ROOT)
            assert result.returncode == 0, (backend, result.stdout, result.stderr)
            assert (result.returncode, result.stdout, result.stderr) == (0, 'generic type behavior: iterations=512 nested=8\n', ''), result
            print(backend + '：泛型单态化与嵌套组合通过', flush=True)
        host_output = work / ('host.exe' if os.name == 'nt' else 'host')
        diagnostics = {'too_many': '至多接受', 'duplicate_parameter': '重复的泛型参数', 'forward_default': '只能引用前面的类型参数', 'self_default': '只能引用前面的类型参数', 'empty_default': '默认类型为空', 'plain_constraint': '未实现接口', 'generic_constraint': '未实现接口', 'nested_constraint': '未实现接口', 'inference_conflict': '推导冲突', 'unknown_type': 'Missing', 'value_recursion': '绕回了自己', 'growing_recursion': '递归类型展开', 'recursive_default': '递归默认类型', 'readonly_generic': '只读', 'nested_borrow_escape': '生命周期'}
        cases={name:((ROOT/'example/samples-error'/('generic_type_'+name+'.chl')).read_text(encoding='utf-8'),diagnostic) for name,diagnostic in diagnostics.items()}
        for name, (code, diagnostic) in cases.items():
            if 'main()' not in code:
                code += 'void main(){}'
            invalid = work / (name + '.chl')
            invalid.write_text(code, encoding='utf-8')
            result = run(compiler, 'check', invalid, 'json')
            assert result.returncode != 0 and diagnostic in result.stdout + result.stderr, (name, result.returncode, result.stdout, result.stderr)
if __name__ == '__main__':
    main()
