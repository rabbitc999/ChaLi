'use strict';

/** 二版容器目录通过真实 MI 字节格式读取，验证范围、预算、递归与竞态。 */
const assert = require('node:assert/strict');
const { DebugMemory } = require('../src/debugMemory');
function target(change = () => {}, race = false) {
  let valueReads = 0;
  const header = Buffer.alloc(24), row = Buffer.alloc(160), context = Buffer.alloc(8), revision = Buffer.alloc(8);
  const object = Buffer.alloc(32), value = Buffer.alloc(96);
  [2n, 8n, 8192n].forEach((item, i) => header.writeBigUInt64LE(item, i * 8));
  row.writeBigUInt64LE(42n); row.writeBigUInt64LE(2n, 8); row.writeBigUInt64LE(1n, 64);
  row.writeBigUInt64LE(12288n, 80);
  [20480n, 1n, 24576n, 1n, 0n, 0n, 16384n, 7n].forEach((item, i) => row.writeBigUInt64LE(item, 96 + i * 8));
  revision.writeBigUInt64LE(7n);
  [100n, 76n, 1n, 0n].forEach((item, i) => object.writeBigUInt64LE(item, i * 8));
  value.writeBigUInt64LE(42n, 8);
  change({ header, row, object, value, revision });
  const blocks = [[4096n, header], [8192n, row], [12288n, context], [16384n, revision], [20480n, object], [24576n, value]];
  const reader = new DebugMemory(async command => {
    if (command.startsWith('-thread-info')) return { threads: [{ id: '1', state: 'stopped', 'target-id': 'Thread 100.42' }] };
    if (command.startsWith('-data-evaluate-expression')) return { value: '4096' };
    const match = /^-data-read-memory-bytes (0x[0-9a-f]+) ([0-9]+)$/.exec(command);
    assert(match, command);
    const address = BigInt(match[1]), size = Number(match[2]);
    const block = blocks.find(([base, bytes]) => address >= base && address + BigInt(size) <= base + BigInt(bytes.length));
    assert(block, '越界物理读取：' + command);
    const begin = Number(address - block[0]);
    if (block[1] === value) ++valueReads;
    if (race && block[1] === value) revision.writeBigUInt64LE(8n);
    return { memory: [{ begin: match[1], contents: block[1].subarray(begin, begin + size).toString('hex') }] };
  });
  return { reader, revision, valueReads: () => valueReads };
}
async function main() {
  const dynamic = await target(state => {
    state.header.writeBigUInt64LE(3n);
    state.row.writeBigUInt64LE(2n, 120);
    state.object.writeBigUInt64LE(79n, 8);
    state.object.writeBigUInt64LE(2n, 16);
    state.value.writeBigUInt64LE(4096n, 56);
  }).reader.snapshot(1);
  assert.equal((await dynamic.object(100n)).kind, 'O');
  assert.equal((await dynamic.object(100n)).values[1].bits, 4096n);
  const healthy = target(), snapshot = await healthy.reader.snapshot(1);
  const concurrent = await Promise.all(Array.from({ length: 8 }, () => snapshot.object(100n)));
  assert(concurrent.every(object => object.values[0].bits === 42n));
  assert.equal(healthy.valueReads(), 1, '并发容器查看重复读取值表');
  await assert.rejects(snapshot.object(999n), /释放/);
  healthy.revision.writeBigUInt64LE(8n);
  await assert.rejects(snapshot.object(100n), /失效/);
  for (const change of [
    state => state.row.writeBigUInt64LE(4097n, 104),
    state => state.object.writeBigUInt64LE(65536n + 76n, 8),
    state => state.object.writeBigUInt64LE(2n, 16),
    state => { state.value.writeBigUInt64LE(1n); state.value.writeBigUInt64LE(1n, 24); },
    state => state.value.writeBigUInt64LE(1n, 40),
    state => state.value.writeBigUInt64LE(3n),
    state => { state.header.writeBigUInt64LE(3n); state.object.writeBigUInt64LE(79n, 8); },
    state => state.object.writeBigUInt64LE(79n, 8),
  ]) {
    const broken = await target(change).reader.snapshot(1);
    await assert.rejects(broken.object(100n));
  }
  const raced = await target(undefined, true).reader.snapshot(1);
  await assert.rejects(raced.object(100n), /失效/);
  console.log('容器目录：二版读取、预算、种类、索引、递归、释放及代次竞态拒绝通过');
}
main().catch(error => { console.error(error); process.exitCode = 1; });
