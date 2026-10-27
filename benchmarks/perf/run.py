#!/usr/bin/env python3
"""跨语言微基准驱动：同一份算法在各语言下各跑若干轮，取中位数并核对校验值。

用法（仓库根目录）：

    python benchmarks/perf/run.py             # 全部配置，3 次进程 × 5 轮
    python benchmarks/perf/run.py --runs 5    # 跑 5 次进程
    python benchmarks/perf/run.py --only chali,c,java-jit

每份程序自己用单调时钟包住 `work()`，打印若干行 `rep <纳秒>` 和一行 `sink <校验值>`；
脚本只做构建、调度、中位数统计与校验值比对，不参与计时。
"""

import argparse
import hashlib
import json
import os
import platform
import re
import shutil
import statistics
import subprocess
import sys
from project_temp import tempfile
from datetime import datetime
from pathlib import Path
from compare_vm import compiler_version
from build_aot import llvm_compiler
from build_lua import HOST as LUA_HOST, VERSION as LUA_VERSION
from compiler_path import compiler_path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent
CASES = ["int_chain", "float_chain", "call", "array", "list"]
CASE_TITLE = {
    "int_chain": "整数依赖链（Lehmer 乘加）",
    "float_chain": "浮点依赖链（乘加）",
    "call": "小函数调用",
    "array": "固定数组循环读取",
    "list": "动态数组 push + 遍历",
    "array_rw": "固定数组读写",
}


def find(name):
    return shutil.which(name)


class Config:
    def __init__(self, key, label, run, build=None, note="", cwd=None):
        self.key = key
        self.label = label
        self.run = run          # (case) -> [cmd...]
        self.build = build      # (case) -> [cmd...] or None
        self.note = note
        self.cwd = cwd          # 单独的工作目录（ChaLi 会把中间产物写到 ./.chali）


def java_cmd(java, mode):
    if mode == "xint":
        return lambda case: [java, "-Xint", "-cp", str(HERE), case]
    return lambda case: [java, "-Xbatch", "-cp", str(HERE), case]


def build_configs():
    configs = []
    chali = os.environ.get("CHALI") or str(compiler_path())
    version = compiler_version(Path(chali).resolve())
    scratch = ROOT / "tmp" / "perf-run"
    scratch.mkdir(parents=True, exist_ok=True)
    configs.append(Config("chali-run", f"ChaLi {version}（LLVM AOT，checked run）",
                          lambda case: [chali, "run", str(HERE / f"{case}.chl")],
                          note="缺省 checked AOT：边界与存活检查保留，进程内静态 LLD", cwd=str(scratch)))
    if llvm_compiler():
        configs.append(Config("chali-aot", f"ChaLi {version}（LLVM AOT，unchecked 导出）",
                              lambda case: [str(HERE / f"{case}-aot.exe")],
                              lambda case: [sys.executable, str(HERE / "build_aot.py"), case, chali],
                              note="同一 ChaLi 计算函数直接生成 LLVM 模块，LLVM 默认 O3、unchecked；C 壳计时，初始化不计时；共享原生标准库"))
    if find("gcc"):
        configs.append(Config("c", "C（gcc -O2）",
                              lambda case: [str(HERE / f"{case}-c.exe")],
                              lambda case: [find("gcc"), "-O2", "-DNDEBUG", str(HERE / f"{case}.c"),
                                            "-o", str(HERE / f"{case}-c.exe")],
                              note="原生机器码基线；volatile 函数入口保留计时边界"))
    if find("g++"):
        configs.append(Config("cpp", "C++17（g++ -O2）",
                              lambda case: [str(HERE / f"{case}-cpp.exe")],
                              lambda case: [find("g++"), "-O2", "-DNDEBUG", str(HERE / f"{case}.cpp"),
                                            "-o", str(HERE / f"{case}-cpp.exe")],
                              note="原生机器码；volatile 函数入口保留计时边界"))
        configs.append(Config("cpp-o3", "C++17（g++ -O3）",
                              lambda case: [str(HERE / f"{case}-cpp-o3.exe")],
                              lambda case: [find("g++"), "-O3", "-DNDEBUG", str(HERE / f"{case}.cpp"),
                                            "-o", str(HERE / f"{case}-cpp-o3.exe")],
                              note="O3 原生参考；保留 O2 配置及历史数据，volatile 函数入口保留计时边界"))
    if find("gcc"):
        configs.append(Config("lua", f"Lua {LUA_VERSION}（官方 VM，默认 GC）",
                              lambda case: [str(LUA_HOST), str(HERE / f"{case}.lua")],
                              lambda case: [sys.executable, str(HERE / "build_lua.py")],
                              note="官方 Lua VM，GCC -O2；64 位整数/双精度，默认 GC；C 宿主仅提供单调时钟，table 使用一基索引"))
    if find("rustc"):
        configs.append(Config("rust", "Rust（rustc -O）",
                              lambda case: [str(HERE / f"{case}-rs.exe")],
                              lambda case: [find("rustc"), "-O", str(HERE / f"{case}.rs"),
                                            "-o", str(HERE / f"{case}-rs.exe")],
                              note="原生机器码；volatile 函数入口保留计时边界"))
    if find("go"):
        configs.append(Config("go", "Go（默认优化）",
                              lambda case: [str(HERE / f"{case}-go.exe")],
                              lambda case: [find("go"), "build", "-o", str(HERE / f"{case}-go.exe"),
                                            str(HERE / f"{case}.go")],
                              note="原生机器码，带 GC"))
    if find("java") and find("javac"):
        configs.append(Config("java-xint", "Java 22 `-Xint`（纯解释）",
                              java_cmd(find("java"), "xint"),
                              lambda case: [find("javac"), "-d", str(HERE), str(HERE / f"{case}.java")],
                              note="HotSpot 解释器，关即时编译"))
        configs.append(Config("java-jit", "Java 22 `-Xbatch`（即时编译）",
                              java_cmd(find("java"), "jit"),
                              note="同一份 class，分层编译，同步编译"))
    if find("node"):
        configs.append(Config("node", "Node 26（V8 即时编译）",
                              lambda case: [find("node"), str(HERE / f"{case}.js")],
                              note="JIT，无预热控制"))
    if find("python"):
        configs.append(Config("python", "CPython 3.12（纯解释）",
                              lambda case: [find("python"), str(HERE / f"{case}.py")],
                              note="默认解释器，无 JIT"))
    return configs


REP_RE = re.compile(r"^rep\s+(\d+)\s*$")
SINK_RE = re.compile(r"^sink\s+(\S+)\s*$")


def run_once(cmd, cwd):
    proc = subprocess.run(cmd, cwd=cwd, capture_output=True, text=True, encoding="utf-8", errors="replace")
    if proc.returncode != 0:
        raise RuntimeError(f"命令失败（{proc.returncode}）：{' '.join(cmd)}\n{proc.stderr[-2000:]}")
    reps, sink = [], None
    for line in proc.stdout.splitlines():
        line = line.strip()
        m = REP_RE.match(line)
        if m:
            reps.append(int(m.group(1)))
            continue
        m = SINK_RE.match(line)
        if m:
            sink = m.group(1)
    if not reps or sink is None:
        raise RuntimeError(f"输出不符合约定：{' '.join(cmd)}\n{proc.stdout[-2000:]}")
    return reps, sink


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--runs", type=int, default=3, help="每个配置跑几次进程（每次内含 5 轮）")
    parser.add_argument("--only", default="", help="只跑这些配置，逗号分隔")
    parser.add_argument("--out", default="", help="结果目录")
    parser.add_argument("--cases", default=",".join(CASES), help="负载名称，逗号分隔；array_rw 需选择提供该负载的配置")
    args = parser.parse_args()
    cases = [item.strip() for item in args.cases.split(",") if item.strip()]
    if not cases or any(case not in CASE_TITLE for case in cases):
        parser.error("未知或空的负载名称")

    configs = build_configs()
    if args.only:
        wanted = [item.strip() for item in args.only.split(",") if item.strip()]
        configs = [c for c in configs if c.key in wanted]
    if not configs:
        parser.error("没有可运行的配置")

    print(f"配置 {len(configs)} 个：{', '.join(c.key for c in configs)}")
    unavailable = {}
    for cfg in configs:
        if cfg.build is None:
            continue
        for case in cases:
            print(f"  构建 {cfg.key}/{case} …", flush=True)
            proc = subprocess.run(cfg.build(case), capture_output=True, text=True, encoding="utf-8", errors="replace",
                                  env=dict(os.environ, PYTHONIOENCODING="utf-8"))
            if cfg.key == "chali-aot" and case == "list" and proc.returncode == 77:
                unavailable.setdefault(cfg.key, {})[case] = proc.stdout + proc.stderr
                print(f"  {cfg.key}/{case}: 不支持，保存诊断", flush=True)
                continue
            if proc.returncode != 0:
                print(proc.stdout[-1500:])
                print(proc.stderr[-1500:])
                sys.exit(f"构建失败：{cfg.key}/{case}")

    results = {}
    sinks = {}
    for cfg in configs:
        for case in cases:
            if case in unavailable.get(cfg.key, {}):
                continue
            samples, checked = [], None
            for _ in range(args.runs):
                reps, sink = run_once(cfg.run(case), cfg.cwd or str(HERE))
                samples.extend(reps)
                if checked is None:
                    checked = sink
                elif checked != sink:
                    sys.exit(f"校验值不一致：{cfg.key}/{case} {checked} != {sink}")
            results.setdefault(cfg.key, {})[case] = samples
            sinks.setdefault(case, {})[cfg.key] = checked
            print(f"  {cfg.key:<10} {case:<12} 中位 {statistics.median(samples)/1e6:9.3f} ms", flush=True)

    # 校验值必须跨语言一致：这是"各语言确实算了同样的活"的唯一证据。
    for case, table in sinks.items():
        values = set(table.values())
        if len(values) != 1:
            sys.exit(f"校验值跨语言不一致：{case} → {table}")

    stamp = datetime.now().strftime("%Y%m%d-%H%M%S")
    host = platform.node() or "host"
    out_dir = Path(args.out) if args.out else ROOT / "benchmarks" / "results" / f"{host.lower()}-{stamp}-perf"
    out_dir.mkdir(parents=True, exist_ok=True)

    summary = {
        "generated": stamp,
        "host": host,
        "platform": platform.platform(),
        "processor": platform.processor(),
        "python": platform.python_version(),
        "runs": args.runs,
        "reps_per_run": 5,
        "cases": cases,
        "sinks": sinks,
        "configs": {},
    }
    compiler = Path(os.environ.get("CHALI") or compiler_path()).resolve()
    summary["chali_compiler"] = {"reported_version": compiler_version(compiler),
                                 "sha256": hashlib.sha256(compiler.read_bytes()).hexdigest()}
    summary["tool_versions"] = {}
    summary["llvm_dependency"] = json.loads((ROOT / "library/llvm/manifest.json").read_text(encoding="utf-8"))
    summary["lua_dependency"] = json.loads((ROOT / "library/lua/manifest.json").read_text(encoding="utf-8"))
    llvm = llvm_compiler()
    if llvm:
        result = subprocess.run([llvm, "--version"], capture_output=True, text=True, encoding="utf-8", errors="replace")
        summary["tool_versions"]["llvm-clang"] = (result.stdout + result.stderr).strip()
    if any(cfg.key == "lua" for cfg in configs):
        summary["tool_versions"]["lua"] = subprocess.check_output([str(LUA_HOST), "--version"], text=True).strip()
        summary["lua_host_sha256"] = hashlib.sha256(LUA_HOST.read_bytes()).hexdigest()
    for tool, flag in [("gcc", "--version"), ("g++", "--version"), ("rustc", "--version"),
                       ("go", "version"), ("java", "-version"), ("node", "--version"), ("python", "--version")]:
        executable = find(tool)
        if executable:
            result = subprocess.run([executable, flag], capture_output=True, text=True, encoding="utf-8", errors="replace")
            summary["tool_versions"][tool] = (result.stdout + result.stderr).strip()
    for cfg in configs:
        available = [case for case in cases if case not in unavailable.get(cfg.key, {})]
        summary["configs"][cfg.key] = {
            "label": cfg.label,
            "note": cfg.note,
            "median_ms": {case: round(statistics.median(results[cfg.key][case]) / 1e6, 3) for case in available},
            "min_ms": {case: round(min(results[cfg.key][case]) / 1e6, 3) for case in available},
            "max_ms": {case: round(max(results[cfg.key][case]) / 1e6, 3) for case in available},
            "samples_ns": {case: results[cfg.key][case] for case in available},
            "unavailable": unavailable.get(cfg.key, {}),
        }
    (out_dir / "results.json").write_text(json.dumps(summary, ensure_ascii=False, indent=2), encoding="utf-8")

    base = "c" if "c" in summary["configs"] else configs[0].key
    base_label = "C" if base == "c" else summary["configs"][base]["label"]
    lines = ["| 配置 | " + " | ".join(CASE_TITLE[c] for c in cases) + " |",
             "| --- | " + " | ".join("---:" for _ in cases) + " |"]
    for cfg in configs:
        cells = []
        for case in cases:
            if case in unavailable.get(cfg.key, {}):
                cells.append("不支持")
                continue
            ms = summary["configs"][cfg.key]["median_ms"][case]
            cells.append(f"{ms:.3f}")
        lines.append(f"| {cfg.label} | " + " | ".join(cells) + " |")
    table = "\n".join(lines)

    rel = ["| 配置 | " + " | ".join(CASE_TITLE[c] + " ÷ " + base_label for c in cases) + " |",
           "| --- | " + " | ".join("---:" for _ in cases) + " |"]
    for cfg in configs:
        cells = []
        for case in cases:
            if case in unavailable.get(cfg.key, {}):
                cells.append("不支持")
                continue
            mine = statistics.median(results[cfg.key][case]) / 1e6
            theirs = statistics.median(results[base][case]) / 1e6
            cells.append("1.00" if theirs <= 0 or cfg.key == base else f"{mine / theirs:.1f}")
        rel.append(f"| {cfg.label} | " + " | ".join(cells) + " |")

    (out_dir / "summary.md").write_text(
        f"# 跨语言微基准（{stamp}）\n\n中位数（ms，越小越快），每格来自 {args.runs} 次进程 × 5 轮。\n\n{table}\n\n"
        f"相对 {base_label} 的倍数（同一列除以基线中位数）：\n\n" + "\n".join(rel) + "\n",
        encoding="utf-8")

    print("\n" + table)
    print(f"\n相对 {base_label} 的倍数：")
    print("\n".join(rel))
    print(f"\n结果目录：{out_dir}")


if __name__ == "__main__":
    main()
