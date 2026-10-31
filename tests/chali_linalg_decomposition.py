"""验证六种 C 数值分解的重构、拥有值及迁移前诊断。"""
from pathlib import Path
import argparse
import hashlib
import json
import os
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile
ROOT=Path(__file__).resolve().parents[1]
BAD=(
 ('let a=std::linalg::Matrix<float64>::zeros(1,2);let result=std::linalg::luDecompose(a);','LU 分解要求方阵'),
 ('let a=std::linalg::Matrix<float64>::identity(2);a.set(0,1,2.0);let result=std::linalg::symmetricEigen(a);','symmetricEigen 要求对称矩阵'),
 ('let a=std::linalg::Matrix<float64>::zeros(0,0);let result=std::linalg::svd(a);','SVD 要求非空矩阵'),
 ('let a=std::linalg::Matrix<float64>::identity(1);let result=std::linalg::jordan2(a);','jordan2 只支持 2x2 矩阵'),
 ('let a=std::linalg::Matrix<float64>::zeros(2,2);a.set(0,1,-1.0);a.set(1,0,1.0);let result=std::linalg::jordan2(a);','jordan2 不支持复特征值'),
 ('let a=std::linalg::Matrix<float64>::identity(2);a.set(0,1,1.0);let result=std::linalg::diagonalize2(a);','矩阵不可对角化'),
)
def main():
    """冻结功能输出及诊断，不生成性能测量数据。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=compiler_path())
    parser.add_argument('--record-baseline',action='store_true')
    args=parser.parse_args();compiler=args.chali.resolve()
    baseline=ROOT/'tests/fixtures/linalg_decomposition_diagnostics.json'
    with tempfile.TemporaryDirectory(prefix='linalg-decomposition-') as directory:
        work=Path(directory)
        def run(command,success=True):
            result=subprocess.run(list(map(str,command)),cwd=ROOT,env=dict(os.environ,CHALI_LANG='zh'),capture_output=True,text=True,encoding='utf-8',errors='replace',timeout=300)
            assert (result.returncode==0)==success,(command,result.returncode,result.stdout,result.stderr)
            return result
        combinations=[('malloc','checked')] if args.record_baseline else [(a,c) for a in ('malloc','pool') for c in ('checked','unchecked')]
        for allocator,checks in combinations:
            target=work/f'{allocator}-{checks}.exe'
            run([compiler,'aot',ROOT/'tests/fixtures/linalg_decomposition_compatibility.chl','--allocator',allocator,'--'+checks,'--out',target])
            result=run([target]);assert result.stdout=='six decompositions, reconstruction, solve and independent ownership OK\n' and not result.stderr,result
            print(f'{allocator}/{checks}: LU, LUR, eigen, SVD, Jordan, diagonalization, solve and independent copies passed',flush=True)
        expected=[] if args.record_baseline else json.loads(baseline.read_text(encoding='utf-8'))['diagnostics']
        for index,(setup,diagnostic) in enumerate(BAD):
            path=work/f'bad-{index}.chl';path.write_text('import std::linalg;\nvoid main(){'+setup+'}',encoding='utf-8')
            result=run([compiler,'run',path,'--out',work/f'bad-{index}'],False)
            first=result.stderr.splitlines()[0];assert diagnostic in first,result
            if args.record_baseline:expected.append(first)
            else:assert first==expected[index],(first,expected[index])
        if args.record_baseline:
            digest=hashlib.sha256(compiler.read_bytes()).hexdigest();assert digest=='ba81494681fd5e6d1284e71b83ae02f190cca1c08c11c14599eb6d15eb86ceb1'
            baseline.write_text(json.dumps(dict(source_commit='8ee8e95c',compiler_sha256=digest,diagnostics=expected),ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
        print('six decomposition rejection diagnostics match frozen implementation',flush=True)
if __name__=='__main__':main()
