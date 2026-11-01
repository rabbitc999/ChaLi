"""真实语言服务的语义进程上限、编辑取消、交互通道及报告淘汰。"""
from pathlib import Path
import argparse
from compiler_path import compiler_path
from project_temp import tempfile
from native_lsp_transport import Transport, position


def main():
    """用完整检查请求制造竞争，核对每个 JSON-RPC 响应和最终版本。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    with tempfile.TemporaryDirectory(prefix='lsp-pipeline-') as directory:
        root = Path(directory)
        server = Transport(compiler, root)
        text = 'void main(){\n' + '\n'.join(f'int64 value{i}={i};' for i in range(3000)) + '\nassert(value2999==2999);}\n'
        paths = [root / f'heavy{i}.chl' for i in range(3)]
        try:
            server.query('initialize', dict(rootUri=root.as_uri()))
            for path in paths:
                server.send(dict(method='textDocument/didOpen', params=dict(textDocument=dict(uri=path.as_uri(), text=text, version=1))))
            span = dict(start=position(text, 0), end=position(text, len(text)))
            for serial, path in enumerate(paths, 100):
                method = 'textDocument/inlayHint' if serial != 101 else 'chali/expandMacro'
                server.send(dict(id=serial, method=method, params=dict(textDocument=dict(uri=path.as_uri()), range=span, mode='all')))
            result = server.query('textDocument/completion', dict(textDocument=dict(uri=paths[0].as_uri()),
                                  position=position(text, text.rindex('value2999') + 5)), timeout=10)
            items = result.get('items', []) if isinstance(result, dict) else result
            assert any(item['label'] == 'value2999' for item in items), items[-10:]
            for serial in range(100, 103):
                response = server.response(serial, timeout=90)
                assert 'error' not in response, response
            stats = server.query('chali/lspStats')
            assert 0 < stats['analysisPeak'] <= 2 and stats['analysisActive'] <= 2, stats
            # 旧版本的排队请求必须各自得到结果或取消响应，不能漏掉响应。
            uri = paths[0].as_uri()
            server.send(dict(method='textDocument/didChange', params=dict(textDocument=dict(uri=uri, version=2), contentChanges=[dict(text=text+'\n')])) )
            for serial in range(200, 220):
                server.send(dict(id=serial, method='textDocument/inlayHint', params=dict(textDocument=dict(uri=uri), range=span)))
            latest = 'void main(){let answer=1;assert(answer==1);}\n'
            server.send(dict(method='textDocument/didChange', params=dict(textDocument=dict(uri=uri, version=3), contentChanges=[dict(text=latest)])))
            cancelled = 0
            for serial in range(200, 220):
                response = server.response(serial, timeout=90)
                if 'error' in response:
                    assert response['error']['code'] == -32800, response
                    cancelled += 1
            assert cancelled > 0
            server.query('textDocument/inlayHint', dict(textDocument=dict(uri=uri), range=dict(start=position(latest, 0), end=position(latest, len(latest)))), timeout=90)
            server.diagnostic(uri, 3, lambda values: not values)
            # 超过驻留入口数量后仍能正确分析，且只保留 64 项／32 MiB。
            small = 'void main(){let answer=1;assert(answer==1);}\n'
            for index in range(66):
                target = (root / f'quota{index}.chl').as_uri()
                server.send(dict(method='textDocument/didOpen', params=dict(textDocument=dict(uri=target, text=small, version=1))))
                server.query('textDocument/inlayHint', dict(textDocument=dict(uri=target), range=dict(start=position(small, 0), end=position(small, len(small)))), timeout=90)
            final = server.query('chali/lspStats')
            assert final['reportEntries'] == 64 and final['reportBytes'] <= 32 * 1024 * 1024, final
            assert final['analysisPeak'] <= 2 and final['cancelled'] >= cancelled, final
        finally:
            server.close()
    print('LSP pipeline: two semantic processes, responsive completion, complete cancellation responses, latest version and bounded reports passed')


if __name__ == '__main__':
    main()
