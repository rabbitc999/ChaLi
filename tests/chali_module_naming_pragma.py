"""核对命名关闭指令、导入边界和增量复用。"""
import argparse
import json
import subprocess
from pathlib import Path
from compiler_path import compiler_path
from project_temp import tempfile

def main():
    """通过真实编译器检查命名关闭指令和缓存边界。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path)
    parser.add_argument('compiler', type=Path, nargs='?')
    options = parser.parse_args()
    compiler = (options.chali or options.compiler or compiler_path()).resolve()
    with tempfile.TemporaryDirectory(prefix='chali-naming-pragma-') as directory:
        folder = Path(directory)
        source = folder / 'main.chl'
        (folder / 'Legacy.chl').write_text('module Legacy;\nint64 read(){return 1;}\n', encoding='utf-8')
        source.write_text('#NoNamingCheck // 使用这个指令\nmodule LocalBad;\nimport Legacy;\n'
                          'void main(){int64 unused_local=0;println("{}",read());}\n', encoding='utf-8')
        arguments = [str(compiler), 'check', str(source), '--warning-profile=editor',
                     '--warn=unused-local', '--diagnostics=json', '--incremental',
                     '--cache-dir', str(folder / 'cache')]
        cache = folder / "cache"
        cache.mkdir()
        written = {}
        for attempt in range(2):
            run = subprocess.run(arguments, cwd=folder, capture_output=True, encoding='utf-8', timeout=120)
            assert run.returncode == 0, run
            diagnostics = json.loads(run.stdout)['diagnostics']
            names = [item for item in diagnostics if item['code'] == 'CHL2103']
            assert len(names) == 1 and 'Legacy' in names[0]['message'] and 'LocalBad' not in names[0]['message'], diagnostics
            assert any(item['code'] == 'CHL2201' for item in diagnostics), diagnostics
            units = {path.name: path.stat().st_mtime_ns for path in cache.glob('*.unit')}
            assert len(units) >= 2, units
            if attempt == 0:
                written = units
            else:
                assert units == written, (written, units)
        text = source.read_text(encoding='utf-8')
        source.write_text(text.split('\n', 1)[1], encoding='utf-8')
        run = subprocess.run(arguments, cwd=folder, capture_output=True, encoding='utf-8', timeout=120)
        assert run.returncode == 0, run
        names = [item['message'] for item in json.loads(run.stdout)['diagnostics'] if item['code'] == 'CHL2103']
        assert len(names) == 2 and any('LocalBad' in message for message in names), names

        print('命名指令：仅当前文件、其他警告、增量缓存复用通过')


if __name__ == "__main__":
    main()
