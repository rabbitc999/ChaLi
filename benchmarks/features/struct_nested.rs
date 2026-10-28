use std::sync::OnceLock;
struct Vector3 { x: i64, y: i64, z: i64 }
struct Transform { position: Vector3, velocity: Vector3, weights: [i64; 4] }
struct Record { transform: Transform, payload: [i64; 8], flags: i64 }
static RECORDS: OnceLock<Vec<Record>> = OnceLock::new();
fn setup() {
    let records = (0..256).map(|i| Record {
        transform: Transform {
            position: Vector3 { x: i * 3 + 1, y: i * 5 + 2, z: i * 7 + 3 },
            velocity: Vector3 { x: i + 4, y: i + 5, z: i + 6 },
            weights: std::array::from_fn(|k| i * 13 + k as i64),
        },
        payload: std::array::from_fn(|k| i * 17 + k as i64 * 3),
        flags: i * 11 + 9,
    }).collect();
    RECORDS.set(records).ok().unwrap();
}
#[inline(never)]
fn work(seed: i64) -> i64 {
    let records = RECORDS.get().unwrap();
    let mut acc = seed;
    for i in 0..262144 {
        let record = &records[((i * 13 + (acc & 255)) & 255) as usize];
        let value = record.transform.position.x + record.transform.position.z
            + record.transform.velocity.y + record.transform.weights[(i & 3) as usize]
            + record.payload[((i + seed) & 7) as usize] + record.flags;
        acc = (acc * 33 + value) & 2147483647;
    }
    acc
}
include!("timing.rs");
