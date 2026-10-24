"""为构建期内嵌资产维护输入与产物的 SHA-256 清单。"""
from hashlib import file_digest, sha256
import json
from pathlib import Path
from project_temp import tempfile
import os

ROOT = Path(__file__).resolve().parents[1]


class AssetCache:
    """只按内容复用产物；时间戳不参与判断，损坏或缺失的文件必须重建。"""

    def __init__(self, name, inputs, directory=None):
        """计算命名输入摘要，输入可以是原始字节或需要读取的文件路径。"""
        self.path = Path(directory or ROOT / 'build/embedded-sha256') / (name + '.sha256')
        self.lines = ['# ChaLi embedded assets SHA256 v1\n']
        values = dict(inputs)
        values['generator:embedded_asset_cache.py'] = Path(__file__)
        for key, value in sorted(values.items()):
            digest = self.digest(value)
            self.lines.append(f'{digest}  {json.dumps("input:" + key, ensure_ascii=False)}\n')

    @staticmethod
    def digest(value):
        """流式散列磁盘文件，内存输入直接散列其真实字节。"""
        if isinstance(value, bytes):
            return sha256(value).hexdigest()
        with Path(value).open('rb') as stream:
            return file_digest(stream, 'sha256').hexdigest()

    def manifest(self, outputs):
        """生成包含每个产物摘要的清单，产物不存在时不允许命中缓存。"""
        lines = list(self.lines)
        for path in sorted(map(Path, outputs)):
            name = path.resolve().as_posix()
            try:
                name = path.resolve().relative_to(ROOT).as_posix()
            except ValueError:
                pass
            lines.append(f'{self.digest(path)}  {json.dumps("output:" + name, ensure_ascii=False)}\n')
        return ''.join(lines)

    def valid(self, outputs):
        """输入、生成器及全部输出的 SHA-256 均相同时才允许跳过生成。"""
        try:
            return self.path.read_text(encoding='utf-8') == self.manifest(outputs)
        except (OSError, UnicodeError):
            return False

    def save(self, outputs):
        """成功生成后原子替换摘要文件，不在失败时写入可命中的缓存。"""
        text = self.manifest(outputs)
        self.path.parent.mkdir(parents=True, exist_ok=True)
        temporary = None
        try:
            with tempfile.NamedTemporaryFile(dir=self.path.parent, prefix='.sha256-', delete=False) as stream:
                temporary = stream.name
                stream.write(text.encode('utf-8'))
            os.replace(temporary, self.path)
        finally:
            if temporary and os.path.exists(temporary):
                os.unlink(temporary)
