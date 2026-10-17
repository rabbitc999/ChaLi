"""Magel 裁剪构建回归：需要默认/裁剪的 magel 与 magel-emit 先完成构建。"""
from pathlib import Path
import argparse
import os
import subprocess

ROOT = Path(__file__).resolve().parents[1]
EXE = ".exe" if os.name == "nt" else ""


def run(args, succeeds=True):
    """执行命令并验证成功/失败状态，保留诊断用于定位。"""
    result = subprocess.run(list(map(str, args)), cwd=ROOT, capture_output=True,
                            text=True, encoding="utf-8", errors="replace")
    if (result.returncode == 0) != succeeds:
        raise AssertionError(f"{args}\n{result.stdout}\n{result.stderr}")
    return result


def main():
    """验证移除行为、保留能力及单文件导出的裁剪定义。"""
    parser = argparse.ArgumentParser()
    parser.add_argument("--magel", default=str(ROOT / ("magel" + EXE)))
    args = parser.parse_args()
    work = ROOT / "build/trim-tests"
    work.mkdir(parents=True, exist_ok=True)
    # 三个综合样例迁移到当前语法后应直接跑通；bigint/bigfloat 需要一行 stdin 输入，
    # 直接喂 example/expected 里 check.sh 用的同一份 .in，避免 --input 传参歧义。
    stdin_by_sample = {"bigint": "example/expected/bigint.in",
                       "bigfloat": "example/expected/bigfloat.in"}
    for sample in ["std_math_clock_random", "bigint", "bigfloat"]:
        command = [args.magel, ROOT / "example/samples" / (sample + ".mgl"),
                   "--out", work / sample]
        stdin_text = (ROOT / stdin_by_sample[sample]).read_text(encoding="utf-8") if sample in stdin_by_sample else None
        result = subprocess.run(list(map(str, command)), cwd=ROOT, capture_output=True,
                                text=True, encoding="utf-8", errors="replace",
                                input=stdin_text)
        if result.returncode != 0:
            raise AssertionError(f"{command}\n{result.stdout}\n{result.stderr}")
    control = ROOT / "example/samples/control.mgl"
    run([args.magel, control, "--out", work / "control-run"])
    # `--emit` 只在子命令形式下生效；直接文件调用会把文件后的内容当程序实参。
    run([args.magel, "emit", control, "--out", work / "control"])
    target = work / ("control" + EXE)
    run([ROOT / "build/magel-emit" / ("magel-emit" + EXE), "--quad", work / "control.quad",
         "--data", work / "control.data.bin", "--layout", work / "control.layout", "--out", target, "--cpp-output"])
    host = work / "host.cpp"
    host.write_text('#define MAGEL_NO_MAIN\n#include "control' + EXE + '.aot.cpp"\n'
                    '#include <cassert>\n'
                    'int main() { alignas(256) static unsigned char pool[2*1024*1024]; '
                    'int result=runMagel(0,nullptr,pool,sizeof(pool)); '
                    'assert(result==0); '
                    'assert(!Memory::defaultRuntimeAllocatorIfCreated()); }\n', encoding="utf-8")
    binary = work / ("host" + EXE)
    flags = ["-DNO_MAGEL_STD", "-DNO_MAGEL_BIG_INT",
             "-DNO_MAGEL_DEFAULT_POOL"]
    run(["g++", "-std=c++17", "-O0", "-pthread", *flags, host, "-o", binary])
    run([binary])
    symbols = run(["nm", "-C", binary]).stdout
    assert "bigint::BigInt::" not in symbols and "bigfloat::BigFloat::" not in symbols
    print("Magel trim checks passed")


if __name__ == "__main__":
    main()
