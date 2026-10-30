#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""把 ChaLi 标准库 .chl 源码 + 生成器运行时资产内嵌进编译器。

读取（路径都以本项目根为基准）：
  * lang.chl、std/*.chl —— 一张
    {模块名 → 源码} 静态表（加载器 chali_module.hpp 先查它，命中就不读文件）；
  * 生成独立程序要用的运行时资产（stackvm 独立模块、胶水、C
    分配器、大数后端）—— 一张 {仓库相对路径 → 源码} 静态表。独立编译器生成器读磁盘失败时
    回退到这张表，于是**单个生成器二进制不依赖仓库目录**也能导出完整程序。

生成 chali_stdlib_embedded.cpp（两张表 + 查询函数），由
独立编译单元编入，声明位于 chali_stdlib.hpp。

文本不再以原始字符串字面量摊在 `.rodata` 里，而是**整体拼接后用 lztext 压缩**，
只把压缩后的字节内联进来，运行时由 table() / generatorAssetTable() 解压重建
（实测 1.48 MB 资产 -> 288 KB，比率 0.185）。压缩算法只有一份 C++ 实现
（lztext/），本脚本调用 `lztext_pack` 命令行工具完成压缩，不在 Python 里重复实现。
行尾仍按 CRLF→LF 归一，与直接读文件的结果保持一致。
"""

import io
from stackvm_sources import asset_names
from pathlib import Path
import os
import shutil
import subprocess
import sys
from project_temp import tempfile
from embedded_asset_cache import AssetCache

# Make 的 $(shell) 不保证继承稍后导出的变量，入口显式传递构建配置。
for option, variable in (("--build-root", "CHALI_BUILD_ROOT"), ("--output", "CHALI_EMBED_OUTPUT")):
    if option in sys.argv:
        at = sys.argv.index(option)
        os.environ[variable] = sys.argv[at + 1]
        del sys.argv[at:at + 2]

DELIM = "PGX"
CHUNK_SIZE = 8000
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CHALI = ROOT


def raw_chunks(content):
    return [
        'R"%s(%s)%s"' % (DELIM, content[i:i + CHUNK_SIZE], DELIM)
        for i in range(0, len(content), CHUNK_SIZE)
    ] or ['R"%s()%s"' % (DELIM, DELIM)]


def pack_tool_path():
    """定位 lztext_pack 命令行工具；找不到就现场编译一个。"""
    env = os.environ.get("LZTEXT_PACK")
    if env and os.path.isfile(env):
        return env
    for candidate in ("build/lztext/lztext-pack.exe", "build/lztext/lztext-pack"):
        path = os.path.join(ROOT, candidate)
        if os.path.isfile(path):
            return path
    compiler = shutil.which("g++") or shutil.which("c++") or shutil.which("clang++")
    if compiler is None:
        raise RuntimeError("找不到 lztext_pack，也没有可用的 C++ 编译器；"
                           "先跑 `make -C source/lztext pack`，或设置 LZTEXT_PACK 指向该工具")
    src_dir = os.path.join(ROOT, "source", "lztext")
    out_dir = os.path.join(ROOT, "build", "lztext")
    os.makedirs(out_dir, exist_ok=True)
    out_path = os.path.join(out_dir,
                            "lztext-pack.exe" if os.name == "nt" else "lztext-pack")
    subprocess.check_call([compiler, "-std=c++17", "-O2", "-I", src_dir,
                           os.path.join(src_dir, "lztext_pack.cpp"),
                           os.path.join(src_dir, "lztext.cpp"),
                           "-o", out_path])
    return out_path


def compress_bundle(blob):
    """用 lztext_pack 压缩一段字节；工具自带解压回读比对，产物一定可解。"""
    tool = pack_tool_path()
    with tempfile.TemporaryDirectory() as workdir:
        in_path = os.path.join(workdir, "in.bin")
        out_path = os.path.join(workdir, "out.bin")
        with open(in_path, "wb") as handle:
            handle.write(blob)
        subprocess.check_call([tool, in_path, out_path])
        with open(out_path, "rb") as handle:
            return handle.read()


def byte_array(name, data):
    """把字节数组写成 C++ 初始化列表，每行 24 个。"""
    lines = ["const unsigned char %s[] = {" % name]
    for i in range(0, len(data), 24):
        chunk = data[i:i + 24]
        lines.append("    " + ", ".join("0x%02x" % b for b in chunk) + ",")
    lines.append("};")
    return "\n".join(lines)


def bundle(entries):
    """把 [(键, 文本)] 拼成一个大块（每段以 '\\0' 结尾），返回 (拼接结果, [(键, 偏移)])。"""
    slices = []
    parts = []
    cursor = 0
    for key, content in entries:
        data = content.encode("utf-8") + b"\0"
        slices.append((key, cursor))
        parts.append(data)
        cursor += len(data)
    return b"".join(parts), slices


def read(path):
    with open(path, "r", encoding="utf-8") as src:
        content = src.read()
    assert ')PGX"' not in content, path
    # 行尾统一 LF：C++ 编译器读源码里的原始字符串也会把 CRLF 归一成 LF，
    # 这里先归一，内联表与直接读文件的结果就一致了。
    return content.replace("\r\n", "\n").replace("\r", "\n")


# 资产键（生成工程用的逻辑路径）保持原样，只把「真仓库里的磁盘位置」映射过去：
# 九个库已搬进 source/，仍留在 library/ 的只有外部依赖（ipage / caspp）。
ASSET_ROOT = {
    "ipage/": "library/ipage/source/",
    "ircode/": "source/",
    "ast/": "source/",
    "preprocess/": "source/",
    "bignum/": "source/",
    "caspp/": "library/",
    "json/": "source/",
    "toml/": "source/",
    "lztext/": "source/",
    "console/": "source/",
    "glue/": "source/",
    "allocator/": "source/",
}


def asset_disk_path(relative):
    """把资产键翻成本仓库里的实际路径。"""
    if relative in ("LICENSE", "NOTICE") or relative.startswith("licenses/"):
        return relative
    if relative == "aot/host_assets.txt":
        return os.environ.get("CHALI_BUILD_ROOT", "build") + "/chali-aot-host/assets.txt"
    if relative == "ipage/LICENSE":
        return "library/ipage/LICENSE"
    if relative.startswith("tools/"):
        return relative
    for prefix, root in ASSET_ROOT.items():
        if relative.startswith(prefix):
            return root + relative
    return "source/" + relative


# 生成器（chali-generator）在运行期需要的运行时资产。相对路径键与仓库布局一致，
# 磁盘读失败时按"整段后缀"或"唯一 basename"回退到这里的文本。
GENERATOR_ASSETS = [
    "tools/chali_manual.json",
    "tools/chali_stdlib_api.json",
    "chali_glue.hpp",
    "chali_aot_debug.hpp",
    "chali_aot_debug.cpp",
    "chali_aot.hpp",
    "chali_aot.cpp",
    "chali_aot_ast.hpp",
    "chali_aot_numeric_list.hpp",
    "chali_aot_target.hpp",
    "chali_aot_target.cpp",
    "chali_aot_llvm.hpp",
    "chali_aot_llvm.cpp",
    "chali_aot_runtime_assets.hpp",
    "chali_aot_runtime_assets.cpp",
    "aot/native_runtime.cpp",
    "aot/backend_profile.cpp",
    "aot/linker_assets.txt",
    "aot/runtime_windows.ll",
    "aot/runtime_linux.ll",
    "aot/runtime_linux_arm64.ll",
    "aot/runtime.c",
    "chali_generic_name.hpp",
    "chali_runtime_allocator.hpp",
    "chali_i18n.hpp",
    "console/utf8.hpp",
    "glue/std_glue.cpp",
    "glue/std_demo.cpp",
    # 直通 ABI：C 胶水、桥接与入口表聚合，生成工程必须一并导出。
    "abi/chali_abi.h",
    "abi/chali_abi_internal.h",
    "abi/chali_abi_runtime.hpp",
    "abi/chali_abi.c",
    "abi/chali_abi_bridge.cpp",
    "abi/abi_print.c",
    "abi/abi_string.c",
    "abi/abi_math.c",
    "abi/abi_clock.c",
    "abi/abi_random.c",
    "glue/std_regex.hpp",
    "glue/std_signal.hpp",
    "glue/std_native_record.hpp",
    "glue/std_collection.hpp",
    "glue/std_scalar_buffer.hpp",
    "glue/std_collection_storage.hpp",
    "glue/std_big_number.hpp",
    "glue/std_file.hpp",
    "glue/std_file_api.hpp",
    "glue/std_desktop_file.hpp",
    "glue/std_structured_data.hpp",
    "glue/std_json.hpp",
    "glue/std_toml.hpp",
    "json/json.hpp",
    "json/json.cpp",
    "toml/toml.hpp",
    "toml/toml.cpp",
    "toml/toml_source.cpp",
    "toml/toml_source.hpp",
    "glue/std_binary.hpp",
    "glue/std_compress.hpp",
    "lztext/lztext.hpp",
    "lztext/lztext.cpp",
    "glue/std_linalg.hpp",
    "glue/std_parse.hpp",
    "glue/std_process.hpp",
    "glue/std_sql.hpp",
    "glue/std_net.hpp",
    "glue/std_image.hpp",
    "glue/std_registry.hpp",
    "glue/std_filesystem.hpp",
    "allocator/chali_pool.h",
    "allocator/chali_pool.c",
    "allocator/chali_allocator.hpp",
    "allocator/system_arena.hpp",
    "allocator/system_arena.cpp",
    "bignum/bigint.hpp",
    "bignum/bigint.cpp",
    "bignum/bigint_multiplication.cpp",
    "bignum/bigfloat.hpp",
    "bignum/bigfloat.cpp",
    "bignum/bigfloat_roots.cpp",
    "bignum/bigfloat_math.hpp",
    "bignum/bigfloat_math_detail.hpp",
    "bignum/bigfloat_transcendental.cpp",
    "bignum/bigfloat_trigonometry.cpp",
]


if Path(CHALI, os.environ.get("CHALI_BUILD_ROOT", "build") + "/chali-aot-host/assets.txt").exists():
    GENERATOR_ASSETS.append("aot/host_assets.txt")

GENERATOR_ASSETS += ["semantics/abi_export.cpp", "glue/aot_modules.inc",
                     "tools/fetch_dependencies.py", "tools/dependencies.lock.json", "tools/cmake_dependencies.cmake",
                     "tools/llvm_static_imports.cpp",
                     "LICENSE", "NOTICE", "licenses/miniz/LICENSE"]

GENERATOR_ASSETS += ['ast/ast_type.cpp', 'ast/ast_node.cpp', 'ast/ast_data.cpp', 'ast/ast_storage_detail.hpp', 'bignum/bigint_division.cpp', 'bignum/bigint_number_theory.cpp', 'bignum/bigint_bits.cpp', 'bignum/bigint_modular.cpp', 'bignum/bigint_conversion.cpp']

GENERATOR_ASSETS += ["ircode/" + name for name in asset_names()]
from cas_sources import asset_names as cas_asset_names
GENERATOR_ASSETS += ["caspp/" + name for name in cas_asset_names()]
GENERATOR_ASSETS += ["ipage/" + path.name for path in sorted(Path(CHALI, "library/ipage/source/ipage").iterdir()) if path.suffix in (".hpp", ".cpp")]
GENERATOR_ASSETS += ["ipage/LICENSE"]

# 胶水接口及实现作为同一组资产导出，固定清单不能遗漏独立编译单元。
GENERATOR_ASSETS += ["chali_temp.hpp", "chali_temp.cpp", "chali_precompiled.hpp", "chali_precompiled.cpp"]
if Path(CHALI, "source/chali_precompiled_data.inc").exists():
    GENERATOR_ASSETS.append("chali_precompiled_data.inc")
GENERATOR_ASSETS += [
    directory + "/" + path.name
    for directory in ("macro", "literals", "syntax", "editor")
    for path in sorted(Path(CHALI, "source", directory).iterdir())
    if path.suffix in (".cpp", ".hpp")
]
GENERATOR_ASSETS = list(dict.fromkeys(GENERATOR_ASSETS + [
    "glue/" + path.name for path in sorted(Path(CHALI, "source/glue").iterdir())
    if path.suffix in (".cpp", ".hpp", ".h", ".c")
]))


def collect_generator_assets():
    assets = []
    for relative in GENERATOR_ASSETS:
        assets.append((relative, read(os.path.join(CHALI, asset_disk_path(relative)))))
    return assets


def collect():
    modules = []  # (name, content)
    std_dir = os.path.join(CHALI, "source", "std")
    for name in sorted(os.listdir(std_dir)):
        if name.endswith(".chl"):
            modules.append(("std::" + os.path.splitext(name)[0], read(os.path.join(std_dir, name))))
    return modules


def dependencies():
    """列出内嵌资产的生成输入，供增量构建精确追踪。"""
    from pathlib import Path
    root = Path(CHALI)
    paths = {Path(asset_disk_path(name)) for name in GENERATOR_ASSETS}
    paths.add(Path(os.environ.get("CHALI_BUILD_ROOT", "build") + "/chali-aot-host/assets.txt"))
    for pattern in ("source/std/*.chl", "doc/book/*.md", "doc/book/images/*", "doc/library/*.md"):
        paths.update(path.relative_to(root) for path in root.glob(pattern) if path.is_file())
    paths.update(map(Path, ("tools/embed_stdlib.py", "tools/project_temp.py", "tools/embedded_asset_cache.py", "tools/stackvm_sources.py", "tools/embed_manual.py", "source/chali.gram",
                           "source/chali_token.gram", "doc/04-language-reference.md",
                           "doc/25-literals-operators-macros.md", "skills/chali-programming/SKILL.md")))
    names = sorted(path.as_posix() for path in paths)
    # Windows GNU make 无法可靠打开非 ASCII 前置文件名，改用固定 ASCII 路径
    # 记录这些文件的精确时间与大小，保持增量依赖可追踪。
    indirect = [name for name in names if not name.isascii() or any(ch.isspace() for ch in name)]
    marker = root / (os.environ.get("CHALI_BUILD_ROOT", "build") + "/chali-frontend/stdlib_manual_inputs.json")
    import json
    payload = json.dumps([(name, (root / name).stat().st_mtime_ns, (root / name).stat().st_size)
                          for name in indirect], ensure_ascii=False, separators=(",", ":")) + "\n"
    marker.parent.mkdir(parents=True, exist_ok=True)
    if not marker.exists() or marker.read_text(encoding="utf-8") != payload:
        marker.write_text(payload, encoding="utf-8")
    return [name for name in names if name not in indirect] + [marker.relative_to(root).as_posix()]


def main():
    from embed_manual import ensure_generated
    ensure_generated()
    modules = collect()
    assets = collect_generator_assets()
    # 整体拼接后再压缩：跨模块、跨资产的重复也能被利用，比率明显好于逐个压缩
    #（实测整体 0.185 vs 逐个约 0.30）。
    modules_blob, modules_slices = bundle(modules)
    assets_blob, assets_slices = bundle(assets)
    out_path = os.environ.get("CHALI_EMBED_OUTPUT", os.path.join(CHALI, "source", "chali_stdlib_embedded.cpp"))
    Path(out_path).parent.mkdir(parents=True, exist_ok=True)
    cache = AssetCache('stdlib', {
        'modules': modules_blob, 'generator-assets': assets_blob,
        'module-index': repr(modules_slices).encode('utf-8'),
        'asset-index': repr(assets_slices).encode('utf-8'),
        'generator:embed_stdlib.py': Path(__file__),
        **{'compressor:' + path.name: path for path in Path(CHALI, 'source/lztext').glob('*') if path.suffix in ('.cpp', '.hpp', '.h')},
    })
    if cache.valid([out_path]):
        print('内嵌标准库与生成器资产 SHA256 相同，跳过生成：' + str(cache.path))
        return 0
    modules_packed = compress_bundle(modules_blob)
    assets_packed = compress_bundle(assets_blob)
    with io.StringIO() as handle:
        handle.write("// 本文件由 tools/embed_stdlib.py 生成，请勿手工修改。\n")
        handle.write("// 生成命令：python tools/embed_stdlib.py\n\n")
        handle.write('#include "chali_stdlib.hpp"\n')
        handle.write("\n")
        handle.write("// 文本以 lztext 压缩形式内联（运行时解压），算法见 lztext/README.md。\n")
        handle.write("#include <cstddef>\n")
        handle.write("#include <string>\n")
        handle.write("#include <vector>\n\n")
        handle.write('#include "lztext/lztext.hpp"\n\n')
        handle.write("namespace chali_module\n{\nnamespace embedded\n{\n\n")

        handle.write("namespace\n{\n\n")
        handle.write("// 全部模块源码按顺序拼接（每段以 '\\0' 结尾）后压缩的结果："
                     "%d 段，原始 %d 字节，压缩后 %d 字节。\n"
                     % (len(modules), len(modules_blob), len(modules_packed)))
        handle.write(byte_array("kModulesPacked", modules_packed))
        handle.write("\n\nstruct ModuleSlice\n{\n"
                     "    const char *name;    ///< 模块名\n"
                     "    std::size_t offset;  ///< 在解压后缓冲区里的起始下标\n"
                     "};\n\n")
        handle.write("const ModuleSlice kModulesIndex[] = {\n")
        for key, offset in modules_slices:
            handle.write('    {"%s", %d},\n' % (key, offset))
        handle.write("};\n\n")
        handle.write("} // namespace\n\n")
        handle.write("""/**\n * @brief 解压一次标准库源码并返回进程内持久的模块视图。\n * @return 以空项结尾的标准库模块表。\n */\nconst Entry *table()
{
    // 解压一次，之后每个模块都指向这块缓冲区里的某段（各段以 '\\0' 结尾）。
    static const std::string k_text =
        lztext::decompressString(kModulesPacked, sizeof(kModulesPacked));
    static const std::vector<Entry> k_entries = [] {
        std::vector<Entry> entries;
        entries.reserve(sizeof(kModulesIndex) / sizeof(kModulesIndex[0]) + 1u);
        for (const ModuleSlice &slice : kModulesIndex)
        {
            if (slice.offset >= k_text.size()) continue;
            entries.push_back(Entry{slice.name, k_text.data() + slice.offset});
        }
        entries.push_back(Entry{nullptr, nullptr});
        return entries;
    }();
    return k_entries.data();
}

""")
        handle.write("/**\n * @brief 按模块限定名复制内嵌源码。\n * @param name 完整模块名。\n * @param out 成功时接收源码。\n * @return 找到模块时为 true。\n */\nbool find(const Memory::String &name, Memory::String *out)\n{\n")
        handle.write("    for (const Entry *entry = table(); entry->name != nullptr; ++entry)\n")
        handle.write("        if (name == entry->name) { *out = entry->text; return true; }\n")
        handle.write("    return false;\n}\n\n")

        handle.write("namespace\n{\n\n")
        handle.write("// 全部生成器资产按顺序拼接（每段以 '\\0' 结尾）后压缩的结果："
                     "%d 项，原始 %d 字节，压缩后 %d 字节。\n"
                     % (len(assets), len(assets_blob), len(assets_packed)))
        handle.write(byte_array("kAssetsPacked", assets_packed))
        handle.write("\n\nstruct AssetSlice\n{\n"
                     "    const char *path;     ///< 仓库相对路径\n"
                     "    std::size_t offset;   ///< 在解压后缓冲区里的起始下标\n"
                     "};\n\n")
        handle.write("const AssetSlice kAssetsIndex[] = {\n")
        for path, offset in assets_slices:
            handle.write('    {"%s", %d},\n' % (path, offset))
        handle.write("};\n\n")
        handle.write("} // namespace\n\n")
        handle.write("""/**\n * @brief 解压一次生成器资产并创建稳定的内容视图。\n * @return 以空项结尾的工程资产表。\n */\nconst GeneratorAsset *generatorAssetTable()
{
    static const std::string k_text =
        lztext::decompressString(kAssetsPacked, sizeof(kAssetsPacked));
    static const std::vector<GeneratorAsset> k_entries = [] {
        std::vector<GeneratorAsset> entries;
        entries.reserve(sizeof(kAssetsIndex) / sizeof(kAssetsIndex[0]) + 1u);
        for (const AssetSlice &slice : kAssetsIndex)
        {
            if (slice.offset >= k_text.size()) continue;
            entries.push_back(GeneratorAsset{slice.path, k_text.data() + slice.offset});
        }
        entries.push_back(GeneratorAsset{nullptr, nullptr});
        return entries;
    }();
    return k_entries.data();
}

""")
        handle.write("/**\n * @brief 取得兼容正反斜杠的路径末尾文件名。\n * @param path 原始资产路径。\n * @return 去除父目录后的文件名。\n */\nMemory::String generatorAssetTail(const Memory::String &path)\n{\n")
        handle.write("    const std::size_t slash = path.find_last_of(\"/\\\\\");\n")
        handle.write("    return (slash == Memory::String::npos) ? path : path.substr(slash + 1u);\n}\n\n")
        handle.write("""/**
 * @brief 按路径后缀或唯一文件名查找内嵌工程资产。
 * @param path 原始资产路径。
 * @param out 成功时接收资产内容。
 * @return 唯一命中时为 true；未找到或存在歧义时为 false。
 */
bool findGeneratorAssetByPath(const Memory::String &path, Memory::String *out)
{
    Memory::String want = path;
    for (char &c : want) if (c == '\\\\') c = '/';
    const Memory::String wantTail = generatorAssetTail(want);
    const GeneratorAsset *baseHit = nullptr;
    std::size_t baseMatches = 0u;
    for (const GeneratorAsset *asset = generatorAssetTable(); asset->path != nullptr; ++asset)
    {
        const Memory::String key(asset->path);
        if (want == key) { *out = asset->text; return true; }
        const Memory::String keyNeedle = Memory::String("/") + key;
        if (want.size() > keyNeedle.size() &&
            want.compare(want.size() - keyNeedle.size(), keyNeedle.size(), keyNeedle) == 0)
        { *out = asset->text; return true; }
        const Memory::String wantNeedle = Memory::String("/") + want;
        if (key.size() > wantNeedle.size() &&
            key.compare(key.size() - wantNeedle.size(), wantNeedle.size(), wantNeedle) == 0)
        { *out = asset->text; return true; }
        if (generatorAssetTail(key) == wantTail) { ++baseMatches; baseHit = asset; }
    }
    if (baseMatches == 1u && baseHit != nullptr) { *out = baseHit->text; return true; }
    return false;
}

""")
        handle.write("} // namespace embedded\n")
        handle.write("} // namespace chali_module\n")

        payload = handle.getvalue().encode("utf-8")
    try:
        with open(out_path, "rb") as existing:
            unchanged = existing.read() == payload
    except FileNotFoundError:
        unchanged = False
    if not unchanged:
        temporary = None
        try:
            with tempfile.NamedTemporaryFile(prefix=".chali-stdlib-", suffix=".tmp",
                                             dir=os.path.dirname(out_path), delete=False) as output:
                temporary = output.name
                output.write(payload)
            os.replace(temporary, out_path)
        finally:
            if temporary is not None and os.path.exists(temporary):
                os.unlink(temporary)
    total = sum(len(content) for _, content in modules)
    cache.save([out_path])
    asset_bytes = sum(len(content) for _, content in assets)
    print("%s %s (%d modules, %d bytes; %d generator assets, %d bytes)"
          % ("unchanged" if unchanged else "wrote", out_path, len(modules), total, len(assets), asset_bytes))
    return 0


if __name__ == "__main__":
    if sys.argv[1:] == ["--dependencies"]:
        print(" ".join(dependencies()))
    elif len(sys.argv) == 3 and sys.argv[1] == "--stamp":
        status = main()
        if status == 0:
            from pathlib import Path
            Path(sys.argv[2]).touch()
        sys.exit(status)
    else:
        sys.exit(main())
