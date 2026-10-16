"""验证块表达式、变长列表宏及插值输出在解释与 AOT 后端的一致性。"""
import argparse
import os
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    """运行成功样例及缺少出口、错误类型、非法引用等诊断检查。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--magel', type=Path, default=ROOT / 'magel.exe')
    parser.add_argument('--front-end-only', action='store_true')
    options = parser.parse_args()
    compiler = options.magel.resolve()
    env = dict(os.environ, MAGEL_LANG='zh', MAGEL_NO_STDIN_INPUT='1')
    with tempfile.TemporaryDirectory(prefix='magel-block-') as directory:
        work = Path(directory)

        def invoke(command):
            """执行编译或测试程序并收集输出。"""
            return subprocess.run(list(map(str, command)), cwd=ROOT, env=env,
                                  capture_output=True, text=True, encoding='utf-8',
                                  errors='replace', timeout=240)

        examples = {
            'block_expression': 'block expression OK\n',
            'break_values': 'break values OK\n',
            'print_interpolation': 'value=3;next=1 2;literal={x};percent=100%\nnested=value 3\nsaved=3\ninterpolation print OK\n',
        }
        for name, expected in examples.items():
            for backend in ('run',) if options.front_end_only else ('run', 'aot'):
                for level in (0, 2):
                    output = work / (name + backend + str(level) + '.exe')
                    result = invoke([compiler, backend, ROOT / 'example/samples' / (name + '.mgl'),
                                     '--out', output, f'--opt-level={level}'])
                    assert result.returncode == 0, (name, backend, result.stdout, result.stderr)
                    if backend == 'aot':
                        result = invoke([output])
                    assert (result.returncode, result.stdout, result.stderr) == (0, expected, ''), result
                    print(f'[通过] {name} {backend} O{level}', flush=True)
        for body in (
            'int32 x = { int32 y = 3; };',
            'int32 x = { if (true) { break 3; } };',
            'int32 x = { break; };',
            'int32 x = { break "wrong"; };',
            'int32 x = { int32 local = 1; break local; }; local = 2;',
            'int32 const& x = { int32 local = 1; break &local; };',
            'while (true) { break 1; }',
            'int64 x = { loop { break; }; };',
            'int64 x = { while (false) { break 1; } };',
            '(int64,String) x = { loop { break 1,2; }; };',
            '(int64,int64) x = { loop { break 1,2,3; }; };',
            'int64 x = { loop { auto f = int64 fn(){break 1;}; break 2; }; };',
            'auto f = int64 fn(){ (if(true){loop{break 1;};}else{2}); };f();',
            'int64 const& x = { loop { int64 local = 1; break &local; }; };',
            'int64 const& x = { for (int64 local=0;local<1;local++) { break &local; } int64 local=2;break &local; };',
            'println(f"value={1}", 2);',
        ):
            source = work / 'invalid.mgl'
            source.write_text('int64 main(){' + body + 'return 0;}', encoding='utf-8')
            result = invoke([compiler, '--check', source])
            assert result.returncode != 0, (body, result.stdout, result.stderr)
            print(f'[通过] 拒绝 {body}', flush=True)


if __name__ == '__main__':
    main()
