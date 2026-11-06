"""在正式采样前后核对全部被测程序、脚本和共享 Java/Go 构件。"""
from pathlib import Path
import argparse
import hashlib
import json
import subprocess
import sys


def digest(path):
    """读取真实文件内容。"""
    with path.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()


def inventory(report):
    """汇总内核、计时壳、解释器、Java class、Go 可执行文件和源码。"""
    root=Path(__file__).resolve().parents[2]
    paths={Path(x['path']) for x in report['tools'].values()}
    for name in report['source_sha256']:
        if digest(root/name)!=report['source_sha256'][name]:raise ValueError(('Source changed after build',name))
        paths.add(root/name)
    for row in report['builds'].values():
        paths.update(Path(p) for p in row['artifacts'])
        command=row['command']
        if '-cp' in command:
            paths.update(Path(command[command.index('-cp')+1]).rglob('*.class'))
        elif '/go/' in str(row['folder']).replace('\\','/'):
            paths.add(Path(command[0]))
    return {str(p):digest(p) for p in sorted(paths) if p.is_file()}


def main():
    """保存首次身份快照，或比较采样结束后的同一批文件。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('report',type=Path)
    parser.add_argument('--after',action='store_true')
    args=parser.parse_args()
    report=json.loads(args.report.read_text(encoding='utf-8'))
    target=args.report.with_name('identity-audit.json')
    files=inventory(report)
    if args.after:
        data=json.loads(target.read_text(encoding='utf-8'))
        if data['files']!=files:raise ValueError('Measured files changed during the run')
        data['after_measurement_identical']=True
        target.write_text(json.dumps(data,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
        print('Identity audit passed:',len(files),'files unchanged')
    else:
        if target.exists():raise FileExistsError(target)
        rust=subprocess.run(['rustup','which','rustc'],capture_output=True,text=True,encoding='utf-8')
        rust_path=Path(rust.stdout.strip()) if rust.returncode==0 else None
        data=dict(files=files,audit_script_sha256=digest(Path(__file__)),
                  rust_compiler_path=str(rust_path) if rust_path else None,
                  rust_compiler_sha256=digest(rust_path) if rust_path and rust_path.is_file() else None,
                  after_measurement_identical=False)
        target.write_text(json.dumps(data,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
        print('Identity snapshot saved:',len(files),'files')


if __name__=='__main__':main()
