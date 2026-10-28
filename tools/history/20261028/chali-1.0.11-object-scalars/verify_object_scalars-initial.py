"""核对对象查询直通入口的正常路径和原诊断。"""
from pathlib import Path
import os
import subprocess
import json
root=Path.cwd();work=root/'tmp/object-scalars-verify';work.mkdir(parents=True,exist_ok=True)
compiler=root/'chali-1.0.11-llvm-external-windows-x64.exe'
old=root/'build/perf-baselines/527f896'/compiler.name
rows=[]
def call(binary,args,disabled=False):
    env=dict(os.environ,CHALI_LANG='zh',CHALI_NO_STDIN_INPUT='1')
    env.pop('CHALI_NO_AOT_OBJECT_SCALARS',None)
    if disabled:env['CHALI_NO_AOT_OBJECT_SCALARS']='1'
    result=subprocess.run([str(binary),*map(str,args)],cwd=root,env=env,capture_output=True,text=True,encoding='utf-8',errors='replace',timeout=240)
    rows.append(dict(command=[str(binary),*map(str,args)],disabled=disabled,code=result.returncode,stdout=result.stdout,stderr=result.stderr))
    return result
for allocator in ('malloc','pool'):
    for disabled in (False,True):
        for checked in (False,True):
            name=f'{allocator}-{disabled}-{checked}';output=work/name
            result=call(compiler,['aot',root/'tests/fixtures/aot_object_scalars.chl','--out',output,'--allocator',allocator,'--checked' if checked else '--unchecked'],disabled)
            assert result.returncode==0,rows[-1]
            result=call(str(output)+'.exe',[])
            assert result.returncode==0 and result.stdout=='object scalar descriptors OK\n' and not result.stderr,rows[-1]
            print(name,'PASS',flush=True)
for name in ('interface_inherit','interface_view_ownership','object_type_combinations'):
    expected=(root/'example/expected'/f'{name}.txt').read_text(encoding='utf-8')
    for disabled in (False,True):
        result=call(compiler,['run',root/'example/samples'/f'{name}.chl','--out',work/f'{name}-{disabled}'],disabled)
        assert result.returncode==0 and result.stdout==expected and not result.stderr,rows[-1]
    print(name,'PASS',flush=True)
for name,source,message in [
    ('readonly','interface I{void write();};struct S:I{int64 x;void write(){self.x=1;}};void main(){S s;I const& i=&s;i.write();}','只读接收者'),
    ('unbound','interface I{int64 read();};void main(){I const& value;println("{}",value.read());}','引用尚未绑定有效元素')]:
    path=work/f'{name}.chl';path.write_text(source,encoding='utf-8')
    for binary,disabled in [(old,False),(compiler,False),(compiler,True)]:
        result=call(binary,['run',path,'--out',work/f'{name}-{len(rows)}'],disabled)
        assert result.returncode!=0 and message in result.stderr,rows[-1]
    print(name,'old/on/off PASS',flush=True)
(work/'results.json').write_text(json.dumps(rows,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
print('ALL PASS',flush=True)
