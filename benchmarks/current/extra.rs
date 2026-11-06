// 缺少的 Rust 接口与数组读写对照，使用真实 dyn trait 分派和标准定长数组。
trait Operation { fn apply(&self,v:i64)->i64; }
struct Multiply;struct Subtract;struct ExclusiveOr;
impl Operation for Multiply {fn apply(&self,v:i64)->i64{(v*3+7)&2147483647}}
impl Operation for Subtract {fn apply(&self,v:i64)->i64{(v*5-11)&2147483647}}
impl Operation for ExclusiveOr {fn apply(&self,v:i64)->i64{((v*7)^13)&2147483647}}
static FIRST:Multiply=Multiply;static SECOND:Subtract=Subtract;static THIRD:ExclusiveOr=ExclusiveOr;
fn setup(){}
#[cfg(interface_mono)]
fn work(seed:i64)->i64 {let operation:&dyn Operation=&FIRST;let mut a=seed;for _ in 0..262144{a=operation.apply(a);}a}
#[cfg(interface_poly)]
fn work(seed:i64)->i64 {let mut a=seed;for i in 0..262144{let operation:&dyn Operation=match(i+seed)&3{1=>&SECOND,2=>&THIRD,_=>&FIRST};a=operation.apply(a);}a}
#[cfg(array_rw)]
fn work(seed:i64)->i64 {let mut v=[0i64;16];for k in 0..16{v[k]=k as i64*3;}let mut a=seed;for i in 0..4000000{let k=(a&15)as usize;v[k]=(v[k]+i)&2147483647;a=(a+v[k])&2147483647;}a}
include!("../features/timing.rs");
