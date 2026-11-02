'use strict';

/** 核对真实存储编码、单次写入、镜像显示一致性、调用方拒绝及旧目录保护。 */
const assert = require('node:assert/strict');
const { PassThrough } = require('node:stream');
const { Adapter } = require('../src/nativeDebugAdapter');
const { encodeAssignment } = require('../src/debugAssignment');
const { parseLayout } = require('../src/debugValue');

async function main() {
  const binding = { name: 'number', type: 'int64', scope: 0, layout: 0, readOnly: false,
    storage: '__box', boxed: true, nativeStorage: 'i64', storageDwarf: '__chali_storage_1', storageBytes: 8 };
  const scalar = parseLayout('i0:0:0:8:0:0:');
  assert.equal((await encodeAssignment(binding, scalar, '-9')).bytes.toString('hex'), 'f7ffffffffffffff');
  assert.equal((await encodeAssignment({ ...binding, type: 'uint8', storageBytes: 1 }, parseLayout('u0:0:0:1:0:0:'), '230')).bytes.toString('hex'), 'e6');
  for (const change of [{ storageDwarf: '' }, { storageDwarf: 'user()' }, { storageBytes: 1 }, { readOnly: true }])
    await assert.rejects(encodeAssignment({ ...binding, ...change }, scalar, '9'));
  const adapter = new Adapter('unused', new PassThrough(), new PassThrough());
  const messages = [], commands = []; adapter.send = value => messages.push(value); adapter.state = 'stopped';
  const frame = { thread: 1, level: 0, metadata: { position: { scope: 0 }, function: { parents: new Map([[0, 0]]), variables: [binding] } } };
  adapter.frames.set(10, frame); adapter.handles.set(20, frame);
  adapter.metadata = { cache: { catalog: { layouts: ['i0:0:0:8:0:0:'] } }, cancel() {} };
  const actual = Buffer.alloc(8); actual.writeBigInt64LE(5n);
  adapter.memoryFor = async () => ({
    read: async (address, length) => { assert.equal(address, 123n); assert.equal(length, 8); return Buffer.from(actual); },
    address: async (address, length) => { assert.equal(address, 123n); assert.equal(length, 8); return 4096n; },
  });
  adapter.gdb = { command: async command => {
    commands.push(command);
    if (command.startsWith('-stack-list-variables')) return { variables: [
      { name: 'number', type: 'int64', value: '5' }, { name: '__chali_storage_1', type: 'uint64', value: '123' },
    ] };
    if (command.startsWith('-data-evaluate-expression')) {
      assert(command.endsWith(JSON.stringify('(unsigned long long)(__chali_storage_1)'))); return { value: '123' };
    }
    assert.equal(command, '-data-write-memory-bytes 0x1000 0900000000000000'); actual.writeBigInt64LE(9n); return {};
  } };
  await adapter.request({ seq: 1, command: 'setVariable', arguments: { variablesReference: 20, name: 'number', value: '9' } });
  assert.equal(messages.find(value => value.request_seq === 1).success, true);
  assert.equal(commands.filter(command => command.startsWith('-data-write')).length, 1);
  await adapter.request({ seq: 2, command: 'variables', arguments: { variablesReference: 20 } });
  const variables = messages.find(value => value.request_seq === 2).body.variables;
  assert.equal(variables.length, 1); assert.equal(variables[0].value, '9'); assert(!variables[0].presentationHint);
  await adapter.request({ seq: 3, command: 'evaluate', arguments: { frameId: 10, expression: 'number+1' } });
  assert.equal(messages.find(value => value.request_seq === 3).body.result, '10');
  frame.level = 1;
  const before = commands.length;
  await adapter.request({ seq: 4, command: 'setVariable', arguments: { variablesReference: 20, name: 'number', value: '10' } });
  assert.equal(messages.find(value => value.request_seq === 4).success, false); assert.equal(commands.length, before);
  assert.match(messages.find(value => value.request_seq === 4).message, /栈顶/);
  frame.level = 0;
  const legacy = { ...binding, storageDwarf: '', storageBytes: 0 };
  frame.metadata.function.variables = [legacy];
  const legacyBefore = commands.length;
  await adapter.request({ seq: 6, command: 'setVariable', arguments: { variablesReference: 20, name: 'number', value: '10' } });
  assert.equal(messages.find(value => value.request_seq === 6).success, false); assert.equal(commands.length, legacyBefore);
  const many = Array.from({ length: 300 }, (_, index) => ({ ...binding, name: 'number' + index, storageDwarf: '__chali_storage_' + index }));
  frame.metadata.function.variables = many;
  let active = 0, peak = 0;
  adapter.gdb.command = async command => {
    if (command.startsWith('-stack-list-variables')) return { variables: many.map(item => ({ name: item.name, type: 'int64', value: '5' })) };
    assert(command.startsWith('-data-evaluate-expression'));
    active++; peak = Math.max(peak, active);
    try { assert(active <= 32, '基础镜像查询不得耗尽 GDB 并发请求预算'); await new Promise(resolve => setTimeout(resolve, 1)); }
    finally { active--; }
    return { value: '123' };
  };
  await adapter.request({ seq: 5, command: 'variables', arguments: { variablesReference: 20 } });
  const manyValues = messages.find(value => value.request_seq === 5);
  assert(manyValues.success); assert.equal(manyValues.body.variables.length, 300); assert(manyValues.body.variables.every(item => item.value === '9'));
  assert(peak > 1 && peak <= 32);
  console.log('基础镜像修改：完整类型编码、单次真实写入、隐藏地址、同暂停真实值显示与调用方/旧描述拒绝通过');
}
main().catch(error => { console.error(error); process.exitCode = 1; });
