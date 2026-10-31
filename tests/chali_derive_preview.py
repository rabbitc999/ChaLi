"""从原生 LSP 核对内建派生的实际方法、模式、字段来源及诊断。"""
import argparse
import json
import os
from pathlib import Path
import subprocess
from compiler_path import compiler_path
from native_lsp_transport import Transport, position
from project_temp import tempfile


def walk(node):
    """遍历编译器返回的有界树，忽略空孩子。"""
    if not isinstance(node, dict):
        return
    yield node
    for child in node.get('children', []):
        yield from walk(child)


def main():
    """只查询未保存源码的真实语义结果，不执行第二套派生器。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--case', action='append', default=[])
    args = parser.parse_args()
    compiler = args.chali.resolve()
    cases = [
        ('struct', '''// 中文来源
#derive(Equal,Hash,Clone,Debug)
struct Data {
    int64 number;
    String name;
    static int64 shared;
}
void main(){Data value;value.number=7;value.name="owned";value.debug();}
''', {'leftEqual', 'hash', 'clone', 'debug'}, {'number', 'name'}),
        ('enum', '''#derive(Equal,Hash,Clone,Debug)
enum Event {
    Empty,
    Pair(int64 left,String right),
    Many(int8 a,int16 b,int32 c,int64 d)
}
void main(){let value=Event::Pair(7,"owned");value.debug();}
''', {'leftEqual', 'hash', 'clone', 'debug'}, {'Empty', 'Pair', 'Many'}),
        ('generic', '''#derive(Debug)
struct Box<T>{T value;}
void main(){Box<int64> number;number.value=7;number.debug();Box<String> text;text.value="x";text.debug();}
''', {'debug'}, {'value'}),
        ('generic_module', '''module alpha;
#derive(Debug)
struct Box<T>{T value;}
void main(){Box<int64> value;value.debug();}
''', {'debug'}, {'value'}),
        ('generic_locations', '''#NoEqualReplace
    #derive(Debug,
            Clone,Equal)
struct Box<T>{T value;}
void main(){Box<int64> number;number.value=7;number.clone();number.debug();}
''', {'debug', 'clone', 'leftEqual'}, {'value'}),
        ('json', '''import std::json;
#derive(Json)
struct Data {
    @std::json::Field("identifier") int64 id;
    @std::json::Field(skip:true) String secret;
}
void main(){Data value;value.toJson();}
''', {'toJson', 'fromJson'}, {'id', 'secret'}),
    ]
    if args.case:
        cases = [case for case in cases if case[0] in args.case]
        assert len(cases) == len(set(args.case))
    with tempfile.TemporaryDirectory(prefix='derive-preview-') as directory:
        folder = Path(directory)
        server = Transport(compiler, str(folder))
        server.query('initialize', dict(rootUri=folder.as_uri()))
        try:
            for name, source, methods, inputs in cases:
                path = folder / (name + '.chl')
                server.send(dict(method='textDocument/didOpen', params=dict(textDocument=dict(uri=path.as_uri(), text=source, version=1))))
                modes = []
                for mode in ('one', 'all'):
                    response = server.query('chali/expandMacro', dict(textDocument=dict(uri=path.as_uri()), mode=mode,
                        position=position(source, source.index('#derive') + 4)), timeout=180)
                    assert not response['diagnostics'], (name, response)
                    entries = json.loads(response['text'])['expansions']
                    assert entries and all(entry['name'].startswith('#derive(') for entry in entries), (name, response)
                    nodes = [node for entry in entries for node in walk(entry['tree'])]
                    actual = {node['name'] for node in nodes if node.get('kind') == 'method'}
                    assert actual == methods, (name, actual)
                    metadata = [node for entry in entries for node in entry['tree']['children'] if node.get('kind') == 'metadata']
                    declared = {node['name'] for group in metadata for node in group['children']}
                    assert declared == inputs, (name, declared)
                    for entry in entries:
                        assert Path(entry['file']).resolve() == path.resolve(), entry
                        assert entry['line'] == source[:source.index('#derive')].count('\n') + 1, entry
                        assert entry['column'] == len(source[:source.index('#derive')].rsplit('\n', 1)[-1].encode('utf-8')) + 1, entry
                    locations = [node['metadataSource'] for node in nodes if node.get('metadataSource')]
                    if name != 'json':
                        assert locations, (name, nodes)
                    assert all(Path(location['file']).resolve() == path.resolve() and 1 <= location['line'] <= len(source.splitlines()) for location in locations)
                    if name in ('struct', 'generic', 'generic_module', 'generic_locations'):
                        declarations = {node['symbol'] for node in nodes if node.get('declares') and node.get('symbol')}
                        references = {node['symbol'] for node in nodes if node.get('declares') is False and node.get('symbol')}
                        assert declarations & references, (declarations, references)
                    if name == 'enum':
                        assert any(node.get('kind') == 'match' for node in nodes), nodes
                        assert any(node.get('kind') == 'case' for node in nodes), nodes
                        assert any(node.get('member') == 'Pair' for node in nodes), nodes
                    if name == 'generic':
                        types = {entry['tree']['type'] for entry in entries}
                        assert any('int64' in value for value in types) and any('String' in value for value in types), types
                    if name == 'generic_module':
                        assert all(entry['tree']['type'] == 'alpha::Box<int64>' for entry in entries), entries
                    if name == 'json':
                        members = {node.get('member') for node in nodes}
                        assert {'serialize<program::Data>', 'deserialize<program::Data>'} <= members, members
                        assert '__chl_' not in response['text'], response
                    assert 'bindingIdentity' not in response['text'] and 'controlContext' not in response['text'], response
                    modes.append(entries)
                assert [(item['file'],item['line'],item['column']) for item in modes[0]] == [(item['file'],item['line'],item['column']) for item in modes[1]], name
                assert not path.exists(), path
                print(name + ' 的方法／来源／符号预览通过', flush=True)
            source = '#derive(Debug)\nstruct Broken{int64 values[2];}\nvoid main(){}\n'
            path = folder / 'invalid.chl'
            server.send(dict(method='textDocument/didOpen', params=dict(textDocument=dict(uri=path.as_uri(), text=source, version=1))))
            response = server.query('chali/expandMacro', dict(textDocument=dict(uri=path.as_uri()), mode='all'), timeout=180)
            assert response['diagnostics'] and '#derive(Debug)' in '\n'.join(response['diagnostics']), response
            assert '派生 #derive(Debug)' in '\n'.join(response['diagnostics']), response
        finally:
            server.close()
        trace = folder / 'trace.chl'
        trace.write_text('''$macro MAKE(name:ident) -> item
#derive(Debug)
struct name{int64 values[2];}
$end_macro
$MAKE(Broken);
void main(){}
''', encoding='utf-8')
        for mode in ('short', 'full'):
            result = subprocess.run([str(compiler), 'check', str(trace), '--macro-backtrace', mode],
                cwd=folder, env=dict(os.environ, CHALI_LANG='zh'), capture_output=True, text=True, encoding='utf-8', timeout=120)
            assert result.returncode != 0 and '派生 #derive(Debug)' in result.stderr, (mode, result)
            if mode == 'full':
                assert '展开宏 $MAKE' in result.stderr, result
            else:
                assert '展开宏 $MAKE' not in result.stderr and '--macro-backtrace full' in result.stderr, result
    print('内建派生预览：结构体、泛型、枚举载荷、JSON、来源导航与诊断通过')


if __name__ == '__main__':
    main()
