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
        finally:
            server.close()
    print('原生 LSP 类型别名展开、跳转、悬停与未完成源码补全通过。')


if __name__ == '__main__':
    main()
