"""从真实生命游戏示例生成隐藏窗口验收与截图，不改变产品输入路径。"""
import argparse
import os
from pathlib import Path
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]


def main():
    """执行模型与实际 GUI，并保留原始帧截图供书稿引用。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--compiler', type=Path, required=True)
    parser.add_argument('--library', type=Path, required=True)
    args = parser.parse_args()
    sys.path.insert(0, str(ROOT / 'tools'))
    from setup_desktop_gui import render_gui_module
    out = ROOT / 'tmp/life-regression'
    out.mkdir(parents=True, exist_ok=True)
    shutil.copy2(args.library.resolve(), out / 'chali_desktop.dll')
    shutil.copy2(ROOT / 'example/desktop-gui/life_model.chl', out / 'life_model.chl')
    template = (ROOT / 'library/desktop_gui/desktop_gui.chl.in').read_text(encoding='utf-8')
    (out / 'std').mkdir(exist_ok=True)
    (out / 'std/gui.chl').write_text(render_gui_module(template, '../chali_desktop.dll'), encoding='utf-8')
    source = (ROOT / 'example/desktop-gui/life.chl').read_text(encoding='utf-8')
    source = source.replace(', 1));', ', 2));', 1)
    source = source.replace('void main() {', '''#abi("C", "./chali_desktop.dll", "chali_gui_test_capture")
bool capture(String path);
#abi("C", "./chali_desktop.dll", "chali_gui_test_input")
bool inject(int32 kind, int32 a, int32 b);
void main() {''', 1)
    source = source.replace('    std::gui::run(void fn mut() {', '''    int64 frames = 0;
    std::gui::run(void fn mut() {
        frames += 1;''', 1)
    # 测试事件只改变本进程隐藏窗口的 raylib 状态，不移动系统鼠标。
    script = '''
        if (frames == 2) { assert(inject((int32)0, (int32)80, (int32)170)); assert(inject((int32)1, (int32)0, (int32)0)); }
        if (frames == 4) { assert(board.at(0, 0) == 1); assert(inject((int32)2, (int32)0, (int32)0)); }
        if (frames == 6) { assert(inject((int32)1, (int32)0, (int32)0)); }
        if (frames == 8) { assert(board.at(0, 0) == 0 && board.population() == 12); assert(inject((int32)2, (int32)0, (int32)0)); }
        if (frames == 10) { assert(inject((int32)1, (int32)1, (int32)0)); }
        if (frames == 12) { assert(inject((int32)0, (int32)260, (int32)250)); }
        if (frames == 14) { assert(pan_x == 180 && pan_y == 80); assert(inject((int32)2, (int32)1, (int32)0)); }
        if (frames == 16) { assert(inject((int32)0, (int32)80, (int32)170)); assert(inject((int32)1, (int32)0, (int32)0)); }
        if (frames == 18) { assert(board.at(-10, -5) == 1); assert(inject((int32)2, (int32)0, (int32)0)); }
        if (frames == 21) { board.seed(); pan_x = 0; pan_y = 0; }
        if (frames == 22) { assert(inject((int32)0, (int32)120, (int32)122)); assert(inject((int32)1, (int32)0, (int32)0)); }
        if (frames == 24) { assert(inject((int32)2, (int32)0, (int32)0)); }
        if (frames == 26) { assert(running); }
        if (frames == 60) { assert(inject((int32)0, (int32)80, (int32)170)); assert(inject((int32)1, (int32)0, (int32)0)); }
        if (frames == 62) { assert(board.at(0, 0) == 0); assert(inject((int32)2, (int32)0, (int32)0)); }
        if (frames == 100) { assert(board.generation > 0); assert(inject((int32)0, (int32)718, (int32)122)); assert(inject((int32)1, (int32)0, (int32)0)); }
        if (frames == 102) { assert(speed.value() > 20 && speed.value() < 40); assert(inject((int32)2, (int32)0, (int32)0)); speed.setValue(30); }
        if (frames == 140) { assert(inject((int32)0, (int32)120, (int32)122)); assert(inject((int32)1, (int32)0, (int32)0)); }
        if (frames == 142) { assert(inject((int32)2, (int32)0, (int32)0)); }
        if (frames == 144) { assert(!running); std::gui::requestClose(); }
'''.replace('@OUT@', out.as_posix())
    source = source.replace('        frames += 1;', '        frames += 1;' + script, 1)
    captures = '\n'.join('        if (frames == ' + str(frame) + ') { assert(capture("' + out.as_posix() + '/' + name + '.png")); }'
                         for frame, name in [(20, 'panned'), (22, 'paused'), (98, 'running'), (138, 'speed')])
    source = source.replace('    }, void fn mut(std::gui::Event event)', captures + '\n    }, void fn mut(std::gui::Event event)', 1)
    (out / 'life.chl').write_text(source, encoding='utf-8')
    env = dict(os.environ, CHALI_QUIET='1', CHALI_LANG='en', TMP=str(ROOT / 'tmp'),
               TEMP=str(ROOT / 'tmp'), TMPDIR=str(ROOT / 'tmp'))
    compiler = str(args.compiler.resolve())
    for mode in ('run', 'aot'):
        for name, path in [('model', ROOT / 'tests/fixtures/desktop_life_model.chl'), ('gui', out / 'life.chl')]:
            target = out / (name + '-' + mode)
            result = subprocess.run([compiler, mode, str(path), '-I', str(out), '--out', str(target)],
                                    cwd=ROOT, env=env, capture_output=True, text=True, encoding='utf-8', timeout=240)
            assert result.returncode == 0, (name, mode, result.stdout, result.stderr)
            if mode == 'aot':
                result = subprocess.run([str(target.with_suffix('.exe'))], cwd=ROOT, env=env,
                                        capture_output=True, text=True, encoding='utf-8', timeout=90)
            assert result.returncode == 0 and not result.stderr, (name, mode, result.stdout, result.stderr)
            if name == 'model':
                assert result.stdout.strip() == 'Life sparse board, toggle, still life, oscillator and glider passed'
            print('Life ' + name + '/' + mode + ' passed', flush=True)


if __name__ == '__main__':
    main()
