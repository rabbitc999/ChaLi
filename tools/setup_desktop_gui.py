"""下载并构建可选桌面宿主，输出 ChaLi 模块与匹配动态库。"""
from pathlib import Path, PurePosixPath
import argparse
import hashlib
import os
import re
import shutil
import subprocess
import sys
import urllib.request
import zipfile

from fetch_dependencies import download_lock
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
DEPENDENCIES = {
    'raylib': ('5.5', '00e8a7c85f7a5221e56ee8e4e1c6642b773eae73d29379a91c8ac5149e803f42'),
    'raygui': ('4.0', 'f8c9f7b63d3c136174e5e2a83b5032c6e38acea307abfd75e7d2aa3daffa84b1'),
}


def file_digest(path):
    """计算磁盘归档的 SHA-256。"""
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def prepare(name, version, expected, cache, workspace, offline):
    """校验上游归档，再在本次构建目录安全展开。"""
    archive = cache / f'{name}-{version}.zip'
    if not archive.is_file() or file_digest(archive) != expected:
        if offline:
            raise RuntimeError(f'离线缓存缺失或损坏：{archive}')
        url = f'https://codeload.github.com/raysan5/{name}/zip/refs/tags/{version}'
        with tempfile.TemporaryDirectory(prefix='desktop-download-') as temp:
            candidate = Path(temp) / archive.name
            with urllib.request.urlopen(url, timeout=90) as source, candidate.open('wb') as target:
                shutil.copyfileobj(source, target)
            if file_digest(candidate) != expected:
                raise RuntimeError(f'{name} 归档摘要不匹配')
            os.replace(candidate, archive)
    prefix = f'{name}-{version}'
    with zipfile.ZipFile(archive) as package:
        for member in package.infolist():
            path = PurePosixPath(member.filename)
            if path.is_absolute() or path.parts[0] != prefix or '..' in path.parts or '\\' in member.filename or ':' in member.filename:
                raise ValueError(f'归档路径不安全：{member.filename}')
        package.extractall(workspace)
    return workspace / prefix


def render_gui_module(template, library):
    """由维护的控件清单生成具名工厂，与 C 枚举使用同一顺序。"""
    definitions = (ROOT / 'library/desktop_gui/controls.def').read_text(encoding='utf-8')
    controls = re.findall(r'^CHALI_GUI_CONTROL\((\w+),\s*(Gui\w+)\)', definitions, re.M)
    if not controls or len({name for name, _ in controls}) != len(controls):
        raise ValueError('GUI 控件清单为空或包含重复名称')
    factories = []
    names = ['/** 查询清单中的控件名称，非法下标返回空串。 */', 'String controlName(int64 index) {']
    for index, (name, upstream) in enumerate(controls):
        parts = name.split('_')
        name = parts[0] + ''.join(part[:1].upper() + part[1:] for part in parts[1:])
        factories += [f'/** 创建 {upstream} 对应的控件状态，绘制时同步派发变化。 */',
                      f'Widget {name}(String text) {{ return createControl({index}, text); }}']
        names.append(f'    if (index == {index}) {{ return "{name}"; }}')
    names += ['    return "";', '}']
    return (template.replace('@LIBRARY@', library).replace('@CONTROL_COUNT@', str(len(controls)))
            .replace('@CONTROL_FACTORIES@', '\n'.join(factories)).replace('@CONTROL_NAMES@', '\n'.join(names)))


def main():
    """用 CMake 生成窗体宿主并部署库、模块和依赖许可证。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out', type=Path, default=ROOT / 'build/desktop_gui')
    parser.add_argument('--cache', type=Path, default=ROOT / 'build/dependencies/desktop_gui')
    parser.add_argument('--offline', action='store_true')
    parser.add_argument('--icons', action='store_true')
    parser.add_argument('--audio', action='store_true')
    parser.add_argument('--static', action='store_true', help='构建供编译器内嵌的原生静态库')
    parser.add_argument('--jobs', type=int, default=2)
    options = parser.parse_args()
    if options.jobs < 1:
        parser.error('--jobs 必须为正整数')
    if not (os.name == 'nt' or sys.platform.startswith('linux')):
        parser.error('当前桌面构建脚本支持 Windows 和 Linux')
    output = options.out.resolve()
    cache = options.cache.resolve()
    sources = ROOT / 'library/desktop_gui'
    library = 'chali_desktop.dll' if os.name == 'nt' else 'chali_desktop.so'
    with tempfile.TemporaryDirectory(prefix='desktop-build-') as temp:
        workspace = Path(temp)
        with download_lock(cache / '.lock'):
            deps = {name: prepare(name, version, checksum, cache, workspace, options.offline)
                    for name, (version, checksum) in DEPENDENCIES.items()}
        build = workspace / 'build'
        stage = workspace / 'install'
        subprocess.run(['cmake', '-S', str(sources), '-B', str(build), '-G', 'Ninja',
                        '-DCMAKE_BUILD_TYPE=Release', f'-DRAYLIB_SOURCE={deps["raylib"].as_posix()}',
                        f'-DRAYGUI_SOURCE={deps["raygui"].as_posix()}', f'-DCHALI_GUI_ICONS={"ON" if options.icons else "OFF"}',
                        f'-DCHALI_NATIVE_STATIC={"ON" if options.static else "OFF"}',
                        f'-DCHALI_DESKTOP_AUDIO={"ON" if options.audio else "OFF"}'], check=True)
        subprocess.run(['cmake', '--build', str(build), '--parallel', str(options.jobs)], check=True)
        subprocess.run(['cmake', '--install', str(build), '--prefix', stage.as_posix()], check=True)
        if options.static:
            shutil.copytree(stage, output, dirs_exist_ok=True)
            print(output / 'chali_desktop.a')
            return
        for name in ('desktop_gui', 'desktop_audio') if options.audio else ('desktop_gui',):
            template = (sources / (name + '.chl.in')).read_text(encoding='utf-8')
            if name == 'desktop_gui':
                template = render_gui_module(template, '../' + library)
            module_name = 'gui' if name == 'desktop_gui' else 'audio'
            (stage / 'std').mkdir(exist_ok=True)
            (stage / 'std' / (module_name + '.chl')).write_text(template.replace('@LIBRARY@', '../' + library), encoding='utf-8')
        shutil.copytree(stage, output, dirs_exist_ok=True)
        if not options.audio:
            (output / 'std' / 'audio.chl').unlink(missing_ok=True)
        for legacy in ('desktop_gui.chl', 'desktop_audio.chl'):
            (output / legacy).unlink(missing_ok=True)
    print(output / library)


if __name__ == '__main__':
    main()
