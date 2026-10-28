"""验证 String 容量复用、扩容释放、值语义及只读／借用限制。"""
from compiler_path import compiler_path
from project_temp import tempfile
from pathlib import Path
import argparse
import json
import os
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def main():
    """在两种分配器与 LLVM 检查模式下保存正例、运行期错误和编译拒绝。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    args = parser.parse_args()
    compiler = args.chali.resolve()
    work = Path(tempfile.mkdtemp(prefix='string-buffer-'))
    records = []

    def execute(command, label, env):
        """记录原始命令、状态及输出，不吞掉编译或运行错误。"""
        result = subprocess.run(list(map(str, command)), cwd=ROOT, env=env,
                                capture_output=True, text=True, encoding='utf-8', timeout=600)
        records.append(dict(label=label, command=list(map(str, command)), code=result.returncode,
                            stdout=result.stdout, stderr=result.stderr))
        (work / 'results.json').write_text(json.dumps(records, ensure_ascii=False, indent=2), encoding='utf-8')
        return result

    environment = dict(os.environ)
    environment['CHALI_LANG'] = 'zh'
    for allocator in ('malloc', 'pool'):
        for checked in ('--checked', '--unchecked'):
            for fallback in (False, True):
                env = dict(environment)
                if fallback:
                    env['CHALI_NO_AOT_RAW_INTRINSICS'] = '1'
                label = allocator + checked + ('-fallback' if fallback else '-direct')
                prefix = work / label
                built = execute([compiler, 'aot', ROOT / 'tests/fixtures/aot_string_buffer.chl',
                                 '--allocator', allocator, checked, '--out', prefix], label + '-build', env)
                assert built.returncode == 0, (label, built.stdout, built.stderr, work)
                ran = execute([prefix.with_suffix('.exe' if os.name == 'nt' else '')], label, env)
                assert ran.returncode == 0 and 'String capacity, copy, UTF-8 and Drop OK' in ran.stdout, (label, ran, work)
    for name, body, diagnostic in (
        ('negative-reserve', 'String value;value.reserve(-1);', '容量不能为负数'),
        ('negative-truncate', 'String value;value.truncate(-1);', '截短长度不能为负数'),
        ('over-limit', 'String value;value.reserve(value.maxCapacity()+1);', '容量超出上限'),
        ('integer-limit', 'String value;value.reserve(9223372036854775807);', '容量超出上限'),
    ):
        source = work / (name + '.chl')
        source.write_text('void main(){' + body + '}\n', encoding='utf-8')
        prefix = work / name
        built = execute([compiler, 'aot', source, '--out', prefix], name + '-build', environment)
        assert built.returncode == 0, (name, built.stdout, built.stderr, work)
        ran = execute([prefix.with_suffix('.exe' if os.name == 'nt' else '')], name, environment)
        assert ran.returncode != 0 and diagnostic in ran.stdout + ran.stderr, (name, ran, work)
    for allocator in ('malloc', 'pool'):
        source = work / ('stale-' + allocator + '.chl')
        source.write_text('void main(){String value="abcdefghijklmnopqrstuvwxyz0123456789";unsafe{'
                          'int64 mut& slot=(int64 mut&)(int64)std::lang::address(&value);'
                          'int64 old=std::lang::__bootStringOwnedData(*slot);value.reserve(100);'
                          'int64 byte=std::lang::__bootStringByte(old,0);println("{}",byte);}}\n', encoding='utf-8')
        prefix = work / ('stale-' + allocator)
        built = execute([compiler, 'aot', source, '--allocator', allocator, '--out', prefix], allocator + '-stale-build', environment)
        assert built.returncode == 0, (built.stdout, built.stderr, work)
        ran = execute([prefix.with_suffix('.exe' if os.name == 'nt' else '')], allocator + '-stale', environment)
        assert ran.returncode != 0 and any(message in ran.stdout + ran.stderr for message in ('未分配或已释放', 'unallocated memory')), (ran, work)
    for name, source, diagnostic in (
        ('const', 'void main(){const String value="abc";value.append("!");}', '不可变变量'),
        ('readonly', 'void bad(String const& value){value.clear();}void main(){String value="abc";bad(&value);}', '只读'),
        ('literal', 'void main(){"abc".reserve(100);}', '字面量只读'),
        ('const-owner', 'struct Box{String text;void reset(){self.text.clear();}};void main(){Box original;const Box box=original;box.reset();}', '只读接收者'),
        ('active-byte', 'void main(){String value="abc";char const& byte=&value[0];value.reserve(100);println("{}",*byte);}', '借用'),
        ('global-byte', 'String value="abc";char const& byte=&value[0];void main(){value.clear();println("{}",*byte);}', '借用'),
    ):
        path = work / (name + '.chl')
        path.write_text(source + '\n', encoding='utf-8')
        checked = execute([compiler, 'check', path], name, environment)
        assert checked.returncode != 0 and diagnostic in checked.stderr + checked.stdout, (name, checked, work)
    print('String buffers: 8 configurations, 6 runtime errors and 6 rejected mutations OK; evidence:', work)


if __name__ == '__main__':
    main()
