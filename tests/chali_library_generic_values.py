"""对照跨库具体泛型聚合、消费者实例及共享类型身份。"""
from pathlib import Path
import argparse
import os
from compiler_path import compiler_path
from project_temp import tempfile
from chali_library_package import invoke, decode


def main():
    """用相同模块源码及原生库分别消费具体泛型返回值和方法。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--old-chali', type=Path)
    args = parser.parse_args()
    compiler = args.chali.resolve()
    os.environ['CHALI_LANG'] = 'zh'
    with tempfile.TemporaryDirectory(prefix='library-generic-values-') as directory:
        work = Path(directory)
        modules = work / 'modules'
        source_modules = work / 'source/modules'
        modules.mkdir()
        source_modules.mkdir(parents=True)
        body = '''
struct Token{int64 tag;}
struct Box<T>{T value;T read(){return self.value;}void set(T next){self.value=next;}
    static Box<T> create(T initial){self.value=initial;return self;}}
Box<int64> makeNumber(int64 value){return Box<int64>::create(value);}
Box<std::lang::String> makeText(std::lang::String value){return Box<std::lang::String>::create(value);}
Box<Token> makeToken(int64 tag){Token value;value.tag=tag;return Box<Token>::create(value);}
Box<int64> echo(Box<int64> value){return value;}
Box<Box<int64>> makeNested(int64 value){return Box<Box<int64>>::create(makeNumber(value));}
Box<Box<T>> nested<T>(T value){return Box<Box<T>>::create(Box<T>::create(value));}
Box<Box<std::lang::String>> makeNestedText(std::lang::String value){return Box<Box<std::lang::String>>::create(makeText(value));}
int64 flag(){Box<bool> value;value.value=true;if(value.read()){return 1;}return 0;}
'''
        for name in ['alpha', 'beta']:
            (source_modules / (name + '.chl')).write_text('module ' + name + ';\n' + body, encoding='utf-8')
        bridge_body = 'import alpha;\nalpha::Box<int64> relay(alpha::Box<int64> value){return alpha::echo(value);}int64 flag(){return alpha::flag();}'
        (source_modules / 'bridge.chl').write_text('module bridge;\n' + bridge_body, encoding='utf-8')
        valid = '''import bridge;
import alpha;
import beta;
import std::memory;
struct String{int64 foreign;}
struct Token{int64 foreign;}
void main(){
let original=alpha::makeNumber(17);
alpha::Box<alpha::Box<int64>> nested=alpha::makeNested(43);assert(nested.read().read()==43);
alpha::Box<alpha::Box<int64>> generated=alpha::nested<int64>(44);assert(generated.read().read()==44);
alpha::Box<alpha::Box<int64>> constructed=alpha::Box<alpha::Box<int64>>::create(alpha::Box<int64>::create(45));
assert(constructed.read().read()==45);
alpha::Box<int64> copy=alpha::echo(original);copy.set(19);
assert(original.read()==17&&copy.read()==19);
beta::Box<int64> other=beta::makeNumber(23);assert(other.read()==23);
let text=alpha::makeText("原始长文本用于跨库具体泛型拥有字段 01234567890123456789");
alpha::Box<std::lang::String> changed=text;changed.set("changed");
assert(text.read()=="原始长文本用于跨库具体泛型拥有字段 01234567890123456789"&&changed.read()=="changed");
alpha::Box<alpha::Token> token=alpha::makeToken(31);assert(token.read().tag==31);
beta::Box<beta::Token> betaToken=beta::makeToken(33);assert(betaToken.read().tag==33);
assert(alpha::flag()==1&&beta::flag()==1&&bridge::flag()==1);
let forwarded=bridge::relay(original);assert(forwarded.read()==17);
let before=std::memory::__memoryLiveBlocks();int64 index=0;
while(index<20){let value=alpha::makeText("循环拥有值用于核对共同类型编号和释放 01234567890123456789");
    let copied=value;copied.set("independent copy");assert(value.read()!=copied.read());
    let nested=beta::makeNestedText("嵌套拥有值用于核对递归类型身份与释放 01234567890123456789");
    let inner=nested.read();inner.set("separate");assert(nested.read().read()!="separate");
    let number=beta::echo(beta::makeNumber(index));assert(number.read()==index);index=index+1;}
assert(std::memory::__memoryLiveBlocks()==before);
}
'''
        source_main = source_modules.parent / 'main.chl'
        source_main.write_text(valid, encoding='utf-8')
        invoke([compiler, 'check', source_main], source_modules.parent)
        invoke([compiler, 'run', source_main, '--out', work / 'source-program'], source_modules.parent)
        packages = []
        for name in ['alpha', 'beta']:
            provider = work / 'provider.chl'
            provider.write_text('module ' + name + ';\n' + body, encoding='utf-8')
            package = modules / (name + '.chlib')
            invoke([compiler, 'lib', 'build', provider, '--module', name,
                    '--package-version', '1.0.0', '--with-bitcode', '--out', package], work)
            packages.append(package)
        provider.write_text('module bridge;\n' + bridge_body, encoding='utf-8')
        bridge = modules / 'bridge.chlib'
        invoke([compiler, 'lib', 'build', provider, '--module', 'bridge', '--package-version', '1.0.0',
                '--with-bitcode', '--out', bridge], work)
        packages.append(bridge)
        provider.unlink()
        consumer = work / 'main.chl'
        consumer.write_text(valid, encoding='utf-8')
        invoke([compiler, 'check', consumer], work)
        for implementation, mode in [('native', 'off'), ('bitcode', 'off'), ('bitcode', 'full'), ('bitcode', 'thin')]:
            for allocator in ['malloc', 'pool']:
                invoke([compiler, 'run', consumer, '--library-implementation', implementation,
                        '--lto-mode', mode, '--allocator', allocator,
                        '--out', work / (implementation + '-' + mode + '-' + allocator)], work)
        obj = work / 'detached'
        invoke([compiler, 'aot', consumer, '--emit-obj', '--out', obj], work)
        linked = work / ('linked.exe' if os.name == 'nt' else 'linked')
        invoke([compiler, 'link', obj.with_suffix('.o'), '--package', packages[0], '--package', packages[1], '--package', packages[2], '--out', linked], work)
        invoke([linked], work)
        for package in packages:
            manifest, _ = decode(package.read_bytes())
            assert 'generic-record-values-v1' in manifest['requiredFeatures'], manifest
        layout_probe = work / 'layout.chl'
        layout_probe.write_text('import alpha;void main(){let value=alpha::echo(alpha::makeNumber(47));assert(value.read()==47);}', encoding='utf-8')
        stale = work / 'stale'
        stale_linked = work / ('stale-linked.exe' if os.name == 'nt' else 'stale-linked')
        invoke([compiler, 'aot', layout_probe, '--emit-obj', '--out', stale], work)
        invoke([compiler, 'link', stale.with_suffix('.o'), '--package', packages[0], '--out', stale_linked], work)
        invoke([stale_linked], work)
        previous_linked = stale_linked.read_bytes()
        previous_package = packages[0].read_bytes()
        provider.write_text('module alpha;\n' + body.replace('struct Box<T>{T value;', 'struct Box<T>{int64 extra;T value;'), encoding='utf-8')
        invoke([compiler, 'lib', 'build', provider, '--module', 'alpha', '--package-version', '2.0.0', '--out', packages[0]], work)
        invoke([compiler, 'link', stale.with_suffix('.o'), '--package', packages[0], '--out', stale_linked], work, False)
        assert stale_linked.read_bytes() == previous_linked
        provider.unlink()
        packages[0].write_bytes(previous_package)
        if args.old_chali:
            rejected = invoke([args.old_chali.resolve(), 'check', consumer], work, False)
            assert '未支持的能力' in rejected.stderr, rejected.stderr
        consumer.write_text('import alpha;import beta;void main(){alpha::Box<int64> value=beta::makeNumber(1);}', encoding='utf-8')
        rejected = invoke([compiler, 'check', consumer], work, False)
        assert '不能隐式转换' in rejected.stderr, rejected.stderr
    print('library generic values: source/package, nominal instances, private instances, owned fields and shared type identities passed')


if __name__ == '__main__':
    main()
