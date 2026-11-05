"""把生命游戏验收的实际帧整理为书稿图片，并生成界面说明图。"""
import argparse
from pathlib import Path
import shutil
from PIL import Image, ImageDraw, ImageFont

ROOT = Path(__file__).resolve().parents[1]


def main():
    """保留实际截图内容，用独立说明栏解释界面区域。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--capture-dir', type=Path, default=ROOT / 'tmp/life-regression')
    parser.add_argument('--font', type=Path, help='支持中文的 TrueType/OpenType 字体')
    args = parser.parse_args()
    candidates = [args.font, Path('C:/Windows/Fonts/msyh.ttc'),
                  Path('/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc')]
    font = next((p for p in candidates if p and p.is_file()), None)
    if font is None:
        raise SystemExit('请用 --font 指定支持中文的字体。')
    destination = ROOT / 'doc/book/images'
    destination.mkdir(parents=True, exist_ok=True)
    frames = ('paused', 'running', 'speed', 'panned')
    for name in frames:
        with Image.open(args.capture_dir / (name + '.png')) as frame:
            if frame.size != (1000, 680):
                raise SystemExit('截图尺寸不符：' + name)
    for name in frames:
        shutil.copy2(args.capture_dir / (name + '.png'), destination / ('life-' + name + '.png'))
    with Image.open(args.capture_dir / 'paused.png') as frame:
        base = frame.convert('RGB')
    canvas = Image.new('RGB', (1400, 800), '#f3f6fb')
    canvas.paste(base, (24, 92))
    draw = ImageDraw.Draw(canvas)
    normal = ImageFont.truetype(str(font), 21)
    small = ImageFont.truetype(str(font), 17)
    title = ImageFont.truetype(str(font), 29)
    draw.text((24, 22), '康威生命游戏 / 界面与操作', font=title, fill='#172b4d')
    entries = [((118, 214), '1  运行控制', 'Start / Pause 开始与暂停', 'Step 单步，Clear 清空'),
               ((740, 214), '2  演化速率', '1–60 代/秒', '界面目标刷新率独立为 60 FPS'),
               ((450, 420), '3  自定义棋盘', '暂停：左键切换细胞', '右键拖动：平移查看世界'),
               ((280, 724), '4  状态栏', '显示运行状态、代数、活细胞数', '视口外细胞继续参与演化')]
    for index, (point, heading, line1, line2) in enumerate(entries):
        y = 110 + index * 165
        draw.rounded_rectangle((1045, y, 1375, y + 137), 12, fill='white', outline='#d6dfed', width=2)
        draw.text((1060, y + 12), heading, font=normal, fill='#1a547e')
        draw.text((1060, y + 53), line1, font=small, fill='#263c56')
        draw.text((1060, y + 88), line2, font=small, fill='#516a87')
        x, y = point
        draw.ellipse((x - 14, y - 14, x + 14, y + 14), fill='#176bd6', outline='white', width=2)
        draw.text((x - 5, y - 12), str(index + 1), font=small, fill='white')
    canvas.save(destination / 'life-interface.png')
    print('生命游戏：四张实际帧与界面说明图已更新。')


if __name__ == '__main__':
    main()
