"""注解与反射基础模块：默认开启、显式关闭、JSON 诊断与增量缓存区分。"""
from compiler_path import compiler_path
from pathlib import Path
import argparse
import json
import os
import subprocess
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
EXE = '.exe' if os.name == 'nt' else ''


def run(arguments, *, success=True, cwd=ROOT, timeout=1800):
    """执行编译或产物，失败时保留完整诊断。"""
    result = subprocess.run(list(map(str, arguments)), cwd=cwd, capture_output=True, text=True,
                            encoding='utf-8', errors='replace', timeout=timeout)
    assert (result.returncode == 0) == success, (arguments, result.returncode, result.stdout, result.stderr)
    return result


def main():
    """验证唯一的 AOT 工具链在开关两种状态下的诊断、产物与缓存行为。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    args = parser.parse_args()
    chali = args.chali.resolve()
    (ROOT / 'tmp').mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='chali-reflection-features-', dir=ROOT / 'tmp') as directory:
        work = Path(directory)
        source = work / 'main.chl'
        source.write_text('void main() { println("feature:42"); }\n', encoding='utf-8')
        annotated = work / 'annotated.chl'
        annotated.write_text('@annotation Label(text:String="ok")\n@Label\nvoid main(){ println("feature:42"); }\n',
                             encoding='utf-8')

        # 默认开启：不加开关也能用注解，AOT 产物照常运行。
        prefix = work / 'initial'
        run([chali, 'run', source, '--out', prefix])
        header = prefix.with_suffix('.quad').read_bytes()[:6]
        assert header[:5] == b'MGQI\x01' and header[5] & 2, header
        product = Path(str(work / 'annotated') + EXE)
        run([chali, 'aot', annotated, '--out', work / 'annotated'])
        assert run([product]).stdout == 'feature:42\n'

        # 显式关闭后注解不可用：文本诊断与 JSON 诊断都要说清楚是功能开关，而不是语法错误。
        rejected = run([chali, '--no-reflection', '--check', annotated], success=False)
        assert '功能未开启' in rejected.stderr, rejected.stderr
        document = json.loads(run([chali, '--no-reflection', '--diagnostics=json', '--check', annotated],
                                  success=False).stdout)
        assert document['schemaVersion'] == 1
        assert [item['code'] for item in document['diagnostics']] == ['CHL1200'], document
        accepted = json.loads(run([chali, '--reflection', '--diagnostics=json', '--check', annotated]).stdout)
        assert [item['code'] for item in accepted['diagnostics']] == [], accepted

        # 增量缓存把开关算进签名：开启后再关闭不能复用带反射表的产物。
        shared = work / 'shared.chl'
        shared.write_text('module shared;\nint64 seed() { return 1; }\n', encoding='utf-8')
        source.write_text('import shared;\nvoid main() { println("{}", shared::seed()); }\n', encoding='utf-8')
        incremental = work / 'incremental'
        for active in (False, True, False):
            command = [chali, 'aot', source, '--emit-llvm', '--no-reflection', '--incremental', '-I', work, '--out', incremental]
            if active:
                command[4] = '--reflection'
            run(command)
            data = incremental.with_suffix('.quad').read_bytes()
            assert bool(data[5] & 2) == active, (active, data[:8])
        print('[通过] 默认开启、显式关闭、JSON 诊断与增量缓存区分全部符合预期')


if __name__ == '__main__':
    main()
