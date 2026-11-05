"""C GUI 绘图及纹理的真实像素验收；Pillow 仅用于读取测试 PNG。"""
from pathlib import Path
import ctypes as c,math,threading
from PIL import Image
import argparse
root=Path(__file__).resolve().parents[1]
parser=argparse.ArgumentParser(description='使用真实隐藏窗口验证绘图像素与 GPU 资源寿命')
parser.add_argument('--library',type=Path,required=True)
args=parser.parse_args()
out=root/'tmp/gui-drawing-native';out.mkdir(parents=True,exist_ok=True)
lib=c.CDLL(str(args.library.resolve()))
class Point(c.Structure):
 """两个 double 的公开 C 值布局。"""
 _fields_=[('x',c.c_double),('y',c.c_double)]
class Bounds(c.Structure):
 """裁剪区的四个 double 布局。"""
 _fields_=[('x',c.c_double),('y',c.c_double),('width',c.c_double),('height',c.c_double)]
def f(name,args,result=c.c_bool):
 """绑定实际 C 签名，避免默认整数宽度截断。"""
 fun=getattr(lib,'chali_gui_'+name);fun.argtypes=args;fun.restype=result;return fun
open=f('open_ex',[c.c_int32,c.c_int32,c.c_char_p,c.c_int32]);close=f('close',[],None)
begin=f('begin_frame',[]);end=f('end_frame',[],None);capture=f('test_capture',[c.c_char_p])
rectangle=f('draw_rectangle',[c.c_float]*5+[c.c_int32,c.c_float,c.c_uint32])
circle=f('draw_circle',[c.c_float]*4+[c.c_int32,c.c_uint32])
line=f('draw_line',[c.c_float]*5+[c.c_uint32]);pixel=f('draw_pixel',[c.c_float,c.c_float,c.c_uint32])
ellipse=f('draw_ellipse',[c.c_float]*4+[c.c_int32,c.c_uint32])
triangle=f('draw_triangle',[c.c_float]*6+[c.c_int32,c.c_uint32])
polygon=f('draw_polygon',[c.c_float,c.c_float,c.c_int32,c.c_float,c.c_float,c.c_float,c.c_uint32])
bezier=f('draw_bezier',[c.c_float]*9+[c.c_uint32]);ring=f('draw_ring',[c.c_float]*6+[c.c_int32,c.c_uint32])
text=f('draw_text',[c.c_char_p,c.c_float,c.c_float,c.c_float,c.c_float,c.c_uint32]);measure=f('measure_text',[c.c_char_p,c.c_float,c.c_float],Point)
clip=f('begin_clip',[c.c_float]*4);unclip=f('end_clip',[],None);bounds=f('clip_bounds',[],Bounds)
load=f('texture_load',[c.c_char_p],c.c_int64);valid=f('texture_valid',[c.c_int64]);drop=f('texture_close',[c.c_int64],None)
texture=f('texture_draw',[c.c_int64]+[c.c_float]*11+[c.c_uint32]);size=f('texture_size',[c.c_int64],Point)
assert not rectangle(0,0,20,20,0,16,0,0xff0000ff)
assert open(480,360,b'Native drawing',2)
assert not rectangle(0,0,20,20,0,16,0,0xff0000ff)
assert measure(b'Test',20,1).x>0
asset=out/'图像.png';Image.new('RGBA',(16,16),(12,34,56,255)).save(asset)
t=load(str(asset).encode());assert t and valid(t) and size(t).x==16 and size(t).y==16
foreign_results=[]
def foreign():
 """异线程调用必须在接触图形上下文前拒绝。"""
 foreign_results.append((valid(t),load(str(asset).encode()),pixel(1,1,0xffffffff)))
thread=threading.Thread(target=foreign);thread.start();thread.join();assert foreign_results==[(False,0,False)]
assert begin()
assert not rectangle(math.nan,0,20,20,0,16,0,0xff0000ff)
assert not line(0,0,10,10,-1,0xffffffff)
assert rectangle(10,10,60,40,0,16,0,0xff0000ff)
assert circle(110,30,20,0,64,0x00ff00ff)
assert ellipse(180,30,30,15,0,0x0000ffff)
assert triangle(230,10,270,10,230,50,0,0xff00ffff)
assert triangle(300,10,300,50,340,10,0,0xffff00ff)
assert polygon(390,30,6,20,0,0,0x00ffffff)
assert line(10,80,100,80,4,0x000000ff)
assert bezier(120,70,150,110,190,30,220,80,3,0x800080ff)
assert ring(280,90,10,25,0,270,64,0x008000ff)
assert circle(350,90,20,3,64,0x808000ff)
assert ellipse(420,90,25,15,1,0x000000ff)
assert rectangle(10,130,80,50,0.3,16,2,0x800000ff)
assert text(b'Native text',110,140,20,1,0x000000ff)
assert texture(t,0,0,16,16,300,140,32,32,0,0,0,0xffffffff)
second=load(str(asset).encode());assert second
assert texture(second,0,0,16,16,350,140,32,32,0,0,0,0xffffffff)
drop(second);assert not valid(second)
assert clip(20,220,40,40);assert clip(40,240,40,40)
b=bounds();assert(b.x,b.y,b.width,b.height)==(40,240,20,20)
assert rectangle(0,200,120,100,0,16,0,0x123456ff);unclip();unclip()
image_path=out/'drawing.png';assert capture(str(image_path).encode());end()
im=Image.open(image_path).convert('RGBA');assert im.size==(480,360)
for position,expected in [((20,20),(255,0,0)),((110,30),(0,255,0)),((180,30),(0,0,255)),((240,20),(255,0,255)),((310,20),(255,255,0)),((390,30),(0,255,255)),((310,150),(12,34,56)),((360,150),(12,34,56)),((45,245),(18,52,86))]:
 assert im.getpixel(position)[:3]==expected,(position,im.getpixel(position),expected)
assert im.getpixel((25,225))[:3]==(245,245,245)
drop(t);drop(t);assert not valid(t)
t=load(str(asset).encode());assert t;close();assert not valid(t)
assert open(160,120,b'Reopened drawing',2);assert not valid(t);close()
print('C drawing: real pixels, both triangle windings, clipping, UTF-8 PNG, owner thread and texture generations passed')
