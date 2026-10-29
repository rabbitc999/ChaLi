"""End-to-end checks for ChaLi's optional persistent compilation cache."""
from compiler_path import compiler_path

from pathlib import Path
import subprocess
from project_temp import tempfile
import os
import re


ROOT = Path(__file__).resolve().parents[1]
EXE = compiler_path()


def run(*args: str, success: bool = True) -> subprocess.CompletedProcess[str]:
    result = subprocess.run([str(EXE), *map(str, args)], cwd=ROOT,
                            capture_output=True, text=True, encoding="utf-8")
    if (result.returncode == 0) != success:
        raise AssertionError(f"exit={result.returncode}\n{result.stdout}\n{result.stderr}")
    return result


def main() -> None:
    if not EXE.exists():
        raise SystemExit("build the ChaLi compiler first: make -C chali")
    (ROOT / "tmp").mkdir(exist_ok=True)
    with tempfile.TemporaryDirectory(prefix="chali-incremental-", dir=ROOT / "tmp") as directory:
        work = Path(directory)
        module = work / "math" / "ops.chl"
        module.parent.mkdir()
        body = module
        source = work / "app.chl"
        source.write_text("module demo;\nimport math::ops;\n"
                          "void main() { std::lang::println(\"{}\", math::ops::twice(2)); return; }\n",
                          encoding="utf-8")
        body.write_text("module math::ops;\nint64 twice(int64 x) { return x + x; }\n", encoding="utf-8")
        output = work / "build" / "app"
        output.parent.mkdir()
        args = ("aot", source, "--emit-llvm", "--incremental", "-I", work, "-I", ROOT,
                "--out", output)

        first = run(*args)
        assert "2 个模块重新预处理" in first.stderr, first.stderr
        original = (output.with_suffix(".ll")).read_bytes()
        cached = run(*args)
        assert "增量编译：2 个模块复用" in cached.stderr, cached.stderr
        assert (output.with_suffix(".ll")).read_bytes() == original

        for unit in Path(str(output) + ".chali-cache").glob("*.unit"):
            unit.write_bytes(b"damaged cache")
        recovered = run(*args)
        assert "增量编译：0 个模块复用" in recovered.stderr, recovered.stderr

        body.write_text("module math::ops;\nint64 twice(int64 x) { return x + x + 1; }\n", encoding="utf-8")
        changed = run(*args)
        assert "1 个模块复用，1 个模块重新预处理" in changed.stderr, changed.stderr
        assert output.with_suffix(".ll").read_bytes() != original

        with output.with_suffix(".ll").open("ab") as stream:
            stream.write(b"tampered\n")
        repaired = run(*args)
        assert "2 个模块复用，0 个模块重新预处理" in repaired.stderr, repaired.stderr
        assert not output.with_suffix(".ll").read_bytes().endswith(b"tampered\n")

        module.write_text("module math::ops;\nint64 twice(int64 x) { return ; }\n",
                          encoding="utf-8")
        run(*args, success=False)
        body.write_text("module math::ops;\nint64 twice(int64 x) { return x + x + 1; }\n", encoding="utf-8")
        run(*args)
        assert "个模块复用" in run(*args).stderr

        # 预处理模块可复用，LLVM 输出按本次调试配置重新生成。
        plain_quad = output.with_suffix(".ll").read_bytes()
        debug = run(*args, "--debug")
        assert "2 个模块复用，0 个模块重新预处理" in debug.stderr, debug.stderr
        assert "增量缓存命中" not in debug.stderr
        assert b"DICompileUnit" in output.with_suffix(".ll").read_bytes()
        assert "个模块复用" in run(*args, "--debug").stderr
        assert not Path(str(output) + ".symbol_map.json").exists()

        # A common include is content-hashed once per compiler invocation.
        shared = work / "shared.chl"
        shared.write_text("module shared; // shared dependency A\n", encoding="utf-8")
        source.write_text(source.read_text(encoding="utf-8").replace('import math::ops;', 'import math::ops;\nimport shared;'), encoding="utf-8")
        module.write_text('module math::ops;\nimport shared;\nint64 twice(int64 x) { return x + x + 1; }\n', encoding="utf-8")
        run(*args)
        checked = run(*args, "--stats")
        counts = re.search(r"增量文件校验：(\d+) 次读取，(\d+) 次复用", checked.stderr)
        assert counts and int(counts[2]) >= 1, checked.stderr
        assert "个模块复用" in checked.stderr
        print("shared dependency hash reads/reuses:", counts[1], counts[2])
        # Same-size edits with restored timestamps must still invalidate across runs.
        stamp = shared.stat()
        shared.write_text("module shared; // shared dependency B\n", encoding="utf-8")
        os.utime(shared, ns=(stamp.st_atime_ns, stamp.st_mtime_ns))
        edited = run(*args)
        assert "3 个模块复用，1 个模块重新预处理" in edited.stderr, edited.stderr

        runtime = run('run', source, "--incremental", "-I", work, "-I", ROOT,
                      "--out", work / "build" / "runtime")
        assert runtime.stdout.strip() == "5", runtime.stdout

    print("ChaLi incremental compilation: OK")


if __name__ == "__main__":
    main()
