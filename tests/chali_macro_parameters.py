"""卫生宏值参数单次求值、副本修改与显式引用绑定回归。"""
import argparse
import os
from pathlib import Path
import subprocess
import unittest
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
COMPILER = compiler_path()


class MacroParameterTests(unittest.TestCase):
    maxDiff = None
    def program(self, source, expected_error=None):
        with tempfile.TemporaryDirectory(prefix='macro-parameters-') as directory:
            path = Path(directory) / 'main.chl'
            path.write_text(source, encoding='utf-8')
            result = subprocess.run([str(COMPILER), 'run', str(path), 'out', str(Path(directory)/'program.exe')],
                cwd=ROOT, env=dict(os.environ, CHALI_LANG='zh', CHALI_NO_STDIN_INPUT='1'),
                capture_output=True, encoding='utf-8', errors='replace', timeout=240)
            if expected_error:
                self.assertNotEqual(result.returncode, 0, result.stdout+result.stderr)
                self.assertIn(expected_error, result.stderr)
            else:
                self.assertEqual((result.returncode,result.stdout,result.stderr),(0,'',''))

    def test_value_and_reference_parameters(self):
        self.program('''
$macro Twice(n)
n+n
$end_macro
$macro Pair(a,b)
a*100+b*10+a+b
$end_macro
$macro Bump(n)
n=n+1;
$end_macro
$macro RefBump(mut &n)
n=n+1;
$end_macro
$macro Read(&n)
n+n
$end_macro
$macro Nested(n)
$Twice($Twice(n))
$end_macro
$macro Ignore(n)
42
$end_macro
int64 count=0;
int64 next(){count=count+1;return count;}
void main(){
    std::lang::assert($Twice(next())==2 && count==1,"single evaluation",1);
    count=0;std::lang::assert($Pair(next(),next())==123 && count==2,"argument order",1);
    count=0;std::lang::assert($Nested(next())==4 && count==1,"nested evaluation",1);
    count=0;std::lang::assert($Ignore(next())==42 && count==1,"unused argument",1);
    int64 x=7;$Bump(x);std::lang::assert(x==7,"value copy",1);
    $RefBump(x);std::lang::assert(x==8,"mutable reference",1);
    std::lang::assert($Read(x)==16,"readonly reference",1);x=9;assert(x==9);
}
''')

    def test_readonly_reference_rejects_mutation(self):
        self.program('''$macro Change(&n)
n=n+1;
$end_macro
void main(){int64 x=1;$Change(x);}
''', '只读')

    def test_mutable_reference_rejects_const(self):
        self.program('''$macro Change(mut &n)
n=n+1;
$end_macro
void main(){const int64 x=1;$Change(x);}
''', '只读')

    def test_owned_parameter_is_independent(self):
        self.program('''$macro Edit(value)
value.push("macro");
$end_macro
$macro RefEdit(mut &value)
value.push("reference");
$end_macro
void main(){List<String> values;values.push("caller");
$Edit(values);assert(values.len()==1);
$RefEdit(values);assert(values.len()==2 && values[1]=="reference");}
''')


if __name__ == '__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=COMPILER)
    arguments,remaining=parser.parse_known_args()
    COMPILER=arguments.chali.resolve()
    unittest.main(argv=[__file__,*remaining],verbosity=2)
