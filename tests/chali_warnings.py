"""警告配置、单次语义检查、原始来源、局部抑制和保留副作用的修复。"""
from pathlib import Path
import argparse
import json
import os
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile


def main():
    """在独立项目验证 CLI、TOML、JSON 及未保存源码的相同行为。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    with tempfile.TemporaryDirectory(prefix='warnings-') as directory:
        work = Path(directory)
        source = work / 'main.chl'
        config = work / 'chali.toml'
        text = 'void main(){\n int64 ignored=7;\n int64 used=2;\n assert(used==2);\n}\n'
        source.write_text(text, encoding='utf-8')

        def check(*flags, expected=0, overlay=None):
            command = [str(compiler), '--diagnostics=json', *flags, str(source)]
            if overlay is not None:
                command.insert(1, '--source-stdin')
            result = subprocess.run(command, cwd=work, input=overlay, capture_output=True,
                                    encoding='utf-8', timeout=90)
            assert result.returncode == expected, (command, result.stdout, result.stderr)
            report = json.loads(result.stdout)
            return report['diagnostics']

        assert not check()
        warnings = check('--warn=unused-local')
        unused = [d for d in warnings if d['code'] == 'CHL2201']
        assert len(unused) == 1, warnings
        diagnostic = unused[0]
        assert diagnostic['severity'] == 'warning' and diagnostic['span']['line'] == 2, diagnostic
        assert diagnostic['source'] == text.splitlines()[1], diagnostic
        assert Path(diagnostic['span']['file']).resolve() == source.resolve(), diagnostic
        fix = diagnostic['fixes'][0]
        assert fix['line'] == fix['endLine'] == 2 and fix['column'] == fix['endColumn'] == 1, fix
        lines = text.splitlines(keepends=True)
        lines.insert(fix['line'] - 1, fix['newText'])
        assert not check('--deny=unused-local', overlay=''.join(lines))
        assert check('--deny=unused-local', expected=1)[0]['severity'] == 'error'
        assert check('--warn=unused-local', '--warnings-as-errors', expected=1)[0]['severity'] == 'error'
        assert not check('--warn=all', '--allow=unused-local')
        config.write_text('[warnings]\nunused-local="error"\n', encoding='utf-8')
        assert check(expected=1)[0]['code'] == 'CHL2201'
        assert not check('--allow=CHL2201')
        config.unlink()
        # 字符串中的伪指令不能抑制下一行；宏参数副本不产生复制警告。
        fake = 'void main(){\n println("// chali: allow-next-line CHL2201");\n int64 ignored=1;\n}\n'
        assert check('--warn=unused-local', overlay=fake)[0]['span']['line'] == 3
        inline = text.replace('int64 ignored=7;', 'int64 ignored=7; // chali: allow CHL2201')
        assert not check('--deny=unused-local', overlay=inline)
        named = 'struct bad_name { int64 value; };\nvoid BadFunction(){return;}\nvoid main(){BadFunction();}\n'
        naming = check('--warn=naming', overlay=named)
        assert {d['code'] for d in naming} == {'CHL2101', 'CHL2102'}, naming
        assert not check('--warning-profile=editor', '--allow=naming', overlay=named)
        for expression in ('int64 x=1; int64 y=x; assert(y==1);',
                           'int64 x=1; int64 y=0; y=x; assert(y==1);',
                           'int64 x=1; int64 mut& p=&x; assert(*p==1);'):
            assert not check('--deny=unused-local', overlay='void main(){'+expression+'}\n')
        config.write_text('[warnings]\nall="error"\nCHL2201="allow"\n', encoding='utf-8')
        assert not check()
        config.unlink()
        for bad in ('--warn=not-a-rule', '--deny=CHL9999'):
            result = subprocess.run([str(compiler), '--check', bad, str(source)], cwd=work,
                                    capture_output=True, encoding='utf-8', timeout=90)
            assert result.returncode == 2 and '未知警告规则' in result.stderr, result
        config.write_text('[warnings]\nunused-local="maybe"\n', encoding='utf-8')
        assert check(expected=1)[0]['code'] == 'CHL1101'
        config.unlink()
        # 错误提升必须阻止运行，不得执行初始化副作用。
        source.write_text('int64 next(){println("side effect");return 1;}\nvoid main(){int64 ignored=next();}\n', encoding='utf-8')
        result = subprocess.run([str(compiler), 'run', '--deny=unused-local', str(source)], cwd=work,
                                capture_output=True, encoding='utf-8', timeout=90)
        assert result.returncode == 1 and not result.stdout, result
    print('warnings: stable rules, precedence, promotion, suppression, safe fix and unsaved source passed')


if __name__ == '__main__':
    main()
