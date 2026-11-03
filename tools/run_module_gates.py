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
    "moves": ["chali_language_move", "chali_resource_lifetime", "chali_native_callbacks", "chali_backend_ownership"],
    "debug": ["chali_native_debug", "chali_debug_optimization"],
    "debug_remote": ["chali_native_remote_debug"],
    "debug_bitcode": ["chali_debug_bitcode_optimization"],
    "debug_libraries": ["chali_library_debug"],
    "editor_host": ["chali_vscode_extension_host"],
    "libraries": ["chali_digest", "chali_library_package", "chali_library_import", "chali_library_inline_imports", "chali_library_build", "chali_library_borrow", "chali_library_values", "chali_library_strings", "chali_library_owned_records", "chali_library_enums", "chali_library_lifetimes", "chali_library_bitcode_header", "chali_library_bitcode", "chali_library_bitcode_values", "chali_library_lto", "chali_library_data", "chali_library_moves", "chali_generator_assets"],
    "defaults": ["chali_default_arguments", "chali_default_ambiguity", "chali_default_module"],
    "interfaces": ["chali_interface_values", "chali_interface_binding_move", "chali_copy_interface", "chali_interface_copy_scalar"],
    "abi": ["chali_link_abi", "chali_abi_values", "chali_abi_contracts", "chali_c_api_contract"],
    "toolchain": ["chali_link_abi", "chali_linker", "chali_assembler", "chali_aot_assembly", "chali_package", "chali_aot_arguments", "chali_aot_project_config"],
    "warnings": ["chali_warnings", "chali_analysis_diagnostics", "chali_match_overlap", "chali_lsp_naming_scopes"],
    "stdlib": ["chali_standard_unit", "chali_precompiled", "chali_generic_scope", "chali_macro_generics"]
              + (["chali_precompiled_readonly"] if os.name == "nt" else []),
    "sql": ["chali_sqlite", "chali_sql_connections", "chali_sql_backends", "chali_reflection_serialization"],
    "net": ["chali_net_c_api", "chali_opcode_std_storage"],
    "image": ["chali_image", "chali_opcode_std_storage"],
    "collections": ["chali_collection_glue", "chali_numeric_list_storage", "chali_numeric_list_append", "chali_map_pair_iterator"],
    "scalar": ["chali_scalar_c_api", "chali_linalg_column", "chali_linalg_solve"],
    "linalg": ["chali_linalg_column", "chali_linalg_solve", "chali_linalg_decomposition"],
    "file": ["chali_file_c_api", "chali_filesystem_c_api", "chali_opcode_resources", "chali_print_streams"],
    "filesystem": ["chali_filesystem_c_api"],
    "regex": ["chali_regex_c_api"],
    "binary": ["chali_binary"],
    "signal": ["chali_signal"],
    "process": ["chali_process_c_api"],
    "compress": ["chali_compress_c_api"],
    "concurrency": ["chali_shared_selfhost", "chali_thread_wait"],
    "serialization": ["chali_serialization_selfhost", "chali_reflection_serialization", "chali_structured_data", "chali_json_derive"],
    "parse": ["chali_parse_selfhost", "chali_precompiled", "chali_glue_edges"],
    "cas": ["chali_cas_c_api", "chali_cas_math", "chali_big_number_c_api", "chali_thread_wait"],
    "big_number": ["chali_big_number_c_api", "chali_cas_math", "chali_reflection_serialization"],
    "demo": ["chali_demo_c_api", "chali_reference_values", "chali_reference_combinations"],
    "c_api": ["chali_native_marshal", "chali_c_api_contract", "chali_generator_assets", "chali_c_api_export", "chali_c_api_registry", "chali_native_callbacks", "chali_abi_contracts", "chali_abi_directives", "chali_abi_values", "chali_glue_edges"],
    "macros": ["chali_ast_macros", "chali_macro_parameters", "chali_macro_categories",
               "chali_macro_branches", "chali_macro_groups", "chali_macro_generics",
               "chali_macro_result_inference", "chali_macro_variadics", "chali_macro_patterns",
               "chali_macro_declarations", "chali_macro_members", "chali_custom_derive", "chali_value_derive", "chali_debug_derive", "chali_json_derive", "chali_declaration_sugars", "chali_macro_metadata",
               "chali_derive_contracts", "chali_derive_preview", "chali_macro_type_metadata", "chali_macro_member_contract", "chali_macro_provenance", "chali_macro_controls", "chali_macro_control_flow",
               "chali_macro_diagnostics", "chali_macro_preview", "chali_macro_control_preview", "chali_macro_effects_preview", "chali_macro_editor",
               "chali_lsp_macro_completion", "chali_lsp_macro_highlighting"],
    "language": ["chali_entry_signature", "chali_function_types", "chali_lambdas", "chali_ast_macros", "chali_macro_branches", "chali_macro_groups", "chali_macro_generics"],
    "borrow": ["chali_borrow_flow", "chali_analysis_diagnostics", "chali_reference_values"],
    "memory": ["chali_type_memory", "chali_resource_lifetime"],
    "generics": ["chali_generic_monomorphization", "chali_generic_scope", "chali_language_move", "chali_tuple_returns"],
    "reflection": ["chali_json_derive", "chali_reflection_features", "chali_reflection_metadata", "chali_reflection_declarations",
                   "chali_reflection_queries", "chali_reflection_calls", "chali_reflection_e2e", "chali_reflection_students",
                   "chali_reflection_serialization", "chali_annotations"],
    "samples": [],
    "tools": ["chali_tool", "chali_package", "chali_map_pair_iterator", "chali_generic_constructors"],
    "lsp": ["chali_lsp_actions", "chali_lsp_editor", "chali_lsp_scheduler", "chali_lsp_pipeline", "chali_lsp_snapshots", "chali_lsp_closure_types", "chali_lsp_risks", "chali_lsp_receiver_types",
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
    parser.add_argument("--build-root", type=Path, help="需要链接前端库的模块门禁使用的隔离构建目录")
    parser.add_argument("--cxx", help="模块 C++ 单元使用的编译器")
    parser.add_argument("--sysroot", type=Path, help="模块 C++ 单元使用的工具链 sysroot")
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
                if module == "macros" and script == "chali_ast_macros":
                    commands[script].append("--default")
                if script in ("chali_annotations", "chali_c_api_registry", "chali_numeric_list_storage", "chali_generator_assets", "chali_library_bitcode_header", "chali_standard_unit", "chali_backend_ownership", "chali_function_types"):
                    if args.build_root: commands[script] += ["--build-root", str(args.build_root.resolve())]
                    if args.cxx: commands[script] += ["--cxx", args.cxx]
                    if args.sysroot: commands[script] += ["--sysroot", str(args.sysroot.resolve())]
            if module == "lsp":
                for script in ("lspCancellation", "semanticProviders", "editorIntegration", "macroHighlighting"):
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
