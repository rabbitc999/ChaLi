use std::sync::OnceLock;
static MATRIX: OnceLock<[[i64; 64]; 64]> = OnceLock::new();
fn setup() {
    MATRIX.set(std::array::from_fn(|row| std::array::from_fn(|column|
        row as i64 * 131 + column as i64 * 17 + 3))).ok().unwrap();
}
#[inline(never)]
fn work(seed: i64) -> i64 {
    let matrix = MATRIX.get().unwrap();
    let mut acc = seed;
    for i in 0..262144 {
        let row = ((acc >> 6) & 63) as usize;
        let column = ((i + acc) & 63) as usize;
        acc = (acc * 33 + matrix[row][column]) & 2147483647;
    }
    acc
}
include!("timing.rs");
