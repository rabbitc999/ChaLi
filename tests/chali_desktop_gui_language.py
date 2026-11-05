"""真实隐藏窗口的语言 ABI、控件、绘图和回调集成测试（Windows）。"""
import argparse
import os
from pathlib import Path
import shutil
import subprocess
import sys
from PIL import Image

ROOT = Path(__file__).resolve().parents[1]


def verify(result, label, expected):
    """检查返回状态、固定输出与诊断，避免仅验证编译成功。"""
    assert (result.returncode, result.stdout.replace('\r\n', '\n'), result.stderr) == (0, expected, ''), (
        label, result.returncode, result.stdout, result.stderr)


def main():
    """生成匹配模块与真实 PNG 资源，再分别执行 checked run 和 AOT。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--compiler', type=Path, required=True)
    parser.add_argument('--library', type=Path, required=True, help='CHALI_GUI_TESTING=ON 的测试 DLL')
    args = parser.parse_args()
    sys.path.insert(0, str(ROOT / 'tools'))
    from setup_desktop_gui import render_gui_module
    output = ROOT / 'tmp/gui-language-regression'
    output.mkdir(parents=True, exist_ok=True)
    template = (ROOT / 'library/desktop_gui/desktop_gui.chl.in').read_text(encoding='utf-8')
    (output / 'std').mkdir(exist_ok=True)
    (output / 'std/gui.chl').write_text(render_gui_module(template, '../chali_desktop.dll'), encoding='utf-8')
    shutil.copy2(args.library.resolve(), output / 'chali_desktop.dll')
    texture = output / '纹理.png'
    Image.new('RGBA', (16, 16), (40, 200, 100, 255)).save(texture)
    environment = dict(os.environ, CHALI_QUIET='1', CHALI_LANG='en', TMP=str(ROOT / 'tmp'),
                       TEMP=str(ROOT / 'tmp'), TMPDIR=str(ROOT / 'tmp'))
    fixtures = [('desktop_gui_integration', 'GUI language ABI, click callback, UTF-8 text and 34 controls passed\n'),
                ('desktop_gui_events', 'GUI drawing, textures, custom widget input and reentrant event bus passed\n')]
    for name, expected in fixtures:
        source = output / (name + '.chl')
        body = (ROOT / 'tests/fixtures' / (name + '.chl')).read_text(encoding='utf-8')
        body = body.replace('@TEXTURE_PATH@', texture.as_posix()).replace('@CAPTURE_PATH@', (output / 'custom.png').as_posix())
        source.write_text(body, encoding='utf-8')
        for mode in ('run', 'aot'):
            prefix = output / (name + '-' + mode)
            result = subprocess.run([str(args.compiler.resolve()), mode, str(source), '-I', str(output),
                                     '--out', str(prefix)], env=environment, cwd=ROOT,
                                    capture_output=True, text=True, encoding='utf-8', timeout=240)
            assert result.returncode == 0, (mode, result.stdout, result.stderr)
            if mode == 'aot':
                result = subprocess.run([str(prefix.with_suffix('.exe'))], env=environment, cwd=ROOT,
                                        capture_output=True, text=True, encoding='utf-8', timeout=60)
            verify(result, name + '/' + mode, expected)
            print('GUI ' + name + '/' + mode + ' passed', flush=True)


if __name__ == '__main__':
    main()
