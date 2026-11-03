"""验证实际宏生成声明的类型、字段、方法及源码来源。"""
from pathlib import Path
import argparse
import json
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile
from native_lsp_transport import Transport, position

ROOT = Path(__file__).resolve().parents[1]


def main():
    """使用用户 RECORD／GETTER 示例查询真实编译器语言服务。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    text = (ROOT / 'tests/fixtures/macro_generated_symbols.chl').read_text(encoding='utf-8')
    with tempfile.TemporaryDirectory(prefix='chali-generated-symbols-') as directory:
        folder = Path(directory)
        fixture = folder / 'metadata.chl'
        fixture.write_text(text, encoding='utf-8')
        analysis_path = folder / 'types.json'
        checked = subprocess.run([str(compiler), 'check', str(fixture), '--analysis-types', str(analysis_path), '--diagnostics=json'], capture_output=True, text=True, encoding='utf-8')
        assert checked.returncode == 0, (checked.stdout, checked.stderr)
        analysis = json.loads(analysis_path.read_text(encoding='utf-8'))
        declarations = analysis.get('generatedDeclarations', [])
        for owner, name, expected in [('', 'Box', 'struct Box'), ('', 'Label', 'struct Label'), ('Box', 'value', 'int64'), ('Label', 'value', 'String'), ('Box', 'create', 'int64'), ('Label', 'create', 'String'), ('Counter', 'read', 'int64')]:
            matches = [item for item in declarations if item['owner'] == owner and item['name'] == name]
            assert matches and expected in str(matches), (owner, name, declarations)
        uri = (folder / 'unsaved.chl').as_uri()
        server = Transport(compiler, directory)
        server.query('initialize', {'rootUri': folder.as_uri()})
        try:
            server.send({'method': 'textDocument/didOpen', 'params': {'textDocument': {'uri': uri, 'text': text, 'version': 1}}})
            def query(method, marker, advance=0):
                offset = text.rindex(marker) + advance
                return server.query('textDocument/' + method, {'textDocument': {'uri': uri}, 'position': position(text, offset)})
            for marker, expected in [('Box::create', 'Box'), ('Label::create', 'Label'), ('counter.read', 'int64 read'), ('box.value', 'int64 value'), ('label.value', 'String value')]:
                advance = marker.index('.') + 1 if '.' in marker else 0
                hover = query('hover', marker, advance)
                assert expected in str(hover), (marker, hover)
            for marker, advance, expected in [('Box::create', 5, 'create'), ('Label::create', 7, 'create'), ('counter.read', 8, 'read'), ('box.value', 4, 'value'), ('label.value', 6, 'value')]:
                items = query('completion', marker, advance)
                if isinstance(items, dict): items = items.get('items', [])
                assert any(item['label'] == expected for item in items), (marker, items)
            location = query('definition', 'counter.read', 8)
            assert location and uri in str(location), location
            for marker, advance, expected in [('Box::create', 0, 'Box'), ('Label::create', 0, 'Label'), ('counter.read', 8, 'read')]:
                definitions = query('definition', marker, advance)
                locations = definitions if isinstance(definitions, list) else [definitions]
                assert locations and locations[0]['uri'] == uri, definitions
                source_range = locations[0]['range']
                begin, end = source_range['start'], source_range['end']
                selected = text.splitlines()[begin['line']][begin['character']:end['character']]
                assert selected == expected, (expected, definitions, selected)
            for marker, expected in [('Box::create(42)', 'int64'), ('Label::create("ChaLi")', 'String')]:
                help_result = query('signatureHelp', marker, marker.index('(') + 1)
                assert expected in str(help_result), (marker, help_result)
            offset = text.rindex('counter.read') + 8
            server.serial += 1
            serial = server.serial
            server.send({'id': serial, 'method': 'textDocument/prepareRename', 'params': {'textDocument': {'uri': uri}, 'position': position(text, offset)}})
            rejected = server.response(serial)
            assert 'error' in rejected and '宏生成声明' in str(rejected), rejected
            symbols = server.query('textDocument/documentSymbol', {'textDocument': {'uri': uri}})
            names = {item['name'] for item in symbols}
            assert {'Box', 'Label', 'Counter', 'read', 'create', 'value'} <= names, symbols
            items = query('completion', 'Box::create', 0)
            if isinstance(items, dict): items = items.get('items', [])
            assert {'Box', 'Label'} <= {item['label'] for item in items}, items
            original = text
            version = 1
            for receiver, expected in [('box.', 'value'), ('label.', 'value'), ('counter.', 'read'), ('Box::', 'create'), ('Label::', 'create')]:
                version += 1
                text = original[:original.rindex('    assert(')] + '    ' + receiver
                server.send({'method': 'textDocument/didChange', 'params': {'textDocument': {'uri': uri, 'version': version}, 'contentChanges': [{'text': text}]}})
                items = server.query('textDocument/completion', {'textDocument': {'uri': uri}, 'position': position(text, len(text))})
                if isinstance(items, dict): items = items.get('items', [])
                assert any(item['label'] == expected for item in items), (receiver, items)
                assert not any('__chali_' in item['label'] for item in items), items
            version += 1
            text = original.replace('$GETTER(read, value);', '$GETTER(peek, value);').replace('counter.read()', 'counter.peek()')
            server.send({'method': 'textDocument/didChange', 'params': {'textDocument': {'uri': uri, 'version': version}, 'contentChanges': [{'text': text}]}})
            items = query('completion', 'counter.peek', 8)
            if isinstance(items, dict): items = items.get('items', [])
            labels = {item['label'] for item in items}
            assert 'peek' in labels and 'read' not in labels, items
            assert 'int64 peek' in str(query('hover', 'counter.peek', 8))
        finally:
            server.close()
    print('LSP generated declarations: unsaved RECORD Box/Label, concrete fields, static create, Counter.read and locations passed')


if __name__ == '__main__':
    main()
