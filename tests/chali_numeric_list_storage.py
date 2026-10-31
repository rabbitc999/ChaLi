"""核对数值 List 紧凑槽与通用回退的布局、位模式、容量和清理。"""
from project_temp import tempfile
from pathlib import Path
import argparse
import json
import os
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def main():
    """链接实际运行时库，保存两个布局模式的断言与原始输出。"""
    sdk=ROOT/'build/dependencies/llvm/static-sdk'
    static=os.name=='nt' and (sdk/'bin/g++.exe').is_file()
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,help='模块门禁兼容参数；单元链接运行时库')
    parser.add_argument('--build-root',type=Path,default=ROOT/('build/llvm-static-gcc16' if static else 'build'))
    parser.add_argument('--cxx')
    parser.add_argument('--sysroot',type=Path)
    args=parser.parse_args()
    build=args.build_root.resolve()
    use_sdk=static and build==(ROOT/'build/llvm-static-gcc16').resolve()
    cxx=args.cxx or (str(sdk/'bin/g++.exe') if use_sdk else 'g++')
    sysroot=args.sysroot or (sdk if Path(cxx).resolve().parent==(sdk/'bin').resolve() else None)
    environment=dict(os.environ)
    if Path(cxx).is_file():
        environment['PATH']=str(Path(cxx).resolve().parent)+os.pathsep+environment.get('PATH','')
    work = Path(tempfile.mkdtemp(prefix='numeric-list-storage-'))
    records = []

    def execute(command, label, env=None):
        """保留失败时可复现的命令和输出。"""
        command = list(map(str, command))
        result = subprocess.run(command, cwd=ROOT, env=environment if env is None else env, capture_output=True,
                                text=True, encoding='utf-8', timeout=600)
        records.append(dict(label=label, command=command, code=result.returncode,
                            stdout=result.stdout, stderr=result.stderr))
        (work / 'results.json').write_text(json.dumps(records, ensure_ascii=False, indent=2), encoding='utf-8')
        assert result.returncode == 0, (records[-1], work)

    unit = work / ('unit.exe' if os.name == 'nt' else 'unit')
    libraries=[build/'chali-ircode-aot-core/libstackvm.a',build/'chali-host-records/librecords.a']
    flags=['-std=c++17','-O2','-pthread','-ffunction-sections','-fdata-sections',
           '-DSTACKVM_AOT_ONLY=1','-DCHALI_CUSTOM_ALLOCATOR=1','-DCHALI_WITH_REFLECTION=1',
           '-DSTACKVM_WITH_REFLECTION=1','-Isource','-Isource/ircode','-Ilibrary','-I.']
    if sysroot:flags+=['--sysroot='+str(sysroot.resolve())]
    windows=['-static','-fuse-ld=lld','-lwinhttp','-lws2_32','-lbcrypt','-lcrypt32'] if os.name=='nt' else ['-ldl']
    execute([cxx,*flags,ROOT/'tests/chali_numeric_list_storage.cpp',ROOT/'source/allocator/system_arena.cpp',
             '-Wl,--gc-sections','-Wl,--start-group',*libraries,'-Wl,--end-group',*windows,'-o',unit],'compile')
    for fallback in (False, True):
        env = dict(os.environ)
        env.pop('CHALI_NO_NUMERIC_LIST_STORAGE', None)
        if fallback:
            env['CHALI_NO_NUMERIC_LIST_STORAGE'] = '1'
        execute([unit], 'generic' if fallback else 'compact', env)
    print('numeric List storage: layouts, values, float bits, fallback, capacity and drop OK;', work)


if __name__ == '__main__':
    main()
