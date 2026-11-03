"""原生多对象、确定性归档、循环依赖、导出和失败事务的实际验收。"""
from pathlib import Path
import argparse
import ctypes
import json
import os
import struct
import subprocess
import threading
import time
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def run(arguments, work, success=True, environment=None):
    """逐项检查工具返回值，保留实际原生错误诊断。"""
    result = subprocess.run(list(map(str, arguments)), cwd=work, capture_output=True,
                            encoding='utf-8', timeout=180, env={**os.environ, **(environment or {})})
    assert (result.returncode == 0) == success, (arguments, result.returncode, result.stdout, result.stderr)
    return result


def main():
    """用真实对象与运行结果验证独立链接闭环。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--external-clang', type=Path, help='额外核对显式外置 LLVM 链接路径')
    args = parser.parse_args()
    compiler = args.chali.resolve()
    info = json.loads(subprocess.check_output([str(compiler), '--build-info'], encoding='utf-8'))
    toolchain = ROOT/'build/dependencies/llvm/toolchain/bin'
    backend = [] if info['llvm'] == 'static' else ['--llvm', toolchain/'clang.exe'] if os.name == 'nt' else []
    if args.external_clang:
        backend = ['--llvm', args.external_clang.resolve()]
    ar_backend = [] if info['llvm'] == 'static' else ['--llvm', toolchain/'llvm-ar.exe'] if os.name == 'nt' else []
    with tempfile.TemporaryDirectory(prefix='native-link-') as folder:
        work = Path(folder)

        def obj(name, body):
            """编译宿主 C 对象，以独立工具验证 C ABI 互调。"""
            source = work/(name+'.c'); source.write_text(body, encoding='utf-8')
            output = work/(name+'.o')
            run(['gcc', '-c', '-O2', *(['-fPIC'] if os.name != 'nt' else []), source, '-o', output], work)
            return output

        first = obj('first', 'int tail(void);int first(void){return tail()+2;}')
        tail = obj('tail', 'int tail(void){return 40;}')
        second = obj('second', 'int first(void);int second(void){return first();}')
        # A→B→A，第二个 A 成员需要在 B 被提取之后再次扫描。
        first = obj('first', 'int second(void);int first(void){return second()+2;}')
        second = obj('second', 'int tail(void);int second(void){return tail();}')
        unused = obj('unused', 'int missing_never_called(void);int unused(void){return missing_never_called();}')
        liba, libb = work/'libalpha.a', work/'libbeta.a'
        run([compiler, 'ar', first, tail, unused, '--out', liba, *ar_backend], work)
        snapshot = liba.read_bytes()
        run([compiler, 'ar', first, tail, unused, '--out', liba, *ar_backend], work)
        assert liba.read_bytes() == snapshot and snapshot.startswith(b'!<arch>\n')
        run([compiler, 'ar', second, '--out', libb, *ar_backend], work)
        main_object = obj('main', 'int first(void);int main(void){return first()==42?0:19;}')
        product = work/('program & 中文.exe' if os.name == 'nt' else 'program & 中文')
        run([compiler, 'link', main_object, liba, libb, '--out', product, *backend], work)
        run([product], work)
        if os.name == 'nt':
            kernel = ctypes.WinDLL('kernel32', use_last_error=True)
            kernel.CreateFileW.argtypes = [ctypes.c_wchar_p, ctypes.c_uint32, ctypes.c_uint32,
                                          ctypes.c_void_p, ctypes.c_uint32, ctypes.c_uint32, ctypes.c_void_p]
            kernel.CreateFileW.restype = ctypes.c_void_p
            kernel.CloseHandle.argtypes = [ctypes.c_void_p]
            sentinel = b'preserve previous output until publish succeeds'
            for release in (True, False):
                product.write_bytes(sentinel)
                handle = kernel.CreateFileW(str(product), 0x80000000, 1, None, 3, 0x80, None)
                assert handle not in (None, ctypes.c_void_p(-1).value), ctypes.get_last_error()
                finished = threading.Event()
                observed = threading.Event()

                def unlock():
                    """等发布暂存文件真实出现后解除占用，保证测试经过替换重试。"""
                    try:
                        deadline = time.monotonic() + 30
                        while not finished.wait(0.01) and time.monotonic() < deadline:
                            if list(work.glob(product.name + '.chali-*.tmp')):
                                observed.set()
                                time.sleep(0.3)
                                break
                    finally:
                        kernel.CloseHandle(handle)

                thread = threading.Thread(target=unlock) if release else None
                if thread:
                    thread.start()
                started = time.monotonic()
                try:
                    result = run([compiler, 'link', main_object, liba, libb, '--out', product, *backend],
                                 work, release)
                    elapsed = time.monotonic() - started
                    if release:
                        assert observed.is_set() and elapsed >= 0.3
                        run([product], work)
                    else:
                        assert 5 <= elapsed < 30 and '无法发布链接产物' in result.stderr, result
                        assert product.read_bytes() == sentinel
                    assert not list(work.glob(product.name + '.chali-*.tmp'))
                    print(f'Windows publish lock: release={release}, elapsed={elapsed:.3f}s, staging removed', flush=True)
                finally:
                    finished.set()
                    if thread:
                        thread.join(timeout=35)
                        assert not thread.is_alive()
                    else:
                        kernel.CloseHandle(handle)
            run([compiler, 'link', main_object, liba, libb, '--out', product, *backend], work)
        run([compiler, 'link', main_object, '--library', 'beta', '--library', 'alpha', '-L', work,
             '--out', product, *backend], work)
        run([product], work)
        consumer = work/'consumer.chl'
        consumer.write_text('#abi("C","@link","first") int32 first();\nvoid main(){println("linked {}",first());}\n', encoding='utf-8')
        run([compiler, 'aot', consumer, '--emit-obj', '--out', work/'consumer', *backend], work)
        language_object = work/'consumer.o'
        language_output = work/('language.exe' if os.name == 'nt' else 'language')
        run([compiler, 'link', language_object, liba, libb, '--out', language_output, *backend], work)
        assert run([language_output], work).stdout == 'linked 42\n'
        # 缺失、重复、截断、薄归档、目标错误均不能覆盖已有成功产物。
        preserved = product.read_bytes()
        duplicate = obj('duplicate', 'int first(void){return 0;}')
        bad = work/'bad.o'; bad.write_bytes(b'\x64\x86\x01')
        thin = work/'thin.a'; thin.write_bytes(b'!<thin>\n')
        for inputs in [[main_object], [main_object, first, duplicate], [bad], [thin], [work/'absent.o']]:
            run([compiler, 'link', *inputs, '--out', product, *backend], work, False)
            assert product.read_bytes() == preserved
        run([compiler, 'link', main_object, '--out', main_object, *backend], work, False)
        run([compiler, 'ar', liba, '--out', liba, *ar_backend], work, False)
        assert liba.read_bytes() == snapshot
        run([compiler, 'ar', bad, '--out', liba, *ar_backend], work, False)
        assert liba.read_bytes() == snapshot
        run([compiler, 'link', main_object, liba, libb, '--entry', 'bad;symbol', '--out', product, *backend], work, False)
        assert product.read_bytes() == preserved
        run([compiler, 'link', main_object, liba, libb, '--out', work/'missing/app', *backend], work, False)
        run([compiler, 'ar', first, '--out', work/'missing/archive.a', *ar_backend], work, False)
        assert not list(work.glob('*.chali-*.tmp'))
        empty = work/'empty.a'
        run([compiler, 'ar', '--out', empty, *ar_backend], work)
        assert empty.read_bytes().startswith(b'!<arch>\n')
        if os.name == 'nt':
            coff = work/'library.lib'
            run([compiler, 'ar', tail, '--out', coff, *ar_backend], work)
            assert coff.read_bytes().startswith(b'!<arch>\n')
        exported = obj('exported', 'int visible(void){return 73;}int hidden(void){return 17;}')
        shared = work/('exports.dll' if os.name == 'nt' else 'exports.so')
        run([compiler, 'link', exported, '--shared', '--export', 'visible', '--out', shared, *backend], work)
        library = ctypes.CDLL(str(shared))
        assert library.visible() == 73
        try:
            library.hidden
            raise AssertionError('未显式导出的内部函数泄漏')
        except AttributeError:
            pass
        # Windows DLL 必须释放后才能清理测试工作区。
        if os.name == 'nt':
            import _ctypes
            _ctypes.FreeLibrary(library._handle)
        del library
        # ctypes 按路径加载不能发现 DLL 自身名字错误；另编译宿主，验证实际导入表及系统加载器。
        host = work/'shared-host.c'
        host.write_text('''/** @brief 由独立链接的共享库提供整数结果。
 * @return 已实现的测试结果 73。 */
int visible(void);
/** @brief 从独立链接的实际库文件建立导入并核对导出函数。
 * @return 结果一致时为零，否则失败。 */
int main(void){return visible()==73?0:1;}
''', encoding='utf-8')
        host_product = work/('shared-host.exe' if os.name == 'nt' else 'shared-host')
        run(['gcc', host, shared, *([] if os.name == 'nt' else ['-Wl,-rpath,'+str(work)]), '-o', host_product], work)
        assert not (work/'output.dll').exists()
        run([host_product], work)
        for target, machine, body in [
            ('x86_64-unknown-linux-gnu', 62, 'mov $60,%rax\nxor %rdi,%rdi\nsyscall\n'),
            ('aarch64-unknown-linux-gnu', 183, 'mov x8,#93\nmov x0,#0\nsvc #0\n')]:
            assembly = work/'entry.s'; assembly.write_text('.text\n.globl _start\n_start:\n'+body, encoding='utf-8')
            native = work/f'entry-{machine}.o'
            run([compiler, 'asm', assembly, '--target', target, '--out', native, *backend], work)
            output = work/f'cross-{machine}'
            run([compiler, 'link', native, '--target', target, '--no-runtime', '--entry', '_start',
                 '--out', output, *backend], work)
            data = output.read_bytes()
            assert data[:4] == b'\x7fELF' and struct.unpack_from('<H', data, 18)[0] == machine
            run([compiler, 'ar', native, '--target', target, '--out', work/f'cross-{machine}.a', *ar_backend], work)
            run([compiler, 'link', native, '--out', product, *backend], work, False)
            assert product.read_bytes() == preserved
            broken = work/f'truncated-{machine}.a'
            broken.write_bytes(b'!<arch>\n'+b'name/           '+b'0           0     0     644     '+b'9999999999'+b'`\n')
            run([compiler, 'link', broken, '--target', target, '--no-runtime', '--out', output, *backend], work, False)
            assert output.read_bytes() == data
        if info['llvm'] == 'static' and os.name == 'nt':
            environment = {'PATH': str(Path(os.environ['SystemRoot'])/'System32'),
                           'CHALI_LLVM_CLANG': str(work/'absent-clang.exe')}
            run([compiler, 'ar', first, tail, unused, '--out', liba], work, environment=environment)
            run([compiler, 'link', main_object, liba, libb, '--out', product], work, environment=environment)
            run([product], work)
    print('native linker: execution, cyclic archives, deterministic extraction, exports, target checks and failure preservation passed')


if __name__ == '__main__':
    main()
