"""核对共享标识自举、线程传输、拥有载荷及空定时源 LLVM 降低。"""
from pathlib import Path
import argparse
import hashlib
import json
import os
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile
ROOT=Path(__file__).resolve().parents[1]
SOURCE=ROOT/'tests/fixtures/shared_reference_compatibility.chl'
BASELINE=ROOT/'tests/fixtures/shared_reference_compatibility.json'
SAMPLES=('typed_messages_shared','thread_messages','thread_channel','thread_pool','event_timer')

def main():
    """冻结迁移前实际结果并核对所有原有并发样例和四种原生配置。"""
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--chali',type=Path,default=compiler_path());parser.add_argument('--record-baseline',action='store_true');args=parser.parse_args();compiler=args.chali.resolve()
    with tempfile.TemporaryDirectory(prefix='shared-selfhost-') as directory:
        work=Path(directory)
        def run(command,environment=None,success=True):
            result=subprocess.run([str(x) for x in command],cwd=ROOT,env=dict(os.environ,CHALI_LANG='zh',**(environment or {})),capture_output=True,text=True,encoding='utf-8',errors='replace',timeout=240)
            assert (result.returncode==0)==success,(command,result.returncode,result.stdout,result.stderr)
            return result
        if args.record_baseline:
            digest=hashlib.sha256(compiler.read_bytes()).hexdigest();assert digest=='41def4497d0b41c1ee6c0e1d1f1c7fdd302eaad501dfb07bad289b3eadafee93','仅允许已核对的 1742ab87 编译器冻结旧共享引用行为'
            result=run([compiler,'run',SOURCE,'--out',work/'baseline']);BASELINE.write_text(json.dumps(dict(source_commit='1742ab87',compiler_sha256=digest,output=result.stdout),ensure_ascii=False,indent=2)+'\n',encoding='utf-8');print('old shared reference behavior frozen');return
        expected=json.loads(BASELINE.read_text(encoding='utf-8'))['output']
        for allocator in ('malloc','pool'):
            for checks in ('checked','unchecked'):
                target=work/f'{allocator}-{checks}.exe';run([compiler,'build',SOURCE,'--allocator',allocator,'--'+checks,'--out',target]);result=run([target]);assert result.stdout==expected and not result.stderr,(allocator,checks,result.stdout,result.stderr);print(f'{allocator}/{checks}: shared aliases, payloads, threads, atomics and timers match baseline')
        for name in SAMPLES:
            result=run([compiler,'run',ROOT/f'example/samples/{name}.chl','--out',work/name]);expected_sample=(ROOT/f'example/expected/{name}.txt').read_text(encoding='utf-8');assert result.stdout==expected_sample and not result.stderr,(name,result.stdout,result.stderr);print(name+' passed')
        result=run([compiler,'run',SOURCE,'--out',work/'no-raw'],{'CHALI_NO_BUILD_RAW_INTRINSICS':'1'});assert result.stdout==expected and not result.stderr,result
        prefix=work/'lowering';run([compiler,'build',SOURCE,'--emit-llvm','--out',prefix]);llvm=prefix.with_suffix('.ll').read_text(encoding='utf-8');assert 'chali_std_bind_reference' not in llvm
        for name in ('__referenceNew','__referenceId','__referenceFrom','__referenceCopy','__referenceDrop','__emptySource'):
            assert ('c"std::concurrency::'+name+'\\00"') not in llvm and ('c"std::event::'+name+'\\00"') not in llvm,name
        print('shared records and empty timer source do not use named host dispatch; raw-disabled path passed')

if __name__=='__main__':main()
