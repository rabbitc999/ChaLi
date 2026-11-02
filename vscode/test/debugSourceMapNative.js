'use strict';

/** 删除原始源码后用真实 GDB 验证映射断点、当前源码路径、词法值与只读表达式。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { spawnSync } = require('node:child_process');
const { DebugClient } = require('./debugClient');
const root = path.resolve(__dirname, '../..'), compiler = process.env.CHALI_BIN;
assert(compiler);
const work = fs.mkdtempSync(path.join(root, 'tmp/debug-source-map-'));
const original = path.join(work, 'original source'), local = path.join(work, 'local 中文');
fs.mkdirSync(original); fs.mkdirSync(local);
const input = path.join(original, 'main.chl'), source = path.join(local, 'main.chl'), prefix = path.join(work, 'program');
fs.writeFileSync(input, 'struct Row{int64 value;};\nvoid main(){\n Row row;row.value=7;\n int64 count=41;\n println("READY");\n assert(count==42);\n}\n');
const built = spawnSync(compiler, ['aot', input, '--debug', '--checked', '--out', prefix], { cwd: root, encoding: 'utf8', windowsHide: true, timeout: 180000 });
assert.equal(built.status, 0, built.stdout + built.stderr); fs.renameSync(input, source);

async function main() {
  const client = new DebugClient(root, compiler);
  try {
    await client.request('initialize', { adapterID: 'chali' });
    await client.request('launch', { executable: prefix + (process.platform === 'win32' ? '.exe' : ''), cwd: root, sourceFileMap: { [original]: local } });
    await client.event('initialized');
    const points = await client.request('setBreakpoints', { source: { path: source }, breakpoints: [{ line: 5 }] });
    assert(points.breakpoints[0].verified, JSON.stringify(points)); await client.request('configurationDone');
    const stop = await client.event('stopped'), frames = (await client.request('stackTrace', { threadId: stop.threadId })).stackFrames;
    assert.equal(frames[0].source.path, source); assert.equal(frames[0].line, 5);
    assert.equal((await client.request('evaluate', { frameId: frames[0].id, expression: 'row.value+count' })).result, '48');
    const scope = (await client.request('scopes', { frameId: frames[0].id })).scopes[0];
    await client.request('setVariable', { variablesReference: scope.variablesReference, name: 'count', value: '42' });
    await client.request('continue', { threadId: stop.threadId }); assert.equal((await client.event('exited')).exitCode, 0); await client.event('terminated');
  } finally { await client.close(); }
  console.log('源码映射真实 GDB：原源码已移走、空格/中文路径、双向断点、调用栈、字段表达式与变量修改通过');
}
main().catch(error => { console.error(error); process.exitCode = 1; });
