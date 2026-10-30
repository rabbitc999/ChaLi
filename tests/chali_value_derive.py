"""验证内建相等、哈希和克隆派生的字段、载荷与拥有值语义。"""
import argparse
import os
from pathlib import Path
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT=Path(__file__).resolve().parents[1]


def main():
    """执行值语义与明确拒绝用例，不用原始内存比较充当协议实现。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=compiler_path())
    parser.add_argument('--case',action='append',default=[])
    args=parser.parse_args()
    leaf='''#derive(Equal,Hash,Clone)
struct Leaf{int64 number;String text;}
'''
    choice=leaf+'''#derive(Equal,Hash,Clone)
enum Choice{None,Pair(Leaf item,String text),Triple(int8 a,int16 b,int32 c),Quad(int8 a,int16 b,int32 c,int64 d)};
'''
    cases=[
        ('scalars','''#derive(Equal,Hash,Clone)
struct Scalars{bool active;char letter;int8 a;uint8 b;int16 c;uint16 d;int32 e;uint32 f;int64 g;uint64 h;float32 i;float64 j;String text;}
void main(){Scalars a;a.active=true;a.letter='Z';a.a=-128;a.b=255;a.c=-32768;a.d=65535;a.e=-2147483648;a.f=4294967295u;a.g=-9223372036854775808;a.h=18446744073709551615u;a.i=(float32)1.5;a.j=-2.25;a.text="值tail";Scalars b=a.clone();assert(a==b&&a.hash(a)==b.hash(b));Map<Scalars,int64> values;values.put(a,7);assert(values.has(b)!=0);b.text="changed";assert(!(a==b)&&values.has(b)==0&&a.text=="值tail");}''',None),
        ('static_fields','''struct Configuration{int64 id;}
#derive(Equal,Hash,Clone)
struct Data{static Configuration configuration;int64 value;}
void main(){Data a;a.value=7;Data b=a.clone();assert(a==b&&a.hash(a)==b.hash(b));}''',None),
        ('nested_owned',leaf+'''#derive(Equal,Hash,Clone)
struct Box<T>{T value;String label;}
void main(){Box<Leaf> a;a.value.number=7;a.value.text="original";a.label="box";let b=a.clone();assert(a==b&&a.hash(a)==b.hash(b));b.value.text="copy";b.label="other";assert(a.value.text=="original"&&a.label=="box"&&!(a==b));}''',None),
        ('enum_payloads',choice+'''void main(){Leaf leaf;leaf.number=7;leaf.text="owned";Choice a=Choice::Pair(leaf,"pair");let b=a.clone();assert(a==b&&a.hash(a)==b.hash(b));Choice c=Choice::Triple(1,2,3);let d=c.clone();assert(c==d&&c.hash(c)==d.hash(d));Choice e=Choice::Quad(1,2,3,4);let f=e.clone();assert(e==f&&e.hash(e)==f.hash(f)&&!(c==e)&&!(a==Choice::None));}''',None),
        ('nested_enum',choice+'''#derive(Equal,Hash,Clone)
struct Box{Choice choice;}
void main(){Box a;a.choice=Choice::Quad(1,2,3,4);let b=a.clone();assert(a==b&&a.hash(a)==b.hash(b));}''',None),
        ('enum_in_enum',choice+'''#derive(Equal,Hash,Clone)
enum Outer{None,Value(Choice value)};
void main(){Outer a=Outer::Value(Choice::Triple(1,2,3));let b=a.clone();assert(a==b&&a.hash(a)==b.hash(b));}''',None),
        ('float_zero','''#derive(Equal,Hash,Clone)
struct Value{float32 small;float64 large;}
void main(){Value positive;positive.small=(float32)0.0;positive.large=0.0;Value negative;negative.small=(float32)-0.0;negative.large=-0.0;assert(positive==negative&&positive.hash(positive)==negative.hash(negative));Map<Value,int64> values;values.put(positive,7);assert(values.has(negative)!=0);}''',None),
        ('hash_uses_argument','''#derive(Equal,Hash)
struct Value{int64 number;}
void main(){Value a;a.number=1;Value b;b.number=2;assert(a.hash(b)==b.hash(b));}''',None),
        ('unicode_map','''#derive(Equal,Hash,Clone)
struct Label{String text;}
void main(){
Label a;a.text="中文😀suffix";let b=a.clone();Map<Label,int64> values;values.put(a,7);
assert(a==b);
assert(a.hash(a)==b.hash(b));
assert(values.has(b)!=0);
b.text="中文😀other";
assert(!(a==b));
assert(values.has(b)==0);
}''',None),
        ('manual_nested','''int64 clones=0;
struct Key:Equal<bool,Key>,Hash<Key>,Clone<Key>{int64 id;String label;
bool leftEqual(Key other){return self.id==other.id;}
uint64 hash(Key value){return (uint64)value.id;}
Key clone(){clones+=1;Key result;result.id=self.id;result.label=self.label;return result;}}
#derive(Equal,Hash,Clone)
struct Box{Key key;}
void main(){Box a;a.key.id=7;a.key.label="original";let b=a.clone();assert(clones==1);b.key.label="changed";assert(a==b&&a.hash(a)==b.hash(b)&&a.key.label=="original");Map<Box,int64> values;values.put(a,9);assert(values.has(b)!=0);}''',None),
        ('resource_clone','''int64 closed=0;
struct Resource:Closeable,Clone<Resource>{int64 id;
void close(){if(self.id!=0){closed+=1;self.id=0;}}
Resource clone(){Resource result;result.id=self.id+1;return move result;}}
#derive(Clone)
struct Box{Resource resource;}
void main(){{Box a;a.resource.id=1;let b=a.clone();assert(a.resource.id==1&&b.resource.id==2);}assert(closed==2);}''',None),
        ('resource_enum_clone','''int64 closed=0;
struct Resource:Closeable,Clone<Resource>{int64 id;
void close(){if(self.id!=0){closed+=1;self.id=0;}}
Resource clone(){Resource result;result.id=self.id+1;return move result;}}
#derive(Clone)
enum Owned{None,Value(Resource resource)};
void main(){{Resource source;source.id=1;Owned a=Owned::Value(move source);let b=a.clone();assert(closed==0);}assert(closed==2);}''',None),
        ('missing_generic','struct Plain{int64 value;}#derive(Hash) struct Box<T>{T value;}void main(){Box<Plain> value;}',['未实现 Hash']),
        ('missing_equal','struct Plain{int64 value;}#derive(Equal) struct Box{Plain value;}void main(){}',['未实现 Equal']),
        ('missing_hash','struct Plain{int64 value;}#derive(Hash) struct Box{Plain value;}void main(){}',['未实现 Hash']),
        ('missing_clone','struct Plain{int64 value;}#derive(Clone) struct Box{Plain value;}void main(){}',['未实现 Clone']),
        ('noncopyable','struct Resource:Closeable{void close(){}}#derive(Clone) struct Box{Resource value;}void main(){}',['未实现 Clone']),
        ('array','''#derive(Equal) struct Box{int64 values[2];}void main(){}''',['数组字段']),
        ('reference','''#derive(Hash) struct Box{int64 const& value;}void main(){}''',['未实现 Hash']),
        ('conflict','''#derive(Hash) struct Box{uint64 hash(Box value){return 7u;}}void main(){}''',['不覆盖已有方法']),
    ]
    if args.case:
        cases=[case for case in cases if case[0] in args.case]
        assert len(cases)==len(set(args.case))
    failures=[]
    with tempfile.TemporaryDirectory(prefix='value-derive-') as directory:
        work=Path(directory)
        for name,source,diagnostics in cases:
            path=work/(name+'.chl')
            path.write_text(source,encoding='utf-8')
            result=subprocess.run([str(args.chali.resolve()),'check' if diagnostics else 'run',str(path),'--out',str(work/name)],cwd=ROOT,env=dict(os.environ,CHALI_LANG='zh'),capture_output=True,text=True,encoding='utf-8',timeout=240)
            valid=result.returncode!=0 and all(text in result.stderr for text in diagnostics) if diagnostics else (result.returncode,result.stdout,result.stderr)==(0,'','')
            if not valid:failures.append((name,result.returncode,result.stdout,result.stderr))
            print(name,'通过' if valid else ('失败 '+repr(failures[-1])),flush=True)
    assert not failures,failures
    print(f'值语义派生通过：{len(cases)} 项')


if __name__=='__main__':main()
