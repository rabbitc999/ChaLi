"""验证插值源码中的原生语义标记及定义链接。"""
from pathlib import Path

from compiler_path import compiler_path
from native_lsp_transport import Transport, position
from project_temp import tempfile


def main():
    """覆盖局部遮蔽、跨模块调用、原始 UTF-16 范围和嵌套插值。"""
    with tempfile.TemporaryDirectory(prefix='chali-lsp-interpolation-') as directory:
        root = Path(directory)
        dependency = 'module ast;\nString dumpNode(int64 expr, int64 pool) { return "node"; }\n'
        target = root / 'ast.chl'
        target.write_text(dependency, encoding='utf-8')
        text = (
            'import ast;\n'
            'void main() {\n'
            '    String name = "outer";\n'
            '    int64 expr = 1, pool = 2;\n'
            '    {\n'
            '        String name = "inner";\n'
            '        String text = f"😀 let {name} = {dumpNode(expr, pool)}";\n'
            '        String nested = f"escaped \\{fake\\} {f\"inner {ast::dumpNode(\n'
            '            expr, /* ignored } */ pool)}\"}";\n'
            '        String quoted = f"{dumpNode(\n'
            '            { int64 local = expr; yield local; }, pool)}";\n'
            '        String literal = "name dumpNode";\n'
            '    }\n'
            '}\n'
        )
        uri = (root / 'main.chl').as_uri()
        server = Transport(compiler_path(), root)
        try:
            init = server.query('initialize', dict(rootUri=root.as_uri(),
                capabilities=dict(textDocument=dict(definition=dict(linkSupport=True)))))
            legend = init['capabilities']['semanticTokensProvider']['legend']['tokenTypes']
            server.send(dict(method='textDocument/didOpen', params=dict(textDocument=dict(uri=uri, text=text, version=1))))
            document = dict(textDocument=dict(uri=uri))
            data = server.query('textDocument/semanticTokens/full', document)['data']
            line = column = end = 0
            tokens = {}
            for i in range(0, len(data), 5):
                dl, dc, length, kind, _ = data[i:i + 5]
                line += dl
                column = dc if dl else column + dc
                assert dl or column >= end, (line, column, end)
                end = column + length
                tokens[line, column] = legend[kind], length

            def locate(needle, symbol):
                return text.index(needle) + needle.index(symbol)

            def check(needle, symbol, kind, destination, source, offset):
                start = locate(needle, symbol)
                origin = position(text, start)
                assert tokens.get((origin['line'], origin['character'])) == (kind, len(symbol)), (needle, tokens.get((origin['line'], origin['character'])))
                links = server.query('textDocument/definition', dict(**document, position=position(text, start + 1)))
                assert isinstance(links, list) and len(links) == 1, (needle, links)
                link = links[0]
                assert link['targetUri'] == destination and link['targetSelectionRange'] == dict(
                    start=position(source, offset), end=position(source, offset + len(symbol))), link
                assert link['originSelectionRange'] == dict(start=origin, end=position(text, start + len(symbol))), link

            check('{name}', 'name', 'variable', uri, text, text.index('name = "inner"'))
            check('{dumpNode(expr', 'dumpNode', 'function', target.as_uri(), dependency, dependency.index('dumpNode'))
            check('dumpNode(expr, pool)', 'expr', 'variable', uri, text, text.index('expr = 1'))
            check('expr, pool)', 'pool', 'variable', uri, text, text.index('pool = 2'))
            check('ast::dumpNode(', 'dumpNode', 'function', target.as_uri(), dependency, dependency.index('dumpNode'))
            check('yield local', 'local', 'variable', uri, text, text.index('local = expr'))
            for fake in ('fake', 'name dumpNode', 'ignored }'):
                assert not server.query('textDocument/definition', dict(**document, position=position(text, text.index(fake) + 1))), fake
            references = server.query('textDocument/references', dict(**document,
                position=position(text, text.index('name = "inner"')), context=dict(includeDeclaration=False)))
            assert any(item['range']['start'] == position(text, locate('{name}', 'name')) for item in references), references

            incomplete = text.replace('{name} = {dumpNode(expr, pool)}";', '{name} = {dumpNode(expr,')
            server.send(dict(method='textDocument/didChange', params=dict(textDocument=dict(uri=uri, version=2),
                contentChanges=[dict(text=incomplete)])))
            at = position(incomplete, incomplete.index('dumpNode(expr') + 1)
            assert server.query('textDocument/definition', dict(**document, position=at))[0]['targetUri'] == target.as_uri()
        finally:
            server.close()
    print('原生 LSP 插值高亮、UTF-16 定义链接、遮蔽、跨模块及未完成输入通过。')


if __name__ == '__main__':
    main()
