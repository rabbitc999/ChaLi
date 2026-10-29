"""else if 与 elif 共用语义的语句、值表达式和求值次数回归。"""
import argparse
import os
from pathlib import Path
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=compiler_path())
    compiler=parser.parse_args().chali.resolve()
    valid='''
int64 calls=0;
bool check(int64 input,int64 expected){calls=calls+1;return input==expected;}
int64 statement(int64 input){
    if(check(input,0)){return 10;}
    else if(check(input,1)){return 11;}
    elif(check(input,2)){return 12;}
    else /* between keywords */ if(check(input,3)){return 13;}
    else{return 14;}
}
String expression(int64 input){
    return if(input==0){"zero"} else if(input==1){"one"} elif(input==2){"two"} else{"other"};
}
void main(){
    calls=0;assert(statement(0)==10 && calls==1);
    calls=0;assert(statement(1)==11 && calls==2);
    calls=0;assert(statement(2)==12 && calls==3);
    calls=0;assert(statement(3)==13 && calls==4);
    calls=0;assert(statement(4)==14 && calls==4);
    assert(expression(0)=="zero" && expression(1)=="one" && expression(2)=="two" && expression(3)=="other");
    int64 value=0;
    if(true){if(false){value=1;}else if(true){value=2;}else{value=3;}}
    else if(true){value=4;}else{value=5;}
    assert(value==2);
}
'''
    invalid=('void main(){let value=if(false){1}else if(true){2};}',
             'void main(){if(false){}else if(true) return;else{}}')
    with tempfile.TemporaryDirectory(prefix='else-if-') as directory:
        for index,source in enumerate((valid,*invalid)):
            path=Path(directory)/f'case{index}.chl';path.write_text(source,encoding='utf-8')
            result=subprocess.run([str(compiler),'run' if index==0 else 'check',str(path),'out',str(Path(directory)/'program.exe')],
                cwd=ROOT,env=dict(os.environ,CHALI_LANG='zh',CHALI_NO_STDIN_INPUT='1'),
                capture_output=True,encoding='utf-8',errors='replace',timeout=240)
            if index==0:assert (result.returncode,result.stdout,result.stderr)==(0,'',''),result
            else:assert result.returncode!=0 and result.stderr,result
    print('else if: mixed chains, statement/expression results, nesting and short-circuit checks passed')


if __name__=='__main__':
    main()
