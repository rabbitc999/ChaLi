'use strict';

/** 基础变量编码必须覆盖完整真实槽，拒绝窄类型越界、镜像、只读及拥有值。 */
const assert = require('node:assert/strict');
const { encodeAssignment } = require('../src/debugAssignment');
const { PassThrough } = require('node:stream');
const { Adapter } = require('../src/nativeDebugAdapter');
const layouts = { int8: ['i', 1], int16: ['i', 2], int32: ['i', 4], int64: ['i', 8], uint8: ['u', 1], uint16: ['u', 2], uint32: ['u', 4], uint64: ['u', 8], float32: ['f', 8], float64: ['f', 8], bool: ['b', 1], char: ['u', 1] };
function encode(type, text, changes = {}, layoutChanges = {}) {
  const [kind, width] = layouts[type] || ['s', 8];
  return encodeAssignment({ type, readOnly: false, nativeStorage: type.startsWith('float') ? 'double' : 'i64', ...changes }, { kind, width, ...layoutChanges }, text);
}
async function main() {
  for (const type of ['int8', 'int16', 'int32', 'int64']) {
    const negative = await encode(type, '-7'); assert.equal(negative.bytes.readBigInt64LE(), -7n); assert.equal(negative.bytes.length, 8);
    assert.equal((await encode(type, '3')).bytes.readBigUInt64LE(), 3n);
  }
  assert.equal((await encode('uint64', '18446744073709551615')).bytes.readBigUInt64LE(), (1n << 64n) - 1n);
  assert.equal((await encode('float32', '0.1')).bytes.readDoubleLE(), Math.fround(0.1));
  assert.equal((await encode('float64', '3/2.0')).bytes.readDoubleLE(), 1.5);
  assert.equal((await encode('bool', 'true && !false')).bytes.readBigUInt64LE(), 1n);
  for (const [text, expected] of [["'A'", 65n], ["'\\n'", 10n], ["'\\xFF'", 255n], ['"Z"', 90n], ["'\\u00E9'", 233n]]) assert.equal((await encode('char', text)).bytes.readBigUInt64LE(), expected);
  for (const [type, text, changes, layout] of [['int8', '128'], ['int8', '-129'], ['uint8', '-1'], ['uint16', '65536'], ['uint32', '4294967296'], ['int64', '9223372036854775808'], ['uint64', '18446744073709551616'], ['bool', '1'], ['int32', '1.5'], ['float64', 'true'], ['float32', '1e39'], ['float64', '18446744073709551615'], ['char', "'世界'"], ['char', "'\\u0100'"], ['int64', 'function()'], ['int64', 'other+1'], ['int64', '1', { readOnly: true }], ['int64', '1', { storage: '__box' }], ['int64', '1', { boxed: true }], ['int64', '1', { address: true }], ['int64', '1', { nativeStorage: '' }], ['int64', '1', {}, { kind: 'u' }], ['String', '1']]) await assert.rejects(encode(type, text, changes, layout));
  await staleRead();
  console.log('变量修改编码：完整 64 位槽、符号扩展、类型范围、f32 舍入、char、只读/镜像/拥有值拒绝通过');
}

/** 写入使之前启动的读取失败，旧聚合引用失效；源码帧和局部作用域仍可重新读取。 */
async function staleRead() {
  const adapter = new Adapter('unused', new PassThrough(), new PassThrough()), messages = [], commands = [];
  adapter.send = value => messages.push(value);
  await adapter.request({ seq: 1, command: 'initialize', arguments: { supportsInvalidatedEvent: true } });
  adapter.state = 'stopped';
  const binding = { name: 'x', dwarf: 'x', type: 'int8', scope: 0, readOnly: false, nativeStorage: 'i64', layout: 0 };
  const frame = { thread: 1, level: 0, metadata: { position: { scope: 0 }, function: { parents: new Map([[0, 0]]), variables: [binding] } } };
  adapter.frames.set(10, frame); adapter.handles.set(20, frame); adapter.handles.set(21, { kind: 'value' });
  adapter.metadata = { cache: { catalog: { layouts: ['i0:0:0:1:0:0:'] } }, cancel() {} };
  let release; const pending = new Promise(resolve => { release = resolve; }); let listings = 0;
  adapter.gdb = { async command(command) {
    commands.push(command);
    if (command.startsWith('-stack-list-variables')) {
      if (++listings === 1) return pending;
      return { variables: [{ name: 'x', type: 'int8', value: '3' }] };
    }
    if (command.startsWith('-data-evaluate-expression')) {
      assert(command.endsWith(JSON.stringify('(unsigned long long)&(x)'))); return { value: '4096' };
    }
    assert.equal(command, '-data-write-memory-bytes 0x1000 0300000000000000'); return {};
  } };
  const reading = adapter.request({ seq: 2, command: 'variables', arguments: { variablesReference: 20 } });
  await adapter.request({ seq: 3, command: 'setVariable', arguments: { variablesReference: 20, name: 'x', value: '3' } });
  assert.equal(messages.find(value => value.request_seq === 3).success, true);
  release({ variables: [{ name: 'x', type: 'int8', value: '-7' }] }); await reading;
  assert.equal(messages.find(value => value.request_seq === 2).success, false);
  assert(!adapter.handles.has(21)); assert(adapter.handles.has(20)); assert(adapter.frames.has(10));
  await adapter.request({ seq: 4, command: 'evaluate', arguments: { frameId: 10, expression: 'x' } });
  assert.equal(messages.find(value => value.request_seq === 4).body.result, '3');
  const before = commands.length;
  await adapter.request({ seq: 5, command: 'setVariable', arguments: { variablesReference: 20, name: 'x', value: 'call()' } });
  assert.equal(messages.find(value => value.request_seq === 5).success, false); assert.equal(commands.length, before);
}
main().catch(error => { console.error(error); process.exitCode = 1; });
