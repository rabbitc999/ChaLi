"""核对紧凑数值槽的旧十类型、扩展 bool／char 和错误行为。"""
from pathlib import Path
import argparse
import os
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT=Path(__file__).resolve().parents[1]


def main():
    """旧版本可只运行兼容矩阵，新版本同时验证两种扩展基础类型。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=compiler_path())
    parser.add_argument('--legacy-only',action='store_true')
    options=parser.parse_args();compiler=options.chali.resolve()
    with tempfile.TemporaryDirectory(prefix='scalar-c-api-') as directory:
        work=Path(directory)
        def run(command,success=True):
            result=subprocess.run(list(map(str,command)),cwd=ROOT,env=dict(os.environ,CHALI_LANG='zh'),capture_output=True,text=True,encoding='utf-8',errors='replace',timeout=300)
            assert (result.returncode==0)==success,(command,result.returncode,result.stdout,result.stderr)
            return result
        original=(ROOT/'tests/fixtures/scalar_storage_compatibility.chl').read_text(encoding='utf-8')
        for allocator in ('malloc','pool'):
            for checks in ('checked','unchecked'):
                source=work/f'{allocator}-{checks}.chl'
                text=original if options.legacy_only else original.replace('void main() {','void main() {\ncheckStorage<bool>(true,false);checkStorage<char>(\'A\',\'z\');')
                source.write_text(text,encoding='utf-8')
                target=work/f'{allocator}-{checks}.exe'
                run([compiler,'build',source,'--allocator',allocator,'--'+checks,'--out',target]);result=run([target])
                assert result.stdout=='ten scalar buffers, boundaries, copies and iteration OK\n' and not result.stderr,result
                print(f'{allocator}/{checks}: {10 if options.legacy_only else 12} scalar buffers, extrema, copies, clear and iteration passed',flush=True)
        if not options.legacy_only:
            for index,statement in enumerate(('a.set(-1,(int64)1);','a.set(1,(int64)1);','a.at(-1);','a.at(1);')):
                source=work/f'bad-{index}.chl';source.write_text('import std::linear_algebra;\nvoid main(){std::linear_algebra::ScalarArray<int64> a;a.push((int64)7);'+statement+'}',encoding='utf-8')
                result=run([compiler,'run',source,'--out',work/f'bad-{index}'],False)
                assert '线性代数存储下标越界' in result.stderr,result.stderr
            print('set and read bounds checks passed')


if __name__=='__main__':main()
