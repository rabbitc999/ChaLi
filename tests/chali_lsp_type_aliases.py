"""验证类型别名的真实 LSP 展开、跳转、悬停及成员补全。"""
from pathlib import Path

from chali_native_lsp_parity import NativeServer
from compiler_path import compiler_path
from native_lsp_transport import position
from project_temp import tempfile


def main():
    """通过模块的未保存修改验证别名的定义与使用位置。"""
    with tempfile.TemporaryDirectory(prefix='chali-lsp-type-aliases-') as directory:
        root = Path(directory)
        server = NativeServer(compiler_path())
        try:
            server.query('initialize', dict(rootUri=root.as_uri()))
            uri = (root / 'main.chl').as_uri()
            (root / 'dep.chl').write_text('module dep;\n', encoding='utf-8')
            dep = (root / 'dep.chl').as_uri()
            alias_source = 'module dep;\nstruct Cell<T> { T value; };\ntype alias Box<T = bool> = Cell<T>;'
            server.documents[dep] = dict(text=alias_source)
            text = 'import dep;\nvoid main() { Box plain; dep::Box<int64> explicit; plain.value = true; explicit.value = 7; }'
            server.documents[uri] = dict(text=text)
            def at(marker, extra=0):
                return dict(textDocument=dict(uri=uri), position=position(text, text.index(marker) + extra))
            expanded = server.query('chali/expandTypeAlias', at('Box plain', 1))
            assert expanded and expanded['spelling'] == 'Box' and 'Cell<bool>' in expanded['type'], expanded
            expanded = server.query('chali/expandTypeAlias', at('dep::Box<int64>', 10))
            assert expanded and expanded['spelling'] == 'dep::Box<int64>' and 'Cell<int64>' in expanded['type'], expanded
            hover = server.query('textDocument/hover', at('Box plain', 1))
            assert 'type alias Box' in hover['contents']['value'] and 'Cell<bool>' in hover['contents']['value'], hover
            definitions = server.query('textDocument/definition', at('Box plain', 1))
            assert definitions and all(item.get('uri', item.get('targetUri')) == dep for item in definitions), definitions
            text = 'import dep;\nvoid main() { Box plain; plain.'
            server.documents[uri] = dict(text=text)
            completions = server.query('textDocument/completion', dict(textDocument=dict(uri=uri), position=position(text, len(text))))
            assert 'value' in {item['label'] for item in completions}, completions
            errors = (root / 'errors.chl').as_uri()
            errors_source = 'module errors;\ntype alias TextResult<T> = Result<T, String>;'
            server.documents[errors] = dict(text=errors_source)
            bridge = (root / 'bridge.chl').as_uri()
            server.documents[bridge] = dict(text='module bridge;\nimport errors;\ntype alias Public<T> = errors::TextResult<T>;')
            declaration = dict(textDocument=dict(uri=errors), position=position(errors_source, errors_source.index('TextResult') + 2))
            symbols = server.query('textDocument/documentSymbol', dict(textDocument=dict(uri=errors)))
            assert 'TextResult' in {item['name'] for item in symbols}, symbols
            expanded = server.query('chali/expandTypeAlias', declaration)
            assert expanded and expanded['mode'] == 'declaration' and expanded['type'] == 'Result<T, String>', expanded

            text = ('import bridge;\nimport errors;\nvoid main() {\n'
                    '    errors::TextResult<int64> qualified = Ok(1);\n'
                    '    Public<int64> shortName = qualified;\n'
                    '    StdResult<int64> standard = Ok(2);\n'
                    '    std::core::StdResult<int64> core = standard;\n'
                    '    errors::TextResult<int64> failed = errors::TextResult<int64>::Fail("missing");\n'
                    '}')
            server.documents[uri] = dict(text=text)
            for marker, error_type in [('TextResult<int64> qualified', 'String'), ('Public<int64>', 'String'),
                                       ('StdResult<int64> standard', 'Error'), ('StdResult<int64> core', 'Error')]:
                expanded = server.query('chali/expandTypeAlias', at(marker, 2))
                assert expanded and expanded['mode'] == 'resolved', (marker, expanded)
                assert 'Result<int64' in expanded['type'] and error_type in expanded['type'], (marker, expanded)
            hover = server.query('textDocument/hover', at('TextResult<int64> qualified', 2))
            assert 'type alias TextResult' in hover['contents']['value'] and 'Result<int64' in hover['contents']['value'], hover
            definitions = server.query('textDocument/definition', at('TextResult<int64> qualified', 2))
            assert definitions and all(item.get('uri', item.get('targetUri')) == errors for item in definitions), definitions
            for marker in ['StdResult<int64> standard', '::Fail']:
                definitions = server.query('textDocument/definition', at(marker, 3))
                assert definitions and any(item.get('uri', item.get('targetUri')) == 'chali-stdlib:/std/core.chl' for item in definitions), (marker, definitions)

            for alias in ['errors::TextResult<int64>', 'StdResult<int64>']:
                text = f'import errors;\nvoid main() {{ {alias} value; {alias}::'
                server.documents[uri] = dict(text=text)
                completions = server.query('textDocument/completion', dict(textDocument=dict(uri=uri), position=position(text, len(text))))
                assert {'Ok', 'Fail'} <= {item['label'] for item in completions}, (alias, completions)
                text = 'import missing;\n' + text
                server.documents[uri] = dict(text=text)
                expanded = server.query('chali/expandTypeAlias', at(alias, alias.index('int64')))
                assert expanded and 'Result<int64' in expanded['type'] and expanded['mode'] == 'provisional', (alias, expanded)

            text = 'import bridge;\nvoid main() { Public<int64> value = Ok(1); }'
            server.documents[uri] = dict(text=text)
            expanded = server.query('chali/expandTypeAlias', at('Public<int64>', 2))
            assert expanded and expanded['mode'] == 'resolved' and 'String' in expanded['type'], expanded
        finally:
            server.close()
    print('原生 LSP 类型别名展开、跳转、悬停与未完成源码补全通过。')


if __name__ == '__main__':
    main()
