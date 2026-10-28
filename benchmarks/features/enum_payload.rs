use std::sync::OnceLock;
struct Payload { x: i64, y: i64 }
enum Event { Empty, One(i64), Pair(i64, i64), Record(Payload) }
static EVENTS: OnceLock<Vec<Event>> = OnceLock::new();
fn setup() {
    EVENTS.set((0..256).map(|i| match i % 4 {
        0 => Event::Empty,
        1 => Event::One(i * 3 + 7),
        2 => Event::Pair(i * 5, i + 11),
        _ => Event::Record(Payload { x: i * 7, y: i + 13 }),
    }).collect()).ok().unwrap();
}
#[inline(never)]
fn work(seed: i64) -> i64 {
    let events = EVENTS.get().unwrap();
    let mut acc = seed;
    for i in 0..262144 {
        let index = ((i * 13 + ((acc >> 7) & 255)) & 255) as usize;
        let value = match &events[index] {
            Event::Empty => 17,
            Event::One(value) => *value,
            Event::Pair(left, right) => left + right,
            Event::Record(value) => value.x + value.y,
        };
        acc = (acc * 33 + value) & 2147483647;
    }
    acc
}
include!("timing.rs");
