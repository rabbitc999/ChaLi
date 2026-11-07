"""校验对象建立及整数设置标量入口的快照、错误、回退与接口重入。"""
from compiler_path import compiler_path
from project_temp import tempfile
from pathlib import Path
import argparse
import json
import os
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def main():
    """使用匹配的运行时库编译单测，再核对标量、原入口及步数检查模式。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    work = Path(tempfile.mkdtemp(prefix='object-construction-scalar-'))
    env = dict(os.environ, CHALI_LANG='zh', CHALI_QUIET='1')
    records = []

    def execute(command, label, extra=None):
        """保存实际命令和原始输出，失败停止但保留本次验证目录。"""
        result = subprocess.run(list(map(str, command)), cwd=ROOT, env=dict(env, **(extra or {})),
                                capture_output=True, text=True, encoding='utf-8', errors='replace', timeout=600)
        records.append(dict(label=label, command=list(map(str,command)), code=result.returncode,
                            stdout=result.stdout, stderr=result.stderr))
        (work/'results.json').write_text(json.dumps(records,ensure_ascii=False,indent=2),encoding='utf-8')
        assert result.returncode == 0, (records[-1],work)
        return result

    info = json.loads(execute([compiler,'--build-info'],'build-info').stdout)
    static = os.name == 'nt' and info['llvm'] == 'static'
    build = ROOT/('build/llvm-static-gcc16' if static else 'build')
    sdk = ROOT/'build/dependencies/llvm/static-sdk'
    cxx = str(sdk/'bin/g++.exe') if static else 'g++'
    options = ['--sysroot='+str(sdk),'-static'] if static else []
    unit = work/('unit.exe' if os.name == 'nt' else 'unit')
    libraries = [build/p for p in ('chali-ircode-aot-core/libstackvm.a','chali-host-records/librecords.a',
                                   'chali-runtime/libast.a','chali-runtime/libpreprocess.a')]
    execute([cxx,*options,'-std=c++17','-O2','-pthread','-ffunction-sections','-fdata-sections',
             '-DSTACKVM_AOT_ONLY=1','-DCHALI_CUSTOM_ALLOCATOR=1','-DCHALI_WITH_REFLECTION=1',
             '-DSTACKVM_WITH_REFLECTION=1','-Isource','-Ilibrary','-I.',
             ROOT/'tests/chali_object_construction_scalar.cpp',ROOT/'source/allocator/system_arena.cpp',
             '-Wl,--gc-sections','-Wl,--start-group',*libraries,'-Wl,--end-group',
             *(['-lwinhttp'] if os.name == 'nt' else []),'-o',unit],'compile-unit',
            {'PATH':str(sdk/'bin')+os.pathsep+env.get('PATH','')} if static else None)
    execute([unit],'unit')
    fixtures = [('aot_interface_copy_scalar','interface snapshots and scalar object copy OK\n'),
                ('aot_interface_wrapper_reentry','interface binding reentry OK\n')]
    modes = [(allocator,fallback,False) for allocator in ('malloc','pool') for fallback in (False,True)]
    modes.append(('malloc',False,True))
    env.pop('CHALI_NO_AOT_OBJECT_CONSTRUCTION_SCALARS',None)
    for allocator,fallback,steps in modes:
        extra = {'CHALI_NO_AOT_OBJECT_CONSTRUCTION_SCALARS':'1'} if fallback else {}
        for fixture,expected in fixtures:
            label = fixture+'-'+allocator+('-generic' if fallback else '-scalar')+('-steps' if steps else '')
            source = ROOT/'tests/fixtures'/(fixture+'.chl')
            prefix = work/label
            limits = ['--step-limit','1000000'] if steps else []
            result = execute([compiler,'run',source,'--allocator',allocator,*limits,'--out',str(prefix)+'-run'],label+'-run',extra)
            assert result.stdout == expected and not result.stderr,(label,result,work)
            execute([compiler,'build',source,'--allocator',allocator,*limits,'--out',prefix],label+'-build',extra)
            result = execute([prefix.with_suffix('.exe' if os.name == 'nt' else '')],label+'-native',extra)
            assert result.stdout == expected and not result.stderr,(label,result,work)
            llvm = prefix.with_suffix('.ll').read_text(encoding='utf-8')
            calls = '\n'.join(line for line in llvm.splitlines() if 'call ' in line)
            assert ('@chali_host_object_new(' in calls) == (not fallback and not steps),(label,work)
            assert ('@chali_host_object_integer_set(' in calls) == (not fallback and not steps),(label,work)
            if fixture == 'aot_interface_copy_scalar' and not fallback and not steps:
                import re
                choose = re.search(r'^define[^\n]*program__choose\(.*?^\}',llvm,re.M|re.S)
                assert choose and '@chali_host_object_integer_set(' in choose[0],(label,work)
    print('object construction scalar: unit, 5 modes checked/unchecked, reentry and LLVM lowering OK;',work)


if __name__ == '__main__':
    main()
