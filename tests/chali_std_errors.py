"""验证标准库公开 Result 的错误协议、结构化载荷和 VM 传播行为，不运行 AOT。"""
from compiler_path import compiler_path
import argparse
import os
from pathlib import Path
import re
import subprocess
from project_temp import tempfile
ROOT = Path(__file__).resolve().parents[1]


def check_contract():
    """检查标准库自己提供的具体错误均实现 Error，公开结果不返回文本错误。"""
    sources = {path.name: path.read_text(encoding="utf-8")
               for path in (ROOT / "source/std").glob("*.chl")}
    errors = set()
    for text in sources.values():
        errors.update(re.findall(r"(?:enum|struct)\s+(\w+Error)\s*:\s*Error\b", text))
    expected = {"MessageError", "IoError", "ParseError", "CliError", "CsvError",
                "DateTimeError", "EncodingError", "Utf8Error", "WatchdogError",
                "RegexError", "ReflectError"}
    assert expected <= errors, expected - errors
    for file, text in sources.items():
        offset = 0
        while True:
            start = text.find("Result<", offset)
            if start < 0:
                break
            cursor, depth, separator = start + 7, 1, None
            while cursor < len(text) and depth:
                token = text[cursor]
                if token == "<":
                    depth += 1
                elif token == ">":
                    depth -= 1
                elif token == "," and depth == 1:
                    separator = cursor
                cursor += 1
            assert not depth, (file, start)
            if separator is not None:
                error = text[separator + 1:cursor - 1].strip().split("::")[-1]
                assert error in errors or error in {"E", "F"}, (file, error)
            offset = cursor



# 每个模块独立编译，避免综合样例的泛型快照放大编译期内存。
SCENARIOS = {
    "cli": 'let error = std::cli::cliError("--count", "参数缺失"); assert(error.toString().contains("--count"));',
    "csv": 'let error = std::csv::csvError(3, 7, "字段错误"); assert(error.line == 3 && error.column == 7); assert(error.toString().contains("字段错误"));',
    "datetime": 'assert((std::datetime::DateTimeError::InvalidDate).toString() != "");',
    "encoding": 'assert((std::encoding::EncodingError::InvalidCharacter(9)).toString().contains("9"));',
    "utf8": 'assert((std::utf8::Utf8Error::InvalidSequence(12)).toString().contains("12"));',
    "watchdog": 'assert((std::watchdog::WatchdogError::InvalidTimeout).toString() != "");',
    "regex": 'assert((std::regex::RegexError::Invalid).toString() != "");',
    "reflect": 'assert((std::reflect::ReflectError::NotFound).toString() != "");',
    "json": 'Result<std::json::JsonValue, MessageError> result = std::json::parse("{bad"); assert(result.isErr()); assert(result.error().toString() != ""); assert(readNumber("42").unwrap() == 42); assert(readNumber("{bad").isErr());',
    "toml": 'Result<std::toml::TomlValue, MessageError> result = std::toml::parse("x = ["); assert(result.isErr()); assert(result.error().toString() != "");',
    "image": 'Result<Image, MessageError> result = Image::make(0, 1); assert(result.isErr()); assert(result.error().toString().contains("宽高"));',
    "sql": 'Query query; query.page(-1); Result<SqlStatement, MessageError> result = query.build(); assert(result.isErr()); assert(result.error().toString().contains("分页"));',
}
ERROR_VALUES = {
    'cli': 'error', 'csv': 'error',
    'datetime': 'std::datetime::DateTimeError::InvalidDate',
    'encoding': 'std::encoding::EncodingError::InvalidCharacter(9)',
    'utf8': 'std::utf8::Utf8Error::InvalidSequence(12)',
    'watchdog': 'std::watchdog::WatchdogError::InvalidTimeout',
    'regex': 'std::regex::RegexError::Invalid',
    'reflect': 'std::reflect::ReflectError::NotFound',
    'json': 'result.error()', 'toml': 'result.error()',
    'image': 'result.error()', 'sql': 'result.error()',
}


def main():
    """用默认 VM 配置验证统一错误接口，并检查公开类型契约。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--chali", type=Path, default=compiler_path())
    args = parser.parse_args()
    check_contract()
    with tempfile.TemporaryDirectory(prefix="chali-std-errors-") as directory:
        result = subprocess.run(
            [str(args.chali.resolve()), "run", str(ROOT / "example/samples/std_errors.chl"),
             "--out", str(Path(directory) / "errors")], cwd=ROOT,
            env=dict(os.environ, CHALI_LANG="zh", CHALI_NO_STDIN_INPUT="1"),
            capture_output=True, text=True, encoding="utf-8", timeout=300)
        assert (result.returncode, result.stdout, result.stderr) == (
            0, "standard Error protocol OK\n", ""), result
    with tempfile.TemporaryDirectory(prefix="chali-error-modules-") as directory:
        for module, statements in SCENARIOS.items():
            source = Path(directory) / (module + ".chl")
            helper = ('Result<int64, MessageError> readNumber(String text) { let value = std::json::parse(text)?; return Result<int64, MessageError>::Ok(value.asInt().unwrap()); }\n' if module == "json" else "")
            helper += ('String describe<E: Error>(E error) { return error.toString(); }\n'
                       'String describeInterface(Error const& error) { return error.toString(); }\n')
            checks = ('let witness = ' + ERROR_VALUES[module] + '; '
                      'assert(describe(witness) == witness.toString()); '
                      'assert(describeInterface(&witness) == witness.toString()); ')
            source.write_text("import std::" + module + ";\n" + helper + "void main() { " + statements + checks + " println(\"OK\"); }\n", encoding="utf-8")
            result = subprocess.run([str(args.chali.resolve()), "run", str(source), "--out", str(Path(directory) / (module + "-out"))], cwd=ROOT, env=dict(os.environ, CHALI_LANG="zh", CHALI_NO_STDIN_INPUT="1"), capture_output=True, text=True, encoding="utf-8", timeout=300)
            assert (result.returncode, result.stdout, result.stderr) == (0, "OK\n", ""), (module, result)
            print("Error 模块通过：" + module, flush=True)
    print("标准库 Error：11 种错误类型、接口与泛型处理、消息所有权、位置、JSON/TOML/图像/文件/SQL 及 ? 传播通过")


if __name__ == "__main__":
    main()
