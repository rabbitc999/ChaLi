"""验证锁定归档、损坏缓存恢复以及失败时不部分安装依赖。"""
from pathlib import Path
from project_temp import tempfile
import importlib.util
import io
import unittest
from unittest.mock import patch
import zipfile

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('chali_fetch_dependencies', ROOT / 'tools/fetch_dependencies.py')
fetcher = importlib.util.module_from_spec(spec)
spec.loader.exec_module(fetcher)


def fixture():
    """构造可独立验证的微型归档，不复制上游库源码。"""
    stream = io.BytesIO()
    with zipfile.ZipFile(stream, 'w') as archive:
        archive.writestr('fixture/a.c', b'source')
        archive.writestr('fixture/b.h', b'header')
        archive.writestr('../unselected.txt', b'ignored')
    data = stream.getvalue()
    package = dict(version='1', url='https://invalid.example/dependency.zip', prefix='fixture/',
                   sha256=fetcher.digest(data), files={
                       'a.c': dict(sha256=fetcher.digest(b'source')),
                       'b.h': dict(sha256=fetcher.digest(b'header'))})
    return data, package


class DependencyTests(unittest.TestCase):
    """用隔离缓存检查成功、篡改和失败恢复路径。"""

    def test_cached_archive_and_repair(self):
        """离线解包忽略未选择成员，篡改文件由验证后的缓存修复。"""
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            data, package = fixture()
            cache = root / 'cache'
            cache.mkdir()
            (cache / 'miniz-1.zip').write_bytes(data)
            fetcher.fetch('miniz', package, root / 'build', cache, True)
            target = root / 'build/miniz/a.c'
            self.assertEqual(target.read_bytes(), b'source')
            self.assertFalse((root / 'unselected.txt').exists())
            target.write_bytes(b'corrupted')
            fetcher.fetch('miniz', package, root / 'build', cache, True)
            self.assertEqual(target.read_bytes(), b'source')

    def test_file_failure_does_not_partially_install(self):
        """后续成员摘要失败时，之前的已有文件不被覆盖。"""
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            data, package = fixture()
            cache = root / 'cache'
            cache.mkdir()
            (cache / 'miniz-1.zip').write_bytes(data)
            package['files']['b.h']['sha256'] = '0' * 64
            destination = root / 'build/miniz'
            destination.mkdir(parents=True)
            (destination / 'a.c').write_bytes(b'previous')
            with self.assertRaisesRegex(RuntimeError, '文件 SHA256'):
                fetcher.fetch('miniz', package, root / 'build', cache, True)
            self.assertEqual((destination / 'a.c').read_bytes(), b'previous')
            self.assertFalse((destination / 'b.h').exists())

    def test_download_checksum_failure(self):
        """下载内容不符时拒绝缓存和安装，不把错误响应当作源码。"""
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            _, package = fixture()
            cache = root / 'cache'
            cache.mkdir()
            with patch.object(fetcher.urllib.request, 'urlopen', return_value=io.BytesIO(b'invalid download')):
                with self.assertRaisesRegex(RuntimeError, '归档 SHA256'):
                    fetcher.fetch('miniz', package, root / 'build', cache, False)
            self.assertFalse((cache / 'miniz-1.zip').exists())
            self.assertFalse((root / 'build').exists())


if __name__ == '__main__':
    unittest.main(verbosity=2)
