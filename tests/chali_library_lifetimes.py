"""验证预编译函数的公开生命周期契约与形参来源借用返回。"""
import argparse
import copy
import os
import re
from pathlib import Path
from compiler_path import compiler_path
from project_temp import tempfile
from chali_library_package import invoke, decode, encode


def symbol(interface, name):
    """从生成接口取得指定普通函数的入口符号。"""
    return re.search(r'#abi\("[^"]+","@link","([^"]+)"\)\n[^\n]*\b' + name + r'\(', interface).group(1)


def main():
    """移除实现源码后核对静态来源、只读／可写返回、重载和逃逸拒绝。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    with tempfile.TemporaryDirectory(prefix='library-lifetimes-') as directory:
        work = Path(directory)
        modules = work / 'modules/demo'
        modules.mkdir(parents=True)
        provider = work / 'provider.chl'
        package = modules / 'loans.chlib'
        source = '''module demo::loans;
struct Point {int64 x;float64 y;};
struct Other {int64 marker;};
enum Event {Empty,Count(int64 number)};
#borrow(#ret,[value])
Point const& view(Point const& value,int64 ignored=0){if(ignored<0){return value;}return value;}
#borrow(#ret,[value])
Point mut& editable(Point mut& value){return value;}
#borrow(#ret,[left])
Point const& first(Point const& left,Point const& right,int64 ignored=0){if(ignored<0&&right.x<0){return left;}return left;}
#borrow(#ret,[right])
Point const& second(Point const& left,Point const& right){if(left.x<0){return right;}return right;}
#borrow(#ret,[value])
Event const& eventView(Event const& value){return value;}
#static(value)
int64 readStatic(Point const& value){return value.x;}
int64 readStatic(Other const& value){return value.marker;}
'''
        provider.write_text(source, encoding='utf-8')
        invoke([compiler, 'lib', 'build', provider, '--module', 'demo::loans', '--package-version', '1.0.0', '--checked', '--out', package], work)
        original = package.read_bytes()
        manifest, entries = decode(original)
        assert {'lifetime-contract-v1', 'borrowed-result-v1', 'plain-enum-value-v1'} <= set(manifest['requiredFeatures'])
        interface = next(entry['data'].decode() for entry in entries if entry['kind'] == 1)
        assert '#static(value)' in interface and '#borrow(#ret,[left])' in interface
        assert 'demo::loans::Point const& view(' in interface and 'demo::loans::Point mut& editable(' in interface
        provider.write_text(source.replace('#borrow(#ret,[value])\nPoint const& view(Point const& value',
                                           '#borrow(#ret,[renamed])\nPoint const& view(Point const& renamed')
                            .replace('{if(ignored<0){return value;}return value;}', '{if(ignored<0){return renamed;}return renamed;}'), encoding='utf-8')
        invoke([compiler, 'lib', 'build', provider, '--module', 'demo::loans', '--package-version', '1.0.0', '--out', package], work)
        renamed = next(entry['data'].decode() for entry in decode(package.read_bytes())[1] if entry['kind'] == 1)
        assert symbol(renamed, 'view') == symbol(interface, 'view'), '形参改名不能改变契约的下标身份'
        package.write_bytes(original)
        probe = work / 'probe.chl'
        probe.write_text('import demo::loans;\nvoid main(){demo::loans::Point a;a.x=1;demo::loans::Point b;b.x=2;assert(demo::loans::first(&a,&b).x==1);}', encoding='utf-8')
        invoke([compiler, 'aot', probe, '--emit-obj', '--out', work / 'probe'], work)
        changed_source = source.replace('#borrow(#ret,[left])\nPoint const& first', '#borrow(#ret,[right])\nPoint const& first')
        changed_source = changed_source.replace('{if(ignored<0&&right.x<0){return left;}return left;}', '{if(ignored<0&&left.x<0){return right;}return right;}')
        provider.write_text(changed_source, encoding='utf-8')
        invoke([compiler, 'lib', 'build', provider, '--module', 'demo::loans', '--package-version', '1.0.0', '--out', package], work)
        changed = next(entry['data'].decode() for entry in decode(package.read_bytes())[1] if entry['kind'] == 1)
        assert symbol(changed, 'first') != symbol(interface, 'first')
        output = work / ('stale.exe' if os.name == 'nt' else 'stale')
        output.write_bytes(b'keep old output')
        invoke([compiler, 'link', work / 'probe.o', '--package', package, '--out', output], work, False)
        assert output.read_bytes() == b'keep old output'
        package.write_bytes(original)
        bad = work / 'bad.chl'
        negative = [
            'void main(){demo::loans::Point p;demo::loans::readStatic(&p);}',
            'demo::loans::Point const& bad(){demo::loans::Point p;return demo::loans::view(&p);}void main(){}',
            'demo::loans::Point mut& bad(demo::loans::Point const& p){return demo::loans::view(p);}void main(){}',
            '#borrow(#ret,[outer])\ndemo::loans::Point const& bad(demo::loans::Point const& outer){demo::loans::Point local;return demo::loans::second(outer,&local);}void main(){}',
            'void main(){let callback=demo::loans::view;}',
        ]
        for text in negative:
            bad.write_text('import demo::loans;\n' + text, encoding='utf-8')
            invoke([compiler, 'check', bad], work, False)
        valid = work / 'main.chl'
        valid.write_text('''import demo::loans;
import std::memory;
demo::loans::Point stable;
#borrow(#ret,[outer])
demo::loans::Point const& selected(demo::loans::Point const& outer){demo::loans::Point local;local.x=999;return demo::loans::first(outer,&local);}
void main(){
 stable.x=7;assert(demo::loans::readStatic(&stable)==7);
 demo::loans::Other other;other.marker=8;assert(demo::loans::readStatic(&other)==8);
 demo::loans::Point point;point.x=41;point.y=2.5;
 demo::loans::Point const& viewed=demo::loans::view(&point);assert(viewed.x==41&&viewed.y==2.5);
 demo::loans::Point mut& writable=demo::loans::editable(&point);writable.x=42;assert(point.x==42);
 assert(selected(&point).x==42);
 demo::loans::Event event=demo::loans::Event::Count(55);
 match(demo::loans::eventView(&event)){demo::loans::Event::Count(number)=>{assert(number==55);}_=>{assert(false);}}
 let before=std::memory::__memoryLiveBlocks();int64 i=0;while(i<50){assert(demo::loans::view(&point).x==42);i=i+1;}
 assert(std::memory::__memoryLiveBlocks()==before);
 println("library lifetime OK");
}
''', encoding='utf-8')
        for feature in ['lifetime-contract-v1', 'borrowed-result-v1']:
            altered = copy.deepcopy(manifest)
            altered['requiredFeatures'].remove(feature)
            package.write_bytes(encode(altered, entries))
            invoke([compiler, 'check', valid], work, False)
        package.write_bytes(original)
        altered_entries = copy.deepcopy(entries)
        for entry in altered_entries:
            if entry['kind'] == 1: entry['data'] = entry['data'].replace(b'#static(value)', b'#static(missing)', 1)
        package.write_bytes(encode(manifest, altered_entries))
        assert '[CHL1210]' in invoke([compiler, 'check', valid], work, False).stderr
        package.write_bytes(original)
        provider.write_text('''module demo::bridge;
import demo::loans;
#borrow(#ret,[value])
demo::loans::Point const& forward(demo::loans::Point const& value){return demo::loans::view(value);}
#static(value)
int64 readStatic(demo::loans::Point const& value){return demo::loans::readStatic(value);}
''', encoding='utf-8')
        bridge = modules / 'bridge.chlib'
        invoke([compiler, 'lib', 'build', provider, '--module', 'demo::bridge', '--package-version', '1.0.0', '--checked', '--out', bridge], work)
        provider.unlink()
        text = valid.read_text(encoding='utf-8').replace('import demo::loans;', 'import demo::bridge;')
        text = text.replace('stable.x=7;', 'stable.x=7;assert(demo::bridge::readStatic(&stable)==7);')
        text = text.replace('assert(selected(&point).x==42);', 'assert(selected(&point).x==42);assert(demo::bridge::forward(&point).x==42);')
        valid.write_text(text, encoding='utf-8')
        for allocator in ['malloc', 'pool']:
            assert invoke([compiler, 'run', valid, '--allocator', allocator, '--out', work / allocator], work).stdout == 'library lifetime OK\n'
            assert not (work / (allocator + '.abi.cpp')).exists(), '语言借用返回应直接使用逻辑地址 ABI'
        invoke([compiler, 'aot', valid, '--emit-obj', '--out', work / 'consumer'], work)
        linked = work / ('linked.exe' if os.name == 'nt' else 'linked')
        invoke([compiler, 'link', work / 'consumer.o', '--package', bridge, '--out', linked], work)
        assert invoke([linked], work).stdout == 'library lifetime OK\n'
    print('library lifetimes: static inputs, overloaded/default calls, borrowed results, escape/readonly rejection, contract hashes and detached linking OK')


if __name__ == '__main__':
    main()
