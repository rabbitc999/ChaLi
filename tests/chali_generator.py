"""Focused checks for the C++ ChaLi toolchain generator."""
from compiler_path import compiler_path

import argparse
import os
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import sys


ROOT = Path(__file__).resolve().parents[1]
VERSION = (ROOT / "VERSION").read_text(encoding="utf-8").strip()
EXPORTED_COMPILER = f"chali-{VERSION}-llvm-external-windows-x64.exe" if os.name == "nt" else "chali"
GENERATOR = compiler_path()
WORK = ROOT / "tmp" / "chali-generator-tests"


def command(arguments, *, succeeds=True):
    """通过统一工具链生成或按清单清理工程。"""
    arguments = list(arguments)
    if arguments[1] != "clear":
        arguments.insert(1, "gen")
    result = subprocess.run(
        [str(item) for item in arguments], cwd=ROOT, text=True,
        encoding="utf-8", errors="replace", capture_output=True, check=False,
    )
    if (result.returncode == 0) != succeeds:
        raise AssertionError(
            f"command returned {result.returncode}: {' '.join(map(str, arguments))}\n"
            f"{result.stdout}\n{result.stderr}"
        )
    return result


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--reuse-build',action='store_true',help='复验失败后保留的独立构建，仅替换与新导出内容不同的受管理文件')
    options=parser.parse_args()
    os.environ.setdefault("CHALI_LLVM_CLANG", str(ROOT / "build/dependencies/llvm/toolchain/bin/clang.exe") if os.name == "nt" else "clang")
    if not GENERATOR.exists():
        raise SystemExit(f"build chali first: {GENERATOR}")
    assert WORK.resolve().is_relative_to((ROOT/'tmp').resolve())
    if options.reuse_build:
        assert (WORK/'compiler/.chali-generator-manifest').is_file() and (WORK/'compiler/build').is_dir(), '需要失败后保留的完整工程和构建目录'
    else:
        shutil.rmtree(WORK, ignore_errors=True)
        WORK.mkdir(parents=True)
    try:
        command([GENERATOR, "compiler", "--out-dir", WORK / "removed-robot", "--module", "robot"], succeeds=False)
        assert not (WORK / "removed-robot").exists()
        command([GENERATOR, "compiler", "--out-dir", WORK / "removed-utils", "--module", "utils"], succeeds=False)
        assert not (WORK / "removed-utils").exists()
        compiler = WORK / "compiler"
        if options.reuse_build:
            updated=WORK/'updated-export'
            command([GENERATOR,'compiler','--out-dir',updated])
            manifest='.chali-generator-manifest'
            previous=(compiler/manifest).read_text(encoding='utf-8').splitlines()
            current=(updated/manifest).read_text(encoding='utf-8').splitlines()
            assert previous==current, '复用只接受相同文件清单；清单变化需全新构建'
            changed=[]
            for relative in current[1:]+[manifest]:
                destination=compiler/relative
                assert destination.resolve().is_relative_to(compiler.resolve()), relative
                source=updated/relative
                assert source.resolve().is_relative_to(updated.resolve()), relative
                if not destination.is_file() or destination.read_bytes()!=source.read_bytes():
                    shutil.copy2(source,destination)
                    changed.append(relative)
            print('独立工程增量复验，已核对全部受管理文件，仅重新生成内容不同项：'+', '.join(changed),flush=True)
        else:
            command([GENERATOR, "compiler", "--out-dir", compiler])
        required = [
            "chali_parser.cpp", "chali_main.cpp", "chali_incremental.hpp", "chali_aot.hpp", "chali_aot.cpp",
            "chali_aot_debug.hpp", "chali_aot_debug.cpp", "chali_aot_llvm.hpp", "chali_aot_llvm.cpp", "aot/linker_assets.txt",
            "aot/runtime.c", "aot/runtime_windows.ll", "aot/runtime_linux.ll",
            "chali_interpolation.hpp", "chali_analysis.hpp", "chali_report.hpp",
            "chali_borrow_checker.hpp", "chali_borrow_flow.hpp", "chali_resource_cleanup.hpp", "chali_resource_cleanup.cpp", "semantics/resource_lifetime.cpp", "semantics/borrow_flow.cpp", "semantics/borrow_return_sources.cpp", "semantics/borrow_facts.cpp", "semantics/expression_effects.cpp",
            "chali_package.hpp", "chali_project.hpp", "chali_cli.hpp", "chali_i18n.hpp", "console/utf8.hpp",
            "runtime/event_service.cpp", "runtime/event_service.hpp", "runtime/execution_plan.hpp", "runtime/allocation_index.hpp",
            "runtime/data_capacity_index.hpp",
             "runtime/chali_pool.c", "runtime/chali_pool.h",
            "runtime/chali_allocator.hpp", "chali_runtime_allocator.hpp",
            "runtime/ast.cpp", "runtime/ast.hpp", "aot/backend_profile.cpp", "CMakeLists.txt", "std/lang.chl",
            "abi/chali_standard_internal.h", "abi/chali_abi.h", "abi/abi_file.cpp",
            "abi/abi_big_number.cpp", "lztext/lztext.cpp",
            "tools/fetch_dependencies.py", "tools/dependencies.lock.json", "tools/cmake_dependencies.cmake",
            "LICENSE", "NOTICE", "licenses/miniz/LICENSE",
            "template/engine.hpp", "chali_aot_ast.hpp", "chali_aot_target.cpp",
            "runtime/artifact_binary.cpp", "semantics/associated_types.cpp",
            ".chali-generator-manifest",
        ]
        required += ["macro/metadata.hpp", "semantics/macro_metadata.cpp", "semantics/macro_type_metadata.cpp",
                     "semantics/macro_associated_metadata.cpp", "semantics/macro_control_flow.cpp", "semantics/editor_macro_effects.cpp", "semantics/custom_derive.cpp"]
        required += ["runtime/" + name for name in (ROOT / "source/ircode/aot_sources.txt").read_text().splitlines()]
        required += ["runtime/" + path.name for path in (ROOT / "source/ircode").glob("*.hpp") if path.name != "register_core.hpp"]
        required += ["runtime/aot_sources.txt", "toml/toml_source.cpp", "toml/toml_source.hpp"]
        required += ["runtime/bigfloat_math.hpp", "runtime/bigfloat_math_detail.hpp", "runtime/bigfloat_transcendental.cpp", "runtime/bigfloat_trigonometry.cpp", "std/cas.chl", "abi/abi_cas.cpp", "caspp/core_sources.txt"]
        required += ["caspp/" + name for name in (ROOT / "library/caspp/core_sources.txt").read_text().splitlines()]
        required += ["runtime/" + name for name in ["ast_type.cpp", "ast_node.cpp", "ast_data.cpp", "ast_storage_detail.hpp", "bigint_multiplication.cpp", "bigint_division.cpp", "bigint_bits.cpp", "bigint_number_theory.cpp", "bigint_modular.cpp", "bigint_conversion.cpp", "bigfloat_roots.cpp"]]
        missing = [name for name in required if not (compiler / name).is_file()]
        assert not (compiler / "sqlite").exists() and not (compiler / "miniz").exists(), "在线依赖源码不能进入导出工程"
        if missing:
            raise AssertionError(f"compiler export is incomplete: {missing}")
        cmake = (compiler / "CMakeLists.txt").read_text(encoding="utf-8")
        for option in ("CHALI_WITH_REFLECTION", "STACKVM_WITH_REFLECTION"):
            if not any(line.startswith(f"option({option} ") and line.endswith(" ON)")
                       for line in cmake.splitlines()):
                raise AssertionError(f"exported compiler must enable {option} by default")
        if "chali_host_records" not in cmake or "chali_vm_core" not in cmake:
            raise AssertionError("exported compiler must separate VM and host records")
        if "std::regex" not in (compiler / "RESULT.md").read_text(encoding="utf-8"):
            raise AssertionError("compiler generator must use the requested std::regex lexer")
        if (compiler / "std/lang.chl").read_bytes() != (ROOT / "source/std/lang.chl").read_bytes():
            raise AssertionError("compiler lang.chl does not match the referenced language library")
        assert not (compiler / "utils.chl").exists()
        assert not (compiler / "reflect.chl").exists()
        # 导出后构建完整编译器并运行 checked AOT/TOML，检查独立单元、相对 include 及静态库链接。
        build = compiler / "build"
        configure = ["cmake", "-S", str(compiler), "-B", str(build), "-DCMAKE_BUILD_TYPE=Release"]
        if os.name == "nt":
            configure += ["-G", "MinGW Makefiles"]
        for arguments in (configure, ["cmake", "--build", str(build), "--target", "chali", "-j4"]):
            result = subprocess.run(arguments, cwd=ROOT, capture_output=True, text=True,
                                    encoding="utf-8", errors="replace", timeout=900)
            assert result.returncode == 0, (arguments, result.stdout, result.stderr)
        dependency_file = build / "build.ninja" if (build / "build.ninja").exists() else build / "CMakeFiles/Makefile.cmake"
        assert "aot_sources.txt" in dependency_file.read_text(encoding="utf-8"), "核心清单修改必须触发 CMake 重新配置"
        probe = WORK / "export_probe.chl"
        probe.write_text('import std::toml;\n'
                         'interface CountFields{int64 countFields();}\n'
                         '$macro CountFields<T>() -> member\nint64 countFields(){int64 result=0;\n'
                         '$for field in $fields<T>()\nresult+=1;\n$end_for\nreturn result;}\n$end_macro\n'
                         '#derive(CountFields)\nstruct DerivedProbe{int64 number;String text;}\n'
                         '$macro Example<T:#res, U:b>(a,b)\ncast<T>(a)+cast<T>(b)\n$end_macro\n'
                         '$macro TwiceValue(x:value)\nx+x\n$end_macro\n'
                         '$macro TwiceExpr(x:expr)\nx+x\n$end_macro\n'
                         'interface Source{type Item;Item get();}\n'
                         'struct Box<T>:Source{type Item=T;T value;T get(){return self.value;}}\n'
                         '$macro Escape(body:block) -> stmt\nloop{body;break;};\n$end_macro\n'
                         'int64 count=0;int64 next(){count+=1;return count;}\n'
                         'void main()\n{\n'
                         '    DerivedProbe derived;CountFields contract=derived;assert(contract.countFields()==2);\n'
                         '    int32 inferred=$Example((int64)3,(int16)4);assert(inferred==7);\n'
                         '    int64 once=$TwiceValue(next());assert(once==2 && count==1);\n'
                         '    count=0;int64 twice=$TwiceExpr(next());assert(twice==3 && count==2);\n'
                         '    String text = "# exported\\nvalue = 42 # inline\\n";\n'
                         '    std::toml::TomlValue root = std::toml::parse(text).unwrap();\n'
                         '    assert(root.dump() == text);\n'
                         '    assert(($methods<Box<int64>>())[0].hasReceiver);\n'
                         '    assert(($interfaces<Box<int64>>())[0].associatedTypes[0].type.name=="int64");\n'
                         '    assert(($fields<Box<int64>>())[0].sourceLine>0);\n'
                         '    int64 visits=0;loop{visits+=1;$Escape({break;});visits+=100;break;}assert(visits==1);\n'
                         '    println("exported compiler AOT/TOML OK");\n}\n', encoding="utf-8")
        result = subprocess.run([str(build / EXPORTED_COMPILER), "run", str(probe), "--out", str(WORK / "export_probe")],
                                cwd=ROOT, capture_output=True, text=True, encoding="utf-8", errors="replace", timeout=90)
        assert result.returncode == 0 and result.stdout == "exported compiler AOT/TOML OK\n" and not result.stderr, result
        aot_probe = WORK / "aot_probe.chl"
        aot_probe.write_text('void main()\n{\n    println("exported AOT entry OK");\n}\n', encoding="utf-8")
        aot_prefix = WORK / "aot_probe"
        result = subprocess.run([str(build / EXPORTED_COMPILER), "aot", str(aot_probe), "--emit-llvm", "--out", str(aot_prefix)],
                                cwd=ROOT, capture_output=True, text=True, encoding="utf-8", errors="replace", timeout=90)
        assert result.returncode == 0 and aot_prefix.with_suffix(".ll").is_file(), result

        for sample in ("cas", "math_generic", "string_methods", "opcode_value_semantics", "std_red_black_delete"):
            # 全量红黑树混合负载以 AOT 验证导出编译器；VM 全量另由运行时回归执行。
            native = sample == "std_red_black_delete"
            target = WORK / (sample + "-aot")
            arguments = [str(build / EXPORTED_COMPILER), "aot" if native else "run",
                         str(ROOT / "example/samples" / (sample + ".chl"))]
            if native:
                arguments += ["--out", str(target)]
            environment = dict(os.environ)
            if os.name == "nt":
                environment.setdefault("CHALI_LLVM_CLANG", str(ROOT / "build/dependencies/llvm/toolchain/bin/clang.exe"))
            result = subprocess.run(arguments, cwd=WORK, env=environment, capture_output=True, text=True,
                                    encoding="utf-8", errors="replace", timeout=180)
            if native:
                assert result.returncode == 0 and not result.stderr, (sample, result)
                result = subprocess.run([str(target) + (".exe" if os.name == "nt" else "")], cwd=WORK,
                                        capture_output=True, text=True, encoding="utf-8", errors="replace", timeout=90)
            expected = (ROOT / "example/expected" / (sample + ".txt")).read_text(encoding="utf-8")
            assert result.returncode == 0 and result.stdout == expected and not result.stderr, (sample, result)
        shutil.rmtree(build)
        (compiler / "keep.txt").write_text("user\n", encoding="utf-8")
        command([GENERATOR, "clear", "--out-dir", compiler])
        remaining = sorted(
            path.relative_to(compiler).as_posix()
            for path in compiler.rglob("*") if path.is_file()
        )
        # clear 只删除生成清单内文件；构建产生的下载缓存和用户文件均保留。
        dependencies = json.loads((ROOT / "tools/dependencies.lock.json").read_text(encoding="utf-8"))['packages']
        expected_remaining = ["keep.txt", "tmp/dependency_downloads/.lock"]
        for name, dependency in dependencies.items():
            relative = "tmp/dependency_downloads/" + name + "-" + dependency['version'] + ".zip"
            expected_remaining.append(relative)
            assert hashlib.sha256((compiler / relative).read_bytes()).hexdigest() == dependency['sha256']
        if remaining != sorted(expected_remaining):
            raise AssertionError(f"compiler clear left unexpected files: {remaining}")

        unsafe = WORK / "unsafe"
        unsafe.mkdir()
        victim = WORK / "victim.txt"
        victim.write_text("keep\n", encoding="utf-8")
        (unsafe / ".chali-generator-manifest").write_text(
            "# ChaLi generator files v1\n../victim.txt\n", encoding="utf-8",
        )
        command([GENERATOR, "clear", "--out-dir", unsafe], succeeds=False)
        if victim.read_text(encoding="utf-8") != "keep\n":
            raise AssertionError("unsafe compiler manifest escaped the output directory")

        command([GENERATOR, "vm", "--out-dir", WORK / "removed-vm"], succeeds=False)
        assert not (WORK / "removed-vm").exists()
    finally:
        if sys.exc_info()[0] is None:
            shutil.rmtree(WORK, ignore_errors=True)
    print("ChaLi generator checks passed")


if __name__ == "__main__":
    main()
