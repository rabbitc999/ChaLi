'use strict';

/** 真实编译进程取消和输入断开验收，不依赖调试器界面的时序。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { DebugClient } = require('./debugClient');
const root = path.resolve(__dirname, '../..');
const compiler = process.env.CHALI_BIN || path.join(root, 'chali.exe');
const work = fs.mkdtempSync(path.join(root, 'tmp/debug-lifecycle-'));
const source = path.join(work, 'main.chl');
fs.writeFileSync(source, 'void main(){int64 x=1;println("{}",x);}\n');

/** 等待适配器自行退出；超时由 close 回收，不把残留进程计为通过。 */
function exited(child) {
  return new Promise((resolve, reject) => {
    if (child.exitCode !== null || child.signalCode !== null) return resolve();
    const timer = setTimeout(() => reject(new Error('输入结束后适配器没有退出')), 10000);
    child.once('exit', () => { clearTimeout(timer); resolve(); });
  });
}

async function main() {
  for (const mode of ['request', 'progress', 'disconnect', 'eof', 'restart']) {
    const client = new DebugClient(root, compiler);
    try {
      await client.request('initialize', { adapterID: 'chali', supportsProgressReporting: true });
      const launching = client.request('launch', { program: source, cwd: root, stopOnEntry: true });
      const rejected = assert.rejects(launching);
      const progress = await client.event('progressStart');
      assert.equal(progress.requestId, launching.requestId);
      if (mode === 'request') await client.request('cancel', { requestId: launching.requestId });
      if (mode === 'progress') await client.request('cancel', { progressId: progress.progressId });
      if (mode === 'disconnect') await client.request('disconnect');
      if (mode === 'eof') client.child.stdin.end();
      if (mode === 'restart') {
        await client.request('restart');
        const stopped = await client.event('stopped');
        assert.equal(stopped.reason, 'breakpoint');
        await client.request('continue', { threadId: stopped.threadId });
      }
      await rejected;
      await client.event('terminated');
      if (mode === 'eof') await exited(client.child);
      assert.equal(client.messages.filter((message) => message.event === 'terminated').length, 1);
      if (mode !== 'eof') assert(client.messages.some((message) => message.event === 'progressEnd'));
    } finally { await client.close(); }
  }
  const running = new DebugClient(root, compiler);
  try {
    await running.request('initialize', { adapterID: 'chali' });
    await running.request('launch', { program: source, cwd: root, stopOnEntry: true });
    await running.event('initialized');
    await running.request('configurationDone');
    await running.event('stopped');
    await running.request('terminate');
    await running.event('terminated');
    await assert.rejects(running.request('threads'), /结束/);
    assert.equal(running.messages.filter((message) => message.event === 'terminated').length, 1);
  } finally { await running.close(); }
  console.log('DAP 生命周期：请求／进度取消、构建中断开／重启、EOF 和 GDB 终止回收通过');
}
main().catch((error) => { console.error(error); process.exitCode = 1; });
