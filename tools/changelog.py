#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""CHANGELOG 的生成与门禁。

`CHANGELOG.md` 里每一节的版本号要和 `VERSION`、和发布 tag 对齐；节内的日期行与条目的
形态也要稳定，否则「改了 VERSION 忘写一节」「打了 tag 却没有对应记录」这类漂移只能靠
人眼发现。这个脚本把两件事都做成命令：

* `--check`  —— 门禁。校验：`VERSION` ↔ 顶级节；每个 `vX.Y.Z` tag ↔ 有对应节；版本号
                唯一且自新到旧递减；每节至少一个 `### 日期` 行与一条 `- ` 条目；
                顶级节不留占位符。任何一条不满足就非零退出。
* `--render` —— 从「上一版本以来的提交」归纳出一节草案，可直接粘贴或 `--write` 写入；
                发版途中还没提交的改动用 `--include-working` 一并纳入。

草案的条目取自提交标题与工作区改动清单 —— 事实来自 git，措辞由人改。改完再跑 `--check`。

用法（在项目根目录跑）：

    python tools/changelog.py --check
    python tools/changelog.py --render --from v0.12.1 --version 0.12.2 --title "一句话标题"
    python tools/changelog.py --render --from <sha> --include-working --write
"""

import argparse
import datetime
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CHANGELOG = ROOT / "CHANGELOG.md"
VERSION_FILE = ROOT / "VERSION"

SECTION = re.compile(r"^##\s+(\d+(?:\.\d+)*)\s+—\s*(.*?)\s*$")
DATE_LINE = re.compile(r"^###\s+\d{4}-\d{2}-\d{2}")
BULLET = re.compile(r"^-\s+\S")
PLACEHOLDER = re.compile(r"待填|TODO|TBD|FIXME|XXX|草案：tools/changelog\.py")
TAG = re.compile(r"^v(\d+(?:\.\d+)*)$")
VERSION_TEXT = re.compile(r"^\d+(?:\.\d+)*$")

# 工作区清单最多列几个文件，超出只报个数（发版时通常就十来处）。
WORKING_LIST_LIMIT = 12


def version_key(text):
    """`0.12.2` / `0.3.2.3` → 整数元组，可直接比大小。"""
    return tuple(int(part) for part in text.split("."))


def read_version():
    text = VERSION_FILE.read_text(encoding="utf-8").strip()
    if not VERSION_TEXT.match(text):
        raise SystemExit(f"读不懂 VERSION 的内容：{text!r}")
    return text


def read_lines():
    return CHANGELOG.read_text(encoding="utf-8").splitlines()


def parse_sections(lines):
    """切成节，返回 [(版本, 标题, 首行号, 末行号)]（行号 0-based，末行号不含）。"""
    heads = []
    for index, line in enumerate(lines):
        match = SECTION.match(line)
        if match:
            heads.append((index, match.group(1), match.group(2)))
    sections = []
    for order, (index, version, title) in enumerate(heads):
        end = heads[order + 1][0] if order + 1 < len(heads) else len(lines)
        sections.append((version, title, index, end))
    return sections


def git(*args):
    return "\n".join(git_lines(*args))


def git_lines(*args):
    """按行取 git 输出。**不要**先 strip 整体：`status --porcelain` 每行以状态位开头，
    整体 strip 会吃掉第一行的状态位，把路径首字符一起带走。"""
    done = subprocess.run(["git", *args], cwd=ROOT, text=True,
                          capture_output=True)
    if done.returncode != 0:
        raise SystemExit(f"git {' '.join(args)} 失败：{done.stderr.strip()}")
    return done.stdout.splitlines()


def tags():
    """仓库里所有 `vX.Y.Z` 形式的 tag（版本号字符串，不带 v）。"""
    found = [TAG.match(name).group(1) for name in git("tag", "--list").splitlines() if TAG.match(name)]
    return sorted(found, key=version_key)


def section_commit(version):
    """定位引入 `## <version> —` 那一节的提交（该节是那次提交写进去的）。"""
    for line in git_lines("log", "--format=%H", "-1", "-S", f"## {version} —",
                          "--", "CHANGELOG.md"):
        if line:
            return line
    return ""


def default_origin(target):
    """缺省起点：上一版本写到哪就从哪开始。

    上一版本 = 顶级节那个版本（正要发布的新版通常还没写进去）；如果 `target` 已经是
    顶级节了（重复渲染同一版），上一版本就取第二节。

    不要用「版本号最大的 tag」当缺省：上一版可能压根没打 tag（比如 0.12.1），那样会把
    好几版的提交都塞进这一节。
    """
    sections = parse_sections(read_lines())
    known = tags()
    if sections and sections[0][0] == target:
        previous = sections[1][0] if len(sections) > 1 else ""
    else:
        previous = sections[0][0] if sections else ""
    if previous and previous in known:
        return f"v{previous}"
    if previous:
        commit = section_commit(previous)
        if commit:
            return commit
        fallback = f"v{known[-1]}" if known else ""
        print(f"提示：定位不到 {previous} 节的提交，起点退回 {fallback or '（无）'}",
              file=sys.stderr)
        return fallback
    return f"v{known[-1]}" if known else ""


def check():
    lines = read_lines()
    sections = parse_sections(lines)
    if not sections:
        print("CHANGELOG.md 里找不到任何 `## x.y.z — 标题` 节", file=sys.stderr)
        return 1

    problems = []
    version = read_version()
    head_version = sections[0][0]
    if version_key(head_version)[:3] != version_key(version)[:3]:
        problems.append(
            f"顶级节是 {head_version}，VERSION 是 {version}：改了 VERSION 就要在最上面补一节")

    seen = {}
    for version_text, _title, _start, _end in sections:
        seen[version_text] = seen.get(version_text, 0) + 1
    for version_text, count in sorted(seen.items()):
        if count > 1:
            problems.append(f"版本 {version_text} 出现了 {count} 节，版本号必须唯一")

    for older, newer in zip(sections[1:], sections[:-1]):
        if version_key(older[0]) >= version_key(newer[0]):
            problems.append(f"版本顺序不对：{newer[0]} 之后应是更旧的版本，却拍到 {older[0]}")

    for version_text, _title, start, end in sections:
        body = lines[start + 1:end]
        if not any(DATE_LINE.match(line) for line in body):
            problems.append(f"{version_text} 节缺少 `### YYYY-MM-DD` 日期行")
        if not any(BULLET.match(line) for line in body):
            problems.append(f"{version_text} 节没有任何 `- ` 条目")

    for line in lines[sections[0][2]:sections[0][3]]:
        if PLACEHOLDER.search(line):
            problems.append(f"顶级节（{head_version}）里还留着占位符：{line.strip()}")
            break

    known = seen
    for tag_version in tags():
        if tag_version not in known:
            problems.append(f"tag v{tag_version} 没有对应的 CHANGELOG 节")

    if problems:
        print("CHANGELOG 门禁未通过：", file=sys.stderr)
        for problem in problems:
            print(f"  - {problem}", file=sys.stderr)
        return 1
    print(f"CHANGELOG：{len(sections)} 节，顶级 {head_version} 与 VERSION({version}) 一致，"
          f"{len(tags())} 个 tag 均有记录")
    return 0


def working_changes():
    """未提交的改动清单：返回 (新增列表, 修改列表)。"""
    added, modified = [], []
    for line in git_lines("status", "--porcelain", "--untracked-files=all"):
        code, path = line[:2], line[3:].strip()
        if code == "??" or code[0] == "A":
            added.append(path)
        else:
            modified.append(path)
    return added, modified


def brief(paths):
    shown = "、".join(f"`{path}`" for path in paths[:WORKING_LIST_LIMIT])
    if len(paths) > WORKING_LIST_LIMIT:
        shown += f" 等共 {len(paths)} 处"
    return shown


def render(args):
    version = args.version or read_version()
    title = args.title or "待填：一句话标题"
    date = args.date or datetime.date.today().isoformat()

    origin = args.from_rev
    if not origin:
        origin = default_origin(version)

    body = []
    if origin:
        subjects = git_lines("log", "--reverse", "--format=%h %s", f"{origin}..HEAD")
        body.extend(f"- {subject}" for subject in subjects)
    else:
        subjects = []

    added, modified = ([], [])
    if args.include_working:
        added, modified = working_changes()
        if added:
            body.append(f"- 工作区新增（{len(added)}）：{brief(added)}")
        if modified:
            body.append(f"- 工作区修改（{len(modified)}）：{brief(modified)}")

    if not body:
        body.append("- 待填：本版改了什么")

    range_note = origin or "（未指定起点，只列工作区改动）"
    if origin and not subjects:
        range_note = f"{origin}..HEAD（无提交，只有工作区改动）"
    comment = (f"草案：tools/changelog.py --render 生成（{range_note}）。条目取自提交标题与"
               f"工作区清单，改成人话、删掉本行再跑 --check")
    section = [f"## {version} — {title}", "", f"### {date}", "", f"<!-- {comment} -->", *body, ""]

    if args.write:
        lines = read_lines()
        sections = parse_sections(lines)
        if not sections:
            raise SystemExit("CHANGELOG.md 里找不到任何 `## x.y.z — 标题` 节，无法定位插入点")
        if version in {head[0] for head in sections}:
            raise SystemExit(f"CHANGELOG.md 里已经有 {version} 节了；先删掉旧的再 --write")
        at = sections[0][2]
        merged = lines[:at] + section + lines[at:]
        CHANGELOG.write_text("\n".join(merged) + "\n", encoding="utf-8")
        print(f"已把 {version} 节写入 CHANGELOG.md（{len(body)} 条草案条目）")
        return 0

    print("\n".join(section))
    return 0


def main():
    parser = argparse.ArgumentParser(
        prog="tools/changelog.py",
        description="CHANGELOG 的生成与门禁",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__)
    parser.add_argument("--check", action="store_true",
                        help="门禁：VERSION / tag / 版本序 / 日期行 / 条目 / 占位符")
    parser.add_argument("--render", action="store_true", help="生成一节草案")
    parser.add_argument("--from", dest="from_rev", metavar="REV",
                        help="起点（tag / 分支 / sha）；缺省用版本号最大的 tag")
    parser.add_argument("--version", dest="version", metavar="X.Y.Z",
                        help="要生成哪一版；缺省读 VERSION")
    parser.add_argument("--title", help="节标题（`## x.y.z — 标题` 的标题部分）")
    parser.add_argument("--date", help="日期行；缺省今天")
    parser.add_argument("--include-working", action="store_true",
                        help="把未提交的工作区改动一并纳入草案")
    parser.add_argument("--write", action="store_true",
                        help="直接写到 CHANGELOG.md 顶部（缺省只打印）")
    args = parser.parse_args()

    if args.check == args.render:
        parser.print_help()
        return 2
    return check() if args.check else render(args)


if __name__ == "__main__":
    sys.exit(main())
