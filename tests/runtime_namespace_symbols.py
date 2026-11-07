"""核对实际构建对象中的 C++ 运行时符号已迁移到 runtime。"""
import argparse
import os
from pathlib import Path
import re
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def main():
    """同时检查定义与未定义引用，防止旧命名空间残留在静态对象中。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--build-root', type=Path, default=Path(os.environ.get('CHALI_BUILD_ROOT', 'build')))
    parser.add_argument('--nm', default=os.environ.get('NM', 'nm'))
    args = parser.parse_args()
    build = args.build_root.resolve()
    artifacts = [build/'chali-ircode-aot-core/libstackvm.a',
                 build/'chali-frontend/libparser.a',
                 build/'chali-host-records/librecords.a',
                 build/'chali-aot-host/native_runtime.o',
                 build/'chali-frontend/chali_aot.o']
    count = 0
    all_symbols = []
    for artifact in artifacts:
        assert artifact.is_file(), artifact
        result = subprocess.run([args.nm, '--demangle', str(artifact)], cwd=ROOT,
                                capture_output=True, text=True, encoding='utf-8', errors='replace', timeout=180)
        assert result.returncode == 0, (artifact, result.stderr)
        all_symbols.append(result.stdout)
        stale = [line for line in result.stdout.splitlines() if re.search(r'\b(?:stackvm::|chali_aot::|runtime::aot::|ChaliAot|M_Aot|g_aotSyntax|kChaliAot|AotCType|aotModulesFor|chali_aot_thread_services)', line)]
        assert not stale, (artifact, stale[:10])
        current = sum(bool(re.search(r'\bruntime::', line)) for line in result.stdout.splitlines())
        assert current > 0, artifact
        count += current
        print(f'{artifact.name}: {current} 个 runtime 限定符号，无旧运行时或构建符号')
    symbols = '\n'.join(all_symbols)
    for required in ('chali_build::', 'runtime::build::', 'chali_build_thread_services'):
        assert required in symbols, required
    print(f'运行时符号表迁移通过：5 个构件，{count} 个限定符号（含未定义引用）')


if __name__ == '__main__':
    main()
