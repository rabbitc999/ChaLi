"""验证公开 rawSize 与 ChaLi 字节／前缀地址实现，不生成 LLVM IR。"""
from pathlib import Path
import json
import os
import re
import subprocess
import sys
root=Path.cwd();work=root/'tmp/std-lang-address-check';work.mkdir(parents=True,exist_ok=True)
compiler=root/'chali-1.0.11-llvm-external-windows-x64.exe';old=root/'build/perf-baselines/2609cf0'/compiler.name
rows=[]
def invoke(binary,args,flags=None):
    env=dict(os.environ,CHALI_LANG='zh',CHALI_NO_STDIN_INPUT='1')
    for key in ('CHALI_NO_AOT_RAW_INTRINSICS','CHALI_NO_AOT_INLINE_HINT'):
        env.pop(key,None)
    env.update(flags or {})
    result=subprocess.run([str(binary),*map(str,args)],cwd=root,env=env,capture_output=True,text=True,encoding='utf-8',errors='replace',timeout=300)
    rows.append(dict(command=[str(binary),*map(str,args)],flags=flags or {},code=result.returncode,stdout=result.stdout,stderr=result.stderr))
    (work/'results.json').write_text(json.dumps(rows,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    return result
before=invoke(old,['run',root/'tests/fixtures/aot_byte_primitives.chl','--out',work/'before'])
assert before.returncode==0 and before.stdout=='byte primitives, wrap, order and UTF-8 OK\n' and not before.stderr,rows[-1]
modes=[('default',{}),('raw-off',{'CHALI_NO_AOT_RAW_INTRINSICS':'1'}),('inline-off',{'CHALI_NO_AOT_INLINE_HINT':'1'})]
for mode,flags in modes:
    for allocator in ('malloc','pool'):
        for checked in (False,True):
            cases=[('aot_byte_primitives','byte primitives, wrap, order and UTF-8 OK\n')]
            if mode=='default' or (allocator=='malloc' and checked):
                cases.append(('aot_raw_size','rawSize constants and storage strides OK\n'))
            for case,expected in cases:
                prefix=work/f'{case}-{mode}-{allocator}-{checked}'
                built=invoke(compiler,['aot',root/'tests/fixtures'/f'{case}.chl','--out',prefix,'--allocator',allocator,'--checked' if checked else '--unchecked'],flags)
                assert built.returncode==0,rows[-1]
                result=invoke(str(prefix)+'.exe',[])
                assert result.returncode==0 and result.stdout==expected and not result.stderr,rows[-1]
                print(prefix.name,'PASS',flush=True)
void=work/'void.chl';void.write_text('void main(){println("{}",rawSize<void>());}\n',encoding='utf-8')
for args in [('check',void,'json'),('aot',void,'--emit-llvm','--out',work/'void')]:
    result=invoke(compiler,args)
    assert result.returncode!=0 and '原始布局查询不支持 void' in result.stdout+result.stderr,rows[-1]
reference={}
for label,binary,flags in [('before',old,{}),('default',compiler,{}),('raw-off',compiler,modes[1][1])]:
    prefix=work/f'errors-{label}'
    built=invoke(binary,['aot',root/'tests/fixtures/aot_byte_primitives.chl','--shared','--unchecked','--out',prefix],flags)
    assert built.returncode==0,rows[-1]
    for case in ('null','unknown','released'):
        result=invoke(sys.executable,[root/'tmp/byte_address_errors.py',str(prefix)+'.dll',case])
        assert result.returncode!=0 and 'chali aot: panic' in result.stderr,rows[-1]
        primary=result.stderr.splitlines()[0]
        primary=re.sub(r'\d+','<number>',primary)
        if label=='before':reference[case]=primary
        else:assert primary==reference[case],(reference[case],rows[-1])
    print(label,'null/unknown/released diagnostics PASS',flush=True)
print('ALL PASS',flush=True)
