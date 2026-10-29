"""核对数值 List 融合追加的全标量类型、浮点位模式、容量、借用与回退。"""
from compiler_path import compiler_path
from project_temp import tempfile
from pathlib import Path
from itertools import product
import argparse
import hashlib
import json
import os
import re
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def fixture():
    """生成完整类型矩阵源码；只验证程序，不参与生产 LLVM 生成。"""
    source = '''#export("numeric_list_fill")
int64 numericFill(int64 seed){
    List<int64> values;int64 i=0;
    while(i<257){values.push(seed+i);i=i+1;}
    unsafe{int64 mut& raw=(int64 mut&)(int64)std::lang::address(&values);
        assert(std::lang::__listStorageCapacity(*raw)==512);}
    int64 result=0;i=0;while(i<257){result=result+values[i];i=i+1;}return result;
}
void main(){assert(numericFill(3)==33667);
'''
    cases = (
        ('int8', '(int8)-128'), ('int8', '(int8)127'), ('uint8', '(uint8)255'),
        ('int16', '(int16)-32768'), ('uint16', '(uint16)65535'),
        ('int32', '(int32)-2147483648'), ('uint32', '(uint32)4294967295u'),
        ('int64', '(int64)0x8000000000000000u'), ('int64', '9223372036854775807'),
        ('uint64', '0xffffffffffffffffu'), ('bool', 'true'), ('bool', 'false'),
        ('char', "'A'"), ('char', "'\\0'"), ('float32', '(float32)1.25'), ('float64', '-2.5'),
    )
    for kind, value in cases:
        source += f'''{{{kind} expected={value};List<{kind}> values;values.push(expected);
assert(values[0]==expected);
List<{kind}> copy=values;values.clear();values.push(expected);
assert(values[0]==expected && copy[0]==expected);
Map<{kind},{kind}> mapping;mapping[expected]=expected;assert(mapping[expected]==expected);
Set<{kind}> keys;keys.add(expected);assert(keys.has(expected)!=0);}}
'''
    for width, patterns in ((64, ('0u', '0x8000000000000000u', '0x7ff8000000000042u',
                                 '0xfff8000000000043u', '0x7ff0000000000000u', '1u')),
                            (32, ('0u', '0x80000000u', '0x7fc00042u', '0xffc00043u', '0x7f800000u', '1u'))):
        for pattern in patterns:
            source += f'''{{List<float{width}> values;uint64 bits={pattern};
values.push(std::lang::__bootFloat{width}FromBits(bits));
assert(std::lang::__bootFloat{width}Bits(values[0])==bits);
List<float{width}> copy=values;values.clear();
assert(std::lang::__bootFloat{width}Bits(copy[0])==bits);}}
'''
    source += '''int64 integerSource=17;
List<float32> converted32;converted32.push(integerSource);assert(converted32[0]==17.0);
List<float64> converted64;converted64.push(integerSource);assert(converted64[0]==17.0);
List<float64> opaqueSource;opaqueSource.push(1.25);List<int64> opaqueDestination;
unsafe{int64 mut& from=(int64 mut&)(int64)std::lang::address(&opaqueSource);
int64 mut& to=(int64 mut&)(int64)std::lang::address(&opaqueDestination);
std::lang::__bootListPushI(*to,std::lang::__listStorageReadI(*from,0,0,0),0,0);
assert(std::lang::__bootListFindI(*to,(int64)std::lang::__bootFloat64Bits(1.25),0,0)==0);}
List<int64> reserved;int64 handle=0;
unsafe{int64 mut& raw=(int64 mut&)(int64)std::lang::address(&reserved);handle=*raw;
std::lang::__bootListReserve(handle,33);}
int64 i=0;while(i<33){reserved.push(i);i=i+1;}
unsafe{assert(std::lang::__listStorageCapacity(handle)==33);}
reserved.push(33);unsafe{assert(std::lang::__listStorageCapacity(handle)==66);}
reserved.clear();reserved.push(7);unsafe{assert(std::lang::__listStorageCapacity(handle)==66);}
List<String> words;words.push("before");List<String> saved=words;
words.clear();words.push("after");assert(saved[0]=="before" && words[0]=="after");
println("numeric List append primitives OK");}
'''
    return source


def main():
    """核对两分配器、融合开关及标量 ABI 开关，不执行性能测试。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    work = Path(tempfile.mkdtemp(prefix='numeric-list-append-'))
    source = work / 'matrix.chl'
    source.write_text(fixture(), encoding='utf-8')
    records = []
    for allocator, fallback, old_abi in product(('malloc', 'pool'), (False, True), (False, True)):
        env = dict(os.environ, CHALI_LANG='zh')
        env.pop('CHALI_NO_AOT_NUMERIC_LIST_APPEND', None)
        env.pop('CHALI_NO_AOT_COLLECTION_SCALARS', None)
        if fallback:
            env['CHALI_NO_AOT_NUMERIC_LIST_APPEND'] = '1'
        if old_abi:
            env['CHALI_NO_AOT_COLLECTION_SCALARS'] = '1'
        label = allocator + ('-bootstrap' if fallback else '-primitive') + ('-old-abi' if old_abi else '-scalar')

        def execute(command, suffix, success=True):
            """记录状态及输出，失败保留可重现的源码和实际命令。"""
            command = list(map(str, command))
            result = subprocess.run(command, cwd=ROOT, env=env, capture_output=True,
                                    text=True, encoding='utf-8', timeout=600)
            records.append(dict(label=label + '-' + suffix, command=command,
                                code=result.returncode, stdout=result.stdout, stderr=result.stderr))
            (work / 'progress.json').write_text(json.dumps(records, ensure_ascii=False, indent=2), encoding='utf-8')
            assert (result.returncode == 0) == success, (records[-1], work)
            return result

        prefix = work / label
        checked = execute([compiler, 'run', source, '--allocator', allocator, '--out', str(prefix) + '-run'], 'checked')
        assert checked.stdout == 'numeric List append primitives OK\n' and not checked.stderr, (checked, work)
        execute([compiler, 'aot', source, '--allocator', allocator, '--out', prefix], 'build')
        native = execute([prefix.with_suffix('.exe' if os.name == 'nt' else '')], 'unchecked')
        assert native.stdout == checked.stdout and not native.stderr, (native, work)
        llvm = prefix.with_suffix('.ll').read_text(encoding='utf-8')
        bodies = re.findall(r'^define[^\n]*program__numericFill\(.*?^\}', llvm, re.M | re.S)
        assert len(bodies) == 1, (label, work)
        primitive = re.search(r'call i64 @chali_host_list_storage(?:_scalar)?\(i64(?: noundef)? 18,', bodies[0])
        assert bool(primitive) != fallback, (label, bodies[0], work)
        (work / (label + '-fill.ll')).write_text(bodies[0] + '\n', encoding='utf-8')
        errors = {
            'invalid': ('void main(){unsafe{std::lang::__bootListPushI(0,1,0,0);}}', 'List storage'),
            'lease': ('''void main(){List<int64> values;values.push(1);int64 handle=0;
unsafe{int64 mut& raw=(int64 mut&)(int64)std::lang::address(&values);handle=*raw;}
for(int64 value in values){unsafe{std::lang::__bootListPushI(handle,value,0,0);}}}''',
                      '活跃 for-in 引用期间不能移动或删除列表元素'),
        }
        for name, (content, diagnostic) in errors.items():
            case = work / (name + '.chl')
            case.write_text(content, encoding='utf-8')
            failed = execute([compiler, 'run', case, '--allocator', allocator,
                              '--out', str(prefix) + '-' + name], name, success=False)
            assert diagnostic in failed.stdout + failed.stderr, (label, name, failed, work)
    (work / 'results.json').write_text(json.dumps(dict(
        compiler_sha256=hashlib.sha256(compiler.read_bytes()).hexdigest(), results=records),
        ensure_ascii=False, indent=2), encoding='utf-8')
    print('numeric List append: 8 modes, all primitive types, Map/Set matrix, float bits, capacity and leases OK;', work)


if __name__ == '__main__':
    main()
