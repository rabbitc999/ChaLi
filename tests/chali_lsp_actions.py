"""真实编译器核验接口／match 编辑及拒绝候选，不执行 TODO panic。"""
from pathlib import Path
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile
from native_lsp_transport import Transport


def main():
    """应用编辑后再次检查，并覆盖非穷尽、已完整、错误与泛型回退。"""
    binary = compiler_path()
    cases = [
        ('interface Read { int64 read(); };\nstruct Device : Read { };\nvoid main(){}\n', '接口'),
        ('interface Read { int64 read(); void change(int64 n); };\nstruct Device : Read { int64 read(){return 1;} };\nvoid main(){}\n', '接口'),
        ('enum Mode { A, B };\nvoid main(){ Mode m=Mode::A; match(m){ A => {} } }\n', 'match'),
        ('enum Mode { A, B };\nvoid main(){ Mode m=Mode::A; match(m){ A => {} B => {} } }\n', None),
        ('enum Mode { A, B };\nvoid main(){ Mode m=Mode::A; match(m){ _ => {} } }\n', None),
        ('interface Read { int64 read(); };\nstruct Device : Read { };\nvoid main(){ missing(); }\n', None),
        ('interface Read { int64 read(); };\nstruct Device : Read { int64 read(){return 1;} };\nvoid main(){}\n', None),
        ('interface Read<T> { T read(); };\nstruct Device : Read<int64> { };\nvoid main(){}\n', None),
    ]
    with tempfile.TemporaryDirectory(prefix='lsp-actions-') as folder:
        root = Path(folder)
        source = root / 'main.chl'
        server = Transport(binary, root)
        try:
            server.query('initialize', dict(rootUri=root.as_uri()))
            for version, (text, expected) in enumerate(cases, 1):
                source.write_text(text, encoding='utf-8')
                server.send(dict(method='textDocument/didOpen', params=dict(textDocument=dict(uri=source.as_uri(), version=version, text=text))))
                actions = server.query('textDocument/codeAction', dict(textDocument=dict(uri=source.as_uri()),
                    range=dict(start=dict(line=0, character=0), end=dict(line=100, character=0)), context=dict(only=['quickfix'])), timeout=45)
                candidates = [action for action in actions if 'TODO' in action['title']]
                assert bool(candidates) == bool(expected), (version, actions)
                if not expected:
                    continue
                change = candidates[0]['edit']['documentChanges'][0]
                assert change['textDocument']['version'] == version
                edit = change['edits'][0]
                point = edit['range']['start']
                lines = text.splitlines(keepends=True)
                prefix = lines[point['line']].encode('utf-16-le')[:point['character']*2].decode('utf-16-le')
                at = sum(map(len, lines[:point['line']])) + len(prefix)
                fixed = text[:at] + edit['newText'] + text[at:]
                source.write_text(fixed, encoding='utf-8')
                checked = subprocess.run([str(binary), '--check', str(source)], capture_output=True, text=True, encoding='utf-8', timeout=60)
                assert checked.returncode == 0, checked.stdout + checked.stderr
                assert 'panic(' in fixed and expected in candidates[0]['title']
                stats = server.query('chali/lspStats')
                # 候选检查不进入预处理／报告缓存；正式根报告只记录原始文本。
                for entry in (Path(stats['cacheDirectory']) / 'diagnostics').glob('*.json'):
                    assert 'TODO: implement interface method' not in entry.read_text(encoding='utf-8')
        finally:
            server.close()
    print('LSP actions: compiler-checked interface/match edits and conservative rejection passed')


if __name__ == '__main__':
    main()
