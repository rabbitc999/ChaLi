'use strict';

/** 线程目录的事件、迟到列表、退出读取与所选线程单步，独立于目标调度时序。 */
const assert = require('node:assert/strict');
const { PassThrough } = require('node:stream');
const { DebugThreads } = require('../src/debugThreads');
const { Adapter } = require('../src/nativeDebugAdapter');

function directory() {
  const events = [], threads = new DebugThreads((reason, id) => events.push([reason, id]));
  threads.start('1'); threads.start('2'); threads.start('2');
  const generation = threads.generation;
  threads.exit('2'); threads.exit('2');
  assert.deepEqual(threads.sync([{ id: '1', name: 'main' }, { id: '2', name: 'stale' }], '2', generation).map(row => row.id), [1]);
  assert.throws(() => threads.resolve(2), /退出/);
  const older = threads.generation; threads.start('3');
  assert.deepEqual(threads.sync([{ id: '1' }], '1', older).map(row => row.id), [1, 3]);
  assert.equal(threads.stop('3'), 3); assert.equal(threads.stop('2'), 3); assert(threads.dead.has(2));
  assert.equal(threads.resolve(), 3);
  assert.deepEqual(events, [['started', 1], ['started', 2], ['exited', 2], ['started', 3]]);
  for (const id of [0, -1, 1.5, '1\n-exec-run', '01', 'all', Number.MAX_SAFE_INTEGER + 1]) assert.throws(() => DebugThreads.id(id));
  assert.throws(() => threads.resolve('1'), /整数/);
  assert.throws(() => threads.sync([{ id: '1' }, { id: '1' }], undefined, threads.generation), /重复/);
}

function adapter() {
  const value = new Adapter('unused', new PassThrough(), new PassThrough());
  const messages = []; value.send = item => messages.push(item); value.state = 'stopped';
  value.threads.start(1); value.threads.start(2); return { value, messages };
}

async function stepping() {
  const { value, messages } = adapter(), commands = [];
  value.gdb = { command: async command => { commands.push(command); return {}; } };
  for (const [index, command] of ['next', 'stepIn', 'stepOut'].entries()) {
    value.state = 'stopped';
    await value.request({ seq: index + 1, command, arguments: { threadId: 2 } });
    assert.equal(messages.find(message => message.request_seq === index + 1).success, true);
  }
  assert.deepEqual(commands, ['-exec-next --thread 2', '-exec-step --thread 2', '-exec-finish --thread 2']);
  value.state = 'stopped'; value.threads.exit(2);
  for (const [index, args] of [{ threadId: 2 }, { threadId: '1\n-exec-run' }, { threadId: 1, singleThread: true }].entries()) {
    await value.request({ seq: index + 4, command: 'next', arguments: args });
    assert.equal(messages.find(message => message.request_seq === index + 4).success, false);
  }
  assert.equal(commands.length, 3);
}

async function exits() {
  const { value, messages } = adapter(); let release;
  value.handles.set(20, { thread: 2, level: 0 }); value.frames.set(10, { thread: 2 });
  value.gdb = { command: async () => new Promise(resolve => { release = resolve; }) };
  const reading = value.request({ seq: 1, command: 'variables', arguments: { variablesReference: 20 } });
  value.threads.exit(2);
  release({ variables: [{ name: 'x', type: 'int64', value: '7' }] }); await reading;
  assert.equal(messages.find(message => message.request_seq === 1).success, false);
  assert.equal(value.frames.size, 0); assert.equal(value.handles.size, 0);
}

async function exceptions() {
  for (const exited of [1, 2]) {
    const { value, messages } = adapter(); let release;
    value.gdb = { command: async command => command === '-thread-info' ? { threads: [{ id: String(exited === 1 ? 2 : 1) }] }
      : new Promise(resolve => { release = resolve; }) };
    const pending = value.reportSignal({ 'thread-id': '1', 'signal-name': 'SIGILL' });
    value.threads.exit(exited); release({ stack: [{ frame: { fullname: 'current.chl', line: '7' } }] }); await pending;
    const stop = messages.find(message => message.event === 'stopped');
    assert.equal(stop.body.threadId, exited === 1 ? 2 : 1);
    assert.equal(stop.body.allThreadsStopped, true);
    if (exited === 2) assert.match(stop.body.text, /current.chl:7/);
  }
}

/** 拒绝无效中断身份，不生成辅助子进程。 */
async function interruptIdentity() {
  const { interruptWindows } = require('../src/debugInterrupt');
  for (const target of [null, 0, -1, '1', Number.MAX_SAFE_INTEGER + 1]) await assert.rejects(interruptWindows(target, 1, 'program.exe', process.env), /身份/);
}

async function main() {
  directory(); await stepping(); await exits(); await exceptions(); await interruptIdentity();
  console.log('全停线程：事件去重、退出墓碑、迟到列表/读取、所选线程单步、退出后异常停点恢复通过');
}
main().catch(error => { console.error(error); process.exitCode = 1; });
