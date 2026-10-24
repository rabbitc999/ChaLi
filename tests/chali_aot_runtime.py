"""比较共享标准库、闭包和隔离线程在 VM 与 LLVM 原生程序中的结果。"""
from compiler_path import compiler_path
import argparse
import os
from pathlib import Path
import subprocess
from project_temp import tempfile
ROOT = Path(__file__).resolve().parents[1]
SAMPLES = [
    'lambda_callbacks', 'lambda_capture_modes', 'std_errors', 'std_structured_data',
    'reflect_serialization', 'std_graph', 'std_tree', 'std_binary', 'std_encoding',
    'std_utf8', 'std_csv', 'std_datetime', 'std_collections_algorithms',
    'thread_messages', 'thread_channel', 'thread_pool', 'event_timer',
    'std_algorithms_extra', 'std_cli', 'std_complex_fft', 'std_compress', 'std_image',
    'std_io_extended', 'std_lang', 'std_linalg_solve_glue', 'std_linkedlist',
    'std_math_clock_random', 'std_net', 'std_numerics_number_theory',
    'std_parse_random_path_memory', 'std_process_probe', 'std_red_black_delete',
    'std_string_builder', 'std_text_kmp', 'watchdog_timeout',
]


def main():
    """复用已维护的语言样例，逐个核对编译、运行、输出和错误。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--samples', nargs='+', choices=SAMPLES, default=SAMPLES)
    parser.add_argument('--traps-only', action='store_true', help='只复验运行期陷阱与输出时机')
    options = parser.parse_args()
    compiler = options.chali.resolve()
    env = dict(os.environ, CHALI_LANG='zh', CHALI_NO_STDIN_INPUT='1')
    with tempfile.TemporaryDirectory(prefix='chali-aot-runtime-') as directory:
        work = Path(directory)

        def execute(command, input_value=None, cwd=None):
            """限制各次编译和运行时间，捕获诊断以定位真实能力缺口。"""
            return subprocess.run(list(map(str, command)), cwd=cwd or work, env=env, capture_output=True,
                                  text=True, encoding='utf-8', errors='replace', input=input_value, timeout=300)

        for name in ([] if options.traps_only else options.samples):
            isolated = work / name
            virtual = isolated / 'vm'
            native = isolated / 'native'
            virtual.mkdir(parents=True)
            native.mkdir()
            sample = ROOT / 'example/samples' / (name + '.chl')
            prefix = isolated / name
            expected = ROOT / 'example/expected' / name
            arguments = expected.with_suffix('.args').read_text(encoding='utf-8').splitlines() if expected.with_suffix('.args').exists() else []
            input_text = expected.with_suffix('.in').read_text(encoding='utf-8') if expected.with_suffix('.in').exists() else ''
            flags = ['--threads'] if name.startswith(('thread_', 'event_')) else []

            reference = execute([compiler, 'run', sample, '--out', str(prefix) + '-vm', *flags, '--', *arguments], input_text, cwd=virtual)
            assert reference.returncode == 0, (name, 'VM', reference)
            built = execute([compiler, 'aot', sample, '--out', prefix, *flags], cwd=native)
            assert built.returncode == 0, (name, 'AOT 编译', built)
            product = prefix.with_suffix('.exe' if os.name == 'nt' else '')
            result = execute([product, *arguments], input_text, cwd=native)
            assert (result.returncode, result.stdout) == (reference.returncode, reference.stdout), (name, result, reference)
            assert not result.stderr, (name, result.stderr)
            print('VM/AOT 运行时差分通过：' + name, flush=True)
        for name, expression, diagnostic in [
            ('integer_zero', 'integer(1) / integer(0)', '除零'),
            ('integer_overflow', 'integer(-9223372036854775807 - 1) / integer(-1)', '溢出'),
            ('shift_range', 'integer(1) << integer(64)', '移位'),
            ('real_zero', 'real(1.0) / real(0.0)', '除零'),
        ]:
            source = work / (name + '.chl')
            source.write_text('int64 integer(int64 value) { return value; }\n'
                              'float64 real(float64 value) { return value; }\n'
                              'void main() { println("before trap"); println("{}", ' + expression +
                              '); println("after trap"); }\n', encoding='utf-8')
            prefix = work / name
            reference = execute([compiler, 'run', source, '--out', str(prefix) + '-vm'])
            assert reference.returncode != 0 and reference.stdout == 'before trap\n' and diagnostic in reference.stderr, reference
            built = execute([compiler, 'aot', source, '--out', prefix])
            assert built.returncode == 0, built
            result = execute([prefix.with_suffix('.exe' if os.name == 'nt' else '')])
            assert result.returncode != 0 and result.stdout == reference.stdout and diagnostic in result.stderr, result
            print('VM/AOT 陷阱与输出时机通过：' + name, flush=True)
        for name, source_text, expected, diagnostic in [
            ('watchdog_expired', 'import std::watchdog;\nvoid main() { println("before stop"); std::watchdog::enableTimeout(10000); while (true) {} }', 'before stop\n', '软重启'),
            ('watchdog_requested', 'import std::watchdog;\nvoid main() { std::watchdog::enable(0); println("before stop"); std::watchdog::requestRestart(); println("after stop"); }', 'before stop\n', '软重启'),
        ]:
            source = work / (name + '.chl')
            source.write_text(source_text, encoding='utf-8')
            prefix = work / name
            flags = ['--threads'] if name == 'child_exit' else []
            reference = execute([compiler, 'run', source, '--out', str(prefix) + '-vm', *flags])
            assert reference.stdout == expected and diagnostic in reference.stderr, reference
            built = execute([compiler, 'aot', source, '--out', prefix, *flags])
            assert built.returncode == 0, built
            result = execute([prefix.with_suffix('.exe' if os.name == 'nt' else '')])
            assert result.returncode == reference.returncode and result.stdout == reference.stdout and diagnostic in result.stderr, (result, reference)
            print('VM/AOT 执行控制通过：' + name, flush=True)


if __name__ == '__main__':
    main()
