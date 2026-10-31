"""核对 CAS 直接 C 入口在两种分配器和检查配置中的不可变值与线程转移。"""
from compiler_path import compiler_path
from project_temp import tempfile
from chali_cas_math import CASES
from pathlib import Path
import argparse
import os
import subprocess

ROOT=Path(__file__).resolve().parents[1]


def main():
    """执行符号变换与跨线程所有权用例，不进行性能计时。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=compiler_path())
    compiler=parser.parse_args().chali.resolve()
    with tempfile.TemporaryDirectory(prefix='cas-c-api-') as directory:
        work=Path(directory)
        for allocator in ('malloc','pool'):
            for checks in ('checked','unchecked'):
                for name in ('transforms','thread_copy'):
                    source=work/(name+'.chl');source.write_text(CASES[name],encoding='utf-8')
                    result=subprocess.run([str(compiler),'run',str(source),'--allocator',allocator,'--'+checks,
                                           '--out',str(work/(name+allocator+checks))],cwd=ROOT,capture_output=True,
                                          encoding='utf-8',errors='replace',timeout=120,env=dict(os.environ,CHALI_LANG='zh'))
                    assert (result.returncode,result.stdout,result.stderr)==(0,'ok\n',''),(name,allocator,checks,result)
                print(allocator+'/'+checks+': symbolic transforms and thread ownership passed',flush=True)


if __name__=='__main__':
    main()
