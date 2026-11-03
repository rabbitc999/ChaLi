'use strict';

/** 真实 C 同步回调在主线程与隔离线程中读取各自的源码帧和堆。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { spawnSync } = require('node:child_process');
const { DebugClient } = require('./debugClient');
const root = path.resolve(__dirname, '../..'), compiler = process.env.CHALI_BIN;
assert(compiler);
const work = fs.mkdtempSync(path.join(root, 'tmp/debug-callback-context-'));
const cc = process.env.CHALI_LLVM_CLANG || (process.platform === 'win32' ? path.join(root, 'build/dependencies/llvm/toolchain/bin/clang.exe') : 'clang');
const library = path.join(work, process.platform === 'win32' ? 'callback.dll' : 'callback.so');
const host = path.join(work, 'host.c');
fs.writeFileSync(host, '#include <stdint.h>\n/** @brief 同步调用语言回调。\n * @param function 回调跳板。\n * @param value 输入整数。\n * @return 回调返回值。 */\n#ifdef _WIN32\n__declspec(dllexport)\n#endif\nint64_t hostApply(int64_t (*function)(int64_t),int64_t value){return function(value);}\n');
const compiled = spawnSync(cc, ['-shared', '-O2', host, '-o', library], { cwd: root, encoding: 'utf8', windowsHide: true });
assert.equal(compiled.status, 0, compiled.stdout + compiled.stderr);
const source = path.join(work, 'main.chl');
const lines = [
  'import std::thread;',
  '#abi("C","' + library.replace(/\\/g, '/') + '","hostApply") int64 hostApply(int64 fn(int64) function,int64 value);',
  'struct Row { int64 marker; };',
  'int64 callback(int64 seed){',
  ' Row row; row.marker=seed;',
  ' int64 data[]=new[2]; data[0]=seed+1;',
  ' println("CALLBACK {}", seed);',
  ' return row.marker+data[0];',
  '}',
  'void worker(){',
  ' int64 workerData[]=new[2]; workerData[0]=222;',
  ' int64 result=hostApply(callback,20);',
  ' assert(result==41);',
  ' println("WORKER BACK");',
  '}',
  'void main(){',
  ' int64 rootData[]=new[2]; rootData[0]=111;',
  ' int64 result=hostApply(callback,10);',
  ' assert(result==21);',
  ' println("ROOT BACK");',
  ' Thread task=std::thread::create(worker);',
  ' std::thread::wait(task);',
  ' assert(rootData[0]==111);',
  '}',
];
fs.writeFileSync(source, lines.join('\n'));
async function variables(client, frameId) {
  const scopes = await client.request('scopes', { frameId });
  return Object.fromEntries((await client.request('variables', { variablesReference: scopes.scopes[0].variablesReference })).variables.map(item => [item.name, item]));
}
async function first(client, value) {
  assert(value?.variablesReference, JSON.stringify(value));
  return (await client.request('variables', { variablesReference: value.variablesReference, count: 1 })).variables[0].value;
}
async function main() {
  for (const allocator of ['malloc', 'pool']) {
    const prefix = path.join(work, allocator);
    const built = spawnSync(compiler, ['aot', source, '--debug', '--checked', '--allocator', allocator, '--out', prefix],
      { cwd: root, env: { ...process.env, CHALI_ABI_CXX: process.env.CHALI_ABI_CXX || 'g++' }, encoding: 'utf8', windowsHide: true, timeout: 180000 });
    assert.equal(built.status, 0, built.stdout + built.stderr);
    const client = new DebugClient(root, compiler);
    try {
      await client.request('initialize', { adapterID: 'chali' });
      await client.request('launch', { executable: prefix + (process.platform === 'win32' ? '.exe' : ''), cwd: work });
      await client.event('initialized');
      await client.request('setBreakpoints', { source: { path: source }, breakpoints: [{ line: 7 }, { line: 23 }] });
      await client.request('configurationDone');
      const threads = [];
      let previous;
      for (const [seed, callerName, dataName, marker] of [[10, 'main', 'rootData', '111'], [20, 'worker', 'workerData', '222']]) {
        const stop = await client.event('stopped'); threads.push(stop.threadId);
        if (previous) await assert.rejects(client.request('variables', { variablesReference: previous }), /过期/);
        const frames = (await client.request('stackTrace', { threadId: stop.threadId })).stackFrames;
        // 从源码表达式核对当前回调的真实上下文和堆，公开求值不接受原生指针转换。
        const published = await client.request('evaluate', { frameId: frames[0].id, expression: 'row.marker + data[0]' });
        assert.equal(published.result, String(seed * 2 + 1));
        const own = await variables(client, frames[0].id);
        assert.equal(await first(client, own.row), String(seed));
        assert.equal(await first(client, own.data), String(seed + 1));
        const caller = frames.find(frame => frame.source?.path === source && frame.name.includes(callerName));
        assert(caller, JSON.stringify(frames));
        assert.equal(await first(client, (await variables(client, caller.id))[dataName]), marker);
        previous = own.data.variablesReference;
        await client.request('continue', { threadId: stop.threadId });
      }
      assert.notEqual(threads[0], threads[1]);
      const returned = await client.event('stopped');
      assert.equal(returned.reason, 'breakpoint', JSON.stringify(returned));
      assert.equal(returned.threadId, threads[0]);
      const remaining = (await client.request('threads')).threads;
      assert.equal(remaining.some(thread => thread.id === threads[1]), false);
      const frame = (await client.request('stackTrace', { threadId: returned.threadId })).stackFrames[0];
      assert.equal((await client.request('evaluate', { frameId: frame.id, expression: 'rootData[0]' })).result, '111');
      await client.request('continue', { threadId: returned.threadId });
      assert.equal((await client.event('exited')).exitCode, 0);
      await client.event('terminated');
    } finally { await client.close(); }
  }
  console.log('同步回调调试：malloc/pool、根线程与隔离线程的上下文、真实堆、调用方帧及过期引用隔离通过');
}
main().catch(error => { console.error(error); process.exitCode = 1; });
