"""核对双实现库包的显式选择、禁用 LTO 物化及兼容性拒绝。"""
import argparse
import copy
import os
from pathlib import Path
from compiler_path import compiler_path
from project_temp import tempfile
from chali_library_package import invoke, decode, encode
from chali_library_bitcode_header import model


def main():
    """使用相同接口的不同实现证明选择生效，并保留失败前的输出。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--external-clang', type=Path, help='额外核对显式外置 LLVM 生产与消费路径')
    args = parser.parse_args()
    compiler = args.chali.resolve()
    with tempfile.TemporaryDirectory(prefix='library-bitcode-') as directory:
        work = Path(directory)
        modules = work / 'modules/demo'
        modules.mkdir(parents=True)
        provider = work / 'provider.chl'
        package = modules / 'bits.chlib'
        snapshots = []
        for number in [111, 222]:
            provider.write_text('module demo::bits;\nint64 answer(){return ' + str(number) + ';}\n', encoding='utf-8')
            invoke([compiler, 'lib', 'build', provider, '--module', 'demo::bits', '--package-version', '1.0.0',
                    '--with-bitcode', '--out', package], work)
            snapshots.append(decode(package.read_bytes()))
        manifest, entries = snapshots[0]
        target = manifest['targets'][0]
        assert target['bitcode'] and target['llvmVersion'] and target['dataLayout']
        other_manifest, other_entries = snapshots[1]
        assert manifest == other_manifest
        # 接口和单元布局保持一致，只替换 bitcode；消费者结果必须反映显式选择。
        second = {entry['name']: entry for entry in other_entries}
        for entry in entries:
            if entry['kind'] not in [3, 4, 5]:
                assert entry == second[entry['name']], entry['name']
        entries = [second[entry['name']] if entry['kind'] == 5 else entry for entry in entries]
        original = encode(manifest, entries)
        package.write_bytes(original)
        provider.unlink()
        consumer = work / 'main.chl'
        for mode, expected in [('native', 111), ('bitcode', 222)]:
            consumer.write_text('import demo::bits;\nvoid main(){assert(demo::bits::answer()==' + str(expected) + ');println("selected OK");}', encoding='utf-8')
            for lto in ['--lto', '--no-lto']:
                for memory in ['malloc', 'pool']:
                    result = invoke([compiler, 'run', consumer, '--library-implementation', mode, lto, '--allocator', memory], work)
                    assert 'selected OK' in result.stdout
            invoke([compiler, 'aot', consumer, '--emit-obj', '--out', work / 'consumer', '--library-implementation', mode], work)
            output = work / ('linked.exe' if os.name == 'nt' else 'linked')
            for lto in ['--lto', '--no-lto']:
                invoke([compiler, 'link', work / 'consumer.o', '--package', package, '--library-implementation', mode, lto, '--out', output], work)
                assert 'selected OK' in invoke([output], work).stdout
        output = work / ('rejected.exe' if os.name == 'nt' else 'rejected')

        def rejected(metadata, payloads, mode='bitcode'):
            """失败不得发布或覆盖既有产物。"""
            package.write_bytes(encode(metadata, payloads))
            output.write_bytes(b'preserve existing output')
            invoke([compiler, 'aot', consumer, '--library-implementation', mode, '--out', output], work, False)
            assert output.read_bytes() == b'preserve existing output'

        wrong_version = copy.deepcopy(manifest)
        wrong_version['targets'][0]['llvmVersion'] = '0.0.0'
        rejected(wrong_version, entries)
        for fields in [dict(triple='aarch64-unknown-linux-gnu'), dict(layout='e-p:32:32'), dict(producer='LLVM0.0.0')]:
            broken_header = copy.deepcopy(entries)
            parameters = dict(triple=target['triple'], layout=target['dataLayout'], producer='LLVM' + target['llvmVersion'])
            parameters.update(fields)
            next(entry for entry in broken_header if entry['kind'] == 5)['data'] = model(**parameters)
            rejected(manifest, broken_header)
        broken = copy.deepcopy(entries)
        next(entry for entry in broken if entry['kind'] == 5)['data'] = b'not bitcode'
        rejected(manifest, broken)
        # 默认原生路径不读取未选择的可选 bitcode。
        package.write_bytes(encode(manifest, broken))
        consumer.write_text('import demo::bits;\nvoid main(){assert(demo::bits::answer()==111);}', encoding='utf-8')
        invoke([compiler, 'run', consumer], work)
        package.write_bytes(original)
        native_only = copy.deepcopy(manifest)
        native_only['targets'][0]['bitcode'] = ''
        native_only['targets'][0]['llvmVersion'] = ''
        rejected(native_only, [entry for entry in entries if entry['kind'] != 5])
        bitcode_only = copy.deepcopy(manifest)
        bitcode_only['targets'][0]['implementation'] = ''
        only_entries = [entry for entry in entries if entry['name'] != target['implementation']]
        rejected(bitcode_only, only_entries, 'native')
        package.write_bytes(encode(bitcode_only, only_entries))
        consumer.write_text('import demo::bits;\nvoid main(){assert(demo::bits::answer()==222);}', encoding='utf-8')
        invoke([compiler, 'run', consumer, '--library-implementation', 'bitcode'], work)
        package.write_bytes(original)
        project = work / 'chali.toml'
        project.write_text('[aot]\nlibrary_implementation = "bitcode"\n', encoding='utf-8')
        consumer.write_text('import demo::bits;\nvoid main(){assert(demo::bits::answer()==222);}', encoding='utf-8')
        invoke([compiler, 'run', consumer], work)
        consumer.write_text('import demo::bits;\nvoid main(){assert(demo::bits::answer()==111);}', encoding='utf-8')
        invoke([compiler, 'run', consumer, '--library-implementation', 'native'], work)
        project.unlink()
        invoke([compiler, 'check', consumer, '--library-implementation', 'automatic'], work, False)
        provider.write_text('module demo::bridge;\nimport demo::bits;\nint64 answer(){return demo::bits::answer();}', encoding='utf-8')
        bridge = modules / 'bridge.chlib'
        invoke([compiler, 'lib', 'build', provider, '--module', 'demo::bridge', '--package-version', '1.0.0',
                '--with-bitcode', '--out', bridge], work)
        provider.unlink()
        for mode, expected in [('native', 111), ('bitcode', 222)]:
            consumer.write_text('import demo::bridge;\nvoid main(){assert(demo::bridge::answer()==' + str(expected) + ');}', encoding='utf-8')
            for lto in ['--lto', '--no-lto']:
                invoke([compiler, 'run', consumer, '--library-implementation', mode, lto], work)
        if args.external_clang:
            clang = args.external_clang.resolve()
            provider.write_text('module demo::bits;\nint64 answer(){return 333;}', encoding='utf-8')
            invoke([compiler, 'lib', 'build', provider, '--module', 'demo::bits', '--package-version', '1.0.0',
                    '--with-bitcode', '--llvm', clang, '--out', package], work)
            provider.unlink()
            consumer.write_text('import demo::bits;\nvoid main(){assert(demo::bits::answer()==333);}', encoding='utf-8')
            for lto in ['--lto', '--no-lto']:
                invoke([compiler, 'run', consumer, '--library-implementation', 'bitcode', '--llvm', clang, lto], work)
        print('bitcode selection: native/bitcode, full/off, malloc/pool, independent link and rejection OK')


if __name__ == '__main__':
    main()
