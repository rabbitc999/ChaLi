"""用实际 LSP 检查 0.17.1 的 match 与插值类型推导。"""
from pathlib import Path
import tempfile
from magel_native_lsp_parity import NativeServer

ROOT=Path(__file__).resolve().parents[1]

def main():
    """auto 类型悬停与成员补全使用新表达式的结果类型。"""
    server=NativeServer(ROOT/'magel.exe')
    try:
        server.query('initialize',{})
        with tempfile.TemporaryDirectory(prefix='magel-lsp-251-') as directory:
            uri=(Path(directory)/'main.mgl').as_uri()
            def query(text,marker,method):
                cursor=text.rindex(marker)+len(marker);before=text[:cursor]
                server.documents[uri]=dict(text=text,version=1)
                position=dict(line=before.count('\n'),character=len(before.rsplit('\n',1)[-1].encode('utf-16-le'))//2)
                return server.query('textDocument/'+method,dict(textDocument=dict(uri=uri),position=position))
            for source,expected in [
                ('int64 main(){auto value=match(1){1=>{true}_=>{false}};value;}','bool value'),
                ('int64 main(){auto value=f"answer {1+2}";value;}','std::lang::String value'),
            ]:
                result=query(source,'value','hover')
                assert expected in result['contents']['value'],(source,result)
            source='struct Entry{int64 value;int64 read(){return value;}};Entry make(){Entry e;return e;}int64 main(){auto entry=match(1){1=>{make()}_=>{make()}};entry.'
            result=query(source,'entry.','completion')
            assert {row['label'] for row in result}=={'value','read'},result
    finally:server.close()
    print('LSP match 与插值的 auto 类型、成员补全回归通过。')

if __name__=='__main__':main()
