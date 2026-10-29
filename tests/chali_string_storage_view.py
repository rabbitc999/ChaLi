"""对照 LLVM 字符串描述符读取与原宿主查询，包括不完整布局和损坏字段。"""
from compiler_path import compiler_path
from project_temp import tempfile
from pathlib import Path
import argparse
import hashlib
import json
import os
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def main():
    """两种分配器、三种读取路径，比较 checked run 与 unchecked AOT。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    work = Path(tempfile.mkdtemp(prefix='string-storage-view-'))
    records = []
    for allocator in ('malloc', 'pool'):
        expected = None
        for mode in ('host', 'view', 'cache-off'):
            env = dict(os.environ, CHALI_LANG='zh')
            env.pop('CHALI_NO_AOT_STRING_STORAGE_VIEW', None)
            env.pop('CHALI_NO_AOT_ADDRESS_CACHE', None)
            if mode == 'host':
                env['CHALI_NO_AOT_STRING_STORAGE_VIEW'] = '1'
            if mode == 'cache-off':
                env['CHALI_NO_AOT_ADDRESS_CACHE'] = '1'
            label = allocator + '-' + mode

            def execute(command, suffix, success=True):
                """逐步保存真实返回值与输出，不把预期失败吞为成功。"""
                process = subprocess.run(list(map(str, command)), cwd=ROOT, env=env,
                                         capture_output=True, text=True, encoding='utf-8', timeout=600)
                item = dict(label=label + '-' + suffix, command=list(map(str, command)),
                            code=process.returncode, stdout=process.stdout, stderr=process.stderr)
                records.append(item)
                (work / (item['label'] + '.json')).write_text(json.dumps(item, ensure_ascii=False, indent=2), encoding='utf-8')
                assert (process.returncode == 0) == success, (item, work)
                return process

            source = ROOT / 'tests/fixtures/aot_string_storage_view.chl'
            prefix = work / label
            checked = execute([compiler, 'run', source, '--allocator', allocator, '--out', str(prefix) + '-run'], 'run')
            if expected is None:
                expected = checked.stdout
            assert checked.stdout == expected and checked.stdout.endswith('string storage views OK\n'), (checked, work)
            assert not checked.stderr, (checked, work)
            execute([compiler, 'aot', source, '--allocator', allocator, '--out', prefix], 'build')
            native = execute([prefix.with_suffix('.exe' if os.name == 'nt' else '')], 'aot')
            assert native.stdout == expected and not native.stderr, (native, work)
            llvm = prefix.with_suffix('.ll').read_text(encoding='utf-8')
            assert ('call i64 @chali_primitive_string_storage(' in llvm) == (mode == 'view'), (label, work)
            for name, offset, query in (
                ('negative-length', 8, '__stringStorageLength'),
                ('negative-data', 24, '__stringStorageData'),
                ('negative-capacity', 16, '__stringStorageCapacity'),
                ('short-length', 0, '__stringStorageLength'),
            ):
                if name == 'short-length':
                    initialize = 'int64 p=std::lang::__stringAllocate(8);'
                else:
                    initialize = ('int64 p=std::lang::__bootStringBuffer(0);'
                                  'int64 warm=std::lang::__stringStorageLength(p);')
                failure_source = work / (label + '-' + name + '.chl')
                failure_source.write_text('void main(){unsafe{' + initialize +
                    'int64 mut& field=(int64 mut&)(p+' + str(offset) + ');*field=0-1;'
                    'println("{}",std::lang::' + query + '(p));}}\n', encoding='utf-8')
                failure = execute([compiler, 'run', failure_source, '--allocator', allocator,
                                   '--out', work / (label + '-' + name)], name, success=False)
                assert '取字符串存储时读不到它' in failure.stderr and not failure.stdout, (failure, work)
    (work / 'results.json').write_text(json.dumps(dict(
        compiler_sha256=hashlib.sha256(compiler.read_bytes()).hexdigest(), results=records),
        ensure_ascii=False, indent=2), encoding='utf-8')
    print('String storage views: 6 configurations, legacy/short/mutable headers and 24 errors OK;', work)


if __name__ == '__main__':
    main()
