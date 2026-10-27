"""验证默认 String 自举的值语义、边界诊断及 VM/AOT 一致性。"""
from compiler_path import compiler_path
from project_temp import tempfile
from pathlib import Path
import argparse
import json
import os
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def main():
    """保存每次编译、运行及 LLVM 检查的证据，任何不一致立即失败。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    options = parser.parse_args()
    compiler = options.chali.resolve()
    work = Path(tempfile.mkdtemp(prefix='bootstrap-string-'))
    env = dict(os.environ)
    env.setdefault('CHALI_LLVM_CLANG', str(ROOT / 'build/dependencies/llvm/toolchain/bin/clang.exe') if os.name == 'nt' else 'clang')

    def execute(command, label):
        """执行一个步骤，将原始输出与状态写入本次验证目录。"""
        result = subprocess.run(list(map(str, command)), cwd=ROOT, env=env,
                                capture_output=True, text=True, encoding='utf-8', timeout=600)
        (work / (label + '.log')).write_text(result.stdout + result.stderr, encoding='utf-8')
        return result

    cases = [('values', ROOT / 'tests/fixtures/bootstrap_string_default.chl', None)]
    for name, statement, diagnostic in [
        ('negative-repeat', '"abc".repeat(-1);', '重复次数不能为负'),
        ('negative-pad', '"abc".padStart(-1);', '填充长度不能为负'),
        ('negative-slice', 'String value="abc"[-1:2];', '切片端点不能为负数'),
        ('invalid-lead', 'String value="abcd"; value[0]=(char)192; value.charLength();', '非法 UTF-8 起始字节'),
        ('invalid-continuation', 'String value="abcd"; value[0]=(char)194; value.charLength();', '非法 UTF-8 延续字节'),
        ('truncated', 'String value="a"; value[0]=(char)240; value.unicodeChars();', '不完整的 UTF-8 字符'),
        ('surrogate', 'String value="abc"; value[0]=(char)237; value[1]=(char)160; value[2]=(char)128; value.reverse();', '非法 Unicode 码点'),
    ]:
        source = work / (name + '.chl')
        source.write_text('void main() { ' + statement + ' }\n', encoding='utf-8')
        cases.append((name, source, diagnostic))
    results = []
    for name, source, diagnostic in cases:
        virtual = execute([compiler, 'run', source, '--out', work / (name + '-vm')], name + '-vm')
        prefix = work / (name + '-aot')
        built = execute([compiler, 'aot', source, '--out', prefix], name + '-build')
        assert built.returncode == 0, (name, built.stdout, built.stderr, work)
        native = execute([prefix.with_suffix('.exe' if os.name == 'nt' else '')], name + '-aot')
        if diagnostic:
            assert virtual.returncode != 0 and diagnostic in virtual.stdout + virtual.stderr, (name, virtual, work)
            assert native.returncode != 0 and diagnostic in native.stdout + native.stderr, (name, native, work)
        else:
            assert (virtual.returncode, virtual.stdout, virtual.stderr) == (0, 'bootstrap-string-default-ok\n', ''), (virtual, work)
            assert (native.returncode, native.stdout, native.stderr) == (0, virtual.stdout, ''), (native, work)
            llvm = prefix.with_suffix('.ll').read_text(encoding='utf-8')
            for obsolete in ['__std_str_lit', '__std_str_join', '__std_str_sub', '__std_str_cmp', '__std_str_pos',
                             'std::lang::__stringNew', 'std::lang::__stringCopy', 'std::lang::__stringDrop']:
                assert obsolete not in llvm, (obsolete, work)
            assert 'llvm.memcpy' in llvm, work
        results.append(dict(case=name, vm='passed', aot='passed'))
        print(name, 'VM/AOT passed', flush=True)
    (work / 'results.json').write_text(json.dumps(results, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    print('default String bootstrap checks passed; logs:', work)


if __name__ == '__main__':
    main()
