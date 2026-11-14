"""构建可选 HTTP 流式传输扩展并部署与宿主动态库匹配的 ChaLi 模块。"""
from pathlib import Path
import argparse
import os
import shutil
import subprocess

from project_temp import tempfile
from native_cmake import generator_args

ROOT = Path(__file__).resolve().parents[1]


def main():
    """校验并构建锁定的 libcurl，生成平台对应的模块声明。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out', type=Path, default=ROOT / 'build/http_transfer')
    parser.add_argument('--archive', type=Path)
    parser.add_argument('--jobs', type=int, default=2)
    options = parser.parse_args()
    if options.jobs < 1:
        parser.error('--jobs 必须为正整数')
    source = ROOT / 'library/http_transfer'
    library = 'chali_http_transfer.dll' if os.name == 'nt' else 'chali_http_transfer.so'
    with tempfile.TemporaryDirectory(prefix='http-transfer-build-') as directory:
        work = Path(directory)
        args = ['cmake','-S',str(source),'-B',str(work/'build'),*generator_args(),'-DCMAKE_BUILD_TYPE=Release']
        if options.archive:
            args.append('-DCURL_ARCHIVE='+options.archive.resolve().as_posix())
        subprocess.run(args, check=True)
        subprocess.run(['cmake','--build',str(work/'build'),'--parallel',str(options.jobs)],check=True)
        stage = work/'install'
        subprocess.run(['cmake','--install',str(work/'build'),'--prefix',stage.as_posix()],check=True)
        template = (source/'http.chl.in').read_text(encoding='utf-8')
        module = stage/'std/net'
        module.mkdir(parents=True)
        shutil.move(stage/library, module/library)
        (module/'http.chl').write_text(template.replace('@LIBRARY@',library),encoding='utf-8')
        shutil.copytree(stage,options.out.resolve(),dirs_exist_ok=True)
    print(options.out.resolve()/'std/net'/library)


if __name__ == '__main__':
    main()
