"""移走实现源码后核对库记录方法、接收者契约与拥有字段。"""
from pathlib import Path
import argparse
import copy
import os
import re
from compiler_path import compiler_path
from project_temp import tempfile
from chali_library_package import invoke, decode, encode


def main():
    """对照源码与库包，并验证方法的只读接收者和复制语义。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--old-chali', type=Path)
    args = parser.parse_args()
    compiler = args.chali.resolve()
    os.environ['CHALI_LANG'] = 'zh'
    with tempfile.TemporaryDirectory(prefix='library-methods-') as directory:
        work = Path(directory)
        modules = work / 'modules'
        modules.mkdir()
        provider = modules / 'methods.chl'
        provider.write_text('''module methods;
struct Counter {
    int64 value;
    Counter(int64 initial=3){self.value=initial;}
    int64 read(){return self.value;}
    void add(int64 delta=2){self.value=self.value+delta;}
    int64 scale(int64 factor){return self.value*factor;}
    float64 scale(float64 factor){return (float64)self.value*factor;}
    Counter combine(Counter other){other.value=self.value+other.value;return other;}
    static Counter create(int64 initial){self.value=initial;return self;}
    static int64 magic(){return 7;}
}
Counter make(int64 initial){return Counter::create(initial);}
int64 inspect(Counter const& value){return value.read();}
struct Factory {
    int64 value;
    int64 read(){return self.value;}
    static Factory create(int64 initial=9){self.value=initial;return self;}
}
struct Text {
    String value;
    String read(){return self.value;}
    void replace(String next){self.value=next;}
    void copyFrom(Text other){self.value=other.value;}
    static Text create(String initial){self.value=initial;return self;}
}
enum Choice{Empty,Value(int64 value);
    int64 read(){match(self){Choice::Empty=>{return 0;}Choice::Value(value)=>{return value;}}return -1;}
}
''', encoding='utf-8')
        consumer = work / 'main.chl'
        valid = '''import methods;
import std::memory;
void main(){
methods::Counter value=methods::Counter::create(20);
assert(value.read()==20&&methods::inspect(&value)==20);
value.add();assert(value.read()==22);
assert(value.scale(2)==44&&value.scale(0.5)==11.0&&methods::Counter::magic()==7);
methods::Counter defaults;assert(defaults.read()==0);
methods::Counter mut& heap=new Counter(8);assert(heap.read()==8);heap.drop();
methods::Factory mut& factory=new Factory(12);assert(factory.read()==12);factory.drop();
let defaultFactory=methods::Factory::create();assert(defaultFactory.read()==9);
let made=methods::make(5);assert(made.read()==5);
let combined=value.combine(made);assert(combined.read()==27&&made.read()==5);
methods::Choice event=methods::Choice::Value(17);assert(event.read()==17);
methods::Counter const& view=&value;assert(view.read()==22);
methods::Text text=methods::Text::create("原始");
let result=text.read();result="独立副本";assert(text.read()=="原始");
text.replace("替换");assert(text.read()=="替换"&&result=="独立副本");
methods::Text const& readonly=&text;assert(readonly.read()=="替换");
let before=std::memory::__memoryLiveBlocks();int64 index=0;
while(index<30){
    methods::Text temporary=methods::Text::create("长文本 A 用于核对跨库方法的真实堆缓冲区复制与释放 0123456789");
    temporary.replace("长文本 B 用于核对参数只移交一次并且不会遗留拥有值 9876543210");
    let copied=temporary.read();
    methods::Text other=methods::Text::create("原有缓冲区也需要释放 original buffer");
    other.copyFrom(temporary);assert(other.read()==copied&&temporary.read()==copied);
    let sum=value.combine(made);assert(sum.read()==27&&made.read()==5);
    index=index+1;
}
assert(std::memory::__memoryLiveBlocks()==before);
}
'''
        consumer.write_text(valid, encoding='utf-8')
        invoke([compiler, 'check', consumer], work)
        invoke([compiler, 'run', consumer, '--out', work / 'source-program'], work)
        package = modules / 'methods.chlib'
        invoke([compiler, 'lib', 'build', provider, '--module', 'methods',
                '--package-version', '1.0.0', '--with-bitcode', '--out', package], work)
        manifest, entries = decode(package.read_bytes())
        assert 'record-methods-v1' in manifest['requiredFeatures'], manifest
        interface = next(item['data'].decode() for item in entries if item['name'] == manifest['interface'])
        assert 'self.value' not in interface and 'return self' not in interface, interface
        provider_source = provider.read_text(encoding='utf-8')
        provider.unlink()
        invoke([compiler, 'check', consumer], work)
        for implementation, mode in [('native', 'off'), ('bitcode', 'off'), ('bitcode', 'full'), ('bitcode', 'thin')]:
            for allocator in ['malloc', 'pool']:
                invoke([compiler, 'run', consumer, '--library-implementation', implementation,
                        '--lto-mode', mode, '--allocator', allocator,
                        '--out', work / (implementation + '-' + mode + '-' + allocator)], work)
        obj = work / 'detached'
        invoke([compiler, 'aot', consumer, '--emit-obj', '--out', obj], work)
        linked = work / ('linked.exe' if os.name == 'nt' else 'linked')
        invoke([compiler, 'link', obj.with_suffix('.o'), '--package', package, '--out', linked], work)
        invoke([linked], work)
        assert 'plain-enum-value-v1' in manifest['requiredFeatures'], manifest
        for body in [
            'methods::Counter value;methods::Counter const& view=&value;view.add();',
            'methods::Text value;methods::Text const& view=&value;view.replace("bad");',
        ]:
            consumer.write_text('import methods;void main(){' + body + '}\n', encoding='utf-8')
            result = invoke([compiler, 'check', consumer], work, False)
            assert '只读接收者不能调用修改方法' in result.stderr, result.stderr
        consumer.write_text(valid, encoding='utf-8')
        invoke([compiler, 'check', consumer], work)
        original = package.read_bytes()
        missing = copy.deepcopy(manifest)
        missing['requiredFeatures'].remove('record-methods-v1')
        package.write_bytes(encode(missing, entries))
        rejected = invoke([compiler, 'check', consumer], work, False)
        assert 'record-methods-v1' in rejected.stderr, rejected.stderr
        package.write_bytes(original)
        for name, change, message in [
            ('receiver', lambda text: text.replace(',"mut")', ',"static")', 1), '接收者'),
            ('implementation', lambda text: re.sub(r'    #abi\([^\n]+\)\n    int64 read\(\);',
                                                   '    int64 read(){return 99;}', text, count=1), '不允许运行期函数体'),
        ]:
            altered = copy.deepcopy(entries)
            changed = change(interface)
            assert changed != interface, name
            next(item for item in altered if item['name'] == manifest['interface'])['data'] = changed.encode()
            package.write_bytes(encode(manifest, altered))
            rejected = invoke([compiler, 'check', consumer], work, False)
            assert message in rejected.stderr, (name, rejected.stderr)
        package.write_bytes(original)
        spoof = work / 'spoof.chl'
        spoof.write_text(interface + '\nvoid main(){}\n', encoding='utf-8')
        rejected = invoke([compiler, 'check', spoof], work, False)
        assert 'record-methods-v1' in rejected.stderr or '语言库 ABI' in rejected.stderr, rejected.stderr
        invoke([compiler, 'check', consumer], work)
        if args.old_chali:
            rejected = invoke([args.old_chali.resolve(), 'check', consumer], work, False)
            assert '未支持的能力' in rejected.stderr, rejected.stderr
        previous_linked = linked.read_bytes()
        provider.write_text(provider_source.replace('struct Counter {', 'struct Counter {int64 extra;'), encoding='utf-8')
        invoke([compiler, 'lib', 'build', provider, '--module', 'methods', '--package-version', '2.0.0', '--out', package], work)
        invoke([compiler, 'link', obj.with_suffix('.o'), '--package', package, '--out', linked], work, False)
        assert linked.read_bytes() == previous_linked
        provider.unlink()
        package.write_bytes(original)
        only_provider = work / 'only-provider.chl'
        only_package = modules / 'only.chlib'
        only_provider.write_text('module only;struct Counter{int64 value;int64 read(){return self.value;}static Counter create(int64 value){self.value=value;return self;}}', encoding='utf-8')
        invoke([compiler, 'lib', 'build', only_provider, '--module', 'only', '--package-version', '1.0.0', '--out', only_package], work)
        only_provider.unlink()
        consumer.write_text('import only;void main(){let value=only::Counter::create(37);assert(value.read()==37);}', encoding='utf-8')
        invoke([compiler, 'run', consumer, '--out', work / 'methods-only'], work)
    print('library methods: source/package, receivers, overloads, defaults, factories and owning fields passed')


if __name__ == '__main__':
    main()
