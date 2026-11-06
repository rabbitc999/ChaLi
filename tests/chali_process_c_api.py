"""核对纯 C 进程入口的分段读取、生命周期、退出状态与迁移前行为。"""
from pathlib import Path
import argparse
import hashlib
import json
import os
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile
ROOT=Path(__file__).resolve().parents[1]
SOURCE=ROOT/'tests/fixtures/process_compatibility.chl'
BASELINE=ROOT/'tests/fixtures/process_compatibility.json'

def main():
    """冻结旧实现的合法结果与关闭后诊断，分别核对四种原生配置。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cc',default=os.environ.get('CC','gcc'));parser.add_argument('--chali',type=Path,default=compiler_path());parser.add_argument('--record-baseline',action='store_true')
    args=parser.parse_args();compiler=args.chali.resolve()
    with tempfile.TemporaryDirectory(prefix='process-c-api-') as directory:
        work=Path(directory)
        def run(command,success=True):
            result=subprocess.run([str(x) for x in command],cwd=ROOT,env=dict(os.environ,CHALI_LANG='zh'),capture_output=True,text=True,encoding='utf-8',errors='replace',timeout=240)
            assert (result.returncode==0)==success,(command,result.returncode,result.stdout,result.stderr)
            return result
        unit=work/'process-unit.exe'
        run([args.cc,'-std=c11','-Wall','-Wextra','-Werror','-I'+str(ROOT/'source'),ROOT/'tests/chali_process_c_api.c','-o',unit])
        print(run([unit,work/'pipe.tmp']).stdout.strip())
        errors={}
        for method in ('processRead','processClose'):
            path=work/(method+'.chl');path.write_text('import std::process;\nvoid main(){Process process=std::process::processRun("echo closed");std::process::processRead(&process);std::process::processClose(&process);std::process::'+method+'(&process);}',encoding='utf-8')
            result=run([compiler,'run',path,'--out',work/(method+'-bad')],False);errors[method]=result.stderr.splitlines()[0]
        if args.record_baseline:
            digest=hashlib.sha256(compiler.read_bytes()).hexdigest()
            assert digest=='bab8a357e69a21c293cd43b68ae6e9e5a02bb7c5aeba0bb51d0b75c48706d9bb','仅允许已核对的 a040210d 编译器冻结旧进程行为'
            output=run([compiler,'run',SOURCE,'--out',work/'baseline'])
            BASELINE.write_text(json.dumps(dict(source_commit='a040210d',compiler_sha256=digest,output=output.stdout,errors=errors),ensure_ascii=False,indent=2)+'\n',encoding='utf-8');print('old process behavior frozen');return
        expected=json.loads(BASELINE.read_text(encoding='utf-8'));assert errors==expected['errors'],(errors,expected['errors'])
        for allocator in ('malloc','pool'):
            for checks in ('checked','unchecked'):
                target=work/f'{allocator}-{checks}.exe';run([compiler,'build',SOURCE,'--allocator',allocator,'--'+checks,'--out',target]);result=run([target])
                assert result.stdout==expected['output'] and not result.stderr,(allocator,checks,result.stdout,result.stderr)
                print(f'{allocator}/{checks}: process behavior and ownership match baseline')
        print('closed-pipe errors preserve original first lines')

if __name__=='__main__':main()
