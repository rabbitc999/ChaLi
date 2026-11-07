"""显式借用契约的来源、短写、泛型与压力回归。"""
from pathlib import Path
import argparse
import os
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
SAMPLES=ROOT/'example/samples'
ERRORS=ROOT/'example/samples-error'
CASES={p.stem.removeprefix('borrow_contract_'):(p,valid)
       for folder,valid in ((SAMPLES,True),(ERRORS,False))
       for p in sorted(folder.glob('borrow_contract_*.chl')) if p.stem!='borrow_contract_pressure'}

def stress_source():
    """读取可直接运行且纳入全量门禁的压力样例。"""
    return (SAMPLES/'borrow_contract_pressure.chl').read_text(encoding='utf-8')


def main():
    """独立拒绝非法程序，并执行长链、跨模块和模板状态压力程序。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--cases-only', action='store_true')
    parser.add_argument('--only', help='仅运行逗号分隔的边界名称')
    args = parser.parse_args()
    compiler = args.chali.resolve()
    env = dict(os.environ,CHALI_LANG='zh',CHALI_QUIET='1')
    selected = {name:case for name,case in CASES.items() if not args.only or name in args.only.split(',')}
    assert selected, args.only
    failures=[]
    with tempfile.TemporaryDirectory(prefix='borrow-contract-stress-') as directory:
        work=Path(directory)
        for name,(path,valid) in selected.items():
            result=subprocess.run([str(compiler),'check',str(path)],cwd=ROOT,env=env,capture_output=True,
                                  text=True,encoding='utf-8',timeout=120)
            accepted = result.returncode == 0
            meaningful = valid or any(code in result.stderr for code in ('CHL1204','CHL1205','CHL1210','CHL1211','CHL1212','CHL1213','CHL1214'))
            if accepted != valid or not meaningful:failures.append((name,valid,result.returncode,result.stdout,result.stderr))
        assert not failures,failures
        if not args.cases_only:
            path=work/'pressure.chl';path.write_text(stress_source(),encoding='utf-8')
            result=subprocess.run([str(compiler),'run',str(path),'--out',str(work/'pressure')],cwd=ROOT,env=env,
                                  capture_output=True,text=True,encoding='utf-8',timeout=240)
            assert (result.returncode,result.stdout,result.stderr)==(0,'borrow stress: iterations=2048 aliases=64 parameters=96\n',''),result
            path=SAMPLES/'borrow_contract_modules.chl'
            result=subprocess.run([str(compiler),'run',str(path),'--out',str(work/'modules')],cwd=ROOT,env=env,
                                  capture_output=True,text=True,encoding='utf-8',timeout=180)
            assert (result.returncode,result.stdout,result.stderr)==(0,'',''),result
    print(f'#borrow：{len(selected)} 个有效／拒绝边界通过；短写保持 target/source 含义')
    if not args.cases_only:print('压力：2048 次循环、64 层别名、96 个实参、重复契约、泛型双实例及跨模块同名调用通过')


if __name__=='__main__':
    main()
