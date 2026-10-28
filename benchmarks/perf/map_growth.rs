// map_growth：标准 HashMap 默认随机哈希，不预留容量。
use std::collections::HashMap;
use std::time::Instant;
const N:i64=20000;
const REPS:usize=5;
fn key(index:i64,seed:i64)->i64 { (index*65537+seed*17)%1000000007+1000000007 }
fn work(seed:i64)->i64 {
    let mut total=seed;
    let mut values: HashMap<i64,i64> = HashMap::new();
    for i in 0..N { values.insert(key(i,seed),i*3+seed); }
    for i in 0..N {
        let k=key(i,seed);
        total=(total*31+values[&k])&2147483647;
        values.insert(k,i*5+seed);
    }
    for i in 0..N {
        let k=key(i,seed);
        total=(total*31+values[&k]+i64::from(values.contains_key(&-k)))&2147483647;
    }
    for i in (0..N).step_by(3) { total+=i64::from(values.remove(&key(i,seed)).is_some()); }
    for i in (0..N).step_by(3) { values.insert(key(i,seed),i*7+seed); }
    for i in 0..N { total=(total*31+values[&key(i,seed)])&2147483647; }
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
