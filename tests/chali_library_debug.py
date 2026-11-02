"""验证预编译库可选调试符号、实现身份及重优化约束。"""
import argparse
import copy
import hashlib
import os
from pathlib import Path
from compiler_path import compiler_path
from project_temp import tempfile
from chali_library_package import invoke, decode, encode


def main():
    """删除提供者源码后消费双实现包，并拒绝符号与实现不一致的包。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    args = parser.parse_args()
    compiler = args.chali.resolve()
    with tempfile.TemporaryDirectory(prefix='library-debug-') as directory:
        work = Path(directory)
        modules = work / 'modules/demo'
        modules.mkdir(parents=True)
        provider = work / 'provider.chl'
        package = modules / 'math.chlib'
        consumer = work / 'main.chl'
        consumer.write_text('import demo::math;\nvoid main(){assert(demo::math::twice(6)==12);println("debug library OK");}', encoding='utf-8')
        for optimized in [False, True]:
            provider.write_text('module demo::math;\nint64 twice(int64 input){\n int64 value=input*2;\n return value;\n}\n', encoding='utf-8')
            invoke([compiler, 'lib', 'build', provider, '--module', 'demo::math', '--package-version', '1.0.0',
                    '--with-bitcode', '--debug-optimized' if optimized else '--debug', '--out', package], work)
            manifest, entries = decode(package.read_bytes())
            target = manifest['targets'][0]
            descriptor = manifest['extensions']['nativeDebug'][target['triple']]
            assert descriptor['optimized'] == optimized and descriptor['format'] == 1
            assert descriptor['prefix'] == 'chali_debug_' + hashlib.sha256(b'demo::math').hexdigest()
            for field, payload in [('implementationSha256', 'implementation'), ('bitcodeSha256', 'bitcode')]:
                actual = next(entry['data'] for entry in entries if entry['name'] == target[payload])
                assert descriptor[field] == hashlib.sha256(actual).hexdigest()
            original = package.read_bytes()
            provider.unlink()
            for implementation in ['native', 'bitcode']:
                for mode in ['off', 'full', 'thin']:
                    result = invoke([compiler, 'run', consumer, '--debug', '--library-implementation', implementation, '--lto-mode', mode], work)
                    assert 'debug library OK' in result.stdout
            output = work / ('rejected.exe' if os.name == 'nt' else 'rejected')
            for field, value in [('format', 2), ('prefix', 'chali_debug_' + '0' * 64),
                                 ('optimized', 'false'), ('implementationSha256', '0' * 64), ('bitcodeSha256', '0' * 64)]:
                wrong = copy.deepcopy(manifest)
                wrong['extensions']['nativeDebug'][target['triple']][field] = value
                package.write_bytes(encode(wrong, entries))
                output.write_bytes(b'preserve existing output')
                invoke([compiler, 'aot', consumer, '--debug', '--out', output], work, False)
                assert output.read_bytes() == b'preserve existing output'
            package.write_bytes(original)
            for debug_flag in [[], ['--debug-optimized']]:
                command = [compiler, 'aot', consumer, '--library-implementation', 'bitcode', '--out', output] + debug_flag
                result = invoke(command, work, optimized)
                if not optimized:
                    assert 'bitcode' in result.stderr and 'O3' in result.stderr
            print('库调试身份、无源码消费与原生/bitcode 三种 LTO：' + ('O3' if optimized else 'O0') + ' 通过')


if __name__ == '__main__':
    main()
