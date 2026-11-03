"""移走源码后验证泛型库的宏、派生和定义模块导入上下文。"""
from pathlib import Path
import argparse
import copy
import json
import os
from compiler_path import compiler_path
from project_temp import tempfile
from chali_library_package import invoke, decode, encode


def main():
    """比较源码及库包消费，并拒绝语义上下文中的普通代码注入。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    args = parser.parse_args()
    compiler = args.chali.resolve()
    os.environ['CHALI_LANG'] = 'zh'
    with tempfile.TemporaryDirectory(prefix='library-template-contexts-') as directory:
        work = Path(directory)
        modules = work / 'modules'
        modules.mkdir()
        provider = modules / 'context.chl'
        package = modules / 'context.chlib'
        provider.write_text('''module context;
import std::parse;
$macro ID(value:expr) -> expr
value
$end_macro
$replace SINGLE(value:expr) -> expr value
$macro GETTER<U>(gname:ident,field:ident) -> member
U gname(){return self.field;}
$end_macro
T identity<T>(T value){return $ID /* portable group */ (value);}
T nested<T>(T value){return $ID($ID(value));}
T single<T>(T value){return $SINGLE(value);}
int64 parsed<T>(T ignored){return std::parse::Parse::parseInt64("42").unwrap();}
struct Box<T>{T value;$GETTER<T>(read,value);}
#derive(Equal,Clone)
struct Derived<T>{T value;}
#NoEqualReplace
#derive(Equal)
struct Explicit<T>{T value;}
''', encoding='utf-8')
        consumer = work / 'main.chl'
        consumer.write_text('''import context;
$macro ID(value:expr) -> expr
-99
$end_macro
void main(){
assert(context::identity<String>("@!literal")=="@!literal");
assert(context::nested<int64>(7)==7&&context::parsed<int64>(0)==42);
assert(context::single<int64>(8)==8);
context::Box<String> box;box.value="text";assert(box.read()=="text");
context::Derived<String> value;value.value="owned";let copied=value.clone();assert(value==copied);
}
''', encoding='utf-8')
        invoke([compiler, 'check', consumer], work)
        invoke([compiler, 'run', consumer, '--out', work / 'source-program'], work)
        invoke([compiler, 'lib', 'build', provider, '--module', 'context',
                '--package-version', '1.0.0', '--with-bitcode', '--out', package], work)
        original = package.read_bytes()
        manifest, entries = decode(original)
        metadata_entry = next(item for item in entries if item['name'] == manifest['templates'])
        metadata = json.loads(metadata_entry['data'])
        assert metadata['imports'] == ['std::parse'], metadata
        assert len(metadata['macros']) == 3, metadata
        assert all('@!macrofragment' not in item['declaration'] for item in metadata['definitions']), metadata
        assert any(item['derivation'].strip() == '#derive(Equal,Clone)' for item in metadata['definitions']), metadata
        assert any('#NoEqualReplace' in item['operatorSwitches'] for item in metadata['definitions']), metadata
        provider.unlink()
        invoke([compiler, 'check', consumer], work)
        for implementation, mode in [('native', 'off'), ('bitcode', 'full'), ('bitcode', 'thin')]:
            for allocator in ['malloc', 'pool']:
                invoke([compiler, 'run', consumer, '--library-implementation', implementation,
                        '--lto-mode', mode, '--allocator', allocator,
                        '--out', work / (implementation + '-' + mode + '-' + allocator)], work)
        mutations = [
            ('macro_implementation', lambda data: data['macros'].__setitem__(0, data['macros'][0] + '\nint64 injected(){return 1;}')),
            ('macro_session', lambda data: data['macros'].__setitem__(0, data['macros'][0].replace('value\n', '$ID@!99(@!macrofragment99_0)\n'))),
            ('derive_implementation', lambda data: data['definitions'][0].__setitem__('derivation', '#derive(Equal)\nint64 injected(){return 1;}')),
            ('switch_implementation', lambda data: data['definitions'][0].__setitem__('operatorSwitches', '#NoEqualReplace\nint64 injected(){return 1;}')),
            ('unpinned_import', lambda data: data['imports'].append('missing')),
        ]
        for name, mutate in mutations:
            altered = copy.deepcopy(metadata)
            mutate(altered)
            changed = copy.deepcopy(entries)
            next(item for item in changed if item['name'] == manifest['templates'])['data'] = (
                json.dumps(altered, ensure_ascii=False, separators=(',', ':')) + '\n').encode()
            package.write_bytes(encode(manifest, changed))
            result = invoke([compiler, 'lib', 'inspect', package], work, False)
            assert '泛型库' in result.stderr + result.stdout, (name, result.stdout, result.stderr)
            invoke([compiler, 'check', consumer], work, False)
        package.write_bytes(original)
        invoke([compiler, 'check', consumer], work)
        macro_source = modules / 'records.chl'
        macro_source.write_text('''module records;
$macro RECORD<T>(name:ident) -> item
struct name{T value;static name create(T input){self.value=input;return self;}}
$end_macro
''', encoding='utf-8')
        macro_package = modules / 'records.chlib'
        invoke([compiler, 'lib', 'build', macro_source, '--module', 'records',
                '--package-version', '1.0.0', '--out', macro_package], work)
        macro_source.unlink()
        consumer.write_text('''import records;
$RECORD<String>(Label);
void main(){Label value=Label::create("generated");assert(value.value=="generated");}
''', encoding='utf-8')
        invoke([compiler, 'check', consumer], work)
        invoke([compiler, 'run', consumer, '--out', work / 'macro-only-program'], work)
        dependency_source = modules / 'dependency.chl'
        dependency_source.write_text('module dependency;int64 add(int64 value){return value+2;}\n', encoding='utf-8')
        bridge_source = modules / 'bridge.chl'
        bridge_source.write_text('''module bridge;
import dependency;
int64 plus<T>(T value){return dependency::add(value);}
''', encoding='utf-8')
        bridge_package = modules / 'bridge.chlib'
        bridge_package.write_bytes(b'previous output')
        rejected = invoke([compiler, 'lib', 'build', bridge_source, '--module', 'bridge',
                           '--package-version', '1.0.0', '--out', bridge_package], work, False)
        assert '导入缺少精确库依赖' in rejected.stderr, rejected.stderr
        assert bridge_package.read_bytes() == b'previous output'
        dependency_package = modules / 'dependency.chlib'
        invoke([compiler, 'lib', 'build', dependency_source, '--module', 'dependency',
                '--package-version', '1.0.0', '--out', dependency_package], work)
        dependency_source.unlink()
        invoke([compiler, 'lib', 'build', bridge_source, '--module', 'bridge',
                '--package-version', '1.0.0', '--out', bridge_package], work)
        bridge_source.unlink()
        bridge_manifest, _ = decode(bridge_package.read_bytes())
        assert [item['module'] for item in bridge_manifest['dependencies']] == ['dependency'], bridge_manifest
        consumer.write_text('import bridge;void main(){assert(bridge::plus<int64>(40)==42);}\n', encoding='utf-8')
        invoke([compiler, 'check', consumer], work)
        invoke([compiler, 'run', consumer, '--out', work / 'pinned-program'], work)
    print('library template contexts: source/package, macros, derives, imports, ownership and rejection passed')


if __name__ == '__main__':
    main()
