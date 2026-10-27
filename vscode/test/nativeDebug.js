'use strict';

/** 使用真实 GDB 验证 AOT 断点、单步、调用栈、变量和表达式。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { spawn } = require('node:child_process');
const root = path.resolve(__dirname, '../..');
const version = fs.readFileSync(path.join(root, 'VERSION'), 'utf8').trim();
const compiler = process.env.CHALI_BIN || path.join(root, `chali-${version}-llvm-external-windows-x64.exe`);
const work = fs.mkdtempSync(path.join(root, 'tmp/native-debug-test-'));
const source = path.join(work, 'example.chl');
fs.writeFileSync(source, 'int64 increment(int64 x) {\n    int64 result = x + 1;\n    return result;\n}\nvoid main() {\n    int64 value = 7;\n    value = increment(value);\n    println("value {}", value);\n}\n');
const child = spawn(process.execPath, [path.join(root, 'vscode/src/nativeDebugAdapter.js'), compiler], { cwd: root, env: process.env, windowsHide: true });
let buffer = Buffer.alloc(0), sequence = 1, stderr = '';
const pending = new Map(), events = [], waiters = [], messages = [];
child.stderr.on('data', (chunk) => { stderr += chunk; });
child.stdout.on('data', (chunk) => {
  buffer = Buffer.concat([buffer, chunk]);
  while (true) {
    const at = buffer.indexOf('\r\n\r\n');
    if (at < 0) break;
    const length = Number(/Content-Length:\s*(\d+)/i.exec(buffer.subarray(0, at).toString())[1]);
    if (buffer.length < at + 4 + length) break;
    const message = JSON.parse(buffer.subarray(at + 4, at + 4 + length).toString());
    buffer = buffer.subarray(at + 4 + length); messages.push(message);
    if (message.type === 'response') {
      const task = pending.get(message.request_seq);
      if (task) { pending.delete(message.request_seq); clearTimeout(task.timer); message.success ? task.resolve(message.body || {}) : task.reject(new Error(message.message)); }
    } else if (message.type === 'event') {
      const index = waiters.findIndex((task) => task.event === message.event);
      if (index < 0) events.push(message);
      else { const [task] = waiters.splice(index, 1); clearTimeout(task.timer); task.resolve(message.body); }
    }
  }
});
/** 按真实 DAP 编码发送请求，任何超时或失败终止验收。 */
function request(command, args = {}) {
  const seq = sequence++;
  const data = Buffer.from(JSON.stringify({ seq, type: 'request', command, arguments: args }));
  return new Promise((resolve, reject) => {
    const timer = setTimeout(() => { pending.delete(seq); reject(new Error('DAP 请求超时：' + command + '\n' + stderr)); }, 180000);
    pending.set(seq, { resolve, reject, timer });
    child.stdin.write(`Content-Length: ${data.length}\r\n\r\n`); child.stdin.write(data);
  });
}
/** 等待异步事件，兼容事件先于请求响应到达。 */
function event(name) {
  const index = events.findIndex((message) => message.event === name);
  if (index >= 0) return Promise.resolve(events.splice(index, 1)[0].body);
  return new Promise((resolve, reject) => {
    const task = { event: name, resolve, reject };
    task.timer = setTimeout(() => { const at = waiters.indexOf(task); if (at >= 0) waiters.splice(at, 1); reject(new Error('DAP 事件超时：' + name + '\n' + stderr)); }, 180000);
    waiters.push(task);
  });
}
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
    await request('stepIn', { threadId: stop.threadId });
    stop = await event('stopped');
    view = await variables(stop.threadId);
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
    await request('continue', { threadId: stop.threadId });
    await event('terminated');
    const output = messages.filter((message) => message.event === 'output').map((message) => message.body.output).join('');
    assert.match(output, /value 8/);
    fs.writeFileSync(path.join(work, 'result.json'), JSON.stringify({ passed: true, compiler, source, messages }, null, 2));
    console.log('原生 DAP：真实断点、源码行、单步进入／越过／跳出、参数／局部变量和表达式通过：' + work);
  } finally {
    fs.writeFileSync(path.join(work, 'messages.json'), JSON.stringify({ stderr, messages }, null, 2));
    await request('disconnect').catch(() => {});
    child.stdin.end();
  }
}
main().catch((error) => { console.error(error); child.kill(); process.exitCode = 1; });
