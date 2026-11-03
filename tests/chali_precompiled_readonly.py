"""在实际禁止项目写入的 Windows DACL 下验证标准库内存装载及项目外产物。"""
from pathlib import Path
from contextlib import contextmanager
import argparse
import os
import shutil
import subprocess
import time
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


@contextmanager
def readonly_workspace():
    """进程和 DACL 已恢复后，有界等待 Windows 释放刚执行文件的删除占用。"""
    directory = tempfile.TemporaryDirectory(prefix='stdlib-readonly-')
    path = Path(directory.name)
    assert path.resolve().parent == (ROOT / 'tmp').resolve()
    try:
        yield path
    finally:
        deadline = time.monotonic() + 5
        retries = 0
        while True:
            try:
                directory.cleanup()
                if retries:
                    print(f'Windows executable cleanup completed after {retries} bounded retries')
                break
            except PermissionError:
                if time.monotonic() >= deadline:
                    raise
                retries += 1
                time.sleep(0.1)


def main():
    """限制仅属于本测试的临时项目，并在任何失败路径恢复原始访问权限。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    if os.name != 'nt':
        raise SystemExit('本专项使用 Windows DACL；其他系统需独立验证实际权限限制')
    with readonly_workspace() as work:
        project = work / 'project'
        project.mkdir()
        isolated = project / compiler.name
        shutil.copy2(compiler, isolated)
        source = project / 'main.chl'
        source.write_text('import std::parse;\nvoid main(){assert(std::parse::Parse::parseInt64("42").unwrap()==42);println("readonly project");}\n', encoding='utf-8')
        original = source.read_bytes()
        backup = work / 'permissions.json'
        script = ROOT / 'tests/fixtures/stdlib_readonly.ps1'
        command = [shutil.which('pwsh') or 'powershell', '-NoProfile', '-ExecutionPolicy', 'Bypass', '-File', str(script),
                   '-Project', str(project), '-Source', str(source), '-Backup', str(backup)]
        def permissions(mode):
            result = subprocess.run([*command, '-Mode', mode], capture_output=True, text=True,
                                    encoding='utf-8', errors='replace', timeout=30)
            assert result.returncode == 0, (mode, result.stdout, result.stderr)
        try:
            permissions('lock')
            denied = 0
            for target in (project / 'forbidden.cache', source):
                try:
                    with target.open('ab'):
                        pass
                except PermissionError:
                    denied += 1
            assert denied == 2, '必须实际拒绝项目新文件及源码写入，不能用只读属性代替权限验证'
            environment = dict(os.environ, CHALI_LANG='zh', CHALI_PYTHON='__missing_python__')
            environment.pop('CHALI_NO_STDLIB_CACHE', None)
            result = subprocess.run([str(isolated), 'run', str(source), '--out', str(work / 'outside-project'), '--stats'],
                                    cwd=project, env=environment, capture_output=True, text=True, encoding='utf-8', timeout=180)
            assert result.returncode == 0 and result.stdout == 'readonly project\n', result
            assert '标准模块二进制命中：std::parse' in result.stderr and '0 源码收集' in result.stderr, result.stderr
            assert source.read_bytes() == original
            assert sorted(p.name for p in project.iterdir()) == sorted((isolated.name, source.name))
            assert not list(work.rglob('*.unit')) and not list(work.rglob('chali-stdlib-cache'))
        finally:
            permissions('restore')
    print('Windows DACL: project creation/source writes denied, isolated binary assets and outside-project AOT passed; permissions restored')


if __name__ == '__main__':
    main()
