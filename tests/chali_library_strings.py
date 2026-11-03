"""验证独立原生库的拥有型 String 参数、返回值和借用。"""
from pathlib import Path
import argparse
import copy
import os
from compiler_path import compiler_path
from project_temp import tempfile
from chali_library_package import invoke, decode, encode


def main():
    """移走提供方源码后核对复制隔离、共享修改及双分配器释放。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--external-clang', type=Path)
    args = parser.parse_args()
    compiler = args.chali.resolve()
    tools = ['--llvm', args.external_clang.resolve()] if args.external_clang else []
    with tempfile.TemporaryDirectory(prefix='library-strings-') as folder:
        work = Path(folder)
        modules = work / 'modules/demo'
        modules.mkdir(parents=True)
        provider = work / 'provider.chl'
        package = modules / 'strings.chlib'
        provider.write_text('module demo::strings;\n'
                            'String echo(String value){return value;}\n'
                            'int64 size(String const& value){return len(value);}\n'
                            'void append(String mut& value){value=value+"!";}\n'
                            'String fresh(String value="默认值"){return value;}\n'
                            'String moved(String value){return move(value);}\n'
                            'String recursive(String value,int64 depth){if(depth==0){return value;}return recursive(value,depth-1);}\n'
                            'String changed(String value){value=value+"!";return value;}\n'
                            'void fail(String message){panic(message);}\n'
                            '#borrow(#ret,[value])\n'
                            'String const& view(String const& value){return value;}\n'
                            '#borrow(#ret,[value])\n'
                            'String mut& editable(String mut& value){return value;}\n', encoding='utf-8')
        invoke([compiler, 'lib', 'build', provider, '--module', 'demo::strings',
                '--package-version', '1.0.0', '--checked', '--with-bitcode', *tools, '--out', package], work)
        manifest, entries = decode(package.read_bytes())
        invoke([compiler, 'lib', 'inspect', package], work)
        assert manifest['targets'][0]['nativeAbi'] == 'chali-owned-v1'
        assert 'owned-string-value-v1' in manifest['requiredFeatures']
        interface = next(entry['data'].decode() for entry in entries if entry['kind'] == 1)
        assert '#abi("ChaLiOwned"' in interface
        assert {'lifetime-contract-v1', 'borrowed-result-v1'} <= set(manifest['requiredFeatures'])
        for target in ['linux-x64', 'linux-arm64']:
            target_package = work / (target + '.chlib')
            invoke([compiler, 'lib', 'build', provider, '--module', 'demo::strings',
                    '--package-version', '1.0.0', '--checked', '--with-bitcode', '--target', target,
                    *tools, '--out', target_package], work)
            target_manifest, _ = decode(target_package.read_bytes())
            assert target_manifest['targets'][0]['nativeAbi'] == 'chali-owned-v1'
        bridge = modules / 'bridge.chlib'
        provider.write_text('module demo::bridge;\nimport demo::strings;\n'
                            'String forward(String value){return demo::strings::echo(value);}\n', encoding='utf-8')
        invoke([compiler, 'lib', 'build', provider, '--module', 'demo::bridge',
                '--package-version', '1.0.0', '--checked', '--with-bitcode', *tools, '--out', bridge], work)
        provider.unlink()
        consumer = work / 'main.chl'
        consumer.write_text('import demo::bridge;\nimport std::memory;\n'
                            'void main(){String value="hello";'
                            'String copied=demo::strings::echo(value);'
                            'demo::strings::append(&value);'
                            'assert(value=="hello!");assert(copied=="hello");'
                            'assert(demo::strings::size(&value)==6);'
                            '{String const& view=demo::strings::view(&value);assert(view=="hello!");}'
                            '{String mut& edit=demo::strings::editable(&value);*edit="changed";}'
                            'assert(value=="changed");assert(copied=="hello");'
                            'assert(demo::strings::fresh()=="默认值");assert(demo::strings::fresh("explicit")=="explicit");'
                            'String longText="长字符串012345678901234567890123456789";'
                            'String longCopy=demo::bridge::forward(longText);'
                            'demo::strings::append(&longText);'
                            'assert(longCopy=="长字符串012345678901234567890123456789");'
                            'assert(longText=="长字符串012345678901234567890123456789!");'
                            'assert(demo::strings::echo("")=="");'
                            'assert(demo::strings::moved(copied)=="hello");'
                            'assert(demo::strings::recursive(copied,3)=="hello");'
                            'assert(demo::strings::changed(copied)=="hello!");assert(copied=="hello");'
                            'let before=std::memory::__memoryLiveBlocks();'
                            'int64 i=0;while(i<100){String item=demo::strings::echo(copied);'
                            'demo::strings::append(&item);assert(item=="hello!");i=i+1;}'
                            'assert(std::memory::__memoryLiveBlocks()==before);'
                            'println("owning String library OK");}', encoding='utf-8')
        for implementation, mode in [('native', 'off'), ('bitcode', 'off'), ('bitcode', 'full'), ('bitcode', 'thin')]:
            for allocator in ['malloc', 'pool']:
                assert invoke([compiler, 'run', consumer, '--allocator', allocator,
                               '--library-implementation', implementation, '--lto-mode', mode,
                               *tools, '--out', work / (implementation + '-' + mode + '-' + allocator)], work).stdout == 'owning String library OK\n'
        output = work / 'unchecked'
        invoke([compiler, 'aot', consumer, *tools, '--out', output], work)
        executable = output.with_suffix('.exe') if os.name == 'nt' else output
        assert invoke([executable], work).stdout == 'owning String library OK\n'
        obj = work / 'detached'
        invoke([compiler, 'aot', consumer, '--emit-obj', *tools, '--out', obj], work)
        linked = work / ('linked.exe' if os.name == 'nt' else 'linked')
        invoke([compiler, 'link', obj.with_suffix('.o'), '--package', bridge, *tools, '--out', linked], work)
        assert invoke([linked], work).stdout == 'owning String library OK\n'
        original = package.read_bytes()
        for mutation in ['feature', 'abi']:
            invalid = copy.deepcopy(manifest)
            if mutation == 'feature':
                invalid['requiredFeatures'].remove('owned-string-value-v1')
            else:
                invalid['targets'][0]['nativeAbi'] = 'chali-value-v1'
            package.write_bytes(encode(invalid, entries))
            rejected = invoke([compiler, 'lib', 'inspect', package], work, False)
            assert 'String 拥有值 ABI 与必需能力不匹配' in rejected.stderr, rejected.stderr
        package.write_bytes(original)
        invalid = work / 'invalid.chl'
        for source in ['#abi("ChaLiOwned","@link","forged") String forged(String value);void main(){}',
                       'import demo::strings;void main(){String value="x";String const& view=&value;demo::strings::append(view);}',
                       'import demo::strings;String const& escape(){String local="x";return demo::strings::view(&local);}void main(){}']:
            invalid.write_text(source, encoding='utf-8')
            invoke([compiler, 'check', invalid], work, False)
        invalid.write_text('import demo::strings;void main(){demo::strings::fail("library String failure");}', encoding='utf-8')
        for implementation in ['native', 'bitcode']:
            failed = invoke([compiler, 'run', invalid, '--library-implementation', implementation,
                             *tools, '--out', work / ('failure-' + implementation)], work, False)
            assert 'library String failure' in failed.stdout + failed.stderr
    print('owning String library: independent copies, borrowed mutation and shared lifetime OK')


if __name__ == '__main__':
    main()
