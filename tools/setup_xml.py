"""构建可选 XML 扩展并部署与宿主动态库匹配的 ChaLi 模块。"""
from pathlib import Path
import argparse
import os
import shutil
import subprocess

from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    """校验并构建锁定的 Expat，生成平台对应的模块声明。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out', type=Path, default=ROOT / 'build/xml')
    parser.add_argument('--archive', type=Path)
    parser.add_argument('--jobs', type=int, default=2)
    options = parser.parse_args()
    if options.jobs < 1:
        parser.error('--jobs 必须为正整数')
    source = ROOT / 'library/xml'
    library = 'chali_xml.dll' if os.name == 'nt' else 'chali_xml.so'
    with tempfile.TemporaryDirectory(prefix='xml-build-') as directory:
        work = Path(directory)
        args = ['cmake','-S',str(source),'-B',str(work/'build'),'-G','Ninja','-DCMAKE_BUILD_TYPE=Release']
        if options.archive:
            args.append('-DEXPAT_ARCHIVE='+options.archive.resolve().as_posix())
        subprocess.run(args, check=True)
        subprocess.run(['cmake','--build',str(work/'build'),'--parallel',str(options.jobs)],check=True)
        stage = work/'install'
        subprocess.run(['cmake','--install',str(work/'build'),'--prefix',stage.as_posix()],check=True)
        template = (source/'xml.chl.in').read_text(encoding='utf-8')
        (stage/'xml.chl').write_text(template.replace('@LIBRARY@',library),encoding='utf-8')
        shutil.copytree(stage,options.out.resolve(),dirs_exist_ok=True)
    print(options.out.resolve()/library)


if __name__ == '__main__':
    main()
