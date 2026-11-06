"""当前版本的完整跨语言实测；不读取历史结果，不给性能作达标判定。"""
from pathlib import Path
from datetime import datetime, timezone, timedelta
import argparse
import hashlib
import importlib.util
import json
import math
import os
import platform
import random
import re
import shutil
import statistics
import subprocess
import sys
import time

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
BASIC = ('int_chain','float_chain','call','array','array_rw','list_growth','map_growth','set_growth')
FEATURES = ('enum_payload','interface_mono','interface_poly','struct_nested','array_2d','array_3d')
STRINGS = ('string_read','string_concat','string_slice','string_find')
CASES = BASIC+FEATURES+STRINGS
LANGUAGES = ('chali','c','cpp','rust','java','lua','python','go')
LUA_HOST=ROOT/'build/dependencies/lua/current'/('luabench.exe' if os.name=='nt' else 'luabench')
LUA_REVISION='7579fc9d7ed90240487251dfb69168f8e64e9294'

C_CLOCK = '''
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif
/**
 * @brief 取得单调纳秒时钟。
 * @return 单调纳秒值。
 */
static int64_t bench_now(void){
#ifdef _WIN32
 LARGE_INTEGER f,t;QueryPerformanceFrequency(&f);QueryPerformanceCounter(&t);
 return t.QuadPart/f.QuadPart*1000000000LL+t.QuadPart%f.QuadPart*1000000000LL/f.QuadPart;
#else
 struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return(int64_t)t.tv_sec*1000000000LL+t.tv_nsec;
#endif
}
'''


def digest(path):
    """记录实际文件字节，而非源文件名或版本标签。"""
    with Path(path).open('rb') as f: return hashlib.file_digest(f,'sha256').hexdigest()


def execute(command, cwd=ROOT, timeout=600):
    """保留命令和全部输出；只有成功执行才能进入下一阶段。"""
    command=list(map(str,command))
    env=dict(os.environ)
    sdk=ROOT/'build/dependencies/llvm/static-sdk/bin'
    if Path(command[0]).parent==sdk:env['PATH']=str(sdk)+os.pathsep+env['PATH']
    p=subprocess.run(command,cwd=cwd,env=env,capture_output=True,text=True,encoding='utf-8',errors='replace',timeout=timeout)
    row=dict(command=command,code=p.returncode,stdout=p.stdout,stderr=p.stderr)
    if p.returncode: raise RuntimeError(json.dumps(row,ensure_ascii=False))
    return row


def save(path,data):
    """按完整记录写入检查点；采样中断不会丢失先前观测。"""
    path.write_text(json.dumps(data,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')


def c_timer(fp=False, kernel='work', setup='((void)0)', finish='((void)0)'):
    """为内核添加易变函数入口，计时只包住工作函数。"""
    scalar='double' if fp else 'int64_t'
    fmt='%.17g' if fp else '%lld'
    cast='double' if fp else 'long long'
    merge='sink*0.5+value' if fp else 'sink+value'
    return C_CLOCK+f'''
/**
 * @brief 校验五个初值，或预热后采集五次工作区间。
 * @param argc 实参数。
 * @param argv 实参。
 * @return 状态。
 */
int main(int argc,char **argv){{
 {setup};{scalar}(*volatile kernel)(int64_t)={kernel};
 if(argc>1 && strcmp(argv[1],"--verify")==0){{for(int s=0;s<5;s++)printf("value %d {fmt}\\n",s,({cast})kernel(s));{finish};return 0;}}
 volatile {scalar} sink=kernel(0);
 for(int64_t s=0;s<5;s++){{int64_t start=bench_now();{scalar} value=kernel(s);int64_t elapsed=bench_now()-start;sink={merge};printf("rep %lld\\n",(long long)elapsed);}}
 printf("sink {fmt}\\n",({cast})sink);{finish};return 0;
}}
'''


def rust_timer(fp=False):
    """使用 black_box 保护入口和校验值；不改变被测内核。"""
    merge='sink*0.5+value' if fp else 'sink+value'
    return f'''
fn main(){{
 let kernel=std::hint::black_box(work as fn(i64)->{'f64' if fp else 'i64'});
 if std::env::args().any(|a|a=="--verify"){{for s in 0..5{{println!("value {{}} {{:.17}}",s,kernel(s));}}return;}}
 let mut sink=kernel(std::hint::black_box(0));
 for s in 0..5{{let start=std::time::Instant::now();let value=kernel(std::hint::black_box(s));let elapsed=start.elapsed().as_nanos();sink={merge};println!("rep {{}}",elapsed);}}
 println!("sink {{:.17}}",std::hint::black_box(sink));
}}
'''


def lua_timer(fp, verify):
    """把 Lua 官方解释器提供的时钟放在相同工作函数调用两侧。"""
    if verify:return '\nfor s=0,4 do print(string.format("value %d %.17g",s,work(s))) end\n'
    merge='sink*0.5+value' if fp else 'sink+value'
    return f'\nlocal sink=work(0)\nfor s=0,4 do local start=now_ns();local value=work(s);local elapsed=now_ns()-start;sink={merge};print("rep "..elapsed) end\nprint(string.format("sink %.17g",sink))\n'


def build_all(args, out, scratch):
    """一次性完成所有语言构建、逐种子核对和工具身份采集，正式计时不混入构建。"""
    compiler=args.chali.resolve()
    sdk=ROOT/'build/dependencies/llvm/static-sdk/bin'
    tools={k:shutil.which(v) for k,v in dict(c='gcc',cpp='g++',rust='rustc',java='java',javac='javac',go='go',python='python').items()}
    if os.name=='nt' and (sdk/'gcc.exe').exists():tools['c']=str(sdk/'gcc.exe');tools['cpp']=str(sdk/'g++.exe')
    native_options=['--sysroot='+str(sdk.parent)] if os.name=='nt' and Path(tools['c']).parent==sdk else []
    clang=ROOT/'build/dependencies/llvm/toolchain/bin/clang.exe' if os.name=='nt' else Path(shutil.which('clang') or '')
    if any(not tools[k] for k in tools):raise RuntimeError(('Missing toolchain',tools))
    if shutil.which('rustup'):
        resolved=execute(['rustup','which','rustc'])['stdout'].strip()
        if Path(resolved).is_file():tools['rust']=resolved
    lua_source=ROOT/'build/dependencies/lua/lua-5.5.1-git'
    lua_build=None
    if 'lua' in args.languages:
        if not lua_source.exists():execute(['git','clone','--depth','1','--branch','v5.5.1','https://github.com/lua/lua.git',lua_source])
        if execute(['git','-C',lua_source,'rev-parse','HEAD'])['stdout'].strip()!=LUA_REVISION:raise RuntimeError('Lua source revision mismatch')
        execute(['git','-C',lua_source,'diff','--exit-code'])
        files=[p for p in sorted(lua_source.glob('*.c')) if p.name not in ('lua.c','onelua.c','ltests.c')]
        source_hashes={p.name:digest(p) for p in [*files,*lua_source.glob('*.h')]}
        lua_command=[tools['c'],*native_options,'-O3','-flto','-DNDEBUG','-static','-I',str(lua_source),str(ROOT/'benchmarks/perf/luabench.c'),*map(str,files),'-lm','-o',str(LUA_HOST)]
        if os.name!='nt':lua_command.extend(['-DLUA_USE_LINUX','-ldl'])
        LUA_HOST.parent.mkdir(parents=True,exist_ok=True)
        receipt=LUA_HOST.with_suffix('.json')
        identity=dict(revision=LUA_REVISION,source_sha256=source_hashes,host_source_sha256=digest(ROOT/'benchmarks/perf/luabench.c'),command=lua_command,compiler_sha256=digest(tools['c']))
        if not LUA_HOST.exists() or not receipt.exists() or json.loads(receipt.read_text(encoding='utf-8'))['identity']!=identity:
            lua_build=execute(lua_command);save(receipt,dict(identity=identity,build=lua_build))
        lua_build=json.loads(receipt.read_text(encoding='utf-8'))
    tools.update(chali=str(compiler),lua=str(LUA_HOST),clang=str(clang))
    metadata={}
    for k,v in tools.items():
        flag=['version'] if k=='go' else ['-version'] if k in ('java','javac') else ['--build-info'] if k=='chali' else ['--version']
        row=execute([v,*flag]);metadata[k]=dict(path=v,sha256=digest(v),version=(row['stdout']+row['stderr']).strip())
    machine=execute(['powershell','-NoProfile','-Command','Get-CimInstance Win32_Processor | Select-Object Name,NumberOfCores,NumberOfLogicalProcessors | ConvertTo-Json -Compress'])['stdout'].strip() if os.name=='nt' else platform.processor()
    report=dict(schema=1,started=datetime.now(timezone(timedelta(hours=8))).isoformat(),platform=platform.platform(),machine=machine,
                logical_cpus=os.cpu_count(),compiler_source_commit=args.compiler_source_commit,compiler_sha256=digest(compiler),
                benchmark_commit=execute(['git','rev-parse','HEAD'])['stdout'].strip(),
                working_tree_dirty=bool(execute(['git','status','--porcelain'])['stdout'].strip()),
                tools=metadata,cases=args.cases,languages=args.languages,runs=args.runs,reps_per_process=5,
                allocator='malloc',warmup='one native/interpreter call; Java >=32 calls and >=1 second per process',
                policy='all combinations rerun; serial sampling; no historical reference or performance verdict',
                builds={},oracle_values={},results={},source_sha256={},lua_source=lua_build)
    files=[*HERE.glob('*'),*[(ROOT/'benchmarks/perf')/(c+ext) for c in BASIC for ext in ('.chl','.c','.cpp','.rs','.lua')],
           *list((ROOT/'benchmarks/features').glob('*')),*list((ROOT/'benchmarks/strings').glob('cases.*')),
           ROOT/'benchmarks/perf/hash_int.h',ROOT/'benchmarks/perf/luabench.c',ROOT/'library/lua/manifest.json']
    for p in files:
        if p.is_file():report['source_sha256'][p.relative_to(ROOT).as_posix()]=digest(p)
    save(out/'results.json',report)
    java_dir=scratch/'java';java_dir.mkdir(exist_ok=True)
    go_bin=scratch/('go.exe' if os.name=='nt' else 'go')
    shared_builds={}
    if 'java' in args.languages:shared_builds['java']=execute([tools['javac'],'-encoding','UTF-8','-d',java_dir,HERE/'Kernels.java'])
    if 'go' in args.languages:shared_builds['go']=execute([tools['go'],'build','-o',go_bin,HERE/'kernels.go',HERE/('clock_windows.go' if os.name=='nt' else 'clock_other.go')])
    report['shared_builds']=shared_builds
    for case in args.cases:
        fp=case=='float_chain'
        for lang in args.languages:
            print('BUILD',case,lang,flush=True)
            folder=scratch/lang/case;folder.mkdir(parents=True,exist_ok=True)
            product=folder/('bench.exe' if os.name=='nt' else 'bench')
            commands=[];artifacts=[]
            def command(values):commands.append(execute(values))
            if lang in ('java','go','python'):
                cmd=[tools['java'],'-Xbatch','-cp',str(java_dir),'Kernels',case] if lang=='java' else [str(go_bin),case] if lang=='go' else [sys.executable,str(HERE/'kernels.py'),case]
                verify=cmd+['--verify']
            elif lang=='chali':
                prefix=folder/'kernel';driver=folder/'driver.c'
                if case in BASIC:
                    src=(ROOT/'benchmarks/perf'/f'{case}.chl').read_text(encoding='utf-8').split('void main()',1)[0].replace('import std::clock;','')
                    src=re.sub(r'const int64 REPS = 5;','',src)
                    src=re.sub(r'(?m)^(int64|float64) work\(',r'#export("f_work") \1 work(',src)
                    path=folder/'kernel.chl';path.write_text(src,encoding='utf-8')
                    timer=c_timer(fp,'f_work','chali_init()','chali_finish()')
                else:
                    path=ROOT/'benchmarks/features'/f'{case}.chl' if case in FEATURES else ROOT/'benchmarks/strings/cases.chl'
                    timer=c_timer(False,'feature_work' if case in FEATURES else case,'chali_init();feature_setup()','chali_finish()')
                command([compiler,'build',path,'--shared','--unchecked','--allocator','malloc','--out',prefix])
                library=prefix.with_suffix('.dll' if os.name=='nt' else '.so')
                driver.write_text('#include "kernel.h"\n'+timer,encoding='utf-8')
                command([tools['clang'],'-O3','-DNDEBUG',driver,library,'-o',product])
                artifacts+=[prefix.with_suffix('.ll'),library,driver]
                cmd=[str(product)];verify=cmd+['--verify']
            elif lang in ('c','cpp'):
                common=[*native_options,'-O3','-flto','-DNDEBUG','-static']
                if lang=='cpp':common+=['-std=c++17']
                if case in BASIC:
                    original=ROOT/'benchmarks/perf'/f'{case}.{lang if lang=="cpp" else "c"}'
                    src=original.read_text(encoding='utf-8')
                    match=re.search(r'(?:static\s+)?(?:std::int64_t|long long|int64_t)\s+now_ns\s*\(',src)
                    if not match:raise ValueError(('clock function',original))
                    src=src[:match.start()]
                    if re.search(r'namespace\s*\{',src):src+='\n}\n'
                    path=folder/('core.cpp' if lang=='cpp' else 'core.c')
                    path.write_text(src+'\n#undef N\n#undef REPS\n'+c_timer(fp),encoding='utf-8')
                    command([tools[lang],*common,'-I',ROOT/'benchmarks/perf',path,'-o',product]);artifacts.append(path)
                else:
                    source=HERE/'extra.c' if lang=='c' else ROOT/'benchmarks/features'/f'{case}.cpp' if case in FEATURES else ROOT/'benchmarks/strings/cases.cpp'
                    options=['-DSELECT='+str((*FEATURES,*STRINGS).index(case))] if lang=='c' else ['-Dfeature_work='+case] if case in STRINGS else []
                    command([tools[lang],*common,*options,source,ROOT/'benchmarks/features/timing.c','-o',product])
                cmd=[str(product)];verify=cmd+['--verify']
            elif lang=='rust':
                extra=case in ('array_rw','interface_mono','interface_poly')
                options=['--edition=2021','-C','opt-level=3','-C','lto=fat','-C','codegen-units=1']
                if extra:
                    source=HERE/'extra.rs';options+=['--cfg',case]
                elif case in BASIC:
                    src=(ROOT/'benchmarks/perf'/f'{case}.rs').read_text(encoding='utf-8').split('fn main()',1)[0]
                    source=folder/'core.rs';source.write_text(src+rust_timer(fp),encoding='utf-8');artifacts.append(source)
                else:
                    source=ROOT/'benchmarks/features'/f'{case}.rs' if case in FEATURES else ROOT/'benchmarks/strings/cases.rs'
                    if case in STRINGS:options+=['--cfg',case]
                command([tools['rust'],*options,source,'-o',product]);cmd=[str(product)];verify=cmd+['--verify']
            elif lang=='lua':
                if case in BASIC:src=(ROOT/'benchmarks/perf'/f'{case}.lua').read_text(encoding='utf-8').split('local sink',1)[0]
                else:
                    source=HERE/'extra.lua' if case in FEATURES else ROOT/'benchmarks/strings/cases.lua'
                    src='BENCH_CASE="'+case+'"\n'+source.read_text(encoding='utf-8')
                    if case in STRINGS:src+='\nsetup()\n'
                path=folder/'run.lua';checked=folder/'verify.lua'
                path.write_text(src+lua_timer(fp,False),encoding='utf-8');checked.write_text(src+lua_timer(fp,True),encoding='utf-8')
                cmd=[str(LUA_HOST),str(path)];verify=[str(LUA_HOST),str(checked)];artifacts.extend([path,checked])
            row=execute(verify)
            if row['stderr']:raise RuntimeError(row)
            values=[float(v) if fp else int(float(v)) for _,v in re.findall(r'^value (\d+) (\S+)$',row['stdout'],re.M)]
            if len(values)!=5:raise RuntimeError(('missing seed verification',case,lang,row))
            report['oracle_values'].setdefault(case,values)
            expected=report['oracle_values'][case]
            if any(not math.isclose(x,y,rel_tol=1e-12,abs_tol=1e-12) if fp else x!=y for x,y in zip(values,expected)):
                raise RuntimeError(('cross-language mismatch',case,lang,values,expected))
            if lang=='java':artifacts.extend(java_dir.rglob('*.class'))
            elif lang=='go':artifacts.append(go_bin)
            elif lang=='python':artifacts.append(HERE/'kernels.py')
            if product.exists():artifacts.append(product)
            report['builds'][case+'/'+lang]=dict(commands=commands,command=cmd,verify=row,values=values,
                artifacts={str(p):digest(p) for p in artifacts},folder=str(folder))
            report['results'][case+'/'+lang]=[]
            save(out/'results.json',report)
    report['build_complete']=True;save(out/'results.json',report)


def measure(args,out):
    """串行运行，逐轮改变语言先后顺序；保留所有有效样本，不删慢值。"""
    report=json.loads((out/'results.json').read_text(encoding='utf-8'))
    if not report.get('build_complete'):raise ValueError('build phase incomplete')
    if report.get('completed'):
        print('Already completed; report unchanged',flush=True)
        return
    report.setdefault('sampling_started',datetime.now(timezone(timedelta(hours=8))).isoformat())
    rng=random.Random(20261106)
    report['scheduling_seed']=20261106
    report.setdefault('schedule',[])
    for round_index in range(report['runs']):
        cases=list(report['cases']);rng.shuffle(cases)
        for case in cases:
            languages=list(report['languages']);rng.shuffle(languages)
            for lang in languages:
                key=case+'/'+lang
                if len(report['results'][key])>round_index:continue
                print('MEASURE',round_index+1,case,lang,flush=True)
                artifact=report['builds'][key]
                for p,h in artifact['artifacts'].items():
                    if digest(p)!=h:raise RuntimeError(('artifact changed',p))
                row=execute(artifact['command'],timeout=600)
                samples=[int(x) for x in re.findall(r'^rep (\d+)$',row['stdout'],re.M)]
                sink=re.findall(r'^sink (\S+)$',row['stdout'],re.M)
                if len(samples)!=5 or min(samples)<=0 or len(sink)!=1 or row['stderr']:raise RuntimeError(row)
                values=report['oracle_values'][case]
                expected=values[0]
                for value in values:expected=expected*0.5+value if case=='float_chain' else expected+value
                observed=float(sink[0]) if case=='float_chain' else int(float(sink[0]))
                if not math.isclose(observed,expected,rel_tol=1e-12,abs_tol=1e-12):raise RuntimeError(('sink',key,observed,expected))
                row.update(round=round_index,samples_ns=samples,sink=sink[0],timestamp=datetime.now(timezone(timedelta(hours=8))).isoformat())
                report['results'][key].append(row)
                report['schedule'].append([round_index,case,lang])
                save(out/'results.json',report)
    report['summary']={}
    for key,rows in report['results'].items():
        samples=[n for row in rows for n in row['samples_ns']]
        report['summary'][key]=dict(median_ms=statistics.median(samples)/1e6,min_ms=min(samples)/1e6,max_ms=max(samples)/1e6,samples=len(samples))
    report['completed']=datetime.now(timezone(timedelta(hours=8))).isoformat();save(out/'results.json',report)
    print('COMPLETE',len(report['summary']),'combinations',sum(x['samples'] for x in report['summary'].values()),'observations',flush=True)


def main():
    """构建与计时可以分开执行；新报告使用独立输出目录。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,required=True)
    parser.add_argument('--compiler-source-commit',required=True)
    parser.add_argument('--out',type=Path,required=True)
    parser.add_argument('--runs',type=int,default=5)
    parser.add_argument('--cases',default=','.join(CASES))
    parser.add_argument('--languages',default=','.join(LANGUAGES))
    mode=parser.add_mutually_exclusive_group();mode.add_argument('--build-only',action='store_true');mode.add_argument('--measure-only',action='store_true')
    args=parser.parse_args();args.cases=args.cases.split(',');args.languages=args.languages.split(',')
    if args.runs<1 or not set(args.cases)<=set(CASES) or not set(args.languages)<=set(LANGUAGES):parser.error('invalid selection')
    out=args.out.resolve();out.mkdir(parents=True,exist_ok=True)
    temporary=ROOT/'tmp';temporary.mkdir(exist_ok=True)
    for name in ('TMPDIR','TMP','TEMP'):os.environ[name]=str(temporary)
    if not args.measure_only:
        if (out/'results.json').exists():parser.error('report exists; choose another output directory')
        scratch=temporary/('current-benchmark-'+str(os.getpid()));scratch.mkdir()
        build_all(args,out,scratch)
    if not args.build_only:measure(args,out)


if __name__=='__main__':main()
