"""从语言实现构建无源码库包，验证基础类型、调用、隐藏与共用运行时。"""
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
        types=['int8','uint8','int16','uint16','int32','uint32','int64','uint64','float32','float64','bool','char']
        declarations=''.join(f'{type} identity_{type}({type} value){{return value;}}\n' for type in types)
        provider.write_text('module demo::math;\nstruct Point {int64 x;float64 y;};\nstruct Bounds {Point low;Point high;char marker;};\ninline int64 hidden(int64 x){return x+1;}\n'
                            '#export("native_add") int64 add(int64 a,int64 b){return hidden(a)+b-1;}\n'
                            'int64 factorial(int64 n){if(n<=1){return 1;}return n*factorial(n-1);}\n'
                            'int64 sum(int64 n){int64 result=0;int64 i=0;while(i<n){result=result+i;i=i+1;}return result;}\n'
                            'int64 overloaded(int64 x){return x+1;}\nfloat64 overloaded(float64 x){return x*2.0;}\n'
                            'float64 multiply(float64 a,float64 b){return a*b;}\nvoid noop(){}\n'+declarations,encoding='utf-8')
        invoke([compiler,'lib','build',provider,'--module','demo::math','--package-version','1.0.0','--out',package],work)
        manifest,entries=decode(package.read_bytes())
        assert manifest['source']=='' and manifest['templates']=='' and manifest['dependencies']==[]
        assert set(manifest['requiredFeatures'])=={'native-c-v1','scalar-unit-v1'}
        interface=next(x['data'].decode() for x in entries if x['kind']==1)
        assert 'factorial' in interface and 'hidden' not in interface and 'return' not in interface
        assert 'struct Point' in interface and 'struct Bounds' in interface and 'demo::math::Point low;' in interface
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
        provider.write_text('module demo::other;\nstruct Point {int32 value;};\nint64 add(int64 a,int64 b){return a-b;}')
        invoke([compiler,'lib','build',provider,'--module','demo::other','--package-version','1.0.0','--out',other],work)
        host_package=package.read_bytes()
        license_file=work/'LICENSE.txt';license_file.write_bytes(b'Example license\n')
        notice_file=work/'NOTICE.txt';notice_file.write_bytes('ChaLi 示例署名\n'.encode())

        system_library='kernel32' if os.name=='nt' else 'm'
        bridge=modules/'bridge.chlib'
        bridge_source='module demo::bridge;\nimport demo::math;\nint64 total(int64 x){return demo::math::add(x,2); }\n'
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
        for target in ['linux-x64','linux-arm64']:
            # 精确依赖摘要随目标对象改变，先恢复对应目标的提供方包。
            package.write_bytes((work/(target+'.chlib')).read_bytes())
            invoke([compiler,'lib','build',provider,'--module','demo::bridge','--package-version','1.0.0','--target',target,'--out',work/(target+'-bridge.chlib')],work)
        package.write_bytes(host_package)
        provider.unlink()
        assertions=''

        for type in types:
            value=dict(int8="-100",uint8="230",int16="-30000",uint16="60000",int32="-123456",uint32="4000000000",int64="-1234567890123",uint64="-1",float32="1.25",float64="-2.25",bool="true",char="'A'")[type]
            assertions+=f'assert(demo::math::identity_{type}(({type}){value})==({type}){value});assert(demo::bridge::identity_{type}(({type}){value})==({type}){value});'
        consumer=work/'consumer.chl'
        consumer.write_text('import demo::math;\nimport demo::other;\nimport demo::bridge;\n#abi("C","@link","native_add") int64 nativeAdd(int64 x,int64 y);\nvoid main(){demo::math::Bounds bounds;bounds.low.x=7;bounds.high.y=2.5;bounds.marker=\'Q\';demo::math::Bounds copied=bounds;copied.low.x=9;assert(bounds.low.x==7);assert(copied.low.x==9);assert(copied.high.y==2.5);assert(copied.marker==\'Q\');assert(nativeAdd(20,22)==42);assert(demo::bridge::total(40)==42);assert(demo::other::add(22,20)==2);assert(demo::math::add(20,22)==42);'
                            'assert(demo::math::factorial(8)==40320);assert(demo::math::sum(10)==45);'
                            'assert(demo::math::overloaded(9)==10);assert(demo::math::overloaded(1.5)==3.0);assert(demo::math::multiply(1.5,2.0)==3.0);demo::math::noop();'+assertions+'println("compiled library OK");}',encoding='utf-8')
        invoke([compiler,'check',consumer],work)
        result=invoke([compiler,'run',consumer,'--out',work/'checked'],work)
        assert result.stdout=='compiled library OK\n'
        detached=work/'detached'
        invoke([compiler,'aot',consumer,'--emit-obj','--out',detached],work)
        linked=work/('linked.exe' if os.name=='nt' else 'linked')
        invoke([compiler,'link',detached.with_suffix('.o'),'--package',bridge,'--package',other,'--out',linked],work)
        assert invoke([linked],work).stdout=='compiled library OK\n'
        good_dependency=package.read_bytes()
        package.write_bytes(good_dependency[:-1]+bytes([good_dependency[-1]^1]))
        invoke([compiler,'run',consumer,'--out',work/'bad-dependency'],work,False)
        package.write_bytes(good_dependency)
        bad=work/'private.chl';bad.write_text('import demo::math;void main(){demo::math::hidden(1);}')
        invoke([compiler,'check',bad],work,False)
        bad.write_text('import demo::math;\nimport demo::other;\nvoid main(){demo::math::Point a;demo::other::Point b=a;}')
        invoke([compiler,'check',bad],work,False)
        original=package.read_bytes()
        for source in ['module demo::math;String text(){return "unsupported";}',
                       'module demo::math;int64 state=1;int64 value(){return state;}',
                       'module demo::math;int64 unusedState=1;int64 value(){return 2;}',
                       'module demo::math;int64 generic<T>(T value){return 0;}',
                       'module demo::math;struct Owned {String value;};int64 value(){return 1;}',
                       'module demo::math;struct Record {int64 x;int64 read(){return self.x;}};int64 value(){return 1;}']:
            provider.write_text(source.replace('module demo::math;','module demo::math;\n'))
            result=invoke([compiler,'lib','build',provider,'--module','demo::math','--package-version','1.0.0','--out',package],work,False)
            assert any(word in result.stdout+result.stderr for word in ["标量","全局","泛型","记录"]),(source,result.stdout,result.stderr)
            assert package.read_bytes()==original
    print('precompiled language scalar libraries OK')


if __name__=='__main__':
    main()
