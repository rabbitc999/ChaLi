"""调用阶段编译器生成标准库二进制，再转换为内嵌 C++ 字节数组。"""
from pathlib import Path
import argparse
import hashlib
import os
import subprocess
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    """只组织编译器构建，不生成 LLVM IR，也不在用户项目建立缓存。"""
    parser = argparse.ArgumentParser(description=__doc__)
    mode = parser.add_mutually_exclusive_group(required=True)
    mode.add_argument('--compiler', type=Path)
    mode.add_argument('--sync-embedded-source', type=Path)
    parser.add_argument('--output', type=Path, default=ROOT / 'source/chali_precompiled_data.inc')
    options = parser.parse_args()
    if options.sync_embedded_source is not None:
        source = options.sync_embedded_source.resolve()
        if source.name != 'chali_stdlib_embedded.cpp' or not source.is_file():
            raise ValueError('同步输入必须是已生成的 chali_stdlib_embedded.cpp')
        destination = ROOT / 'source/chali_stdlib_embedded.cpp'
        if destination.is_symlink():
            raise ValueError('生成源目标不能是符号链接')
        data = source.read_bytes()
        if destination.is_file() and destination.read_bytes() == data:
            return
        temporary = None
        try:
            with tempfile.NamedTemporaryFile(prefix='stdlib-source-', delete=False) as stream:
                temporary = Path(stream.name)
                stream.write(data)
            os.replace(temporary, destination)
        finally:
            if temporary is not None:
                temporary.unlink(missing_ok=True)
        print('已同步独立工程使用的内嵌标准库生成源')
        return
    compiler = options.compiler.resolve()
    with tempfile.TemporaryDirectory(prefix='stdlib-binary-') as folder:
        work = Path(folder)
        source = work / 'core.chl'
        source.write_text('void main() {}\n', encoding='utf-8')
        binary = work / 'core.unit'
        environment = dict(os.environ, CHALI_LANG='zh')
        result = subprocess.run([str(compiler), 'check', str(source), '--build-stdlib-unit', str(binary)],
                                cwd=work, env=environment, capture_output=True, timeout=180)
        if result.returncode:
            raise RuntimeError(result.stderr.decode('utf-8', errors='replace'))
        data = binary.read_bytes()
    lines = ['// 由 tools/build_stdlib_units.py 生成；从原始标准库维护。',
             '// 二进制 SHA256: ' + hashlib.sha256(data).hexdigest(),
             'constexpr unsigned char kStandardUnit[] = {']
    for at in range(0, len(data), 24):
        lines.append('    ' + ', '.join(f'0x{byte:02x}' for byte in data[at:at + 24]) + ',')
    checksum = 14695981039346656037
    for byte in data:
        checksum = ((checksum ^ byte) * 1099511628211) & ((1 << 64) - 1)
    lines += ['};', 'constexpr std::size_t kStandardUnitSize = sizeof(kStandardUnit);',
              f'constexpr std::uint64_t kStandardUnitChecksum = {checksum}ull;', '']
    output = '\n'.join(lines)
    options.output.parent.mkdir(parents=True, exist_ok=True)
    if not options.output.exists() or options.output.read_text(encoding='utf-8') != output:
        options.output.write_text(output, encoding='utf-8')
    print(f'标准库核心二进制：{len(data)} 字节；SHA256 {hashlib.sha256(data).hexdigest()}')


if __name__ == '__main__':
    main()
