'use strict';

/** 优化构建依赖 DWARF 可见值，拒绝歧义布局、不可用值和固定帧槽写入。 */
const assert = require('node:assert/strict');
const { PassThrough } = require('node:stream');
const { Adapter } = require('../src/nativeDebugAdapter');
const { visibleBindings } = require('../src/debugMetadata');

async function main() {
  const variables = [
    { name: 'live', type: 'int64', layout: 0, nativeStorage: 'i64', readOnly: false },
    { name: 'gone', type: 'int64', layout: 0, nativeStorage: 'i64', readOnly: false },
    { name: 'shadow', scope: 0 }, { name: 'shadow', scope: 2 },
    { name: 'record', scope: 0 }, { name: 'record', scope: 2 },
    { name: 'privateMacro', macro: 1 },
  ];
  const metadata = { optimized: true, function: { variables } };
  assert.deepEqual([...visibleBindings(metadata).keys()], ['live', 'gone']);
  const adapter = new Adapter('unused', new PassThrough(), new PassThrough());
  const messages = [], commands = [];
  adapter.send = value => messages.push(value); adapter.state = 'stopped';
  const frame = { thread: 1, level: 0, metadata };
  adapter.frames.set(10, frame); adapter.handles.set(20, frame);
  adapter.metadata = { cache: { catalog: { optimized: true, layouts: ['i0:0:0:8:0:0:'] } }, cancel() {} };
  adapter.gdb = { command: async command => {
    commands.push(command); assert(command.startsWith('-stack-list-variables'));
    return { variables: [{ name: 'live', type: 'int64', value: '7' }, { name: 'gone', type: 'int64', value: '<optimized out>' },
      { name: 'shadow', type: 'int64', value: '11' }, { name: 'record', type: 'Row', value: '4096' }] };
  } };
  await adapter.request({ seq: 1, command: 'variables', arguments: { variablesReference: 20 } });
  const rows = messages.find(value => value.request_seq === 1).body.variables;
  assert.equal(rows[0].value, '7'); assert.match(rows[1].value, /已优化/);
  assert.match(rows[2].value, /布局不可用/); assert.match(rows[3].value, /布局不可用/);
  assert(rows.every(row => row.presentationHint.attributes.includes('readOnly')));
  assert(rows.every(row => row.variablesReference === 0 && !row.memoryReference));
  await adapter.request({ seq: 2, command: 'evaluate', arguments: { frameId: 10, expression: 'live+1' } });
  assert.equal(messages.find(value => value.request_seq === 2).body.result, '8');
  await adapter.request({ seq: 3, command: 'evaluate', arguments: { frameId: 10, expression: 'gone' } });
  assert.match(messages.find(value => value.request_seq === 3).message, /已被优化/);
  const before = commands.length;
  await adapter.request({ seq: 4, command: 'setVariable', arguments: { variablesReference: 20, name: 'live', value: '9' } });
  assert.match(messages.find(value => value.request_seq === 4).message, /优化调试不修改/);
  assert.equal(commands.length, before);
  await adapter.request({ seq: 5, command: 'scopes', arguments: { frameId: 10 } });
  assert.match(messages.find(value => value.request_seq === 5).body.scopes[0].name, /O3/);
  console.log('优化调试：DWARF 可读值、已优化状态、歧义绑定、布局拒绝及只读作用域通过');
}
main().catch(error => { console.error(error); process.exitCode = 1; });
