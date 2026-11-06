"""ChaLi 1.0.0 无音频宣传片；ManimGL 1.7.2。

从 doc/images/ 读取小猫宣传图与图标。
"""
from pathlib import Path
from manimlib import *

HERE = Path(__file__).resolve().parents[2] / 'doc' / 'images'
PAPER = '#FFF8EF'
INK = '#193B5A'
MUTED = '#718297'
BLUE = '#789EBF'
GOLD = '#F4BE47'
LINE = '#DCE3EA'
TEAL = '#5D9E9B'


def text(value, size=32, color=INK, mono=False):
    return Text(value, font='Consolas' if mono else 'Microsoft YaHei', font_size=size).set_color(color)


def panel(width, height, color=WHITE):
    return RoundedRectangle(width=width, height=height, corner_radius=.18, fill_color=color, fill_opacity=1, stroke_color=LINE, stroke_width=1.4)


def pill(value, width, color=WHITE, size=24):
    return VGroup(panel(width,.65,color),text(value,size))


class ChaLiPromo(Scene):
    def construct(self):
        self.base = VGroup(
            Rectangle(width=FRAME_WIDTH,height=FRAME_HEIGHT,fill_color=PAPER,fill_opacity=1,stroke_width=0),
            Circle(radius=2.4,fill_color='#F9E8C5',fill_opacity=.36,stroke_width=0).move_to([7,3,0]),
            Circle(radius=2.0,fill_color='#DDE8F0',fill_opacity=.4,stroke_width=0).move_to([-7,-3.4,0]),
        )
        self.brand = text('ChaLi 1.0.0',22).move_to([-5.7,3.55,0])
        self.add(self.base)
        self.opening()
        self.add(self.brand)
        self.first_line()
        self.libraries()
        self.syntax()
        self.learning()
        self.ending()

    def title(self, value, subtitle=None):
        heading=text(value,50).to_edge(LEFT,buff=.75).set_y(2.6)
        self.play(FadeIn(heading,shift=UP*.14),run_time=.7)
        if subtitle:
            sub=text(subtitle,25,MUTED).to_edge(LEFT,buff=.78).set_y(1.82)
            self.play(FadeIn(sub,shift=UP*.1),run_time=.5)

    def finish(self, start, duration, clear=True):
        transition=.65 if clear else 0
        remaining=duration-(self.get_time()-start)-transition
        if remaining>0:self.wait(remaining)
        if clear:
            moving=Group(*[item for item in self.mobjects if item is not self.base and item is not self.brand])
            self.play(FadeOut(moving,shift=UP*.12),run_time=transition)

    def opening(self):
        start=self.get_time()
        hero=ImageMobject(str(HERE/'chali-poster.png')).set_width(FRAME_WIDTH+.08)
        band=Rectangle(width=FRAME_WIDTH,height=1.13,fill_color=PAPER,fill_opacity=.94,stroke_width=0).to_edge(DOWN,buff=0)
        caption=text('把想法，搭成作品。',43).set_y(-3.43)
        self.play(FadeIn(hero),run_time=1)
        self.play(FadeIn(band),FadeIn(caption,shift=UP*.12),run_time=.8)
        self.play(hero.animate.scale(1.025),run_time=4.5,rate_func=smooth)
        self.finish(start,8)

    def first_line(self):
        start=self.get_time()
        self.title('从第一行，开始创造。','熟悉的表达方式，迈出自己的第一步。')
        editor=panel(8.0,3.5).move_to([-1.9,-.25,0])
        dots=VGroup(*[Dot(radius=.055,color=c) for c in ['#D98C7D',GOLD,TEAL]]).arrange(RIGHT,buff=.13).move_to([-5.35,1.16,0])
        filename=text('hello.chl',20,MUTED,True).move_to([-1.9,1.16,0])
        self.play(FadeIn(editor),FadeIn(dots),FadeIn(filename),run_time=.6)
        rows=VGroup(text('void main() {',29,INK,True),text('    println("Hello, ChaLi!");',29,INK,True),text('}',29,INK,True)).arrange(DOWN,aligned_edge=LEFT,buff=.25).move_to([-2.1,-.3,0])
        self.play(LaggedStart(*[Write(row) for row in rows],lag_ratio=.35),run_time=1.7)
        result=VGroup(panel(3.1,1.65,'#ECF3F6'),text('Hello, ChaLi!',27,INK,True)).move_to([4.25,-.25,0])
        arrow=Arrow([2.2,-.25,0],[2.65,-.25,0],buff=0,color=BLUE,stroke_width=3)
        self.play(ShowCreation(arrow),FadeIn(result,shift=RIGHT*.15),run_time=.8)
        command=pill('chali run hello.chl',6.2,size=27).move_to([-2.3,-2.65,0])
        note=text('LLVM AOT · 原生运行',25,TEAL).move_to([3.45,-2.65,0])
        self.play(FadeIn(command),FadeIn(note),run_time=.6)
        self.finish(start,10)

    def feature_icon(self,kind,color):
        if kind=='gui':
            frame=RoundedRectangle(width=.85,height=.64,corner_radius=.09,stroke_color=color,stroke_width=3)
            bar=Line([-.42,.15,0],[.42,.15,0],color=color,stroke_width=2)
            square=RoundedRectangle(width=.25,height=.22,corner_radius=.04,fill_color=color,fill_opacity=.65,stroke_width=0).move_to([-.17,-.09,0])
            return VGroup(frame,bar,square)
        if kind=='audio':
            bars=VGroup()
            for index,height in enumerate([.26,.46,.7,.5,.34]):
                bar=RoundedRectangle(width=.1,height=height,corner_radius=.04,fill_color=color,fill_opacity=1,stroke_width=0)
                bar.move_to([-.32+.16*index,0,0]);bars.add(bar)
            return bars
        if kind=='xml':return text('< />',36,color,True)
        body=RoundedRectangle(width=.68,height=.43,corner_radius=.08,fill_color=color,fill_opacity=1,stroke_width=0).shift(DOWN*.16)
        arch=Arc(start_angle=0,angle=PI,radius=.23,color=color,stroke_width=5).shift(UP*.13)
        return VGroup(body,arch)

    def libraries(self):
        start=self.get_time()
        self.title('小组件，组成大作品。','从界面到数据，为创作准备常用能力。')
        specs=[('GUI','窗口、绘图与事件回调','gui',BLUE),('音频','为应用加入声音能力','audio',TEAL),('XML','读取、遍历与转义','xml',BLUE),('密码学','随机数、摘要与签名验证','crypto',GOLD)]
        cards=[]
        for index,(name,description,kind,color) in enumerate(specs):
            icon=self.feature_icon(kind,color).move_to([-1.95,0,0])
            label=text(name,31).move_to([.25,.3,0])
            desc=text(description,20,MUTED).move_to([.35,-.38,0])
            card=VGroup(panel(5.75,1.85),icon,label,desc).move_to([(-3.1 if index%2==0 else 3.1),(.4 if index<2 else -1.75),0])
            cards.append(card)
        self.play(LaggedStart(*[FadeIn(card,shift=UP*.2) for card in cards],lag_ratio=.22),run_time=1.8)
        note=text('x64 编译器内嵌原生服务',23,MUTED).set_y(-3.2)
        self.play(FadeIn(note),run_time=.6)
        self.play(cards[0].animate.shift(UP*.09),cards[3].animate.shift(UP*.09),rate_func=there_and_back,run_time=1.4)
        self.finish(start,12)

    def syntax(self):
        start=self.get_time()
        self.title('让表达，更直接。','数组、元组与清晰的值语义。')
        left=VGroup(panel(5.75,2.85),text('数组',24,MUTED).set_y(.85),text('T[] a',64,INK,True),text('局部拥有数组',24,MUTED).set_y(-.95)).move_to([-3.1,-.25,0])
        right=VGroup(panel(5.75,2.85),text('元组',24,MUTED).set_y(.85),text('pairs[0][0]',43,INK,True),text('按常量下标读取',24,MUTED).set_y(-.95)).move_to([3.1,-.25,0])
        self.play(FadeIn(left,shift=RIGHT*.2),FadeIn(right,shift=LEFT*.2),run_time=.9)
        checks=VGroup(pill('类型检查',3.35,'#EAF1F5',24),pill('所有权与借用',3.35,'#F9ECD1',24),pill('资源管理',3.35,'#EAF1F5',24)).arrange(RIGHT,buff=.4).set_y(-2.6)
        self.play(LaggedStart(*[FadeIn(item,shift=UP*.12) for item in checks],lag_ratio=.2),run_time=1.1)
        self.finish(start,10)

    def learning(self):
        start=self.get_time()
        self.title('从入门，到真正的作品。','书稿、参考手册与示例，随时陪你查阅。')
        mascot=ImageMobject(str(HERE/'chali-logo.png')).set_height(3.75).move_to([-4.4,-.5,0])
        self.play(FadeIn(mascot,shift=UP*.16),run_time=.8)
        rows=[]
        for index,(number,label) in enumerate([('30','章中文书稿'),('50','个标准库模块'),('158','份示例源码')]):
            y=.65-index*1.15
            bg=panel(7.0,.95).move_to([2.35,y,0])
            count=text(number,50,INK,True).move_to([-.05,y,0])
            caption=text(label,29).move_to([3.1,y,0])
            rows.append(VGroup(bg,count,caption))
        self.play(LaggedStart(*[FadeIn(row,shift=RIGHT*.18) for row in rows],lag_ratio=.25),run_time=1.7)
        formats=text('CHM + 压缩 HTML · 目录、索引、离线阅读',25,MUTED).set_y(-3.0)
        self.play(FadeIn(formats),run_time=.6)
        self.finish(start,12)

    def ending(self):
        start=self.get_time()
        title=text('ChaLi 1.0.0',76).set_y(2.25)
        subtitle=text('下一件作品，从这里开始。',39).set_y(1.25)
        self.play(FadeIn(title,shift=UP*.1),FadeIn(subtitle,shift=UP*.1),run_time=1)
        platforms=VGroup(pill('Windows x64',3.65,size=25),pill('Ubuntu 22.04 x64',3.65,size=24),pill('CentOS Stream 9 x64',3.65,size=22)).arrange(RIGHT,buff=.3).set_y(-.05)
        self.play(LaggedStart(*[FadeIn(item,shift=UP*.15) for item in platforms],lag_ratio=.18),run_time=1.1)
        url=VGroup(panel(9.2,.95,'#F6D47D'),text('github.com/rabbitc999/ChaLi',31,INK,True)).set_y(-1.5)
        self.play(FadeIn(url,shift=UP*.12),run_time=.7)
        note=text('Ubuntu ARM64：编译器前端与 LSP',23,MUTED).set_y(-2.65)
        self.play(FadeIn(note),run_time=.5)
        self.finish(start,12,clear=False)
