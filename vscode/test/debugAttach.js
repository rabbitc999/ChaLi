'use strict';

/** 附加关闭失败必须保留会话供重试，保留目标的关闭路径不得结束调试器进程树。 */
const assert = require('node:assert/strict');
const { PassThrough } = require('node:stream');
const protocol = require('../src/debugProtocol');
const originalStop = protocol.stopProcessTree;
const killed = [];
protocol.stopProcessTree = async child => { if (child) killed.push(child); };
const { Adapter } = require('../src/nativeDebugAdapter');

function fixture() {
  const adapter = new Adapter('unused', new PassThrough(), new PassThrough());
  adapter.attached = true; adapter.identityVerified = true; adapter.state = 'stopped';
  adapter.inferiorPid = 123; adapter.attachCommandSent = true;
  adapter.threads.start(1);
  const commands = [], process = { exitCode: 0, signalCode: null };
  const gdb = { process, fail() {}, command: async command => { commands.push(command); return {}; } };
  adapter.gdb = gdb;
  return { adapter, commands, gdb, process };
}

async function main() {
  const kept = fixture();
  await kept.adapter.shutdown();
  assert.deepEqual(kept.commands, ['-target-detach', '-gdb-exit']);
  assert.equal(kept.adapter.gdb, null); assert(!killed.includes(kept.process));

  const failed = fixture(); let rejectDetach = true;
  failed.gdb.command = async command => {
    failed.commands.push(command);
    if (command === '-target-detach' && rejectDetach) throw new Error('模拟断开失败');
    return {};
  };
  await assert.rejects(failed.adapter.shutdown({ keepTarget: true }), /模拟断开失败/);
  assert.equal(failed.adapter.gdb, failed.gdb); assert.equal(failed.adapter.state, 'stopped');
  assert.equal(failed.adapter.shutdownPromise, null);
  assert.deepEqual(failed.commands, ['-target-detach']); assert(!killed.includes(failed.process));
  rejectDetach = false;
  await failed.adapter.shutdown({ keepTarget: true });
  assert.deepEqual(failed.commands, ['-target-detach', '-target-detach', '-gdb-exit']);
  assert(!killed.includes(failed.process));

  const terminated = fixture();
  await terminated.adapter.shutdown({ keepTarget: false });
  assert.deepEqual(terminated.commands, ['-interpreter-exec console "kill"', '-gdb-exit']);
  assert(killed.includes(terminated.process));

  for (const mode of ['terminal', 'sourceBuilt']) {
    const value = fixture(); value.adapter.attached = false;
    value.adapter[mode] = mode === 'terminal' ? {} : true;
    await assert.rejects(value.adapter.shutdown({ keepTarget: true }), /不能.*保留/);
    assert.deepEqual(value.commands, []);
  }

  const running = fixture(); running.adapter.state = 'running';
  let interruptions = 0;
  running.adapter.pendingAttachStop = { stale: true }; running.adapter.attachPending = false;
  running.adapter.stopForClose = async (_gdb, initiate) => { assert.equal(initiate, true); interruptions++; };
  await running.adapter.shutdown({ keepTarget: true });
  assert.equal(interruptions, 1); assert(!killed.includes(running.process));
  const pending = fixture();
  pending.adapter.state = 'building'; pending.adapter.attachPending = true;
  pending.adapter.inferiorPid = null;
  pending.adapter.stopForClose = async (_gdb, initiate) => {
    assert.equal(initiate, false);
    pending.adapter.inferiorPid = 123; // 接管通知迟于取消请求，但必须先解除这次真实接管。
  };
  await pending.adapter.shutdown({ keepTarget: true });
  assert.deepEqual(pending.commands, ['-target-detach', '-gdb-exit']);
  assert(!killed.includes(pending.process));
  console.log('附加关闭、失败重试与目标保留单元通过');
}

main().catch(error => { console.error(error); process.exitCode = 1; })
  .finally(() => { protocol.stopProcessTree = originalStop; });
