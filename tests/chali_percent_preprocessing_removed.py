"""拒绝旧百分号指令及 -D，保留取余、字符串、注释、模块和源码行号。"""
from compiler_path import compiler_path
from project_temp import tempfile
from pathlib import Path
import argparse
import os
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def main():
    """核对文件、标准输入、导入模块及增量缓存均不接受百分号预处理。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    env = dict(os.environ, CHALI_LANG='zh', CHALI_QUIET='1')
    with tempfile.TemporaryDirectory(prefix='percent-removed-') as directory:
        work = Path(directory)
        source = work / 'main.chl'
        module = work / 'probe.chl'

        def invoke(arguments, text=None):
            """执行编译器并捕获诊断，所有产物放入本次临时目录。"""
            return subprocess.run([str(compiler), *map(str, arguments)], cwd=ROOT, env=env,
                                  input=text, capture_output=True, text=True, encoding='utf-8', timeout=180)

        directives = ('define VALUE 1', 'undefine VALUE', 'symbol VALUE', 'unsymbol VALUE',
                      'if 0', 'ifdef VALUE', 'ifndef VALUE', 'elif 1', 'else', 'endif',
                      'error removed', 'warning removed', 'include "probe.chl"', 'import "probe.chl"')
        for directive in directives:
            text = '// 首行占位\n%' + directive + '\nvoid main() {}\n'
            source.write_text(text, encoding='utf-8')
            result = invoke(['check', source])
            assert result.returncode != 0 and 'main.chl' in result.stderr, (directive, result)
            result = invoke(['check', '--stdin'], text)
            assert result.returncode != 0, (directive, result)

        source.write_text('void main() {}\n', encoding='utf-8')
        for arguments in (['-DVALUE=1'], ['-D', 'VALUE'], ['--define', 'VALUE=1']):
            result = invoke(['check', source, *arguments])
            assert result.returncode == 2 and '不认识的选项' in result.stderr, result
        assert '--define' not in invoke(['--help']).stdout

        source.write_text('import probe;\nvoid main() { assert(probe::value()==2); }\n', encoding='utf-8')
        module.write_text('module probe;\nint64 value() { return 8 % 3; }\n', encoding='utf-8')
        arguments = ['check', source, '--incremental', '--cache-dir', work / 'cache']
        assert invoke(arguments).returncode == 0
        assert invoke(arguments).returncode == 0
        module.write_text('module probe;\n// 保留第三行诊断\n%if 0\nint64 value() { return 99; }\n%endif\n', encoding='utf-8')
        result = invoke(arguments)
        assert result.returncode != 0 and 'probe.chl' in result.stderr, result
        assert '第 3 行' in result.stderr or ':3:' in result.stderr, result.stderr

        source.write_text('''// %error 不能执行
/* %if 0 */
$macro VALUE()
8 % 3
$end_macro
void main() {
    assert($VALUE() == 2);
    String literal = "%define %if %warning";
    assert(literal.length() > 0);
    println("percent literals and macros OK");
}
''', encoding='utf-8')
        result = invoke(['run', source, '--out', work / 'valid'])
        assert result.returncode == 0 and result.stdout == 'percent literals and macros OK\n', result
    print('percent preprocessing removed: 14 directives, CLI, stdin, imports, cache, locations and literals OK')


if __name__ == '__main__':
    main()
