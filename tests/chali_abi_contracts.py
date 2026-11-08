"""验证原生 ABI 返回值所有权、同步回调、有限头文件导入与失败清理。"""
from compiler_path import compiler_path
import argparse
import os
from pathlib import Path
import subprocess
from project_temp import tempfile
ROOT = Path(__file__).resolve().parents[1]


def main():
    """构建原生测试库并通过实际编译器验证 ABI 合约。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--cc', default=os.environ.get('CHALI_ABI_CC', 'gcc'))
    parser.add_argument('--aot', action='store_true', help='验证原生产物中的所有权、闭包和同步 C 回调')
    args = parser.parse_args()
    compiler = args.chali.resolve()
    with tempfile.TemporaryDirectory(prefix='chali abi contracts ') as directory:
        work = Path(directory)
        env = dict(os.environ, CHALI_ABI_CACHE_DIR=str(work / 'cache'),
                   CHALI_LLVM_CLANG=(str(ROOT / 'build/dependencies/llvm/toolchain/bin/clang.exe') if os.name == 'nt' else os.environ.get('CHALI_LLVM_CLANG', 'clang')))

        def invoke(command):
            """执行外部命令，失败时保留完整诊断。"""
            command = list(command)
            if len(command) > 1 and str(command[1]) == 'run' and '--out' not in command:
                command += ['--out', work / ('run-' + Path(command[2]).stem)]
            result = subprocess.run(list(map(str, command)), cwd=ROOT, env=env,
                                    capture_output=True, timeout=600)
            assert result.returncode == 0, (command, result.stdout, result.stderr)
            return result.stdout

        for option in ('--abi-prepare', '--abi-cache', '--abi-bridge', '--abi-write-bridge', '--abi-no-cache'):
            result = subprocess.run([str(compiler), option], cwd=work, env=dict(env, CHALI_LANG='zh'),
                                    capture_output=True, encoding='utf-8', timeout=60)
            assert result.returncode == 2 and option in result.stderr and '已删除' in result.stderr, result
        assert not (work / '.chali').exists() and not (work / 'cache').exists()

        library = work / ('native.dll' if os.name == 'nt' else 'native.so')
        provider = work / 'native.c'
        provider.write_text('#include "' + (ROOT / 'doc/examples/abi_contracts/native.c').as_posix() + '"\n'
                            '/**\n * @brief 验证默认宿主符号的缩写和大小写转换。\n'
                            ' * @param value 输入值。\n * @return 原样返回输入值。\n */\n'
                            'API int32_t chali_program_read_http_value(int32_t value){return value;}\n', encoding='utf-8')
        invoke([args.cc, '-shared', '-fPIC', provider, '-o', library])
        convention = work / 'convention.chl'
        convention.write_text('#abi("C", "' + library.as_posix() + '", "") '
                              'int32 readHTTPValue(int32 value); '
                              'void main(){println("{}",readHTTPValue((int32)42));}', encoding='utf-8')
        assert invoke([compiler, 'run', convention]).strip() == b'42'
        unicode_library = work / ('原生 入口' + library.suffix)
        unicode_library.write_bytes(library.read_bytes())
        for label, path, symbol, message in (
            ('unicode', unicode_library, 'identity', None),
            ('missing-library', work / ('absent' + library.suffix), 'identity', '无法加载动态库'),
            ('missing-symbol', library, '__absent_native_symbol__', '动态库中找不到 C 符号')):
            probe = work / (label + '.chl')
            probe.write_text('#abi("C", "' + path.as_posix() + '", "' + symbol + '") '
                             'int32 scalar(int32 value); void main(){println("{}",scalar((int32)42));}', encoding='utf-8')
            result = subprocess.run([str(compiler), 'run', str(probe), '--out', str(work / label)],
                                    cwd=ROOT, env=dict(env, CHALI_LANG='zh'), capture_output=True,
                                    encoding='utf-8', timeout=120)
            if message is None:
                assert (result.returncode, result.stdout, result.stderr) == (0, '42\n', ''), result
            else:
                assert result.returncode != 0 and message in result.stdout + result.stderr, result

        source = work / 'demo.chl'
        source.write_text((ROOT / 'doc/examples/abi_contracts/demo.chl').read_text(encoding='utf-8')
                          .replace('./native.dll', library.as_posix()), encoding='utf-8')
        if args.aot:
            output = work / 'contracts'
            invoke([compiler, 'build', source, '--out', output])
            assert invoke([str(output) + ('.exe' if os.name == 'nt' else '')]).strip() == b'ABI contracts OK'
            print('BUILD C ABI 返回所有权、闭包与嵌套同步回调通过')
        else:
            assert invoke([compiler, 'run', source]).strip() == b'ABI contracts OK'
        imported = work / 'imported.chl'
        invoke([compiler, 'gen', 'abi', ROOT / 'doc/examples/abi_contracts/import.h',
                '--library', library.as_posix(), '--out', imported])
        with imported.open('a', encoding='utf-8') as stream:
            stream.write('\nvoid main(){assert(c_identity((int32)7)==C_FIRST);'
                         'let cb=int32 fn(int32 x){return x;};'
                         'assert(c_apply(cb,(int32)3)==7);return;}\n')
        invoke([compiler, 'run', imported])
        for label, body, expected_failure in (
            ('direct', 'panic("callback failure");return x;', b'callback failure'),
            ('nested', 'return apply(int32 fn(int32 y){panic("callback failure");return y;},x);', b'callback failure'),
            ('binary', 'uint8 data[1];data[0]=(uint8)2;std::binary::BinaryReader reader=std::binary::BinaryReader::fromArray(data,1);reader.readBool();return x;',
             '二进制读取：bool 字节必须为 0 或 1'.encode('utf-8'))):
            failure = work / ('callback-failure-' + label + '.chl')
            failure.write_text(('import std::binary;\n' if label == 'binary' else '') + '#abi("C", "' + library.as_posix() + '", "callback_owned") '
                               'String owned(int32 fn(int32) cb) ["free_owned"]; '
                               '#abi("C", "' + library.as_posix() + '", "apply") '
                               'int32 apply(int32 fn(int32) cb,int32 x); '
                               'void main(){let cb=int32 fn(int32 x){' + body + '};'
                               'String text=owned(cb);}', encoding='utf-8')
            marker = work / ('released-' + label + '.txt')
            env['CHALI_ABI_FREE_MARKER'] = str(marker)
            if args.aot:
                prefix = work / ('failure-' + label)
                invoke([compiler, 'build', failure, '--checked', '--out', prefix])
                command = [str(prefix) + ('.exe' if os.name == 'nt' else '')]
            else:
                command = [str(compiler), 'run', str(failure), '--out', str(work / ('run-failure-' + label))]
            result = subprocess.run(command, cwd=ROOT, env=env, capture_output=True, timeout=600)
            assert result.returncode != 0 and expected_failure in result.stderr + result.stdout, result
            assert marker.read_text().strip() == 'free', label
        print('Native ABI ownership, callbacks, header import and failure cleanup: OK')


if __name__ == '__main__':
    main()
