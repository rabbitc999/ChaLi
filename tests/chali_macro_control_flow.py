"""验证宏模板和调用方片段的控制出口、函数边界及清理。"""
import argparse
import os
from pathlib import Path
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT=Path(__file__).resolve().parents[1]


def main():
    """实际执行有限循环，核对来源归属并拒绝越界控制片段。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=compiler_path())
    parser.add_argument('--case',action='append',default=[])
    options=parser.parse_args()
    loop='''$macro WRAP(body:block) -> stmt
int64 local=0;
while(local<1){local+=1;body;}
$end_macro
'''
    cases=[
        ('caller_break',loop+'''void main(){int64 value=0;loop{value+=1;$WRAP({break;});value+=100;break;}assert(value==1);}''',None),
        ('caller_continue',loop+'''void main(){int64 turns=0;int64 after=0;while(turns<3){turns+=1;$WRAP({continue;});after+=1;}assert(turns==3&&after==0);}''',None),
        ('caller_return',loop+'''int64 read(){$WRAP({return 42;});return 0;}void main(){assert(read()==42);}''',None),
        ('caller_yield',loop+'''void main(){int64 value=loop{$WRAP({yield 42;});yield 0;};assert(value==42);}''',None),
        ('local_loop',loop+'''void main(){int64 value=0;$WRAP({loop{value+=1;break;}value+=10;});assert(value==11);}''',None),
        ('template_break','''$macro STOP() -> stmt
break;
$end_macro
void main(){int64 value=0;loop{value+=1;$STOP();value+=100;}assert(value==1);}''',None),
        ('template_continue','''$macro NEXT() -> stmt
continue;
$end_macro
void main(){int64 value=0;int64 after=0;while(value<3){value+=1;$NEXT();after+=1;}assert(after==0);}''',None),
        ('template_return','''$macro DONE() -> stmt
return 42;
$end_macro
int64 read(){$DONE();}void main(){assert(read()==42);}''',None),
        ('expression_return','''$macro BAD() -> expr
return 7;
1
$end_macro
int64 read(){int64 value=$BAD();return value;}void main(){assert(read()==1);}''',['expr 宏模板']),
        ('expression_value','''$macro VALUE(x:value) -> expr
int64 local=x+1;
local+1
$end_macro
void main(){assert($VALUE(40)==42);}''',None),
        ('nested',loop+'''$macro OUT(body:block) -> stmt
$WRAP(body);
$end_macro
void main(){int64 value=0;loop{value+=1;$OUT({break;});value+=100;break;}assert(value==1);}''',None),
        ('closure_local_return','''$macro MAKE() -> expr
fn(){return 42;}
$end_macro
void main(){let f=$MAKE();assert(f()==42);}''',None),
        ('generic_closure_return','''$macro MAKE() -> expr
fn<T>(T value){return value;}
$end_macro
void main(){let f=$MAKE();assert(f(42)==42);}''',None),
        ('cross_function_return','''$macro MAKE(body:block) -> expr
fn(){body;return 0;}
$end_macro
void main(){let f=$MAKE({return 42;});f();}''',['函数边界']),
        ('argument_closure',loop+'''void main(){int64 value=0;$WRAP({let f=fn(){return 42;};value=f();});assert(value==42);}''',None),
        ('expression_caller_yield','''$macro EXPR(body:block) -> expr
int64 local=3;
body;
local
$end_macro
void main(){int64 value=loop{int64 ignored=$EXPR({yield 42;});yield 0;};assert(value==42);}''',None),
        ('continue_cleanup',loop+'''int64 cleaned=0;void cleanup(){cleaned+=1;}void main(){int64 turns=0;while(turns<3){defer cleanup();turns+=1;$WRAP({continue;});}assert(cleaned==3);}''',None),
        ('break_cleanup',loop+'''int64 cleaned=0;void cleanup(){cleaned+=1;}void main(){loop{defer cleanup();$WRAP({break;});}assert(cleaned==1);}''',None),
        ('return_cleanup',loop+'''int64 cleaned=0;void cleanup(){cleaned+=1;}int64 read(){defer cleanup();$WRAP({return 42;});return 0;}void main(){assert(read()==42&&cleaned==1);}''',None),
        ('caller_named',loop+'''void main(){int64 value=0;outer:loop{loop{value+=1;$WRAP({break outer;});value+=100;break;}value+=1000;break;}assert(value==1);}''',None),
        ('borrow_reachability','''$macro WRAP(body:block) -> stmt
loop{body;break;};
$end_macro
void main(){List<int64> values;loop{$WRAP({break;});List<int64> moved=__move(values);}values.push(7);assert(values[0]==7);}''',None),
    ]
    propagation='''struct Failure:Error{String toString(){return "failure";}}
Result<int64,Error> leaf(bool fail){if(fail){Failure error;return Result<int64,Error>::Err(error);}return Result<int64,Error>::Ok(41);}
'''
    verify='void main(){assert(outer(false).unwrap()==42);assert(outer(true).isErr());}'
    cases.extend([
        ('caller_propagation',loop+propagation+'Result<int64,Error> outer(bool fail){$WRAP({let value=leaf(fail)?;return Result<int64,Error>::Ok(value+1);});return Result<int64,Error>::Ok(0);}'+verify,None),
        ('template_propagation',propagation+'''$macro GET(value:expr,out:ident) -> stmt
let out=value?;
$end_macro
Result<int64,Error> outer(bool fail){$GET(leaf(fail),value);return Result<int64,Error>::Ok(value+1);}
'''+verify,None),
        ('expression_propagation',propagation+'''$macro GET(value:expr) -> expr
value?
$end_macro
Result<int64,Error> outer(bool fail){let value=$GET(leaf(fail));return Result<int64,Error>::Ok(value+1);}
'''+verify,['expr 宏模板']),
        ('expression_caller_propagation',propagation+'''$macro ID(value:expr) -> expr
value
$end_macro
Result<int64,Error> outer(bool fail){let value=$ID(leaf(fail)?);return Result<int64,Error>::Ok(value+1);}
'''+verify,None),
        ('option_propagation',loop+'''Option<int64> leaf(bool none){if(none){return Option<int64>::None;}return Option<int64>::Value(41);}
Option<int64> outer(bool none){$WRAP({let value=leaf(none)?;return Option<int64>::Value(value+1);});return Option<int64>::None;}
void main(){assert(outer(false).unwrap()==42);assert(outer(true).isNone());}''',None),
        ('generic_nested',loop+'''$macro OUT<T>(body:block) -> stmt
$WRAP(body);
$end_macro
void main(){int64 turns=0;int64 after=0;while(turns<3){turns+=1;$OUT<int64>({continue;});after+=1;}assert(after==0);}''',None),
    ])
    owned='''int64 drops=0;
struct Item:Drop{int64 score;void drop(){drops+=1;}}
$macro OWN(item:value,body:block) -> stmt
item.score+=1;
body;
$end_macro
'''
    cases.extend([
        ('owned_break',owned+'void main(){Item item;item.score=7;loop{$OWN(item,{break;});}assert(item.score==7&&drops==1);}',None),
        ('owned_continue',owned+'void main(){Item item;item.score=7;int64 turns=0;while(turns<3){turns+=1;$OWN(item,{continue;});}assert(item.score==7&&drops==3);}',None),
        ('owned_return',owned+'void run(){Item item;item.score=7;$OWN(item,{return;});}void main(){run();assert(drops==2);}',None),
        ('caller_named_while',loop+'void main(){int64 value=0;outer:while(true){loop{value+=1;$WRAP({break outer;});value+=100;break;}value+=1000;break;}assert(value==1);}',None),
        ('expression_local_named','''$macro VALUE() -> expr
local: loop{break local;}
42
$end_macro
void main(){int64 value=0;loop{value=$VALUE();value+=1;break;}assert(value==43);}''',None),
        ('return_expression_boundary','''$macro DONE() -> stmt
return 42;
$end_macro
int64 read(){return {$DONE();yield 0;};}void main(){assert(read()==42);}''',['禁止嵌套 return']),
        ('return_expression_label','''$macro EXIT() -> stmt
break local;
$end_macro
int64 read(){return {local: loop{$EXIT();}yield 0;};}void main(){read();}''',['禁止按循环标签退出']),
        ('return_expression_local_function','''$macro FUNCTION() -> expr
fn(){return 42;}
$end_macro
int64 read(){return {let f=$FUNCTION();yield f();};}void main(){assert(read()==42);}''',None),
    ])
    cases.extend([
        ('return_expression_propagation',propagation+'''$macro GET(value:expr,out:ident) -> stmt
let out=value?;
$end_macro
Result<int64,Error> outer(bool fail){return {$GET(leaf(fail),value);yield Result<int64,Error>::Ok(value+1);};}
'''+verify,['return 表达式内部禁止使用']),
        ('return_cached_caller_propagation',propagation+'''$macro ID(value:value) -> expr
value
$end_macro
Result<int64,Error> outer(bool fail){return Result<int64,Error>::Ok($ID(leaf(fail)?)+1);}
'''+verify,None),
    ])
    if options.case:
        cases=[item for item in cases if item[0] in options.case]
        assert len(cases)==len(set(options.case)),options.case
    failures=[]
    with tempfile.TemporaryDirectory(prefix='macro-control-flow-') as directory:
        work=Path(directory)
        for name,source,diagnostic in cases:
            path=work/(name+'.chl')
            path.write_text(source,encoding='utf-8')
            result=subprocess.run([str(options.chali.resolve()),'check' if diagnostic else 'run',str(path),'--out',str(work/name)],cwd=ROOT,env=dict(os.environ,CHALI_LANG='zh'),capture_output=True,text=True,encoding='utf-8',timeout=180)
            valid=result.returncode!=0 and all(part in result.stderr for part in diagnostic) if diagnostic else (result.returncode,result.stdout,result.stderr)==(0,'','')
            if not valid: failures.append((name,result.returncode,result.stdout,result.stderr))
    assert not failures,failures
    print(f'宏控制流来源归属通过：{len(cases)} 项')


if __name__=='__main__':
    main()
