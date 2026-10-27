"""按模块执行门禁，并以可复现随机种子抽样复测已通过的门禁。"""
from compiler_path import compiler_path
import argparse
import json
import os
from pathlib import Path
import random
import sys
from project_temp import tempfile
import time

from parallel_gates import default_jobs, positive_jobs, run_tasks

ROOT = Path(__file__).resolve().parents[1]
MODULES = {
    "language": ["chali_entry_signature", "chali_function_types", "chali_lambdas", "chali_ast_macros", "chali_macro_branches", "chali_macro_groups", "chali_macro_generics"],
    "borrow": ["chali_borrow_flow", "chali_analysis_diagnostics", "chali_reference_values"],
    "memory": ["chali_memory_guard", "chali_resource_lifetime", "chali_watchdog"],
    "generics": ["chali_generic_monomorphization", "chali_language_move", "chali_tuple_returns"],
    "samples": [],
    "tools": ["chali_tool", "chali_package", "chali_map_pair_iterator", "chali_generic_constructors"],
    "lsp": ["chali_lsp_closure_types", "chali_lsp_risks", "chali_lsp_receiver_types",
            "chali_lsp_overloads", "chali_lsp_naming_scopes", "chali_lsp_symbols",
            "chali_lsp_lambda_capture", "chali_lsp_semantic_features",
            "chali_lsp_macro_completion", "chali_lsp_macro_highlighting",
            "chali_native_lsp_parity", "chali_lsp_performance", "chali_lsp_highlight_speed"],
}


def main():
    """运行指定模块，逐项保存日志与结果，失败时返回非零状态。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("modules", nargs="+", choices=[*MODULES, "docs"])
    parser.add_argument("--chali", type=Path, default=compiler_path())
    parser.add_argument("--random-recheck", type=int, default=0, help="从已通过门禁抽样复测的次数")
    parser.add_argument("--seed", type=int, default=20261013)
    parser.add_argument("--jobs", type=positive_jobs, default=None, help="并行工作数，默认最多四个；1 为串行")
    args = parser.parse_args()
    try:
        jobs = args.jobs if args.jobs is not None else default_jobs()
    except argparse.ArgumentTypeError as error:
        parser.error(str(error))
    if args.random_recheck < 0:
        parser.error("随机复测次数必须非负")
    base = ROOT / "tmp" / "module-gates"
    base.mkdir(parents=True, exist_ok=True)
    output = Path(tempfile.mkdtemp(prefix=time.strftime("%Y%m%d-%H%M%S-"), dir=base))
    commands = {}
    for module in args.modules:
        if module == "samples":
            commands["samples"] = ["sh", "check.sh"]
        elif module == "docs":
            for script, flags in (("check_doc_links", []), ("export_chali_ebnf", ["--check"]),
                                  ("library_reference", ["--check"])):
                commands[script] = [sys.executable, str(ROOT / "tools" / (script + ".py")), *flags]
        else:
            for script in MODULES[module]:
                commands[script] = [sys.executable, str(ROOT / "tests" / (script + ".py")),
                                    "--chali", str(args.chali.resolve())]
                if module == "lsp" and script == "chali_lsp_closure_types":
                    commands[script].append("--lsp-only")
            if module == "lsp":
                for script in ("lspCancellation", "semanticProviders", "macroHighlighting"):
                    commands[script] = ["node", str(ROOT / "vscode" / "test" / (script + ".js"))]
    environment = dict(DEMO=args.chali.resolve().as_posix(), CHALI_COMPILER=str(args.chali.resolve()),
                       CHALI_TEST_COMPILER=str(args.chali.resolve()))
    tasks = [dict(title=name, name=name, command=command, recheck=False)
             for name, command in commands.items()]
    results = run_tasks(tasks, output, jobs, ROOT, environment, dict(seed=args.seed))
    # 按登记顺序抽样，不让完成顺序改变相同种子的复测选择。
    passed = [item["name"] for item in results if item["exit_code"] == 0]
    rng = random.Random(args.seed)
    rechecks = []
    for _ in range(args.random_recheck):
        if not passed:
            break
        name = rng.choice(passed)
        # 同名脚本可能使用固定工作目录，复测必须串行。
        rechecks.append(dict(title="随机复测 " + name, name=name, command=commands[name],
                             recheck=True, parallel=False))
    if rechecks:
        repeated = run_tasks(rechecks, output / "rechecks", 1, ROOT, environment, dict(seed=args.seed),
                             index_offset=len(results))
        results.extend(repeated)
        (output / "results.json").write_text(json.dumps(dict(seed=args.seed, jobs=jobs, results=results),
                                                       ensure_ascii=False, indent=2), encoding="utf-8")
    return int(any(result["exit_code"] != 0 for result in results))


if __name__ == "__main__":
    sys.exit(main())
