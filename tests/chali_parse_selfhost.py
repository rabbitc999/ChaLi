"""对照迁移前真实编译器，验证自举解析的数值、错误及原生产物行为。"""
from compiler_path import compiler_path
from pathlib import Path
import argparse
import hashlib
import json
import os
import subprocess
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
FIXTURE = ROOT / 'tests/fixtures/parse_compatibility.json'
CASES = [
    '', '0', '-0', '1', '-1', '42', '-42', 'true', 'false', 'True', 'FALSE',
    '+1', '-', '+', ' 1', '1 ', '\t1', '1\n', '01', '000', '-000',
    '9223372036854775807', '9223372036854775808', '-9223372036854775808',
    '-9223372036854775809', '18446744073709551615', '18446744073709551616',
    '00018446744073709551615', '00018446744073709551616',
    '999999999999999999999999999999999x', 'x9999999999999999999999999999999',
    '-999999999999999999999999999999999x', '999999999999999999999999999999999.0',
    '1x', '1.0', '-1.0', '.5', '-.5', '1e2', '1e309', '-1e309',
    '1e-9999', '1e309x', 'nan', 'NaN', 'nan(payload)', 'inf', '-infinity',
    '0x1.8p+2', '0x10', '0b10', '0xff', '12,5', '中文', '１２', '🙂',
    '  ', '\t', '\n', '0' * 160, '-' + '0' * 160,
    '0' * 160 + '42', '9' * 160, '9' * 160 + 'x', '0' * 160 + 'x',
]


def program():
    """同一输入同时核对四个公开入口，保留错误接口的真实文本。"""
    push = '\n'.join('inputs.push(' + json.dumps(s, ensure_ascii=False) + ');' for s in CASES)
    return '''import std::parse;
void main() {
List<String> inputs;
''' + push + '''
int64 index=0;
while(index<inputs.len()) {
    String text=inputs[index];
    match(std::parse::Parse::parseInt64(text)) {
        Ok(value) => {println("{}:i:ok:{}",index,value);}
        Err(error) => {println("{}:i:err:{}",index,error.toString());}
    }
    match(std::parse::Parse::parseUInt64(text)) {
        Ok(value) => {println("{}:u:ok:{}",index,value);}
        Err(error) => {println("{}:u:err:{}",index,error.toString());}
    }
    match(std::parse::Parse::parseFloat64(text)) {
        Ok(value) => {println("{}:f:ok:{:17F}",index,value);}
        Err(error) => {println("{}:f:err:{}",index,error.toString());}
    }
    match(std::parse::Parse::parseBool(text)) {
        Ok(value) => {println("{}:b:ok:{}",index,(int64)value);}
        Err(error) => {println("{}:b:err:{}",index,error.toString());}
    }
    index=index+1;
}
}
'''


def main():
    """冻结真实旧实现输出，或在四种原生模式下逐字节比较当前实现。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--record-baseline', action='store_true', help='仅用于显式冻结迁移前已知源码编译器')
    args = parser.parse_args()
    compiler = args.chali.resolve()
    env = dict(os.environ, CHALI_LANG='zh')
    with tempfile.TemporaryDirectory(prefix='parse-selfhost-') as directory:
        work = Path(directory)
        source = work / 'parse.chl'
        source.write_text(program(), encoding='utf-8')

        def invoke(*command):
            result = subprocess.run([str(x) for x in command], cwd=work, env=env,
                                    capture_output=True, encoding='utf-8', errors='replace', timeout=240)
            assert result.returncode == 0, (command, result.stdout, result.stderr)
            assert not result.stderr, result.stderr
            return result.stdout

        if args.record_baseline:
            compiler_digest = hashlib.sha256(compiler.read_bytes()).hexdigest()
            assert compiler_digest == '015936954c3c2d77f2fba47f9ffd57afab32d02dc47c55be5fefdb28ae4d1d68', '只能从已核对的 0ebdaa8d 编译器冻结兼容基线'
            output = invoke(compiler, 'run', source, '--out', work / 'baseline')
            assert len(output.splitlines()) == len(CASES) * 4
            FIXTURE.write_text(json.dumps(dict(source_commit='0ebdaa8d', compiler_sha256=compiler_digest,
                                              cases=CASES, output=output), ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
            print(f'baseline: {len(CASES)} inputs, {len(CASES)*4} public results frozen')
            return
        baseline = json.loads(FIXTURE.read_text(encoding='utf-8'))
        assert baseline['cases'] == CASES
        for allocator in ('malloc', 'pool'):
            for mode in ('checked', 'unchecked'):
                target = work / (allocator + '-' + mode + ('.exe' if os.name == 'nt' else ''))
                invoke(compiler, 'aot', source, '--allocator', allocator, '--' + mode, '--out', target)
                output = invoke(target)
                assert output == baseline['output'], (allocator, mode, output, baseline['output'])
                print(f'{allocator}/{mode}: {len(CASES)*4} parse compatibility results passed', flush=True)


if __name__ == '__main__':
    main()
