from pathlib import Path
import subprocess,os,json,re
root=Path.cwd();work=root/'tmp/string-memory-matrix';work.mkdir(exist_ok=True);new=root/'chali-1.0.11-llvm-external-windows-x64.exe';old=root/'build/perf-baselines/6e85699'/new.name
source=root/'tests/fixtures/bootstrap_string_default.chl';results=[]
configs=[('old',old,'malloc','--checked',None)]+[(a+m,new,a,m,None) for a in ['malloc','pool'] for m in ['--checked','--unchecked']]+[('raw-off',new,'malloc','--checked','CHALI_NO_AOT_RAW_INTRINSICS'),('inline-off',new,'malloc','--checked','CHALI_NO_AOT_AST_INLINE')]
for label,compiler,allocator,mode,flag in configs:
    env=dict(os.environ)
    if flag:env[flag]='1'
    prefix=work/label
    cmds=[[compiler,'aot',source,'--allocator',allocator,mode,'--out',prefix],[prefix.with_suffix('.exe')]]
    for command in cmds:
        p=subprocess.run(list(map(str,command)),env=env,capture_output=True,text=True,encoding='utf-8',timeout=600)
        results.append(dict(label=label,command=list(map(str,command)),code=p.returncode,stdout=p.stdout,stderr=p.stderr));(work/'results.json').write_text(json.dumps(results,ensure_ascii=False,indent=2),encoding='utf-8');assert p.returncode==0,(label,p.stdout,p.stderr)
    print(label,p.stdout.strip(),flush=True)
for label,compiler in [('old',old),('new',new)]:
    p=subprocess.run([str(compiler),'aot',str(source),'--emit-llvm','--out',str(work/label)],capture_output=True,text=True,encoding='utf-8',timeout=600);assert p.returncode==0,p.stderr
    clang=root/'build/dependencies/llvm/toolchain/bin/clang.exe';p=subprocess.run([str(clang),'-O3','-S','-emit-llvm',str(work/(label+'.ll')),'-o',str(work/(label+'-o3.ll'))],capture_output=True,text=True,encoding='utf-8',timeout=600);assert p.returncode==0,p.stderr
    ir=(work/(label+'-o3.ll')).read_text(encoding='utf-8');stats={}
    for name in ['__bootStringBuffer','__bootStringCopyBytes','__bootStringSet']:
        match=re.search(r'define[^\n]*@[^\n]*'+name+r'\([^\n]*\{\n(.*?)\n}',ir,re.S);assert match,name
        body=match.group(1);stats[name]=dict(lines=len(body.splitlines()),allocations=len(re.findall(r'call[^\n]*@chali_host_alloc\(',body)),calls=len(re.findall(r'\bcall\b',body)))
    (work/(label+'-stats.json')).write_text(json.dumps(stats,indent=2),encoding='utf-8');print(label,stats,flush=True)
