"""验证自举二进制读取、浮点位模式、所有现有错误例与原生四组合。"""
from compiler_path import compiler_path
from pathlib import Path
import argparse
import hashlib
import json
import os
import struct
import subprocess
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
BASELINE = ROOT / 'tests/fixtures/binary_float_bits.json'
PATTERNS = {
    32: [0, 0x80000000, 0x3fc00000, 0x7f800000, 0xff800000, 0x7fc12345,
         0x7fa12345, 1, 0x7fffff, 0x800000, 0x7f7fffff, 0x3f800001],
    64: [0, 0x8000000000000000, 0x3ff8000000000000, 0x7ff0000000000000, 0xfff0000000000000,
         0x7ff8123456789abc, 0x7ff0123456789abc, 1, 0xfffffffffffff, 0x10000000000000,
         0x7fefffffffffffff, 0x3ff0000000000001],
}


def bit_program():
    """通过公开浮点编码入口记录标量和数组读取的实际规范位模式。"""
    code = ['import std::binary;', 'void main(){']
    for width, values in PATTERNS.items():
        for endian in ('little', 'big'):
            raw = b''.join(value.to_bytes(width // 8, endian) for value in values)
            code += ['{', f'uint8 data[{len(raw)}];']
            code += [f'data[{i}]=(uint8){value};' for i, value in enumerate(raw)]
            code += [f'std::binary::BinaryReader reader=std::binary::BinaryReader::fromArray(data,{len(raw)});',
                     'reader.setBigEndian(' + ('true' if endian == 'big' else 'false') + ');']
            for array in (False, True):
                if array:
                    code += ['assert(reader.seek(0));', f'List<float{width}> values=reader.readFloat{width}Array({len(values)});']
                code += ['{int64 i=0;', f'while(i<{len(values)}){{',
                         f'float{width} value=' + ('values[i];' if array else f'reader.readFloat{width}();'),
                         f'List<uint8> bytes=std::lang::float{width}ToBytes(value,false);',
                         f'print("{width}:{endian}:{"array" if array else "scalar"}:{{}}",i);',
                         'int64 j=0;while(j<bytes.len()){print(":{}",(int64)bytes[j]);j=j+1;}println("");i=i+1;}}']
            code += ['assert(reader.remaining()==0);', '}']
    return '\n'.join(code + ['}'])


def main():
    """比较原生行为与冻结旧实现，不再执行已删除的 VM／胶水裁剪产物。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--record-baseline', action='store_true')
    args = parser.parse_args()
    compiler = args.chali.resolve()
    environment = dict(os.environ, CHALI_LANG='zh')
    with tempfile.TemporaryDirectory(prefix='binary-selfhost-') as directory:
        work = Path(directory)
        sequence = 0

        def run(command, success=True):
            result = subprocess.run([str(x) for x in command], cwd=ROOT, env=environment,
                                    capture_output=True, encoding='utf-8', errors='replace', timeout=240)
            assert (result.returncode == 0) == success, (command, result.returncode, result.stdout, result.stderr)
            return result

        def execute(source, success=True):
            nonlocal sequence
            sequence += 1
            argument = source.relative_to(ROOT) if source.is_relative_to(ROOT / 'example') else source
            return run([compiler, 'run', argument, '--out', work / f'run-{sequence}'], success)

        bits = work / 'bits.chl'
        bits.write_text(bit_program(), encoding='utf-8')
        if args.record_baseline:
            digest = hashlib.sha256(compiler.read_bytes()).hexdigest()
            assert digest == 'ea50e30cbb086d5b4697ed7c9b59d0b0aba6cfe318667142b454da31d0e54a52', '仅允许已核对的 e4c28f83 编译器冻结旧行为'
            output = execute(bits)
            assert not output.stderr and len(output.stdout.splitlines()) == 96
            BASELINE.write_text(json.dumps(dict(source_commit='e4c28f83', compiler_sha256=digest,
                                                patterns={str(k): v for k, v in PATTERNS.items()}, output=output.stdout),
                                           ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
            print('old binary reader: 96 scalar/array float bit-pattern results frozen')
            return
        baseline = json.loads(BASELINE.read_text(encoding='utf-8'))
        assert baseline['patterns'] == {str(k): v for k, v in PATTERNS.items()}
        samples = ['std_binary', 'lang_float_to_int', 'lang_float_bytes', 'lang_float64_bytes']
        for allocator in ('malloc', 'pool'):
            for checks in ('checked', 'unchecked'):
                for name in samples + ['bits', 'edges']:
                    source = bits if name == 'bits' else ROOT / 'tests/fixtures/binary_edge_cases.chl' if name == 'edges' else ROOT / 'example/samples' / (name + '.chl')
                    target = work / f'{name}-{allocator}-{checks}.exe'
                    run([compiler, 'aot', source, '--allocator', allocator, '--' + checks, '--out', target])
                    result = run([target])
                    expected = baseline['output'] if name == 'bits' else 'binary edge cases OK\n' if name == 'edges' else (ROOT / 'example/expected' / (name + '.txt')).read_text(encoding='utf-8')
                    assert (result.stdout, result.stderr) == (expected, ''), (name, allocator, checks, result)
                print(f'{allocator}/{checks}: binary scalars, arrays, strings and 96 float bit-pattern results passed', flush=True)
        prefix = work / 'binary-ir'
        run([compiler, 'aot', ROOT / 'example/samples/std_binary.chl', '--emit-llvm', '--out', prefix])
        assert 'chali_std_bind_binary' not in prefix.with_suffix('.ll').read_text(encoding='utf-8')
        errors = sorted((ROOT / 'example/samples-error').glob('binary_*.chl')) + sorted((ROOT / 'example/samples-error').glob('float_int*.chl'))
        for source in errors:
            expected = (ROOT / 'example/expected-error' / (source.stem + '.txt')).read_text(encoding='utf-8').strip()
            result = execute(source, False)
            assert result.stderr.splitlines()[0] == expected, (source, result.stderr, expected)
        print(f'{len(errors)} existing binary/float failure cases passed', flush=True)
        for value, label in [(float('nan'), 'nan'), (float('inf'), 'inf'), (-float('inf'), 'negative-inf'), (-0.0, 'negative-zero')]:
            for fmt, kind in [('f', 'Float32'), ('d', 'Float64')]:
                raw = struct.pack('<' + fmt, value)
                setup = ['import std::binary;', 'void main(){', f'uint8 bytes[{len(raw)}];']
                setup += [f'bytes[{i}]=(uint8){b};' for i, b in enumerate(raw)]
                setup += [f'std::binary::BinaryReader reader=std::binary::BinaryReader::fromArray(bytes,{len(raw)});',
                          f'{kind.lower()} value=reader.read{kind}();']
                source = work / 'special.chl'
                for width in (32, 64):
                    check = ('assert(std::lang::toInt32(value).unwrap()==(int32)0);' if label == 'negative-zero' else 'assert(std::lang::toInt32(value).isNone());') if width == 32 else 'std::lang::toInt64(value);'
                    source.write_text('\n'.join(setup + [check, '}']), encoding='utf-8')
                    result = execute(source, width == 32 or label == 'negative-zero')
                    if width == 64 and label != 'negative-zero':
                        assert 'std::lang::toInt64：浮点值不是有限数或超出 int64 范围' in result.stderr
                    else:
                        assert not result.stderr, result.stderr


if __name__ == '__main__':
    main()
