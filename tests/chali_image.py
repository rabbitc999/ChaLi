"""验证图像 C 迁移的现有输出、四配置及文件读写往返。"""
from pathlib import Path
import argparse
import json
import os
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile
ROOT=Path(__file__).resolve().parents[1]
def main():
    """所有产物位于项目 tmp，真实原生执行不使用 VM 或四元式导出。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=compiler_path())
    args=parser.parse_args();compiler=args.chali.resolve()
    with tempfile.TemporaryDirectory(prefix='image-c-api-') as directory:
        work=Path(directory)
        def run(command):
            result=subprocess.run(list(map(str,command)),cwd=ROOT,env=dict(os.environ,CHALI_LANG='zh'),capture_output=True,text=True,encoding='utf-8',errors='replace',timeout=300)
            assert result.returncode==0,(command,result.returncode,result.stdout,result.stderr)
            return result
        for allocator in ('malloc','pool'):
            for checks in ('checked','unchecked'):
                prefix=work/f'{allocator}-{checks}'
                run([compiler,'aot',ROOT/'example/samples/std_image.chl','--allocator',allocator,'--'+checks,'--out',prefix])
                result=run([prefix.with_suffix('.exe' if os.name=='nt' else '')])
                expected=(ROOT/'example/expected/std_image.txt').read_text(encoding='utf-8')
                assert result.stdout==expected and not result.stderr,result
                print(f'{allocator}/{checks}: pixels, convolution, pooling, colors, Otsu, BMP/PPM/PGM roundtrip passed',flush=True)
                extended=work/f'{allocator}-{checks}-extended'
                run([compiler,'aot',ROOT/'tests/fixtures/image_compatibility.chl','--allocator',allocator,'--'+checks,'--out',extended])
                result=run([extended.with_suffix('.exe' if os.name=='nt' else '')])
                frozen=json.loads((ROOT/'tests/fixtures/image_compatibility.json').read_text(encoding='utf-8'))['stdout']
                assert result.stdout==frozen and not result.stderr,result
                print(f'{allocator}/{checks}: recoverable diagnostics and reused reply append match frozen compiler',flush=True)
        path=json.dumps((work/'图片-roundtrip.bmp').as_posix(),ensure_ascii=False)
        bad=json.dumps((work/'missing'/'bad.bmp').as_posix())
        source=work/'io.chl'
        source.write_text('import std::image;\nvoid main(){Image image=Image::make(4,2).unwrap();image.setPixel(0,0,10,20,30,255);image.setPixel(3,1,200,100,50,255);assert(image.saveBmp('+path+').unwrap()==78);Image loaded=Image::open('+path+').unwrap();Pixel a=loaded.getPixel(0,0).unwrap();Pixel b=loaded.getPixel(3,1).unwrap();assert(loaded.width()==4 && loaded.height()==2 && a.r==10 && a.g==20 && a.b==30 && b.r==200 && b.g==100 && b.b==50);assert(image.saveBmp('+bad+').isErr());Image copy=image;image.setPixel(0,0,1,2,3,255);assert(copy.getPixel(0,0).unwrap().r==10);println("image files and independent ownership OK");}',encoding='utf-8')
        result=run([compiler,'run',source,'--out',work/'io'])
        assert result.stdout=='image files and independent ownership OK\n' and not result.stderr,result
        assert (work/'图片-roundtrip.bmp').read_bytes()[:2]==b'BM'
        result=run([compiler,'run',ROOT/'example/samples/std_string_builder.chl','--out',work/'builder'])
        assert result.stdout==(ROOT/'example/expected/std_string_builder.txt').read_text(encoding='utf-8') and not result.stderr,result
    print('Image file IO, independent copies and StringBuilder cross-module sample passed')
if __name__=='__main__':main()
