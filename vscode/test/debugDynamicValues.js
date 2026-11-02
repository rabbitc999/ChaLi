'use strict';

/** 核对接口实际类型、按值快照、分页和未知类型，禁止目标执行。 */
const assert = require('node:assert/strict');
const { decode } = require('../src/debugValue');
const boxed = { kind: 'O', name: 'Read', children: [] };
const record = { kind: 'S', name: 'Row', width: 16, children: [
  { kind: 'i', name: 'first', width: 8, offset: 0 },
  { kind: 'i', name: 'second', width: 8, offset: 8 },
] };
async function main() {
  let reads = 0, revision = 1;
  const values = [{ flags: 0, bits: 42n }, { flags: 0, bits: 4096n }];
  const memory = {
    validateObjects: async () => { assert.equal(revision, 1, '对象快照已经失效'); },
    object: async () => { await memory.validateObjects(); return { kind: 'O', count: 2, first: 0, values }; },
    dynamicLayout: id => id === 42n ? record : null,
    read: async (address, count) => {
      ++reads; assert.equal(count, 8); assert([4096n, 4104n].includes(address));
      const bytes = Buffer.alloc(8); bytes.writeBigInt64LE(address === 4096n ? 7n : 9n); return bytes;
    },
  };
  assert.equal((await decode(boxed, 100n, memory, { count: 0 })).text, 'Row {…}');
  assert.equal(reads, 0, '摘要读取了无关字段');
  const page = await decode(boxed, 100n, memory, { start: 1, count: 1 });
  assert.deepEqual(page.children.map(item => [item.name, item.value]), [['second', '9']]);
  values[1] = { flags: 1, first: 2, count: 2 };
  values.push({ flags: 0, bits: 11n }, { flags: 0, bits: 12n });
  assert.deepEqual((await decode(boxed, 100n, memory)).children.map(item => item.value), ['11', '12']);
  values[0].bits = 0n;
  assert.equal((await decode(boxed, 100n, memory)).text, '<未初始化装箱>');
  values[0].bits = 99n;
  assert.equal((await decode(boxed, 100n, memory)).text, '<未知实际类型 99>');
  values[0].bits = 42n;
  const floating = Buffer.alloc(8); floating.writeDoubleLE(1.25);
  values[1] = { flags: 4, bits: floating.readBigUInt64LE() };
  memory.dynamicLayout = () => ({ kind: 'f', name: 'float32', width: 8, children: [] });
  assert.equal((await decode(boxed, 100n, memory)).text, 'float32 1.25', '存储宽度不能冒充实际类型');
  values[0].flags = 4;
  await assert.rejects(decode(boxed, 100n, memory), /类型号/);
  revision = 2;
  await assert.rejects(decode(boxed, 100n, memory), /失效/);
  console.log('动态装箱：实际类型、按值／地址存储、分页、未知／空类型、非法标签和失效拒绝通过');
}
main().catch(error => { console.error(error); process.exitCode = 1; });
