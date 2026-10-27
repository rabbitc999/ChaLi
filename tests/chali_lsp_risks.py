"""通过真实 LSP 检查闭包作用域、返回冲突及编辑后的类型一致性。"""
from compiler_path import compiler_path
from pathlib import Path
from project_temp import tempfile
from native_lsp_transport import Transport, position
import re


def main():
    """只发协议请求，同时核对悬停和补全，不调用 run 或 AOT。"""
    cases = [
        ('int32 seed = 1; let target = fn() { seed };', 'int32 fn()'),
        ('int32 seed = 1; { bool seed = true; let target = fn() { seed }; target; }', 'bool fn()'),
        ('int32 seed = 1; let first = fn() { seed }; { bool seed = true; let target = fn() { seed }; target; }', 'bool fn()'),
        ('int32 seed = 1; let target = fn(bool seed) { seed };', 'bool fn(bool)'),
        ('let target = fn(int32 x) { let local = x; local };', 'int32 fn(int32)'),
        ('let target = fn(int32 x) { let local = fn(bool x) { x }; local };', '(bool fn(bool)) fn(int32)'),
        ('let original = fn(int32 x) { x }; let target = original;', 'int32 fn(int32)'),
        ('let original = fn(int32 x) { x }; let target = original(1);', 'int32'),
        ('let target = fn(bool b) { if (b) { return 1; } return true; };', None),
        ('let target = fn(bool b) { if (b) { return 1; } return missing; };', None),
        ('let target = if (true) { 1 } else { missing };', None),
        ('let target = if (true) { 1 } else {};', None),
        ('let target = fn() { if (true) { 1 } else { missing } };', None),
        ('let target = fn() { return fn(int32 x) { return x; }; };', '(int32 fn(int32)) fn()'),
        ('let target = int64 fn(int64) fn(int64 fn(int64) a, int64 fn(int64) b) { a };', '(int64 fn(int64)) fn(int64 fn(int64), int64 fn(int64))'),
        ('let target = (int64 fn(int64)) fn(int64 fn(int64) a, int64 fn(int64) b) { a };', '(int64 fn(int64)) fn(int64 fn(int64), int64 fn(int64))'),
        ('let target = int64 fn(int64) fn(int64 fn(int64) a, int64 fn(iny64) b) { a };', '(int64 fn(int64)) fn(int64 fn(int64), int64 fn(iny64))'),
    ]
    with tempfile.TemporaryDirectory(prefix='chali-lsp-risks-') as folder:
        server = Transport(compiler_path(), folder)
        server.query('initialize', dict(rootUri=Path(folder).as_uri()))
        try:
            uri = (Path(folder) / 'risks.chl').as_uri()
            for version, (body, expected) in enumerate(cases, 1):
                text = 'void main() { ' + body + ' target; }'
                if version == 1:
                    server.send(dict(method='textDocument/didOpen', params=dict(textDocument=dict(uri=uri, version=version, text=text))))
                else:
                    server.send(dict(method='textDocument/didChange', params=dict(textDocument=dict(uri=uri, version=version), contentChanges=[dict(text=text)])))
                at = position(text, text.index('target;') + 2)
                hover = server.query('textDocument/hover', dict(textDocument=dict(uri=uri), position=at))
                completion = server.query('textDocument/completion', dict(textDocument=dict(uri=uri), position=at))
                entries = completion.get('items', []) if isinstance(completion, dict) else completion
                item = next((item for item in entries if item['label'] == 'target'), None)
                assert item is not None, (body, hover)
                if expected is None:
                    assert hover and '尚不能唯一确定类型' in hover['contents']['value'], (body, hover)
                    assert item.get('detail') == 'let target', (body, item)
                else:
                    assert hover and expected + ' target' in hover['contents']['value'], (body, expected, hover)
                    assert item.get('detail') == expected + ' target', (body, expected, item)
                declaration = text.index('let target') + 1
                keyword = server.query('textDocument/hover', dict(textDocument=dict(uri=uri), position=position(text, declaration)))
                assert keyword and keyword['contents'] == hover['contents'], (body, keyword, hover)
            # 同一 URI 中先输入未闭合调用，再替换闭包签名，检查签名不会复用旧版本。
            for version, (argument, result) in enumerate((('int32', 'int32'), ('bool', 'bool')), len(cases) + 1):
                text = f'void main() {{ let target = fn({argument} x) {{ x }}; target('
                server.send(dict(method='textDocument/didChange', params=dict(textDocument=dict(uri=uri, version=version), contentChanges=[dict(text=text)])))
                help = server.query('textDocument/signatureHelp', dict(textDocument=dict(uri=uri), position=position(text, len(text))))
                assert help and help['signatures'][0]['label'] == f'函数 [1/1] {result} fn({argument})', help
            text = 'void main() { let f = int64 fn(int64) fn(int64 fn(int64) a, int64 fn(int64) b) { a }; f('
            server.send(dict(method='textDocument/didChange', params=dict(textDocument=dict(uri=uri, version=len(cases) + 3), contentChanges=[dict(text=text)])))
            help = server.query('textDocument/signatureHelp', dict(textDocument=dict(uri=uri), position=position(text, len(text))))
            assert help and help['signatures'][0]['label'] == '函数 [1/1] (int64 fn(int64)) fn(int64 fn(int64), int64 fn(int64))', help
            hover = server.query('textDocument/hover', dict(textDocument=dict(uri=uri), position=position(text, text.index('{ a }') + 2)))
            assert hover and 'int64 fn(int64) a' in hover['contents']['value'], hover
            for number, argument in enumerate(('int64', 'iny64')):
                text = f'int64 increment(int64 x) {{ return x + 1; }} void main() {{ let f = int64 fn(int64) fn(int64 fn(int64) a, int64 fn({argument}) b) {{ a }}; let value = f(increment, increment)(1); return; }}'
                uri = (Path(folder) / f'higher_order_{number}.chl').as_uri()
                server.send(dict(method='textDocument/didOpen', params=dict(textDocument=dict(uri=uri, version=1, text=text))))
                diagnostics = server.diagnostic(uri, 1, predicate=lambda ds: argument == 'int64' or any(item.get('severity') == 1 for item in ds))['diagnostics']
                errors = [item for item in diagnostics if item.get('severity') == 1]
                if argument == 'int64':
                    assert not errors, diagnostics
                else:
                    assert errors, diagnostics
            recursive = '''void main() {
    int64 fn(int64) fac;
    fac = fn(x) { if (x >= 1) { fac(x - 1) * x } else { 1 } };
    println("fac(3)={}", fac(3));
}'''
            fibonacci = '''void main() {
    int64 fn(int64) fib;
    fib = fn(x) { if (x == 0) { 0 } elif (x == 1) { 1 } else { fib(x-1) + fib(x-2) } };
    println("fib(3)={}", fib(3));
}'''
            counted = fibonacci.replace('    int64 fn(int64) fib;', '    int64 counter = 0;\n    int64 fn(int64) fib;').replace('fn(x) { if', 'fn mut[counter](x) { int64 result = if').replace('fib(x-2) } };', 'fib(x-2) }; counter += 1; return result; };')
            counted = counted.replace('    println("fib(3)={}", fib(3));', '    println("counter={}", counter);\n    println("fib(3)={}", fib(3));\n    println("counter={}", counter);')
            assignments = [
                (recursive, 'int64'),
                (fibonacci, 'int64'),
                (counted, 'int64'),
                ('void main() { bool fn(bool) callback; callback = fn(x) { x }; }', 'bool'),
                ('void main() { int64 fn(int64) callback; { bool fn(bool) callback; callback = fn(x) { x }; } }', 'bool'),
                ('struct Holder { int32 fn(int32) callback; }; void main() { Holder holder; holder.callback = fn(x) { x }; }', 'int32'),
            ]
            for number, (text, expected) in enumerate(assignments):
                uri = (Path(folder) / f'assignment_{number}.chl').as_uri()
                server.send(dict(method='textDocument/didOpen', params=dict(textDocument=dict(uri=uri, version=1, text=text))))
                for match in re.finditer(r'\bx\b', text):
                    at = position(text, match.start())
                    hover = server.query('textDocument/hover', dict(textDocument=dict(uri=uri), position=at))
                    assert hover and expected + ' x' in hover['contents']['value'], (text, hover)
                    completion = server.query('textDocument/completion', dict(textDocument=dict(uri=uri), position=at))
                    items = completion.get('items', []) if isinstance(completion, dict) else completion
                    assert any(item['label'] == 'x' and item.get('detail') == expected + ' x' for item in items), text
                if number < 3:
                    hints = server.query('textDocument/inlayHint', dict(textDocument=dict(uri=uri), range=dict(start=position(text, 0), end=position(text, len(text)))))
                    parameter = re.search(r'fn(?: mut\[counter\])?\(x\)', text)
                    assert any(item['label'] == ': int64' and item['position'] == position(text, parameter.end() - 1) for item in hints), hints
                    diagnostics = server.diagnostic(uri, 1)['diagnostics']
                    assert not [item for item in diagnostics if item.get('severity') == 1], (text, diagnostics)
        finally:
            server.close()
    print(f'LSP 闭包风险与连续编辑：{len(cases)} 组通过。')


if __name__ == '__main__':
    main()
