"""校验 Lua 严格更快及损坏样本拒绝，不运行性能负载。"""
from pathlib import Path
from project_temp import tempfile
import subprocess
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'benchmarks'))
from evaluate_saved import samples_and_median, verdict


class PerformanceGateTests(unittest.TestCase):
    """以边界值和坏样本检验实际验收入口，避免靠舍入值或均值冒充达标。"""

    def test_cpp_is_reference_only(self):
        """即使远慢于 C++，只要快于 Lua 就通过；C++ 更快不能抵消慢于 Lua。"""
        self.assertTrue(verdict(1000, 1, 1001)['passed'])
        self.assertFalse(verdict(1, 1000, 0.5)['passed'])
        self.assertIsNone(verdict(1, 1000, None)['passed'])

    def test_lua_tie_and_missing_reference(self):
        """与 Lua 相同不算更快；无参考明确保留 None。"""
        self.assertFalse(verdict(110, 100, 110)['passed'])
        self.assertIsNone(verdict(110, 100, None)['lua_pass'])

    def test_interface_limits_boundary(self):
        """单态 40 ms、多态 400 ms 含等号通过；超限或慢于 Lua 仍拒绝。"""
        for case, limit in (('interface_mono', 40), ('interface_poly', 400)):
            self.assertTrue(verdict(limit - 0.000001, 0.1, None, case)['passed'])
            self.assertTrue(verdict(limit, 0.1, None, case)['passed'])
            self.assertFalse(verdict(limit + 0.000001, 0.1, None, case)['passed'])
            self.assertFalse(verdict(limit - 1, 0.1, limit - 2, case)['passed'])

    def test_bad_durations(self):
        """负数、无穷和 NaN 不得混入门槛运算。"""
        for invalid in (0, -1, True, float('inf'), float('nan')):
            with self.subTest(invalid=invalid), self.assertRaises(ValueError):
                verdict(invalid, 100, None)

    def test_recompute_median(self):
        """偏态样本以中位数判定，并拒绝与原始样本不符的报告值。"""
        cell = {'samples_ns': {'array': [1_000_000] * 13 + [4_000_000] * 12},
                'median_ms': {'array': 1.0}}
        report = {'configs': {'chali-aot': cell}}
        self.assertEqual(samples_and_median(report, 'array'), (1.0, 1.0, 25))
        cell['median_ms']['array'] = 0.5
        with self.assertRaises(ValueError):
            samples_and_median(report, 'array')

    def test_short_or_corrupt_samples(self):
        """不满正式样本数或混入非时间数据时拒绝。"""
        for samples in ([1_000_000] * 24, [1_000_000] * 24 + [True], [1_000_000] * 24 + [float('nan')]):
            with self.subTest(samples=samples), self.assertRaises(ValueError):
                samples_and_median({'configs': {'chali-aot': {
                    'samples_ns': {'array': samples}, 'median_ms': {'array': 1.0}}}}, 'array')

    def test_failed_process(self):
        """进程带 stderr 时不接纳其中看似正常的计时。"""
        cell = {'processes': [{'samples_ns': [1_000_000] * 25, 'stderr': 'unexpected'}], 'median_ms': 1.0}
        with self.assertRaises(ValueError):
            samples_and_median({'configurations': {'enum_payload': {'chali-aot': cell}}}, 'enum_payload')

    def test_cli_rejects_inconsistent_inputs(self):
        """缺项、重复、混合编译器和不匹配二进制均不得产生有效验收结果。"""
        root = Path(__file__).resolve().parents[1]
        reports = [root / 'benchmarks/results' / name / 'results.json' for name in (
            '1.0.11-multi-address-basic', '1.0.11-multi-address-features', '1.0.11-multi-address-strings')]
        with tempfile.TemporaryDirectory(prefix='performance-gate-rejections-') as temporary:
            work = Path(temporary)
            fake = work / 'compiler.bin'
            fake.write_bytes(b'not a compiler')
            cases = [
                ('missing', reports[:1], [], '缺少负载'),
                ('duplicate', [reports[0], reports[0]], [], '重复负载'),
                ('binary', reports, ['--compiler', fake], '编译器与测量报告'),
                ('mixed', [reports[0], root / 'benchmarks/results/1.0.11-feature-baseline/results.json', reports[2]], [], '同一实际编译器'),
            ]
            for name, sources, extra, message in cases:
                with self.subTest(name=name):
                    output = work / name
                    process = subprocess.run(list(map(str, [sys.executable, root / 'benchmarks/evaluate_saved.py',
                        '--reports', *sources, *extra, '--out', output])), cwd=root,
                        capture_output=True, text=True, encoding='utf-8', timeout=30)
                    self.assertEqual(process.returncode, 2, process.stderr)
                    self.assertIn(message, process.stderr)
                    self.assertFalse(output.exists())


if __name__ == '__main__':
    unittest.main()
