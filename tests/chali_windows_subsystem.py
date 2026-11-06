"""Verify Windows PE application subsystems and project option precedence."""

import argparse
import os
from pathlib import Path
import struct
import subprocess

from compiler_path import compiler_path
from project_temp import tempfile


def subsystem(path):
    """Read the subsystem field of the generated x64 PE executable."""
    data = path.read_bytes()
    assert data[:2] == b'MZ', path
    pe, = struct.unpack_from('<I', data, 0x3c)
    assert data[pe:pe + 6] == b'PE\0\0\x64\x86', path
    assert struct.unpack_from('<H', data, pe + 24)[0] == 0x20b, path
    return struct.unpack_from('<H', data, pe + 24 + 68)[0]


def main():
    """Run Windows application exports, a separate link, and invalid options."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    args = parser.parse_args()
    if os.name != 'nt':
        print('Windows PE subsystem acceptance requires Windows.')
        return
    compiler = args.chali.resolve()
    with tempfile.TemporaryDirectory(prefix='windows-subsystem-') as directory:
        work = Path(directory)
        source = work / 'app.chl'
        source.write_text('void main() {}\n', encoding='utf-8')

        def chali(*arguments, error=None):
            """Invoke the selected compiler and verify success or a diagnostic."""
            result = subprocess.run([compiler, *map(str, arguments)], cwd=work, capture_output=True,
                                    encoding='utf-8', errors='replace', timeout=180)
            if error is None:
                assert result.returncode == 0, (arguments, result)
            else:
                assert result.returncode != 0 and error in result.stderr, (arguments, result)

        chali('aot', source, '--out', work / 'default')
        assert subsystem(work / 'default.exe') == 3
        chali('aot', source, '--subsystem', 'windows', '--out', work / 'gui')
        assert subsystem(work / 'gui.exe') == 2
        subprocess.run([work / 'gui.exe'], cwd=work, check=True, timeout=15)
        chali('aot', source, '--emit-obj', '--out', work / 'object')
        chali('link', work / 'object.o', '--subsystem', 'windows', '--out', work / 'linked.exe')
        assert subsystem(work / 'linked.exe') == 2
        chali('aot', source, '--subsystem', 'bad', error='--subsystem')
        chali('aot', source, '--subsystem', 'windows', '--shared', error='--subsystem')
        chali('aot', source, '--subsystem', 'windows', '--target', 'linux-x64', error='--subsystem')
        (work / 'chali.toml').write_text('[chali]\nmain = "app.chl"\n[build]\nsubsystem = "windows"\n', encoding='utf-8')
        chali('aot', source, '--out', work / 'project')
        assert subsystem(work / 'project.exe') == 2
        chali('aot', source, '--subsystem', 'console', '--out', work / 'override')
        assert subsystem(work / 'override.exe') == 3
        chali('run', source)
    print('Windows PE default, GUI, standalone link, project configuration and console override passed.')


if __name__ == '__main__':
    main()
