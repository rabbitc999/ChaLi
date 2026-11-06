'use strict';

/** 真实 GDB 在工作线程单步，再选主线程越过等待，核对线程退出和旧帧拒绝。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { spawn, spawnSync } = require('node:child_process');
const { DebugClient } = require('./debugClient');
const { interruptWindows } = require('../src/debugInterrupt');
const { stopProcessTree } = require('../src/debugProtocol');
const root = path.resolve(__dirname, '../..'), compiler = process.env.CHALI_BIN;
assert(compiler);
const work = fs.mkdtempSync(path.join(root, 'tmp/debug-threads-'));
const source = path.join(work, 'main.chl');
const lines = [
  'import std::thread;',
  'int64 helper(int64 value){',
  ' println("HELPER");',
  ' return value+1;',
  '}',
  'void worker(){',
  ' int64 marker=20;',
  ' marker=helper(marker);',
  ' println("WORKER FINISH {}",marker);',
  '}',
  'void main(){',
  ' Thread task=std::thread::create(worker);',
  ' std::thread::wait(task);',
  ' println("MAIN AFTER WAIT");',
  '}',
];
fs.writeFileSync(source, lines.join('\n'));

async function stack(client, threadId) {
  return (await client.request('stackTrace', { threadId })).stackFrames;
}

/** 长耗时单步期间的显式暂停不能被内部帧过滤重新恢复。 */
async function pauseStep(allocator) {
  const file = path.join(work, 'pause.chl'), prefix = path.join(work, 'pause-' + allocator);
  fs.writeFileSync(file, 'import std::thread;\nvoid main(){\n std::thread::sleepFor(5000);\n println("AFTER PAUSE");\n}\n');
  const built = spawnSync(compiler, ['build', file, '--debug', '--checked', '--allocator', allocator, '--out', prefix],
    { cwd: root, env: process.env, encoding: 'utf8', windowsHide: true, timeout: 180000 });
  assert.equal(built.status, 0, built.stdout + built.stderr);
  const client = new DebugClient(root, compiler);
  try {
    await client.request('initialize', { adapterID: 'chali' });
    await client.request('launch', { executable: prefix + (process.platform === 'win32' ? '.exe' : ''), cwd: root }); await client.event('initialized');
    await client.request('setBreakpoints', { source: { path: file }, breakpoints: [3, 4].map(line => ({ line })) });
    await client.request('configurationDone'); const first = await client.event('stopped');
    await client.request('next', { threadId: first.threadId });
    await client.request('pause', { threadId: first.threadId });
    const paused = await client.event('stopped'); assert.equal(paused.reason, 'pause'); assert.equal(paused.allThreadsStopped, true);
    const before = (await stack(client, paused.threadId))[0].instructionPointerReference;
    await new Promise(resolve => setTimeout(resolve, 50));
    assert.equal((await stack(client, paused.threadId))[0].instructionPointerReference, before);
    await client.request('continue', { threadId: first.threadId });
    const after = await client.event('stopped'); assert.equal(after.reason, 'breakpoint');
    assert.equal((await stack(client, after.threadId))[0].line, 4);
    await client.request('continue', { threadId: after.threadId }); assert.equal((await client.event('exited')).exitCode, 0); await client.event('terminated');
  } finally { await client.close(); }
}

async function main() {
  if (process.platform === 'win32') {
    // 映像不匹配时，辅助路径必须拒绝中断并保持普通子进程运行。
    const child = spawn(process.execPath, ['-e', 'setInterval(() => {}, 1000)'], { windowsHide: true });
    try {
      const temporary = path.join(root, 'tmp');
      await assert.rejects(interruptWindows(child.pid, process.pid, path.join(work, 'wrong-image.exe'),
        { ...process.env, TMPDIR: temporary, TMP: temporary, TEMP: temporary }), /映像身份/);
      assert.equal(child.exitCode, null); assert.equal(child.signalCode, null);
    } finally { await stopProcessTree(child); }
  }
  for (const allocator of ['malloc', 'pool']) {
    const prefix = path.join(work, allocator);
    const built = spawnSync(compiler, ['build', source, '--debug', '--checked', '--allocator', allocator, '--out', prefix],
      { cwd: root, env: process.env, encoding: 'utf8', windowsHide: true, timeout: 180000 });
    assert.equal(built.status, 0, built.stdout + built.stderr);
    const client = new DebugClient(root, compiler);
    try {
      await client.request('initialize', { adapterID: 'chali', supportsInvalidatedEvent: true });
      await client.request('launch', { executable: prefix + (process.platform === 'win32' ? '.exe' : ''), cwd: root }); await client.event('initialized');
      const installed = await client.request('setBreakpoints', { source: { path: source }, breakpoints: [8, 14].map(line => ({ line })) });
      assert(installed.breakpoints.every(point => point.verified)); await client.request('configurationDone');
      let stop = await client.event('stopped'); const worker = stop.threadId;
      assert.equal(stop.allThreadsStopped, true); assert.match((await stack(client, worker))[0].name, /(?:^|::)worker$/);
      const threads = (await client.request('threads')).threads;
      let parent;
      for (const thread of threads) {
        if (thread.id !== worker && (await stack(client, thread.id)).some(frame => /(?:^|::)main$/.test(frame.name))) { parent = thread.id; break; }
      }
      assert(parent && parent !== worker, JSON.stringify(threads));
      await assert.rejects(client.request('next', { threadId: '1\n-exec-run' }), /线程/);
      await assert.rejects(client.request('next', { threadId: worker, singleThread: true }), /全停/);
      await client.request('stepIn', { threadId: worker }); stop = await client.event('stopped');
      assert.equal(stop.threadId, worker); assert.equal(stop.allThreadsStopped, true); assert.match((await stack(client, worker))[0].name, /(?:^|::)helper$/);
      await client.request('next', { threadId: worker }); stop = await client.event('stopped');
      assert.equal(stop.threadId, worker); assert.match((await stack(client, worker))[0].name, /(?:^|::)helper$/);
      await client.request('stepOut', { threadId: worker }); stop = await client.event('stopped');
      assert.equal(stop.threadId, worker); const frames = await stack(client, worker); assert.match(frames[0].name, /(?:^|::)worker$/);
      const scopes = await client.request('scopes', { frameId: frames[0].id });
      const oldScope = scopes.scopes[0].variablesReference;
      // GDB 当前选择工作线程；明确选择主线程单步，工作线程退出后主线程返回源码。
      await client.request('next', { threadId: parent }); stop = await client.event('stopped');
      assert.equal(stop.threadId, parent); assert.equal(stop.allThreadsStopped, true);
      const returned = await stack(client, parent);
      assert.match(returned[0].name, /(?:^|::)main$/, JSON.stringify({ stop, returned }));
      // Windows 的原生 finish 可先返回调用指令所在行，再越过该行到下一条源码停点。
      if (returned[0].line === 13) {
        await client.request('next', { threadId: parent }); stop = await client.event('stopped');
        assert.equal(stop.threadId, parent);
      }
      assert.equal((await stack(client, parent))[0].line, 14);
      const exited = client.messages.filter(message => message.event === 'thread' && message.body.reason === 'exited' && message.body.threadId === worker);
      assert.equal(exited.length, 1);
      assert.equal(client.messages.filter(message => message.event === 'thread' && message.body.reason === 'started' && message.body.threadId === worker).length, 1);
      assert(!(await client.request('threads')).threads.some(thread => thread.id === worker));
      await assert.rejects(client.request('stackTrace', { threadId: worker }), /退出|不存在/);
      await assert.rejects(client.request('next', { threadId: worker }), /退出|不存在/);
      await assert.rejects(client.request('scopes', { frameId: frames[0].id }), /过期/);
      await assert.rejects(client.request('variables', { variablesReference: oldScope }), /过期/);
      await client.request('continue', { threadId: parent }); assert.equal((await client.event('exited')).exitCode, 0); await client.event('terminated');
    } finally { await client.close(); }
    await pauseStep(allocator);
  }
  console.log('全停线程真实 GDB：malloc/pool、线程事件、所选线程三种单步、退出/过期拒绝、长单步暂停与映像核对通过');
}
main().catch(error => { console.error(error); process.exitCode = 1; });
