"""核对显式宏参数类别、求值次数、语法隔离和引用行为。"""
import argparse
import os
from pathlib import Path
import subprocess

from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    """使用真实编译器运行正例，并核对类别错配和名称角色错误。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--raw', action='store_true')
    options = parser.parse_args()
    compiler = options.chali.resolve()
    definitions = '''
int64 count=0;
int64 next(){count+=1;return count;}
int64 trace=0;
int64 first(){trace=trace*10+1;return 3;}
int64 second(){trace=trace*10+2;return 4;}
void verify(bool condition){if(!condition){int64 zero=0;let failure=1/zero;}}
$macro VALUE(x:value)
x+x
$end_macro
$macro EXPR(x:expr)
x+x
$end_macro
$macro IGNORE_VALUE(x:value)
0
$end_macro
$macro IGNORE_EXPR(x:expr)
0
$end_macro
$macro COPY(x:value)
{x+=1;yield x;}
$end_macro
$macro UPDATE(mut &x:value)
x+=1;
$end_macro
$macro DECLARE(name:ident, value:value)
int64 name=value;
$end_macro
$macro ZERO(T:type)
cast<T>(0)
$end_macro
$macro STATEMENT(body:stmt)
body;
$end_macro
$macro BLOCK(body:block)
body;
$end_macro
$replace INLINE(x:expr) x+x
$macro FORWARD(x:expr)
$EXPR(x)
$end_macro
$macro FORWARD_VALUE(x:value)
$EXPR(x)
$end_macro
$macro FORWARD_TO_VALUE(x:expr)
$VALUE(x)
$end_macro
$macro TYPED<T:#res,U:b>(a:value,b:value)
{T left=cast<T>(a);U right=b;yield left+cast<T>(right);}
$end_macro
$macro MIXED(a:value,b:expr)
a+b+b
$end_macro
$macro MIXED_REVERSE(a:expr,b:value)
a+b
$end_macro
$macro SYNTAX_BUMP(x:expr)
x+=1;
$end_macro
enum Choice {Value(int64 item), Empty}
$macro TAKE(T:type,item:value)
match(item){T::Value(value)=>{value} T::Empty=>{0}}
$end_macro
'''
    valid = definitions + '''
void main(){
    count=0;int64 once=$VALUE(next());verify(once==2 && count==1);
    count=0;int64 twice=$EXPR(next());verify(twice==3 && count==2);
    count=0;int64 unused=$IGNORE_VALUE(next());verify(unused==0 && count==1);
    count=0;unused=$IGNORE_EXPR(next());verify(unused==0 && count==0);
    int64 original=10;int64 copied=$COPY(original);verify(original==10 && copied==11);
    $UPDATE(original);verify(original==11);
    $DECLARE(created,42);verify(created==42);
    int32 zero=$ZERO(int32);verify(zero==0);
    $STATEMENT(original+=1;);verify(original==12);
    $BLOCK({original+=1;});verify(original==13);
    count=0;int64 inlineValue=$INLINE(next());verify(inlineValue==3 && count==2);
    count=0;int64 forwarded=$FORWARD(next());verify(forwarded==3 && count==2);
    count=0;forwarded=$FORWARD_VALUE(next());verify(forwarded==2 && count==1);
    count=0;forwarded=$FORWARD_TO_VALUE(next());verify(forwarded==2 && count==1);
    int32 typed=$TYPED((int64)1,(int16)2);verify(typed==3);
    count=0;int64 mixed=$MIXED(next(),next());verify(mixed==6 && count==3);
    trace=0;int64 ordered=$MIXED_REVERSE(first(),second());verify(ordered==7 && trace==21);
    $SYNTAX_BUMP(original);verify(original==14);
    int64 payload=$TAKE(Choice,Choice::Value(42));verify(payload==42);
}
'''
    cases = [('valid', valid, None)]
    if not options.raw:
        cases.append(('generic_interface_types', definitions + '''
struct Holder<T>{T item;}
interface Read {int64 get();}
struct Reader:Read {int64 get(){return 42;}}
$macro CONSTRUCT(T:type)
{T result;yield result;}
$end_macro
$macro CONVERT(T:type,value:value)
{T result=value;yield result;}
$end_macro
void main(){
    Holder<int64> holder=$CONSTRUCT(Holder<int64>);holder.item=42;verify(holder.item==42);
    Reader reader;Read result=$CONVERT(Read,reader);verify(result.get()==42);
}
''', None))
        cases.append(('module_types', '''import left;
import right;
$macro CONSTRUCT(T:type)
{T result;yield result;}
$end_macro
void main(){
    left::Holder<int64> a=$CONSTRUCT(left::Holder<int64>);
    right::Holder<int64> b=$CONSTRUCT(right::Holder<int64>);
    a.item=11;b.item=22;assert(a.item==11 && b.item==22);
}
''', None))
    for name, call in [('ident', '$DECLARE(1+2,42);'), ('type', 'int64 value=$ZERO(1+2);'),
                       ('value', 'int64 value=$VALUE(int64);'), ('expr', 'int64 value=$EXPR(int64);'),
                       ('block', '$BLOCK(1+2);'), ('stmt', '$STATEMENT(int64);')]:
        cases.append((name, definitions + 'void main(){' + call + '}', '形参类别不匹配'))
    cases += [
        ('unknown', '$macro BAD(x:mystery)\nx\n$end_macro\nvoid main(){}', '不支持的宏形参类别'),
        ('reference', '$macro BAD(&x:expr)\nx\n$end_macro\nvoid main(){}', '引用参数只能标注 value'),
        ('readonly', '$macro BAD(&x:value)\nx+=1;\n$end_macro\nvoid main(){int64 x=0;$BAD(x);}', '只读'),
        ('name_value', '$macro BAD(x:value)\nint64 x=1;\n$end_macro\nvoid main(){$BAD(created);}', '标识符'),
        ('name_expr', '$macro BAD(x:expr)\nint64 x=1;\n$end_macro\nvoid main(){$BAD(created);}', '标识符'),
    ]
    with tempfile.TemporaryDirectory(prefix='macro-categories-') as folder:
        for module in ('left', 'right'):
            (Path(folder) / (module + '.chl')).write_text('module ' + module + ';\nstruct Holder<T>{T item;}\n', encoding='utf-8')
        for name, source, diagnostic in cases:
            path = Path(folder) / (name + '.chl')
            path.write_text(source, encoding='utf-8')
            result = subprocess.run([str(compiler), 'check' if diagnostic else 'run', str(path),
                                     *(['--raw'] if options.raw else []), '--out', str(Path(folder) / name)],
                                    cwd=ROOT, env=dict(os.environ, CHALI_LANG='zh'), capture_output=True,
                                    encoding='utf-8', timeout=180)
            if diagnostic:
                assert result.returncode != 0 and diagnostic in result.stderr, (name, result)
            else:
                assert (result.returncode, result.stdout, result.stderr) == (0, '', ''), (name, result)
    print(f'宏显式参数类别通过：{len(cases)} 项，raw={options.raw}')


if __name__ == '__main__':
    main()
