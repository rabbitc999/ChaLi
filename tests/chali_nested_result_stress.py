"""嵌套 Result、连续传播、错误类型匹配与资源清理压力回归。"""
from pathlib import Path
import argparse
import os
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT=Path(__file__).resolve().parents[1]
FIXTURES=ROOT/'example/samples'


def main():
    """核对用户示例及 checked／unchecked 各一万次多层传播。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=compiler_path())
    compiler=parser.parse_args().chali.resolve()
    env=dict(os.environ,CHALI_LANG='zh',CHALI_QUIET='1')
    with tempfile.TemporaryDirectory(prefix='nested-result-stress-') as directory:
        work=Path(directory)
        for name,expected in [('borrow_loop_heap_traversal','heap traversal: while,for,loop types=int64,float64,String closed=384\n'),('enum_payload_ownership','enum payload ownership: closed=5120 deferred=5120 cross-module generic borrow\n'),('enum_payload_wrapping','enum payload wrapping: assignments=2048 once=1 generic,custom,Option,Result\n'),('generic_type_behavior','generic type behavior: iterations=512 nested=8\n'),('generic_nested_inference','nested generic inference: String,int64\n'),('generic_borrow_control_flow','generic flow: iterations=2048 types=int64,float64,String cleanups=4096\n'),('nested_result','Ok=type1\nErr=type2\nErr=type3\nErr=type3\nErr=type3\n'),('nested_result_stress','nested Result: calls=10000 closed=20000 deferred=10000\n'),('nested_result_string_conversion','implicit String conversion: Ok(Err(inner))\n')]:
            for checked in (True,False):
                target=work/(name+('-checked' if checked else '-unchecked'))
                result=subprocess.run([str(compiler),'build',str(FIXTURES/(name+'.chl')),'--checked' if checked else '--unchecked','--out',str(target)],cwd=ROOT,env=env,capture_output=True,text=True,encoding='utf-8',timeout=240)
                assert result.returncode==0,result
                executable=target.with_suffix('.exe') if os.name=='nt' else target
                result=subprocess.run([str(executable)],cwd=ROOT,env=env,capture_output=True,text=True,encoding='utf-8',timeout=90)
                assert (result.returncode,result.stdout,result.stderr)==(0,expected,''),(name,checked,result)
        negatives={p.stem:p.read_text(encoding='utf-8') for p in sorted((ROOT/'example/samples-error').glob('nested_result_*.chl'))}
        negatives.update({p.stem:p.read_text(encoding='utf-8') for p in sorted((ROOT/'example/samples-error').glob('enum_payload_*.chl'))})
        for name,source in negatives.items():
            path=work/(name+'.chl');path.write_text(source,encoding='utf-8')
            result=subprocess.run([str(compiler),'check',str(path)],cwd=ROOT,env=env,capture_output=True,text=True,encoding='utf-8',timeout=120)
            assert result.returncode!=0 and result.stderr and '语法错误' not in result.stderr,(name,result)
    print('嵌套 Result：用户原例、两种检查模式各10000次调用／20000次close／10000次defer及枚举自动包装／借用移动拒绝边界通过')


if __name__=='__main__':
    main()
