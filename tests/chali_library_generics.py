"""移走实现源码后核对库泛型定义、原生入口、模块身份和损坏元数据。"""
from pathlib import Path
import argparse
import copy
import json
import os
from compiler_path import compiler_path
from project_temp import tempfile
from chali_library_package import invoke, decode, encode


def main():
    """消费真实模板载荷，负例先验证合法对照再核对具体拒绝原因。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--external-clang', type=Path)
    parser.add_argument('--old-chali', type=Path)
    args = parser.parse_args()
    compiler = args.chali.resolve()
    os.environ['CHALI_LANG'] = 'zh'
    tools = ['--llvm', args.external_clang.resolve()] if args.external_clang else []
    with tempfile.TemporaryDirectory(prefix='library-generics-') as directory:
        work = Path(directory)
        modules = work / 'modules'
        modules.mkdir()
        provider = work / 'provider.chl'
        alpha = modules / 'alpha.chlib'
        beta = modules / 'beta.chlib'
        pure = modules / 'pure.chlib'
        source_modules = work / 'source-comparison/modules'
        source_modules.mkdir(parents=True)
        shared = '''
struct Box<T>{T value;T read(){return self.value;}static Box<T> create(T input){self.value=input;return self;}}
Box<T> boxed<T>(T value){return Box<T>::create(value);}
T identity<T>(T value){return value;}
T recurse<T>(T value,int64 depth){if(depth==0){return value;}return recurse<T>(value,depth-1);}
T withNative<T>(T value){return value+anchor();}
T defaulted<T=int64>(T value){return value;}
int64 counted<N:int64=3>(int64 value){return value+N;}
'''
        for module, package, code in [
            ('alpha', alpha, 'interface Coded{int64 code();}\nint64 code<T:Coded>(T value){return value.code();}\nint64 anchor(){return identity<int64>(11);}\n'
             'inline int64 hidden(int64 value=2){return value+5;}\n'
             'int64 privateUse<T>(T value){return hidden(value);}\n'
             'int64 privateDefault<T>(T ignored){return hidden();}\n'
             'int64 fn(int64) privateCallback<T>(T ignored){return hidden;}\n'
             'int64 overloaded(int64 value){return value+1;}\nfloat64 overloaded(float64 value){return value+0.5;}\n' + shared),
            ('beta', beta, 'int64 anchor(){return identity<int64>(22);}\n' + shared),
            ('pure', pure, 'T identity<T>(T value){return value;}\n'),
        ]:
            provider.write_text('module ' + module + ';\n' + code, encoding='utf-8')
            (source_modules / (module + '.chl')).write_bytes(provider.read_bytes())
            invoke([compiler, 'lib', 'build', provider, '--module', module,
                    '--package-version', '1.0.0', '--with-bitcode', *tools, '--out', package], work)
            manifest, entries = decode(package.read_bytes())
            assert manifest['source'] == '' and 'generic-definitions-v1' in manifest['requiredFeatures'], manifest
            metadata = next(entry['data'] for entry in entries if entry['name'] == manifest['templates'])
            definitions = json.loads(metadata)
            assert definitions['format'] == 1 and definitions['module'] == module
            assert definitions['definitions'] and str(provider) not in metadata.decode(), metadata
            interface = next(entry['data'].decode() for entry in entries if entry['name'] == manifest['interface'])
            assert 'identity__int64' not in interface, interface
            if module == 'alpha':
                assert 'inline #abi' in interface and 'hidden(' in interface and 'chaliDefault_' in interface, interface
        provider.unlink()
        consumer = work / 'main.chl'
        valid = '''import alpha;
import alpha;
import beta;
import pure;
struct Local:Coded{int64 code(){return 31;}}
void main(){
assert(alpha::anchor()==11&&beta::anchor()==22);
assert(alpha::identity<int64>(42)==42&&beta::identity<int64>(43)==43);
assert(alpha::identity(44)==44&&beta::identity("inferred")=="inferred");
assert(alpha::identity<String>("中文") == "中文"&&pure::identity<String>("pure")=="pure");
assert(alpha::recurse<String>("recursive",4)=="recursive");
assert(alpha::withNative<int64>(1)==12&&beta::withNative<int64>(1)==23);
assert(alpha::privateUse<int64>(7)==12&&alpha::privateDefault<int64>(0)==7);
let callback=alpha::privateCallback<int64>(0);assert(callback(3)==8);
int64 overloaded=90;
int64 fn(int64) selected=alpha::overloaded;
float64 fn(float64) fractional=alpha::overloaded;
int64 fn() anchorFunction=alpha::anchor;
assert(overloaded==90&&selected(4)==5&&fractional(1.0)==1.5&&anchorFunction()==11);
let selectedCopy=alpha::identity<int64 fn(int64)>(selected);assert(selectedCopy(8)==9);
assert(alpha::defaulted<>(8)==8&&alpha::counted<>(4)==7&&alpha::counted<9>(4)==13);
Local local;assert(alpha::code<Local>(local)==31);
alpha::Box<int64> number=alpha::boxed<int64>(17);
alpha::Box<String> text=alpha::boxed<String>("owned");
beta::Box<int64> other=beta::boxed<int64>(19);
assert(number.read()==17&&text.read()=="owned"&&other.read()==19);
List<String> values;values.push("original");let copied=alpha::identity<List<String>>(values);copied[0]="copy";
assert(values[0]=="original"&&copied[0]=="copy");
}
'''
        consumer.write_text(valid, encoding='utf-8')
        source_consumer = source_modules.parent / 'main.chl'
        source_consumer.write_text(valid, encoding='utf-8')
        invoke([compiler, 'check', source_consumer], source_modules.parent)
        invoke([compiler, 'run', source_consumer, *tools, '--out', work / 'source-program'], source_modules.parent)
        invoke([compiler, 'check', consumer], work)
        for implementation, mode in [('native', 'off'), ('bitcode', 'off'), ('bitcode', 'full'), ('bitcode', 'thin')]:
            for allocator in ['malloc', 'pool']:
                invoke([compiler, 'run', consumer, '--allocator', allocator,
                        '--library-implementation', implementation, '--lto-mode', mode,
                        *tools, '--out', work / (implementation + '-' + mode + '-' + allocator)], work)
        for name, code, message in [
            ('constraint', 'void main(){alpha::code<int64>(1);}', '未实现接口'),
            ('nominal_type', 'void main(){alpha::Box<int64> a;beta::Box<int64> b=a;}', '不能隐式转换'),
            ('bad_argument', 'void main(){alpha::identity<Missing>(1);}', 'Missing'),
            ('private_call', 'void main(){alpha::hidden(1);}', '不能从模块'),
            ('private_value', 'void main(){int64 fn(int64) value=alpha::hidden;}', '不能从模块'),
            ('private_bare_value', 'void main(){int64 fn(int64) value=hidden;}', '不能从模块'),
        ]:
            bad = work / (name + '.chl')
            bad.write_text('import alpha;\nimport beta;\n' + code, encoding='utf-8')
            rejected = invoke([compiler, 'check', bad], work, False)
            assert message in rejected.stderr, rejected
        original = alpha.read_bytes()
        manifest, entries = decode(original)
        metadata_entry = next(entry for entry in entries if entry['name'] == manifest['templates'])
        metadata = json.loads(metadata_entry['data'])
        if args.old_chali:
            rejected = invoke([args.old_chali.resolve(), 'check', consumer], work, False)
            assert 'generic-definitions-v1' in rejected.stderr, rejected
        mutations = []
        changed = copy.deepcopy(metadata)
        changed['format'] = 2
        mutations.append(('version', changed, '版本'))
        changed = copy.deepcopy(metadata)
        changed['module'] = 'other'
        mutations.append(('module', changed, '身份'))
        changed = copy.deepcopy(metadata)
        changed['definitions'].append(copy.deepcopy(changed['definitions'][0]))
        mutations.append(('duplicate', changed, '名称'))
        changed = copy.deepcopy(metadata)
        changed['definitions'][0]['declaration'] += '\nint64 smuggled(){return 1;}'
        mutations.append(('ordinary_body', changed, '非模板'))
        changed = copy.deepcopy(metadata)
        changed['definitions'][0]['declaration'] = changed['definitions'][0]['declaration'].replace('{', '{\n%include "missing-source.chl"\n', 1)
        mutations.append(('preprocess', changed, '预处理'))
        changed = copy.deepcopy(metadata)
        changed['definitions'][0]['declaration'] = changed['definitions'][0]['declaration'].replace('{', '{$bad@!123;', 1)
        mutations.append(('session_marker', changed, '宏片段'))
        changed = copy.deepcopy(metadata)
        changed['definitions'][0]['parameters'][0]['fallback'] = 'int64'
        changed['definitions'][0]['parameters'][0]['hasDefault'] = False
        mutations.append(('parameter_mismatch', changed, '不一致'))
        for name, changed, message in mutations:
            altered = copy.deepcopy(entries)
            next(entry for entry in altered if entry['name'] == manifest['templates'])['data'] = (
                json.dumps(changed, ensure_ascii=False, separators=(',', ':')) + '\n').encode('utf-8')
            alpha.write_bytes(encode(manifest, altered))
            rejected = invoke([compiler, 'lib', 'inspect', alpha], work, False)
            assert message in rejected.stdout + rejected.stderr, (name, rejected)
            invoke([compiler, 'check', consumer], work, False)
            alpha.write_bytes(original)
        missing = copy.deepcopy(manifest)
        missing['requiredFeatures'].remove('generic-definitions-v1')
        alpha.write_bytes(encode(missing, entries))
        invoke([compiler, 'lib', 'inspect', alpha], work)
        rejected = invoke([compiler, 'check', consumer], work, False)
        assert 'generic-definitions-v1' in rejected.stderr, rejected
        alpha.write_bytes(original)
        invoke([compiler, 'check', consumer], work)
    print('library generics: source-free templates, pure template unit, native entries, constraints/defaults, nominal types, ownership and malformed metadata passed')


if __name__ == '__main__':
    main()
