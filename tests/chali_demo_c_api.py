"""核对纯 C 演示入口、完整声明注入与既有宿主互操作样例的行为。"""
from compiler_path import compiler_path
from project_temp import tempfile
from pathlib import Path
import argparse
import os
import subprocess

ROOT=Path(__file__).resolve().parents[1]


def main():
    """四配置验证具名句柄、枚举、接口继承、指针、Option 引用及捕获闭包。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=compiler_path())
    compiler=parser.parse_args().chali.resolve()
    names=('c_api_host','cpp_glue','variadic_glue','function_pointer_glue','glue_types','glue_interface_inherit',
           'glue_system_interfaces','glue_members','pointer_glue','reference_glue_optional','obj')
    with tempfile.TemporaryDirectory(prefix='demo-c-api-') as directory:
        work=Path(directory)
        for allocator in ('malloc','pool'):
            for checks in ('checked','unchecked'):
                for name in names:
                    source=ROOT/'example/samples'/(name+'.chl')
                    result=subprocess.run([str(compiler),'run',str(source),'--allocator',allocator,'--'+checks,
                                           '--out',str(work/(name+allocator+checks))],cwd=ROOT,capture_output=True,
                                          encoding='utf-8',errors='replace',timeout=120,env=dict(os.environ,CHALI_LANG='zh'))
                    assert (result.returncode,result.stdout,result.stderr)==(0,(ROOT/'example/expected'/(name+'.txt')).read_text(encoding='utf-8'),''),(name,allocator,checks,result)
                print(allocator+'/'+checks+': eleven frozen C host and type identity samples passed',flush=True)
        source=ROOT/'example/samples-error/glue_member_unbound.chl'
        result=subprocess.run([str(compiler),'run',str(source),'--out',str(work/'missing')],cwd=ROOT,
                              capture_output=True,encoding='utf-8',timeout=120,env=dict(os.environ,CHALI_LANG='zh'))
        assert result.returncode and 'glue_demo_rect_missing' in result.stdout+result.stderr,result
        print('Unbound member remains rejected')


if __name__=='__main__':
    main()
