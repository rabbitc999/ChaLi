use std::sync::OnceLock;
struct Data { text: String, first: String, second: String, width: usize }
static DATA: OnceLock<Data> = OnceLock::new();
const MASK: i64 = 2147483647;

fn setup() {
    let unit = "ChaLi-字符串-🙂-0123456789|";
    let first = format!("{}a", unit);
    let second = format!("{}B", unit);
    let width = first.len();
    let mut text = String::new();
    for index in 0..256 { text.push_str(unit); text.push((b'A' + (index % 26) as u8) as char); }
    assert!(DATA.set(Data { text, first, second, width }).is_ok());
}

#[inline(never)]
fn read_work(mut seed: i64) -> i64 {
    let source = DATA.get().unwrap().text.as_bytes();
    for index in 0..131072_i64 {
        let at = ((index * 13 + ((seed >> 7) & 65535)) % source.len() as i64) as usize;
        seed = (seed * 33 + source[at] as i64) & MASK;
    }
    seed
}

#[inline(never)]
fn concat_work(seed: i64) -> i64 {
    let data = DATA.get().unwrap();
    let mut result = String::new();
    for index in 0..2048_i64 {
        result.push_str(if (index + seed) & 1 == 0 { &data.first } else { &data.second });
    }
    let mut acc = seed;
    for value in result.bytes() { acc = (acc * 33 + value as i64) & MASK; }
    acc
}

#[inline(never)]
fn slice_work(mut seed: i64) -> i64 {
    let data = DATA.get().unwrap();
    let initial = seed;
    for index in 0..1024_i64 {
        let block = (index * 13 + (seed & 127)) & 127;
        let start = block as usize * data.width;
        let width = (((index + initial) & 3) + 1) as usize * data.width;
        let other = if index & 1 == 0 { start } else { ((block + 13) & 127) as usize * data.width };
        let part = data.text[start..start + width].to_owned();
        let compared = data.text[other..other + width].to_owned();
        seed = (seed * 33 + part.as_bytes()[0] as i64 + part.as_bytes()[part.len() - 1] as i64 +
                if part == compared { 1 } else { 0 }) & MASK;
    }
    seed
}

#[inline(never)]
fn find_work(mut seed: i64) -> i64 {
    let data = DATA.get().unwrap();
    let initial = seed;
    let patterns = ["字符串", "🙂", "不存在!", "012345"];
    for index in 0..256_i64 {
        let start = (((seed >> 7) + index) & 255) as usize * data.width;
        let found = data.text[start..].find(patterns[((index + initial) & 3) as usize]);
        let position = found.map(|at| (start + at) as i64).unwrap_or(-1);
        seed = (seed * 33 + position + 1) & MASK;
    }
    seed
}

#[cfg(string_read)] use read_work as work;
#[cfg(string_concat)] use concat_work as work;
#[cfg(string_slice)] use slice_work as work;
#[cfg(string_find)] use find_work as work;
include!("../features/timing.rs");
