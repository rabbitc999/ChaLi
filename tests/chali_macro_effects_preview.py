"""核对真实宏预览中的元数据导航、借用效果及实际控制出口分类。"""
import argparse
import json
from pathlib import Path
from compiler_path import compiler_path
from native_lsp_transport import Transport
from project_temp import tempfile


def walk(node):
    """遍历公开预览树，不依赖内部节点编号。"""
    if not isinstance(node,dict):return
    yield node
    for child in node.get('children',[]):yield from walk(child)


def main():
    """查询未保存文档的一层／全部展开并验证效果与来源。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=compiler_path())
    compiler=parser.parse_args().chali.resolve()
    source='''struct Cell{
int64 value;
}
$macro LABEL<T>() -> expr
($fields<T>())[0].name
$end_macro
$macro CHANGE(mut &value) -> stmt
value+=1;
$end_macro
$macro FN() -> expr
fn(){return 7;}
$end_macro
$macro WRAP(body:block) -> stmt
int64 round=0;
while(round<1){round+=1;body;}
$end_macro
void main(){
let label=$LABEL<Cell>();
int64 value=1;
$CHANGE(value);
let local=$FN();
loop{$WRAP({break;});}
assert(label=="value"&&value==2&&local()==7);
}
'''
    with tempfile.TemporaryDirectory(prefix='macro-effects-preview-') as directory:
        folder=Path(directory)
        path=folder/'main.chl'
        server=Transport(compiler,str(folder))
        server.query('initialize',dict(rootUri=folder.as_uri()))
        try:
            server.send(dict(method='textDocument/didOpen',params=dict(textDocument=dict(uri=path.as_uri(),text=source,version=1))))
            for mode in ('one','all'):
                preview=server.query('chali/expandMacro',dict(textDocument=dict(uri=path.as_uri()),mode=mode))
                assert not preview['diagnostics'],preview
                data=json.loads(preview['text'])
                entries={entry['name'].split('::')[-1]:entry['tree'] for entry in data['expansions']}
                assert {'LABEL','CHANGE','FN','WRAP'}<=entries.keys(),entries
                assert entries['CHANGE']['effects']['mutableBorrows']>0,entries['CHANGE']
                assert entries['WRAP']['effects']['introducedBindings']>0,entries['WRAP']
                assert not entries['WRAP']['effects']['truncated']
                for key in ('callerControl','localControl','unresolvedControl'):
                    assert 'return' not in entries['FN']['effects'][key],entries['FN']
                if mode=='all':
                    assert 'break' in entries['WRAP']['effects']['callerControl'],entries['WRAP']
                    locations=[node['metadataSource'] for node in walk(entries['LABEL']) if node.get('kind')=='string' and 'metadataSource' in node]
                    assert locations and any(Path(location['file']).resolve()==path.resolve() and location['line']==2 and location['column']>0 for location in locations),entries['LABEL']
                assert 'controlTargetId' not in preview['text'] and 'bindingIdentity' not in preview['text']
            assert not path.exists()
        finally:server.close()
    print('宏效果与来源预览：只读元数据导航、可写借用、调用方退出与局部函数边界通过')


if __name__=='__main__':main()
