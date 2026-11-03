"""对照无源码库包的基础标量逻辑引用、返回来源及方法参数。"""
from pathlib import Path
import argparse
import copy
import os
from compiler_path import compiler_path
from project_temp import tempfile
from chali_library_package import invoke, decode, encode


def main():
    """保留标量宽度、只读及可写借用，不把逻辑地址转换为物理指针。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--old-chali', type=Path)
    args = parser.parse_args()
    compiler = args.chali.resolve()
    os.environ['CHALI_LANG'] = 'zh'
    types = [
        ('bool', 'false', 'true'), ('char', "'a'", "'z'"),
        ('int8', '-120', '-113'), ('uint8', '249', '251'),
        ('int16', '-31000', '-32000'), ('uint16', '64000', '65000'),
        ('int32', '-2000000000', '-2100000000'), ('uint32', '4100000000', '4200000000'),
        ('int64', '-9007199254740993', '-9007199254740995'),
        ('uint64', '18446744073709551613u', '18446744073709551614u'),
        ('float32', '1.25', '2.5'), ('float64', '-1.125', '3.25'),
    ]
    declarations = '\n'.join(
        f'{kind} read_{kind}({kind} const& value){{return *value;}}'
        f'void write_{kind}({kind} mut& value,{kind} next){{*value=next;}}'
        for kind, _, _ in types)
    body = declarations + '''
#borrow(#ret,[value])
int64 const& view(int64 const& value){return value;}
#borrow(#ret,[value])
int64 mut& editable(int64 mut& value){return value;}
#static(value)
int64 readStatic(int64 const& value){return *value;}
int64 sum(int64 const& values,int64 count){int64 total=0;int64 index=0;while(index<count){total=total+values[index];index=index+1;}return total;}
struct Counter{int64 value;int64 adjust(int64 mut& other){*other=*other+self.value;return *other;}}
'''
    bridge_body = '''import scalars;
#borrow(#ret,[value])
int64 mut& relay(int64 mut& value){return scalars::editable(value);}
'''
    assertions = '\n'.join(
        f'{kind} value_{kind}=({kind})({initial});assert(scalars::read_{kind}(&value_{kind})==({kind})({initial}));'
        f'scalars::write_{kind}(&value_{kind},({kind})({changed}));assert(value_{kind}==({kind})({changed}));'
        for kind, initial, changed in types)
    valid = '''import scalars;
import bridge;
import std::memory;
int64 stable=37;
void main(){
''' + assertions + '''
int64 value=17;int64 const& readonly=scalars::view(&value);assert(*readonly==17);
int64 mut& writable=bridge::relay(&value);*writable=23;assert(value==23&&*readonly==23);
int64 values[3];values[0]=2;values[1]=3;values[2]=5;assert(scalars::sum(values,3)==10);
scalars::write_int64(&values[1],7);assert(values[1]==7&&scalars::sum(values,3)==14);
scalars::Counter counter;counter.value=4;scalars::Counter const& cview=&counter;
assert(cview.adjust(&value)==27&&value==27&&counter.value==4);
scalars::write_int64(&counter.value,8);assert(counter.value==8);
assert(scalars::readStatic(&stable)==37);
let before=std::memory::__memoryLiveBlocks();int64 index=0;
while(index<30){int64 mut& heap=new int64;*heap=index;{int64 mut& alias=bridge::relay(heap);*alias=*alias+1;assert(*heap==index+1);}heap.drop();index=index+1;}
assert(std::memory::__memoryLiveBlocks()==before);
}
'''
    with tempfile.TemporaryDirectory(prefix='library-scalar-refs-') as directory:
        work = Path(directory)
        modules = work / 'modules'
        modules.mkdir()
        provider = modules / 'scalars.chl'
        bridge_source = modules / 'bridge.chl'
        consumer = work / 'main.chl'
        provider.write_text('module scalars;\n' + body, encoding='utf-8')
        bridge_source.write_text('module bridge;\n' + bridge_body, encoding='utf-8')
        consumer.write_text(valid, encoding='utf-8')
        invoke([compiler, 'check', consumer], work)
        invoke([compiler, 'run', consumer, '--out', work / 'source-program'], work)
        package = modules / 'scalars.chlib'
        bridge = modules / 'bridge.chlib'
        invoke([compiler, 'lib', 'build', provider, '--module', 'scalars', '--package-version', '1.0.0',
                '--checked', '--with-bitcode', '--out', package], work)
        provider.unlink()
        invoke([compiler, 'lib', 'build', bridge_source, '--module', 'bridge', '--package-version', '1.0.0',
                '--checked', '--with-bitcode', '--out', bridge], work)
        bridge_source.unlink()
        manifest, entries = decode(package.read_bytes())
        original = package.read_bytes()
        assert 'scalar-reference-v1' in manifest['requiredFeatures'], manifest
        invoke([compiler, 'check', consumer], work)
        for implementation, mode in [('native', 'off'), ('bitcode', 'off'), ('bitcode', 'full'), ('bitcode', 'thin')]:
            for allocator in ['malloc', 'pool']:
                invoke([compiler, 'run', consumer, '--library-implementation', implementation, '--lto-mode', mode,
                        '--allocator', allocator, '--out', work / (implementation + '-' + mode + '-' + allocator)], work)
        obj = work / 'detached'
        invoke([compiler, 'aot', consumer, '--emit-obj', '--out', obj], work)
        linked = work / ('linked.exe' if os.name == 'nt' else 'linked')
        invoke([compiler, 'link', obj.with_suffix('.o'), '--package', bridge, '--out', linked], work)
        invoke([linked], work)
        unchecked = work / 'unchecked'
        invoke([compiler, 'aot', consumer, '--out', unchecked], work)
        invoke([unchecked.with_suffix('.exe') if os.name == 'nt' else unchecked], work)
        bounds = work / 'bounds.chl'
        bounds.write_text('import scalars;void main(){int64 values[2];values[0]=1;values[1]=2;println("{}",scalars::sum(values,3));}', encoding='utf-8')
        bounds_output = work / 'bounds'
        invoke([compiler, 'aot', bounds, '--out', bounds_output], work)
        rejected = invoke([bounds_output.with_suffix('.exe') if os.name == 'nt' else bounds_output], work, False)
        assert '越界' in rejected.stderr or '边界' in rejected.stderr, rejected.stderr
        for text in [
            'void main(){int64 value=1;int64 const& view=&value;scalars::write_int64(view,2);}',
            'void main(){float64 value=1.0;scalars::read_int64(&value);}',
            'int64 const& bad(){int64 local=1;return scalars::view(&local);}void main(){}',
            'void main(){int64 value=1;scalars::readStatic(&value);}',
        ]:
            consumer.write_text('import scalars;\n' + text, encoding='utf-8')
            invoke([compiler, 'check', consumer], work, False)
        consumer.write_text(valid, encoding='utf-8')
        for invalid in [
            '#export("physical_reference") int64 read(int64 const& value){return *value;}',
            'int64 const& view(int64 const& value){return value;}',
            'int64 read(int64 const& value=(int64 const&)0){return *value;}',
        ]:
            provider.write_text('module scalars;\n' + invalid, encoding='utf-8')
            invoke([compiler, 'lib', 'build', provider, '--module', 'scalars', '--package-version', '2.0.0', '--out', package], work, False)
            assert package.read_bytes() == original
        provider.unlink()
        layout_probe = work / 'layout.chl'
        layout_probe.write_text('import scalars;void main(){int64 value=1;assert(scalars::read_int64(&value)==1);}', encoding='utf-8')
        stale = work / 'stale'
        stale_output = work / ('stale-linked.exe' if os.name == 'nt' else 'stale-linked')
        invoke([compiler, 'aot', layout_probe, '--emit-obj', '--out', stale], work)
        changed_body = body.replace('int64 read_int64(int64 const& value)', 'int64 read_int64(int32 const& value)')
        assert changed_body != body
        provider.write_text('module scalars;\n' + changed_body, encoding='utf-8')
        invoke([compiler, 'lib', 'build', provider, '--module', 'scalars', '--package-version', '2.0.0', '--out', package], work)
        provider.unlink()
        stale_output.write_bytes(b'keep prior scalar result')
        invoke([compiler, 'link', stale.with_suffix('.o'), '--package', package, '--out', stale_output], work, False)
        assert stale_output.read_bytes() == b'keep prior scalar result'
        package.write_bytes(original)
        consumer.write_text('import scalars;void main(){int64 value=1;assert(scalars::read_int64(&value)==1);}', encoding='utf-8')
        invoke([compiler, 'check', consumer], work)
        missing = copy.deepcopy(manifest)
        missing['requiredFeatures'].remove('scalar-reference-v1')
        package.write_bytes(encode(missing, entries))
        rejected = invoke([compiler, 'check', consumer], work, False)
        assert 'scalar-reference-v1' in rejected.stderr, rejected.stderr
        package.write_bytes(original)
        consumer.write_text(valid, encoding='utf-8')
        if args.old_chali:
            rejected = invoke([args.old_chali.resolve(), 'check', consumer], work, False)
            assert '未支持的能力' in rejected.stderr, rejected.stderr
    print('scalar references: source/package, widths, readonly/mutable, lifetime, forwarding, methods and allocators passed')


if __name__ == '__main__':
    main()
