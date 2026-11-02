'use strict';

/** 容器快照按真实值节点解码，不允许将宿主节点当作逻辑内存地址。 */
const assert = require('node:assert/strict');
const { decode } = require('../src/debugValue');
const integer = { kind: 'i', width: 8, name: 'int64' };
const text = { kind: 's', width: 8, name: 'String' };
const number = bits => ({ flags: 0, bits: BigInt.asUintN(64, BigInt(bits)), count: 0 });
const records = new Map();
let expired = false;
const memory = {
  async read() { assert.fail('快照值不能通过逻辑内存读取'); },
  async validateObjects() { if (expired) throw new Error('对象快照已经失效'); },
  async object(handle) { await this.validateObjects(); const object = records.get(handle); if (!object) throw new Error('句柄已释放'); return object; },
};

async function main() {
  const list = { kind: 'L', children: [integer] };
  records.set(100n, { kind: 'L', first: 0, count: 3, values: [number(-1), number(42), number(99)] });
  const page = await decode(list, 100n, memory, { start: 1, count: 1 });
  assert.equal(page.text, 'List[3]');
  assert.deepEqual(page.children.map(item => [item.name, item.value]), [['1', '42']]);
  assert.equal(page.children[0].address, undefined);
  const pair = { kind: 'S', name: 'Pair', children: [{ ...integer, name: 'first', offset: 0 }, { kind: 'b', width: 1, name: 'flag', offset: 8 }] };
  const map = { kind: 'M', children: [text, pair] };
  records.set(200n, { kind: 'M', first: 0, count: 1, values: [
    { flags: 2, text: Buffer.from('世界'), bits: 0n },
    { flags: 1, first: 2, count: 2 }, number(7), number(1),
  ] });
  const entry = (await decode(map, 200n, memory)).children[0];
  const fields = (await decode(entry.childLayout, undefined, memory, {}, entry.stored)).children;
  assert.equal(fields[0].value, '"世界"');
  assert.equal(fields[1].type, 'Pair');
  const nested = await decode(fields[1].childLayout, undefined, memory, {}, fields[1].stored);
  assert.deepEqual(nested.children.map(item => [item.name, item.value]), [['first', '7'], ['flag', 'true']]);
  records.set(300n, { kind: 'E', first: 0, count: 1, values: [number(-1)] });
  assert.equal((await decode({ kind: 'E', children: [integer] }, 300n, memory)).children[0].value, '-1');
  const raw = Buffer.alloc(8); raw.writeDoubleLE(1.25);
  records.set(400n, { kind: 'L', first: 0, count: 1, values: [{ flags: 4, bits: raw.readBigUInt64LE() }] });
  assert.equal((await decode({ kind: 'L', children: [{ kind: 'f', width: 8 }] }, 400n, memory)).children[0].value, '1.25');
  records.set(500n, { kind: 'L', first: 0, count: 1, values: [number(100)] });
  const child = (await decode({ kind: 'L', children: [list] }, 500n, memory)).children[0];
  assert.equal(child.value, 'List[3]');
  assert.equal((await decode(child.childLayout, undefined, memory, { count: 1 }, child.stored)).children[0].value, '-1');
  await assert.rejects(decode({ kind: 'E', children: [integer] }, 100n, memory), /不匹配/);
  await assert.rejects(decode(list, 999n, memory), /释放/);
  const box = { kind: 'S', name: 'Box', children: [{ ...pair, name: 'pair', width: 16, offset: 0 }, { ...integer, name: 'tail', offset: 16 }] };
  records.set(600n, { kind: 'L', first: 0, count: 1, values: [
    { flags: 1, first: 1, count: 3 }, number(7), number(1), number(99),
  ] });
  const boxed = (await decode({ kind: 'L', children: [box] }, 600n, memory)).children[0];
  const boxFields = (await decode(boxed.childLayout, undefined, memory, {}, boxed.stored)).children;
  assert.equal(boxFields[1].value, '99');
  const projected = await decode(boxFields[0].childLayout, undefined, memory, {}, boxFields[0].stored);
  assert.deepEqual(projected.children.map(item => item.value), ['7', 'true']);
  expired = true;
  await assert.rejects(decode(entry.childLayout, undefined, memory, {}, entry.stored), /失效/);
  console.log('容器解码：分页、Map 键值、记录字段、字符串、浮点、嵌套及失效拒绝通过');
}
main().catch(error => { console.error(error); process.exitCode = 1; });
