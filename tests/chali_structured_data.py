"""验证实际 JSON/TOML 内核、原生执行、独立结果与关闭后拒绝。"""
from pathlib import Path
import argparse
import os
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile
ROOT=Path(__file__).resolve().parents[1]

def main():
    """编译仓库内核测试并核对当前 AOT 契约，不引用已删除的 VM／裁剪产物。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=compiler_path())
    parser.add_argument('--cxx',default=os.environ.get('CXX','g++'))
    args=parser.parse_args();compiler=args.chali.resolve()
    with tempfile.TemporaryDirectory(prefix='structured-data-') as directory:
        work=Path(directory)
        def run(command,success=True):
            result=subprocess.run([str(x) for x in command],cwd=ROOT,capture_output=True,text=True,encoding='utf-8',errors='replace',env=dict(os.environ,CHALI_LANG='zh'),timeout=300)
            assert (result.returncode==0)==success,(command,result.returncode,result.stdout,result.stderr)
            return result
        for name in ('json','toml'):
            target=work/(name+'.exe')
            sources=[ROOT/f'source/{name}/test_{name}.cpp',ROOT/f'source/{name}/{name}.cpp']
            if name=='toml':sources.append(ROOT/'source/toml/toml_source.cpp')
            run([args.cxx,'-std=c++17','-O2',*sources,'-o',target]);run([target])
            print(name+' C++ parser kernel passed')
        source=ROOT/'example/samples/std_structured_data.chl'
        expected=(ROOT/'example/expected/std_structured_data.txt').read_text(encoding='utf-8')
        for checks in ('checked','unchecked'):
            target=work/(checks+'.exe')
            run([compiler,'aot',source,'--'+checks,'--out',target]);result=run([target])
            assert result.stdout==expected and not result.stderr,(checks,result.stdout,result.stderr)
        result=run([compiler,'run',ROOT/'example/samples-error/structured_closed.chl','--out',work/'closed'],False)
        expected_error=(ROOT/'example/expected-error/structured_closed.txt').read_text(encoding='utf-8').splitlines()[0]
        assert result.stderr.splitlines()[0]==expected_error,result.stderr
        print('structured data checked/unchecked and closed resource diagnostics passed')

if __name__=='__main__':main()
