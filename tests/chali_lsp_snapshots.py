"""验证语义报告合并、内存复用、未保存依赖与旧版本发布保护。"""
from pathlib import Path
import argparse
import json
import shutil
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile
from native_lsp_transport import Transport, position


def main():
    """在隔离编译器旁删除磁盘报告，确认内存复用仍执行依赖校验。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    binary = parser.parse_args().chali.resolve()
    with tempfile.TemporaryDirectory(prefix='lsp-snapshot-') as directory:
        root = Path(directory)
        compiler = root / 'compiler' / binary.name
        compiler.parent.mkdir()
        shutil.copy2(binary, compiler)
        project = root / 'project'
        project.mkdir()
        source = project / 'main.chl'
        dependency = project / 'dep.chl'
        dependency.write_text('module dep;\nint64 read(){return 1;}\n', encoding='utf-8')
        text = 'import dep;\n$macro once(n)\nn\n$end_macro\nvoid main(){int64 fixed=dep::read();let value=$once(fixed);assert(value==1);}\n'
        source.write_text(text, encoding='utf-8')
        server = Transport(compiler, project)
        uri = source.as_uri()
        try:
            server.query('initialize', dict(rootUri=project.as_uri()))
            server.send(dict(method='textDocument/didOpen', params=dict(textDocument=dict(uri=uri, text=text, version=1))))
            server.send(dict(id=100, method='textDocument/inlayHint', params=dict(textDocument=dict(uri=uri),
                        range=dict(start=position(text, 0), end=position(text, len(text))))))
            server.send(dict(id=101, method='chali/expandMacro', params=dict(textDocument=dict(uri=uri), mode='all')))
            for serial in (100, 101):
                response = server.response(serial, timeout=90)
                assert 'error' not in response, response
            server.diagnostic(uri, 1, lambda values: not values, timeout=90)
            stats = server.query('chali/lspStats')
            assert stats['checks'] == 1 and stats['mergedChecks'] >= 1, stats
            cache = Path(stats['cacheDirectory'])
            for path in (cache / 'diagnostics').glob('*.json'):
                path.unlink()
            before = len(server.notifications)
            server.send(dict(method='textDocument/didSave', params=dict(textDocument=dict(uri=uri))))
            server.diagnostic(uri, 1, lambda values: not values, after=before)
            reused = server.query('chali/lspStats')
            assert reused['checks'] == 1 and reused['memoryReportHits'] > stats['memoryReportHits'], reused
            assert reused['reportEntries'] == 1 and 0 < reused['reportBytes'] <= 32 * 1024 * 1024, reused
            # 不同源码版本各自保存报告，撤销后命中旧快照。
            def edit(body, version):
                server.send(dict(method='textDocument/didChange', params=dict(textDocument=dict(uri=uri, version=version),
                                 contentChanges=[dict(text=body)])))
                server.diagnostic(uri, version, lambda values: not values)
            edit(text + '\n\n', 2)
            alternate = server.query('chali/lspStats')
            assert alternate['checks'] > reused['checks'], alternate
            edit(text + '\n', 3)
            intermediate = server.query('chali/lspStats')
            edit(text + '\n\n', 4)
            restored = server.query('chali/lspStats')
            assert restored['checks'] == intermediate['checks'] and restored['diagnosticHits'] > intermediate['diagnosticHits'], restored
            # 原先不存在的项目清单被创建，也要触发入口重新检查。
            config = project / 'chali.toml'
            config.write_text('[warnings]\nall="allow"\n', encoding='utf-8')
            before = len(server.notifications)
            server.send(dict(method='workspace/didChangeWatchedFiles', params=dict(changes=[dict(uri=config.as_uri(), type=1)])))
            server.diagnostic(uri, 4, lambda values: not values, after=before)
            configured = server.query('chali/lspStats')
            assert configured['checks'] > reused['checks'], configured
            # 未保存依赖改变实际类型，不能复用入口旧报告。
            before = len(server.notifications)
            changed = 'module dep;\nString read(){return "changed";}\n'
            server.send(dict(method='textDocument/didOpen', params=dict(textDocument=dict(uri=dependency.as_uri(), text=changed, version=1))))
            diagnostic = server.diagnostic(uri, 4, lambda values: any(d['severity'] == 1 for d in values), after=before)
            assert any('String' in d['message'] for d in diagnostic['diagnostics']), diagnostic
            assert server.query('chali/lspStats')['checks'] > configured['checks']
            # 快速替换入口及依赖，等待最终版本，随后不允许旧结果覆盖。
            latest = 'import dep;\nvoid main(){String value=dep::read();assert(value=="changed");}\n'
            for version in range(5, 12):
                server.send(dict(method='textDocument/didChange', params=dict(textDocument=dict(uri=uri, version=version), contentChanges=[dict(text=latest)])))
            server.diagnostic(uri, 11, lambda values: not values)
            after = len(server.notifications)
            server.query('textDocument/inlayHint', dict(textDocument=dict(uri=uri), range=dict(start=position(latest, 0), end=position(latest, len(latest)))))
            for event in server.notifications[after:]:
                if event.get('method') == 'textDocument/publishDiagnostics' and event['params'].get('uri') == uri:
                    assert event['params']['version'] == 11 and not event['params']['diagnostics'], event
        finally:
            server.close()
        # JSON 诊断的源码摘录来自未保存文本，不能回读磁盘上的旧程序。
        overlay = 'void main(){\n missingUnsaved();\n}\n'
        result = subprocess.run([str(compiler), '--check', '--diagnostics=json', '--source-stdin', str(source)],
                                input=overlay, capture_output=True, encoding='utf-8', cwd=project, timeout=90)
        assert result.returncode != 0, result
        diagnostics = json.loads(result.stdout)['diagnostics']
        assert diagnostics[0]['source'] == overlay.splitlines()[1], diagnostics
    print('LSP snapshots: one semantic check, memory reuse, dependency invalidation, latest version and unsaved excerpts passed')


if __name__ == '__main__':
    main()
