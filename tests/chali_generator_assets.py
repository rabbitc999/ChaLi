"""解压实际前端库中的 ABI 资产，防止新原语只从工作区磁盘导出而未被嵌入。"""
from pathlib import Path
from project_temp import tempfile
import argparse
import os
import subprocess

ROOT=Path(__file__).resolve().parents[1]


def main():
    """以匹配发行库的工具链连接资产专项，逐项比较源字节。"""
    sdk=ROOT/'build/dependencies/llvm/static-sdk'
    default=(ROOT/'build/llvm-static-gcc16') if os.name=='nt' and sdk.exists() else ROOT/'build'
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path)
    parser.add_argument('--build-root',type=Path,default=default)
    parser.add_argument('--cxx',default=str(sdk/'bin/g++.exe') if os.name=='nt' and sdk.exists() else 'g++')
    parser.add_argument('--sysroot',type=Path)
    args=parser.parse_args();build=args.build_root.resolve()
    use_sdk=Path(args.cxx).resolve().parent==(sdk/'bin').resolve()
    environment=dict(os.environ,PATH=str(Path(args.cxx).resolve().parent)+os.pathsep+os.environ.get('PATH',''))
    with tempfile.TemporaryDirectory(prefix='generator-assets-') as directory:
        work=Path(directory);output=work/('assets.exe' if os.name=='nt' else 'assets')
        flags=['-std=c++17','-O1','-DCHALI_CUSTOM_ALLOCATOR=1','-DSTACKVM_AOT_ONLY=1','-DSTACKVM_WITH_REFLECTION=1','-DCHALI_WITH_REFLECTION=1','-I'+str(ROOT/'source'),'-I'+str(ROOT/'library'),'-pthread']
        if args.sysroot or use_sdk:flags+=['--sysroot='+str((args.sysroot or sdk).resolve())]
        command=[args.cxx,*flags,ROOT/'tests/chali_generator_assets.cpp',ROOT/'source/allocator/system_arena.cpp',
                 '-Wl,--gc-sections','-Wl,--start-group',build/'chali-frontend/libparser.a',
                 build/'chali-ircode-aot-core/libstackvm.a',build/'chali-host-records/librecords.a','-Wl,--end-group','-o',output]
        command+=['-static','-fuse-ld=lld','-lws2_32','-lwinhttp','-lbcrypt','-lcrypt32'] if os.name=='nt' else ['-ldl']
        result=subprocess.run(list(map(str,command)),cwd=ROOT,env=environment,capture_output=True,encoding='utf-8',errors='replace',timeout=180)
        assert result.returncode==0,(result.stdout,result.stderr)
        result=subprocess.run([str(output),str(ROOT)],cwd=work,env=environment,capture_output=True,encoding='utf-8',timeout=60)
        assert result.returncode==0,(result.stdout,result.stderr)
        print(result.stdout.strip())


if __name__=='__main__':
    main()
