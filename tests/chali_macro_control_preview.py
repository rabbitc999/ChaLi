"""验证宏控制语句的来源和真实出口导航，不公开内部控制节点编号。"""
import argparse
import json
from pathlib import Path
from compiler_path import compiler_path
from native_lsp_transport import Transport
from project_temp import tempfile


def walk(node):
    """遍历公开的展开树。"""
    if not isinstance(node,dict): return
    yield node
    for child in node.get('children',[]): yield from walk(child)


def main():
    """向实际语言服务器查询未保存源码，并核对调用方循环的位置。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=compiler_path())
    compiler=parser.parse_args().chali.resolve()
    source='''$macro WRAP(body:block) -> stmt
int64 once=0;
while(once<1){once+=1;body;}
$end_macro
void main()
{
    loop
    {
        $WRAP({break;});
    }
    return;
}
'''
    with tempfile.TemporaryDirectory(prefix='macro-control-preview-') as directory:
        folder=Path(directory)
        path=folder/'main.chl'
        server=Transport(compiler,str(folder))
        server.query('initialize',dict(rootUri=folder.as_uri()))
        try:
            server.send(dict(method='textDocument/didOpen',params=dict(textDocument=dict(uri=path.as_uri(),text=source,version=1))))
            for mode in ('one','all'):
                preview=server.query('chali/expandMacro',dict(textDocument=dict(uri=path.as_uri()),mode=mode))
                assert not preview['diagnostics'],preview
                tree=json.loads(preview['text'])
                nodes=[node for entry in tree['expansions'] for node in walk(entry['tree'])]
                exits=[node for node in nodes if node.get('kind')=='break']
                assert exits and all(node['origin']=='argument' for node in exits),nodes
                assert any(node.get('kind')=='while' and node['origin']=='definition' for node in nodes),nodes
                if mode=='all':
                    target=exits[0]['controlTarget']
                    assert Path(target['file']).resolve()==path.resolve() and target['line']==7,target
                assert 'controlContext' not in preview['text'] and 'bindingIdentity' not in preview['text'],preview
            assert not path.exists()
        finally: server.close()
    print('宏控制流预览：一层／全部来源、真实循环出口与只读导航通过')


if __name__=='__main__': main()
