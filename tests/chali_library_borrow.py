"""验证语言单元直接借用调用方记录，覆盖共享内存、跨库和布局变更。"""
from pathlib import Path
import argparse
import os
import re
from compiler_path import compiler_path
from project_temp import tempfile
from chali_library_package import invoke, decode


def main():
    """删除实现源码后消费记录借用入口，不借助宿主 C 结构体封送。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=compiler_path())
    compiler=parser.parse_args().chali.resolve()
    with tempfile.TemporaryDirectory(prefix='library-borrow-') as folder:
        work=Path(folder);modules=work/'modules/demo';modules.mkdir(parents=True)
        provider=work/'provider.chl';geometry=modules/'geometry.chlib'
        source=('module demo::geometry;\n'
                'struct Point {int64 x;float64 y;};\n'
                'struct Rect {Point first;Point second;uint8 marker;};\n'
                'int64 read(Point const& value){return value.x;}\n'
                'float64 sum(Rect const& value){return (float64)value.first.x+value.first.y+(float64)value.second.x+value.second.y;}\n'
                'void bump(Point mut& value,int64 delta){value.x=value.x+delta;value.y=value.y+0.5;}\n'
                'void adjust(Rect mut& value){bump(&value.first,2);value.marker=(uint8)230;}\n'
                'int64 recursive(Point const& value,int64 depth){if(depth<=0){return value.x;}return recursive(value,depth-1);}\n')
        provider.write_text(source,encoding='utf-8')
        invoke([compiler,'lib','build',provider,'--module','demo::geometry','--package-version','1.0.0','--checked','--out',geometry],work)
        manifest,entries=decode(geometry.read_bytes())
        assert manifest['targets'][0]['nativeAbi']=='chali-borrow-v1'
        assert 'borrowed-record-v1' in manifest['requiredFeatures']
        interface=next(e['data'].decode() for e in entries if e['kind']==1)
        assert '#abi("ChaLiBorrow"' in interface and 'demo::geometry::Point const& value' in interface
        original=geometry.read_bytes()
        provider.write_text(source.replace('int64 read(', '#static(value)\nint64 read('),encoding='utf-8')
        rejected=invoke([compiler,'lib','build',provider,'--module','demo::geometry','--package-version','1.0.0','--out',geometry],work,False)
        assert '生命周期契约' in rejected.stderr and geometry.read_bytes()==original
        provider.write_text(source,encoding='utf-8')
        for target in ['windows-x64','linux-x64','linux-arm64']:
            invoke([compiler,'lib','build',provider,'--module','demo::geometry','--package-version','1.0.0','--checked','--target',target,'--out',work/(target+'.chlib')],work)
        bridge=modules/'bridge.chlib'
        provider.write_text('module demo::bridge;\nimport demo::geometry;\n'
                            'float64 total(demo::geometry::Rect const& value){return demo::geometry::sum(value);}\n'
                            'void edit(demo::geometry::Rect mut& value){demo::geometry::adjust(value);}\n',encoding='utf-8')
        invoke([compiler,'lib','build',provider,'--module','demo::bridge','--package-version','1.0.0','--checked','--out',bridge],work)
        provider.unlink()
        (modules/'other.chl').write_text('module demo::other;\nstruct Point {int64 unused;};',encoding='utf-8')
        consumer=work/'main.chl'
        consumer.write_text('import demo::other;\nimport demo::bridge;\n'
                            'void main(){demo::geometry::Point point;point.x=40;point.y=1.5;'
                            'assert(demo::geometry::read(&point)==40);demo::geometry::bump(&point,2);assert(point.x==42);assert(point.y==2.0);'
                            'assert(demo::geometry::recursive(&point,5)==42);'
                            'demo::geometry::Rect rect;rect.first=point;rect.second.x=3;rect.second.y=4.0;'
                            'assert(demo::bridge::total(&rect)==51.0);demo::bridge::edit(&rect);assert(rect.first.x==44);assert(rect.first.y==2.5);assert(rect.marker==(uint8)230);'
                            'println("borrowed library OK");}',encoding='utf-8')
        assert invoke([compiler,'run',consumer,'--out',work/'checked'],work).stdout=='borrowed library OK\n'
        output=work/'unchecked'
        invoke([compiler,'aot',consumer,'--out',output],work)
        assert not output.with_suffix('.abi.cpp').exists()
        read_symbol=re.search(r'#abi\("ChaLiBorrow","@link","([^"]+)"\)\nint64 read\(',interface).group(1)
        llvm=output.with_suffix('.ll').read_text(encoding='utf-8')
        assert re.search(r'declare i64 @"?'+read_symbol+r'"?\(i64\)',llvm), [line for line in llvm.splitlines() if read_symbol in line]
        assert re.search(r'call i64 @"?'+read_symbol+r'"?\(i64',llvm), [line for line in llvm.splitlines() if read_symbol in line]
        executable=output.with_suffix('.exe') if os.name=='nt' else output
        assert invoke([executable],work).stdout=='borrowed library OK\n'
        detached=work/'detached';invoke([compiler,'aot',consumer,'--emit-obj','--out',detached],work)
        linked=work/('linked.exe' if os.name=='nt' else 'linked')
        invoke([compiler,'link',detached.with_suffix('.o'),'--package',bridge,'--out',linked],work)
        assert invoke([linked],work).stdout=='borrowed library OK\n'
        bad=work/'bad.chl'
        for text in ['import demo::geometry;\nvoid main(){demo::geometry::Point point;demo::geometry::Point const& view=&point;demo::geometry::bump(view,1);}',
                     '#abi("ChaLiBorrow","@link","forged") int64 forged(int64 value);void main(){}']:
            bad.write_text(text,encoding='utf-8');invoke([compiler,'check',bad],work,False)
        # 独立对象只依赖 geometry，确保失败来自布局符号而非 bridge 内容锁。
        consumer.write_text('import demo::geometry;\nvoid main(){demo::geometry::Point point;point.x=42;assert(demo::geometry::read(&point)==42);}',encoding='utf-8')
        stale=work/'stale';invoke([compiler,'aot',consumer,'--emit-obj','--out',stale],work)
        matching=work/('matching.exe' if os.name=='nt' else 'matching')
        invoke([compiler,'link',stale.with_suffix('.o'),'--package',geometry,'--out',matching],work)
        invoke([matching],work)
        provider.write_text(source.replace('struct Point {int64 x;','struct Point {int64 pad;int64 x;'),encoding='utf-8')
        invoke([compiler,'lib','build',provider,'--module','demo::geometry','--package-version','2.0.0','--checked','--out',geometry],work)
        new_manifest,new_entries=decode(geometry.read_bytes())
        new_interface=next(e['data'].decode() for e in new_entries if e['kind']==1)
        assert re.findall(r'chali_lib_[0-9a-f]+',interface)!=re.findall(r'chali_lib_[0-9a-f]+',new_interface)
        linked.write_bytes(b'preserved executable')
        invoke([compiler,'link',stale.with_suffix('.o'),'--package',geometry,'--out',linked],work,False)
        assert linked.read_bytes()==b'preserved executable'
        invoke([compiler,'run',consumer,'--out',work/'new-layout'],work)
        assert new_manifest['version']=='2.0.0' and geometry.read_bytes()!=original
    print('borrowed records: checked/unchecked, nested modules, mutation and layout rejection OK')


if __name__=='__main__':
    main()
