"""用真实编译与 LSP 验证一层／全部宏预览、缓存和源码来源。"""
import argparse
import json
import os
from pathlib import Path
import subprocess
from compiler_path import compiler_path
from native_lsp_transport import Transport, position
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def nodes(tree):
    """遍历预览公开树，不读取编译器内部节点编号。"""
    if not isinstance(tree, dict):
        return
    yield tree
    for child in tree.get('children', []):
        yield from nodes(child)


def main():
    """核对两种预览与实际执行一致，并验证版本更新使缓存失效。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    source = '''$macro DOUBLE(n:value) -> expr
n+n
$end_macro
$macro OUT(n:value) -> expr
let local=$DOUBLE(n);
local+local
$end_macro
int64 calls=0;
int64 next(){calls+=1;return calls;}
void main(){
String text="你好";let first=$OUT(next());let second=$DOUBLE(2);
assert(first==4&&second==4&&calls==1&&text=="你好");
}
'''
    with tempfile.TemporaryDirectory(prefix='macro-preview-') as directory:
        work = Path(directory)
        path = work / 'preview.chl'
        uri = path.as_uri()
        server = Transport(compiler, directory)
        server.query('initialize', dict(rootUri=work.as_uri()))
        try:
            server.send(dict(method='textDocument/didOpen', params=dict(textDocument=dict(uri=uri, text=source, version=1))))
            request = dict(textDocument=dict(uri=uri))
            one = server.query('chali/expandMacro', dict(request, mode='one'))
            all_result = server.query('chali/expandMacro', dict(request, mode='all'))
            assert not one['diagnostics'] and not all_result['diagnostics'], (one, all_result)
            before = json.loads(one['text'])
            after = json.loads(all_result['text'])
            preview_lines = all_result['text'].splitlines()
            assert len(all_result['mappings']) == len(after['expansions']), all_result
            for mapping in all_result['mappings']:
                anchor = preview_lines[mapping['expandedLine']]
                assert anchor == '    {', (mapping, anchor)
            outer_one = next(item for item in before['expansions'] if item['name'] == 'OUT')
            outer_all = next(item for item in after['expansions'] if item['name'] == 'OUT')
            assert any(node['kind'] == 'macro_call' for node in nodes(outer_one['tree'])), outer_one
            complete = list(nodes(outer_all['tree']))
            assert not any(node['kind'] == 'macro_call' for node in complete), outer_all
            caches = [node for node in complete if node.get('parameterCache') == 'value']
            assert caches and any(node.get('type') == 'int64' for node in caches), complete
            assert any(node.get('name') == 'local' and node.get('type') == 'int64' for node in complete), complete
            declarations = {node['symbol'] for node in complete if node.get('declares') and node.get('symbol')}
            references = {node['symbol'] for node in complete if node.get('declares') is False and node.get('symbol')}
            assert declarations & references, complete
            assert any(node.get('origin') == 'argument' for node in complete), complete
            assert any(node.get('definition', {}).get('line') == 4 for node in complete), complete
            for node in complete:
                if node.get('source', {}).get('file') and Path(node['source']['file']) == path:
                    assert 1 <= node['source']['line'] <= source.count('\n') + 1, node
            assert 'bindingIdentity' not in all_result['text'] and '__chali_decl_' not in all_result['text'], all_result
            repeated = server.query('chali/expandMacro', dict(request, mode='all'))
            assert repeated['text'] == all_result['text'], repeated
            selected = server.query('chali/expandMacro', dict(request, mode='all', position=position(source, source.index('$DOUBLE(2)') + 2)))
            entries = json.loads(selected['text'])['expansions']
            assert len(entries) == 1 and entries[0]['name'] == 'DOUBLE', selected
            invalid = server.query('chali/expandMacro', dict(request, mode='invalid'))
            assert invalid['diagnostics'], invalid
            changed = source.replace('n+n\n', 'n+n+n\n').replace('first==4&&second==4', 'first==6&&second==6')
            server.send(dict(method='textDocument/didChange', params=dict(textDocument=dict(uri=uri, version=2), contentChanges=[dict(text=changed)])))
            updated = server.query('chali/expandMacro', dict(request, mode='all'))
            assert not updated['diagnostics'] and updated['text'] != all_result['text'], updated
            dependency = work / 'parts.chl'
            dependency.write_text('module parts;\n$macro EXTRA(n:value) -> expr\nn+1\n$end_macro\n', encoding='utf-8')
            imported_uri = (work / 'imported.chl').as_uri()
            imported = 'import parts;\nvoid main(){let value=$parts::EXTRA(2);}'
            server.send(dict(method='textDocument/didOpen', params=dict(textDocument=dict(uri=imported_uri, text=imported, version=1))))
            imported_request = dict(textDocument=dict(uri=imported_uri), mode='all')
            first_import = server.query('chali/expandMacro', imported_request)
            assert not first_import['diagnostics'], first_import
            dependency.write_text(dependency.read_text(encoding='utf-8').replace('n+1', 'n+9'), encoding='utf-8')
            second_import = server.query('chali/expandMacro', imported_request)
            assert not second_import['diagnostics'] and second_import['text'] != first_import['text'], second_import
            imported_nodes = [node for entry in json.loads(second_import['text'])['expansions'] for node in nodes(entry['tree'])]
            assert any(node.get('value') == 9 for node in imported_nodes), imported_nodes
            assert any(Path(node.get('definition', {}).get('file', '')) == dependency for node in imported_nodes), imported_nodes
            member_source = '''$macro METHODS(name:ident) -> member
type Item=int64;
Item helper(){return 7;}
Item name(){return self.helper();}
$end_macro
struct Cell{$METHODS(read);}
void main(){Cell c;assert(c.read()==7);}
'''
            member_uri = (work / 'members.chl').as_uri()
            server.send(dict(method='textDocument/didOpen', params=dict(textDocument=dict(uri=member_uri, text=member_source, version=1))))
            member_preview = server.query('chali/expandMacro', dict(textDocument=dict(uri=member_uri), mode='all'))
            assert not member_preview['diagnostics'], member_preview
            member_nodes = [node for entry in json.loads(member_preview['text'])['expansions'] for node in nodes(entry['tree'])]
            published = next(node for node in member_nodes if node.get('kind') == 'method' and node.get('name') == 'read')
            expected_column = member_source.splitlines()[5].index('read') + 1
            assert published['source']['line'] == 6 and published['source']['column'] == expected_column, published
            private = next(node for node in member_nodes if node.get('kind') == 'method' and node.get('name') == 'helper')
            assert any(node.get('kind') == 'member' and node.get('symbol') == private.get('symbol') for node in member_nodes), member_nodes
            member_once = server.query('chali/expandMacro', dict(textDocument=dict(uri=member_uri), mode='one'))
            assert not member_once['diagnostics'] and '__chali_decl_' not in member_once['text'], member_once
            field_source = '''$macro SUM<T>(&obj:value) -> expr
int64 total=0;
$for field in $fields<T>()
total+=$member(obj,field);
$end_for
total
$end_macro
struct Value{int64 number;}
void main(){Value v;let total=$SUM<Value>(v);}
'''
            field_uri = (work / 'fields.chl').as_uri()
            server.send(dict(method='textDocument/didOpen', params=dict(textDocument=dict(uri=field_uri, text=field_source, version=1))))
            field_preview = server.query('chali/expandMacro', dict(textDocument=dict(uri=field_uri), mode='all'))
            assert not field_preview['diagnostics'], field_preview
            field_nodes = [node for entry in json.loads(field_preview['text'])['expansions'] for node in nodes(entry['tree'])]
            assert any(node.get('field', {}).get('name') == 'number' and node['field']['owner'].endswith('Value') for node in field_nodes), field_nodes
            assert any(node.get('parameterCache') == 'borrow' for node in field_nodes), field_nodes
        finally:
            server.close()
        assert not path.exists(), 'LSP 不应写入未保存文档'
        path.write_text(source, encoding='utf-8')
        result = subprocess.run([str(compiler), 'run', str(path), '--out', str(work / 'run')], cwd=ROOT, env=dict(os.environ, CHALI_LANG='zh'), capture_output=True, text=True, encoding='utf-8', timeout=240)
        assert (result.returncode, result.stdout, result.stderr) == (0, '', ''), result
        diagnostic = work / 'diagnostic.chl'
        diagnostic.write_text('$macro INNER()\n$compile_error("stop");\n$end_macro\n$macro OUTER()\n$INNER();\n$end_macro\nvoid main(){$OUTER();}', encoding='utf-8')
        outputs = {}
        for mode in ('short', 'full'):
            result = subprocess.run([str(compiler), 'check', str(diagnostic), '--macro-backtrace', mode], cwd=ROOT, env=dict(os.environ, CHALI_LANG='zh'), capture_output=True, text=True, encoding='utf-8', timeout=240)
            assert result.returncode != 0 and 'stop' in result.stderr and result.stdout == '', result
            outputs[mode] = result.stderr
        assert outputs['short'].count('展开宏 $') == 1 and outputs['full'].count('展开宏 $') >= 2, outputs
        assert '--macro-backtrace full' in outputs['short'], outputs
    print('生产宏一层／全部预览、值缓存、卫生符号、类型、中文位置、版本缓存与简短／完整诊断通过')


if __name__ == '__main__':
    main()
