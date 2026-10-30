"""验证默认 program 模块和外部宏的专用补全，不混入普通符号。"""
from compiler_path import compiler_path
import argparse
from pathlib import Path
from project_temp import tempfile
from native_lsp_transport import Transport, position

ROOT = Path(__file__).resolve().parents[1]


def main():
    """在实际 LSP 连接中检查未保存文档、导入宏和限定宏名称。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--chali", type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    with tempfile.TemporaryDirectory(prefix="chali-macro-completion-") as directory:
        folder = Path(directory)
        (folder / "helpers.chl").write_text(
            "module helpers;\n$macro external\n42\n$end_macro\nint64 ordinary() { return 1; }\n", encoding="utf-8")
        server = Transport(compiler, directory)
        server.query("initialize", {"rootUri": folder.as_uri()})
        try:
            for index, suffix in enumerate(["$", "$ext", "$helpers::"]):
                text = "import helpers;\n$macro local\n1\n$end_macro\nvoid main() { int64 variable = 0; " + suffix
                uri = (folder / ("case" + str(index) + ".chl")).as_uri()
                server.send({"method": "textDocument/didOpen", "params": {"textDocument": {"uri": uri, "text": text, "version": 1}}})
                response = server.query("textDocument/completion", {"textDocument": {"uri": uri}, "position": position(text, len(text))})
                items = response.get("items", []) if isinstance(response, dict) else response
                assert items, (suffix, response)
                assert all(item["label"].startswith("$") for item in items), (suffix, items)
                assert any(item["label"].endswith("external") for item in items), (suffix, items)
                assert not any(item["label"] in {"variable", "ordinary", "main", "int64"} for item in items), items
                assert all(not item.get("insertText", "").startswith("$$") for item in items), items
            text = '$macro Update(mut &target, &source)\ntarget=source;\n$end_macro\nvoid main(){ $'
            uri = (folder / 'reference-parameters.chl').as_uri()
            server.send({"method": "textDocument/didOpen", "params": {"textDocument": {"uri": uri, "text": text, "version": 1}}})
            response = server.query("textDocument/completion", {"textDocument": {"uri": uri}, "position": position(text, len(text))})
            items = response.get("items", []) if isinstance(response, dict) else response
            update = next(item for item in items if item['label'].endswith('Update'))
            inserted = update.get('insertText', update.get('textEdit', {}).get('newText', ''))
            assert inserted and '&' not in inserted and 'mut ' not in inserted, update
            text = '$macro Categories(a:value,b:expr)\na+b\n$end_macro\nvoid main(){ $'
            uri = (folder / 'category-parameters.chl').as_uri()
            server.send({"method": "textDocument/didOpen", "params": {"textDocument": {"uri": uri, "text": text, "version": 1}}})
            response = server.query("textDocument/completion", {"textDocument": {"uri": uri}, "position": position(text, len(text))})
            items = response.get("items", []) if isinstance(response, dict) else response
            category = next(item for item in items if item['label'].endswith('Categories'))
            inserted = category.get('insertText', category.get('textEdit', {}).get('newText', ''))
            assert inserted and ':value' not in inserted and ':expr' not in inserted and ': value' not in inserted and ': expr' not in inserted, category
        finally:
            server.close()
    print("LSP 宏补全：外部模块、$ 前缀与限定名称通过")


if __name__ == "__main__":
    main()
