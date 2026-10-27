"""核对 AOT 可达常量裁剪、稀疏初始化、布局与程序体积。"""
from pathlib import Path
import argparse
import hashlib
import json
import os
import platform
import re
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    """对同源程序执行 VM、完整映像 AOT、默认 AOT，并保留体积和摘要。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--baseline', type=Path)
    parser.add_argument('--out', type=Path, default=ROOT / 'tmp/aot-data-results.json')
    args = parser.parse_args()
    compiler = args.chali.resolve()
    work = Path(tempfile.mkdtemp(prefix='aot-data-'))
    env = dict(os.environ)
    env.setdefault('CHALI_LLVM_CLANG', str(ROOT / 'build/dependencies/llvm/toolchain/bin/clang.exe') if os.name == 'nt' else 'clang')
    env.pop('CHALI_AOT_KEEP_DATA', None)
    report = {'version': (ROOT / 'VERSION').read_text().strip(), 'platform': platform.platform(),
              'configuration': 'external LLVM; default optimization and reflection; explicit --checked for all AOT variants',
              'compiler_sha256': hashlib.sha256(compiler.read_bytes()).hexdigest(), 'work': str(work), 'cases': {}}
    if args.baseline:
        report['baseline_sha256'] = hashlib.sha256(args.baseline.read_bytes()).hexdigest()
    cases = {
        'empty': ('void main() {}\n', ''),
        'unused_literal': (''.join('String unused_' + str(index) + '() { return "DEAD_POOL_SENTINEL_' + str(index) + '_' + 'Z' * 1024 + '"; }\n' for index in range(64)) + 'void main() { println("alive"); }\n', 'alive\n'),
        'zero_array': ('void main() { int64 values[8192]; assert(values[0] == 0 && values[8191] == 0); values[8191] = 47; println("{} {}", len(values), values[8191]); }\n', '8192 47\n'),
        'empty_string': ('void main() { String value = ""; String duplicate = value; assert(duplicate.byteLength() == 0); println("{}", duplicate.byteLength()); }\n', '0\n'),
        'unused_function_value': ('int64 dead_target(int64 x) { println("DEAD_FUNCTION_BODY_SENTINEL"); return x + 1; }\nint64 never_called() { int64 fn(int64) method = dead_target; return method(4); }\nvoid main() { println("alive"); }\n', 'alive\n'),
    }
    fixtures = ['fixed_array_values', 'array_parameter_values', 'function_types', 'lambda_callbacks', 'raw_pointer', 'raw_storage']
    for name in fixtures:
        cases[name] = ((ROOT / 'example/samples' / (name + '.chl')).read_text(encoding='utf-8'), None)
    for name, (source, expected) in cases.items():
        path = work / (name + '.chl')
        path.write_text(source, encoding='utf-8')
        entry = {'source': source, 'source_sha256': hashlib.sha256(source.encode()).hexdigest(), 'results': {}}
        report['cases'][name] = entry

        def invoke(command, label, execution_env=env):
            """保存原始输出，失败立即终止，禁止发布部分通过结果。"""
            result = subprocess.run(list(map(str, command)), cwd=ROOT, env=execution_env, capture_output=True, text=True, encoding='utf-8', timeout=900)
            (work / (name + '-' + label + '.log')).write_text(result.stdout + result.stderr, encoding='utf-8')
            assert result.returncode == 0, (name, label, result.returncode, work)
            return result

        virtual = invoke([compiler, 'run', path], 'vm')
        if expected is None:
            expected = virtual.stdout
        assert virtual.stdout == expected, (name, virtual.stdout, expected)
        variants = [('full', compiler, dict(env, CHALI_AOT_KEEP_DATA='1')), ('sparse', compiler, env)]
        if args.baseline:
            variants.insert(0, ('baseline', args.baseline.resolve(), env))
        for label, binary, execution_env in variants:
            prefix = work / (name + '-' + label)
            invoke([binary, 'aot', path, '--checked', '--out', prefix], label + '-build', execution_env)
            product = Path(str(prefix) + ('.exe' if os.name == 'nt' else ''))
            output = invoke([product], label + '-run', execution_env)
            assert output.stdout == expected and not output.stderr, (name, label, output)
            llvm = prefix.with_suffix('.ll').read_text(encoding='utf-8')
            image = re.search(r'@chali_data_image = constant \[(\d+) x i8\]', llvm)
            layout = re.search(r'@chali_data_layout = constant \[(\d+) x i8\]', llvm)
            entry['results'][label] = {'exe_bytes': product.stat().st_size, 'image_bytes': int(image[1]), 'layout_bytes': int(layout[1]), 'sha256': hashlib.sha256(product.read_bytes()).hexdigest(), 'stdout': output.stdout, 'image_summary': re.findall(r'^; AOT data image:.*$', llvm, re.M)}
            if name == 'unused_literal' and label == 'sparse':
                dead = ''.join('\\%02X' % byte for byte in b'DEAD_POOL_SENTINEL_')
                assert dead not in llvm, 'unreachable literal remains in LLVM pool'
            if name == 'unused_function_value' and label == 'sparse':
                assert not re.search(r'define[^\n]*@f_dead_target\(', llvm), 'unreachable function-value target remains a root'
                dead = ''.join('\\%02X' % byte for byte in b'DEAD_FUNCTION_BODY_SENTINEL')
                assert dead not in llvm, 'unreachable callback constants remain'
        for field in ['image_bytes', 'layout_bytes']:
            assert entry['results']['sparse'][field] <= entry['results']['full'][field], (name, field, entry)
        args.out.parent.mkdir(parents=True, exist_ok=True)
        args.out.write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
        print(name, {key: value['exe_bytes'] for key, value in entry['results'].items()}, flush=True)
    print('AOT sparse data checks passed:', args.out)


if __name__ == '__main__':
    main()
