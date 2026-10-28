// set_growth：标准 HashSet 默认随机哈希，不预留容量。
use std::collections::HashSet;
use std::time::Instant;
const N:i64=20000;
const REPS:usize=5;
fn key(index:i64,seed:i64)->i64 { (index*65537+seed*17)%1000000007+1000000007 }
fn work(seed:i64)->i64 {
    let mut total=seed;
    let mut values: HashSet<i64> = HashSet::new();
    for i in 0..N { values.insert(key(i,seed)); }
    for i in 0..N { values.insert(key(i,seed)); }
    for i in 0..N {
        let k=key(i,seed);
        total=(total*31+i64::from(values.contains(&k))+2*i64::from(values.contains(&-k)))&2147483647;
    }
    for i in (0..N).step_by(3) { total+=i64::from(values.remove(&key(i,seed))); }
    for i in (0..N).step_by(3) { values.insert(key(i,seed)); }
    for i in 0..N { total=(total*31+i64::from(values.contains(&key(i,seed))))&2147483647; }
    total
}

fn main() {
    let kernel: fn(i64) -> i64 = work;
    let mut sink = kernel(0);
    for r in 0..REPS {
        let start = Instant::now();
        sink += std::hint::black_box(kernel)(r as i64);
        println!("rep {}", start.elapsed().as_nanos());
    }
    println!("sink {}", sink);
}
