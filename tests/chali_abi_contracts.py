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
    parser.add_argument('--cc', default='gcc')
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

        library = work / ('native.dll' if os.name == 'nt' else 'native.so')
        invoke([args.cc, '-shared', '-fPIC', ROOT / 'doc/examples/abi_contracts/native.c', '-o', library])
        source = work / 'demo.chl'
        source.write_text((ROOT / 'doc/examples/abi_contracts/demo.chl').read_text(encoding='utf-8')
                          .replace('./native.dll', library.as_posix()), encoding='utf-8')
        if args.aot:
            output = work / 'contracts'
            invoke([compiler, 'aot', source, '--out', output])
            assert invoke([str(output) + ('.exe' if os.name == 'nt' else '')]).strip() == b'ABI contracts OK'
            print('AOT C ABI 返回所有权、闭包与嵌套同步回调通过')
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
        for label, body in (
            ('direct', 'panic("callback failure");return x;'),
            ('nested', 'return apply(int32 fn(int32 y){panic("callback failure");return y;},x);')):
            failure = work / ('callback-failure-' + label + '.chl')
            failure.write_text('#abi("C", "' + library.as_posix() + '", "callback_owned") '
                               'String owned(int32 fn(int32) cb) ["free_owned"]; '
                               '#abi("C", "' + library.as_posix() + '", "apply") '
                               'int32 apply(int32 fn(int32) cb,int32 x); '
                               'void main(){let cb=int32 fn(int32 x){' + body + '};'
                               'String text=owned(cb);}', encoding='utf-8')
            marker = work / ('released-' + label + '.txt')
            env['CHALI_ABI_FREE_MARKER'] = str(marker)
            if args.aot:
                prefix = work / ('failure-' + label)
                invoke([compiler, 'aot', failure, '--checked', '--out', prefix])
                command = [str(prefix) + ('.exe' if os.name == 'nt' else '')]
            else:
                command = [str(compiler), 'run', str(failure), '--out', str(work / ('run-failure-' + label))]
            result = subprocess.run(command, cwd=ROOT, env=env, capture_output=True, timeout=600)
            assert result.returncode != 0 and b'callback failure' in result.stderr + result.stdout, result
            assert marker.read_text().strip() == 'free', label
        print('Native ABI ownership, callbacks, header import and failure cleanup: OK')


if __name__ == '__main__':
    main()
