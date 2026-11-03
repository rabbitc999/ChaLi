"""验证模块头同行后缀、字面量隔离、工程抽取及原生包诊断。"""
from pathlib import Path
import argparse
import json
import os
from compiler_path import compiler_path
from project_temp import tempfile
from chali_library_package import invoke


def main():
    """在真实库包上核对普通头部语句边界，不靠分行改变消费语义。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    with tempfile.TemporaryDirectory(prefix='library-inline-import-') as directory:
        work = Path(directory)
        modules = work / 'modules/demo'
        modules.mkdir(parents=True)
        provider = work / 'provider.chl'
        package = modules / 'inline_import.chlib'
        provider.write_text('module demo::inline_import;struct Message {String text;};'
                            'int64 answer(){return 42;}'
                            'Message make(){Message value;value.text="loaded";return value;}', encoding='utf-8')
        invoke([compiler, 'lib', 'build', provider, '--module', 'demo::inline_import',
                '--package-version', '1.0.0', '--checked', '--out', package], work)
        invoke([compiler, 'lib', 'inspect', package], work)
        provider.write_text('module demo::second;int64 value(){return 2;}', encoding='utf-8')
        invoke([compiler, 'lib', 'build', provider, '--module', 'demo::second',
                '--package-version', '1.0.0', '--checked', '--out', modules / 'second.chlib'], work)
        provider.unlink()
        (modules / 'source.chl').write_text('module demo::source;import demo::second;int64 value(){return demo::second::value();}', encoding='utf-8')
        suffix = ('void main(){demo::inline_import::Message value=demo::inline_import::make();'
                  'assert(value.text=="loaded");assert(demo::inline_import::answer()==42);println("inline imports OK");}')
        consumer = work / 'main.chl'
        sources = [
            'import demo::inline_import;' + suffix,
            'module app;import demo::inline_import;import demo::second;' + suffix,
            '/* import absent; */ module /* gap */ app; import demo /* gap */ :: inline_import;' + suffix,
            'module\n app;import\n demo::inline_import;import demo::second;\r\n' + suffix,
            'import demo::inline_import; /* opening\nimport absent;\n*/ import demo::second;' + suffix,
            'module app;import demo::inline_import;' + suffix.replace('println("inline imports OK");',
                'String fake="/*";String quoted="import absent;";char slash=\'/\';'
                'assert(f"{str("import missing; //")}"=="import missing; //");println("inline imports OK");'),
        ]
        for index, source in enumerate(sources):
            consumer.write_bytes(source.encode('utf-8'))
            invoke([compiler, 'check', consumer], work)
            assert invoke([compiler, 'run', consumer, '--out', work / ('case-' + str(index))], work).stdout == 'inline imports OK\n'
        consumer.write_text('import demo::source;void main(){assert(demo::source::value()==2);}', encoding='utf-8')
        invoke([compiler, 'run', consumer, '-I', work / 'modules', '--out', work / 'transitive'], work)
        for source, message in [
            ('import demo::missing;void main(){}', 'demo::missing'),
            ('void main(){} import demo::inline_import;', 'import 必须写在模块文件头部'),
            ('module one;module two;void main(){}', '同一文件只能声明一次 module'),
            ('void main(){} module late;', 'module 声明必须是文件开头'),
            ('module std::forbidden;import demo::inline_import;void main(){}', '保留给标准库'),
            ('String fake="/*";\nimport demo::inline_import;void main(){}', 'import 必须写在模块文件头部'),
        ]:
            consumer.write_text(source, encoding='utf-8')
            rejected = invoke([compiler, 'check', consumer], work, False)
            assert message in rejected.stderr, rejected.stderr
        original = package.read_bytes()
        damaged = bytearray(original)
        damaged[-1] ^= 1
        package.write_bytes(damaged)
        consumer.write_text('import demo::inline_import;void main(){}', encoding='utf-8')
        rejected = invoke([compiler, 'check', consumer], work, False)
        assert '库条目 SHA-256 不匹配' in rejected.stderr and 'Undeclared' not in rejected.stderr, rejected.stderr
        package.write_bytes(original)
        manifest = work / 'chali.toml'
        manifest.write_text('[package]\nname="inline-project"\nversion="0.1.0"\n'
                            '[chali]\nimport_paths=["modules"]\nimport=["demo::source"]\n', encoding='utf-8')
        source = '/* leading */ module app;import demo::inline_import;import demo::second;' + suffix
        consumer.write_text(source, encoding='utf-8')
        assert invoke([compiler, 'run', consumer, '--out', work / 'project'], work).stdout == 'inline imports OK\n'
        dependencies = invoke([compiler, '--deps', consumer], work).stdout.replace('\\', '/')
        assert package.as_posix() in dependencies and (modules / 'source.chl').as_posix() in dependencies, dependencies
        output = work / 'unchecked'
        invoke([compiler, 'aot', consumer, '--out', output], work)
        executable = output.with_suffix('.exe') if os.name == 'nt' else output
        assert invoke([executable], work).stdout == 'inline imports OK\n'
        body = 'void main(){ missingIdentifier=1;}'
        leading = '\t/* 中文列占位 */ '
        consumer.write_text(leading + body, encoding='utf-8')
        control = invoke([compiler, 'check', consumer, '--diagnostics=json'], work, False)
        control_diagnostics = json.loads(control.stdout)['diagnostics']
        baseline = next(item for item in control_diagnostics if 'missingIdentifier' in item['message'])
        prefix = 'module app;import demo::inline_import;'
        source = leading + prefix + body
        consumer.write_text(source, encoding='utf-8')
        rejected = invoke([compiler, 'check', consumer, '--diagnostics=json'], work, False)
        diagnostics = json.loads(rejected.stdout)['diagnostics']
        assert diagnostics, (rejected.stdout, rejected.stderr)
        expected = baseline['span']['column'] + len(prefix)
        assert any(item.get('span', {}).get('line') == 1 and item.get('span', {}).get('column') == expected
                   and item.get('source') == source and Path(item['span']['file']).resolve() == consumer.resolve()
                   for item in diagnostics), diagnostics
    print('inline native imports: statement boundaries, comments/strings, projects, dependency errors and source columns OK')


if __name__ == '__main__':
    main()
