"""运行时 C JSON 与原解析器的差分及独立快照所有权回归。"""
from pathlib import Path
import ctypes as c,json,random
import argparse,os,subprocess,shlex
ROOT=Path(__file__).resolve().parents[1]
parser=argparse.ArgumentParser(description='C JSON 与保留的编译器 JSON 逐字节差分及独立所有权检查')
parser.add_argument('--library',type=Path,required=True)
parser.add_argument('--cxx',default=os.environ.get('CXX','g++'))
parser.add_argument('--reference-library',type=Path)
args=parser.parse_args()
root=ROOT/'tmp/json-c-reference';root.mkdir(parents=True,exist_ok=True)
reference=args.reference_library
if not reference:
 reference=root/('oracle.dll' if os.name=='nt' else 'oracle.so')
 subprocess.run([args.cxx,'-std=c++17','-O2','-shared','-static-libgcc','-static-libstdc++',
                 *(['-static'] if os.name=='nt' else ['-fPIC']),*shlex.split(os.environ.get('CXXFLAGS','')),
                 str(ROOT/'tests/chali_json_c_reference.cpp'),str(ROOT/'source/json/json.cpp'),
                 '-o',str(reference)],check=True,cwd=ROOT)
new=c.CDLL(str(args.library.resolve()));old=c.CDLL(str(reference.resolve()));crt=c.CDLL('msvcrt.dll') if os.name=='nt' else c.CDLL(None)
crt.free.argtypes=[c.c_void_p]
def bind(lib,name,args,result):
 f=getattr(lib,name);f.argtypes=args;f.restype=result;return f
parse=bind(new,'chali_json_parse',[c.c_char_p,c.c_size_t],c.c_void_p)
valid=bind(new,'chali_json_valid',[c.c_void_p],c.c_int)
kind=bind(new,'chali_json_kind',[c.c_void_p],c.c_int)
dump=bind(new,'chali_json_dump',[c.c_void_p,c.POINTER(c.c_size_t)],c.c_void_p)
free=bind(new,'chali_json_free',[c.c_void_p],None)
oparse=bind(old,'oracle_parse',[c.c_char_p,c.c_size_t],c.c_void_p)
okind=bind(old,'oracle_kind',[c.c_void_p],c.c_int)
odump=bind(old,'oracle_dump',[c.c_void_p,c.POINTER(c.c_size_t)],c.c_void_p)
ofree=bind(old,'oracle_free',[c.c_void_p],None)
otextfree=bind(old,'oracle_text_free',[c.c_void_p],None)
rng=random.Random(1934)
def tree(depth=0):
 choices=[None,False,True,rng.randrange(-2**63,2**63),rng.uniform(-1e100,1e100),'中🌏\\\"\n\t\0'+str(rng.randrange(100))]
 if depth<5:
  choice=rng.randrange(10)
  if choice==8:return [tree(depth+1) for _ in range(rng.randrange(7))]
  if choice==9:return {'key'+str(i):tree(depth+1) for i in range(rng.randrange(6))}
 return rng.choice(choices)
cases=[json.dumps(tree(),ensure_ascii=False,separators=(',',':')).encode() for _ in range(1600)]
cases += [b'{"a":1,"b":2,"a":3}',b'{"a":1,"a":null}',b'1e400',b'-1e400',b'1e-400',b'9223372036854775808',b'18446744073709551616',b'"\xff"',b'"\\ud800"',b'"\\udc00"',b'"\\ud800\\udc00"',b'"\\u0000"',b'\xef\xbb\xbf{}',b'',b'null x',b'NaN',b'Infinity',b'+1',b'01',b'[1,]',b'{"x":1,}',b'"\\x20"',b'"\x00"']
cases += [b'['*n+b'0'+b']'*n for n in (255,256,257,300)]
cases += [b'"'+bytes([i])+b'"' for i in range(256)]
cases += [b'"\\'+bytes([i])+b'"' for i in range(256)]
cases += [b'"\\u'+format(i,'04x').encode()+b'"' for i in [0,31,127,128,0xd7ff,0xd800,0xdbff,0xdc00,0xdfff,0xe000,0xffff]]
for i,text in enumerate(cases):
 a=parse(text,len(text));b=oparse(text,len(text));assert a
 try:
  assert bool(valid(a))==bool(b),(i,text,'parse',bool(valid(a)),bool(b))
  if b:
   assert kind(a)==okind(b),(i,text,'kind',kind(a),okind(b))
   n=c.c_size_t();m=c.c_size_t();x=dump(a,c.byref(n));y=odump(b,c.byref(m));assert x and y
   try:
    actual=c.string_at(x,n.value)
    assert actual==c.string_at(y,m.value),(i,text,'dump',actual,c.string_at(y,m.value))
    if i<1600:assert json.loads(actual)==json.loads(text),(i,text,'independent Python model')
   finally:crt.free(x);otextfree(y)
 finally:free(a);ofree(b)
print('C JSON exact baseline parse/type/dump differential:',len(cases),'passed')

clone=bind(new,'chali_json_clone',[c.c_void_p],c.c_void_p)
close=bind(new,'chali_json_close',[c.c_void_p],None)
find=bind(new,'chali_json_find',[c.c_void_p,c.c_char_p,c.c_size_t],c.c_void_p)
size=bind(new,'chali_json_size',[c.c_void_p],c.c_size_t)
at=bind(new,'chali_json_at',[c.c_void_p,c.c_size_t],c.c_void_p)
key=bind(new,'chali_json_key',[c.c_void_p,c.c_size_t,c.POINTER(c.c_size_t)],c.c_void_p)
get_text=bind(new,'chali_json_text',[c.c_void_p,c.POINTER(c.c_size_t)],c.c_void_p)
integer=bind(new,'chali_json_integer',[c.c_void_p],c.c_int64)
for _ in range(300):
 text=b'{"a\\u0000b":"x\\u0000y","items":[1,2,3],"empty":null}'
 parent=parse(text,len(text));assert valid(parent) and size(parent)==3
 length=c.c_size_t();p=key(parent,0,c.byref(length));assert c.string_at(p,length.value)==b'a\0b'
 child=find(parent,b'a\0b',3);snapshot=clone(child);items=find(parent,b'items',5)
 close(parent);close(parent);assert not valid(parent) and valid(child) and size(items)==3
 p=get_text(snapshot,c.byref(length));assert c.string_at(p,length.value)==b'x\0y'
 leaf=at(items,2);assert integer(leaf)==3
 missing=at(items,3);assert missing and not valid(missing)
 for value in (parent,child,snapshot,items,leaf,missing):free(value)
print('C JSON: 300 independent child/clone/close/NUL-key lifecycle checks passed')
