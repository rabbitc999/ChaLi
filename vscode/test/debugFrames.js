'use strict';

/** 真实 GDB 验证非栈顶调用帧的记录、数组与内存引用，以及只读查看不恢复目标执行。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { spawnSync } = require('node:child_process');
const { DebugClient } = require('./debugClient');
const root = path.resolve(__dirname, '../..');
const compiler = process.env.CHALI_BIN || path.join(root, 'chali.exe');
const work = fs.mkdtempSync(path.join(root, 'tmp/debug-frames-'));
const source = path.join(work, 'main.chl');
const lines = [
  'struct Pair { int64 first; int64 second; };',
  'int64 calls;',
  'int64 inspect(Pair item) {',
  '    calls = calls + 1;',
  '    int64 marker = 5;',
  '    println("INSIDE {}", marker);',
  '    return item.first;',
  '}',
  'void main() {',
  '    Pair row; row.first=11; row.second=22;',
  '    int64 values[]=new[2]; values[0]=7; values[1]=8;',
  '    int64 result = inspect(row);',
  '    println("OUTSIDE {} {}", result, calls);',
  '}',
];
fs.writeFileSync(source, lines.join('\n') + '\n');
const stopLine = lines.indexOf('    println("INSIDE {}", marker);') + 1;
const built = spawnSync(compiler, ['aot', source, '--debug', '--checked', '--allocator', 'malloc', '--out', path.join(work, 'program')], { cwd: root, env: process.env, encoding: 'utf8', windowsHide: true, timeout: 180000 });
assert.equal(built.status, 0, built.stdout + built.stderr);

/** 按帧序号取可见变量，保留展开引用。 */
async function variables(client, thread, level) {
  const stack = await client.request('stackTrace', { threadId: thread });
  const frame = level === 0 ? stack.stackFrames[0]
    : stack.stackFrames.find((item) => /main/.test(item.name)) || stack.stackFrames[stack.stackFrames.length - 1];
  const scopes = await client.request('scopes', { frameId: frame.id });
  const data = await client.request('variables', { variablesReference: scopes.scopes[0].variablesReference });
  return { frame, values: Object.fromEntries(data.variables.map((item) => [item.name, item])) };
}

async function main() {
  const client = new DebugClient(root, compiler);
  try {
    await client.request('initialize', { adapterID: 'chali' });
    await client.request('launch', { executable: path.join(work, 'program' + (process.platform === 'win32' ? '.exe' : '')), cwd: root });
    await client.event('initialized');
    await client.request('setBreakpoints', { source: { path: source }, breakpoints: [{ line: stopLine }] });
    await client.request('configurationDone');
    const stop = await client.event('stopped');

    // 栈顶帧：按值参数与局部变量照常展开。
    const inside = await variables(client, stop.threadId, 0);
    assert.match(inside.frame.name, /inspect/);
    assert.equal(inside.values.item.value, '{…}');
    const item = await client.request('variables', { variablesReference: inside.values.item.variablesReference });
    assert.deepEqual(Object.fromEntries(item.variables.map((entry) => [entry.name, entry.value])), { first: '11', second: '22' });
    assert.equal(inside.values.marker.value, '5');

    // 非栈顶帧：调用方的记录与数组同样可以展开并读取真实字节。
    const outside = await variables(client, stop.threadId, 1);
    assert.match(outside.frame.name, /main/);
    assert.equal(outside.values.row.value, '{…}');
    assert(outside.values.row.memoryReference, '调用方聚合值需要内存引用');
    const row = await client.request('variables', { variablesReference: outside.values.row.variablesReference });
    assert.deepEqual(Object.fromEntries(row.variables.map((entry) => [entry.name, entry.value])), { first: '11', second: '22' });
    assert.equal(outside.values.values.value, '[2]');
    const elements = await client.request('variables', { variablesReference: outside.values.values.variablesReference });
    assert.deepEqual(Object.fromEntries(elements.variables.map((entry) => [entry.name, entry.value])), { '0': '7', '1': '8' });
    const raw = await client.request('readMemory', { memoryReference: outside.values.values.memoryReference, count: 16 });
    const bytes = Buffer.from(raw.data, 'base64');
    assert.equal(bytes.readBigInt64LE(0), 7n);
    assert.equal(bytes.readBigInt64LE(8), 8n);

    // 查看期间目标必须保持暂停：停点之后不能再有继续或新的停止事件。
    const afterStop = client.messages.slice(client.messages.findIndex((message) => message.event === 'stopped') + 1);
    assert(!afterStop.some((message) => message.event === 'continued'), '查看不能恢复目标执行');
    assert(!afterStop.some((message) => message.event === 'stopped'), '查看不能产生新的停点');
    await client.request('continue', { threadId: stop.threadId });
    await client.event('terminated');
    const output = client.messages.filter((message) => message.event === 'output').map((message) => message.body.output).join('');
    assert.match(output, /OUTSIDE 11 1/, output);
  } finally {
    fs.writeFileSync(path.join(work, 'messages.json'), JSON.stringify({ messages: client.messages, stderr: client.stderr }, null, 2));
    await client.close();
  }
  console.log('原生调试调用帧：栈顶与非栈顶帧记录、数组、内存引用及只读查看不恢复执行通过');
}
main().catch((error) => { console.error(error); process.exitCode = 1; });
