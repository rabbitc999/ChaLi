"""用真实原生 LSP 验证 if / ? 成功类型、泛型错误适配与传播悬停。"""
from pathlib import Path
import tempfile
from magel_native_lsp_parity import NativeServer

ROOT=Path(__file__).resolve().parents[1]


def main():
    """编辑器表达式推导必须与条件和传播的成功值保持一致。"""
    server=NativeServer(ROOT/'magel.exe')
    prelude='''struct Entry { bool flag; int64 read(){return 1;} };
Entry makeEntry(){Entry entry;return entry;}
Result<Entry,String> load(){return Result<Entry,String>::Ok(makeEntry());}
Result<Entry,int64> raw(){return Result<Entry,int64>::Ok(makeEntry());}
String toText(int64 error){return "error";}
'''
    try:
        with tempfile.TemporaryDirectory(prefix='magel-lsp-250-') as directory:
            uri=(Path(directory)/'main.mgl').as_uri()
            server.query('initialize',{})
            def query(body,marker,method='completion'):
                text=prelude+body;cursor=text.rindex(marker)+len(marker);before=text[:cursor]
                server.documents[uri]=dict(text=text,version=1)
                position=dict(line=before.count('\n'),character=len(before.rsplit('\n',1)[-1].encode('utf-16-le'))//2)
                return server.query('textDocument/'+method,dict(textDocument=dict(uri=uri),position=position))
            for body,marker in [
                ('int64 main(){auto entry=if(true){makeEntry()}else{makeEntry()};entry.','entry.'),
                ('Result<int64,String> use(){auto entry=load()?;entry.','entry.'),
                ('Result<int64,String> use(){load()?.','load()?.'),
                ('Result<int64,String> use(){auto entry=raw().mapErr<String>(toText)?;entry.','entry.'),
            ]:
                result=query(body,marker)
                assert {r['label'] for r in result}=={'flag','read'},(body,result)
            for body,expected in [
                ('int64 main(){auto value=if(true){1}else{2};value;}','int64 value'),
                ('int64 main(){auto value=if(true){true}else{bool other=false;break other;};value;}','bool value'),
                ('Result<int64,String> use(){auto value=load()?;value;}','Entry value'),
            ]:
                result=query(body,'value','hover')
                assert expected in result['contents']['value'],(body,result)
            result=query('Result<int64,String> use(){auto entry=load()?;return Result<int64,String>::Ok(1);}','load()?','hover')
            text=result['contents']['value']
            assert 'Entry' in text and 'ErrorResidual' in text and 'use' in text,(text,result)
    finally:server.close()
    print('LSP 条件与传播表达式类型、auto、成员及传播悬停全部通过。')


if __name__=='__main__':main()
