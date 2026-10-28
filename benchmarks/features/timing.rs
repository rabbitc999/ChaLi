// 与 C 壳保持相同的构造、预热、五次工作及校验归并顺序。
fn main() {
    setup();
    let kernel = std::hint::black_box(work as fn(i64) -> i64);
    if std::env::args().any(|arg| arg == "--verify") {
        for seed in 0..5 { println!("value {} {}", seed, kernel(seed)); }
        return;
    }
    let mut sink = kernel(std::hint::black_box(0));
    for seed in 0..5 {
        let start = std::time::Instant::now();
        let value = kernel(std::hint::black_box(seed));
        let elapsed = start.elapsed().as_nanos();
        sink += value;
        println!("rep {}", elapsed);
    }
    println!("sink {}", std::hint::black_box(sink));
}
