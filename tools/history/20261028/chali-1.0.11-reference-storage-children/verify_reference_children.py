"""检查非 kids 子节点中的取址与现有标量、拥有值行为。"""
from pathlib import Path
import os
import subprocess
import json

root = Path.cwd()
work = root / 'tmp/reference-children-verify'
work.mkdir(parents=True,exist_ok=True)
compiler = root / 'chali-1.0.11-llvm-external-windows-x64.exe'
rows = []
for mode in ('enabled','calls-off','all-off'):
    env = dict(os.environ,CHALI_LANG='zh',CHALI_NO_STDIN_INPUT='1')
    env.pop('CHALI_NO_CALL_SCOPED_REFERENCE_STORAGE',None)
    env.pop('CHALI_NO_SCOPED_REFERENCE_STORAGE',None)
    if mode == 'calls-off': env['CHALI_NO_CALL_SCOPED_REFERENCE_STORAGE']='1'
    if mode == 'all-off': env['CHALI_NO_SCOPED_REFERENCE_STORAGE']='1'
    for allocator in ('malloc','pool'):
        for checked in (False,True):
            for case, expected in [('aot_reference_storage_children','reference storage indexed children OK\n'),
                                   ('aot_scoped_reference_storage','scoped scalar storage, real borrows and owned copies OK\n')]:
                name=f'{case}-{mode}-{allocator}-{checked}'
                output=work/name
                command=[str(compiler),'aot',str(root/'tests/fixtures'/f'{case}.chl'),'--out',str(output),'--allocator',allocator,'--checked' if checked else '--unchecked']
                build=subprocess.run(command,cwd=root,env=env,capture_output=True,text=True,encoding='utf-8',errors='replace',timeout=240)
                rows.append(dict(command=command,mode=mode,code=build.returncode,stdout=build.stdout,stderr=build.stderr))
                assert build.returncode==0,rows[-1]
                run=subprocess.run([str(output)+'.exe'],cwd=root,env=env,capture_output=True,text=True,encoding='utf-8',errors='replace',timeout=60)
                rows.append(dict(command=[str(output)+'.exe'],mode=mode,code=run.returncode,stdout=run.stdout,stderr=run.stderr))
                assert run.returncode==0 and run.stdout==expected and not run.stderr,rows[-1]
                print(name,'PASS',flush=True)
(work/'results.json').write_text(json.dumps(rows,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
print('24 configurations PASS',flush=True)
