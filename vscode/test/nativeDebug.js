'use strict';

/** 使用真实 GDB 验证 BUILD 断点、单步、调用栈、变量和表达式。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { DebugClient } = require('./debugClient');
const root = path.resolve(__dirname, '../..');
const version = fs.readFileSync(path.join(root, 'VERSION'), 'utf8').trim();
const compiler = process.env.CHALI_BIN || path.join(root, `chali-${version}-llvm-external-windows-x64.exe`);
const work = fs.mkdtempSync(path.join(root, 'tmp/native-debug-test-'));
const source = path.join(work, 'example.chl');
fs.writeFileSync(source, 'int64 increment(int64 x) {\n    int64 result = x + 1;\n    return result;\n}\nvoid main() {\n    int64 value = 7;\n    value = increment(value);\n    println("value {}", value);\n}\n');
const client = new DebugClient(root, compiler);
const messages = client.messages;
const request = (...args) => client.request(...args);
const event = (...args) => client.event(...args);
/** 查询停止位置的本地变量。 */
async function variables(thread) {
  const stack = await request('stackTrace', { threadId: thread });
  assert(stack.stackFrames.length > 0, stack);
  const top = stack.stackFrames[0];
  const scopes = await request('scopes', { frameId: top.id });
  const data = await request('variables', { variablesReference: scopes.scopes[0].variablesReference });
  return { top, values: Object.fromEntries(data.variables.map((value) => [value.name, value.value])) };
}
/** 验证机器码实际执行并逐步检查源变量。 */
async function main() {
  try {
    await request('initialize', { adapterID: 'chali', linesStartAt1: true, columnsStartAt1: true });
    await request('launch', { program: source, cwd: root, debuggerPath: process.env.CHALI_GDB || 'gdb' });
    await event('initialized');
    const points = await request('setBreakpoints', { source: { path: source }, breakpoints: [{ line: 7 }] });
    assert.equal(points.breakpoints[0].verified, true, JSON.stringify(points));
    await request('configurationDone');
    let stop = await event('stopped');
    assert.equal(stop.reason, 'breakpoint');
    let view = await variables(stop.threadId);
    assert.equal(view.top.line, 7, JSON.stringify(view));
    assert.equal(view.values.value, '7', JSON.stringify(view));
    const evaluated = await request('evaluate', { frameId: view.top.id, expression: 'value + 3', context: 'watch' });
    assert.equal(evaluated.result, '10');
    const oldFrame = view.top.id;
    await request('stepIn', { threadId: stop.threadId });
    stop = await event('stopped');
    view = await variables(stop.threadId);
    await assert.rejects(request('scopes', { frameId: oldFrame }), /过期/);
    assert.match(view.top.name, /increment/);
    assert.equal(view.values.x, '7', JSON.stringify(view));
    await request('next', { threadId: stop.threadId });
    stop = await event('stopped');
    view = await variables(stop.threadId);
    assert.equal(view.values.result, '8', JSON.stringify(view));
    await request('stepOut', { threadId: stop.threadId });
    stop = await event('stopped');
    view = await variables(stop.threadId);
    assert.match(view.top.name, /main/);
    await request('restart');
    stop = await event('stopped');
    assert.equal(stop.reason, 'breakpoint');
    view = await variables(stop.threadId);
    assert.equal(view.top.line, 7);
    assert.equal(view.values.value, '7');
    await request('continue', { threadId: stop.threadId });
    await event('terminated');
    const output = messages.filter((message) => message.event === 'output').map((message) => message.body.output).join('');
    assert.match(output, /value 8/);
    fs.writeFileSync(path.join(work, 'result.json'), JSON.stringify({ passed: true, compiler, source, messages }, null, 2));
    console.log('原生 DAP：真实断点、源码行、单步进入／越过／跳出、参数／局部变量和表达式通过：' + work);
  } finally {
    fs.writeFileSync(path.join(work, 'messages.json'), JSON.stringify({ stderr: client.stderr, messages }, null, 2));
    await client.close();
  }
}
main().catch((error) => { console.error(error); void client.close(); process.exitCode = 1; });
