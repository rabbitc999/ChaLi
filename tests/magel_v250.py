"""0.17.0 的条件表达式与自定义传播：真实解释执行、AOT 和错误诊断回归。"""
from pathlib import Path
import os
import subprocess
import tempfile

ROOT=Path(__file__).resolve().parents[1]
COMPILER=ROOT/('magel.exe' if os.name=='nt' else 'magel')


def main():
    """覆盖短路、初始化和赋值、嵌套出口、资源以及传播协议。"""
    cases={
        'if': r'''int64 calls=0;
int32 next(){calls=calls+1;return 2;}
int32 select(bool good){return if(good){1}else{2};}
int32 take(int32 x){return x;}
int64 order=0;int32 first(){order=order*10+1;return 1;}int32 second(){order=order*10+2;return 2;}
int32 pair(int32 a,int32 b){return a+b;}
int64 main(){
 int32 p=1;
 bool odd=if(p%2==1){true}else{false};
 int32 x=if(p%3==1){1}elif(next()==2){2}else{3};
 assert(odd);assert(x==1);assert(calls==0);
 x=if(false){next()}else{int32 f=3;break f;};assert(x==3);assert(calls==0);
 x=take(if(true){4}else{next()});assert(x==4);assert(calls==0);
 x=if(false){1}elif(next()==2){5}elif(next()==2){6}else{7};assert(x==5);assert(calls==1);
 x=10+(if(true){2}else{next()});assert(x==12);assert(calls==1);
 assert(select(false)==2);
 x=first()+(if(true){second()}else{3});assert(x==3);assert(order==12);
 order=0;x=pair(first(),if(true){second()}else{3});assert(x==3);assert(order==12);
 bool skipped=false && (if(true){next()==2}else{true});assert(!skipped);assert(calls==1);
 String text=if(false){"bad"}else{String local="value";break local+"!";};assert(text=="value!");
 int64 count=0;
 while(count<3){int32 value=if(count==0){count=count+1;continue;}else{2};assert(value==2);break;}
 assert(count==1);
 println("if ok");return 0;
}''',
        'custom': r'''enum Checked : Try<int64,String>, FromResidual<Checked,String> {
 Value(int64 value), Failed(String error);
 TryBranch<int64,String> branch(){match(self){
 Checked::Value(value)=>{return TryBranch<int64,String>::Continue(value);}
 Checked::Failed(error)=>{return TryBranch<int64,String>::Break(error);}}}
 static Checked fromResidual(String error){return Checked::Failed(error);}
};
Checked read(bool good){if(good){return Checked::Value(3);}return Checked::Failed("bad");}
Checked run(bool good){auto x=read(good)?;return Checked::Value(x+1);}
int64 main(){match(run(true)){Checked::Value(v)=>{assert(v==4);}Checked::Failed(e)=>{panic(e);}}
match(run(false)){Checked::Value(v)=>{panic("bad success");}Checked::Failed(e)=>{assert(e=="bad");}}
println("custom ok");return 0;}
''',
        'adapters': r'''int64 errors=0;
String errorText(int64 code){errors=errors+1;return "error";}
String missing(){errors=errors+1;return "missing";}
Result<int64,String> run(bool good){
 auto x=if(good){Result<int64,int64>::Ok(4)}else{Result<int64,int64>::Err(2)};
 auto value=x.mapErr<String>(errorText)?;
 return Result<int64,String>::Ok(value+1);
}
int64 main(){assert(run(true).unwrap()==5);assert(errors==0);assert(run(false).error()=="error");assert(errors==1);
 Option<int64> present=Option<int64>::Value(7);Option<int64> absent=Option<int64>::None;
 assert(present.okOr<String>("no").unwrap()==7);
 assert(present.okOrElse<String>(missing).unwrap()==7);assert(errors==1);
 assert(absent.okOrElse<String>(missing).error()=="missing");assert(errors==2);
 assert(absent.okOr<String>("no").error()=="no");
 assert(Result<int64,String>::Err("bad").ok().isNone());println("adapters ok");return 0;}
''',
    }
    cases['interop']=r'''int64 moved=0;
enum Packet : Try<String,ErrorResidual<String>> {
 Value(String value), Failed(String error);
 TryBranch<String,ErrorResidual<String>> branch(){match(self){
 Packet::Value(value)=>{return TryBranch<String,ErrorResidual<String>>::Continue(value);}
 Packet::Failed(error)=>{ErrorResidual<String> residual;residual.error=error;return TryBranch<String,ErrorResidual<String>>::Break(residual);}}}
};
Packet packet(bool good){if(good){return Packet::Value("abc");}return Packet::Failed("bad");}
Result<int64,String> standard(bool good){auto text=packet(good)?;return Result<int64,String>::Ok(len(text));}
struct Reply : FromResidual<Reply,ErrorResidual<String>> {
 bool good;String message;
 static Reply fromResidual(ErrorResidual<String> residual){Reply reply;reply.good=false;reply.message=residual.error;return reply;}
};
Reply custom(bool good){auto value=standard(good)?;Reply reply;reply.good=true;reply.message="ok";return reply;}
enum MoveCarrier : TryMove<int64,NoneResidual>, FromResidual<MoveCarrier,NoneResidual> {
 Value(int64 value), Empty;
 TryBranch<int64,NoneResidual> intoBranch(){moved=moved+1;match(self){
 MoveCarrier::Value(value)=>{return TryBranch<int64,NoneResidual>::Continue(value);}
 MoveCarrier::Empty=>{return TryBranch<int64,NoneResidual>::Break(NoneResidual::None);}}}
 static MoveCarrier fromResidual(NoneResidual residual){return MoveCarrier::Empty;}
};
MoveCarrier consume(bool good){MoveCarrier value=if(good){MoveCarrier::Value(4)}else{MoveCarrier::Empty};auto number=move(&value)?;return MoveCarrier::Value(number+1);}
int64 global=9;
int64 const& globalRef(bool good){return if(good){&global}else{&global};}
int64 main(){assert(standard(true).unwrap()==3);assert(standard(false).error()=="bad");
 Reply success=custom(true);Reply failure=custom(false);assert(success.good);assert(!failure.good);assert(failure.message=="bad");
 match(consume(true)){MoveCarrier::Value(value)=>{assert(value==5);}MoveCarrier::Empty=>{panic("bad move");}}
 match(consume(false)){MoveCarrier::Value(value)=>{panic("bad move");}MoveCarrier::Empty=>{}}
 assert(moved==2);int64 local=3;int64 const& selected=if(true){&local}else{&global};assert(selected==3);assert(globalRef(false)==9);
 println("interop ok");return 0;}
'''
    errors={
      'missing_else':('int64 main(){int32 x=if(true){1};return 0;}','else'),
      'missing_value':('int64 main(){bool x=if(true){true;}else{false};return 0;}','产生结果'),
      'mixed_types':('int64 main(){auto x=if(true){1}else{"no"};return 0;}','类型'),
      'unknown_try':('int64 main(){auto x=1?;return 0;}','Try'),
      'missing_receiver':('Result<int64,String> f(){Option<int64> x=Option<int64>::None;auto y=x?;return Result<int64,String>::Ok(y);}int64 main(){return 0;}','FromResidual'),
      'branch_borrow':('int64 const& bad(){return if(true){int64 local=1;break &local;}else{int64 other=2;break &other;};}int64 main(){return 0;}','引用'),
      'choose_removed':('int64 main(){return choose(true,1,2);}','choose'),
      'choose_lazy_removed':('int64 main(){return chooseLazy(true,fn(){1},fn(){2});}','chooseLazy'),
    }
    with tempfile.TemporaryDirectory(prefix='magel-250-') as directory:
        folder=Path(directory)
        for name,source in cases.items():
            if os.environ.get('MAGEL_V250_CASE') and name!=os.environ['MAGEL_V250_CASE']:continue
            path=folder/(name+'.mgl');path.write_text(source,encoding='utf-8')
            run=subprocess.run([str(COMPILER),str(path),'--out',str(folder/(name+'-vm'))],capture_output=True,text=True,encoding='utf-8',timeout=90)
            assert run.returncode==0,(name,run.stdout,run.stderr)
            executable=folder/(name+('.exe' if os.name=='nt' else ''))
            build=subprocess.run([str(COMPILER),'aot',str(path),'out',str(executable),'opt','0'],capture_output=True,text=True,encoding='utf-8',timeout=180)
            assert build.returncode==0,(name,build.stdout,build.stderr)
            aot=subprocess.run([str(executable)],capture_output=True,text=True,encoding='utf-8',timeout=30)
            assert aot.returncode==0 and aot.stdout==run.stdout,(name,aot.stdout,aot.stderr,run.stdout)
            print(name+': VM / AOT 通过',flush=True)
        for name,(source,fragment) in errors.items():
            if os.environ.get("MAGEL_V250_CASE"):continue
            path=folder/(name+'.mgl');path.write_text(source,encoding='utf-8')
            run=subprocess.run([str(COMPILER),'check',str(path)],capture_output=True,text=True,encoding='utf-8',timeout=90)
            assert run.returncode!=0 and fragment in run.stdout+run.stderr,(name,run.stdout,run.stderr)
    print('0.17.0 条件表达式、短路、传播与适配器回归全部通过。')


if __name__=='__main__':main()
