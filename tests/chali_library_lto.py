"""实际核对 Full LTO／ThinLTO 的跨库内联、常量传播、无用代码删除及模式转换。"""
import argparse
import os
from pathlib import Path
import re
import shutil
import struct
from compiler_path import compiler_path
from project_temp import tempfile
from chali_library_package import invoke, decode


def pe_export(data, name):
    """从 PE32+ 的实际导出表取得函数虚拟地址，不依赖被剥离的 COFF 符号。"""
    pe = struct.unpack_from('<I', data, 60)[0]
    assert data[pe:pe + 4] == b'PE\0\0'
    sections = struct.unpack_from('<H', data, pe + 6)[0]
    optional_size = struct.unpack_from('<H', data, pe + 20)[0]
    optional = pe + 24
    assert struct.unpack_from('<H', data, optional)[0] == 0x20b
    base = struct.unpack_from('<Q', data, optional + 24)[0]
    rows = [struct.unpack_from('<IIII', data, optional + optional_size + index * 40 + 8) for index in range(sections)]

    def offset(rva):
        """将节内 RVA 映射到文件字节，拒绝仅存在于虚拟区的地址。"""
        for virtual_size, virtual, raw_size, raw in rows:
            if virtual <= rva < virtual + max(virtual_size, raw_size):
                assert rva - virtual < raw_size
                return raw + rva - virtual
        raise AssertionError(('RVA outside sections', rva))

    directory = offset(struct.unpack_from('<I', data, optional + 112)[0])
    count, functions, names, ordinals = struct.unpack_from('<IIII', data, directory + 24)
    for index in range(count):
        start = offset(struct.unpack_from('<I', data, offset(names) + index * 4)[0])
        text = data[start:data.index(b'\0', start)].decode('ascii')
        if text == name:
            ordinal = struct.unpack_from('<H', data, offset(ordinals) + index * 2)[0]
            return base + struct.unpack_from('<I', data, offset(functions) + ordinal * 4)[0]
    raise AssertionError(('missing export', name))


def instructions(objdump, library, work):
    """仅反汇编实际导出函数，截到第一个返回指令以排除相邻代码。"""
    if os.name == 'nt':
        address = pe_export(library.read_bytes(), 'f_probe')
    else:
        symbols = invoke([objdump, '-T', library], work).stdout
        line = next(line for line in symbols.splitlines() if line.split() and line.split()[-1] == 'f_probe')
        address = int(line.split()[0], 16)
    listing = invoke([objdump, '-d', '--no-show-raw-insn', '--start-address=' + str(address),
                      '--stop-address=' + str(address + 4096), library], work).stdout
    result = []
    selected = []
    for line in listing.splitlines():
        match = re.match(r'\s*[0-9a-fA-F]+:\s+(\S+)(.*)', line)
        if match:
            result.append(match[1])
            selected.append(line)
            if match[1].startswith('ret'):
                return result, '\n'.join(selected)
    raise AssertionError(('missing function return', listing))


def main():
    """构建两个库并验证优化效果，不把结构验收当作性能测量。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--objdump', default=shutil.which('objdump'))
    parser.add_argument('--cc', default=os.environ.get('CHALI_ABI_CC', shutil.which('gcc')))
    parser.add_argument('--external-clang', type=Path)
    args = parser.parse_args()
    assert args.objdump and args.cc, 'LTO 结构验收需要 objdump 和 C 编译器'
    compiler = args.chali.resolve()
    tools = ['--llvm', args.external_clang.resolve()] if args.external_clang else []
    with tempfile.TemporaryDirectory(prefix='library-lto-') as directory:
        work = Path(directory)
        modules = work / 'modules/demo'
        modules.mkdir(parents=True)
        source = work / 'provider.chl'
        marker = 0x13579bdf2468ace0
        native_marker = struct.pack('<Q', marker)
        consumer = work / 'main.chl'
        consumer.write_text('import demo::bridge;\nint64 probe(int64 x){return demo::bridge::compute(x)+20;}\nvoid main(){assert(probe(1)==42);}', encoding='utf-8')
        host = work / 'host.c'
        host.write_text('''#include <stdint.h>
#include <stdio.h>
void chali_init(void);void chali_finish(void);int64_t f_probe(int64_t);
/** @brief 核对跨库计算在多个输入上的实际结果。
 * @return 全部一致为零，否则失败。 */
int main(void){chali_init();for(int64_t x=-20;x<20;++x){if(f_probe(x)!=x*3+39)return 1;}chali_finish();puts("LTO result OK");return 0;}
''', encoding='utf-8')
        for producer_mode in ['full', 'thin']:
            source.write_text('module demo::base;\nint64 times(int64 x){return x*3;}\nint64 salt(){return 19;}\nint64 unused(){return ' + str(marker) + ';}', encoding='utf-8')
            invoke([compiler, 'lib', 'build', source, '--module', 'demo::base', '--package-version', '1.0.0',
                    '--with-bitcode', '--lto-mode', producer_mode, *tools, '--out', modules / 'base.chlib'], work)
            manifest, entries = decode((modules / 'base.chlib').read_bytes())
            assert native_marker in next(entry['data'] for entry in entries if entry['name'] == manifest['targets'][0]['implementation'])
            source.write_text('module demo::bridge;\nimport demo::base;\nint64 compute(int64 x){return demo::base::times(x)+demo::base::salt();}', encoding='utf-8')
            invoke([compiler, 'lib', 'build', source, '--module', 'demo::bridge', '--package-version', '1.0.0',
                    '--with-bitcode', '--lto-mode', producer_mode, *tools, '--out', modules / 'bridge.chlib'], work)
            source.unlink()
            for mode in ['off', 'full', 'thin']:
                prefix = work / (producer_mode + '-' + mode)
                invoke([compiler, 'aot', consumer, '--shared', '--export', 'probe', '--library-implementation', 'bitcode',
                        '--lto-mode', mode, *tools, '--out', prefix], work)
                library = prefix.with_suffix('.dll' if os.name == 'nt' else '.so')
                executable = work / ('host.exe' if os.name == 'nt' else 'host')
                invoke([args.cc, host, library, *([] if os.name == 'nt' else ['-Wl,-rpath,' + str(work)]), '-o', executable], work)
                assert 'LTO result OK' in invoke([executable], work).stdout
                operations, listing = instructions(args.objdump, library, work)
                calls = [op for op in operations if op in ['call', 'callq', 'jmp', 'jmpq', 'bl', 'blr', 'b']]
                if mode == 'off':
                    assert calls, listing
                    assert native_marker in library.read_bytes(), '对照产物须实际保留未引用入口'
                else:
                    if os.name == 'nt':
                        assert not calls, listing
                    assert re.search(r'\b0x27\b|#39\b', listing), ('跨库常量应合并为 39', listing)
                    assert re.search(r'lea\s+[^\n]*,[^,()]+,2\)|imul\s+[^\n]*0x3\b|lsl\s*#(?:0x)?1\b', listing), ('跨库乘三应进入导出函数', listing)
                    assert native_marker not in library.read_bytes(), 'LTO 须删除未引用入口及常量'
                checks = 'call/unused-entry control' if mode == 'off' else 'cross-library inline/constant folding/DCE'
                print(producer_mode + ' -> ' + mode + ': result and ' + checks + ' OK', flush=True)
        invoke([compiler, 'aot', consumer, '--emit-obj', '--library-implementation', 'bitcode', '--out', work / 'consumer', *tools], work)
        for mode in ['off', 'full', 'thin']:
            executable = work / ('linked.exe' if os.name == 'nt' else 'linked')
            invoke([compiler, 'link', work / 'consumer.o', '--package', modules / 'bridge.chlib',
                    '--library-implementation', 'bitcode', '--lto-mode', mode, *tools, '--out', executable], work)
            invoke([executable], work)
        project = work / 'chali.toml'
        for mode in ['off', 'full', 'thin']:
            project.write_text('[aot]\nlto = "' + mode + '"\n', encoding='utf-8')
            invoke([compiler, 'run', consumer, '--library-implementation', 'bitcode', *tools], work)
            invoke([compiler, 'run', consumer, '--library-implementation', 'bitcode', '--no-lto', *tools], work)
        project.write_text('[aot]\nlto = "automatic"\n', encoding='utf-8')
        invoke([compiler, 'check', consumer], work, False)
        project.unlink()
        invoke([compiler, 'aot', consumer, '--lto-mode', 'automatic'], work, False)
        print('independent link, project modes, CLI overrides and invalid-mode rejection OK', flush=True)


if __name__ == '__main__':
    main()
