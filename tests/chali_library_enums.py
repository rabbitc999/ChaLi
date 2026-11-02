"""验证无拥有状态枚举的库参数、返回、借用、跨库转发及独立链接。"""
import argparse
import os
import platform
from pathlib import Path
from compiler_path import compiler_path
from project_temp import tempfile
from chali_library_package import invoke, decode


def main():
    """删除提供方源码，核对载荷、独立副本、借用与存活块。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    with tempfile.TemporaryDirectory(prefix='library-enums-') as directory:
        work = Path(directory)
        modules = work / 'modules/demo'
        modules.mkdir(parents=True)
        provider = work / 'provider.chl'
        package = modules / 'events.chlib'
        source = '''module demo::events;
struct Point {int64 x;float64 y;};
enum Inner {None,Index(int64 index)};
enum Event {Empty,Count(int64 value),Pair(int64 left,float64 right),At(Point position),Nested(Inner value),Bits(uint8 tiny,bool flag,char letter,float32 decimal)};
int64 innerScore(Inner value){match(value){Inner::None=>{return 0;}Inner::Index(index)=>{return index;}}return -1;}
int64 score(Event value){match(value){Event::Empty=>{return 0;}Event::Count(number)=>{return number;}Event::Pair(left,right)=>{return left+(int64)right;}Event::At(position)=>{return position.x+(int64)position.y;}Event::Nested(inner)=>{return innerScore(inner);}Event::Bits(tiny,flag,letter,decimal)=>{if(flag){return (int64)tiny+(int64)letter+(int64)decimal+1;}return 0;}}return -1;}
Event make(int64 tag){if(tag==0){return Event::Empty;}if(tag==1){return Event::Count(31);}if(tag==2){return Event::Pair(7,2.5);}if(tag==3){Point p;p.x=40;p.y=2.5;return Event::At(p);}if(tag==4){return Event::Nested(Inner::Index(55));}return Event::Bits((uint8)255,true,'A',(float32)1.25);}
Event echo(Event value){return value;}
Event changed(Event value){value=Event::Count(99);return value;}
Event recurse(Event value,int64 depth){if(depth<=0){return value;}return recurse(value,depth-1);}
int64 read(Event const& value){match(value){Event::Count(number)=>{return number;}_=>{return -1;}}return -2;}
void replace(Event mut& value){value=Event::Count(77);}
'''
        provider.write_text(source, encoding='utf-8')
        host_target = 'windows-x64' if os.name == 'nt' else 'linux-arm64' if platform.machine().lower() in ('aarch64', 'arm64') else 'linux-x64'
        for target in ['windows-x64', 'linux-x64', 'linux-arm64']:
            invoke([compiler, 'lib', 'build', provider, '--module', 'demo::events', '--package-version', '1.0.0',
                    '--checked', '--target', target, '--out', package if target == host_target else work / (target + '.chlib')], work)
        manifest, entries = decode(package.read_bytes())
        assert manifest['targets'][0]['nativeAbi'] == 'chali-value-v1'
        assert 'plain-enum-value-v1' in manifest['requiredFeatures']
        interface = next(entry['data'].decode() for entry in entries if entry['kind'] == 1)
        assert '#abi("ChaLiValue"' in interface and '#abi("ChaLiBorrow"' in interface
        assert 'demo::events::Event' in interface and 'const& value' in interface
        original = package.read_bytes()
        for rejected in ['enum Owned {Empty,Text(String value)};Owned owned(Owned value){return value;}',
                         '#export("physical_enum") Event physical(Event value){return value;}']:
            provider.write_text(source + '\n' + rejected + '\n', encoding='utf-8')
            invoke([compiler, 'lib', 'build', provider, '--module', 'demo::events', '--package-version', '1.0.0', '--out', package], work, False)
            assert package.read_bytes() == original
        layout = work / 'layout.chl'
        layout.write_text('import demo::events;\nvoid main(){assert(demo::events::score(demo::events::make(2))==9);}', encoding='utf-8')
        invoke([compiler, 'aot', layout, '--emit-obj', '--out', work / 'layout'], work)
        provider.write_text(source.replace('enum Event {Empty,Count(int64 value)', 'enum Event {Count(int64 value),Empty'), encoding='utf-8')
        invoke([compiler, 'lib', 'build', provider, '--module', 'demo::events', '--package-version', '1.0.0', '--out', package], work)
        failed_output = work / ('layout.exe' if os.name == 'nt' else 'layout-linked')
        failed_output.write_bytes(b'preserve existing output')
        invoke([compiler, 'link', work / 'layout.o', '--package', package, '--out', failed_output], work, False)
        assert failed_output.read_bytes() == b'preserve existing output'
        package.write_bytes(original)
        provider.write_text('''module demo::bridge;
import demo::events;
demo::events::Event forward(demo::events::Event value){return demo::events::echo(value);}
demo::events::Event make(int64 tag){return demo::events::make(tag);}
void replace(demo::events::Event mut& value){demo::events::replace(value);}
''', encoding='utf-8')
        bridge = modules / 'bridge.chlib'
        invoke([compiler, 'lib', 'build', provider, '--module', 'demo::bridge', '--package-version', '1.0.0', '--checked', '--out', bridge], work)
        assert 'plain-enum-value-v1' in decode(bridge.read_bytes())[0]['requiredFeatures']
        provider.write_text('''module demo::borrows;
import demo::events;
int64 read(demo::events::Event const& value){return demo::events::read(value);}
void replace(demo::events::Event mut& value){demo::events::replace(value);}
''', encoding='utf-8')
        borrows = modules / 'borrows.chlib'
        invoke([compiler, 'lib', 'build', provider, '--module', 'demo::borrows', '--package-version', '1.0.0', '--checked', '--out', borrows], work)
        borrowed_manifest = decode(borrows.read_bytes())[0]
        assert borrowed_manifest['targets'][0]['nativeAbi'] == 'chali-borrow-v1'
        assert 'plain-enum-value-v1' in borrowed_manifest['requiredFeatures']
        provider.unlink()
        for owner in ['missing::events::Event', 'Event', 'std::lang::Event']:
            wrong = work / 'wrong.chl'
            wrong.write_text('import demo::events;\nenum Event {Pair(int64 left,float64 right)};\n'
                             'void main(){let value=demo::events::make(2);match(value){' + owner
                             + '::Pair(left,right)=>{} _=>{}}}', encoding='utf-8')
            rejected = invoke([compiler, 'check', wrong], work, False)
            assert '分支的枚举类型必须' in rejected.stderr, rejected.stderr
        consumer = work / 'main.chl'
        consumer.write_text('''import demo::bridge;
import demo::borrows;
import std::memory;
enum Event {Other,Pair(int64 left,float64 right)};
void main(){
 Option<int64> standard=std::lang::Option::Value(12);
 match(standard){std::lang::Option::Value(value)=>{assert(value==12);}std::lang::Option::None=>{assert(false);}}
 assert(demo::events::score(demo::bridge::make(0))==0);
 assert(demo::events::score(demo::bridge::make(1))==31);
 assert(demo::events::score(demo::bridge::make(2))==9);
 assert(demo::events::score(demo::bridge::make(3))==42);
 assert(demo::events::score(demo::bridge::make(4))==55);
 assert(demo::events::score(demo::bridge::make(5))==322);
 match(demo::bridge::forward(demo::bridge::make(2))){
  demo::events::Event::Pair(left,right)=>{assert(left==7);assert(right==2.5);}
  _=>{assert(false);}
 }
 match(demo::bridge::make(3)){
  demo::events::Event::At(position)=>{assert(position.x==40);assert(position.y==2.5);}
  _=>{assert(false);}
 }
 match(demo::bridge::forward(demo::bridge::make(5))){
  demo::events::Event::Bits(tiny,flag,letter,decimal)=>{assert(tiny==(uint8)255);assert(flag);assert(letter=='A');assert(decimal==(float32)1.25);}
  _=>{assert(false);}
 }
 let original=demo::events::make(1);
 let copied=demo::bridge::forward(original);
 let changed=demo::events::changed(copied);
 assert(demo::events::score(original)==31);
 assert(demo::events::score(copied)==31);
 assert(demo::events::score(changed)==99);
 assert(demo::borrows::read(&original)==31);
 demo::bridge::replace(&original);
 assert(demo::events::read(&original)==77);
 demo::borrows::replace(&original);
 assert(demo::events::score(copied)==31);
 let before=std::memory::__memoryLiveBlocks();
 int64 i=0;while(i<50){assert(demo::events::score(demo::events::recurse(demo::bridge::make(i%6),3))>=0);i=i+1;}
 assert(std::memory::__memoryLiveBlocks()==before);
 println("enum values OK");
}
''', encoding='utf-8')
        for allocator in ['malloc', 'pool']:
            assert invoke([compiler, 'run', consumer, '--allocator', allocator, '--out', work / allocator], work).stdout == 'enum values OK\n'
        output = work / 'unchecked'
        invoke([compiler, 'aot', consumer, '--out', output], work)
        executable = output.with_suffix('.exe') if os.name == 'nt' else output
        assert invoke([executable], work).stdout == 'enum values OK\n'
        obj = work / 'consumer'
        invoke([compiler, 'aot', consumer, '--emit-obj', '--out', obj], work)
        linked = work / ('linked.exe' if os.name == 'nt' else 'linked')
        invoke([compiler, 'link', obj.with_suffix('.o'), '--package', bridge, '--package', borrows, '--out', linked], work)
        assert invoke([linked], work).stdout == 'enum values OK\n'
    print('enum library ABI: empty/multiple/nested payloads, values, references, recursion, lifetimes and detached linking OK')


if __name__ == '__main__':
    main()
