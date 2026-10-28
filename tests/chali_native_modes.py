"""1.0.8 简单验收：原生运行、检查默认值、数组、死根与已删除入口。"""
from pathlib import Path
import argparse
import hashlib
import json
import os
import re
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    """只使用一个小程序验证运行和导出，不执行完整回归或性能基准。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--baseline', type=Path)
    parser.add_argument('--out', type=Path, default=ROOT / 'tmp/native-modes-results.json')
    args = parser.parse_args()
    compiler = args.chali.resolve()
    work = Path(tempfile.mkdtemp(prefix='native-modes-'))
    env = dict(os.environ)
    env.setdefault('CHALI_LLVM_CLANG', str(ROOT / 'build/dependencies/llvm/toolchain/bin/clang.exe') if os.name == 'nt' else 'clang')
    report = {'scope': 'simple smoke only; no full regression or performance campaign',
              'compiler_sha256': hashlib.sha256(compiler.read_bytes()).hexdigest(), 'commands': []}

    def execute(command, expected=0, execution_env=env):
        """保存退出码与原始输出，任何不一致使简单验收失败。"""
        result = subprocess.run(list(map(str, command)), cwd=ROOT, env=execution_env, capture_output=True,
                                text=True, encoding='utf-8', timeout=300)
        report['commands'].append({'command': list(map(str, command)), 'code': result.returncode,
                                   'stdout': result.stdout, 'stderr': result.stderr})
        args.out.parent.mkdir(parents=True, exist_ok=True)
        args.out.write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
        assert result.returncode == expected, (command, result.returncode, result.stderr, work)
        return result

    info = json.loads(execute([compiler, '--build-info']).stdout)
    assert info['version'] == (ROOT / 'VERSION').read_text().strip() and info['execution'] == 'aot'
    assert info['vm'] is False and info['svmb'] is False
    report['build_info'] = info
    source = work / 'native 中文 & space.chl'
    source.write_text('''int64 globals[16];
int64 dead_target(int64 value) { println("UNREACHABLE_CALLBACK_SENTINEL"); return value + 1; }
int64 never_called() { int64 fn(int64) method = dead_target; return method(4); }
void main(List<String> args) {
    int64 values[1024];
    assert(globals[0] == 0 && globals[15] == 0 && values[0] == 0);
    values[1023] = 42;
    println("{} {} {}", len(values), values[0], values[1023]);
    println("{} {}", args.len(), args[0]);
}
''', encoding='utf-8')
    report['source'] = source.read_text(encoding='utf-8')
    report['source_sha256'] = hashlib.sha256(source.read_bytes()).hexdigest()
    expected = '1024 0 42\n1 中文 & space\n'
    run_prefix = work / 'checked'
    result = execute([compiler, 'run', source, '--out', run_prefix, '--', '中文 & space'])
    assert result.stdout == expected, result
    checked = run_prefix.with_suffix('.ll').read_text(encoding='utf-8')
    assert 'checks: on' in checked
    prefix = work / 'unchecked'
    execute([compiler, 'aot', source, '--out', prefix])
    llvm = prefix.with_suffix('.ll').read_text(encoding='utf-8')
    assert 'checks: off' in llvm
    product = Path(str(prefix) + ('.exe' if os.name == 'nt' else ''))
    assert execute([product, '中文 & space']).stdout == expected
    dead = ''.join('\\%02X' % byte for byte in b'UNREACHABLE_CALLBACK_SENTINEL')
    assert dead not in llvm and not re.search(r'define[^\n]*dead_target\(', llvm)
    assert not re.search(r'constant \[1024 x i64\]', llvm)
    report['image_summary'] = re.findall(r'^; AOT data image:.*$', llvm, re.M)
    report['native_bytes'] = product.stat().st_size
    report['llvm_bytes'] = prefix.with_suffix('.ll').stat().st_size
    if args.baseline:
        baseline = args.baseline.resolve()
        report['baseline_sha256'] = hashlib.sha256(baseline.read_bytes()).hexdigest()
        old = work / 'baseline'
        execute([baseline, 'aot', source, '--unchecked', '--out', old])
        old_product = Path(str(old) + ('.exe' if os.name == 'nt' else ''))
        assert execute([old_product, '中文 & space']).stdout == expected
        report['baseline_bytes'] = old_product.stat().st_size
        report['baseline_llvm_bytes'] = old.with_suffix('.ll').stat().st_size
    for command in ([compiler, 'svmb', source], [compiler, 'run', source, '--svmb'], [compiler, '--mode', 'svmb', source]):
        execute(command, expected=2)
    raw = work / 'quad'
    execute([compiler, 'aot', source, '--emit-llvm', '--out', raw])
    assert raw.with_suffix('.ll').exists() and not raw.with_suffix('.quad').exists() and not raw.with_suffix('.svmb').exists()
    symbols = subprocess.check_output(['ar', 't', ROOT / 'build/chali-ircode-aot-core/libstackvm.a'], text=True)
    assert not any(name in symbols for name in ['vm_execution.o', 'register_core.o', 'program_binary.o'])
    report['interpreter_and_bytecode_objects_absent'] = True
    report['passed'] = True
    args.out.write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    print('Simple native smoke passed:', args.out)
    print('EXE bytes:', report.get('baseline_bytes'), '->', report['native_bytes'])


if __name__ == '__main__':
    main()
