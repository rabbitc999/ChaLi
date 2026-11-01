"""真实汇编器的对象、重定位、包含、诊断、事务发布及 AOT 链接验收。"""
from pathlib import Path
import argparse
import json
import os
import platform
import struct
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def invoke(arguments, work, success=True, environment=None):
    """独立 argv 运行，失败案例仍保留并检查原始诊断。"""
    result = subprocess.run(list(map(str, arguments)), cwd=work, capture_output=True, encoding='utf-8', timeout=180,
                            env={**os.environ, 'CHALI_LANG':'zh', **(environment or {})})
    assert (result.returncode == 0) == success, (arguments,result.returncode,result.stdout,result.stderr)
    return result


def sources(target):
    """按目标真实调用约定构造引用另一对象数据的函数。"""
    if 'aarch64' in target:
        body='add x0,x0,x1\nadd x0,x0,#EXTRA\nadrp x9,asm_data\nadd x9,x9,:lo12:asm_data\nldr x9,[x9]\nadd x0,x0,x9\nret\n'
    else:
        left,right=('rcx','rdx') if 'windows' in target else ('rdi','rsi')
        body=f'leaq (%{left},%{right}),%rax\naddq $EXTRA,%rax\naddq asm_data(%rip),%rax\nretq\n'
    floating = 'fadd d0,d0,d0\nret\n' if 'aarch64' in target else 'addsd %xmm0,%xmm0\nretq\n'
    return '.include "constants.inc"\n.text\n.globl asm_add\nasm_add:\n'+body+'.globl asm_scale\nasm_scale:\n'+floating


def object_target(path, machine, windows=False):
    """只验证对象格式及架构，不把交叉对象当作已运行的程序。"""
    data=path.read_bytes()
    if windows:
        assert struct.unpack_from('<H',data)[0]==machine and struct.unpack_from('<H',data,2)[0]>0
    else:
        assert data[:4]==b'\x7fELF' and struct.unpack_from('<H',data,18)[0]==machine


def main():
    """用实际编译器验证独立汇编与语言单元共同链接。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=compiler_path())
    compiler=parser.parse_args().chali.resolve()
    info=json.loads(subprocess.check_output([str(compiler),'--build-info'],encoding='utf-8'))
    clang=ROOT/'build/dependencies/llvm/toolchain/bin/clang.exe'
    backend=[] if info['llvm']=='static' else ['--llvm',str(clang)] if clang.exists() else []
    host='x86_64-w64-windows-gnu' if os.name=='nt' else ('aarch64-unknown-linux-gnu' if platform.machine().lower() in ('arm64','aarch64') else 'x86_64-unknown-linux-gnu')
    with tempfile.TemporaryDirectory(prefix='assembler-') as folder:
        work=Path(folder);include=work/'include & 中文';include.mkdir()
        (include/'constants.inc').write_text('.set EXTRA,3\n',encoding='utf-8')
        assembly=work/'function & 中文.s';data=work/'data.s'
        data.write_text('.data\n.p2align 3\n.globl asm_data\nasm_data:\n.quad 7\n',encoding='utf-8')
        for target,machine,windows in [('x86_64-w64-windows-gnu',0x8664,True),('x86_64-unknown-linux-gnu',62,False),('aarch64-unknown-linux-gnu',183,False)]:
            assembly.write_text(sources(target),encoding='utf-8')
            output=work/('target-'+str(machine)+'.o')
            invoke([compiler,'asm',assembly,'--target',target,'--cpu','generic' if 'aarch64' in target else 'x86-64','--out',output,'-I'+str(include),*backend],work)
            object_target(output,machine,windows)
        assembly.write_text(sources(host),encoding='utf-8')
        debug=work/'debug.o'
        invoke([compiler,'asm',assembly,'--out',debug,'--debug','--include-path',include,*backend],work)
        assert b'.debug_line' in debug.read_bytes()
        invoke([compiler,'asm',assembly,'-I',include,*backend],work)
        assert assembly.with_suffix('.obj' if os.name=='nt' else '.o').exists()
        old=work/'preserved.o';old.write_bytes(b'original object')
        bad=work/'bad.s';bad.write_text('.text\nnot_an_instruction\n',encoding='utf-8')
        failure=invoke([compiler,'asm',bad,'--out',old,*backend],work,False)
        assert 'bad.s:2:' in failure.stderr and old.read_bytes()==b'original object',failure.stderr
        missing=work/'missing.s';missing.write_text('.include "missing-file.inc"\n',encoding='utf-8')
        invoke([compiler,'asm',missing,'--out',old,*backend],work,False)
        assert old.read_bytes()==b'original object'
        invoke([compiler,'asm',assembly,'--out',old,'--cpu','__invalid_cpu__','-I',include,*backend],work,False)
        assert old.read_bytes()==b'original object'
        preprocessed=work/'preprocessed.S';preprocessed.write_text('.text\n',encoding='utf-8')
        invoke([compiler,'asm',preprocessed,'--out',old,*backend],work,False)
        original=assembly.read_bytes()
        invoke([compiler,'asm',assembly,'--out',assembly,'-I',include,*backend],work,False)
        assert assembly.read_bytes()==original
        invoke([compiler,'asm',assembly,'--out',work/'missing-parent/out.o','-I',include,*backend],work,False)
        assert not list(work.glob('*.chali-*.tmp'))
        invoke([compiler,'asm',assembly,'--target','riscv64-unknown-linux-gnu','--out',old,*backend],work,False)
        assert old.read_bytes()==b'original object'
        invoke([compiler,'asm',assembly,'--out',old,'-I',include,*backend],work)
        assert old.read_bytes()!=b'original object'
        consumer=work/'main.chl';consumer.write_text('#abi("C","@link","asm_add") int64 asm_add(int64 a,int64 b);\n#abi("C","@link","asm_scale") float64 asm_scale(float64 value);\nvoid main(){assert(asm_scale(2.5)==5.0);println("asm {}",asm_add(20,22));}\n',encoding='utf-8')
        output=work/'final & 中文'
        invoke([compiler,'aot',consumer,'--asm',assembly,'--asm',data,'--asm-include',include,'--out',output,*backend],work)
        assert invoke([output.with_suffix('.exe') if os.name=='nt' else output],work).stdout=='asm 52\n'
        native=work/'native.c';library=work/('native.dll' if os.name=='nt' else 'native.so')
        native.write_text('struct Packet{long long value;};\n#if defined(_WIN32)\n__declspec(dllexport)\n#endif\nstruct Packet transform(struct Packet p){p.value+=1;return p;}\n',encoding='utf-8')
        invoke(['gcc','-shared',*(['-fPIC'] if os.name!='nt' else []),native,'-o',library],work)
        mixed=work/'mixed.chl'
        mixed.write_text('#abi("C","@link","asm_add") int64 asm_add(int64 a,int64 b);\n#abi("C","@link","asm_scale") float64 asm_scale(float64 value);\nstruct Packet{int64 value;};\n#abi("C",'+json.dumps(library.as_posix())+',"transform") Packet transform(Packet value);\nvoid main(){assert(asm_scale(2.5)==5.0);Packet p;p.value=asm_add(20,22);let result=transform(p);println("mixed {}",result.value);}\n',encoding='utf-8')
        invoke([compiler,'aot',mixed,'--asm',assembly,'--asm',data,'--asm-include',include,'--out',work/'mixed',*backend],work)
        assert invoke([work/('mixed.exe' if os.name=='nt' else 'mixed')],work).stdout=='mixed 53\n'
        unsupported=work/'unsupported.chl'
        unsupported.write_text('struct Packet{String value;};#abi("C","@link","transform") Packet transform(Packet value);void main(){}',encoding='utf-8')
        failure=invoke([compiler,'--check',unsupported],work,False)
        assert 'abi' in failure.stderr.lower()
        for flag in ['--emit-asm','--emit-obj','--emit-llvm']:
            invoke([compiler,'aot',consumer,'--asm',assembly,flag,'--out',work/'invalid',*backend],work,False)
        if info['llvm']=='static':
            environment={'PATH':str(Path(os.environ['SystemRoot'])/'System32'),'CHALI_LLVM_CLANG':str(work/'missing-clang.exe')}
            invoke([compiler,'asm',assembly,'--out',work/'isolated.o','-I',include],work,environment=environment)
    print('assembler: three targets, relocation link, includes, debug, atomic failure, dialect/target rejection and AOT integration passed')


if __name__=='__main__':
    main()
