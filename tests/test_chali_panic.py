"""真实 CHALI 嵌套 panic 与线程 panic 诊断回归（1.0.8 单一路径：没有符号压缩开关）。"""
import argparse
import os
from pathlib import Path
import subprocess
from project_temp import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', default='chali')
    args = parser.parse_args()
    compiler = Path(args.chali).resolve()
    if not compiler.exists():
        compiler = compiler.with_suffix('.exe')
    with tempfile.TemporaryDirectory(prefix='chali-panic-') as folder:
        directory = Path(folder)
        source = directory / 'nested.chl'
        source.write_text('int64 inner() { std::lang::panic("nested failure"); return 0; }\n'
                          'int64 outer() { return inner(); }\nvoid main() { let __boom = outer(); }\n',
                          encoding='utf-8')

        def command(items):
            result = subprocess.run([str(x) for x in items], cwd=directory, capture_output=True,
                                    env=dict(os.environ, CHALI_LANG='zh'),
                                    timeout=15 if len(items) > 1 and items[1] == 'run' else 180)
            return (result, result.stderr.decode('utf-8', errors='replace'))

        result, stderr = command([compiler, 'run', source, '--out', directory / 'plain'])
        assert result.returncode != 0 and 'nested failure' in stderr
        assert stderr.splitlines()[0].startswith('chali aot: panic：'), stderr
        # 追踪始终带源码位置与调用栈：崩溃诊断不再受产物打包方式影响。
        assert '调用栈' in stderr and 'inner' in stderr, stderr

        threaded = directory / 'threaded.chl'
        threaded.write_text('import std::thread;\nvoid spin() { while (true) {} }\n'
                            'void blocked() { std::lang::String x = std::thread::receive(); }\n'
                            'void leaf() { std::lang::panic("worker failure"); }\n'
                            'void crash() { leaf(); }\n'
                            'void main() { Thread a = std::thread::create(spin); Thread b = std::thread::create(blocked);'
                            ' Thread c = std::thread::create(crash); while (true) {} return; }\n', encoding='utf-8')
        result, stderr = command([compiler, 'run', threaded])
        assert result.returncode != 0 and 'worker failure' in stderr, stderr
        assert '调用栈' in stderr and 'leaf' in stderr, stderr

        for mode, body in (('mainpanic', 'Thread a = std::thread::create(spin); std::lang::panic("main failure"); return;'),
                           ('autojoin', 'Thread a = std::thread::create(spin); Thread c = std::thread::create(crash); while (std::thread::state(c) == std::thread::RUNNING) { std::thread::sleepFor(1); } return;'),
                           ('wait', 'Thread a = std::thread::create(spin); Thread c = std::thread::create(crash); std::thread::wait(c); return;'),
                           ('receive', 'Thread a = std::thread::create(spin); Thread c = std::thread::create(crash); std::lang::String x = std::thread::receive(); return;')):
            threaded.write_text('import std::thread;\nvoid spin() { while (true) {} }\n'
                                'void leaf() { std::lang::panic("worker failure"); }\nvoid crash() { leaf(); }\n'
                                'void main() { ' + body + ' }\n', encoding='utf-8')
            result, stderr = command([compiler, 'run', threaded])
            assert result.returncode != 0 and ('main failure' if mode == 'mainpanic' else 'worker failure') in stderr, stderr


if __name__ == '__main__':
    main()
