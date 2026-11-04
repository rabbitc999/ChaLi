"""验证发布版本检查、归档内容及失败时的原子替换。"""
import importlib.util
import json
from pathlib import Path
from project_temp import tempfile
import unittest
import zipfile
import hashlib
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('chali_release', ROOT / 'tools/release.py')
release = importlib.util.module_from_spec(spec)
spec.loader.exec_module(release)


class ReleaseTests(unittest.TestCase):
    """使用独立目录校验发布产物，不触及仓库标签及远端。"""

    def test_version_drift(self):
        """插件版本漂移和非法 VERSION 必须在执行构建前失败。"""
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            (root / 'vscode').mkdir()
            (root / 'VERSION').write_text('1.0.0', encoding='utf-8')
            (root / 'vscode/package.json').write_text('{"version":"1.0.0"}', encoding='utf-8')
            self.assertEqual(release.version_of(root), '1.0.0')
            (root / 'vscode/package.json').write_text('{"version":"0.9.0"}', encoding='utf-8')
            with self.assertRaisesRegex(ValueError, 'VERSION'):
                release.version_of(root)
            (root / 'VERSION').write_text('0.21.0', encoding='utf-8')
            (root / 'vscode/package.json').write_text('{"version":"1.0.0","chaliCompilerVersion":"0.21.0"}', encoding='utf-8')
            with self.assertRaisesRegex(ValueError, 'VERSION'):
                release.version_of(root)
            with self.assertRaisesRegex(ValueError, 'VERSION'):
                release.editor_version_of(root)
            (root / 'VERSION').write_text('../bad', encoding='utf-8')
            with self.assertRaisesRegex(ValueError, 'VERSION'):
                release.version_of(root)

    def test_isolated_build_config(self):
        """指定隔离目录时不读取根目录中过期的反射构建配置。"""
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            (root / 'vscode').mkdir()
            (root / 'vscode/package.json').write_text('{"version":"1.0.0","chaliCompilerVersion":"1.0.0"}', encoding='utf-8')
            (root / 'VERSION').write_text('1.0.0', encoding='utf-8')
            (root / 'doc/book/latex').mkdir(parents=True)
            (root / 'doc/book/latex/book.pdf').write_bytes(b'%PDF-1.7')
            binary = root / 'chali'
            binary.write_bytes(b'compiler')
            vsix = root / 'editor.vsix'
            with zipfile.ZipFile(vsix, 'w') as package:
                package.writestr('extension/package.json', '{"version":"1.0.0","chaliCompilerVersion":"1.0.0"}')
                package.writestr('extension.vsixmanifest', '<Package><Identity Version="1.0.0"/></Package>')
                package.writestr('extension/bin/chali', b'compiler')
            isolated = root / 'isolated'
            (isolated / 'chali-runtime').mkdir(parents=True)
            config = isolated / 'chali-runtime/buildConfig.json'
            config.write_text('CHALI_WITH_REFLECTION=1 STACKVM_WITH_REFLECTION=1', encoding='utf-8')
            with patch.object(release, 'compiler_version', return_value='1.0.0'):
                release.verify_assets(root, binary, vsix, '1.0.0', False, build_root=isolated)
                config.write_text('CHALI_WITH_REFLECTION=0 STACKVM_WITH_REFLECTION=0', encoding='utf-8')
                with self.assertRaisesRegex(ValueError, '反射'):
                    release.verify_assets(root, binary, vsix, '1.0.0', False, build_root=isolated)

    def test_archive_and_failed_replacement(self):
        """归档包含源码身份、样例和正确摘要；输入丢失不破坏原 ZIP。"""
        with tempfile.TemporaryDirectory() as folder:
            root = Path(folder)
            (root / 'doc/book/latex').mkdir(parents=True)
            (root / 'example/samples').mkdir(parents=True)
            (root / 'doc/book/latex/book.pdf').write_bytes(b'%PDF-1.7\n')
            for name in ('VERSION', 'LICENSE', 'NOTICE', 'CHANGELOG.md'):
                (root / name).write_text('0.21.0', encoding='utf-8')
            for name in ('miniz', 'ipage', 'caspp'):
                directory = root / ('licenses' if name == 'miniz' else 'library') / name
                directory.mkdir(parents=True)
                (directory / 'LICENSE').write_text('fixture license: ' + name, encoding='utf-8')
            for name in release.SAMPLES:
                (root / f'example/samples/{name}.chl').write_text('void main(){}', encoding='utf-8')
            binary, vsix = root / 'ChaLi.exe', root / 'chali-editor-1.0.0.vsix'
            binary.write_bytes(b'compiler')
            vsix.write_bytes(b'extension')
            output = root / 'output with spaces'
            archive = release.package_assets(root, output, binary, vsix, '0.21.0', 'windows-x64', 'abc123', True)
            with zipfile.ZipFile(archive) as packed:
                self.assertIsNone(packed.testzip())
                metadata = json.loads(packed.read('release.json'))
                self.assertEqual(metadata['commit'], 'abc123')
                self.assertEqual(metadata['version'], '0.21.0')
                self.assertIn('chali-editor-1.0.0.vsix', packed.namelist())
                self.assertTrue(metadata['workingTreeDirty'])
                self.assertEqual(packed.read('ChaLi.exe'), b'compiler')
                self.assertIn('examples/any_generics.chl', packed.namelist())
                self.assertIn('NOTICE', packed.namelist())
                for name in ('miniz', 'ipage', 'caspp'):
                    self.assertEqual(packed.read('licenses/' + name + '/LICENSE'), ('fixture license: ' + name).encode())
            for line in (output / 'SHA256SUMS.txt').read_text().splitlines():
                digest, name = line.split('  ', 1)
                self.assertEqual(digest, hashlib.sha256((output / name).read_bytes()).hexdigest())
            original = archive.read_bytes()
            (root / f'example/samples/{release.SAMPLES[-1]}.chl').unlink()
            with self.assertRaises(OSError):
                release.package_assets(root, output, binary, vsix, '0.21.0', 'windows-x64', 'new', False)
            self.assertEqual(archive.read_bytes(), original)
            self.assertFalse(list(output.glob('*.zip.tmp')))


if __name__ == '__main__':
    unittest.main(verbosity=2)
