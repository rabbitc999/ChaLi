from pathlib import Path
import os,subprocess,json
root=Path.cwd();work=root/'tmp/map-buffer-model';work.mkdir(exist_ok=True);exe='chali-1.0.11-llvm-external-windows-x64.exe';results=[]
for label,compiler,allocator,mode,flag in [('old',root/'build/perf-baselines/7f2f2e8'/exe,'malloc','--checked',False),('malloc',root/exe,'malloc','--checked',False),('pool',root/exe,'pool','--unchecked',False),('fallback',root/exe,'malloc','--checked',True)]:
    env=dict(os.environ)
    if flag:env['CHALI_NO_AOT_RAW_INTRINSICS']='1'
    prefix=work/label
    for command in [[compiler,'aot',root/'tests/fixtures/bootstrap_hashed_default.chl','--allocator',allocator,mode,'--out',prefix],[prefix.with_suffix('.exe')]]:
        p=subprocess.run(list(map(str,command)),env=env,capture_output=True,text=True,encoding='utf-8',timeout=600);results.append(dict(label=label,command=list(map(str,command)),code=p.returncode,stdout=p.stdout,stderr=p.stderr));(work/'results.json').write_text(json.dumps(results,ensure_ascii=False,indent=2),encoding='utf-8');assert p.returncode==0,(label,p.stdout,p.stderr)
    assert 'bootstrap-hashed-default-ok' in p.stdout
    print(label,'independent collision model OK',flush=True)
