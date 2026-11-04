"""验证具名循环退出、作用域清理与宏卫生。"""
import argparse
import json
import os
from pathlib import Path
import subprocess

from compiler_path import compiler_path
from project_temp import tempfile


def main():
    """实际运行正例并核对非法标签的诊断。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    environment = dict(os.environ, CHALI_LANG='zh', CHALI_QUIET='1')
    program = '''int64 trace = 0;
void record(int64 digit) { trace = trace * 10 + digit; }

$macro WITH_LOOP(body:block) -> stmt
inner: while (true) {
    body;
    break inner;
}
$end_macro

void main() {
    int64 key = 7;
    Map<int64, int64> map = {key: 9};
    assert(map[key] == 9);
    outer: for (item in range(0, 2)) {
        inner: while (true) {
            defer record(2);
            loop {
                scope(exit) { record(1); }
                break inner;
            };
            assert(false);
        }
        record(3);
        break outer;
    }
    assert(trace == 123);
    $WITH_LOOP({ record(4); });
    inner: while (true) {
        $WITH_LOOP({ break inner; });
        assert(false);
    }
    last: loop { break last; }
    all: while (true) {
        loop { break all; };
        assert(false);
    }
    println("{}", trace);
}
'''
    cases = (
        ('run', program, '1234\n'),
        ('check', 'void main(){while(true){break 7;}}', 'CHL1001'),
        ('check', 'void main(){while(true){break #level(2);}}', 'CHL1001'),
        ('check', 'void main(){1: while(true){break;}}', '循环标签必须是标识符'),
        ('check', 'void main(){while(true){break absent;}}', '找不到可退出的循环标签：absent'),
        ('check', 'void main(){same: while(true){same: while(true){break same;}}}', '活动循环标签重复：same'),
        ('check', 'void main(){gone: while(true){break gone;} break gone;}', '找不到可退出的循环标签：gone'),
        ('check', 'void main(){while(true){break all;}}', '找不到可退出的循环标签：all'),
        ('check', 'void main(){while(true){break level 2;}}', 'CHL1001'),
        ('check', 'void main(){while(true){break level all;}}', 'CHL1001'),
        ('check', 'void main(){outer: while(true){let f=fn(){break outer;};break outer;}}', '找不到可退出的循环标签：outer'),
        ('check', '$macro STOP() -> stmt\nbreak outer;\n$end_macro\nvoid main(){outer: while(true){$STOP();}}',
         '找不到可退出的循环标签：outer'),
    )
    with tempfile.TemporaryDirectory(prefix='chali-loop-labels-') as directory:
        work = Path(directory)
        for index, (command, code, expected) in enumerate(cases):
            source = work / f'case-{index}.chl'
            source.write_text(code, encoding='utf-8')
            arguments = [str(compiler), command, str(source)]
            if command == 'run':
                arguments += ['--out', str(work / f'case-{index}')]
            elif expected == 'CHL1001':
                arguments.append('json')
            result = subprocess.run(arguments, cwd=work, env=environment,
                                    capture_output=True, encoding='utf-8', timeout=180)
            if command == 'run':
                assert (result.returncode, result.stdout, result.stderr) == (0, expected, ''), result
            elif expected == "CHL1001":
                diagnostics = json.loads(result.stdout)["diagnostics"]
                assert result.returncode != 0 and any(item["code"] == expected for item in diagnostics), result
            else:
                assert result.returncode != 0 and expected in result.stderr, result
    print('named loop exits, cleanup, duplicate labels and macro hygiene passed')


if __name__ == '__main__':
    main()
