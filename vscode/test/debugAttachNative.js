'use strict';

/** 本机真实进程附加：身份拒绝、映射源码、修改后分离、运行中分离、显式终止和取消。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { spawn, spawnSync } = require('node:child_process');
const { DebugClient } = require('./debugClient');
const { stopProcessTree } = require('../src/debugProtocol');
const root = path.resolve(__dirname, '../..'), compiler = process.env.CHALI_BIN;
assert(compiler);
const work = fs.mkdtempSync(path.join(root, 'tmp/debug-attach-'));
const targetEnv = { ...process.env };
if (process.platform === 'linux') {
  const file = path.join(work, 'allow_attach.c'), library = path.join(work, 'allow_attach.so');
  fs.writeFileSync(file, '#include <sys/prctl.h>\n#include <unistd.h>\n/**\n * @brief 仅为专项目标允许同一用户附加，不改变系统 ptrace 配置。\n * @return 无；无法设置时以 97 退出。\n */\n__attribute__((constructor)) static void chali_test_allow_attach(void){if(prctl(PR_SET_PTRACER,PR_SET_PTRACER_ANY,0,0,0))_exit(97);}\n');
  const built = spawnSync(process.env.CC || 'cc', ['-shared', '-fPIC', file, '-o', library], { cwd: root, encoding: 'utf8' });
  assert.equal(built.status, 0, built.stdout + built.stderr);
  targetEnv.LD_PRELOAD = library + (targetEnv.LD_PRELOAD ? ':' + targetEnv.LD_PRELOAD : '');
}

/** 从调用方启动目标，标准流与调试器独立，分离后仍归测试调用方管理。 */
async function target(executable) {
  const child = spawn(executable, [], { cwd: work, env: targetEnv, windowsHide: true, detached: process.platform !== 'win32' });
  child.output = ''; child.errors = ''; child.completed = false;
  child.once('close', () => { child.completed = true; });
  try { await new Promise((resolve, reject) => {
    const timer = setTimeout(() => reject(new Error('附加目标未就绪：' + child.errors)), 10000);
    child.stdout.setEncoding('utf8'); child.stderr.setEncoding('utf8');
    child.stdout.on('data', text => { child.output += text; if (child.output.includes('READY')) { clearTimeout(timer); resolve(); } });
    child.stderr.on('data', text => { child.errors += text; if (child.errors.includes('READY')) { clearTimeout(timer); resolve(); } });
    child.once('error', error => { clearTimeout(timer); reject(error); });
    child.once('exit', () => { clearTimeout(timer); if (!(child.output + child.errors).includes('READY')) reject(new Error('附加目标提前退出：' + child.errors)); });
  }); } catch (error) { await stopProcessTree(child); throw error; }
  return child;
}

/** 等待已恢复目标自行退出；强制清理不能冒充程序正常完成。 */
async function targetExit(child) {
  if (child.completed) return;
  await new Promise((resolve, reject) => {
    const timer = setTimeout(() => reject(new Error('分离后目标未自行完成：' + child.errors)), 10000);
    child.once('close', () => { clearTimeout(timer); resolve(); });
  });
}

/** 配置本机附加及可选源码断点，默认保留入口全停。 */
async function connect(child, executable, original, local, breakpoint = true, stopOnAttach = true) {
  const client = new DebugClient(root, compiler);
  try {
    await client.request('initialize', { adapterID: 'chali', supportsInvalidatedEvent: true });
    await client.request('attach', { processId: String(child.pid), executable, cwd: root, stopOnAttach, sourceFileMap: { [original]: local } });
    await client.event('initialized');
    if (breakpoint) {
      const installed = await client.request('setBreakpoints', { source: { path: path.join(local, 'main.chl') }, breakpoints: [{ line: 6 }] });
      assert(installed.breakpoints[0].verified, JSON.stringify(installed));
    }
    await client.request('configurationDone'); return client;
  } catch (error) { await client.close(); throw error; }
}

/** 读取当前源码暂停的词法变量与帧，用于核对真实程序进度。 */
async function values(client, stop) {
  const frame = (await client.request('stackTrace', { threadId: stop.threadId })).stackFrames[0];
  const scope = (await client.request('scopes', { frameId: frame.id })).scopes[0].variablesReference;
  const rows = (await client.request('variables', { variablesReference: scope })).variables;
  return { frame, scope, rows: Object.fromEntries(rows.map(row => [row.name, row])) };
}

async function main() {
  for (const allocator of ['malloc', 'pool']) {
    const directory = path.join(work, allocator), original = path.join(directory, 'original source'), local = path.join(directory, 'local 中文');
    fs.mkdirSync(original, { recursive: true }); fs.mkdirSync(local);
    const input = path.join(original, 'main.chl'), executable = path.join(directory, 'program' + (process.platform === 'win32' ? '.exe' : ''));
    fs.writeFileSync(input, 'import std::thread;\nvoid main(){\n int64 running=1;int64 count=0;String owned="kept";\n eprintln("READY");\n while(running!=0){\n  count=count+1;\n  std::thread::sleepFor(10);\n }\n assert(owned=="kept");println("DONE");\n}\n');
    const built = spawnSync(compiler, ['aot', input, '--debug', '--checked', '--allocator', allocator, '--out', executable.replace(/\.exe$/, '')],
      { cwd: root, encoding: 'utf8', windowsHide: true, timeout: 180000 });
    assert.equal(built.status, 0, built.stdout + built.stderr); fs.renameSync(input, path.join(local, 'main.chl'));
    let child = await target(executable), client;
    try {
      const refused = new DebugClient(root, compiler);
      try {
        await refused.request('initialize', { adapterID: 'chali' });
        await assert.rejects(refused.request('attach', { processId: child.pid, executable: compiler, cwd: root }), /身份不匹配/);
        await refused.event('terminated'); assert.equal(child.exitCode, null);
      } finally { await refused.close(); }
      client = await connect(child, executable, original, local);
      const entry = await client.event('stopped'); assert.equal(entry.reason, 'entry'); assert.equal(entry.allThreadsStopped, true);
      await client.request('continue', { threadId: entry.threadId });
      const stop = await client.event('stopped'), data = await values(client, stop);
      assert.equal(data.frame.source.path, path.join(local, 'main.chl')); assert.equal(data.frame.line, 6);
      assert.equal((await client.request('evaluate', { frameId: data.frame.id, expression: 'running' })).result, '1');
      await client.request('setVariable', { variablesReference: data.scope, name: 'running', value: '0' });
      await client.request('disconnect'); await client.event('terminated');
      await targetExit(child); assert.equal(child.exitCode, 0, child.errors); assert.match(child.output, /DONE/);
    } finally { await client?.close(); await stopProcessTree(child); }

    child = await target(executable); client = null;
    try {
      client = await connect(child, executable, original, local, true, false);
      const stopped = await client.event('stopped'), data = await values(client, stopped);
      await client.request('setVariable', { variablesReference: data.scope, name: 'running', value: '0' });
      await client.request('continue', { threadId: stopped.threadId });
      assert.equal((await client.event('exited')).exitCode, 0);
      await client.event('terminated'); await targetExit(child);
      assert.match(child.output, /DONE/);
    } finally { await client?.close(); await stopProcessTree(child); }

    child = await target(executable); client = null;
    try {
      client = await connect(child, executable, original, local, true, false);
      const stop = await client.event('stopped'), before = await values(client, stop);
      const count = BigInt(before.rows.count.value);
      await client.request('disconnect', { terminateDebuggee: false }); await client.event('terminated'); await client.close(); client = null;
      await new Promise(resolve => setTimeout(resolve, 100)); assert.equal(child.exitCode, null);
      client = await connect(child, executable, original, local, true, false);
      const again = await client.event('stopped'), after = await values(client, again);
      assert(BigInt(after.rows.count.value) > count);
      await assert.rejects(client.request('restart'), /附加会话/);
      await client.request('setBreakpoints', { source: { path: path.join(local, 'main.chl') }, breakpoints: [] });
      await client.request('continue', { threadId: again.threadId });
      await client.request('disconnect'); await client.event('terminated'); await client.close(); client = null;
      await new Promise(resolve => setTimeout(resolve, 100)); assert.equal(child.exitCode, null);
      client = await connect(child, executable, original, local, false);
      await client.event('stopped'); await client.request('terminate'); await client.event('terminated');
      await targetExit(child); assert(!child.output.includes('DONE'));
    } finally { await client?.close(); await stopProcessTree(child); }

    child = await target(executable); client = new DebugClient(root, compiler);
    try {
      await client.request('initialize', { adapterID: 'chali' });
      const attaching = client.request('attach', { processId: child.pid, executable, cwd: root });
      const rejected = assert.rejects(attaching, /取消|关闭/);
      await client.event('thread'); await client.request('cancel', { requestId: attaching.requestId });
      await rejected; await client.event('terminated');
      await new Promise(resolve => setTimeout(resolve, 100)); assert.equal(child.exitCode, null);
    } finally { await client.close(); await stopProcessTree(child); }
  }
  console.log('本机附加真实 GDB：malloc/pool、目标/符号身份、映射与修改、暂停/运行中分离、重新附加、显式终止和取消保留目标通过');
}
main().catch(error => { console.error(error); process.exitCode = 1; });
