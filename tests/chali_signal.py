"""验证信号算法自举的冻结对照、四配置、独立存储与错误行为。"""
from pathlib import Path
import argparse
import hashlib
import json
import math
import os
import struct
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile
ROOT=Path(__file__).resolve().parents[1]
BASELINE=ROOT/'tests/fixtures/signal_compatibility.json'


def program():
    """生成覆盖空、奇数、二次幂长度和逆变换的确定性复数输入。"""
    parts=['import std::complex;\nimport std::signal;\nimport std::math;\nvoid main(){']
    for n in (0,1,2,3,4,7,8,16,32):
        parts+=['{List<std::complex::Complex<float64>> input;']
        for i in range(n):
            parts += [f'input.push(std::complex::Complex<float64>::of({((i*7)%13-6)/8:.8f},{((i*11)%17-8)/4:.8f}));']
        methods=['dft','idft']+(['fft','ifft'] if n>0 and n&(n-1)==0 else [])
        for method in methods:
            parts += ['{', f'List<std::complex::Complex<float64>> output=std::signal::{method}(input);', f'assert(output.len()=={n});', 'int64 i=0; while(i<output.len()){std::complex::Complex<float64> v=output[i];', 'List<uint8> a=std::core::float64ToBytes(v.re,false);List<uint8> b=std::core::float64ToBytes(v.im,false);', 'int64 j=0;while(j<8){print("{} ",(int64)a[j]);j++;}j=0;while(j<8){print("{} ",(int64)b[j]);j++;}println("");i++;}', 'output.clear();',f'assert(input.len()=={n});','}']
        if n and n&(n-1)==0:
            parts+=['{List<std::complex::Complex<float64>> restored=std::signal::ifft(std::signal::fft(input));int64 i=0;while(i<input.len()){std::complex::Complex<float64> a=input[i];std::complex::Complex<float64> b=restored[i];assert(std::math::abs(a.re-b.re)<0.00000001);assert(std::math::abs(a.im-b.im)<0.00000001);i++;}}']
        parts+=['}']
    for pattern in ('infinity','negative-infinity','nan','negative-zero','max-finite','subnormal'):
        bits={'infinity':[0,0,0,0,0,0,240,127],'negative-infinity':[0,0,0,0,0,0,240,255],'nan':[1,0,0,0,0,0,248,127],'negative-zero':[0,0,0,0,0,0,0,128],'max-finite':[255,255,255,255,255,255,239,127],'subnormal':[1,0,0,0,0,0,0,0]}[pattern]
        for method in ('dft','idft','fft','ifft'):
            parts+=['{uint8 raw[8];']+[f'raw[{i}]=(uint8){value};' for i,value in enumerate(bits)]
            parts+=['float64 special=std::core::bytesToFloat64(raw,8,false).unwrap();List<std::complex::Complex<float64>> input;', 'input.push(std::complex::Complex<float64>::of(special,special));input.push(std::complex::Complex<float64>::of(1.0,0.0));',f'List<std::complex::Complex<float64>> output=std::signal::{method}(input);', 'int64 i=0;while(i<output.len()){std::complex::Complex<float64> v=output[i];List<uint8> a=std::core::float64ToBytes(v.re,false);List<uint8> b=std::core::float64ToBytes(v.im,false);int64 j=0;while(j<8){print("{} ",(int64)a[j]);j++;}j=0;while(j<8){print("{} ",(int64)b[j]);j++;}println("");i++;}}']
    return '\n'.join(parts+['}'])


def values(output):
    """按字节还原双精度结果，避免十进制显示精度掩盖差异。"""
    return [struct.unpack('<dd',bytes(map(int,line.split()))) for line in output.splitlines()]


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=compiler_path())
    parser.add_argument('--record-baseline',action='store_true')
    args=parser.parse_args();compiler=args.chali.resolve()
    with tempfile.TemporaryDirectory(prefix='signal-selfhost-') as directory:
        work=Path(directory);source=work/'signal.chl';source.write_text(program(),encoding='utf-8')
        def run(command,success=True):
            result=subprocess.run([str(x) for x in command],cwd=ROOT,capture_output=True,text=True,encoding='utf-8',errors='replace',timeout=300,env=dict(os.environ,CHALI_LANG='zh'))
            assert (result.returncode==0)==success,(command,result.stdout,result.stderr)
            return result
        if args.record_baseline:
            digest=hashlib.sha256(compiler.read_bytes()).hexdigest()
            assert digest=='ed18cc49ae70b9270e91ec72cc55b42082cbffb57c22510ee0facb91a6e6d4bd','只允许已核对的 568c8e12 编译器冻结原实现'
            output=run([compiler,'run',source,'--out',work/'baseline'])
            BASELINE.write_text(json.dumps(dict(source_commit='568c8e12',compiler_sha256=digest,output=output.stdout),indent=2)+'\n',encoding='utf-8')
            print('old signal implementation frozen');return
        expected=values(json.loads(BASELINE.read_text(encoding='utf-8'))['output'])
        for allocator in ('malloc','pool'):
            for checks in ('checked','unchecked'):
                target=work/f'{allocator}-{checks}.exe'
                run([compiler,'build',source,'--allocator',allocator,'--'+checks,'--out',target])
                actual=values(run([target]).stdout);assert len(actual)==len(expected)
                for pair,wanted in zip(actual,expected):
                    assert all((math.isnan(x) and math.isnan(y)) or math.isclose(x,y,rel_tol=1e-12,abs_tol=1e-12) for x,y in zip(pair,wanted)),(pair,wanted)
                print(f'{allocator}/{checks}: {len(actual)} complex results match baseline')
        prefix=work/'sample';run([compiler,'build',ROOT/'example/samples/std_complex_fft.chl','--emit-llvm','--out',prefix])
        llvm=prefix.with_suffix('.ll').read_text(encoding='utf-8');assert 'chali_std_bind_signal' not in llvm
        for method in ('fft','ifft'):
            for size in (0,3):
                bad=work/'bad.chl';bad.write_text('import std::complex;\nimport std::signal;\nvoid main(){List<std::complex::Complex<float64>> input;'+''.join('input.push(std::complex::Complex<float64>::of(1.0,0.0));' for _ in range(size))+f'std::signal::{method}(input);'+'}',encoding='utf-8')
                result=run([compiler,'run',bad,'--out',work/f'bad-{method}-{size}'],False)
                assert f'chali build: {method} 长度必须是正的 2 的幂' in result.stderr,result.stderr
        print('four invalid lengths preserve diagnostics; no old signal binder')

if __name__=='__main__':main()
