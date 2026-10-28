"""保存快照开关、分配器、检查模式、上下文和生命周期证据。"""
from pathlib import Path
import json
import os
import re
import subprocess
import sys
root=Path.cwd();work=root/'tmp/data-snapshot-verify';work.mkdir(parents=True,exist_ok=True)
compiler=root/'chali-1.0.11-llvm-external-windows-x64.exe';old=root/'build/perf-baselines/fa45b73'/compiler.name
rows=[]
def call(binary,args,disabled=False):
    env=dict(os.environ,CHALI_LANG='zh',CHALI_NO_STDIN_INPUT='1');env.pop('CHALI_NO_AOT_DATA_SNAPSHOT',None)
    if disabled:env['CHALI_NO_AOT_DATA_SNAPSHOT']='1'
    result=subprocess.run([str(binary),*map(str,args)],cwd=root,env=env,capture_output=True,text=True,encoding='utf-8',errors='replace',timeout=240)
    rows.append(dict(command=[str(binary),*map(str,args)],disabled=disabled,code=result.returncode,stdout=result.stdout,stderr=result.stderr))
    return result
cases=[('aot_data_snapshot','data snapshot and heap fallback OK\n'),('aot_data_snapshot_thread','data snapshot isolated contexts OK\n')]
for case,expected in cases:
    before=call(old,['run',root/'tests/fixtures'/f'{case}.chl','--out',work/(case+'-before')])
    assert before.returncode==0 and before.stdout==expected and not before.stderr,rows[-1]
for allocator in ('malloc','pool'):
    for disabled in (False,True):
        for checked in (False,True):
            for case,expected in cases:
                name=f'{case}-{allocator}-{disabled}-{checked}';output=work/name
                result=call(compiler,['aot',root/'tests/fixtures'/f'{case}.chl','--out',output,'--allocator',allocator,'--checked' if checked else '--unchecked'],disabled)
                assert result.returncode==0,rows[-1]
                result=call(str(output)+'.exe',[])
                assert result.returncode==0 and result.stdout==expected and not result.stderr,rows[-1]
                print(name,'PASS',flush=True)
source=root/'tests/fixtures/aot_data_snapshot.chl'
text=source.read_text(encoding='utf-8');start=text.index('void main()')
invalid=work/'bounds.chl';invalid.write_text(text[:start]+'void main(){setup();println("{}",dataSum(7));}\n',encoding='utf-8')
for label,binary,disabled in [('before',old,False),('enabled',compiler,False),('disabled',compiler,True)]:
    result=call(binary,['run',invalid,'--out',work/('bounds-'+label)],disabled)
    assert result.returncode!=0 and '越界' in result.stderr,rows[-1]
    prefix=work/('lifecycle-'+label)
    result=call(binary,['aot',source,'--shared','--checked','--out',prefix],disabled)
    assert result.returncode==0,rows[-1]
    result=call(sys.executable,[root/'tmp/data_snapshot_lifecycle.py',str(prefix)+'.dll'])
    assert result.returncode==0 and not result.stderr,rows[-1]
    print(label,'bounds and lifecycle PASS',flush=True)
    ir=Path(str(prefix)+'.ll').read_text(encoding='utf-8')
    direct=len(re.findall(r'call ptr @chali_host_data_snapshot\(',ir))
    if label=='enabled':assert direct>=3,direct
    else:assert direct==0,direct
(work/'results.json').write_text(json.dumps(rows,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
print('ALL PASS',flush=True)
