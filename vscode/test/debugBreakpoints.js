'use strict';

/** 真实 GDB 验证条件、命中次数、函数与日志断点，不执行用户方法。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { spawnSync } = require('node:child_process');
const { DebugClient } = require('./debugClient');
const root = path.resolve(__dirname, '../..');
const compiler = process.env.CHALI_BIN || path.join(root, 'chali.exe');
const work = fs.mkdtempSync(path.join(root, 'tmp/debug-breakpoints-'));
const source = path.join(work, 'main.chl');
const lines = [
  'int64 add(int64 a, int64 b) {',
  '    int64 sum = a + b;',
  '    return sum;',
  '}',
  'void main() {',
  '    int64 total = 0;',
  '    for (int64 i in std::lang::range(0, 4)) {',
  '        total = add(total, i);',
  '    }',
  '    println("TOTAL {}", total);',
  '}',
];
fs.writeFileSync(source, lines.join('\n') + '\n');
const body = lines.indexOf('    int64 sum = a + b;') + 1;
const call = lines.indexOf('        total = add(total, i);') + 1;
const built = spawnSync(compiler, ['aot', source, '--debug', '--checked', '--allocator', 'malloc', '--out', path.join(work, 'program')], { cwd: root, env: process.env, encoding: 'utf8', windowsHide: true, timeout: 180000 });
assert.equal(built.status, 0, built.stdout + built.stderr);
const executable = path.join(work, 'program' + (process.platform === 'win32' ? '.exe' : ''));

/** 建立一次会话并停在配置阶段，便于先设置断点再启动。 */
async function start() {
  const client = new DebugClient(root, compiler);
  const capabilities = await client.request('initialize', { adapterID: 'chali' });
  assert.equal(capabilities.supportsConditionalBreakpoints, true);
  assert.equal(capabilities.supportsHitConditionalBreakpoints, true);
  assert.equal(capabilities.supportsFunctionBreakpoints, true);
  assert.equal(capabilities.supportsLogPoints, true);
  await client.request('launch', { executable, cwd: root });
  await client.event('initialized');
  return client;
}

/** 读取当前暂停帧的局部变量。 */
async function variables(client, thread) {
  const stack = await client.request('stackTrace', { threadId: thread });
  const top = stack.stackFrames[0];
  const scopes = await client.request('scopes', { frameId: top.id });
  const data = await client.request('variables', { variablesReference: scopes.scopes[0].variablesReference });
  return { top, values: Object.fromEntries(data.variables.map((value) => [value.name, value.value])) };
}

async function main() {
  // 条件断点：只在 b == 2 的调用停住。
  const conditional = await start();
  try {
    const points = await conditional.request('setBreakpoints', { source: { path: source }, breakpoints: [{ line: body, condition: 'b == 2' }] });
    assert.equal(points.breakpoints[0].verified, true, JSON.stringify(points));
    await conditional.request('configurationDone');
    const stop = await conditional.event('stopped');
    const view = await variables(conditional, stop.threadId);
    assert.equal(view.top.line, body);
    assert.equal(view.values.a, '1');
    assert.equal(view.values.b, '2');
    await conditional.request('continue', { threadId: stop.threadId });
    await conditional.event('terminated');
  } finally { await conditional.close(); }

  // 命中次数：第四次命中 add 才停，之后不再重复暂停。
  const hits = await start();
  try {
    const points = await hits.request('setBreakpoints', { source: { path: source }, breakpoints: [{ line: body, hitCondition: '4' }] });
    assert.equal(points.breakpoints[0].verified, true, JSON.stringify(points));
    await hits.request('configurationDone');
    const stop = await hits.event('stopped');
    const view = await variables(hits, stop.threadId);
    assert.equal(view.values.a, '3', JSON.stringify(view));
    await hits.request('continue', { threadId: stop.threadId });
    await hits.event('terminated');
  } finally { await hits.close(); }

  // 函数断点：用源码名登记，在函数入口停下。
  const functions = await start();
  try {
    const points = await functions.request('setFunctionBreakpoints', { breakpoints: [{ name: 'program::add' }] });
    assert.equal(points.breakpoints[0].verified, true, JSON.stringify(points));
    await functions.request('configurationDone');
    const stop = await functions.event('stopped');
    const stack = await functions.request('stackTrace', { threadId: stop.threadId });
    assert.match(stack.stackFrames[0].name, /add/);
    // 函数断点每次调用都会命中，继续前先撤掉才能跑到进程结束。
    const removed = await functions.request('setFunctionBreakpoints', { breakpoints: [] });
    assert.deepEqual(removed.breakpoints, []);
    await functions.request('continue', { threadId: stop.threadId });
    await functions.event('terminated');
  } finally { await functions.close(); }

  // 日志断点：输出插值文本但不暂停，程序直接跑完。
  const logs = await start();
  try {
    const points = await logs.request('setBreakpoints', { source: { path: source }, breakpoints: [{ line: call, logMessage: 'i={i}' }] });
    assert.equal(points.breakpoints[0].verified, true, JSON.stringify(points));
    await logs.request('configurationDone');
    await logs.event('terminated');
    const output = logs.messages.filter((message) => message.event === 'output').map((message) => message.body.output).join('');
    for (const value of ['i=0', 'i=1', 'i=2', 'i=3']) assert(output.includes(value), output);
    assert(!logs.messages.some((message) => message.event === 'stopped'), '日志断点不能报告暂停');
  } finally { await logs.close(); }

  // 无效条件、命中次数与空函数名必须拒绝，不留下半成品断点。
  const invalid = await start();
  try {
    const points = await invalid.request('setBreakpoints', { source: { path: source }, breakpoints: [
      { line: body, condition: 'a ==\nb' }, { line: body, hitCondition: 'many' }, { line: body, condition: 'a == 1' }] });
    assert.equal(points.breakpoints[0].verified, false);
    assert.equal(points.breakpoints[1].verified, false);
    assert.equal(points.breakpoints[2].verified, true);
    const bad = await invalid.request('setFunctionBreakpoints', { breakpoints: [{ name: '' }] });
    assert.equal(bad.breakpoints[0].verified, false);
    await invalid.request('configurationDone');
    const stop = await invalid.event('stopped');
    const view = await variables(invalid, stop.threadId);
    assert.equal(view.values.a, '1', JSON.stringify(view));
    await invalid.request('continue', { threadId: stop.threadId });
    await invalid.event('terminated');
  } finally { await invalid.close(); }
  console.log('调试断点：条件、命中次数、函数断点、日志插值及无效输入拒绝通过');
}
main().catch((error) => { console.error(error); process.exitCode = 1; });
