'use strict';

/** 在 Linux 通过真实 TCP gdbserver 调试 ChaLi；不以协议桩冒充远程执行。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { spawn, spawnSync } = require('node:child_process');
const { DebugClient } = require('./debugClient');
const { stopProcessTree } = require('../src/debugProtocol');
const root = path.resolve(__dirname, '../..');
const compiler = process.env.CHALI_BIN;
const work = fs.mkdtempSync(path.join(root, 'tmp/debug-remote-native-'));

/** 启动测试自己拥有的单会话 gdbserver，读取内核 PID 和自动分配的监听端口。 */
async function server(executable) {
  const child = spawn(process.env.CHALI_GDBSERVER || 'gdbserver', ['--once', '127.0.0.1:0', executable],
    { cwd: work, env: process.env, detached: true });
  child.output = ''; child.errors = ''; child.completed = false;
  child.stdout.on('data', data => { child.output += data; });
  child.once('close', () => { child.completed = true; });
  try {
    await new Promise((resolve, reject) => {
      const timer = setTimeout(() => reject(new Error('gdbserver 未就绪：' + child.errors)), 10000);
      child.stderr.on('data', data => {
        child.errors += data;
        const port = /Listening on port (\d+)/.exec(child.errors), pid = /pid = (\d+)/.exec(child.errors);
        if (port && pid) { child.target = Number(pid[1]); child.port = Number(port[1]); clearTimeout(timer); resolve(); }
      });
      child.once('error', error => { clearTimeout(timer); reject(error); });
      child.once('exit', () => { clearTimeout(timer); reject(new Error('gdbserver 提前退出：' + child.errors)); });
    });
    return child;
  } catch (error) { await stopProcessTree(child); throw error; }
}

/** 只回收本测试启动并取得 PID 的目标和服务，不影响已有远程服务。 */
async function cleanup(child) {
  if (child?.target) try { process.kill(child.target, 'SIGKILL'); } catch (error) { if (error.code !== 'ESRCH') throw error; }
  await stopProcessTree(child);
}

async function main() {
  assert.equal(process.platform, 'linux', '真实远程专项要求 Linux 宿主和 Linux gdbserver');
  assert(compiler, '需要 CHALI_BIN');
  for (const allocator of ['malloc', 'pool']) {
    const original = path.join(work, allocator, 'target source'), local = path.join(work, allocator, 'local 中文');
    fs.mkdirSync(original, { recursive: true }); fs.mkdirSync(local);
    const source = path.join(original, 'main.chl'), executable = path.join(work, allocator, 'program');
    fs.writeFileSync(source, 'import std::thread;\nvoid main(){\n int64 running=1;int64 count=7;String owned="kept";\n while(running!=0){\n  count=count+1;\n  std::thread::sleepFor(10);\n }\n assert(owned=="kept");println("DONE");\n}\n');
    const built = spawnSync(compiler, ['aot', source, '--debug', '--checked', '--allocator', allocator, '--out', executable], { cwd: root, encoding: 'utf8', timeout: 180000 });
    assert.equal(built.status, 0, built.stdout + built.stderr);
    fs.renameSync(source, path.join(local, 'main.chl'));
    if (allocator === 'malloc') for (const wrong of ['image', 'pid', 'cancel']) {
      const rejectedTarget = await server(executable), rejected = new DebugClient(root, compiler);
      try {
        await rejected.request('initialize', { adapterID: 'chali' });
        const attaching = rejected.request('attach', {
          executable: wrong === 'image' ? process.execPath : executable,
          processId: rejectedTarget.target + (wrong === 'pid' ? 1 : 0),
          remote: { host: '127.0.0.1', port: rejectedTarget.port }, cwd: root,
        });
        if (wrong === 'cancel') {
          // 先确认服务端已报告真实线程，再取消尚未完成身份核对的连接。
          const rejectedAttach = assert.rejects(attaching, /取消/);
          await rejected.event('thread');
          await rejected.request('cancel', { requestId: attaching.requestId });
          await rejectedAttach;
        } else await assert.rejects(attaching, wrong === 'image' ? /身份不匹配/ : /PID/);
        await rejected.event('terminated');
        process.kill(rejectedTarget.target, 0);
        assert(!rejected.messages.some(message => message.event === 'stopped'));
        console.log('真实 gdbserver 拒绝错误' + wrong + '，目标保留通过');
      } finally { await rejected.close(); await cleanup(rejectedTarget); }
    }
    const child = await server(executable), client = new DebugClient(root, compiler);
    try {
      await client.request('initialize', { adapterID: 'chali' });
      await client.request('attach', { executable, processId: child.target, remote: { host: '127.0.0.1', port: child.port },
        cwd: root, stopOnAttach: false, sourceFileMap: { [original]: local } });
      await client.event('initialized');
      const points = await client.request('setBreakpoints', { source: { path: path.join(local, 'main.chl') }, breakpoints: [{ line: 5 }] });
      assert(points.breakpoints[0].verified, JSON.stringify(points));
      await client.request('configurationDone');
      const stop = await client.event('stopped');
      const frame = (await client.request('stackTrace', { threadId: stop.threadId })).stackFrames[0];
      assert.equal(frame.source.path, path.join(local, 'main.chl')); assert.equal(frame.line, 5);
      assert.equal((await client.request('evaluate', { expression: 'count+running', frameId: frame.id })).result, '8');
      await client.request('setBreakpoints', { source: { path: path.join(local, 'main.chl') }, breakpoints: [] });
      await client.request('continue', { threadId: stop.threadId });
      await new Promise(resolve => setTimeout(resolve, 100));
      await client.request('pause', { threadId: stop.threadId });
      const paused = await client.event('stopped'); assert.equal(paused.reason, 'pause');
      assert.equal(paused.allThreadsStopped, true);
      await assert.rejects(client.request('scopes', { frameId: frame.id }), /失效|过期/);
      await client.request('setBreakpoints', { source: { path: path.join(local, 'main.chl') }, breakpoints: [{ line: 5 }] });
      await client.request('continue', { threadId: paused.threadId });
      const again = await client.event('stopped');
      const resumedFrame = (await client.request('stackTrace', { threadId: again.threadId })).stackFrames[0];
      assert.equal(resumedFrame.line, 5);
      const scope = (await client.request('scopes', { frameId: resumedFrame.id })).scopes[0].variablesReference;
      await client.request('setVariable', { variablesReference: scope, name: 'running', value: '0' });
      await client.request('disconnect'); await client.event('terminated');
      if (!child.completed) await new Promise((resolve, reject) => {
        const timer = setTimeout(() => reject(new Error('远程目标分离后未正常退出：' + child.errors)), 10000);
        child.once('close', () => { clearTimeout(timer); resolve(); });
      });
      assert.match(child.output, /DONE/);
      console.log('真实 Linux gdbserver：' + allocator + '，源码映射、表达式、修改及分离后正常完成通过');
    } finally { await client.close(); await cleanup(child); }
  }
  const panicSource = path.join(work, 'panic.chl'), panicExecutable = path.join(work, 'panic');
  fs.writeFileSync(panicSource, 'void main(){\n assert(false,"remote-panic-marker");\n}\n');
  const panicBuilt = spawnSync(compiler, ['aot', panicSource, '--debug', '--checked', '--out', panicExecutable], { cwd: root, encoding: 'utf8', timeout: 180000 });
  assert.equal(panicBuilt.status, 0, panicBuilt.stdout + panicBuilt.stderr);
  const panicTarget = await server(panicExecutable), panicClient = new DebugClient(root, compiler);
  try {
    await panicClient.request('initialize', { adapterID: 'chali' });
    await panicClient.request('attach', { executable: panicExecutable, processId: panicTarget.target,
      remote: { host: '127.0.0.1', port: panicTarget.port }, cwd: root, stopOnAttach: false });
    await panicClient.event('initialized'); await panicClient.request('configurationDone');
    const stop = await panicClient.event('stopped');
    assert.equal(stop.reason, 'exception'); assert.match(stop.text, /remote-panic-marker/);
    await panicClient.request('terminate'); await panicClient.event('terminated');
    console.log('真实 gdbserver：Linux 目标 ABI 的 panic 消息和显式终止通过');
  } finally { await panicClient.close(); await cleanup(panicTarget); }
}
main().catch(error => { console.error(error); process.exitCode = 1; });
