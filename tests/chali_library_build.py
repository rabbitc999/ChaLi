"""从语言实现构建无源码库包，验证标量入口、默认值、公开类型及精确依赖。"""
from pathlib import Path
import argparse
import os
import json
import subprocess
from chali_library_package import invoke, decode
from compiler_path import compiler_path
from project_temp import tempfile


def main():
    """导出后删除提供方实现，在独立消费者中普通导入并执行。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=compiler_path())
    compiler=parser.parse_args().chali.resolve()
    with tempfile.TemporaryDirectory(prefix='library-build-') as directory:
        work=Path(directory);modules=work/'modules/demo';modules.mkdir(parents=True)
        provider=work/'provider.chl';package=modules/'math.chlib'
        tick_source=work/'tick.s';tick_object=work/'tick.o'
        arm=os.name!='nt' and 'aarch64' in os.uname().machine.lower()
        tick_source.write_text('.text\n.globl native_tick\nnative_tick:\n'+
                              ('adrp x1,tick_count\nadd x1,x1,:lo12:tick_count\nldr x0,[x1]\nadd x0,x0,#1\nstr x0,[x1]\nret\n' if arm else
                               'movq tick_count(%rip),%rax\naddq $1,%rax\nmovq %rax,tick_count(%rip)\nretq\n')+
                              '.data\n.p2align 3\ntick_count:\n.quad 0\n')
        invoke([compiler,'asm',tick_source,'--out',tick_object],work)
        types=['int8','uint8','int16','uint16','int32','uint32','int64','uint64','float32','float64','bool','char']
        values=dict(int8="-100",uint8="230",int16="-30000",uint16="60000",int32="-123456",uint32="4000000000",int64="-1234567890123",uint64="(uint64)-1",float32="1.25",float64="-2.25",bool="true",char="'A'")
        declarations=''.join(f'{type} identity_{type}({type} value=({type}){values[type]}){{return value;}}\n' for type in types)
        provider.write_text('module demo::math;\ninterface Error {int64 code();int64 offset(int64 delta);void ping();static bool recognized();};\ninterface DetailedError : Error {bool retry();};\nstruct Point {int64 x;float64 y;};\nenum Event {Empty,Count(int64 value),Pair(int64 left,float64 right),At(Point position)};\nstruct Bounds {Point low;Point high;char marker;};\ninline int64 hidden(int64 x){return x+1;}\n'
                            '#abi("C","@link","native_tick") int64 nativeTick();\n'
                            'inline int64 hiddenDefault(){return nativeTick();}\n'
                            'int64 withDefault(int64 x=hiddenDefault()){return x;}\n'
                            'int64 twoDefaults(int64 a=hiddenDefault(),int64 b=hiddenDefault()){return a*10+b;}\n'
                            '#export("native_add") int64 add(int64 a,int64 b){return hidden(a)+b-1;}\n'
                            'int64 factorial(int64 n){if(n<=1){return 1;}return n*factorial(n-1);}\n'
                            'int64 sum(int64 n){int64 result=0;int64 i=0;while(i<n){result=result+i;i=i+1;}return result;}\n'
                            'int64 overloaded(int64 x){return x+1;}\nfloat64 overloaded(float64 x){return x*2.0;}\n'
                            'float64 multiply(float64 a,float64 b){return a*b;}\nvoid noop(){}\n'+declarations,encoding='utf-8')
        invoke([compiler,'lib','build',provider,'--module','demo::math','--package-version','1.0.0','--out',package],work)
        manifest,entries=decode(package.read_bytes())
        assert manifest['source']=='' and manifest['templates']=='' and manifest['dependencies']==[]
        assert set(manifest['requiredFeatures'])=={'native-c-v1','scalar-unit-v1','shared-type-id-v1'}
        interface=next(x['data'].decode() for x in entries if x['kind']==1)
        assert 'factorial' in interface and 'hidden' not in interface and 'return' not in interface
        assert 'struct Point' in interface and 'struct Bounds' in interface and 'demo::math::Point low;' in interface
        assert 'enum Event' in interface and 'Pair(int64 left, float64 right)' in interface and 'At(demo::math::Point position)' in interface
        assert 'interface Error' in interface and 'interface DetailedError : Error' in interface and 'static bool recognized();' in interface
        assert {x['kind'] for x in entries}=={1,3}
        obj=work/'unit.o';obj.write_bytes(next(x['data'] for x in entries if x['kind']==3))
        symbols=subprocess.run(['nm','-g','--defined-only',str(obj)],capture_output=True,text=True,timeout=30)
        assert symbols.returncode==0,(symbols.stdout,symbols.stderr)
        for forbidden in ['main','chali_init','chali_finish','chali_program_abi_bind','chali_program_std_bind']:
            assert not any(line.split() and line.split()[-1]==forbidden for line in symbols.stdout.splitlines()),symbols.stdout
        for target in ['windows-x64','linux-x64','linux-arm64']:
            cross=work/(target+'.chlib')
            invoke([compiler,'lib','build',provider,'--module','demo::math','--package-version','1.0.0','--target',target,'--out',cross],work)
            invoke([compiler,'lib','inspect',cross,'--target',target],work)
            cross_manifest,cross_entries=decode(cross.read_bytes())
            assert len(cross_manifest['targets'])==1 and {x['kind'] for x in cross_entries}=={1,3}
        other=modules/'other.chlib'
        invoke([compiler,'lib','build',provider,'--module','demo::other','--package-version','1.0.0','--out',other],work,False)
        assert not other.exists()
        provider.write_text('module demo::other;\ninterface Error {bool other();};\nstruct Point {int32 value;};\nint64 add(int64 a,int64 b){return a-b;}')
        invoke([compiler,'lib','build',provider,'--module','demo::other','--package-version','1.0.0','--out',other],work)
        host_package=package.read_bytes()
        license_file=work/'LICENSE.txt';license_file.write_bytes(b'Example license\n')
        notice_file=work/'NOTICE.txt';notice_file.write_bytes('ChaLi 示例署名\n'.encode())

        system_library='kernel32' if os.name=='nt' else 'm'
        bridge=modules/'bridge.chlib'
        bridge_source='module demo::bridge;\nimport demo::math;\ninterface BridgeError : DetailedError {int64 sourceId();};\nstruct Wrapper {demo::math::Bounds bounds;demo::math::Event event;int64 tag;};\nint64 total(int64 x){return demo::math::add(x,2); }\n'
        bridge_source+=''.join(f'{type} identity_{type}({type} value){{return demo::math::identity_{type}(value);}}\n' for type in types)
        provider.write_text(bridge_source,encoding='utf-8')
        invoke([compiler,'lib','build',provider,'--module','demo::bridge','--package-version','1.0.0','--out',bridge,
                '--license','Apache-2.0','--license-asset',f'legal/LICENSE.txt={license_file}',
                '--license-asset',f'legal/NOTICE.txt={notice_file}','--system-library',system_library,'--runtime-library','example-plugin.dll'],work)
        bridge_manifest,bridge_entries=decode(bridge.read_bytes())
        assert bridge_manifest['license']=='Apache-2.0'
        assert bridge_manifest['licenseFiles']==['legal/LICENSE.txt','legal/NOTICE.txt']
        assert bridge_manifest['targets'][0]['systemLibraries']==[system_library]
        assert bridge_manifest['targets'][0]['runtimeLibraries']==['example-plugin.dll']
        producer_info=bridge_manifest['extensions']['producer']
        build_info=json.loads(invoke([compiler,'--build-info'],work).stdout)
        assert producer_info==dict(generator='ast-llvm',optimization='O3',checked=False,runtime='consumer',llvmLinkage=build_info['llvm'])
        assert {e['name']:e['data'] for e in bridge_entries if e['kind']==8}=={'legal/LICENSE.txt':license_file.read_bytes(),'legal/NOTICE.txt':notice_file.read_bytes()}
        protected=bridge.read_bytes()
        for invalid in ['../bad=', 'interface/main.chl=']:
            invoke([compiler,'lib','build',provider,'--module','demo::bridge','--package-version','1.0.0','--out',bridge,'--license-asset',invalid+str(license_file)],work,False)
            assert bridge.read_bytes()==protected
        invoke([compiler,'lib','build',provider,'--module','demo::bridge','--package-version','1.0.0','--out',bridge,
                '--license-asset',f'legal/LICENSE.txt={license_file}','--license-asset',f'legal/LICENSE.txt={notice_file}'],work,False)
        assert bridge.read_bytes()==protected
        invoke([compiler,'check',provider,'--library-license','Apache-2.0'],work,False)
        original_license=license_file.read_bytes()
        invoke([compiler,'lib','build',provider,'--module','demo::bridge','--package-version','1.0.0','--out',license_file,'--license-asset',f'legal/LICENSE.txt={license_file}'],work,False)
        assert license_file.read_bytes()==original_license
        license_file.unlink();notice_file.unlink()
        assert [x['module'] for x in bridge_manifest['dependencies']]==['demo::math']
        bridge_interface=next(e['data'].decode() for e in bridge_entries if e['kind']==1)
        assert 'demo::math::Bounds bounds;' in bridge_interface and 'struct Bounds' not in bridge_interface
        for target in ['linux-x64','linux-arm64']:
            # 精确依赖摘要随目标对象改变，先恢复对应目标的提供方包。
            package.write_bytes((work/(target+'.chlib')).read_bytes())
            invoke([compiler,'lib','build',provider,'--module','demo::bridge','--package-version','1.0.0','--target',target,'--out',work/(target+'-bridge.chlib')],work)
        package.write_bytes(host_package)
        provider.unlink()
        assertions=''

        for type in types:
            value=dict(int8="-100",uint8="230",int16="-30000",uint16="60000",int32="-123456",uint32="4000000000",int64="-1234567890123",uint64="-1",float32="1.25",float64="-2.25",bool="true",char="'A'")[type]
            assertions+=f'assert(demo::math::identity_{type}()==({type}){value});assert(demo::math::identity_{type}(({type}){value})==({type}){value});assert(demo::bridge::identity_{type}(({type}){value})==({type}){value});'
        consumer=work/'consumer.chl'
        consumer.write_text('import demo::math;\nimport demo::other;\nimport demo::bridge;\n#abi("C","@link","native_add") int64 nativeAdd(int64 x,int64 y);\nstruct IoError : BridgeError {int64 value;int64 code(){return self.value;}int64 offset(int64 amount){return self.value+amount;}void ping(){}static bool recognized(){return true;}bool retry(){return true;}int64 sourceId(){return 12;}};\nint64 enumScore(demo::math::Event event){match(event){demo::math::Event::Empty=>{return 0;}demo::math::Event::Count(value)=>{return value;}demo::math::Event::Pair(left,right)=>{return left+(int64)right;}demo::math::Event::At(position)=>{return position.x;}}return -1;}\nvoid main(){IoError error;error.value=41;demo::math::Error const& errorRef=&error;assert(errorRef.code()==41);assert(errorRef.offset(1)==42);errorRef.ping();assert(IoError::recognized());match(errorRef){IoError(value)=>{assert(value.value==41);} _=>{assert(false);}}demo::math::Bounds bounds;bounds.low.x=7;bounds.high.y=2.5;bounds.marker=\'Q\';demo::math::Bounds copied=bounds;copied.low.x=9;assert(bounds.low.x==7);demo::bridge::Wrapper wrapper;wrapper.bounds=copied;wrapper.event=demo::math::Event::At(copied.low);wrapper.tag=12;demo::bridge::Wrapper wrapperCopy=wrapper;wrapperCopy.bounds.low.x=15;assert(wrapper.bounds.low.x==9);assert(wrapperCopy.bounds.high.y==2.5);assert(wrapperCopy.tag==12);assert(enumScore(wrapperCopy.event)==9);wrapper.event=demo::math::Event::Count(99);assert(enumScore(wrapper.event)==99);assert(enumScore(wrapperCopy.event)==9);assert(enumScore(demo::math::Event::Empty)==0);assert(enumScore(demo::math::Event::Pair(5,2.5))==7);assert(copied.low.x==9);assert(copied.high.y==2.5);assert(copied.marker==\'Q\');assert(demo::math::withDefault(77)==77);assert(demo::math::withDefault()==1);assert(demo::math::withDefault()==2);assert(demo::math::withDefault(78)==78);assert(demo::math::withDefault()==3);assert(demo::math::twoDefaults()==45);assert(demo::math::twoDefaults(1)==16);assert(demo::math::twoDefaults(1,2)==12);assert(demo::math::withDefault()==7);assert(nativeAdd(20,22)==42);assert(demo::bridge::total(40)==42);assert(demo::other::add(22,20)==2);assert(demo::math::add(20,22)==42);'
                            'assert(demo::math::factorial(8)==40320);assert(demo::math::sum(10)==45);'
                            'assert(demo::math::overloaded(9)==10);assert(demo::math::overloaded(1.5)==3.0);assert(demo::math::multiply(1.5,2.0)==3.0);demo::math::noop();'+assertions+'println("compiled library OK");}',encoding='utf-8')
        invoke([compiler,'check',consumer],work)
        result=invoke([compiler,'run',consumer,'--link',tick_object,'--out',work/'checked'],work)
        assert result.stdout=='compiled library OK\n'
        detached=work/'detached'
        invoke([compiler,'aot',consumer,'--emit-obj','--out',detached],work)
        linked=work/('linked.exe' if os.name=='nt' else 'linked')
        invoke([compiler,'link',detached.with_suffix('.o'),tick_object,'--package',bridge,'--package',other,'--out',linked],work)
        assert invoke([linked],work).stdout=='compiled library OK\n'
        good_dependency=package.read_bytes()
        package.write_bytes(good_dependency[:-1]+bytes([good_dependency[-1]^1]))
        invoke([compiler,'run',consumer,'--link',tick_object,'--out',work/'bad-dependency'],work,False)
        package.write_bytes(good_dependency)
        bridge_only=work/'bridge-only.chl'
        bridge_only.write_text('import demo::bridge;\nvoid main(){demo::bridge::Wrapper wrapper;wrapper.bounds.low.x=42;assert(wrapper.bounds.low.x==42);wrapper.event=demo::math::Event::Pair(3,4.5);match(wrapper.event){demo::math::Event::Pair(left,right)=>{assert(left==3);assert(right==4.5);} _=>{assert(false);}}}')
        invoke([compiler,'run',bridge_only,'--link',tick_object,'--out',work/'bridge-only'],work)
        bad=work/'private.chl';bad.write_text('import demo::math;void main(){demo::math::hidden(1);}')
        invoke([compiler,'check',bad],work,False)
        bad.write_text('import demo::math;\nimport demo::other;\nvoid main(){demo::math::Point a;demo::other::Point b=a;}')
        invoke([compiler,'check',bad],work,False)
        (modules/'source.chl').write_text('module demo::source;\nstruct Source {int64 value;};')
        bad.write_text('import demo::bridge;\nstruct Broken : BridgeError {int64 code(){return 1;}};void main(){}')
        invoke([compiler,'check',bad],work,False)
        original=package.read_bytes()
        for source in ['module demo::math;#export("native_text") String text(){return "unsupported";}',
                       'module demo::math;struct Point {int64 x;};Point make(){Point value;value.x=1;return value;}int64 read(Point value=make()){return value.x;}',
                       'module demo::math;int64 state=1;int64 value(){return state;}',
                       'module demo::math;int64 unusedState=1;int64 value(){return 2;}',
                       'module demo::math;int64 generic<T>(T value){return 0;}',
                       'module demo::math;struct Owned {List<int64> value;};int64 value(){return 1;}',
                       'module demo::math;enum Owned {Empty,Items(List<int64> value)};int64 value(){return 1;}',
                       'module demo::math;interface Defaulted {int64 code(){return 1;}};int64 value(){return 1;}',
                       'module demo::math;struct Record {int64 x;int64 read(){return self.x;}};int64 value(){return 1;}',
                       'module demo::math;import demo::source;struct Record {demo::source::Source item;};int64 value(){return 1;}']:
            provider.write_text(source.replace('module demo::math;','module demo::math;\n').replace('import demo::source;','import demo::source;\n'))
            result=invoke([compiler,'lib','build',provider,'--module','demo::math','--package-version','1.0.0','--out',package],work,False)
            assert any(word in result.stdout+result.stderr for word in ["标量","全局","泛型","记录","接口","聚合"]),(source,result.stdout,result.stderr)
            assert package.read_bytes()==original
    print('precompiled scalar entries, defaults and public types OK')


if __name__=='__main__':
    main()
