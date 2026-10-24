"""验证资产内容缓存、损坏恢复以及 gen.sh 在 Windows 编码环境下的中文输出。"""
from pathlib import Path
import os
import shutil
import subprocess
import sys
import re
import time
from project_temp import tempfile
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from embedded_asset_cache import AssetCache
import embed_stdlib


def verify_cache(folder):
    """时间戳变化不使内容缓存失效，输入变化与输出损坏必须使缓存失效。"""
    source = folder / 'input.txt'
    output = folder / 'output.cpp'
    source.write_bytes(b'input one')
    output.write_bytes(b'generated one')
    create = lambda: AssetCache('unit', {'source': source, 'options': b'level=4'}, folder / 'cache')
    cache = create()
    assert not cache.valid([output])
    cache.save([output])
    assert create().valid([output])
    stat = source.stat()
    os.utime(source, ns=(stat.st_atime_ns, stat.st_mtime_ns + 1000000000))
    assert create().valid([output]), '只更改时间戳不应重建'
    source.write_bytes(b'input two')
    os.utime(source, ns=(stat.st_atime_ns, stat.st_mtime_ns))
    assert not create().valid([output]), '相同长度与旧时间戳不能掩盖字节变化'
    create().save([output])
    assert not AssetCache('unit', {'source': source, 'options': b'level=3'}, folder / 'cache').valid([output])
    output.write_bytes(b'corrupted')
    assert not create().valid([output])
    create().save([output])
    output.unlink()
    assert not create().valid([output])
    output.write_bytes(b'new output')
    create().save([output])
    create().path.write_bytes(b'\xffinvalid')
    assert not create().valid([output])


def verify_generator(folder):
    """真实执行生成器主体，确认命中后既不调用压缩器也不改产物时间戳。"""
    (folder / 'source/lztext').mkdir(parents=True)
    output = folder / 'source/chali_stdlib_embedded.cpp'
    modules = [('std::test', 'void test() {}')]
    counts = []

    def compress(blob):
        """记录进入昂贵压缩阶段的次数；小型测试不需要外部 C++ 工具。"""
        counts.append(blob)
        return b'test-packed'

    def cache(name, inputs):
        """将测试摘要保存在独立目录，避免污染真实构建缓存。"""
        return AssetCache(name, inputs, folder / 'cache')

    with patch.object(embed_stdlib, 'CHALI', str(folder)), \
         patch.object(embed_stdlib, 'collect', lambda: modules), \
         patch.object(embed_stdlib, 'collect_generator_assets', lambda: [('sample.hpp', '// sample')]), \
         patch.object(embed_stdlib, 'compress_bundle', compress), \
         patch.object(embed_stdlib, 'AssetCache', cache), \
         patch('embed_manual.ensure_generated', lambda: None):
        assert embed_stdlib.main() == 0 and len(counts) == 2
        stamp = output.stat().st_mtime_ns
        assert embed_stdlib.main() == 0 and len(counts) == 2
        assert output.stat().st_mtime_ns == stamp
        modules[0] = ('std::changed', modules[0][1])
        assert embed_stdlib.main() == 0 and len(counts) == 4, '索引改名也必须重建'
        output.write_bytes(b'broken')
        assert embed_stdlib.main() == 0 and len(counts) == 6
        output.unlink()
        assert embed_stdlib.main() == 0 and len(counts) == 8


def verify_encoding(folder):
    """通过真实 sh 与 Python 子进程检查入口会覆盖不匹配的 Windows 默认编码。"""
    (folder / 'tools').mkdir(parents=True)
    (folder / 'source').mkdir()
    shutil.copy2(ROOT / 'gen.sh', folder / 'gen.sh')
    for name in ['embed_i18n.py', 'stackvm_sources.py', 'cas_sources.py', 'embed_manual.py', 'embed_stdlib.py', 'expand_grammar.py']:
        (folder / 'tools' / name).write_text('print("内嵌资产生成与校验完成")\n', encoding='utf-8')
    engine = folder / 'engine.sh'
    engine.write_text('#!/bin/sh\nprintf "解析器生成完成\\n"\n', encoding='utf-8', newline='\n')
    engine.chmod(0o755)
    env = dict(os.environ, ENGINE=engine.as_posix(), PYTHONUTF8='0', PYTHONIOENCODING='cp936')
    result = subprocess.run(['sh', 'gen.sh'], cwd=folder, env=env, capture_output=True, timeout=30)
    assert result.returncode == 0, result.stderr
    text = result.stdout.decode('utf-8', errors='strict')
    assert text.count('内嵌资产生成与校验完成') == 6 and '解析器生成完成' in text, text
    assert '\ufffd' not in text


def verify_make(folder):
    """执行真实内嵌依赖规则，验证输入改变不会留下旧对象，摘要命中也不重建对象。"""
    for name in ['source', 'tools', 'build']:
        (folder / name).mkdir(parents=True, exist_ok=True)
    makefile = (ROOT / 'local.mk').read_text(encoding='utf-8')
    rules = re.search(r'CHALI_EMBED_STAMP :=.*?^\$\(CHALI_STDLIB_OBJ\):.*?$', makefile, re.M | re.S)[0]
    (folder / 'Makefile').write_text(
        'all: build/embedded.o\nPYTHON := python\nFRONTEND_BUILD := build\n'
        'CHALI_EMBED_INPUTS := input\nCHALI_STDLIB_OBJ := build/embedded.o\n' + rules + '\n'
        'build/embedded.o: source/chali_stdlib_embedded.cpp\n'
        '\tpython -c "from pathlib import Path; Path(\'build/embedded.o\').write_bytes(Path(\'$<\').read_bytes())"\n',
        encoding='utf-8', newline='\n')
    (folder / 'tools/embed_stdlib.py').write_text(
        'from pathlib import Path\nimport sys\n'
        'source = Path("input").read_bytes()\noutput = Path("source/chali_stdlib_embedded.cpp")\n'
        'if not output.exists() or output.read_bytes() != source: output.write_bytes(source)\n'
        'if "--stamp" in sys.argv: Path(sys.argv[sys.argv.index("--stamp") + 1]).touch()\n', encoding='utf-8')
    previous = 0
    for index, content in enumerate([b'first asset', b'changed asset', b'changed asset']):
        if index:
            time.sleep(1.1)  # Windows GNU make 的依赖比较可能只有整秒精度。
        (folder / 'input').write_bytes(content)
        result = subprocess.run(['sh', '-c', 'make all'], cwd=folder, capture_output=True, timeout=30)
        assert result.returncode == 0, result.stderr
        output = folder / 'build/embedded.o'
        assert output.read_bytes() == content, '输入改变后必须更新实际对象资产'
        current = output.stat().st_mtime_ns
        if index == 2:
            assert current == previous, '内容未变时不能重新构建对象'
        previous = current


def main():
    """在私有临时目录核对所有缓存失效条件与 UTF-8 控制台链路。"""
    with tempfile.TemporaryDirectory(prefix='chali-embedded-cache-') as directory:
        folder = Path(directory)
        verify_cache(folder)
        verify_generator(folder / 'generator')
        verify_encoding(folder / 'encoding')
        verify_make(folder / 'make')
    print('内嵌资产 SHA256：内容命中、索引变更、损坏/缺失恢复、Make 依赖更新与命中时对象复用、gen.sh 中文 UTF-8 输出通过')


if __name__ == '__main__':
    main()
