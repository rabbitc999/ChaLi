"""验证预编译记录的共享生命周期类型身份与新鲜值转移。"""
from pathlib import Path
import argparse
import os
import re
import subprocess
import json
from unittest.mock import patch
from compiler_path import compiler_path
from project_temp import tempfile
from chali_library_package import invoke, decode


def main():
    """改变消费者类型分配顺序后核对跨库、递归、移动与清理。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    with tempfile.TemporaryDirectory(prefix='library-moves-') as folder:
        work = Path(folder)
        modules = work / 'modules/demo'
        modules.mkdir(parents=True)
        provider = work / 'provider.chl'
        first = modules / 'first.chlib'
        provider.write_text('module demo::first;\nstruct Point {int64 x;float64 y;};\n'
                            'int64 literal(){return 33;}\n'
                            'Point make(int64 x){Point result;result.x=x;result.y=1.5;return result;}\n'
                            'Point recurse(Point value,int64 depth){if(depth<=0){return value;}value.x=value.x+1;return recurse(value,depth-1);}\n'
                            'Point explicitMove(Point value){Point copied=move value;copied.x=copied.x+2;return copied;}\n', encoding='utf-8')
        with patch.dict(os.environ, {'CHALI_BACKEND_PROFILE': '1'}):
            invoke([compiler, 'lib', 'build', provider, '--module', 'demo::first', '--package-version', '1.0.0', '--checked', '--out', first], work)
        manifest, assets = decode(first.read_bytes())
        assert 'shared-type-id-v1' in manifest['requiredFeatures']
        second = modules / 'second.chlib'
        provider.write_text('module demo::second;\nimport demo::first;\n'
                            'demo::first::Point forward(int64 x){return demo::first::make(x);}\n'
                            'demo::first::Point transform(demo::first::Point value){let moved=demo::first::explicitMove(value);return demo::first::recurse(moved,3);}\n', encoding='utf-8')
        invoke([compiler, 'lib', 'build', provider, '--module', 'demo::second', '--package-version', '1.0.0', '--checked', '--out', second], work)
        provider.unlink()
        (modules / 'noise.chl').write_text('module demo::noise;\nstruct Point {int64 unused;};\nint64 moveNoise(){Point source;source.unused=99;Point target=move source;return target.unused;}\n', encoding='utf-8')
        consumer = work / 'main.chl'
        consumer.write_text('import demo::noise;\nimport demo::second;\nimport std::memory;\nvoid main(){'
                            'assert(demo::noise::moveNoise()==99);assert(demo::first::literal()==33);let original=demo::second::forward(40);'
                            'let changed=demo::second::transform(original);assert(original.x==40 && changed.x==45 && changed.y==1.5);'
                            'let before=std::memory::__memoryLiveBlocks();int64 i=0;while(i<50){'
                            'let temporary=demo::second::transform(demo::second::forward(i));assert(temporary.x==i+5);i=i+1;}'
                            'assert(std::memory::__memoryLiveBlocks()==before);'
                            'println("shared moves OK");}', encoding='utf-8')
        for allocator in ['malloc', 'pool']:
            assert invoke([compiler, 'run', consumer, '--allocator', allocator, '--out', work / allocator], work).stdout == 'shared moves OK\n'
        output = work / 'unchecked'
        invoke([compiler, 'aot', consumer, '--out', output], work)
        executable = output.with_suffix('.exe') if os.name == 'nt' else output
        assert invoke([executable], work).stdout == 'shared moves OK\n'
        llvm = output.with_suffix('.ll').read_text(encoding='utf-8')
        assert 'chali_native_type_' in llvm and '@chali_host_storage_lifetime' in llvm
        native = next(e['data'] for e in assets if e['kind'] == 3)
        symbols = re.findall(r'@"(chali_native_type_[0-9a-f]+)" = constant i64', llvm)
        assert any(name.encode() in native for name in symbols)
        assert not output.with_suffix('.abi.cpp').exists()
        obj = work / 'consumer'
        invoke([compiler, 'aot', consumer, '--emit-obj', '--out', obj], work)
        linked = work / ('linked.exe' if os.name == 'nt' else 'linked')
        invoke([compiler, 'link', obj.with_suffix('.o'), '--package', second, '--out', linked], work)
        assert invoke([linked], work).stdout == 'shared moves OK\n'
        profile = work / 'unit-profile.json'
        with patch.dict(os.environ, {'CHALI_BACKEND_PROFILE_OUTPUT': str(profile)}):
            assert invoke([linked], work).stdout == 'shared moves OK\n'
        events = json.loads(profile.read_text(encoding='utf-8'))['events']
        assert any('\tdemo::first::' in event['site'] and '\tast=' in event['site'] for event in events)
        invalid = modules / 'invalid.chlib'
        provider.write_text('module demo::invalid;\nstruct Point {int64 x;};\n'
                            'int64 repeated(Point value){Point first=move value;Point second=move value;return second.x;}\n', encoding='utf-8')
        result = subprocess.run(list(map(str, [compiler, 'lib', 'build', provider, '--module', 'demo::invalid',
                                               '--package-version', '1.0.0', '--checked', '--out', invalid])),
                                cwd=work, capture_output=True, encoding='utf-8', timeout=180)
        if result.returncode:
            assert '源对象已经销毁' in result.stdout + result.stderr or 'CHL1205' in result.stdout + result.stderr
        else:
            consumer.write_text('import demo::invalid;\nvoid main(){demo::invalid::Point value;value.x=1;demo::invalid::repeated(value);}', encoding='utf-8')
            result = invoke([compiler, 'run', consumer, '--out', work / 'invalid'], work, False)
            assert '源对象已经销毁' in result.stderr
    print('shared record lifetimes: reordered types, fresh values, recursion, explicit moves and stable live blocks OK')


if __name__ == '__main__':
    main()
