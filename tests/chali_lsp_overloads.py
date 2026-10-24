"""通过真实语言服务验证函数及宏的重载筛选、选择保留和独立详情。"""
from compiler_path import compiler_path
import argparse
from pathlib import Path
from project_temp import tempfile
from native_lsp_transport import Transport, position

ROOT = Path(__file__).resolve().parents[1]

FUNCTIONS = '''/// 整数重载文档
int64 choose(int64 value) { return value; }
/// 字符串重载文档
String choose(String value) { return value; }
int64 defaults(int64 first, int64 second = 7) { return first + second; }
int64 many(int64 first, int64 ...rest) { return first; }
T identity<T>(T value) { return value; }
'''
MACROS = '''/// 单参数宏文档
$macro report(value)
value
$end_macro
/// 双参数宏文档
$macro report(left, right)
left + right
$end_macro
$macro report[items]
items
$end_macro
$replace plus(left, right) left + right
'''


def main():
    """在不完整文档上逐次请求签名帮助，核对可见候选和当前实参。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    with tempfile.TemporaryDirectory(prefix='chali-overloads-') as directory:
        folder = Path(directory)
        server = Transport(compiler, directory)
        server.query('initialize', {'rootUri': folder.as_uri()})
        serial = 0

        def request(source, context=None, method='textDocument/signatureHelp', at=None):
            """打开独立缓冲区，避免上一次的诊断或候选缓存影响本次请求。"""
            nonlocal serial
            serial += 1
            uri = (folder / f'case{serial}.chl').as_uri()
            server.send({'method': 'textDocument/didOpen', 'params': {
                'textDocument': {'uri': uri, 'text': source, 'version': 1}}})
            params = {'textDocument': {'uri': uri}, 'position': position(source, len(source) if at is None else at)}
            if context is not None:
                params['context'] = {'triggerKind': 3, 'isRetrigger': True, 'activeSignatureHelp': context}
            return server.query(method, params)

        def signatures(result, count, kind='函数'):
            """核对每个候选自身的计数标签，不把多个说明合并进同一条。"""
            assert result and len(result['signatures']) == count, result
            for index, signature in enumerate(result['signatures']):
                assert signature['label'].startswith(f'{kind} [{index + 1}/{count}] '), result
            return result['signatures']

        try:
            prefix = FUNCTIONS + 'void main() { choose('
            both = request(prefix)
            items = signatures(both, 2)
            assert '整数重载文档' in items[0]['documentation']['value'], both
            assert '字符串重载文档' not in items[0]['documentation']['value'], both
            assert '字符串重载文档' in items[1]['documentation']['value'], both
            both['activeSignature'] = 1
            selected = request(prefix, both)
            signatures(selected, 2)
            assert selected['activeSignature'] == 1, selected
            assert 'int64 value' in signatures(request(prefix + '42'), 1)[0]['label']
            assert 'String value' in signatures(request(prefix + '"hello"'), 1)[0]['label']
            # 参数修改后原来的选择已被筛掉，自动选择剩余候选。
            narrowed = request(prefix + '42', both)
            signatures(narrowed, 1)
            assert narrowed['activeSignature'] == 0, narrowed
            signatures(request(FUNCTIONS + 'void main() { defaults(1, '), 1)
            variadic = request(FUNCTIONS + 'void main() { many(1, 2, ')
            signatures(variadic, 1)
            assert variadic['signatures'][0]['activeParameter'] == 1, variadic
            signatures(request(FUNCTIONS + 'void main() { identity("x"'), 1)
            complete = FUNCTIONS + 'void main() { choose("hello"); }'
            hover = request(complete, method='textDocument/hover', at=complete.rfind('choose') + 2)
            value = hover['contents']['value']
            assert '字符串重载文档' in value and '整数重载文档' not in value, hover
            macro_prefix = MACROS + 'void main() { $report('
            macro_both = request(macro_prefix)
            signatures(macro_both, 2, '宏')
            macro_both['activeSignature'] = 1
            kept = request(macro_prefix, macro_both)
            assert kept['activeSignature'] == 1, kept
            macro_one = request(macro_prefix + '1, ')
            assert 'left, right' in signatures(macro_one, 1, '宏')[0]['label'], macro_one
            brackets = request(MACROS + 'void main() { $report[')
            assert '[items]' in signatures(brackets, 1, '宏')[0]['label'], brackets
            signatures(request(MACROS + 'void main() { $plus(1, '), 1, '宏')
            complete = MACROS + 'void main() { $report(1, 2); }'
            hover = request(complete, method='textDocument/hover', at=complete.rfind('$report') + 2)
            value = hover['contents']['value']
            assert '双参数宏文档' in value and '单参数宏文档' not in value, hover
        finally:
            server.close()
    print('LSP 重载：独立说明、候选计数、类型与数量筛选、选择保留、默认/变长/泛型、不完整输入及宏分组通过')


if __name__ == '__main__':
    main()
