"""验证任意维数组、可扩容数组和统一 new 分配在 VM/AOT 的一致性。"""
from pathlib import Path
import argparse
import os
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    """执行正例、运行时边界反例和旧分配语法诊断。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--vm-only', action='store_true')
    args = parser.parse_args()
    compiler = args.chali.resolve()
    env = dict(os.environ, CHALI_LANG='zh', CHALI_NO_STDIN_INPUT='1')
    backends = ('run',) if args.vm_only else ('run', 'aot')
    positive = ('multidimensional_arrays', 'dynamic_array', 'copy_interface', 'array2d', 'rarray',
                'fixed_array_values', 'allocated_default_ownership', 'drop_destructor')
    failures = {
        'first_dimension': ('int64 a[2,3,4]; a[2,0,0]=1;', '第 1 维'),
        'middle_dimension': ('int64 a[2,3,4]; a[0,3,0]=1;', '第 2 维'),
        'last_dimension': ('int64 a[2,3,4]; a[0,0,4]=1;', '第 3 维'),
        'negative_index': ('int64 a[2,3,4]; a[0,-1,0]=1;', '第 2 维'),
        'dynamic_read': ('std::array::DynamicArray<int64> a; a.reserve(8); println("{}",a[0]);', '下标越界'),
        'dynamic_write': ('std::array::DynamicArray<int64> a; a.push(1); a.set(1,2);', '下标越界'),
        'negative_capacity': ('std::array::DynamicArray<int64> a; a.reserve(-1);', '容量必须非负'),
        'negative_length': ('std::array::DynamicArray<int64> a; a.resize(-1);', '长度必须非负'),
    }
    with tempfile.TemporaryDirectory(prefix='multidimensional-arrays-') as directory:
        work = Path(directory)

        def execute(source, backend, stem):
            """运行 VM 或先构建再执行独立 AOT 程序。"""
            target = work / (stem + '-' + backend + ('.exe' if os.name == 'nt' else ''))
            command = [str(compiler), backend, str(source), '--out', str(target)]
            result = subprocess.run(command, cwd=ROOT, env=env, capture_output=True,
                                    text=True, encoding='utf-8', timeout=300)
            if backend == 'aot':
                assert result.returncode == 0, (stem, 'AOT 编译', result.stdout, result.stderr)
                result = subprocess.run([str(target)], cwd=ROOT, env=env, capture_output=True,
                                        text=True, encoding='utf-8', timeout=90)
            return result

        for name in positive:
            expected = (ROOT / 'example/expected' / (name + '.txt')).read_text(encoding='utf-8')
            for backend in backends:
                result = execute(ROOT / 'example/samples' / (name + '.chl'), backend, name)
                assert result.returncode == 0 and result.stdout == expected and not result.stderr, (
                    name, backend, result.returncode, result.stdout, result.stderr)
                print(name + ': ' + backend + ' 通过', flush=True)
        for name, (body, diagnostic) in failures.items():
            path = work / (name + '.chl')
            path.write_text('import std::array;\nvoid main(){' + body + '}\n', encoding='utf-8')
            for backend in backends:
                result = execute(path, backend, name)
                expected_diagnostic = diagnostic
                if backend == 'aot':
                    expected_diagnostic = {
                        'first_dimension': '下标 2，长度 2',
                        'middle_dimension': '下标 3，长度 3',
                        'last_dimension': '下标 4，长度 4',
                        'negative_index': '下标 -1，长度 3',
                    }.get(name, diagnostic)
                assert result.returncode != 0 and expected_diagnostic in result.stdout + result.stderr, (
                    name, backend, result.stdout, result.stderr)
                print(name + ': ' + backend + ' 拒绝通过', flush=True)
        for name, body in {
            'partial_index': 'int64 a[2,3,4]; println("{}",a[0,1]);',
            'partial_length': 'int64 a[2,3,4]; println("{}",len(a[100]));',
            'extra_index': 'int64 a[2,3,4]; a[0,1,2,3]=1;',
            'zero_extent': 'int64 a[2,0,4];',
            'negative_extent': 'int64 a[2,-1,4];',
            'removed_heap': 'int64 mut& p=heap int64;',
            'removed_heap_ctor': 'int64 mut& p=heap int64(1);',
        }.items():
            path = work / (name + '.chl')
            path.write_text('void main(){' + body + '}\n', encoding='utf-8')
            result = subprocess.run([str(compiler), 'check', str(path)], cwd=ROOT, env=env,
                                    capture_output=True, text=True, encoding='utf-8', timeout=120)
            assert result.returncode != 0, (name, '错误形式被接受')
        path = work / 'heap_identifier.chl'
        path.write_text('void main(){int64 heap=7;int64 mut& p=new int64;*p=heap;assert(*p==7);p.drop();}\n', encoding='utf-8')
        for backend in backends:
            result = execute(path, backend, 'heap_identifier')
            assert result.returncode == 0, (backend, result.stdout, result.stderr)
    print('数组与 new 分配验证通过', flush=True)


if __name__ == '__main__':
    main()
