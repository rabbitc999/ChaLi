"""验证显式 #res 结果来源与实参来源混合推导，推导不执行实参。"""
from pathlib import Path
import argparse
import os
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
DEFINITION = '''
$macro Example<T:#res, U:b>(a, b)
{
    T left = cast<T>(a);
    U right = b;
    yield left + cast<T>(right);
}
$end_macro
'''


def main():
    """运行期核对实际求值次数，并检查缺少结果上下文及错误来源的诊断。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    valid = DEFINITION + '''
int64 calls = 0;
int64 nextA(){calls=calls*10+1;return 3;}
int16 nextB(){calls=calls*10+2;return (int16)4;}
int32 returned(){return $Example((int64)5,(int16)6);}
int32 consume(int32 value){return value;}
interface Value { int64 get(); }
struct Box: Value { int64 get(){return 42;} }
$macro Wrap<T:#res, U:b>(a,b)
{
    T result=b;
    yield result;
}
$end_macro
T genericReturn<T>(int64 a,int16 b){return $Example(a,b);}
$replace Inline<T:#res, U:b>(a,b) cast<T>(a)+cast<T>(b)
$macro Forward<T:#res, U:b>(a,b)
    $Example<T,U>(a,b)
$end_macro
$macro Nested(a,b)
    $Example(a,b)
$end_macro
void main(){
    int32 initialized=$Example(nextA(),nextB());
    assert(initialized==7 && calls==12);
    int32 assigned=0;assigned=$Example((int64)7,(int16)8);
    assert(assigned==15 && returned()==11);
    assert(consume($Example((int64)9,(int16)10))==19);
    int32 nested=$Forward((int64)11,(int16)12);assert(nested==23);
    int32 nestedExpected=$Nested((int64)11,(int16)12);assert(nestedExpected==23);
    let explicitValue=$Example<int32,int16>((int64)13,(int16)14);
    assert(explicitValue==27);
    float64 real=$Example(1.25,2.5);assert(real==3.75);
    float64 explicitWins=$Example<int32,float64>(1.75,2.5);assert(explicitWins==3.0);
    int32 inlineResult=$Inline((int64)3,(int16)4);assert(inlineResult==7);
    assert(genericReturn<int32>(3,(int16)4)==7);
    Box box;Value owned=$Wrap(0,box);assert(owned.get()==42);
}
'''
    rejected = [
        (DEFINITION + 'void main(){let value=$Example(1,2);}', '缺少静态类型约束'),
        (DEFINITION + 'void main(){int32 value=$Example<int32>(1,2);}', '类型实参数量'),
        ('$macro Bad<T:missing>(a)\ncast<T>(a)\n$end_macro\nvoid main(){int64 x=$Bad(1);}', '推导来源不是形参'),
        ('''interface Expected { int64 get(); }
struct Unrelated { int64 value; }
$macro Wrap<T:#res,U:b>(a,b)
{ T result=b; yield result; }
$end_macro
void main(){Unrelated source;Expected result=$Wrap(0,source);}
''', '类型未实现目标接口'),
    ]
    with tempfile.TemporaryDirectory(prefix='macro-result-inference-') as directory:
        work = Path(directory)
        for index, (text, diagnostic) in enumerate([(valid, None), *rejected]):
            source = work / f'case{index}.chl'
            source.write_text(text, encoding='utf-8')
            result = subprocess.run([str(compiler), 'run' if diagnostic is None else 'check', str(source),
                                     '--out', str(work / f'case{index}')], cwd=ROOT,
                                    env=dict(os.environ, CHALI_LANG='zh'), capture_output=True,
                                    encoding='utf-8', timeout=240)
            if diagnostic is None:
                assert (result.returncode, result.stdout, result.stderr) == (0, '', ''), result
            else:
                assert result.returncode != 0 and diagnostic in result.stderr, result
    print('#res and argument inference: initialization, assignment, return, argument, nesting, explicit types and single evaluation passed')


if __name__ == '__main__':
    main()
