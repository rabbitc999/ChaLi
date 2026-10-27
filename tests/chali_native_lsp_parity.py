"""把既有 LSP 能力测试逐项转发给原生服务，防止迁移时损失编辑功能。"""
from pathlib import Path
import sys
from compiler_path import compiler_path
from project_temp import tempfile
from native_lsp_transport import Transport, position

ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from chali_lsp import ApiIndex


class NativeServer:
    """仅负责测试中的传输适配；所有编辑请求由真实 C++ 服务处理。"""
    def __init__(self,compiler):
        self.documents={}
        self.api_index=ApiIndex()
        self.sent={}
        self.transport=Transport(compiler)

    def send(self,message):
        self.transport.send(message)

    def query(self,method,params):
        for uri,document in self.documents.items():
            if uri in self.sent and self.sent[uri][0]==document['text']:continue
            version=self.sent.get(uri, ('', 0))[1] + 1 if uri in self.sent else 1
            if uri not in self.sent:
                self.send(dict(method='textDocument/didOpen',params={'textDocument':dict(uri=uri,text=document['text'],version=version)}))
            else:
                self.send(dict(method='textDocument/didChange',params={'textDocument':dict(uri=uri,version=version),'contentChanges':[{'text':document['text']}]}))
            self.sent[uri]=(document['text'], version)
        return self.transport.query(method, params)

    def close(self):
        self.transport.close()


def main():
    """比较参考声明索引与原生悬停，并检查未保存模块中的泛型字段。"""
    with tempfile.TemporaryDirectory(prefix='chali-native-parity-') as folder:
        root = Path(folder)
        server = NativeServer(compiler_path())
        try:
            server.query('initialize', dict(rootUri=root.as_uri()))
            uri = (root / 'main.chl').as_uri()
            text = 'struct Box<T> { T item; }; int64 read(int64 x) { return x; } void main() { Box<int64> box; let result = box.item; result; }'
            server.documents[uri] = dict(text=text)
            index = ApiIndex()
            records = index.scan(uri, text)
            names = {item['name'] for item in [*records, *index.symbols[uri]]}
            symbols = server.query('textDocument/documentSymbol', dict(textDocument=dict(uri=uri)))
            assert {'Box', 'read', 'main'} <= names & {item['name'] for item in symbols}
            hover = server.query('textDocument/hover', dict(textDocument=dict(uri=uri), position=position(text, text.rindex('result') + 2)))
            assert hover and 'int64 result' in hover['contents']['value'], hover
            dep = (root / 'dep.chl').as_uri()
            server.documents[dep] = dict(text='module dep; struct Entry { bool flag; };')
            text = 'import dep; void main() { dep::Entry entry; entry.'
            server.documents[uri] = dict(text=text)
            completion = server.query('textDocument/completion', dict(textDocument=dict(uri=uri), position=position(text, len(text))))
            items = completion.get('items', []) if isinstance(completion, dict) else completion
            assert 'flag' in {item['label'] for item in items}, items
        finally:
            server.close()
    print('原生 LSP 声明索引对照、泛型字段与未保存依赖通过。')


if __name__=='__main__':
    main()
