"""核对有界几何增长、List 保留容量与过大需求诊断。"""
from compiler_path import compiler_path
from project_temp import tempfile
from pathlib import Path
import argparse
import json
import os
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def main():
    """在隔离产物中核对分配器、检查模式和原语开关并保留原始输出。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    options = parser.parse_args()
    compiler = options.chali.resolve()
    work = Path(tempfile.mkdtemp(prefix='buffer-growth-'))
    results = []

    def execute(command, label, environment):
        """保存命令、退出状态与原始输出，便于归档诊断。"""
        result = subprocess.run(list(map(str, command)), cwd=ROOT, env=environment,
                                capture_output=True, text=True, encoding='utf-8', timeout=600)
        results.append(dict(label=label, command=list(map(str, command)), code=result.returncode,
                            stdout=result.stdout, stderr=result.stderr))
        (work / 'results.json').write_text(json.dumps(results, ensure_ascii=False, indent=2), encoding='utf-8')
        return result

    environment = dict(os.environ)
    for allocator in ('malloc', 'pool'):
        for mode in ('--checked', '--unchecked'):
            for fallback in (False, True):
                env = dict(environment)
                if fallback:
                    env['CHALI_NO_AOT_RAW_INTRINSICS'] = '1'
                label = allocator + mode + ('-fallback' if fallback else '-direct')
                prefix = work / label
                built = execute([compiler, 'aot', ROOT / 'tests/fixtures/aot_buffer_growth.chl',
                                 '--allocator', allocator, mode, '--out', prefix], label + '-build', env)
                assert built.returncode == 0, (label, built.stdout, built.stderr, work)
                ran = execute([prefix.with_suffix('.exe' if os.name == 'nt' else '')], label, env)
                assert ran.returncode == 0 and 'bounded buffer growth and retained List capacity OK' in ran.stdout, (label, ran, work)
    for name, arguments, diagnostic in (
        ('overflow', '9223372036854775806,2,9223372036854775806,0,9223372036854775807', '缓冲区容量超出上限'),
        ('over-max', '7,4,8,0,10', '缓冲区容量超出上限'),
        ('negative', '0,-1,0,0,10', '缓冲区容量参数无效'),
        ('bad-capacity', '9,0,8,0,10', '缓冲区容量参数无效'),
        ('bad-minimum', '0,1,0,11,10', '缓冲区容量参数无效'),
    ):
        source = work / (name + '.chl')
        source.write_text('void main(){std::lang::__bootBufferCapacity(' + arguments + ');}\n', encoding='utf-8')
        prefix = work / name
        built = execute([compiler, 'aot', source, '--out', prefix], name + '-build', environment)
        assert built.returncode == 0, (name, built.stdout, built.stderr, work)
        ran = execute([prefix.with_suffix('.exe' if os.name == 'nt' else '')], name, environment)
        assert ran.returncode != 0 and diagnostic in ran.stdout + ran.stderr, (name, ran, work)
    print('buffer growth: 8 configurations and 5 capacity diagnostics OK; evidence:', work)


if __name__ == '__main__':
    main()
