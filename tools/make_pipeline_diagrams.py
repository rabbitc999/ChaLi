#!/usr/bin/env python3
"""绘制中英文编译与 AOT 导出流程图；沿用总览图主题，三倍绘制后降采样。"""
from pathlib import Path
import math
import os

os.environ["SE_IMAGE_SCALE"] = "3"
os.environ["SE_IMAGE_OUT_SCALE"] = "1"
from overview_canvas import (
    Canvas, font, FONT_SANS, FONT_SANS_BOLD, SCALE, TITLE, SUBTITLE,
    PANEL, PANEL_EDGE, STEP_FILL, STEP_EDGE, LANG_FILL, LANG_EDGE,
    PTITLE, ARROW,
)

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "doc/images"


def arrow(canvas, points, head=True):
    """绘制任意方向的折线箭头，线端补圆并同步写入 SVG。"""
    scaled = [(x * SCALE, y * SCALE) for x, y in points]
    canvas.draw.line(scaled, fill=ARROW, width=3 * SCALE)
    for x, y in scaled:
        canvas.draw.ellipse((x - SCALE, y - SCALE, x + SCALE, y + SCALE), fill=ARROW)
    coords = " ".join(f"{a:g},{b:g}" for a, b in points)
    canvas.svg.append(f'<polyline points="{coords}" fill="none" stroke="{ARROW}" '
                      'stroke-width="3" stroke-linecap="round" stroke-linejoin="round"/>')
    if not head:
        return
    x, y = points[-1]
    px, py = points[-2]
    angle = math.atan2(y - py, x - px)
    head = [(x, y)] + [
        (x - 11 * math.cos(angle) + side * 5 * math.sin(angle),
         y - 11 * math.sin(angle) - side * 5 * math.cos(angle)) for side in (-1, 1)
    ]
    canvas.draw.polygon([(a * SCALE, b * SCALE) for a, b in head], fill=ARROW)
    tip = " ".join(f"{a:g},{b:g}" for a, b in head)
    canvas.svg.append(f'<polygon points="{tip}" fill="{ARROW}"/>')


def box(canvas, x, y, width, heading, lines, green=False, height=120):
    """绘制节点并检查每行文字宽度，避免文案超出边界。"""
    canvas.rect(x, y, width, height, LANG_FILL if green else PANEL,
                LANG_EDGE if green else PANEL_EDGE, radius=15)
    for offset, text in enumerate((heading, *lines)):
        face = font(FONT_SANS_BOLD if offset == 0 else FONT_SANS, 23 if offset == 0 else 18)
        if canvas.draw.textlength(text, font=face) / SCALE > width - 30:
            raise ValueError(f"流程节点文字过宽：{text}")
        canvas.text_center(x + width / 2, y + 30 + offset * 30, text, face,
                           PTITLE if offset == 0 else SUBTITLE)


def title(canvas, heading, detail):
    """绘制图题与简短说明。"""
    canvas.text(40, 58, heading, font(FONT_SANS_BOLD, 33), TITLE)
    canvas.text(40, 95, detail, font(FONT_SANS, 19), SUBTITLE)


def compilation(english=False):
    """区分条件预处理、语义期宏展开、四元式优化及 VM 装配出口。"""
    c = Canvas()
    title(c, "ChaLi · Compilation" if english else "ChaLi · 编译过程",
          "Generated front end → optimized quadruples → LLVM AOT" if english else
          "文法生成前端 → 语义与借用检查 → 四元式优化 → LLVM AOT")
    nodes = [
        (40, 145, "1  Source" if english else "1  源码与模块", [".chl + imports", "Embedded standard library"] if english else [".chl 与模块依赖", "内嵌标准库"]),
        (560, 145, "2  Preprocessing" if english else "2  条件预处理", ["%define / %if / %ifdef", "Source locations retained"] if english else ["%define / %if / %ifdef", "保留源文件与行号"]),
        (1080, 145, "3  Lexing and parsing" if english else "3  词法与语法解析", ["Generated from chali.gram", "Build AST, including macro templates"] if english else ["由 chali.gram 生成的前端", "构建 AST 与宏模板"]),
        (1080, 315, "4  Semantic analysis" if english else "4  语义分析与宏展开", ["Hygienic expansion + type inference", "Overloads and monomorphisation"] if english else ["卫生 AST 宏展开与类型推导", "名称、重载与泛型单态化"]),
        (560, 315, "5  Borrow checking" if english else "5  独立借用检查", ["Control flow and reference origins", "Ownership and lifetime diagnostics"] if english else ["控制流与引用来源", "拥有关系及生命周期诊断"]),
        (40, 315, "6  Generate quadruples" if english else "6  生成四元式", ["Compiler IR + data + layout", "Source locations retained"] if english else ["四元式 IR、数据区与布局", "保留诊断来源"]),
    ]
    for x, y, heading, lines in nodes:
        box(c, x, y, 440, heading, lines)
    for points in [((480, 205), (560, 205)), ((1000, 205), (1080, 205)),
                   ((1300, 265), (1300, 315)), ((1080, 375), (1000, 375)),
                   ((560, 375), (480, 375)), ((260, 435), (260, 485))]:
        arrow(c, points)
    box(c, 40, 485, 720, "7  Optimize quadruples" if english else "7  四元式优化",
        ["Full stackvm::optimize pipeline" if english else "完整 stackvm::optimize 优化流程",
         "Full optimization budget before LLVM lowering" if english else "完整优化预算；结果送入 LLVM 后端"], green=True)
    arrow(c, ((760, 545), (800, 545)))
    box(c, 800, 485, 720, "8  Lower to LLVM" if english else "8  降低为 LLVM",
        ["Types, labels, integer and comparison lowering" if english else "类型与标签、整数及比较跳转降低",
         "Reachable functions, sparse initial data" if english else "可达函数、稀疏初值与原生链接"], green=True)
    outputs = [
        (40, "Quadruple artifacts" if english else "四元式产物", ["Generated IR + data + layout", "Optimized view: CHALI_DUMP_OPT"] if english else ["生成的四元式、数据区与布局", "优化检查：CHALI_DUMP_OPT"]),
        (560, "run · checked AOT" if english else "run · checked AOT", ["Compile native code and execute", "Checks enabled"] if english else ["编译原生机器码并执行", "开启数组与指针检查"]),
        (1080, "aot · native export" if english else "aot · 原生导出", ["Unchecked export by default", "Executable or shared library"] if english else ["缺省 unchecked 导出", "可执行文件或动态库"]),
    ]
    arrow(c, ((40, 375), (20, 375), (20, 675), (260, 675), (260, 720)))
    arrow(c, ((1100, 605), (1100, 665), (780, 665), (780, 720)))
    arrow(c, ((1300, 605), (1300, 720)))
    for x, heading, lines in outputs:
        box(c, x, 720, 440, heading, lines, green=True)
    return c


def aot(english=False):
    """绘制四元式经 LLVM 生成原生产物的两种编译器配置。"""
    c = Canvas()
    title(c, "ChaLi · AOT export" if english else "ChaLi · AOT 导出流程",
          "Checked source → optimized quadruples → LLVM → native artifact" if english else
          "源码检查 → 四元式优化 → LLVM 优化与机器码生成 → 原生产物")
    rows = [
        (40, "1  Source and checks" if english else "1  源码与前端检查",
         [".chl / imports / AST macros", "Types, ownership and lifetimes"] if english else
         [".chl、模块与 AST 宏", "类型、拥有关系及生命周期"]),
        (560, "2  Optimized quadruples" if english else "2  优化后的四元式",
         ["Shared front end and optimizer", "Direct LLVM instruction lowering"] if english else
         ["复用前端与四元式优化器", "直接降低为 LLVM 指令"]),
        (1080, "3  LLVM module" if english else "3  LLVM 中间表示",
         [".ll + embedded AOT runtime", "--emit-llvm stops here"] if english else
         [".ll 与内嵌 AOT 运行时", "--emit-llvm 在此停止"]),
    ]
    for x, heading, lines in rows:
        box(c, x, 150, 440, heading, lines)
    arrow(c, ((480, 210), (560, 210)))
    arrow(c, ((1000, 210), (1080, 210)))
    arrow(c, ((1300, 270), (1300, 310), (400, 310), (400, 355)))
    arrow(c, ((1300, 310), (1160, 310), (1160, 355)))
    box(c, 40, 355, 720, "4a  Built-in LLVM" if english else "4a  静态内置 LLVM",
        ["LLVM optimization + x64 object code" if english else "LLVM 优化与 x64 目标代码生成",
         "In-process LLD + embedded CRT assets" if english else "进程内 LLD 与内嵌 CRT 链接资产"], green=True)
    box(c, 800, 355, 720, "4b  External LLVM" if english else "4b  外置 LLVM 工具链",
        ["LLVM clang driver + LLD" if english else "调用 LLVM clang 驱动与 LLD",
         "library/llvm or explicit --llvm path" if english else "library/llvm 或 --llvm 指定路径"], green=True)
    arrow(c, ((400, 475), (400, 520), (1160, 520), (1160, 475)), head=False)
    arrow(c, ((780, 520), (780, 565)))
    box(c, 40, 565, 1480, "5  Link native artifact" if english else "5  链接原生产物",
        ["Object code + AOT runtime + CRT + explicit C ABI libraries" if english else
         "目标代码 + AOT 运行时 + CRT + 显式声明的 C ABI 库",
         "--shared / --export / --link select the output and integration" if english else
         "通过 --shared、--export、--link 配置动态库、导出函数与链接库"], green=True)
    arrow(c, ((780, 685), (780, 735)))
    box(c, 40, 735, 1480, "6  Deliver and run" if english else "6  交付与运行",
        ["Executable or shared library; no VM or LLVM DLL required by the artifact" if english else
         "可执行文件或动态库；产物无需 VM 或 LLVM DLL",
         "Ship application C ABI dependencies when used" if english else
         "使用应用自身的 C ABI 动态库时，一并交付对应依赖"], height=120)
    return c


def main():
    """生成四套 PNG/SVG 资产，所有文案与几何布局均来自本脚本。"""
    OUT.mkdir(parents=True, exist_ok=True)
    for english in (False, True):
        for name, draw in (("compilation", compilation), ("aot", aot)):
            path = OUT / f"chali-{name}{'-en' if english else ''}"
            draw(english).save(str(path.with_suffix('.png')), str(path.with_suffix('.svg')))
            if not english:
                book_path = ROOT / 'doc/book/images' / path.with_suffix('.png').name
                book_path.parent.mkdir(parents=True, exist_ok=True)
                book_path.write_bytes(path.with_suffix('.png').read_bytes())
            print(path.name)


if __name__ == "__main__":
    main()
