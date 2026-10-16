"""验证生成器下载成功、网络失败及错误响应时的文件替换与清理。"""
import io
import os
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch
import urllib.error

ROOT = Path(__file__).resolve().parents[1]
DOWNLOAD = (ROOT / "gen.sh").read_text(encoding="utf-8").split("python - <<'PY'\n", 1)[1].split("\nPY\n", 1)[0]


class DownloadTests(unittest.TestCase):
    """在隔离目录执行 gen.sh 中的实际下载代码。"""

    def run_download(self, response, expected=None):
        """模拟下载并检查只在内容有效时替换生成器。"""
        previous = Path.cwd()
        with tempfile.TemporaryDirectory(prefix="magel-generator-") as directory:
            try:
                os.chdir(directory)
                executable = Path("ParseEngine.exe")
                executable.write_bytes(b"MZprevious")
                arguments = {"side_effect": response} if isinstance(response, Exception) else {"return_value": response}
                with patch("urllib.request.urlopen", **arguments) as request:
                    if expected is None:
                        exec(compile(DOWNLOAD, "gen.sh:download", "exec"), {})
                    else:
                        with self.assertRaisesRegex(SystemExit, expected):
                            exec(compile(DOWNLOAD, "gen.sh:download", "exec"), {})
                    request.assert_called_once_with(
                        "https://gitee.com/rabbitc999/ParseEngine/releases/download/latest/ParseEngine.exe",
                        timeout=60,
                    )
                self.assertEqual(executable.read_bytes(), b"MZlatest" if expected is None else b"MZprevious")
                self.assertEqual(sorted(path.name for path in Path.cwd().iterdir()), ["ParseEngine.exe"])
            finally:
                os.chdir(previous)

    def test_latest_release(self):
        """有效响应替换旧文件并删除临时文件。"""
        self.run_download(io.BytesIO(b"MZlatest"))

    def test_network_failure(self):
        """网络失败保留旧文件并清理临时文件。"""
        self.run_download(urllib.error.URLError("offline"), "下载 ParseEngine 失败")

    def test_html_response(self):
        """成功状态下返回网页也不能覆盖生成器。"""
        self.run_download(io.BytesIO(b"<html>error</html>"), "不是 Windows 可执行文件")

    def test_empty_response(self):
        """空下载不能替换生成器。"""
        self.run_download(io.BytesIO(), "不是 Windows 可执行文件")


if __name__ == "__main__":
    unittest.main()
