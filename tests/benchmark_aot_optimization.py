"""在同一机器交错重测优化前后 AOT 与 JDK 8/22，保留逐次耗时与校验和。"""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import platform
import statistics
import subprocess

ROOT=Path(__file__).resolve().parents[1]
WORK=ROOT/'build/aot-optimization-benchmark'
KERNELS=('scalar','fixed_array','heap_array','container','function_calls')


def main():
    """使用固定优化级别、运行时输入、独立预热与交错顺序进行对照。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--magel',type=Path,default=ROOT/'magel.exe')
    parser.add_argument('--baseline-executable',type=Path,required=True)
    parser.add_argument('--baseline-commit',default='12c4dc4')
    parser.add_argument('--jdk8',type=Path,required=True)
    parser.add_argument('--jdk22',type=Path,required=True)
    parser.add_argument('--iterations',type=int,default=200000)
    parser.add_argument('--rounds',type=int,default=3)
    parser.add_argument('--repeats',type=int,default=3)
    parser.add_argument('--skip-build',action='store_true')
    parser.add_argument('--out',type=Path,default=ROOT/'doc/benchmarks/aot-optimization-2026-10-16.json')
    args=parser.parse_args();assert min(args.iterations,args.rounds,args.repeats)>0
    env=dict(os.environ,MAGEL_LANG='zh',MAGEL_NO_STDIN_INPUT='0')
    WORK.mkdir(parents=True,exist_ok=True)

    def run(command,**kwargs):
        """执行测试命令，失败时保存完整输出。"""
        result=subprocess.run(list(map(str,command)),cwd=ROOT,env=env,capture_output=True,encoding='utf-8',errors='replace',timeout=900,**kwargs)
        assert result.returncode==0,(command,result.stdout,result.stderr)
        return result

    compiler=args.magel.resolve();after=WORK/'after.exe';baseline=args.baseline_executable.resolve()
    source=ROOT/'tests/fixtures/aot_optimization_benchmark.mgl'
    if not args.skip_build:
        result=run([compiler,'aot',source,'--out',after,'--opt-level=3','--checked','--keep-intermediate'])
        (WORK/'after-build.log').write_text(result.stdout+result.stderr,encoding='utf-8')
    java_source=ROOT/'tests/fixtures/AotOptimizationBenchmark.java'
    for label,jdk,expected in [('jdk8',args.jdk8, '1.8.'),('jdk22',args.jdk22,'22.')]:
        jdk=jdk.resolve();version=run([jdk/'bin/java.exe','-version']);assert expected in version.stderr,version
        dest=WORK/label/'classes';dest.mkdir(parents=True,exist_ok=True)
        run([jdk/'bin/javac.exe','-encoding','UTF-8','-d',dest,java_source])
    commands={'before':[baseline],'after':[after]}
    versions={}
    for label,jdk in [('jdk8',args.jdk8),('jdk22',args.jdk22)]:
        jdk=jdk.resolve();versions[label]=run([jdk/'bin/java.exe','-version']).stderr.strip()
        commands[label]=[jdk/'bin/java.exe','-Xms256m','-Xmx256m','-Xbatch','-cp',WORK/label/'classes','AotOptimizationBenchmark']
    value=1
    for i in range(args.iterations):value=(value*17+i)%1000000007
    expected={k:(value if k in ('scalar','function_calls') else args.iterations*(args.iterations+1)//2) for k in KERNELS}
    samples={label:{k:[] for k in KERNELS} for label in commands};raw=[]
    labels=list(commands)
    for round_index in range(args.rounds):
        order=labels[round_index%len(labels):]+labels[:round_index%len(labels)]
        for label in order:
            result=run(commands[label],input=f'{args.iterations}\n{args.repeats}\n')
            lines=result.stdout.splitlines();assert len(lines)==len(KERNELS)*args.repeats,(label,result)
            observed=[]
            for line in lines:
                kernel,nanos,checksum=line.split();nanos=int(nanos);checksum=int(checksum)
                assert checksum==expected[kernel] and nanos>0,(label,line,expected)
                samples[label][kernel].append(nanos/1e6);observed.append({'kernel':kernel,'nanoseconds':nanos,'checksum':checksum})
            raw.append({'round':round_index,'implementation':label,'samples':observed})
            print('completed',round_index,label,flush=True)
    medians={label:{k:statistics.median(v) for k,v in data.items()} for label,data in samples.items()}
    report={'date':datetime.datetime.now(datetime.timezone(datetime.timedelta(hours=8))).isoformat(),'version':(ROOT/'VERSION').read_text().strip(),'baseline_commit':args.baseline_commit,'candidate_base':run(['git','rev-parse','HEAD']).stdout.strip(),'candidate_dirty':run(['git','status','--porcelain']).stdout.splitlines(),'platform':platform.platform(),'cpu':platform.processor(),'host_compiler':run(['g++','--version']).stdout.splitlines()[0],'java_versions':versions,'iterations':args.iterations,'rounds':args.rounds,'repeats_per_round':args.repeats,'configuration':{'aot':'-O3 --checked on both versions; default reflection enabled; full quad optimizer; no unsafe fast-math','java':'-Xms256m -Xmx256m -Xbatch; 20 warmup iterations per process','timing':'monotonic nanoseconds around each kernel only; excludes startup, compilation, parsing and printing; includes kernel array/list initialization and cleanup','aot_warmup':1,'java_array_note':'Java fixed_array and heap_array both use long[64]; ArrayList<Long> uses boxed elements, Magel uses List<int64>; results compare algorithms, not identical layouts'},'source_sha256':hashlib.sha256(source.read_bytes()).hexdigest(),'java_source_sha256':hashlib.sha256(java_source.read_bytes()).hexdigest(),'binary_sha256':{label:hashlib.sha256(path.read_bytes()).hexdigest() for label,path in [('before',baseline),('after',after)]},'binary_sizes':{'before':baseline.stat().st_size,'after':after.stat().st_size},'expected_checksums':expected,'samples_ms':samples,'median_ms':medians,'speedup_before_over_after':{k:medians['before'][k]/medians['after'][k] for k in KERNELS},'raw':raw}
    report['compiler_sha256']=hashlib.sha256(compiler.read_bytes()).hexdigest()
    report['optimization_source_sha256']={name:hashlib.sha256((ROOT/name).read_bytes()).hexdigest() for name in ['source/magel-emit/aot.cpp','source/magel-emit/main.cpp','library/stackvm/stackvm.hpp','library/stackvm/parts/aot_bridge.cpp']}
    release=WORK/'jdk8-release.json'
    if release.exists():
        metadata=json.loads(release.read_text(encoding='utf-8'))
        archive=next(asset for asset in metadata['assets'] if asset['name'].startswith('OpenJDK8U-jdk_x64_windows_hotspot_') and asset['name'].endswith('.zip'))
        report['jdk8_distribution']={'release':metadata['tag_name'],'url':archive['browser_download_url'],'archive_sha256':hashlib.sha256((WORK/archive['name']).read_bytes()).hexdigest(),'checksum_verified':True}
    args.out.parent.mkdir(parents=True,exist_ok=True);args.out.write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    print(json.dumps({'median_ms':medians,'speedup':report['speedup_before_over_after']},indent=2),flush=True)


if __name__=='__main__':main()
