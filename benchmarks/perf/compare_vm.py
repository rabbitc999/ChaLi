"""测量当前编译器或交错对照两份编译器，保存原始计时和结果校验。"""
import argparse
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import platform
import statistics
import subprocess
import time
from project_temp import tempfile
ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
CASES = ["call", "array", "array_rw", "list"]


def compiler_version(compiler):
    """从编译器自身的 LSP 初始化响应读取版本，不用工作区版本标记旧产物。"""
    messages = [{"jsonrpc": "2.0", "id": 1, "method": "initialize", "params": {}},
                {"jsonrpc": "2.0", "id": 2, "method": "shutdown", "params": None},
                {"jsonrpc": "2.0", "method": "exit", "params": None}]
    chunks = [json.dumps(message).encode() for message in messages]
    wire = b"".join(("Content-Length: " + str(len(chunk)) + "\r\n\r\n").encode() + chunk for chunk in chunks)
    result = subprocess.run([str(compiler), "lsp"], input=wire, capture_output=True, timeout=30, check=True)
    output = result.stdout
    while output:
        header, body = output.split(b"\r\n\r\n", 1)
        size = next(int(line.split(b":", 1)[1]) for line in header.split(b"\r\n") if line.lower().startswith(b"content-length:"))
        message = json.loads(body[:size])
        if message.get("id") == 1:
            return message["result"]["serverInfo"]["version"]
        output = body[size:]
    raise RuntimeError("LSP 初始化没有返回编译器版本")


def measure(compiler, case, work):
    """执行一次独立进程，取得程序内部的五轮计时与确定性校验值。"""
    command = [str(compiler), "run", str(HERE / (case + ".chl"))]
    started = time.perf_counter_ns()
    result = subprocess.run(command,
                            cwd=work, env=dict(os.environ, CHALI_LANG="zh"),
                            capture_output=True, text=True, encoding="utf-8", timeout=900 if case == "list" else 180)
    if result.returncode:
        raise RuntimeError(result.stderr)
    times = [int(line.split()[1]) for line in result.stdout.splitlines() if line.startswith("rep ")]
    sinks = [line.split()[1] for line in result.stdout.splitlines() if line.startswith("sink ")]
    if len(times) != 5 or len(sinks) != 1:
        raise RuntimeError(result.stdout)
    return times, sinks[0], dict(command=command, stdout=result.stdout, stderr=result.stderr,
                                wall_ns=time.perf_counter_ns()-started)


def main():
    """采集当前版本；提供旧编译器时按轮次交错执行顺序。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--before", type=Path)
    parser.add_argument("--after", type=Path, required=True)
    parser.add_argument("--runs", type=int, default=3)
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--cases", nargs="+", choices=CASES, default=CASES)
    args = parser.parse_args()
    if args.runs < 1:
        parser.error("--runs 必须为正数")
    compilers = {"after": args.after.resolve()}
    if args.before:
        compilers = {"before": args.before.resolve(), **compilers}
    report = {"generated_utc": datetime.now(timezone.utc).isoformat(),
              "host": platform.node(), "platform": platform.platform(),
              "processor": platform.processor(), "cpu_count": os.cpu_count(), "runs": args.runs,
              "unit": "nanoseconds", "compiler": {}, "cases": {}}
    for key, compiler in compilers.items():
        report["compiler"][key] = {"path": str(compiler), "reported_version": compiler_version(compiler),
                                   "sha256": hashlib.sha256(compiler.read_bytes()).hexdigest()}
    with tempfile.TemporaryDirectory(prefix="chali-vm-compare-") as work:
        for case in args.cases:
            entry = {key: [] for key in compilers}
            entry["processes"] = {key: [] for key in compilers}
            entry["source_sha256"] = hashlib.sha256((HERE/(case+'.chl')).read_bytes()).hexdigest()
            entry["sink"] = None
            for run in range(args.runs):
                for key in (list(compilers) if run % 2 == 0 else list(reversed(compilers))):
                    times, sink, process = measure(compilers[key], case, work)
                    if entry["sink"] is not None and sink != entry["sink"]:
                        raise RuntimeError(f"{case}: 校验值不一致")
                    entry["sink"] = sink
                    entry[key].extend(times)
                    entry['processes'][key].append(process)
            entry["after_median"] = statistics.median(entry["after"])
            if args.before:
                entry["before_median"] = statistics.median(entry["before"])
                entry["speedup"] = entry["before_median"] / entry["after_median"]
            report["cases"][case] = entry
            args.out.parent.mkdir(parents=True, exist_ok=True)
            args.out.write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
            print(f"{case}: {entry['after_median'] / 1e6:.3f} ms", flush=True)
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


if __name__ == "__main__":
    main()
