'use strict';

/** 核对闭包标签、捕获顺序、稳定单元、只读提示与预算内分页，不执行目标代码。 */
const assert = require('node:assert/strict');
const { decode } = require('../src/debugValue');
const fn = { kind: 'F', name: 'int64 fn()', children: [] };
async function main() {
  const capture = { kind: 'i', name: 'int64', width: 8, children: [] };
  const info = { name: 'counter', captures: [
    { name: 'borrowed', layout: 0, mode: 0, address: true },
    { name: 'copied', layout: 0, mode: 2, address: false },
  ] };
  const values = [{ flags: 0, bits: 24n }, { flags: 0, bits: 4096n }, { flags: 8, bits: 4104n }];
  let valid = true, reads = 0;
  const memory = {
    closureInfo: address => address === 24n ? info : null,
    layout: index => index === 0 ? capture : null,
    object: async () => { assert(valid, '闭包快照已经失效'); return { kind: 'F', count: 3, first: 0, values }; },
    read: async (address, count) => {
      ++reads; assert.equal(count, 8); assert([4096n, 4104n].includes(address));
      const bytes = Buffer.alloc(8); bytes.writeBigInt64LE(address === 4096n ? 7n : 11n); return bytes;
    },
  };
  const handle = 0x4000000000000000n;
  assert.match((await decode(fn, handle, memory, { count: 0 })).text, /counter \[2\]/);
  assert.equal(reads, 0);
  const page = await decode(fn, handle, memory, { start: 1, count: 1 });
  assert.deepEqual(page.children.map(item => [item.name, item.value, item.readOnly]), [['copied', '11', false]]);
  assert.equal((await decode(fn, handle, memory)).children[0].readOnly, true);
  assert.equal((await decode(fn, 24n, memory)).text, '<函数 counter>');
  assert.equal((await decode(fn, 0n, memory)).text, '<空函数值>');
  assert.equal((await decode(fn, 100n, memory)).text, '<未知函数值>');
  info.captures.pop(); await assert.rejects(decode(fn, handle, memory), /数量/);
  valid = false; await assert.rejects(decode(fn, handle, memory), /失效/);
  console.log('闭包解码：逻辑函数标签、来源／稳定单元、捕获方式、分页、空／未知值及失效拒绝通过');
}
main().catch(error => { console.error(error); process.exitCode = 1; });
