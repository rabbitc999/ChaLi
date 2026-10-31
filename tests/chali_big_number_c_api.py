"""验证大数 C 入口的数值、拥有值、输入及四种 AOT 配置。"""
from compiler_path import compiler_path
from project_temp import tempfile
from pathlib import Path
import argparse
import os
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def main():
    """以既有期望输出和显式断言覆盖数值操作，不进行性能计时。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    args = parser.parse_args()
    compiler = args.chali.resolve()
    with tempfile.TemporaryDirectory(prefix='big-number-c-api-') as directory:
        work = Path(directory)
        cases = []
        for sample in ('bigint', 'bigfloat', 'bigint_methods', 'opcode_big_numbers', 'std_numerics_number_theory'):
            cases.append((sample, (ROOT / 'example/samples' / (sample + '.chl')).read_text(encoding='utf-8'),
                          (ROOT / 'example/expected' / (sample + '.txt')).read_text(encoding='utf-8'),
                          (ROOT / 'example/expected' / (sample + '.in')).read_text(encoding='utf-8')
                          if (ROOT / 'example/expected' / (sample + '.in')).exists() else ''))
        cases.append(('input', 'void main(){BigInteger a=inputBigInteger();BigDecimal b=inputBigDecimal();'
                      'println("{} {}",a,b);}', '12345678901234567890 1.25\n', '12345678901234567890 1.25\n'))
        for allocator in ('malloc', 'pool'):
            for checks in ('checked', 'unchecked'):
                for name, code, expected, input_text in cases:
                    source = work / (name + '.chl')
                    source.write_text(code, encoding='utf-8')
                    command = [str(compiler), 'run', str(source), '--allocator', allocator,
                               '--' + checks, '--out', str(work / (name + allocator + checks))]
                    result = subprocess.run(command, cwd=ROOT, input=input_text, capture_output=True,
                                            encoding='utf-8', errors='replace', timeout=120,
                                            env=dict(os.environ, CHALI_LANG='zh'))
                    assert (result.returncode, result.stdout, result.stderr) == (0, expected, ''), (name, allocator, checks, result)
                print(allocator + '/' + checks + ': six numeric and input cases passed', flush=True)
        for name, code, input_text, diagnostic in (
            ('missing', 'void main(){inputBigInteger();}', '', '输入不够'),
            ('bad_integer', 'void main(){inputBigInteger();}', 'invalid', '输入项不是合法的大整数'),
            ('bad_decimal', 'void main(){inputBigDecimal();}', 'invalid', '输入项不是合法的大浮点数'),
            ('missing_decimal', 'void main(){inputBigDecimal();}', '', '输入不够，取不到大浮点数')):
            source=work/(name+'.chl');source.write_text(code,encoding='utf-8')
            result=subprocess.run([str(compiler),'run',str(source),'--out',str(work/name)],cwd=ROOT,
                                  input=input_text,capture_output=True,encoding='utf-8',timeout=120,
                                  env=dict(os.environ,CHALI_LANG='zh'))
            assert result.returncode and diagnostic in result.stderr,(name,result)
        print('Four input failure diagnostics passed')


if __name__ == '__main__':
    main()
