"""C TOML 的递归原实现差分、Python 模型与独立快照回归。"""
from pathlib import Path
import ctypes as c,random,json,math
try:
 import tomllib
except ModuleNotFoundError:
 import tomli as tomllib
import argparse,os,subprocess,shlex,base64
ROOT=Path(__file__).resolve().parents[1]
parser=argparse.ArgumentParser(description='运行时 C TOML 的原实现差分、独立模型和快照所有权回归')
parser.add_argument('--library',type=Path,required=True)
parser.add_argument('--cxx',default=os.environ.get('CXX','g++'))
parser.add_argument('--reference-library',type=Path)
args=parser.parse_args()
root=ROOT/'tmp/toml-c-reference';root.mkdir(parents=True,exist_ok=True)
reference=args.reference_library
if not reference:
 reference=root/('oracle.dll' if os.name=='nt' else 'oracle.so')
 subprocess.run([args.cxx,'-std=c++17','-O2','-shared','-static-libgcc','-static-libstdc++',
                 *(['-static'] if os.name=='nt' else ['-fPIC']),*shlex.split(os.environ.get('CXXFLAGS','')),
                 str(ROOT/'tests/chali_toml_c_reference.cpp'),str(ROOT/'source/toml/toml.cpp'),str(ROOT/'source/toml/toml_source.cpp'),
                 '-o',str(reference)],check=True,cwd=ROOT)
new=c.CDLL(str(args.library.resolve()));old=c.CDLL(str(reference.resolve()));crt=(c.CDLL('msvcrt.dll') if os.name=='nt' else c.CDLL(None));crt.free.argtypes=[c.c_void_p]
def bind(lib,name,args,result):
 f=getattr(lib,name);f.argtypes=args;f.restype=result;return f
parse=bind(new,'chali_toml_parse',[c.c_char_p,c.c_size_t],c.c_void_p)
valid=bind(new,'chali_toml_valid',[c.c_void_p],c.c_int)
free=bind(new,'chali_toml_free',[c.c_void_p],None)
error=bind(new,'chali_toml_error',[c.c_void_p],c.c_char_p)
oparse=bind(old,'oracle_parse',[c.c_char_p,c.c_size_t],c.c_void_p)
ofree=bind(old,'oracle_free',[c.c_void_p],None)
textfree=bind(old,'oracle_text_free',[c.c_void_p],None)
kinds=[bind(new,'chali_toml_kind',[c.c_void_p],c.c_int),bind(old,'oracle_kind',[c.c_void_p],c.c_int)]
sizes=[bind(new,'chali_toml_size',[c.c_void_p],c.c_size_t),bind(old,'oracle_size',[c.c_void_p],c.c_size_t)]
dumps=[bind(new,'chali_toml_dump',[c.c_void_p,c.POINTER(c.c_size_t)],c.c_void_p),bind(old,'oracle_dump',[c.c_void_p,c.POINTER(c.c_size_t)],c.c_void_p)]
ats=[bind(new,'chali_toml_at',[c.c_void_p,c.c_size_t],c.c_void_p),bind(old,'oracle_at',[c.c_void_p,c.c_size_t],c.c_void_p)]
keys=[bind(new,'chali_toml_key',[c.c_void_p,c.c_size_t,c.POINTER(c.c_size_t)],c.c_void_p),bind(old,'oracle_key',[c.c_void_p,c.c_size_t,c.POINTER(c.c_size_t)],c.c_void_p)]
def dumped(which,value):
 length=c.c_size_t();out=dumps[which](value,c.byref(length));assert out
 try:return c.string_at(out,length.value)
 finally:(crt.free if which==0 else textfree)(out)
def walk(a,b,path):
 assert kinds[0](a)==kinds[1](b),(path,'kind')
 x,y=dumped(0,a),dumped(1,b);assert x==y,(path,'dump',x,y)
 assert sizes[0](a)==sizes[1](b),(path,'size')
 for i in range(sizes[0](a)):
  if kinds[0](a)==6:
   names=[]
   for which,value in [(0,a),(1,b)]:
    length=c.c_size_t();p=keys[which](value,i,c.byref(length));assert p;names.append(c.string_at(p,length.value))
   assert names[0]==names[1],(path,'keys',names)
  child=ats[0](a,i);reference=ats[1](b,i)
  try:walk(child,reference,path+[i])
  finally:free(child);ofree(reference)
rng=random.Random(20261105)
def value(depth=0):
 k=rng.randrange(8 if depth<4 else 6)
 if k==0:return 'true' if rng.randrange(2) else 'false'
 if k==1:return str(rng.randrange(-2**63,2**63))
 if k==2:return repr(rng.uniform(-1e40,1e40))
 if k==3:return json.dumps('中🌏\\\"\n\t'+str(rng.randrange(100)),ensure_ascii=False)
 if k==4:return rng.choice(['1979-05-27','07:32:00.123456789','1979-05-27T07:32:00.123456789Z','1979-05-27 07:32:00.123456789-07:00'])
 if k==5:return rng.choice(['inf','-inf','nan','0xdead_beef','0o755','0b1010'])
 if k==6:return '['+', '.join(value(depth+1) for _ in range(rng.randrange(5)))+']'
 return '{'+', '.join('key'+str(i)+' = '+value(depth+1) for i in range(rng.randrange(5)))+'}'
cases=[('# preserved\n'+''.join('key'+str(i)+' = '+value()+' # item\n' for i in range(rng.randrange(1,7)))).encode() for _ in range(600)]
cases += [b'',b'\xef\xbb\xbfx = 1\n',b'a = {"a.b" = 1, x = {z=2}, y = [1,2]}\n',b'[a]\nx = 1\n[a.b]\ny=2\n',b'[[a]]\nx=1\n[[a]]\nx=2\n',b'x="\\u0000"\n',b'x=1979-05-27T07:32:60Z\n']
cases += [b'x='+b'['*n+b'0'+b']'*n for n in (29,30,31,255,256,257)]
fixtures=json.loads((ROOT/'tests/fixtures/tomlc17_parser_cases.json').read_text(encoding='utf-8'))
cases += [base64.b64decode(row['base64']) for row in fixtures['cases']]
cases += [b'x='+token+b'\n' for token in [b'0',b'-0',b'+0',b'1_000',b'0_1',b'-0x1',b'+0x1',b'9223372036854775808',b'-9223372036854775809',b'1e999999',b'-1e999999',b'1e-999999',b'.1',b'1.',b'01',b'01.0',b'1e1',b'+inf',b'-nan',b'"\\u0000"',b'"\\ud800"',b'"\\udc00"',b'"\\ud800\\udc00"',b'"\\x41"',b'"\\e"',b'07:32',b'1979-05-27T07:32Z',b'07:32:00.123456789123',b'1979-05-27t07:32:00.000000001+00:00']]
cases += [b'a={x=1,}\n',b'a={x=1,\ny=2}\n',b'a={x=[1,\n2]}\n',b'a={x=[1, # allowed inside array\n2]}\n',b'a={x=1 # inline comment invalid\n}\n',b'a={x=1}\na.x=2\n',b'x=1\nx=2\n',b'x=[1,]\n']
cases += [b'\xef\xbb\xbf'+text for text in [b'x=1979-05-27T07:32:60.123456789Z\n',b'x=07:32:00.123456789\n',b'x=1e999999\n',b'[1e999999]\nx=1\n','"中文"=1979-05-27T07:32:00.123456789Z\n'.encode('utf-8')]]
cases += [b'x=0000-01-01\n',b'x=2000-02-31\n',b'x=2001-02-29T12:34:56Z\n',b'x=2020-04-31\n',b'x=2.2250738585072014e-308\n',b'x=5e-324\n']
cases += [b'x=1.'+b'0'*200+b'1\n',b'x=07:32:00.'+b'123456789'*30+b'\n',b'x=1979-05-27T07:32:60.'+b'123456789'*30+b'-07:00\n']
get_integer=bind(new,'chali_toml_integer',[c.c_void_p],c.c_int64)
get_real=bind(new,'chali_toml_real',[c.c_void_p],c.c_double)
get_boolean=bind(new,'chali_toml_boolean',[c.c_void_p],c.c_int)
get_string=bind(new,'chali_toml_text',[c.c_void_p,c.POINTER(c.c_size_t)],c.c_void_p)
get_date=bind(new,'chali_toml_date_time',[c.c_void_p,c.POINTER(c.c_size_t)],c.c_void_p)
def model(node,expected):
 import datetime
 kind=kinds[0](node)
 if isinstance(expected,bool):assert kind==1 and bool(get_boolean(node))==expected
 elif isinstance(expected,int):assert kind==2 and get_integer(node)==expected
 elif isinstance(expected,float):
  actual=get_real(node);assert kind==3 and ((math.isnan(actual) and math.isnan(expected)) or actual==expected)
 elif isinstance(expected,str):
  n=c.c_size_t();p=get_string(node,c.byref(n));assert kind==4 and p and c.string_at(p,n.value)==expected.encode('utf-8')
 elif isinstance(expected,(datetime.date,datetime.time)):
  n=c.c_size_t();p=get_date(node,c.byref(n));assert kind==7 and p
  assert tomllib.loads('value='+c.string_at(p,n.value).decode())['value']==expected
 else:
  assert kind==(6 if isinstance(expected,dict) else 5) and sizes[0](node)==len(expected)
  values=list(expected.values()) if isinstance(expected,dict) else expected
  if isinstance(expected,dict):
   for index,key_name in enumerate(expected):
    n=c.c_size_t();p=keys[0](node,index,c.byref(n));assert p and c.string_at(p,n.value)==key_name.encode('utf-8')
  for index,child_model in enumerate(values):
   child=ats[0](node,index)
   try:model(child,child_model)
   finally:free(child)
for i,text in enumerate(cases):
 a=parse(text,len(text));b=oparse(text,len(text));assert a
 try:
  assert bool(valid(a))==bool(b),(i,text,error(a),'parse',bool(b))
  if b:walk(a,b,[i])
  if i<600:model(a,tomllib.loads(text.decode()))
 finally:free(a);ofree(b)
print('C TOML recursive parse/type/key/dump differential:',len(cases),'passed')

clone=bind(new,'chali_toml_clone',[c.c_void_p],c.c_void_p)
close=bind(new,'chali_toml_close',[c.c_void_p],None)
find=bind(new,'chali_toml_find',[c.c_void_p,c.c_char_p,c.c_size_t],c.c_void_p)
date=bind(new,'chali_toml_date_time',[c.c_void_p,c.POINTER(c.c_size_t)],c.c_void_p)
for _ in range(300):
 text=b'# original\nx=1979-05-27T07:32:60.123456789Z\nitems=[1,2,3]\n'
 parent=parse(text,len(text));child=find(parent,b'x',1);snapshot=clone(child);whole=clone(parent)
 close(parent);close(parent);assert not valid(parent) and valid(child) and valid(snapshot)
 free(child)
 n=c.c_size_t();p=date(snapshot,c.byref(n));assert c.string_at(p,n.value)==b'1979-05-27T07:32:60.123456789Z'
 assert dumped(0,whole)==text
 for item in (parent,snapshot,whole):free(item)
print('C TOML: 300 parent/child/clone/date/original-source lifecycle checks passed')
