"""验证预编译数据重定位、普通记录返回及布局损坏拒绝。"""
from pathlib import Path
import argparse
import copy
import hashlib
import os
from compiler_path import compiler_path
from project_temp import tempfile
from chali_library_package import invoke, decode, encode


def symbol(manifest, layout, data):
    """独立核对单元名、目标与完整数据布局的身份。"""
    identity = (manifest['module'] + '\n' + manifest['targets'][0]['triple'] + '\n').encode() + layout + b'\n' + data
    return 'chali_unit_data_' + hashlib.sha256(identity).hexdigest()


def main():
    """实现源码删除后，由最终程序的数据映像承载多个独立库的返回模板。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    with tempfile.TemporaryDirectory(prefix='library-data-') as folder:
        work = Path(folder)
        modules = work / 'modules/demo'
        modules.mkdir(parents=True)
        provider = work / 'provider.chl'
        first = modules / 'first.chlib'
        source = ('module demo::first;\n'
                  'struct Point {int64 x;float64 y;};\n'
                  'struct Pair {Point left;Point right;};\n'
                  'Point make(int64 x,float64 y){Point result;result.x=x;result.y=y;return result;}\n'
                  'Point shift(Point value,int64 delta){value.x=value.x+delta;return value;}\n'
                  'Pair pair(int64 x){Pair value;value.left.x=x;value.left.y=1.5;value.right.x=x+1;value.right.y=2.5;return value;}\n')
        provider.write_text(source, encoding='utf-8')
        for target in ['windows-x64', 'linux-x64', 'linux-arm64']:
            invoke([compiler, 'lib', 'build', provider, '--module', 'demo::first',
                    '--package-version', '1.0.0', '--checked', '--target', target,
                    '--out', first if target == 'windows-x64' else work / (target + '.chlib')], work)
        manifest, entries = decode(first.read_bytes())
        target = manifest['targets'][0]
        assert 'unit-data-v1' in manifest['requiredFeatures']
        data = next(e['data'] for e in entries if e['name'] == target['unitData'])
        layout = next(e['data'] for e in entries if e['name'] == target['unitLayout'])
        assert target['unitSymbol'] == symbol(manifest, layout, data)
        assert all(e['kind'] == 9 for e in entries if e['name'] in [target['unitData'], target['unitLayout']])
        second = modules / 'second.chlib'
        provider.write_text('module demo::second;\nimport demo::first;\n'
                            'struct Point {int64 x;float64 y;};\n'
                            'Point make(int64 x){Point value;value.x=x;value.y=9.5;return value;}\n', encoding='utf-8')
        invoke([compiler, 'lib', 'build', provider, '--module', 'demo::second',
                '--package-version', '1.0.0', '--checked', '--out', second], work)
        provider.unlink()
        consumer = work / 'main.chl'
        consumer.write_text('import demo::second;\nimport demo::first;\nimport std::memory;\nvoid main(){'
                            'String padding="unrelated consumer data";assert(padding=="unrelated consumer data");'
                            'let a=demo::first::make(40,1.5);let b=demo::second::make(90);'
                            'assert(a.x==40 && a.y==1.5);assert(b.x==90 && b.y==9.5);'
                            'let shifted=demo::first::shift(a,2);assert(shifted.x==42 && a.x==40);'
                            'let pair=demo::first::pair(10);assert(pair.left.x==10 && pair.right.x==11 && pair.right.y==2.5);'
                            'let before=std::memory::__memoryLiveBlocks();int64 i=0;while(i<100){'
                            'let temporary=demo::first::make(i,2.5);assert(temporary.x==i && temporary.y==2.5);i=i+1;}'
                            'assert(std::memory::__memoryLiveBlocks()==before);assert(a.x==40 && b.x==90);'
                            'println("unit data OK");}', encoding='utf-8')
        for allocator in ['malloc', 'pool']:
            assert invoke([compiler, 'run', consumer, '--allocator', allocator, '--out', work / allocator], work).stdout == 'unit data OK\n'
        output = work / 'unchecked'
        invoke([compiler, 'aot', consumer, '--out', output], work)
        assert not output.with_suffix('.abi.cpp').exists()
        executable = output.with_suffix('.exe') if os.name == 'nt' else output
        assert invoke([executable], work).stdout == 'unit data OK\n'
        llvm = output.with_suffix('.ll').read_text(encoding='utf-8')
        assert '@' + target['unitSymbol'] + ' = constant i64 ' in llvm
        detached = work / 'detached'
        invoke([compiler, 'aot', consumer, '--emit-obj', '--out', detached], work)
        linked = work / ('linked.exe' if os.name == 'nt' else 'linked')
        invoke([compiler, 'link', detached.with_suffix('.o'), '--package', second, '--out', linked], work)
        assert invoke([linked], work).stdout == 'unit data OK\n'
        original = first.read_bytes()
        stripped = copy.deepcopy(manifest)
        for key in ['unitData', 'unitLayout', 'unitSymbol']:
            stripped['targets'][0].pop(key)
        stripped['requiredFeatures'].remove('unit-data-v1')
        first.write_bytes(encode(stripped, [e for e in entries if e['kind'] != 9]))
        consumer.write_text('import demo::first;\nvoid main(){let value=demo::first::make(1,2.0);}', encoding='utf-8')
        rejected = invoke([compiler, 'check', consumer], work, False)
        assert '已验证的单元数据登记' in rejected.stderr
        first.write_bytes(original)
        bad = work / 'bad.chlib'
        changed = copy.deepcopy(manifest)
        changed['targets'][0]['unitSymbol'] = target['unitSymbol'][:-1] + ('0' if target['unitSymbol'][-1] != '0' else '1')
        bad.write_bytes(encode(changed, entries))
        invoke([compiler, 'lib', 'inspect', bad], work, False)
        changed = copy.deepcopy(manifest)
        changed['requiredFeatures'].remove('unit-data-v1')
        bad.write_bytes(encode(changed, entries))
        invoke([compiler, 'lib', 'inspect', bad], work, False)
        changed = copy.deepcopy(manifest)
        assets = copy.deepcopy(entries)
        invalid_layout = b'outside offset=999999999 size=8 stride=8\n'
        next(e for e in assets if e['name'] == target['unitLayout'])['data'] = invalid_layout
        changed['targets'][0]['unitSymbol'] = symbol(changed, invalid_layout, data)
        bad.write_bytes(encode(changed, assets))
        rejected = invoke([compiler, 'lib', 'inspect', bad], work, False)
        assert '布局越界' in rejected.stderr
    print('unit data: relocated record returns, two libraries/allocators, stable live blocks and corrupt layout rejection OK')


if __name__ == '__main__':
    main()
