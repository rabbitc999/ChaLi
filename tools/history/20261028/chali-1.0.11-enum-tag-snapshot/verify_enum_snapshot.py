"""验证标签快照开关、分配器、检查模式与枚举样例。"""
import os
from pathlib import Path
import subprocess
import json
import re

root = Path.cwd()
work = root / 'tmp/enum-snapshot-check'
work.mkdir(parents=True, exist_ok=True)
compiler = root / 'chali-1.0.11-llvm-external-windows-x64.exe'
old = root / 'build/perf-baselines/a619614' / compiler.name
records = []
def invoke(binary, args, disabled=False):
    env = dict(os.environ, CHALI_LANG='zh', CHALI_NO_STDIN_INPUT='1')
    env.pop('CHALI_NO_ENUM_TAG_SNAPSHOT', None)
    if disabled:
        env['CHALI_NO_ENUM_TAG_SNAPSHOT'] = '1'
    result = subprocess.run([str(binary), *map(str,args)], cwd=root, env=env,
                            capture_output=True, text=True, encoding='utf-8', errors='replace', timeout=240)
    records.append(dict(command=[str(binary),*map(str,args)], disabled=disabled,
                        code=result.returncode, stdout=result.stdout, stderr=result.stderr))
    return result

fixture = root / 'tests/fixtures/aot_enum_tag_snapshot.chl'
for allocator in ('malloc','pool'):
    for disabled in (False,True):
        for checked in (False,True):
            out = work / f'fixture-{allocator}-{disabled}-{checked}.exe'
            args = ['aot',fixture,'--out',out,'--allocator',allocator,'--checked' if checked else '--unchecked']
            build = invoke(compiler,args,disabled)
            assert build.returncode == 0, records[-1]
            run = invoke(out,[])
            assert run.returncode == 0 and run.stdout == 'enum tag snapshot OK\n' and not run.stderr, records[-1]
            print(out.name, 'PASS',flush=True)

for name in ('enums','enum_payload_pointer','enum_drop_active_payload','nested_enum_ownership',
             'enum_methods','match_expression','result_match_loop'):
    source = root / 'example/samples' / (name+'.chl')
    expected = (root / 'example/expected' / (name+'.txt')).read_text(encoding='utf-8')
    for disabled in (False,True):
        output = work / f'{name}-{disabled}.exe'
        run = invoke(compiler,['run',source,'--out',output],disabled)
        assert run.returncode == 0 and run.stdout == expected and not run.stderr, records[-1]
    print(name,'PASS',flush=True)

for name in ('enum_match_duplicate','enum_match_nonexhaustive','enum_payload_count',
             'enum_payload_type','enum_unknown_variant','enum_pattern_expression'):
    source = root / 'example/samples-error' / (name+'.chl')
    before = invoke(old,['check',source,'json'])
    for disabled in (False,True):
        after = invoke(compiler,['check',source,'json'],disabled)
        assert before.returncode != 0 and after.returncode == before.returncode
        assert re.findall(r'CHL\d+',before.stdout+before.stderr) == re.findall(r'CHL\d+',after.stdout+after.stderr)
    print(name,'diagnostics PASS',flush=True)
(work/'results.json').write_text(json.dumps(records,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
print('ALL PASS',flush=True)
