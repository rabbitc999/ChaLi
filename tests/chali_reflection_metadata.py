"""验证类型化反射元数据随四元式导出：开关决定特征位与表段，AOT 继续可编译可运行。"""
from __future__ import annotations
from compiler_path import compiler_path
import argparse
import os
from pathlib import Path
import subprocess
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
EXE = '.exe' if os.name == 'nt' else ''


def varint(data, position):
    """读一个变长整数，返回 (值, 新位置)。"""
    value = 0
    shift = 0
    while True:
        byte = data[position]
        position += 1
        value |= (byte & 127) << shift
        if byte < 128:
            return value, position
        shift += 7


def sections(quad):
    """解析四元式头：返回 (特征位, 四个基础段的 (起点, 长度), 反射表 (起点, 长度))。"""
    assert quad[:5] == b'MGQI\x01', quad[:5]
    position = 5
    features, position = varint(quad, position)
    pools = []
    for _ in range(4):
        size, position = varint(quad, position)
        pools.append((position, size))
        position += size
    size, position = varint(quad, position)
    return features, pools, (position, size)


def main() -> None:
    """检查关闭反射不留表，开启反射写入非空表，且 AOT 产物行为一致。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    options = parser.parse_args()
    chali = options.chali.resolve()
    env = dict(os.environ, CHALI_LANG='zh', CHALI_NO_STDIN_INPUT='1')
    checks = 0

    def run(command, *, success: bool = True, cwd: Path = ROOT):
        nonlocal checks
        result = subprocess.run(list(map(str, command)), cwd=cwd, env=env, capture_output=True, text=True,
                                encoding='utf-8', errors='replace', timeout=600)
        assert (result.returncode == 0) == success, (command, result.stdout, result.stderr)
        checks += 1
        return result

    with tempfile.TemporaryDirectory(prefix='reflection-metadata-', dir=ROOT / 'tmp') as temporary:
        work = Path(temporary)
        plain = work / 'plain.chl'
        plain.write_text('void main(){}\n', encoding='utf-8')
        annotated = work / 'annotated.chl'
        annotated.write_text('@annotation Label(text:String="ok")\n@Label\nvoid main(){ println("reflection ok"); }\n',
                             encoding='utf-8')

        prefix = work / 'plain'
        run([chali, 'emit', plain, '--no-reflection', '--out', prefix])
        features, _, reflection = sections(prefix.with_suffix('.quad').read_bytes())
        assert not features & 2, features
        assert reflection[1] == 0, reflection

        run([chali, 'emit', plain, '--reflection', '--out', prefix])
        features, _, reflection = sections(prefix.with_suffix('.quad').read_bytes())
        assert features & 2, features
        assert reflection[1] > 0, reflection
        table = prefix.with_suffix('.quad').read_bytes()[reflection[0]:reflection[0] + reflection[1]]
        assert table[:4] == b'MCRF', table[:8]
        assert int.from_bytes(table[8:12], 'little') > 0, table[:16]

        # 反射表是编译期产物，AOT 编译与运行不受影响；注解参数仍按类型检查。
        product = Path(str(work / 'annotated') + EXE)
        run([chali, 'aot', annotated, '--reflection', '--out', work / 'annotated'])
        assert run([product]).stdout == 'reflection ok\n'
        bad = work / 'bad.chl'
        bad.write_text('@annotation Label(text:int32)\n@Label("wrong")\nvoid main(){}\n', encoding='utf-8')
        run([chali, 'emit', bad, '--reflection', '--out', work / 'bad'], success=False)
        print(f'反射元数据传输及导出：{checks} 项集成检查通过')


if __name__ == '__main__':
    main()
