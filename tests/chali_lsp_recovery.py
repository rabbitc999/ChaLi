"""验证单次 LSP 检查、编辑恢复与编译器同级缓存配置。"""
from pathlib import Path
import os
import shutil
import time

from compiler_path import compiler_path
from native_lsp_transport import Transport, position
from project_temp import tempfile


ROOT = Path(__file__).resolve().parents[1]


def main():
    """通过真实服务核对并行导入、未保存编辑及定期清理。"""
    with tempfile.TemporaryDirectory(prefix='lsp-recovery-') as directory:
        work = Path(directory)
        compiler = compiler_path()
        binary = work / compiler.name
        shutil.copy2(compiler, binary)
        config = work / 'compiler.toml'
        config.write_text('[lsp]\ncache = false\ncleanup_minutes = 1\n', encoding='utf-8')
        cache = work / 'build/lsp'
        project = work / 'project'
        project.mkdir()
        main_uri = (project / 'main.chl').as_uri()
        fixture = (ROOT / 'tests/fixtures/macro_generated_symbols.chl').read_text(encoding='utf-8')
        fixture = fixture.replace('    Box box = Box::create(42);', '    let box = Box::create(42);')
        header = 'import left;\nimport right;\nimport common;\n'
        valid = header + fixture + '\n'
        original = '    assert(box.value == 42'
        broken = valid.replace(original, '    assert(, box.value == 42')
        assert broken != valid
        server = Transport(binary, project)
        try:
            server.query('initialize', {'rootUri': project.as_uri()})
            for name, text in [('left', 'module left;\nimport common;\n'),
                               ('right', 'module right;\nimport common;\n'),
                               ('common', 'module common;\n')]:
                server.send(dict(method='textDocument/didOpen', params=dict(textDocument=dict(
                    uri=(project / f'{name}.chl').as_uri(), text=text, version=1))))
            server.send(dict(method='textDocument/didOpen', params=dict(textDocument=dict(
                uri=main_uri, text=valid, version=1))))

            def hints(text):
                return server.query('textDocument/inlayHint', dict(textDocument=dict(uri=main_uri),
                                    range=dict(start=position(text, 0), end=position(text, len(text)))))

            def box_hint(text):
                return any(item['position']['line'] == position(text, text.index('let box'))['line']
                           and 'Box' in item['label'] for item in hints(text))

            assert box_hint(valid)
            assert box_hint(valid)
            stats = server.query('chali/lspStats')
            assert not stats['persistentCache'] and stats['cleanupMinutes'] == 1, stats
            assert stats['diagnosticHits'] == stats['restored'] == 0, stats
            assert stats['checks'] >= 2, stats
            if (os.cpu_count() or 1) > 1:
                assert stats['parallelModules'] >= 2, stats

            def change(uri, text, version):
                server.send(dict(method='textDocument/didChange', params=dict(textDocument=dict(uri=uri, version=version),
                                 contentChanges=[dict(text=text)])))

            change(main_uri, broken, 2)
            assert box_hint(broken)
            server.diagnostic(main_uri, 2, lambda diagnostics: bool(diagnostics))
            completion = server.query('textDocument/completion', dict(textDocument=dict(uri=main_uri),
                                      position=position(broken, broken.rindex('box.value') + 4)))
            assert any(item['label'] == 'value' for item in completion), completion
            # 修改之前的声明或任一实际依赖后，旧的编译器提示必须失效。
            change(main_uri, broken.replace('Box::create(42)', '+'), 3)
            assert not box_hint(broken.replace('Box::create(42)', '+'))
            change((project / 'common.chl').as_uri(), 'module common;\nint64 updated = 1;\n', 2)
            change(main_uri, broken, 4)
            assert not box_hint(broken)
            assert not cache.exists(), list(cache.rglob('*'))

            # 服务保持空闲，配置的计时线程仍会移除非当天文件。
            cache.mkdir(parents=True)
            stale = cache / 'yesterday.unit'
            stale.write_text('expired', encoding='utf-8')
            past = time.time() - 86400
            os.utime(stale, (past, past))
            today = cache / 'today.unit'
            today.write_text('current', encoding='utf-8')
            deadline = time.monotonic() + 70
            while stale.exists() and time.monotonic() < deadline:
                time.sleep(0.25)
            assert not stale.exists() and today.exists()
        finally:
            server.close()
    print('原生 LSP 并行导入、语法提示恢复、依赖失效、禁用磁盘缓存及空闲定期清理通过。')


if __name__ == '__main__':
    main()
