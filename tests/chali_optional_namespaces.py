"""核对内置模块的 std:: 命名，并执行实际安装的 GUI/音频/XML 模块。"""
from pathlib import Path
import argparse
import json
import os
import re
import subprocess
import shutil
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
SOURCE = '''import std::gui;
import std::audio;
import std::xml;
void main() {
    assert(std::gui::controlCount() == 34);
    assert(std::gui::openWindow(160, 100, "std namespace integration", 2));
    assert(std::gui::beginFrame());
    std::gui::endFrame();
    let textBox = std::gui::textBox("中文");
    assert(textBox.isValid() && textBox.setText("updated") && textBox.text() == "updated");
    textBox.close();
    int64 kind = 0;
    while (kind < std::gui::controlCount()) {
        let widget = std::gui::createControl(kind, "One;Two;Three");
        assert(widget.isValid() && widget.setVisible(true) && widget.setEnabled(true));
        assert(std::gui::beginFrame());
        widget.draw(std::gui::Rect::at(5, 5, 120, 80));
        std::gui::endFrame();
        widget.close();
        assert(!widget.isValid());
        kind += 1;
    }
    std::gui::EventBus bus;
    int64 events = 0;
    assert(bus.once("named", void fn mut(std::gui::Event event) { events += 1; }) != 0);
    assert(bus.emit(std::gui::customEvent("named")) == 1 && events == 1);
    assert(bus.emit(std::gui::customEvent("named")) == 0);
    std::gui::close();
    assert(std::gui::shouldClose());
    std::audio::close();
    assert(std::audio::load("missing-audio.wav") == 0);
    assert(!std::audio::play(1));
    assert(!std::audio::stop(1));
    assert(!std::audio::isPlaying(1));
    assert(!std::audio::unload(1));
    std::audio::close();
    let document = std::xml::parse("<message from='ChaLi'>hello</message>").unwrap();
    assert(document.name() == "message");
    assert(document.childCount() == 1);
    assert(std::xml::escapeText("<&>") == "&lt;&amp;&gt;");
    assert(std::xml::escapeAttribute("'") == "&apos;");
    assert(std::xml::parse("<bad></wrong>").isErr());
    println("std::gui/std::audio/std::xml installed modules passed");
}
'''

PROGRAMS = {
    'hello': 'void main() { println("Hello World"); }',
    'unused': '''import std::gui;
import std::audio;
import std::xml;
bool unused() { return std::audio::open(); }
void main() { println("Hello World"); }
''',
    'scalar': '''import std::gui;
void main() {
    assert(!std::gui::frameActive());
    println("scalar");
}
''',
    'gui': '''import std::gui;
void main() {
    assert(std::gui::openWindow(160, 100, "ChaLi", 2));
    assert(std::gui::beginFrame());
    assert(std::gui::measureText("ChaLi", 14).x > 0);
    std::gui::endFrame();
    std::gui::close();
    println("gui");
}
''',
    'audio': '''import std::audio;
void main() {
    std::audio::close();
    assert(std::audio::load("missing-audio.wav") == 0);
    assert(!std::audio::play(1));
    assert(!std::audio::isPlaying(1));
    println("audio");
}
''',
    'xml': '''import std::xml;
void main() {
    let root = std::xml::parse("<root attr='值'/>").unwrap();
    assert(root.name() == "root" && root.childCount() == 0);
    assert(std::xml::escapeText("<&>") == "&lt;&amp;&gt;");
    assert(std::xml::escapeAttribute("'") == "&apos;");
    assert(std::xml::parse("<root></other>").isErr());
    println("xml");
}
''',
    'combined': SOURCE,
    'crypto': (ROOT / 'example/crypto/demo.chl').read_text(encoding='utf-8').replace('crypto OK', 'crypto'),
}


def embedded(args):
    """只携带生成程序运行，核对模块裁剪、Unicode 路径与平台导入表。"""
    results = []
    env = dict(os.environ, CHALI_QUIET='1')
    with tempfile.TemporaryDirectory(prefix='std-embedded-native-') as folder:
        work = Path(folder)
        for name, content in PROGRAMS.items():
            source = work / (name + '.chl')
            source.write_text(content, encoding='utf-8')
            modes = args.lto_modes or (('off', 'full', 'thin') if name in ('hello', 'unused', 'combined', 'crypto') else ('thin',))
            for lto in modes:
                target = work / (name + '-' + lto)
                command = [str(args.chali.resolve()), 'aot', str(source), '--out', str(target), '--lto-mode', lto]
                compile_env = dict(env, CHALI_ABI_CXX='not-a-cxx-compiler') if name == 'scalar' else env
                result = subprocess.run(command, cwd=work, env=compile_env, capture_output=True, text=True, encoding='utf-8', timeout=240)
                assert result.returncode == 0, (command, result.stdout, result.stderr)
                product = target.with_suffix('.exe') if os.name == 'nt' else target
                destination = work / ('仅有程序 ' + name + ' ' + lto)
                destination.mkdir()
                relocated = destination / product.name
                shutil.copy2(product, relocated)
                execution = subprocess.run([str(relocated)], cwd=work, env=env,
                                           capture_output=True, text=True, encoding='utf-8', timeout=60)
                expected = 'Hello World' if name in ('hello', 'unused') else (
                    'std::gui/std::audio/std::xml installed modules passed' if name == 'combined' else name)
                assert (execution.returncode, execution.stdout.strip(), execution.stderr) == (0, expected, ''), execution
                assert list(destination.iterdir()) == [relocated]
                if os.name == 'nt':
                    report = subprocess.check_output([args.objdump, '-p', str(product)], text=True, encoding='utf-8')
                    imports = re.findall(r'DLL Name:\s*(\S+)', report, flags=re.I)
                    assert imports and all(not re.match(r'chali_|libgcc|libstdc\+\+|libwinpthread', lib, re.I) for lib in imports), imports
                else:
                    report = subprocess.check_output(['readelf', '-d', str(relocated)], text=True)
                    imports = re.findall(r'\(NEEDED\).*?\[([^\]]+)\]', report)
                    assert not any('chali_' in lib or 'raylib' in lib or 'expat' in lib or 'sodium' in lib for lib in imports), imports
                result = dict(sample=name, mode=lto, bytes=relocated.stat().st_size, imports=imports)
                if name == 'unused':
                    baseline = next(record for record in results if record['sample'] == 'hello' and record['mode'] == lto)
                    assert (result['bytes'], imports) == (baseline['bytes'], baseline['imports']), result
                results.append(result)
                print(result, flush=True)
        checked = subprocess.run([str(args.chali.resolve()), 'run', str(work / 'combined.chl'),
                                  '--out', str(work / 'checked')], cwd=work, env=env,
                                 capture_output=True, text=True, encoding='utf-8', timeout=240)
        assert (checked.returncode, checked.stdout.strip(), checked.stderr) == (
            0, 'std::gui/std::audio/std::xml installed modules passed', ''), checked
    if args.evidence:
        args.evidence.write_text(json.dumps(results, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    print('Embedded native modules passed (AOT and checked run)', flush=True)


def main():
    """使用安装后的路径，验证模块发现、相对 DLL 路径和运行结果。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--desktop', type=Path, default=ROOT / 'build/desktop_audio')
    parser.add_argument('--xml', type=Path, default=ROOT / 'build/xml')
    parser.add_argument('--embedded', action='store_true')
    parser.add_argument('--lto-modes', nargs='+', choices=('off','full','thin'), help='只验证指定 LTO 模式，缺省覆盖完整矩阵')
    parser.add_argument('--objdump', default='objdump')
    parser.add_argument('--evidence', type=Path)
    args = parser.parse_args()
    sources = sorted((ROOT / 'source/std').glob('*.chl')) + sorted((ROOT / 'library').glob('**/*.chl.in'))
    for path in sources:
        declaration = re.search(r'^\s*module\s+([^;]+);', path.read_text(encoding='utf-8'), re.M)
        assert declaration and declaration[1].startswith('std::'), path
    if args.embedded:
        embedded(args)
        return
    for root, modules, legacy in [(args.desktop, ('gui', 'audio'), ('desktop_gui', 'desktop_audio')),
                                  (args.xml, ('xml',), ('xml',))]:
        for name in modules:
            text = (root / 'std' / (name + '.chl')).read_text(encoding='utf-8')
            assert re.search(r'^module std::' + name + ';', text), (root, name)
        for name in legacy:
            assert not (root / (name + '.chl')).exists(), (root, name)
    with tempfile.TemporaryDirectory(prefix='std-namespaces-') as folder:
        work = Path(folder)
        source = work / 'main.chl'
        source.write_text(SOURCE, encoding='utf-8')
        for mode in ('run', 'aot'):
            target = work / mode
            result = subprocess.run([str(args.chali.resolve()), mode, str(source), '-I', str(args.desktop.resolve()),
                                     '-I', str(args.xml.resolve()), '--out', str(target)], cwd=ROOT,
                                    capture_output=True, text=True, encoding='utf-8', timeout=240)
            assert result.returncode == 0, (mode, result.stdout, result.stderr)
            if mode == 'aot':
                result = subprocess.run([str(target.with_suffix('.exe') if os.name == 'nt' else target)], cwd=ROOT,
                                        capture_output=True, text=True, encoding='utf-8', timeout=60)
            assert (result.returncode, result.stdout.strip(), result.stderr) == (
                0, 'std::gui/std::audio/std::xml installed modules passed', ''), (mode, result)
            print('installed std namespaces/' + mode + ' passed', flush=True)
    print(str(len(sources)) + ' standard module declarations use std::; audio checked without opening a device')


if __name__ == '__main__':
    main()
