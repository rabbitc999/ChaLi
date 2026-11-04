"""验证成员宏自动登记普通及泛型接口。"""

import argparse
import json
from pathlib import Path
import subprocess

from compiler_path import compiler_path
from native_lsp_transport import Transport, position
from project_temp import tempfile


def descendants(node):
    """遍历编辑器展开树的结构节点。"""
    if node is None:
        return
    yield node
    for child in node.get("children", ()):
        yield from descendants(child)


def main():
    """实际调用生成的方法，并检查无效的接口登记。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--chali", type=Path, default=compiler_path())
    args = parser.parse_args()
    constrained = """
interface Count<T> { int64 count(); }
interface More : Count<int64> {}
$macro COUNT(name:ident) -> member
$implement More;
int64 name() { return 42; }
$end_macro
struct Record { $COUNT(count); }
int64 read_count<T:Count<int64>>(T value) { return value.count(); }
"""
    cases = (
        ("interfaces", """
interface Read<T> { T read(); }
interface Named { String label(); }
$macro AUTO<T>(reader:ident, labeler:ident) -> member
$implement Read<T>;
$implement Named;
$implement Named;
T reader() { return self.value; }
String labeler() { return "auto"; }
$end_macro
struct Entry<T> {
    T value;
    $AUTO<T>(read, label);
}
void main() {
    Entry<int64> entry;
    entry.value = 42;
    Read<int64> reader = entry;
    Named named = entry;
    println("{} {}", reader.read(), named.label());
}
""", "42 auto", None),
        ("concrete_generic_interfaces", """
import std::macro;
interface Count<T> { int64 count(); }
$macro COUNT<T>(method:ident) -> member
$implement Count<T>;
$implement Count<String>;
int64 method() { return 2; }
$end_macro
struct Entry { $COUNT<int64>(count); }
struct Point {
    int64 value;
    $std::macro::Equal<Point>(leftEqual);
}
void main() {
    Entry entry;
    Count<int64> number = entry;
    Count<String> text = entry;
    Point first;
    Point second;
    assert(first == second);
    second.value = 1;
    assert(first != second);
    println("{} {}", number.count(), text.count());
}
""", "2 2", None),
        ("constrained_inferred", constrained + """
void main() { Record value; println("{}", read_count(value)); }
""", "42", None),
        ("constrained_explicit", constrained + """
void main() { Record value; println("{}", read_count<Record>(value)); }
""", "42", None),
        ("missing_constraint", """
interface Count { int64 count(); }
$macro COUNT(name:ident) -> member
$implement Count;
int64 name() { return 42; }
$end_macro
struct Valid { $COUNT(count); }
struct Other { int64 count() { return 42; } }
int64 read_count<T:Count>(T value) { return value.count(); }
void main() { Other value; assert(read_count<Other>(value) == 42); }
""", None, "未实现接口"),
        ("missing_method", """
interface Read<T> { T read(); }
$macro AUTO<T>() -> member
$implement Read<T>;
$end_macro
struct Entry { $AUTO<int64>(); }
void main() {}
""", None, "read"),
        ("non_interface", """
struct Ordinary { int64 value; }
$macro AUTO() -> member
$implement Ordinary;
$end_macro
struct Entry { $AUTO(); }
void main() {}
""", None, "$implement 需要已声明的接口类型"),
        ("interface_host", """
interface Named { String label(); }
$macro AUTO() -> member
$implement Named;
$end_macro
interface Bad { $AUTO(); }
void main() {}
""", None, "$implement 需要结构体或枚举宿主"),
    )
    with tempfile.TemporaryDirectory(prefix="macro-implement-") as directory:
        for name, code, expected, diagnostic in cases:
            path = Path(directory) / (name + ".chl")
            path.write_text(code, encoding="utf-8")
            result = subprocess.run(
                [str(args.chali), "run" if expected else "check", str(path)],
                capture_output=True, text=True, encoding="utf-8", errors="replace", timeout=180,
            )
            if expected:
                assert result.returncode == 0 and expected in result.stdout.splitlines(), (name, result)
            else:
                assert result.returncode != 0 and diagnostic in result.stderr, (name, result)
            print(f"{name} 通过")
        folder = Path(directory)
        source = cases[0][1]
        uri = (folder / "macro_preview.chl").as_uri()
        server = Transport(args.chali.resolve(), str(folder))
        server.query("initialize", {"rootUri": folder.as_uri()})
        try:
            server.send({"method": "textDocument/didOpen", "params": {"textDocument": {
                "uri": uri, "text": source, "version": 1,
            }}})
            for mode in ("one", "all"):
                result = server.query("chali/expandMacro", {
                    "textDocument": {"uri": uri}, "mode": mode,
                    "position": position(source, source.index("$AUTO<T>") + 2),
                }, timeout=180)
                assert not result["diagnostics"], result
                nodes = [
                    node
                    for item in json.loads(result["text"])["expansions"]
                    for node in descendants(item["tree"])
                ]
                assert any(node["kind"] == "implements" and "Read" in node["type"] for node in nodes), result
        finally:
            server.close()


if __name__ == "__main__":
    main()
