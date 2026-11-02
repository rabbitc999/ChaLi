'use strict';

/** 校验版本化布局的解析、标量／记录／数组／字符串／引用解码、分页与损坏拒绝。 */
const assert = require('node:assert/strict');
const { parseLayout, decode, expandable } = require('../src/debugValue');

/** 按布局格式编码节点，测试与编译器使用同一编码。 */
function shape(kind, name = '', children = [], extra = {}) {
  const value = { kind, name, offset: extra.offset || 0, count: extra.count || 0, width: extra.width || 8, children };
  const number = (item) => String(item) + ':';
  let out = kind + number(Buffer.byteLength(name)) + name + number(value.offset) + number(value.count) + number(value.width) + '0:' + number(children.length);
  for (const child of children) out += shape(child.kind, child.name, child.children || [], child);
  return out;
}

/** 构造只读内存快照：堆块从 0x100 开始，静态数据区到 0x100。 */
function memory(blocks) {
  const ranges = blocks.map(([address, bytes]) => ({ address: BigInt(address), bytes }));
  return {
    dataSize: 0x100n,
    async read(address, size) {
      const range = ranges.find((item) => address >= item.address && address + BigInt(size) <= item.address + BigInt(item.bytes.length));
      if (!range) throw new Error('越界读取');
      return range.bytes.subarray(Number(address - range.address), Number(address - range.address) + size);
    },
  };
}

async function main() {
  const int32 = { kind: 'i', name: 'int32', width: 4 };
  const parsed = parseLayout(shape('S', 'Pair', [
    { kind: 'i', name: 'first', width: 8, offset: 0 },
    { kind: 'u', name: 'tag', width: 1, offset: 8 },
  ]));
  assert.equal(parsed.kind, 'S');
  assert.equal(parsed.children.length, 2);

  const pair = Buffer.alloc(24);
  pair.writeBigInt64LE(-5n, 0);
  pair.writeUInt8(200, 8);
  const decoded = await decode(parsed, 0x100n, memory([[0x100, pair]]));
  assert.equal(decoded.text, '{}');
  const children = await decode(parsed, 0x100n, memory([[0x100, pair]]), { count: 2 });
  assert.deepEqual(children.children.map((item) => [item.name, item.value]), [['first', '-5'], ['tag', '200']]);
  assert.equal(children.children[0].childLayout, null);

  // 数组长度头在句柄前 8 字节，元素按宽度读取。
  const array = Buffer.alloc(0x20);
  array.writeBigInt64LE(3n, 0);            // 0x100 长度
  [7n, 8n, 9n].forEach((value, index) => array.writeBigInt64LE(value, 8 + index * 8));
  const arrayLayout = parseLayout(shape('a', 'int64[]', [{ kind: 'i', name: 'int64', width: 8 }], { width: 8 }));
  const arrayView = await decode(arrayLayout, 0x108n, memory([[0x100, array]]));
  assert.equal(arrayView.text, '[3]');
  assert.deepEqual(arrayView.children.map((item) => item.value), ['7', '8', '9']);

  // 字符串：静态数据以 NUL 结尾，堆内联头与带缓冲头都要能读。
  const staticData = Buffer.alloc(0xf0);
  staticData.write('hello\0tail', 0, 'utf8');
  const stringLayout = parseLayout(shape('s', 'String'));
  assert.equal((await decode(stringLayout, 0x10n, memory([[0x10, staticData]]))).text, '"hello"');
  const inline = Buffer.alloc(16);
  inline.writeBigInt64LE(5n, 0);
  inline.write('world', 8, 'utf8');
  assert.equal((await decode(stringLayout, 0x200n, memory([[0x200, inline]]))).text, '"world"');
  const header = Buffer.alloc(32), data = Buffer.from('buffered', 'utf8');
  header.writeBigInt64LE(-1n, 0);
  header.writeBigInt64LE(8n, 8);
  header.writeBigInt64LE(8n, 16);
  header.writeBigUInt64LE(0x300n, 24);
  assert.equal((await decode(stringLayout, 0x200n, memory([[0x200, header], [0x300, data]]))).text, '"buffered"');

  // 顶层引用本身就是指针：解码直接跟随，空指针返回 null。
  const refLayout = parseLayout(shape('R', '引用', [parsed]));
  const refView = await decode(refLayout, 0x100n, memory([[0x100, pair]]));
  assert.equal(refView.text, '&{}');
  assert.deepEqual(refView.children.map((item) => item.name), ['first', 'tag']);
  assert.equal((await decode(refLayout, 0n, memory([]))).text, 'null');
  // 记录里的引用字段先读槽内指针，再提供展开地址。
  const holder = Buffer.alloc(0x10); holder.writeBigUInt64LE(0x100n);
  const holderLayout = parseLayout(shape('S', 'Holder', [{ kind: 'R', name: '引用', offset: 0, width: 8, children: [parsed] }]));
  const holderView = await decode(holderLayout, 0x500n, memory([[0x500, holder], [0x100, pair]]), { count: 1 });
  const link = holderView.children[0];
  assert.equal(link.value, '&<引用>');
  assert.equal(link.address, 0x100n);
  assert.equal((await decode(link.childLayout, link.address, memory([[0x100, pair]]))).text, '&{}');

  // 分页只读请求范围，且标记还有更多。
  const many = Buffer.alloc(0x100);
  many.writeBigInt64LE(10n, 0);
  for (let index = 0; index < 10; ++index) many.writeBigInt64LE(BigInt(index), 8 + index * 8);
  const page = await decode(arrayLayout, 0x108n, memory([[0x100, many]]), { start: 2, count: 3 });
  assert.equal(page.text, '[10…]');
  assert.deepEqual(page.children.map((item) => item.name), ['2', '3', '4']);

  // 损坏布局必须在解析期拒绝。
  for (const text of ['', '?0:', 'i9:', 'i0:0:0:8:0:1', shape('S', 'x', [{ kind: 'i' }]).slice(0, -3), 'z0:0:0:8:0:0:'])
    assert.throws(() => parseLayout(text), /布局/);
  assert.equal(expandable(stringLayout), true);
  assert.equal(expandable(parseLayout(shape('i', 'int64'))), false);

  // 读取失败必须是可报告的子项，而不是整个响应失败。
  const broken = await decode(parsed, 0x500n, memory([]), { count: 2 });
  assert.match(broken.children[0].value, /不可读/);
  console.log('调试布局：解析、标量、记录、数组、字符串、引用、分页与损坏拒绝通过');
}
main().catch((error) => { console.error(error); process.exitCode = 1; });
