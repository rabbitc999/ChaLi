"""真实 CHALI 声明到元数据：布局、注解、稳定身份与容器。"""
from __future__ import annotations
from compiler_path import compiler_path
import argparse
import struct
from pathlib import Path
import subprocess
from project_temp import tempfile
ROOT = Path(__file__).resolve().parents[1]


def main():
    """验证前端限制，以及四元式里反射表记录的真实声明身份。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    args = parser.parse_args()
    checks = 0

    def run(command, success=True, cwd=ROOT):
        nonlocal checks
        result = subprocess.run(list(map(str, command)), cwd=cwd, capture_output=True, text=True,
                                encoding='utf-8', errors='replace', timeout=240)
        assert (result.returncode == 0) == success, (command, result.stdout, result.stderr)
        checks += 1
        return result

    with tempfile.TemporaryDirectory(prefix='reflection-declarations-', dir=ROOT / 'build') as temporary:
        work = Path(temporary)
        sample = ROOT / 'example/reflection/declarations.chl'
        # 反射表随四元式导出，AOT 编译只读同一份前端产物。
        run([args.chali, 'aot', '--reflection', '--emit-llvm', sample, '--out', work / 'program'])
        source = work / 'case.chl'

        def check(text, error=None, enabled=True):
            source.write_text(text, encoding='utf-8')
            result = run([args.chali, *(['--reflection'] if enabled else ['--no-reflection']), '--check', source],
                         success=error is None)
            if error:
                assert error in result.stderr, result.stderr

        check('@Reflect(1) int64 f(){return 1;} void main(){}', '不接受参数')
        check('@Reflect struct S{@Reflect int64 x;}; void main(){}', '成员随类型暴露')
        check('@Reflect void main(){}', '功能未开启', enabled=False)
        check('@std::reflect::Reflect void main(){}')
        check('@annotation Reflect(x:int32)\n@Reflect(1) void main(){}')
        check('struct Box<T>{T value;};\n@Reflect int64 read(Box<int64> x){return x.value;}\nvoid main(){return;}')
        (work / 'chali.toml').write_text('[package]\nname="reflection"\nversion="0.1.0"\n[chali]\nimport_paths=["."]\n',
                                         encoding='utf-8')
        for module in ('one', 'two'):
            (work / (module + '.chl')).write_text('module ' + module + ';\n@Reflect struct Same{int64 value;};\n',
                                                  encoding='utf-8')
        source.write_text('import one;\nimport two;\nvoid main(){return;}\n', encoding='utf-8')
        run([args.chali, 'aot', '--reflection', '--emit-llvm', source, '--out', work / 'names'], cwd=work)

        def identities(prefix):
            quad = prefix.with_suffix('.quad').read_bytes()
            assert quad[:5] == b'MGQI\x01'
            position = 5

            def varint():
                nonlocal position
                value = 0
                shift = 0
                while True:
                    byte = quad[position]
                    position += 1
                    value |= (byte & 127) << shift
                    if byte < 128:
                        return value
                    shift += 7

            assert varint() & 2
            for _ in range(4):
                size = varint()
                position += size
            size = varint()
            data = quad[position:position + size]
            count = struct.unpack_from('<I', data, 8)[0]
            offset = 12
            result = {}
            for _ in range(count):
                identity, size = struct.unpack_from('<QI', data, offset)
                offset += 12
                result[data[offset:offset + size].decode('utf-8')] = identity
                offset += size
            return result

        names = identities(work / 'names')
        assert names['struct:one::Same'] != names['struct:two::Same']
        snapshots = []
        for index, alias in enumerate(('String', 'std::lang::String')):
            extra = 'struct Unrelated{int32 x;};\n' if index else ''
            source.write_text(extra + 'struct Box<T>{T value;};\n@Reflect String read(Box<' + alias +
                              '> value){return value.value;}\nvoid main(){return;}\n', encoding='utf-8')
            prefix = work / ('identity' + str(index))
            run([args.chali, 'aot', '--reflection', '--emit-llvm', source, '--out', prefix])
            snapshots.append(identities(prefix))
        assert snapshots[0] == snapshots[1], snapshots
        assert 'struct:program::Box<std::lang::String>' in snapshots[0], snapshots
        assert not any(('__string' in key for key in snapshots[0])), snapshots
        print(f'CHALI 声明元数据：{checks} 项集成检查通过')


if __name__ == '__main__':
    main()
