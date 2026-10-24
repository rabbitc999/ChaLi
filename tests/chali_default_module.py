"""验证用户文件的默认 program 模块、显式模块及跨文件引用。"""
from compiler_path import compiler_path
import argparse
import os
from pathlib import Path
import subprocess
from project_temp import tempfile
ROOT = Path(__file__).resolve().parents[1]


def main():
    """通过真实编译执行检查默认身份，不用文件名推断用户模块。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--chali", type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    with tempfile.TemporaryDirectory(prefix="chali-default-module-") as directory:
        folder = Path(directory)
        (folder / "program.chl").write_text("int64 answer() { return 42; }\n", encoding="utf-8")
        cases = {
            "implicit": 'int64 answer() { return 42; }\nvoid main() { assert(program::answer() == 42); println($module); }',
            "explicit": 'module custom;\nint64 answer() { return 42; }\nvoid main() { assert(custom::answer() == 42); println($module); }',
            "explicit_case": 'module customCase;\nint64 answer() { return 42; }\nvoid main() { assert(customCase::answer() == 42); println($module); }',
            "import": 'module consumer;\nimport program;\nvoid main() { assert(program::answer() == 42); println($module); }',
        }
        for name, source in cases.items():
            path = folder / (name + ".chl")
            path.write_text(source + "\n", encoding="utf-8")
            result = subprocess.run([str(compiler), "run", str(path), "--out", str(folder / (name + "-out"))], cwd=ROOT,
                                    env=dict(os.environ, CHALI_LANG="zh", CHALI_NO_STDIN_INPUT="1"), capture_output=True, text=True, encoding="utf-8", timeout=120)
            expected = {"explicit": "custom", "explicit_case": "customCase", "import": "consumer"}.get(name, "program")
            assert (result.returncode, result.stdout, result.stderr) == (0, expected + "\n", ""), (name, result)
    print("默认 program 模块：限定调用、显式声明、跨文件导入与 $module 通过")


if __name__ == "__main__":
    main()
