"""用同一程序验证 AOT 按需链接、全标准库对照和真实旧版文件体积。"""
from pathlib import Path
import argparse
from datetime import datetime, timezone
import hashlib
import json
import os
import platform
import re
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools'))
from compiler_path import compiler_path
from project_temp import tempfile
from release import compiler_version


def digest(path):
    """按实际文件字节计算 SHA256。"""
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    """分别构建并实际运行，保留 LLVM 模块选择、PE 节区及完整诊断。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--baseline', type=Path)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    compiler = args.chali.resolve()
    work = Path(tempfile.mkdtemp(prefix='aot-size-'))
    report = dict(generated_utc=datetime.now(timezone.utc).isoformat(), host=platform.platform(),
                  work=str(work), configurations={}, cases={},
                  limitations=['Windows PE file sizes; runtime memory is not measured',
                               'default O3, full quad budget, reflection and safety checks',
                               'baseline uses its original compiler settings; same-version full/demand isolates module selection'])
    configurations = [('demand', compiler, False), ('full', compiler, True)]
    if args.baseline:
        configurations.append(('baseline', args.baseline.resolve(), False))
    for name, path, full in configurations:
        report['configurations'][name] = dict(path=str(path), version=compiler_version(path), sha256=digest(path), full_stdlib=full)
    cases = {
        'empty': ('void main() {}\n', ''),
        'print': ('void main() { println("AOT size OK"); }\n', 'AOT size OK\n'),
        'string': ('void main() { String text="hello"; assert(text.repeat(2)=="hellohello"); println("String OK"); }\n', 'String OK\n'),
        'list': ('void main() { List<int64> values; values.push(7); values.push(9); assert(values.pop()==9); assert(values[0]==7); println("List OK"); }\n', 'List OK\n'),
        'unused_import': ('import std::cas;\nimport std::sql;\nvoid main() { println("AOT size OK"); }\n', 'AOT size OK\n'),
        'cas': ('import std::cas;\nvoid main() { std::cas::Expr e=std::cas::Expr::parse("x+1"); assert(e.str().length()>0); println("CAS OK"); }\n', 'CAS OK\n'),
    }

    def save():
        """持续保留已完成结果。"""
        args.out.parent.mkdir(parents=True, exist_ok=True)
        args.out.write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')

    for case, (source, expected) in cases.items():
        path = work / (case + '.chl')
        path.write_text(source, encoding='utf-8')
        entry = dict(source=source, source_sha256=digest(path), expected_stdout=expected, results={})
        report['cases'][case] = entry
        for name, binary, full in configurations:
            env = dict(os.environ, CHALI_LLVM_CLANG=str(ROOT / 'build/dependencies/llvm/toolchain/bin/clang.exe') if os.name == 'nt' else os.environ.get('CHALI_LLVM_CLANG', 'clang'))
            env.pop('CHALI_AOT_FULL_STDLIB', None)
            if full:
                env['CHALI_AOT_FULL_STDLIB'] = '1'
            prefix = work / (case + '-' + name)
            result = subprocess.run([str(binary), 'aot', str(path), '--out', str(prefix)], cwd=ROOT,
                                    env=env, capture_output=True, text=True, encoding='utf-8', timeout=1200)
            (work / (case + '-' + name + '-build.log')).write_text(result.stdout + result.stderr, encoding='utf-8')
            assert result.returncode == 0 and not result.stderr, (case, name, result)
            product = Path(str(prefix) + ('.exe' if os.name == 'nt' else ''))
            result = subprocess.run([str(product)], cwd=ROOT, env=env, capture_output=True,
                                    text=True, encoding='utf-8', timeout=120)
            assert result.returncode == 0 and result.stdout == expected and not result.stderr, (case, name, result)
            section = subprocess.run(['objdump', '-h', str(product)], capture_output=True, text=True, encoding='utf-8', check=True).stdout
            (work / (case + '-' + name + '-sections.log')).write_text(section, encoding='utf-8')
            llvm = prefix.with_suffix('.ll')
            modules = []
            if llvm.exists():
                modules = re.findall(r'^; AOT standard modules: (.*)$', llvm.read_text(encoding='utf-8'), flags=re.M)
            entry['results'][name] = dict(bytes=product.stat().st_size, executable_sha256=digest(product),
                                         stdout=result.stdout, standard_modules=modules, sections=section)
            save()
            print(case, name, product.stat().st_size, flush=True)
    print('AOT size checks passed:', args.out, flush=True)


if __name__ == '__main__':
    main()
