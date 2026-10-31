"""对照字节封装、错误优先级、拥有值和 C 帧格式转换。"""
from pathlib import Path
import argparse
import hashlib
import json
import os
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile
ROOT=Path(__file__).resolve().parents[1]
SOURCE=ROOT/'tests/fixtures/serialization_compatibility.chl'
BASELINE=ROOT/'tests/fixtures/serialization_compatibility.json'

def main():
    """以真实迁移前编译器冻结结果，核对四种原生配置。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=compiler_path())
    parser.add_argument('--record-baseline',action='store_true')
    args=parser.parse_args();compiler=args.chali.resolve()
    with tempfile.TemporaryDirectory(prefix='serialization-selfhost-') as directory:
        work=Path(directory)
        def run(command):
            result=subprocess.run([str(x) for x in command],cwd=ROOT,env=dict(os.environ,CHALI_LANG='zh'),capture_output=True,text=True,encoding='utf-8',errors='replace',timeout=300)
            assert result.returncode==0,(command,result.stdout,result.stderr)
            return result
        if args.record_baseline:
            digest=hashlib.sha256(compiler.read_bytes()).hexdigest()
            assert digest=='2eb462732821c9d39aa012d55f9a10e264414a717d6dd97e5c139d7848cb1b4e','仅允许已核对的 e25443fc 编译器冻结行为'
            result=run([compiler,'run',SOURCE,'--out',work/'baseline'])
            BASELINE.write_text(json.dumps(dict(source_commit='e25443fc',compiler_sha256=digest,output=result.stdout),ensure_ascii=False,indent=2)+'\n',encoding='utf-8');print('old serialization behavior frozen');return
        expected=json.loads(BASELINE.read_text(encoding='utf-8'))['output']
        for allocator in ('malloc','pool'):
            for checks in ('checked','unchecked'):
                target=work/f'{allocator}-{checks}.exe'
                run([compiler,'aot',SOURCE,'--allocator',allocator,'--'+checks,'--out',target]);actual=run([target])
                assert actual.stdout==expected,(allocator,checks,actual.stdout,expected)
                assert not actual.stderr,actual.stderr
                print(f'{allocator}/{checks}: versioned bytes, diagnostics and TOML match baseline')
        prefix=work/'serde';run([compiler,'aot',SOURCE,'--emit-llvm','--out',prefix])
        llvm=prefix.with_suffix('.ll').read_text(encoding='utf-8')
        assert 'c"std::serialization::__pack\\00"' not in llvm and 'c"std::serialization::__unpack\\00"' not in llvm,'selfhost functions must not use named host dispatch'
        print('selfhost byte helpers do not call old named host entries')

if __name__=='__main__':main()
