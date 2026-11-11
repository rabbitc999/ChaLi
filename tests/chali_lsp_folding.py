"""真实 LSP 验证嵌套块、词法隔离、未闭合块和缓冲区更新。"""
from pathlib import Path
import argparse
from compiler_path import compiler_path
from native_lsp_transport import Transport


def main():
    """直接检查协议区间及未保存文档的新版本。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    transport = Transport(parser.parse_args().chali.resolve())
    try:
        capabilities = transport.query('initialize')['capabilities']
        assert capabilities['foldingRangeProvider'] is True
        uri = 'untitled:folding.chl'
        source = 'fn main() {\n  if true {\n    print("}"); // {\n  }\n  /* {\n  } */\n}\nfn empty() {}\nfn unfinished() {\n  print("你好😀");'
        transport.send(dict(method='textDocument/didOpen', params=dict(textDocument=dict(uri=uri, languageId='chali', version=1, text=source))))
        params = dict(textDocument=dict(uri=uri))
        assert transport.query('textDocument/foldingRange', params) == [dict(startLine=0, endLine=5), dict(startLine=1, endLine=2), dict(startLine=8, endLine=9)]
        transport.send(dict(method='textDocument/didChange', params=dict(textDocument=dict(uri=uri, version=2), contentChanges=[dict(text='fn main() {}')])) )
        assert transport.query('textDocument/foldingRange', params) == []
    finally:
        transport.close()
    print('LSP folding passed')


if __name__ == '__main__':
    main()
