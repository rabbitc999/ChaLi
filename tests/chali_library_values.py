"""验证预编译普通记录按值参数和跨库独立副本。"""
from pathlib import Path
import argparse
import os
from compiler_path import compiler_path
from project_temp import tempfile
from chali_library_package import invoke, decode


def main():
    """移除实现源码后比较记录复制及共享分配器的行为。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    with tempfile.TemporaryDirectory(prefix='library-values-') as folder:
        work = Path(folder)
        modules = work / 'modules/demo'
        modules.mkdir(parents=True)
        provider = work / 'provider.chl'
        package = modules / 'values.chlib'
        source = ('module demo::values;\n'
                  'struct Point {int64 x;float64 y;};\n'
                  'struct Pair {Point left;Point right;uint8 tag;};\n'
                  'int64 shift(Point value,int64 delta){value.x=value.x+delta;value.y=value.y+0.5;return value.x;}\n'
                  'int64 read(Point value){value.x=value.x+1;return value.x;}\n'
                  'float64 twice(Pair value){value.left.x=shift(value.left,2);value.right.x=shift(value.right,3);value.tag=(uint8)255;return (float64)value.left.x+(float64)value.right.x+value.left.y+value.right.y;}\n')
        provider.write_text(source, encoding='utf-8')
        for target in ['windows-x64', 'linux-x64', 'linux-arm64']:
            invoke([compiler, 'lib', 'build', provider, '--module', 'demo::values',
                    '--package-version', '1.0.0', '--checked', '--target', target,
                    '--out', package if target == 'windows-x64' else work / (target + '.chlib')], work)
        manifest, entries = decode(package.read_bytes())
        assert manifest['targets'][0]['nativeAbi'] == 'chali-value-v1'
        assert 'plain-record-value-v1' in manifest['requiredFeatures']
        interface = next(e['data'].decode() for e in entries if e['kind'] == 1)
        assert '#abi("ChaLiValue"' in interface
        original = package.read_bytes()
        provider.write_text(source + 'Point returned(Point value){return value;}\n', encoding='utf-8')
        invoke([compiler, 'lib', 'build', provider, '--module', 'demo::values',
                '--package-version', '1.0.0', '--out', package], work, False)
        assert package.read_bytes() == original
        provider.write_text('module demo::bridge;\nimport demo::values;\n'
                            'int64 forward(demo::values::Point value){return demo::values::shift(value,10);}\n', encoding='utf-8')
        bridge = modules / 'bridge.chlib'
        invoke([compiler, 'lib', 'build', provider, '--module', 'demo::bridge',
                '--package-version', '1.0.0', '--checked', '--out', bridge], work)
        provider.unlink()
        consumer = work / 'main.chl'
        consumer.write_text('import demo::bridge;\nimport std::memory;\nvoid main(){'
                            'demo::values::Point original;original.x=40;original.y=1.5;'
                            'assert(demo::values::read(original)==41);assert(original.x==40);'
                            'assert(demo::bridge::forward(original)==50);assert(original.y==1.5);assert(original.x==40);'
                            'demo::values::Pair pair;pair.left=original;pair.right=original;pair.tag=(uint8)1;'
                            'assert(demo::values::twice(pair)==88.0);assert(pair.left.x==40);assert(pair.tag==(uint8)1);'
                            'let before=std::memory::__memoryLiveBlocks();'
                            'int64 i=0;while(i<100){assert(demo::values::shift(original,2)==42);i=i+1;}'
                            'assert(std::memory::__memoryLiveBlocks()==before);'
                            'println("record values OK");}', encoding='utf-8')
        for allocator in ['malloc', 'pool']:
            assert invoke([compiler, 'run', consumer, '--allocator', allocator, '--out', work / allocator], work).stdout == 'record values OK\n'
        output = work / 'unchecked'
        invoke([compiler, 'aot', consumer, '--out', output], work)
        executable = output.with_suffix('.exe') if os.name == 'nt' else output
        assert invoke([executable], work).stdout == 'record values OK\n'
        assert not output.with_suffix('.abi.cpp').exists()
        obj = work / 'consumer'
        invoke([compiler, 'aot', consumer, '--emit-obj', '--out', obj], work)
        linked = work / ('linked.exe' if os.name == 'nt' else 'linked')
        invoke([compiler, 'link', obj.with_suffix('.o'), '--package', bridge, '--out', linked], work)
        assert invoke([linked], work).stdout == 'record values OK\n'
    print('plain record parameters: independent nested copies, two allocators and detached linking OK')


if __name__ == '__main__':
    main()
