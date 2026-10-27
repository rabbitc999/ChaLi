"""检查语义高亮的长行性能、UTF-16 位置及编辑失效。"""
from compiler_path import compiler_path
from pathlib import Path
from project_temp import tempfile
from native_lsp_transport import Transport, position
import json
import statistics
import time


def main():
    """测量相同文本及修改文本的请求，并验证完整 token 编码。"""
    results = {}
    with tempfile.TemporaryDirectory(prefix='chali-lsp-highlight-speed-') as folder:
        server = Transport(compiler_path(), folder)
        server.query('initialize', dict(rootUri=Path(folder).as_uri()))
        try:
            for layout in ('lines', 'long_line'):
                separator = '\n' if layout == 'lines' else ' '
                text = 'void main() { /* 😀 中文 */' + separator.join(f'int64 value{i} = {i};' for i in range(1600)) + ' value1599; }'
                uri = (Path(folder) / (layout + '.chl')).as_uri()
                server.send(dict(method='textDocument/didOpen', params=dict(textDocument=dict(uri=uri, text=text, version=1))))
                times = []
                original = None
                for _ in range(4):
                    begin = time.perf_counter()
                    data = server.query('textDocument/semanticTokens/full', dict(textDocument=dict(uri=uri)))['data']
                    times.append(time.perf_counter() - begin)
                    if original is not None:
                        assert data == original
                    original = data
                line = column = 0
                found = False
                target = position(text, text.rindex('value1599'))
                for i in range(0, len(data), 5):
                    dl, dc, length, kind, modifiers = data[i:i + 5]
                    line += dl
                    column = dc if dl else column + dc
                    assert length > 0 and dl >= 0 and (dl > 0 or dc >= 0)
                    if (line, column) == (target['line'], target['character']):
                        assert length == len('value1599') and kind == 4
                        found = True
                assert found
                edited = text.replace('/* 😀 中文 */', '/* 😀😀 中文 */\n')
                server.send(dict(method='textDocument/didChange', params=dict(textDocument=dict(uri=uri, version=2), contentChanges=[dict(text=edited)])))
                changed = server.query('textDocument/semanticTokens/full', dict(textDocument=dict(uri=uri)))['data']
                assert changed != original
                results[layout] = dict(first_seconds=round(times[0], 6), median_seconds=round(statistics.median(times[1:]), 6), tokens=len(original) // 5)
                assert max(times) < 8, (layout, times)
        finally:
            server.close()
    assert results['long_line']['median_seconds'] < max(0.1, results['lines']['median_seconds'] * 10), results
    print(json.dumps(results, ensure_ascii=False))


if __name__ == '__main__':
    main()
