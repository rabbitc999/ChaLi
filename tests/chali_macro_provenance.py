"""验证编译期描述符的真实声明来源及折叠属性的只读边界。"""
import argparse
import json
import os
from pathlib import Path
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT=Path(__file__).resolve().parents[1]


def main():
    """从真实编译器查询原文件位置，并覆盖泛型与跨模块重定位。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=compiler_path())
    parser.add_argument('--case',action='append',default=[])
    args=parser.parse_args()
    cases=[
        ('field', '''struct Cell{
int64 value;
}
void main(){assert(($fields<Cell>())[0].sourceLine==2);assert(($fields<Cell>())[0].sourceColumn>0);assert(($fields<Cell>())[0].sourceFile==__SOURCE__);}''',None),
        ('payload', '''enum Choice{
Value(
int64 number,
String text
),None
};
void main(){assert(($variants<Choice>())[0].sourceLine==2);assert(($variants<Choice>())[0].payloads[0].sourceLine==3);assert(($variants<Choice>())[0].payloads[1].sourceLine==4);}''',None),
        ('methods', '''struct Cell{
int64 read(int16 value){return cast<int64>(value);}
int64 read(int32 value){return cast<int64>(value);}
}
void main(){assert(($methods<Cell>())[0].sourceLine==2);assert(($methods<Cell>())[1].sourceLine==3);assert(($methods<Cell>())[0].parameters[0].sourceLine==2);}''',None),
        ('inherited', '''interface Base{
int64 read(){return 7;}
}
struct Cell:Base{}
void main(){assert(($interfaces<Cell>())[0].sourceLine==1);assert(($methods<Cell>())[0].sourceLine==2);}''',None),
        ('associated', '''interface Source{
type Item;
Item get();
}
struct Cell:Source{type Item=int64;int64 get(){return 7;}}
void main(){assert(($interfaces<Cell>())[0].associatedTypes[0].sourceLine==2);}''',None),
        ('generic', '''struct Box<T>{
T value;
T read(){return self.value;}
}
void main(){assert(($fields<Box<int64>>())[0].sourceLine==2);assert(($methods<Box<int64>>())[0].sourceLine==3);assert(($fields<Box<String>>())[0].sourceLine==2);}''',None),
        ('cross_module', '''import data;
void main(){assert(($fields<data::Cell>())[0].sourceLine==3);assert(($fields<data::Cell>())[0].sourceFile==__DATA__);}''',None),
        ('generic_early', '''struct Box<T>{
T value;
T read(){return self.value;}
}
void main(){Box<int64> item;assert(($fields<Box<int64>>())[0].sourceLine==2);assert(($methods<Box<int64>>())[0].sourceLine==3);}''',None),
        ('generic_multiline', '''struct Box<
T
>{
T value;
T read(){return self.value;}
}
void main(){assert(($fields<Box<int64>>())[0].sourceLine==4);assert(($methods<Box<int64>>())[0].sourceLine==5);}''',None),
        ('generic_module', '''import data;
void main(){assert(($fields<data::Box<int16>>())[0].sourceFile==__DATA__);assert(($fields<data::Box<int16>>())[0].sourceLine==6);}''',None),
        ('readonly', 'struct Cell{int64 value;}void main(){($fields<Cell>())[0].sourceLine=2;}', ['赋值']),
        ('diagnostic', '''struct Cell{
int64 value;
}
$macro CHECK<T>() -> stmt
$for field in $fields<T>()
$compile_assert(false,"source failure");
$end_for
$end_macro
void main(){$CHECK<Cell>();}''',['声明位置','source failure',':2:']),
    ]
    if args.case:
        cases=[case for case in cases if case[0] in args.case]
        assert len(cases)==len(set(args.case))
    failures=[]
    with tempfile.TemporaryDirectory(prefix='macro-provenance-') as directory:
        work=Path(directory)
        data=work/'data.chl'
        data.write_text('module data;\nstruct Cell{\nint64 value;\n}\nstruct Box<T>{\nT value;\n}\n',encoding='utf-8')
        for name,source,diagnostics in cases:
            path=work/(name+'.chl')
            source=source.replace('__SOURCE__',json.dumps(path.as_posix())).replace('__DATA__',json.dumps(data.as_posix()))
            path.write_text(source,encoding='utf-8')
            result=subprocess.run([str(args.chali.resolve()),'check' if diagnostics else 'run',str(path),'--out',str(work/name)],cwd=ROOT,env=dict(os.environ,CHALI_LANG='zh'),capture_output=True,text=True,encoding='utf-8',timeout=240)
            valid=result.returncode!=0 and all(text in result.stderr for text in diagnostics) if diagnostics else (result.returncode,result.stdout,result.stderr)==(0,'','')
            if not valid:failures.append((name,result.returncode,result.stdout,result.stderr))
    assert not failures,failures
    print(f'元数据来源通过：{len(cases)} 项')


if __name__=='__main__':main()
