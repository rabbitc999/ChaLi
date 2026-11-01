"""验证真实 .s 输出、重新汇编链接执行、项目输出选择和交叉对象。"""
from pathlib import Path
import argparse
import itertools
import json
import os
import platform
import shutil
import struct
import subprocess
import zlib
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
SOURCE = '''struct Pair { int64 first; int64 second; };
int64 total(int64 n) {
    List<int64> values;
    int64 index=0;
    while(index<n){values.push(index);index+=1;}
    int64 sum=0;
    for(int64 value in values){sum+=value;}
    Pair pair;pair.first=sum;pair.second=3;
    return pair.first+pair.second;
}
void main(){println("assembly {}",total(10));}
'''


def command(arguments, cwd, expected=0, environment=None):
    """以独立 argv 执行工具，失败时保留完整诊断。"""
    result = subprocess.run(list(map(str, arguments)), cwd=cwd, capture_output=True,
                            encoding='utf-8', timeout=180, env={**os.environ, 'CHALI_LANG':'zh', **(environment or {})})
    assert result.returncode == expected, (arguments, result.returncode, result.stdout, result.stderr)
    return result


def main():
    """比较正常对象路径与机器汇编路径，不计时也不生成生产 LLVM IR。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--llvm', type=Path)
    parser.add_argument('--runtime-assets', type=Path)
    args = parser.parse_args()
    compiler = args.chali.resolve()
    info = json.loads(subprocess.check_output([str(compiler), '--build-info'], encoding='utf-8'))
    clang = args.llvm or Path(os.environ.get('CHALI_LLVM_CLANG', ROOT / 'build/dependencies/llvm/toolchain/bin/clang.exe'))
    if not clang.is_file():
        clang = Path(shutil.which('clang') or '')
    assert clang.is_file(), '重新汇编验收需要匹配的 clang'
    clang = clang.resolve()
    build_root = ROOT / (('build/llvm-static-gcc16' if info['llvm'] == 'static' else 'build/llvm-external-gcc11') if os.name == 'nt' else 'build')
    assets = args.runtime_assets or build_root / 'chali-aot-host/assets.txt'
    assert assets.is_file(), assets
    with tempfile.TemporaryDirectory(prefix='aot-assembly-') as directory:
        work = Path(directory)
        source = work / 'assembly 中文.chl'
        source.write_text(SOURCE, encoding='utf-8')
        runtime = work / 'runtime'
        runtime.mkdir()
        for line in assets.read_text(encoding='ascii').splitlines():
            name, size, data = line.split()
            assert Path(name).name == name
            raw = zlib.decompress(bytes.fromhex(data), wbits=-15)
            assert len(raw) == int(size)
            (runtime / name).write_bytes(raw)
        backend = [] if info['llvm'] == 'static' else ['--llvm', clang]
        target = 'x86_64-w64-windows-gnu' if os.name == 'nt' else ('aarch64-unknown-linux-gnu' if platform.machine().lower() in ('aarch64','arm64') else 'x86_64-unknown-linux-gnu')
        for index, flags in enumerate([['--checked'], ['--unchecked'], ['--checked', '--debug']]):
            prefix = work / f'case{index}'
            command([compiler, 'aot', source, '--emit-asm', '--out', prefix, *flags, *backend], work)
            assembly = prefix.with_suffix('.s')
            content = assembly.read_text(encoding='utf-8')
            assert '.text' in content and 'target triple =' not in content and content.strip()
            assert not prefix.with_suffix('.exe').exists() and not prefix.with_suffix('.o').exists()
            if '--debug' in flags:
                assert '.debug_info' in content and '.debug_line' in content
            obj = prefix.with_suffix('.o')
            command([clang, '--target='+target, '-c', assembly, '-o', obj], work)
            program = work / (f'roundtrip{index}.exe' if os.name == 'nt' else f'roundtrip{index}')
            linking = [clang, '--target='+target, obj, '-o', program, '-fuse-ld=lld', '-L'+str(runtime),
                       '-Wl,--start-group', '-lchali_aot_host', runtime/'libstdc++.a', '-lgcc_eh', '-lgcc']
            if os.name == 'nt':
                linking += ['-static', '-lwinpthread', '-lwinhttp', '-lws2_32', '-lcrypt32', '-lbcrypt',
                            *[runtime/name for name in ['gcc_libmingw32.a','gcc_libmingwex.a','gcc_libmoldname.a']]]
            else:
                linking += ['-pthread', '-lm', '-ldl']
            command([*linking, '-Wl,--end-group'], work)
            actual = command([program], work).stdout
            direct = work / f'direct{index}'
            command([compiler, 'aot', source, '--out', direct, *flags, *backend], work)
            direct_program = direct.with_suffix('.exe') if os.name == 'nt' else direct
            assert actual == command([direct_program], work).stdout == 'assembly 48\n'
        for name, triple, machine in [('arm64', 'aarch64-unknown-linux-gnu', 183), ('x64','x86_64-unknown-linux-gnu',62)]:
            prefix = work / name
            command([compiler, 'aot', source, '--emit-asm', '--target', triple, '--out', prefix, *backend], work)
            obj = prefix.with_suffix('.o')
            command([clang, '--target='+triple, '-c', prefix.with_suffix('.s'), '-o', obj], work)
            raw = obj.read_bytes()
            assert raw[:4] == b'\x7fELF' and struct.unpack_from('<H', raw, 18)[0] == machine
            debug_prefix = work / (name + '-debug')
            command([compiler, 'aot', source, '--emit-asm', '--debug', '--checked', '--target', triple, '--out', debug_prefix, *backend], work)
            debug_assembly = debug_prefix.with_suffix('.s').read_text(encoding='utf-8')
            assert '.debug_info' in debug_assembly and 'chali_debug_catalog' in debug_assembly and 'chali_debug_positions' in debug_assembly
            debug_object = debug_prefix.with_suffix('.o')
            command([clang, '--target='+triple, '-c', debug_prefix.with_suffix('.s'), '-o', debug_object], work)
            debug_bytes = debug_object.read_bytes()
            assert debug_bytes[:4] == b'\x7fELF' and struct.unpack_from('<H', debug_bytes, 18)[0] == machine
        for left, right in itertools.combinations(['--emit-asm','--emit-obj','--emit-llvm'], 2):
            result = command([compiler,'aot',source,left,right,'--out',work/'invalid'],work,expected=2)
            assert '只能选择一种' in result.stderr
        abi_source = work / 'abi.chl'
        abi_source.write_text('struct Packet{int64 value;};\n#abi("C","missing_native.dll","transform") Packet transform(Packet value);\nvoid main(){Packet p;p.value=7;let result=transform(p);println("{}",result.value);}',encoding='utf-8')
        command([compiler,'aot',abi_source,'--emit-asm','--out',work/'abi',*backend],work,
                environment={'CHALI_ABI_CXX':str(work/'missing-cxx'), 'CHALI_ABI_NO_CACHE':'1'})
        assert (work/'abi.s').is_file() and (work/'abi.abi.cpp').stat().st_size > 0
        assert not (work/'abi.o').exists() and not (work/'abi.exe').exists()
        manifest = work / 'chali.toml'
        native = 'windows-x64' if os.name == 'nt' else ('linux-arm64' if platform.machine().lower() in ('aarch64','arm64') else 'linux-x64')
        manifest.write_text('[aot]\noutput="assembly"\ntarget="'+native+'"\n',encoding='utf-8')
        prefix = work/'configured'
        command([compiler,'aot',source,'--out',prefix,*backend],work)
        assert prefix.with_suffix('.s').is_file()
        command([compiler,'aot',source,'--emit-obj','--out',work/'override',*backend],work)
        assert (work/'override.o').is_file() and not (work/'override.s').exists()
        assert command([compiler,'run',source,'--out',work/'run',*backend],work).stdout == 'assembly 48\n'
    print('assembly: checked/unchecked/debug native roundtrips, two ELF targets including debug source maps, exclusive flags, TOML override, ABI sidecar without a C++ compiler and run default passed')


if __name__ == '__main__':
    main()
