"""验证 C 帧压缩流、原始字节、拥有结果及损坏数据诊断。"""
from pathlib import Path
import argparse
import json
import os
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile
ROOT=Path(__file__).resolve().parents[1]
SOURCE=ROOT/'tests/fixtures/compress_compatibility.chl'
BASELINE=ROOT/'tests/fixtures/compress_compatibility.json'
BAD=([],[0],[77,76,90,50,0,0],[77,76,90,49,0,3,65],[77,76,90,49,2,0],[77,76,90,49,1,1],[77,76,90,49,0]+[255]*10)

def main():
    """对照冻结的完整压缩字节，核对四配置及全部损坏类别。"""
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--chali',type=Path,default=compiler_path());args=parser.parse_args();compiler=args.chali.resolve()
    with tempfile.TemporaryDirectory(prefix='compress-c-api-') as directory:
        work=Path(directory)
        def run(command,success=True):
            result=subprocess.run([str(x) for x in command],cwd=ROOT,env=dict(os.environ,CHALI_LANG='zh'),capture_output=True,text=True,encoding='utf-8',errors='replace',timeout=240)
            assert (result.returncode==0)==success,(command,result.returncode,result.stdout,result.stderr)
            return result
        expected=json.loads(BASELINE.read_text(encoding='utf-8'))['output']
        for allocator in ('malloc','pool'):
            for checks in ('checked','unchecked'):
                target=work/f'{allocator}-{checks}.exe';run([compiler,'build',SOURCE,'--allocator',allocator,'--'+checks,'--out',target]);result=run([target]);assert result.stdout==expected and not result.stderr,(allocator,checks,result.stdout,result.stderr);print(f'{allocator}/{checks}: seven frozen compression streams and owned raw contents match')
        accepted=work/'empty-prefix.chl'
        accepted.write_text('import std::compress;\nvoid main(){List<uint8> bytes;bytes.push((uint8)77);bytes.push((uint8)76);bytes.push((uint8)90);bytes.push((uint8)49);bytes.push((uint8)0);assert(std::compress::decompress(bytes)=="");}',encoding='utf-8')
        run([compiler,'run',accepted,'--out',work/'empty-prefix'])
        for index,values in enumerate(BAD):
            path=work/f'bad-{index}.chl';path.write_text('import std::compress;\nvoid main(){List<uint8> bytes;'+''.join(f'bytes.push((uint8){value});' for value in values)+'std::compress::decompress(bytes);}',encoding='utf-8')
            result=run([compiler,'run',path,'--out',work/f'bad-{index}'],False)
            assert 'std::compress::decompress：数据不是有效的 lztext 流或已损坏' in result.stderr.splitlines()[0],result.stderr
        result=run([compiler,'run',ROOT/'example/samples/std_compress.chl','--out',work/'sample']);assert result.stdout==(ROOT/'example/expected/std_compress.txt').read_text(encoding='utf-8') and not result.stderr,result
        print('seven damaged stream classes and original sample passed')

if __name__=='__main__':main()
