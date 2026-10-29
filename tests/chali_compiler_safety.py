"""对已确认的数值边界、拥有值传播和并发数据映像问题执行定向回归。"""
from compiler_path import compiler_path
import argparse
import os
from pathlib import Path
import subprocess
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def run_native(cxx, work, sanitize):
    """直接编译实际库单元，验证精确数值、溢出拒绝与跨编译器模板兼容。"""
    executable = work / ('native-safety.exe' if os.name == 'nt' else 'native-safety')
    sources = [ROOT / 'tests/chali_native_safety.cpp']
    for pattern in ['source/bignum/bigint*.cpp', 'source/bignum/bigfloat*.cpp', 'source/ast/ast*.cpp']:
        sources.extend(sorted(ROOT.glob(pattern)))
    sources += [ROOT / name for name in ['source/json/json.cpp', 'source/toml/toml.cpp', 'source/toml/toml_source.cpp']]
    flags = ['-std=c++17', '-O1' if sanitize else '-O2', '-DCHALI_WITH_REFLECTION=1', '-DSTACKVM_WITH_REFLECTION=1']
    if sanitize:
        flags += ['-g', '-fsanitize=address,undefined,float-cast-overflow', '-fno-omit-frame-pointer', '-no-pie']
    subprocess.run([cxx, *flags, *map(str, sources), '-o', str(executable)], cwd=ROOT, check=True, timeout=300)
    result = subprocess.run([str(executable)], cwd=work, capture_output=True, text=True, encoding='utf-8', timeout=60)
    assert result.returncode == 0 and result.stdout == 'native adversarial boundaries passed\n' and not result.stderr, result


def run_owned(compiler, work, env):
    """重复成功、失败与容器载荷传播，要求输出正确并且没有资源残留诊断。"""
    result = subprocess.run([str(compiler), 'run', str(ROOT / 'tests/fixtures/compiler_safety_owned_try.chl'), '--out', str(work / 'owned')], cwd=work, capture_output=True, text=True, encoding='utf-8', errors='replace', env=env, timeout=180)
    assert result.returncode == 0 and result.stdout == 'owned propagation cleanup OK\n' and not result.stderr, result


def run_snapshot(compiler, work, env):
    """验证原生产物使用内嵌数据，不读取遗留的外部四元式数据映像。"""
    source = work / 'snapshot.chl'
    count = 2000
    source.write_text('void main(){\n' + 'println("snapshot-safe");\n' * count + '}\n', encoding='utf-8')
    prefix = work / 'snapshot'
    built = subprocess.run([str(compiler), 'aot', str(source), '--out', str(prefix)],
                           cwd=work, capture_output=True, text=True, encoding='utf-8', env=env, timeout=300)
    assert built.returncode == 0, built
    for suffix in ('.data.bin', '.layout', '.quad'):
        assert not prefix.with_suffix(suffix).exists(), suffix
        prefix.with_suffix(suffix).write_bytes(b'invalid external snapshot\0')
    source.write_text('void main(){panic("modified source");}', encoding='utf-8')
    product = prefix.with_suffix('.exe' if os.name == 'nt' else '')
    result = subprocess.run([str(product)], cwd=work, capture_output=True, text=True,
                            encoding='utf-8', env=env, timeout=120)
    assert (result.returncode, result.stdout, result.stderr) == (0, 'snapshot-safe\n' * count, ''), result


def main():
    """执行针对性回归；可显式开启 sanitizer，不扩展随机探索。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--cxx', default='g++')
    parser.add_argument('--sanitize-native', action='store_true')
    parser.add_argument('--native-only', action='store_true')
    args = parser.parse_args()
    env = dict(os.environ, CHALI_LANG='zh', CHALI_NO_STDIN_INPUT='1', ASAN_OPTIONS='detect_leaks=1:halt_on_error=1', UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1')
    with tempfile.TemporaryDirectory(prefix='chali-known-safety-') as directory:
        work = Path(directory)
        run_native(args.cxx, work, args.sanitize_native)
        if not args.native_only:
            run_owned(args.chali.resolve(), work, env)
            run_snapshot(args.chali.resolve(), work, env)
    print('已确认问题回归通过：数值边界、平台模板' + ('' if args.native_only else '、拥有值传播、数据快照'))


if __name__ == '__main__':
    main()
