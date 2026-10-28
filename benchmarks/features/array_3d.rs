use std::sync::OnceLock;
static CUBE: OnceLock<[[[i64; 16]; 16]; 16]> = OnceLock::new();
fn setup() {
    CUBE.set(std::array::from_fn(|x| std::array::from_fn(|y|
        std::array::from_fn(|z| x as i64 * 257 + y as i64 * 31 + z as i64 * 7 + 3)))).ok().unwrap();
}
#[inline(never)]
fn work(seed: i64) -> i64 {
    let cube = CUBE.get().unwrap();
    let mut acc = seed;
    for i in 0..262144 {
        let x = ((acc >> 4) & 15) as usize;
        let y = ((acc >> 8) & 15) as usize;
        let z = ((i + seed) & 15) as usize;
        acc = (acc * 33 + cube[x][y][z]) & 2147483647;
    }
    acc
}
include!("timing.rs");
