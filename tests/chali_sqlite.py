"""验证 SQLite 真实数据库在解释器、VM 的行为；只写隔离临时目录。"""
from pathlib import Path
from contextlib import closing
import os
import sqlite3
import shutil
import subprocess
from project_temp import tempfile
from chali_glue_edges import ROOT, CHALI, EXE, run

def main():
    work = ROOT / 'tmp/chali-sqlite'
    work.mkdir(parents=True, exist_ok=True)
    os.environ['SQLITE_LIBRARY'] = str(work / 'missing-sqlite3.dll')
    source = ROOT / 'doc/examples/sqlite/demo.chl'
    expected = 'sqlite database checks passed\n'
    with tempfile.TemporaryDirectory(prefix='database-', dir=work) as temporary:
        directory = Path(temporary)
        for backend in ('run', 'aot'):
            prefix = work / ('sqlite-' + backend)
            database = directory / (backend + '.db')
            if backend == 'run':
                result = run(CHALI, 'run', source, '--out', prefix, '--', database)
            else:
                run(CHALI, 'aot', source, '--out', prefix)
                result = run(str(prefix) + EXE, database)
            assert result.stdout == expected and (not result.stderr), (backend, result.stdout, result.stderr)
        for path in directory.glob('*.db'):
            with closing(sqlite3.connect(path)) as connection:
                assert connection.execute('SELECT value FROM persisted').fetchone() == ('持久化',)
    print('SQLite VM/AOT static database checks passed')
if __name__ == '__main__':
    main()
