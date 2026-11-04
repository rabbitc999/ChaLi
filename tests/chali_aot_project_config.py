"""验证工程 AOT 汇编、对象、库、搜索路径、库锁、LTO 与命令行覆盖。"""

import argparse
import os
from pathlib import Path
import subprocess

from compiler_path import compiler_path
from project_temp import tempfile

def write(path, text):
    """写入 UTF-8 文本，父目录必须已存在。"""
    path.write_text(text, encoding="utf-8")


def main():
    """在独立工程中运行配置解析、链接、搜索路径与优先级检查。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--chali", type=Path, default=compiler_path())
    args = parser.parse_args()
    compiler = args.chali.resolve()
    environment = dict(os.environ)
    with tempfile.TemporaryDirectory(prefix="aot-project-config-") as directory:
        project = Path(directory)
        (project / "src").mkdir()
        (project / "build").mkdir()
        (project / "extra").mkdir()
        (project / "native" / "include").mkdir(parents=True)
        write(project / "src" / "main.chl", 'import helper;\nvoid main(){ println("{}", twice(21)); }\n')
        write(project / "extra" / "helper.chl", "module helper;\nint64 twice(int64 value) { return value * 2; }\n")
        write(project / "native" / "include" / "extra.inc", "# project include resolved\n")
        write(project / "native" / "extra.s", '.text\n.globl chali_project_extra\n.include "extra.inc"\nchali_project_extra:\n    ret\n')
        write(project / "native" / "broken.s", '.text\n.include "missing.inc"\n')
        config = (
            '[chali]\nmain = "src/main.chl"\n\n'
            '[aot]\nassembly_sources = ["native/extra.s"]\n'
            'assembly_includes = ["native/include"]\nsearch_paths = ["extra"]\n'
        )
        write(project / "chali.toml", config)

        def run(*arguments, expected=0):
            """在工程目录运行编译器并核对退出码。"""
            result = subprocess.run([str(compiler), *map(str, arguments)], cwd=project, env=environment,
                                    capture_output=True, text=True, encoding="utf-8", errors="replace", timeout=180)
            if result.returncode != expected:
                raise AssertionError((arguments, result.returncode, result.stdout, result.stderr))
            return result

        # 清单中的汇编源、汇编包含路径和模块搜索路径生效。
        run("aot", "--out", "build/app")
        executable = project / "build" / ("app.exe" if os.name == "nt" else "app")
        assert executable.is_file(), "配置构建没有生成可执行文件"
        output = subprocess.run([str(executable)], cwd=project, capture_output=True, text=True, encoding="utf-8", timeout=60)
        assert output.returncode == 0 and output.stdout.strip() == "42", (output.returncode, output.stdout, output.stderr)

        # 关闭 LTO 和显式重新启用均需完成真实链接。
        write(project / "chali.toml", config + 'lto = false\n')
        run("aot", "--out", "build/no-lto")
        run("aot", "--lto", "--out", "build/override-lto")
        run("aot", "--no-lto", "--out", "build/explicit-no-lto")
        for name in ("no-lto", "override-lto", "explicit-no-lto"):
            executable = project / "build" / (name + (".exe" if os.name == "nt" else ""))
            output = subprocess.run([str(executable)], cwd=project, capture_output=True, text=True, encoding="utf-8", timeout=60)
            assert output.returncode == 0 and output.stdout.strip() == "42", (name, output.returncode, output.stdout, output.stderr)
        write(project / "chali.toml", config)

        # 缺少汇编包含路径时必须失败，证明确实读取了清单中的 --asm-include。
        write(project / "chali.toml", config.replace('assembly_includes = ["native/include"]\n', ""))
        run("aot", "--out", "build/no-include", expected=1)
        write(project / "chali.toml", config)

        # 缺少模块搜索路径时 import 必须失败。
        write(project / "chali.toml", config.replace('search_paths = ["extra"]\n', ""))
        run("aot", "--out", "build/no-search", expected=1)
        write(project / "chali.toml", config)

        # 命令行 --asm 覆盖清单里的汇编源：清单指向坏文件，命令行给出正确文件。
        broken = config.replace('assembly_sources = ["native/extra.s"]', 'assembly_sources = ["native/broken.s"]')
        write(project / "chali.toml", broken)
        run("aot", "--out", "build/broken", expected=1)
        run("aot", "--asm", "native/extra.s", "--out", "build/override")
        assert (project / "build" / ("override.exe" if os.name == "nt" else "override")).is_file(), "命令行汇编源没有覆盖清单"

        # 清单中的未知库名进入最终链接并失败，说明 libraries 已被使用。
        write(project / "chali.toml", config + 'libraries = ["chali_no_such_library"]\n')
        result = run("aot", "--out", "build/bad-library", expected=1)
        assert "chali_no_such_library" in (result.stdout + result.stderr), "链接错误没有提到清单里的库"
        # 对象路径相对清单解析，缺失对象必须进入链接诊断。
        write(project / "chali.toml", config + 'objects = ["native/missing.o"]\n')
        result = run("aot", "--out", "build/bad-object", expected=1)
        assert "missing.o" in (result.stdout + result.stderr), "链接错误没有提到清单里的对象"
        # 独立汇编生成真实对象，再由清单交给最终链接。
        run("asm", "native/extra.s", "--out", "native/extra.o", "-I", "native/include")
        objects = config.replace('assembly_sources = ["native/extra.s"]', 'objects = ["native/extra.o"]')
        write(project / "chali.toml", objects)
        run("aot", "--out", "build/with-object")
        executable = project / "build" / ("with-object.exe" if os.name == "nt" else "with-object")
        output = subprocess.run([str(executable)], cwd=project, capture_output=True, text=True, encoding="utf-8", timeout=60)
        assert output.returncode == 0 and output.stdout.strip() == "42", (output.returncode, output.stdout, output.stderr)
    print("工程 AOT 配置：汇编、对象、搜索路径、库、LTO 与命令行覆盖通过")


if __name__ == "__main__":
    main()
