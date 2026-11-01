"""真实编译器验证警告修复、标准库只读导航与保留来源的项目配置编辑。"""
from pathlib import Path
import argparse
import json
import shutil
import struct
import subprocess
import tomllib
import time
from compiler_path import compiler_path
from project_temp import tempfile
from native_lsp_transport import Transport, position


def main():
    """从隔离编译器查询源码和配置，不依赖仓库目录或 Python 开发命令。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    binary = parser.parse_args().chali.resolve()
    with tempfile.TemporaryDirectory(prefix='lsp-editor-') as directory:
        root = Path(directory)
        compiler = root / 'compiler' / binary.name
        compiler.parent.mkdir()
        shutil.copy2(binary, compiler)
        project = root / 'project'
        project.mkdir()
        source = project / 'main.chl'
        text = 'import std::parse;\nvoid main(){\n int64 ignored=1;\n assert(std::parse::Parse::parseInt64("3").unwrap()==3);\n}\n'
        source.write_text(text, encoding='utf-8')
        config = project / 'chali.toml'
        original = '[package]\nname="editor"\ndescription="""\n[aot]\ntarget="literal, not a setting"\n"""\n# preserve this comment\n[aot] # table comment\ntarget="windows-x64" # target comment\noutput="object"\n[warnings]\nunused-local="warn"\n'
        config.write_text(original, encoding='utf-8')
        server = Transport(compiler, project)
        uri = source.as_uri()
        try:
            capabilities = server.query('initialize', dict(rootUri=project.as_uri()))['capabilities']
            assert capabilities['codeActionProvider']['codeActionKinds'] == ['quickfix']
            info = server.query('chali/projectInfo', dict(uri=uri))
            assert info['valid'] and info['llvm'] in ('static', 'external') and info['output'] == 'object', info
            assert {entry['label'] for entry in info['targets']} == {'windows-x64', 'linux-x64', 'linux-arm64'}, info
            dirty = original + '# unsaved comment\n'
            edit = server.query('chali/projectEdit', dict(uri=uri, text=dirty, version=7, key='target', value='linux-arm64'))
            change = edit['documentChanges'][0]
            assert change['textDocument']['uri'] == config.as_uri() and change['textDocument']['version'] == 7, change
            updated = change['edits'][0]['newText']
            assert tomllib.loads(updated)['package']['description'] == tomllib.loads(original)['package']['description']
            assert tomllib.loads(updated)['aot']['target'] == 'linux-arm64'
            assert all(comment in updated for comment in ('# preserve this comment', '# target comment', '# unsaved comment'))
            assert config.read_text(encoding='utf-8') == original, 'LSP must not write project files'
            config.write_text(updated, encoding='utf-8')
            server.send(dict(method='workspace/didChangeWatchedFiles', params=dict(changes=[dict(uri=config.as_uri(), type=2)])))
            info = server.query('chali/projectInfo', dict(uri=uri))
            assert info['target'] == 'aarch64-unknown-linux-gnu' and not info['windows'], info
            server.send(dict(method='textDocument/didOpen', params=dict(textDocument=dict(uri=uri, text=text, version=1))))
            diagnostics = server.diagnostic(uri, 1, lambda values: any(d.get('code') == 'CHL2201' for d in values))
            assert len(diagnostics['diagnostics']) == 1, diagnostics
            actions = server.query('textDocument/codeAction', dict(textDocument=dict(uri=uri),
                range=dict(start=dict(line=2, character=0), end=dict(line=2, character=50)), context=dict(only=['quickfix'], diagnostics=[])))
            assert len(actions) == 1 and actions[0]['kind'] == 'quickfix', actions
            fixed = actions[0]['edit']['documentChanges'][0]
            assert fixed['textDocument']['version'] == 1 and fixed['textDocument']['uri'] == uri
            insertion = fixed['edits'][0]
            lines = text.splitlines(keepends=True)
            lines.insert(insertion['range']['start']['line'], insertion['newText'])
            text = ''.join(lines)
            assert 'int64 ignored=1;' in text
            server.send(dict(method='textDocument/didChange', params=dict(textDocument=dict(uri=uri, version=2), contentChanges=[dict(text=text)])))
            server.diagnostic(uri, 2, lambda values: not values)
            cursor = position(text, text.index('parseInt64') + 4)
            definitions = server.query('textDocument/definition', dict(textDocument=dict(uri=uri), position=cursor))
            assert definitions and definitions[0]['uri'].startswith('chali-stdlib:/std/'), definitions
            target = definitions[0]
            standard = server.query('chali/standardSource', dict(uri=target['uri']))
            assert standard['readonly'] and standard['uri'] == target['uri'] and 'module std::parse' in standard['text']
            line = standard['text'].splitlines()[target['range']['start']['line']]
            begin = target['range']['start']['character']
            end = target['range']['end']['character']
            assert line.encode('utf-16-le')[begin*2:end*2].decode('utf-16-le') == 'parseInt64', target
            server.send(dict(id=500, method='textDocument/prepareRename', params=dict(textDocument=dict(uri=uri), position=cursor)))
            assert 'error' in server.response(500), 'standard declarations must not be renameable'
            server.send(dict(id=501, method='chali/standardSource', params=dict(uri='chali-stdlib:/../../AGENTS.md')))
            assert 'error' in server.response(501), 'source endpoint must not read arbitrary files'
            # 标准库导航索引不写入语言服务的磁盘索引。
            stats = server.query('chali/lspStats')
            for path in (Path(stats['cacheDirectory']) / 'index').glob('*.json'):
                entry = json.loads(path.read_text(encoding='utf-8'))
                assert not entry['uri'].startswith('chali-stdlib:'), path
            assert not list(project.rglob('*.unit')) and not (project / '.chali/chali-stdlib-cache').exists()
            other = root / 'other'
            other.mkdir()
            other_source = other / 'main.chl'
            other_source.write_text('void main(){}\n', encoding='utf-8')
            info = server.query('chali/projectInfo', dict(uri=other_source.as_uri()))
            assert not info['exists'], info
            creation = server.query('chali/projectEdit', dict(uri=other_source.as_uri(), text='', version=None, key='output', value='object'))
            assert creation['documentChanges'][0]['kind'] == 'create' and not (other / 'chali.toml').exists()
            assert tomllib.loads(creation['documentChanges'][1]['edits'][0]['newText'])['aot']['output'] == 'object'
        finally:
            server.close()
        # 项目清单由编辑结果决定目标与输出；只验证 Linux ARM64 对象，不执行目标机器码。
        command = [str(compiler), 'aot', str(source), '--out', str(root / 'cross')]
        if info['llvm'] == 'external':
            clang = Path(__file__).resolve().parents[1] / 'build/dependencies/llvm/toolchain/bin/clang.exe'
            if clang.exists(): command += ['--llvm', str(clang)]
        result = subprocess.run(command, cwd=project,
                                capture_output=True, encoding='utf-8', timeout=180)
        assert result.returncode == 0, result.stdout + result.stderr
        product = (root / 'cross.o').read_bytes()
        assert product[:4] == b'\x7fELF' and struct.unpack_from('<H', product, 18)[0] == 183
        # 子进程已 wait／close；Windows 的短暂映像扫描可能晚于进程退出释放文件。
        for attempt in range(30):
            try:
                compiler.unlink()
                break
            except PermissionError:
                if attempt == 29:
                    raise
                time.sleep(0.1)
    print('LSP editor: versioned warning fix, real read-only standard source, safe TOML edits and ARM64 object build passed')


if __name__ == '__main__':
    main()
