"""使用真实原生 LSP 验证命名警告、限定作用域补全、标准库内部符号隐藏与标准库类型字段文档。"""
import argparse
from compiler_path import compiler_path
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path
from threading import Barrier
import json
import subprocess
from project_temp import tempfile
import sys
from native_lsp_transport import Transport

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT/'tools'))


def main():
    """批量发送未保存文档，核对警告级别、直接成员过滤及悬停说明。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    requests = [dict(id=1, method='initialize', params={})]
    documents = {}
    sequence = 2

    def document(folder, name, text, method=None, marker=None):
        nonlocal sequence
        uri = (folder/name).as_uri()
        documents[name] = uri
        requests.append(dict(method='textDocument/didOpen', params=dict(
            textDocument=dict(uri=uri, text=text, version=1))))
        if not method:
            return None
        cursor = text.index(marker) + len(marker)
        before = text[:cursor]
        position = dict(line=before.count('\n'), character=len(before.rsplit('\n',1)[-1].encode('utf-16-le'))//2)
        current = sequence
        sequence += 1
        requests.append(dict(id=current, method='textDocument/'+method,
            params=dict(textDocument=dict(uri=uri), position=position)))
        return current

    with tempfile.TemporaryDirectory(prefix='chali-lsp-scopes-') as directory:
        folder = Path(directory)
        requests[0]['params'] = dict(rootUri=folder.as_uri(), capabilities={'textDocument': {'definition': {'linkSupport': True}}})
        (folder/'a').mkdir()
        (folder/'a/iter').mkdir()
        (folder/'a/iter/child.chl').write_text('module a::iter::child; int64 hidden(){return 1;}\n', encoding='utf-8')
        (folder/'a/iter.chl').write_text('module a::iter;\nstruct LocalType { int64 field; int64 method(){return 1;} };\nint64 localFunction(){return 1;}\n', encoding='utf-8')
        bad = '/* 😀 */ struct bad_type { int64 Bad_Method(){return 1;} }; int64 Bad_Function(){return 0;} void main(){}'
        document(folder, 'bad.chl', bad)
        good = 'struct GoodType { GoodType() {} int64 goodMethod(){return 1;} }; void main(){}'
        requests.append(dict(method='textDocument/didChange', params=dict(
            textDocument=dict(uri=documents['bad.chl'], version=2), contentChanges=[dict(text=good)])))
        document(folder, 'good.chl', good)
        invalid_module = '/* 😀 */ module useful::BadName; void main(){}'
        document(folder, 'bad-module.chl', invalid_module)
        requests.append(dict(method='textDocument/didChange', params=dict(
            textDocument=dict(uri=documents['bad-module.chl'], version=2),
            contentChanges=[dict(text='module useful::good_name; void main(){}')])))
        requests.append(dict(method='textDocument/didChange', params=dict(
            textDocument=dict(uri=documents['bad-module.chl'], version=3),
            contentChanges=[dict(text='#NoNamingCheck // 使用这个指令\n' + invalid_module)])))

        imports = document(folder, 'imports.chl', 'import std::', 'completion', 'std::')
        import_root = document(folder, 'import-root.chl', 'import ', 'completion', 'import ')
        import_local = document(folder, 'import-local.chl', 'import a::iter::', 'completion', 'a::iter::')
        std = document(folder, 'std.chl', 'void main(){std::', 'completion', 'std::')
        # `__` 前缀是编译器/宿主协议：标准库模块补全一个都不该带出来。
        modules = ('std::core', 'std::iter', 'std::math', 'std::number_theory', 'std::numerics',
                   'std::text', 'std::path', 'std::date_time', 'std::linear_algebra', 'std::macro',
                   'std::gui', 'std::audio', 'std::xml', 'std::crypto')
        private = {module: document(folder, f'private-{index}.chl',
                    f'import {module};\nvoid main(){{ {module}::', 'completion', module + '::')
                   for index, module in enumerate(modules)}
        macro = document(folder, 'std-macro.chl',
            'import std::macro;\nvoid main(){ $std::macro::', 'completion', '$std::macro::')
        function = document(folder, 'function.chl',
            'import std::date_time;\nvoid main(){DateTime value;let output=std::date_time::formatUtc(value);}',
            'definition', 'std::date_time::formatUt')
        local = document(folder, 'local.chl', 'import a::iter;\nvoid main(){a::iter::', 'completion', 'a::iter::')
        unrelated = document(folder, 'unrelated.chl', 'import a::iter;\nvoid main(){b::iter::', 'completion', 'b::iter::')
        member = document(folder, 'member.chl', 'import a::iter;\nvoid main(){a::iter::LocalType::', 'completion', 'LocalType::')
        field = document(folder, 'field.chl', 'import std::date_time;\nvoid main(){DateTime date; date.year; return;}', 'hover', 'date.year')
        type_hover = document(folder, 'type.chl', 'import std::linear_algebra;\nvoid main(){LuFactorization value;return;}', 'hover', 'LuFactorization')
        constant_source = 'import std::math;\nvoid main(){float64 value=std::math::PI;}'
        constant_hover = document(folder, 'constant-hover.chl', constant_source, 'hover', 'std::math::P')
        constant_definition = document(folder, 'constant-definition.chl', constant_source, 'definition', 'std::math::P')
        contextual = 'void main(){int64 root=1; int64 copy=root; println(copy); let f=fn copy(){return copy;};}'
        coloring = document(folder, 'contextual.chl', contextual, 'semanticTokens/full', 'println')
        local_link = document(folder, 'local-link.chl', contextual, 'definition', 'println(cop')
        requests += [dict(id=sequence, method='shutdown'), dict(method='exit')]
        server = Transport(compiler, folder)
        try:
            for request in requests[:-2]:
                server.send(request)
                method=request.get('method')
                item=request.get('params',{}).get('textDocument',{})
                if method=='textDocument/didOpen' and item.get('uri') in (documents['bad.chl'],documents['good.chl'],documents['bad-module.chl']):
                    server.diagnostic(item['uri'],item['version'])
                if method=='textDocument/didChange' and item.get('uri') in (documents['bad.chl'],documents['bad-module.chl']):
                    server.diagnostic(item['uri'],item['version'])
            responses={}
            for request in requests[:-2]:
                if 'id' in request:
                    response=server.response(request['id']);assert 'error' not in response,response
                    responses[request['id']]=response['result']
            diagnostics={}
            for message in server.notifications:
                if message.get('method')=='textDocument/publishDiagnostics':
                    diagnostics.setdefault(message['params']['uri'],[]).append(message['params']['diagnostics'])
        finally:server.close()
        warnings = diagnostics[documents['bad.chl']][0]
        assert len(warnings) == 3 and all(d['severity']==2 for d in warnings), warnings
        assert {d['code'] for d in warnings} == {'CHL2101','CHL2102'}
        for warning in warnings:
            assert warning['data']['fixes'], warning
            span = warning['range']
            name = bad.encode('utf-16-le')[span['start']['character']*2:span['end']['character']*2].decode('utf-16-le')
            assert name in ('bad_type', 'Bad_Method', 'Bad_Function'), warning
        assert diagnostics[documents['bad.chl']][-1] == [], diagnostics[documents['bad.chl']]
        assert diagnostics[documents['good.chl']][-1] == [], diagnostics[documents['good.chl']]
        module_warnings = diagnostics[documents['bad-module.chl']]
        assert len(module_warnings[0]) == 1 and module_warnings[0][0]['code'] == 'CHL2103', module_warnings
        assert module_warnings[0][0]['severity'] == 2 and module_warnings[1:] == [[], []], module_warnings
        span = module_warnings[0][0]['range']
        source = invalid_module.encode('utf-16-le')
        assert source[span['start']['character']*2:span['end']['character']*2].decode('utf-16-le') == 'useful::BadName', span

        labels = lambda key: {r['label'] for r in responses[key]}
        assert all(r['kind']==9 for r in responses[imports]+responses[import_root]+responses[import_local])
        assert {'std','a'} <= labels(import_root), responses[import_root]
        assert labels(import_local)=={'child'}, responses[import_local]
        assert 'core' not in labels(imports) and 'iter' in labels(imports), responses[imports]
        assert 'core' in labels(std) and 'iter' in labels(std) and 'pause' not in labels(std)
        for module, request in private.items():
            leaked = sorted(label for label in labels(request) if label.startswith('__'))
            assert not leaked, (module, leaked)
        assert 'range' in labels(private['std::core']) and 'abs' in labels(private['std::math'])
        assert 'intIter' in labels(private['std::iter']) and 'range' not in labels(private['std::iter'])
        for module, expected, old in (
            ('std::number_theory', {'gcd', 'powMod', 'modInverse'}, {'pow_mod', 'mod_inverse'}),
            ('std::date_time', {'formatUtc', 'parseUtc'}, {'format_utc', 'parse_utc'}),
            ('std::linear_algebra', {'luDecompose', 'symmetricEigen'}, {'lu_decompose', 'symmetric_eigen'}),
            ('std::gui', {'openWindow', 'beginFrame', 'controlCount', 'textBox'}, {'open_window', 'begin_frame', 'control_count', 'abi'}),
            ('std::audio', {'isPlaying', 'load', 'close'}, {'is_playing', 'abi'}),
            ('std::xml', {'XmlNode', 'escapeText', 'escapeAttribute'}, {'escape_text', 'escape_attribute', 'abi'}),
            ('std::crypto', {'randomHex', 'sha256', 'hmacSha256', 'verifyEd25519'}, {'random_hex', 'hmac_sha256', 'verify_ed25519', 'abi'}),
        ):
            actual = labels(private[module])
            assert expected <= actual and not old & actual, (module, actual)
        tokens = {}
        line = column = 0
        data = responses[coloring]['data']
        for delta_line, delta_column, length, kind, _ in zip(*(iter(data),)*5):
            column = delta_column if delta_line else column + delta_column
            line += delta_line
            if line == 0:
                tokens[column] = (contextual[column:column+length], kind)
        assert [kind for word, kind in tokens.values() if word == 'root'] == [4, 4], tokens
        assert [kind for word, kind in tokens.values() if word == 'copy'] == [4, 4, 7, 4], tokens
        local_target = responses[local_link][0]
        assert local_target['targetUri'] == documents['local-link.chl'], local_target
        assert local_target['targetSelectionRange']['start']['character'] == contextual.index('copy='), local_target
        constants = {item['label']: item for item in responses[private['std::math']] if item['kind'] == 21}
        assert {'PI', 'E'} <= constants.keys(), constants
        assert '圆周率' in str(responses[constant_hover]), responses[constant_hover]
        target = responses[constant_definition]
        if isinstance(target, list):
            target = target[0]
        assert target['targetUri'].endswith('/std/math.chl'), target
        expected_macros = {'$MIN', '$MAX', '$CLAMP', '$Equal', '$Hash', '$Clone', '$Debug'}
        assert labels(macro) == expected_macros and len(responses[macro]) == len(expected_macros), responses[macro]
        destination = responses[function]
        if isinstance(destination, list):
            assert len(destination) == 1, destination
            destination = destination[0]
        assert destination and destination['targetUri'].startswith('chali-stdlib:'), destination
        assert labels(local) == {'LocalType','localFunction','child'}, responses[local]
        assert labels(unrelated) == set(), responses[unrelated]
        assert labels(member) == {'field','method'}, responses[member]
        assert '公历年份' in responses[field]['contents']['value'], responses[field]
        assert 'LU 分解结果' in responses[type_hover]['contents']['value'], responses[type_hover]

        barrier = Barrier(2)

        def concurrent_diagnostics(index):
            """两个原生语言服务同时检查独立文档，复现进程间临时目录竞争。"""
            client = Transport(compiler, folder)
            try:
                client.query('initialize', {'rootUri': folder.as_uri()})
                uri = (folder / f'concurrent-{index}.chl').as_uri()
                source = 'module shared::BadName; void main() {}'
                barrier.wait(timeout=30)
                client.send(dict(method='textDocument/didOpen', params=dict(
                    textDocument=dict(uri=uri, text=source, version=1))))
                result = client.diagnostic(uri, 1, timeout=90)
                assert [entry.get('code') for entry in result['diagnostics']] == ['CHL2103'], result
            finally:
                client.close()

        with ThreadPoolExecutor(max_workers=2) as pool:
            list(pool.map(concurrent_diagnostics, range(2)))

    print('LSP 命名警告、构造器豁免、模块及类型作用域、类型与字段文档、标准库内部符号隐藏全部通过。')


if __name__ == '__main__':
    main()
