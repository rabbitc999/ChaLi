"""对两个真实编译器构件交错采样接口成本，并可单独关闭构造标量入口。"""
from pathlib import Path
import argparse
import json
import os
import random
import re
import shutil
import statistics
import subprocess

from run import CASES, ROOT, digest, driver, now, optimized_functions, oracle, save, source
from project_temp import tempfile


def main():
    """先构建及逐输入核对，再以相邻的前后构件运行同一用例，保留全部样本。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--before', type=Path, required=True)
    parser.add_argument('--after', type=Path, required=True)
    parser.add_argument('--before-source-commit')
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--runs', type=int, default=5)
    parser.add_argument('--iterations', type=int, default=262144)
    parser.add_argument('--cases', default=','.join(CASES))
    parser.add_argument('--with-generic-construction', action='store_true')
    parser.add_argument('--allocator', choices=('malloc','pool'), default='malloc')
    parser.add_argument('--clang', type=Path, default=ROOT/'build/dependencies/llvm/toolchain/bin/clang.exe' if os.name == 'nt' else None)
    parser.add_argument('--cc', type=Path, default=ROOT/'build/dependencies/llvm/static-sdk/bin/gcc.exe' if os.name == 'nt' else None)
    args = parser.parse_args()
    cases = args.cases.split(',')
    if args.runs < 1 or args.iterations < 1 or len(set(cases)) != len(cases) or not set(cases) <= set(CASES):
        parser.error('无效的轮数、循环规模或用例')
    out = args.out.resolve()
    if not out.is_relative_to(ROOT/'build/validation') or (out.exists() and any(out.iterdir())):
        parser.error('--out 必须为 build/validation 下新的空目录')
    out.mkdir(parents=True,exist_ok=True)
    (out/'analysis').mkdir()
    cc = args.cc.resolve() if args.cc else Path(shutil.which('cc') or 'cc')
    clang = args.clang.resolve() if args.clang else Path(shutil.which('clang') or 'clang')
    env = dict(os.environ,CHALI_QUIET='1',CHALI_LANG='zh')
    for name in ('CHALI_BACKEND_PROFILE','CHALI_BACKEND_PROFILE_OUTPUT','CHALI_NO_AOT_OBJECT_CONSTRUCTION_SCALARS'):
        env.pop(name,None)
    cc_env = {'PATH':str(cc.parent)+os.pathsep+env.get('PATH','')}
    configs = {'before':args.before.resolve(),'after':args.after.resolve()}
    if args.with_generic_construction:
        configs['after_generic_construction'] = args.after.resolve()
    report = dict(schema=1,started=now(),iterations=args.iterations,runs=args.runs,allocator=args.allocator,
                  before_source_commit=args.before_source_commit,commands=[],compilers={},builds={},
                  cases={name:dict(function=CASES[name][0],group=CASES[name][1],description=CASES[name][2]) for name in cases},
                  results={},schedule=[],policy='同一用例的构件顺序随机交错；不剔除慢样本；无插桩计时')

    def execute(command, extra=None):
        """记录完整命令、输出及退出码，构建失败时停止并保留检查点。"""
        command = list(map(str,command))
        result = subprocess.run(command,cwd=ROOT,env=dict(env,**(extra or {})),capture_output=True,
                                text=True,encoding='utf-8',errors='replace',timeout=600)
        row = dict(command=command,code=result.returncode,stdout=result.stdout,stderr=result.stderr)
        report['commands'].append(row)
        save(out/'results.json',report)
        if result.returncode:
            raise RuntimeError(row)
        return result.stdout

    report['source_base_commit'] = execute(['git','rev-parse','HEAD']).strip()
    report['source_diff'] = execute(['git','diff','--','source/chali_aot.cpp','source/aot/native_runtime.cpp',
        'source/ircode/native_memory.cpp','source/ircode/native_memory.hpp','source/ircode/native_objects.cpp',
        'source/ircode/native_objects.hpp','source/ircode/native_closure.cpp','source/ircode/object_store.cpp',
        'source/ircode/runtime_objects.hpp'])
    report['cc_version'] = execute([cc,'--version'],cc_env)
    report['clang_version'] = execute([clang,'--version'])
    report['source_sha256'] = dict(compare=digest(__file__),generator=digest(Path(__file__).with_name('run.py')),
                                   original=digest(ROOT/'benchmarks/features/interface_poly.chl'))
    if os.name == 'nt':
        report['machine'] = execute(['powershell','-NoProfile','-Command',
            'Get-CimInstance Win32_Processor | Select-Object Name,NumberOfCores,NumberOfLogicalProcessors | ConvertTo-Json -Compress']).strip()
    expected = {group:oracle(group,args.iterations) for group in ('selection','mixed','mono')}
    report['oracle'] = expected
    with tempfile.TemporaryDirectory(prefix='interface-compare-') as temporary:
        work = Path(temporary)
        products = {}
        program = source(args.iterations)[1]
        (out/'kernel.chl').write_text(program,encoding='utf-8')
        for label,compiler in configs.items():
            print('BUILD',label,flush=True)
            report['compilers'][label] = dict(path=str(compiler),sha256=digest(compiler),
                build_info=json.loads(execute([compiler,'--build-info'])))
            folder = work/label
            folder.mkdir()
            path = folder/'kernel.chl'
            path.write_text(program,encoding='utf-8')
            prefix = folder/'kernel'
            extra = {'CHALI_NO_AOT_OBJECT_CONSTRUCTION_SCALARS':'1'} if label == 'after_generic_construction' else {}
            execute([compiler,'build',path,'--shared','--unchecked','--allocator',args.allocator,'--out',prefix],extra)
            library = prefix.with_suffix('.dll' if os.name == 'nt' else '.so')
            c = folder/'driver.c'
            c.write_text(driver(list(CASES)),encoding='utf-8')
            exe = folder/('bench.exe' if os.name == 'nt' else 'bench')
            options = ['--sysroot='+str(cc.parent.parent)] if os.name == 'nt' and (cc.parent.parent/'chali-sdk.json').exists() else []
            execute([cc,*options,'-O3','-DNDEBUG',*(['-static'] if os.name == 'nt' else []),c,library,'-o',exe],cc_env)
            products[label] = exe
            report['builds'][label] = dict(library_sha256=digest(library),exe_sha256=digest(exe),compile_env=extra)
            optimized = out/'analysis'/(label+'.ll')
            execute([clang,'-O3','-S','-emit-llvm',prefix.with_suffix('.ll'),'-o',optimized])
            llvm = optimized.read_text(encoding='utf-8')
            assert '@chali_backend_profile_event(' not in llvm
            functions = optimized_functions(llvm,{name:CASES[name][0] for name in CASES})
            for name,item in functions.items():
                (folder/(name+'.ll')).write_text(item.pop('body')+'\n',encoding='utf-8')
            report['builds'][label]['optimized_exports'] = functions
            call_lines = '\n'.join(line for line in llvm.splitlines() if 'call ' in line)
            report['builds'][label]['integer_set_callsites'] = call_lines.count('@chali_host_object_integer_set(')
            report['builds'][label]['create_callsites'] = call_lines.count('@chali_host_object_new(')
            for case in cases:
                result = execute([exe,list(CASES).index(case),'--verify'])
                values = [int(v) for _,v in re.findall(r'^value (\d+) (-?\d+)$',result,re.M)]
                assert values == expected[CASES[case][1]],(label,case,result)
        for label,compiler in configs.items():
            assert digest(compiler) == report['compilers'][label]['sha256']
        print('VERIFY complete; SERIAL INTERLEAVED TIMING',flush=True)
        rng = random.Random(20261107)
        for round_index in range(args.runs):
            order = list(cases)
            rng.shuffle(order)
            for case in order:
                labels = list(configs)
                rng.shuffle(labels)
                for label in labels:
                    exe = products[label]
                    assert digest(exe) == report['builds'][label]['exe_sha256']
                    assert digest(exe.parent/('kernel.dll' if os.name == 'nt' else 'kernel.so')) == report['builds'][label]['library_sha256']
                    output = execute([exe,list(CASES).index(case)])
                    assert not report['commands'][-1]['stderr']
                    samples = [(int(s),int(t),int(v)) for s,t,v in re.findall(r'^rep (\d+) (\d+) (-?\d+)$',output,re.M)]
                    values = expected[CASES[case][1]]
                    assert len(samples)==5 and [s for s,_,_ in samples]==list(range(5))
                    assert all(t>0 and v==values[s] for s,t,v in samples),(label,case,output)
                    assert re.search(r'^sink '+str(values[0]+sum(values))+r'$',output,re.M),(label,case,output)
                    key=label+'/'+case
                    report['results'].setdefault(key,[]).append(dict(round=round_index,timestamp=now(),samples=samples))
                    report['schedule'].append([round_index,label,case])
                    save(out/'results.json',report)
            print('MEASURE round',round_index+1,'/',args.runs,flush=True)
        report['summary'] = {}
        for key,rows in report['results'].items():
            values = [t for row in rows for _,t,_ in row['samples']]
            q = statistics.quantiles(values,n=4)
            report['summary'][key] = dict(samples=len(values),median_ms=statistics.median(values)/1e6,
                q1_ms=q[0]/1e6,q3_ms=q[2]/1e6,min_ms=min(values)/1e6,max_ms=max(values)/1e6,
                process_medians_ms=[statistics.median(t for _,t,_ in row['samples'])/1e6 for row in rows])
        shutil.copytree(work,out/'artifacts')
        report['artifact_sha256'] = {p.relative_to(out).as_posix():digest(p) for p in (out/'artifacts').rglob('*') if p.is_file()}
        report['completed'] = now()
        save(out/'results.json',report)
        for case in cases:
            print(case,{label:round(report['summary'][label+'/'+case]['median_ms'],4) for label in configs},flush=True)
    print('Evidence:',out,flush=True)


if __name__ == '__main__':
    main()
