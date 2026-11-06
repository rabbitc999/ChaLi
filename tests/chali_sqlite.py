"""验证 SQLite 真实数据库在原生 checked／unchecked 的行为；只写隔离临时目录。"""
from pathlib import Path
from contextlib import closing
import argparse
import os
import sqlite3
import shutil
import subprocess
from project_temp import tempfile
from chali_glue_edges import ROOT, CHALI, EXE, run

def main():
    global CHALI
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=CHALI)
    CHALI=parser.parse_args().chali.resolve()
    work = Path(tempfile.mkdtemp(prefix='sqlite-c-api-'))
    work.mkdir(parents=True, exist_ok=True)
    os.environ['SQLITE_LIBRARY'] = str(work / 'missing-sqlite3.dll')
    source = ROOT / 'doc/examples/sqlite/demo.chl'
    expected = 'sqlite database checks passed\n'
    with tempfile.TemporaryDirectory(prefix='database-', dir=work) as temporary:
        directory = Path(temporary)
        for backend in ('run', 'build'):
            prefix = work / ('sqlite-' + backend)
            database = directory / (backend + '.db')
            if backend == 'run':
                result = run(CHALI, 'run', source, '--out', prefix, '--', database)
            else:
                run(CHALI, 'build', source, '--out', prefix)
                result = run(str(prefix) + EXE, database)
            assert result.stdout == expected and (not result.stderr), (backend, result.stdout, result.stderr)
        for path in directory.glob('*.db'):
            with closing(sqlite3.connect(path)) as connection:
                assert connection.execute('SELECT value FROM persisted').fetchone() == ('持久化',)
    print('SQLite checked/unchecked static database checks passed')
if __name__ == '__main__':
    main()
