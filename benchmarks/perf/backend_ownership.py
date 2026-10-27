"""同一编译器对照 P0～P2 优化开关，分离 AOT 计时与所有权计数。"""
from pathlib import Path
import argparse
from collections import Counter
from datetime import datetime, timezone
import hashlib
import json
import os
import platform
import statistics
import subprocess
import sys
import time

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'tools'))
from compiler_path import compiler_path
from project_temp import tempfile
from release import compiler_version

DISABLE = {name:'1' for name in ['CHALI_NO_ARRAY_COPY_BULK','CHALI_NO_AOT_BULK_COPY',
           'CHALI_NO_AOT_ADDRESS_CACHE','CHALI_NO_AOT_INLINE_HINT','CHALI_NO_AOT_DIRECT_CALLS']}


def main():
    """先编译全部产物，交错计时；计时结束后另行执行插桩产物。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=compiler_path())
    parser.add_argument('--operations',type=int,default=1000)
    parser.add_argument('--runs',type=int,default=3)
    parser.add_argument('--cases',nargs='+',choices=['return_chain','aggregate_copy','enum_payload',
                        'scalar_array_copy','field_read','function_value','small_method','segmented_list'])
    parser.add_argument('--out',type=Path,required=True)
    args=parser.parse_args()
    if args.operations<1 or args.runs<1:parser.error('operations 和 runs 必须为正数')
    compiler=args.chali.resolve();n=args.operations
    output=args.out.resolve();output.parent.mkdir(parents=True,exist_ok=True)
    work=Path(tempfile.mkdtemp(prefix='backend-ownership-perf-'))
    prefix=(ROOT/'example/samples/backend_ownership.chl').read_text(encoding='utf-8').split('void main()')[0]
    segmented=(ROOT/'benchmarks/perf/bootstrap_list_segmented.chl').read_text(encoding='utf-8')
    bodies={
        'return_chain': ('int64 total=0;', 'Parcel value=forwardParcel(i+seed);total=total+value.data[0];'),
        'aggregate_copy': ('Parcel source=makeParcel(seed);int64 total=0;', 'Parcel value=duplicateParcel(&source);total=total+value.data[0];'),
        'enum_payload': ('int64 total=0;', 'Envelope value=wrapParcel(i+seed);match(value){Envelope::Empty=>{} Envelope::Filled(item)=>{total=total+item.data[0];}}'),
        'scalar_array_copy': ('int64 source[]=new[512];int64 j=0;while(j<512){source[j]=j+seed;j=j+1;}int64 total=0;', 'int64 values[]=source;total=total+values[i%512];'),
        'field_read': ('Parcel value=makeParcel(seed);int64 total=0;', 'total=total+readParcel(&value);'),
        'function_value': ('int64 total=0;', 'int64 fn(int64) operation=increment;total=total+operation(i+seed);'),
        'small_method': ('PerfPoint value;value.x=seed;value.y=7;int64 total=0;', 'total=total+value.sum();'),
        'segmented_list': ('BootstrapList<int64> values;int64 j=0;while(j<'+str(n)+'){values.push(j+seed);j=j+1;}int64 total=0;', 'total=total+values.pop();'),
    }
    report=dict(version=compiler_version(compiler),compiler_sha256=hashlib.sha256(compiler.read_bytes()).hexdigest(),
                generated_utc=datetime.now(timezone.utc).isoformat(),host=platform.platform(),processor=platform.processor(),
                operations=n,runs=args.runs,repetitions=5,warmups=1,optimization='LLVM O3; full quad budget; checked; reflection enabled',
                baseline_flags=DISABLE,cases={},execution_order=[],work=str(work))
    def save():
        """逐步保存原始证据，避免失败丢失已完成结果。"""
        output.write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    def execute(command,env,timeout=600):
        """保存输出和进程时间；失败或诊断不混入正常计时。"""
        start=time.perf_counter_ns()
        x=subprocess.run(list(map(str,command)),cwd=ROOT,env=env,capture_output=True,text=True,encoding='utf-8',timeout=timeout)
        assert x.returncode==0 and not x.stderr,(command,x.returncode,x.stdout,x.stderr)
        return dict(command=list(map(str,command)),stdout=x.stdout,stderr=x.stderr,wall_ns=time.perf_counter_ns()-start)
    envbase=dict(os.environ,CHALI_LANG='zh')
    for name in [*DISABLE,'CHALI_BACKEND_PROFILE','CHALI_BACKEND_PROFILE_OUTPUT']:envbase.pop(name,None)
    for name,(setup,body) in bodies.items():
        if args.cases and name not in args.cases:continue
        cycles,rest=divmod(n,512)
        expected={'return_chain':6*n*(n-1)//2+10*n,'aggregate_copy':10*n,'enum_payload':6*n*(n-1)//2+10*n,
                  'scalar_array_copy':6*(cycles*512*511//2+rest*(rest-1)//2)+10*n,'field_read':20*n,
                  'function_value':6*n*(n-1)//2+16*n,'small_method':52*n,'segmented_list':6*n*(n-1)//2+10*n}[name]
        source='import std::clock;\n'+prefix+'\n'+segmented+'\nstruct PerfPoint{int64 x;int64 y;int64 sum(){return self.x+self.y;}};\n'
        source+='int64 work(int64 seed){'+setup+'int64 i=0;while(i<'+str(n)+'){'+body+'i=i+1;}return total;}\n'
        source+='void main(){int64 sink=work(0);int64 times[5];int64 r=0;while(r<5){int64 start=std::clock::nowNanos();sink=sink+work(r);times[r]=std::clock::nowNanos()-start;r=r+1;}r=0;while(r<5){println("rep {}",times[r]);r=r+1;}println("sink {}",sink);}\n'
        path=work/(name+'.chl');path.write_text(source,encoding='utf-8')
        saved=output.parent/'sources'/path.name;saved.parent.mkdir(exist_ok=True);saved.write_text(source,encoding='utf-8')
        entry=dict(expected_sink=expected,source_sha256=hashlib.sha256(path.read_bytes()).hexdigest(),source='sources/'+path.name,
                   builds={},commands={},measurements={},profiles={},llvm_evidence={})
        report['cases'][name]=entry
        for variant,flags in [('baseline',DISABLE),('optimized',{})]:
            for profile in [False,True]:
                key=variant+('-profile' if profile else '')
                env=dict(envbase,**flags)
                if profile:env['CHALI_BACKEND_PROFILE']='1'
                target=work/(name+'-'+key)
                entry['builds'][key]=execute([compiler,'aot',path,'--out',target],env)
                entry['commands'][key]=[str(target)+('.exe' if os.name=='nt' else '')]
                if not profile:
                    entry['measurements'][variant]=[]
                    ir=Path(str(target)+'.ll').read_text(encoding='utf-8')
                    entry['llvm_evidence'][variant]=dict(destination_mentions=ir.count('__chali_into'),
                       bulk_calls=ir.count('call i64 @chali_host_copy_words('),address_resolves=ir.count('call ptr @chali_host_address('),
                       indirect_calls=ir.count('call i64 @chali_invoke('),inline_hints=ir.count('inlinehint'))
        save();print('准备完成：'+name,flush=True)
    for name,entry in report['cases'].items():
        for run in range(args.runs):
            order=['baseline','optimized'] if run%2==0 else ['optimized','baseline']
            report['execution_order'].append(dict(case=name,run=run,order=order))
            for variant in order:
                x=execute(entry['commands'][variant],envbase)
                times=[int(line.split()[1]) for line in x['stdout'].splitlines() if line.startswith('rep ')]
                sink=[int(line.split()[1]) for line in x['stdout'].splitlines() if line.startswith('sink ')]
                assert len(times)==5 and sink==[entry['expected_sink']],(name,variant,x)
                x['times_ns']=times;entry['measurements'][variant].append(x)
                save();print(name+' '+variant+' '+str(run+1)+': '+str(round(statistics.median(times)/1e6,3))+' ms',flush=True)
        entry['median_ns']={variant:statistics.median(t for x in samples for t in x['times_ns']) for variant,samples in entry['measurements'].items()}
        entry['speedup']=entry['median_ns']['baseline']/entry['median_ns']['optimized'];save()
    for name,entry in report['cases'].items():
        for variant in ['baseline','optimized']:
            profile=output.parent/(name+'-'+variant+'-profile.json')
            env=dict(envbase,CHALI_BACKEND_PROFILE_OUTPUT=str(profile))
            x=execute(entry['commands'][variant+'-profile'],env)
            assert [int(line.split()[1]) for line in x['stdout'].splitlines() if line.startswith('sink ')]==[entry['expected_sink']]
            data=json.loads(profile.read_text(encoding='utf-8'));counts=Counter()
            for event in data['events']:counts[event['site'].split('\t')[0]]+=event['count']
            entry['profiles'][variant]=dict(file=profile.name,counts=dict(counts),execution=x)
            save()
        print('计数完成：'+name,flush=True)
    print('完整结果：'+str(output),flush=True)


if __name__=='__main__':main()
