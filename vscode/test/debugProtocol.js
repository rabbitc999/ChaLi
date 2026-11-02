'use strict';

/** 验证 DAP 消息边界、GDB/MI 迟到响应和暂停代次隔离。 */
const assert = require('node:assert/strict');
const { PassThrough, Writable } = require('node:stream');
const { EventEmitter } = require('node:events');
const { RequestReader, HEADER_LIMIT, BODY_LIMIT } = require('../src/debugProtocol');
const { Adapter, Gdb, parseMi } = require('../src/nativeDebugAdapter');

/** 将真实请求封装成可任意分片的字节流。 */
function packet(seq, command, args = {}) {
  const body = Buffer.from(JSON.stringify({ seq, type: 'request', command, arguments: args }));
  return Buffer.concat([Buffer.from(`Content-Length: ${body.length}\r\n\r\n`), body]);
}

/** 等待异步响应边界。 */
function tick() { return new Promise((resolve) => setImmediate(resolve)); }

/** 消息读取覆盖 UTF-8 分片、粘包、坏头、预算和截断。 */
function framing() {
  const seen = [], failures = [];
  const reader = new RequestReader((value) => seen.push(value), (error) => failures.push(error));
  const input = Buffer.concat([packet(1, 'evaluate', { expression: '中文😀' }), packet(2, 'threads')]);
  for (const byte of input) reader.push(Buffer.from([byte]));
  assert.deepEqual(seen.map((value) => value.command), ['evaluate', 'threads']);
  assert.equal(seen[0].arguments.expression, '中文😀');
  assert.equal(failures.length, 0);
  for (const data of [
    Buffer.from('Content-Length: -1\r\n\r\n'),
    Buffer.from('Content-Length: 2\r\nContent-Length: 2\r\n\r\n{}'),
    Buffer.from(`Content-Length: ${BODY_LIMIT + 1}\r\n\r\n`),
    Buffer.alloc(HEADER_LIMIT + 1, 65),
    Buffer.from('Content-Length: 1\r\n\r\n{'),
    Buffer.from('Content-Length: 2\r\n\r\n{}'),
  ]) {
    let count = 0;
    const invalid = new RequestReader(() => assert.fail('无效请求不得分派'), () => count++);
    invalid.push(data); invalid.push(packet(3, 'threads'));
    assert.equal(count, 1);
    assert.equal(invalid.buffer.length, 0);
  }
  let truncated = false;
  const partial = new RequestReader(() => assert.fail(), () => { truncated = true; });
  partial.push(Buffer.from('Content-Length: 20\r\n\r\n{')); partial.finish();
  assert(truncated);
}

/** 用字节流模拟 MI 子进程，验证 token、超时、取消和异步事件。 */
async function mi() {
  assert.deepEqual(parseMi('{stack=[frame={level="0",func="中文😀"}]}'), { stack: [{ frame: { level: '0', func: '中文😀' } }] });
  assert.equal(parseMi('"\\344\\270\\255"'), '中');
  for (const bad of ['"unfinished', '{a="x"', '[a="x"', '{}junk']) assert.throws(() => parseMi(bad));
  const child = new EventEmitter();
  const commands = [];
  child.stdout = new PassThrough(); child.stderr = new PassThrough();
  child.stdin = new Writable({ write(chunk, encoding, callback) { commands.push(chunk.toString()); callback(); } });
  child.exitCode = null; child.signalCode = null;
  const gdb = new Gdb('fake-gdb', '.', {}, { spawn: () => child });
  const first = gdb.command('-thread-info');
  const second = gdb.command('-data-evaluate-expression "1"');
  child.stdout.write('2^done,value="1"\n1^done,threads=[]\n');
  assert.deepEqual(await first, { threads: [] });
  assert.deepEqual(await second, { value: '1' });
  await assert.rejects(gdb.command('-slow', { timeoutMs: 5 }), /超时/);
  let settled = false;
  const next = gdb.command('-next').then(() => { settled = true; });
  child.stdout.write('3^done\n'); await tick(); assert.equal(settled, false);
  child.stdout.write('4^done\n'); await next;
  const controller = new AbortController();
  const cancelled = gdb.command('-read', { signal: controller.signal });
  controller.abort(); await assert.rejects(cancelled, /取消/);
  child.stdout.write('5^done,value="late"\n');
  assert.equal(gdb.pending.size, 0);
  const running = [], stopped = [];
  gdb.on('running', (value) => running.push(value));
  gdb.on('stopped', (value) => stopped.push(value));
  child.stdout.write('*running,thread-id="all"\n*stopped,reason="breakpoint-hit",thread-id="2"\n');
  assert.equal(running.length, 1); assert.equal(stopped[0]['thread-id'], '2');
  const waiting = gdb.command('-waiting');
  child.exitCode = 1; child.emit('exit', 1);
  await assert.rejects(waiting, /退出/); assert.equal(gdb.pending.size, 0);
  assert.equal(commands.length, 6);
}

/** 暂停时发起读取，再继续运行，迟到结果必须失败而非展示旧数据。 */
async function session() {
  const input = new PassThrough();
  const adapter = new Adapter('unused', input, new PassThrough());
  const messages = [];
  adapter.send = (value) => messages.push(value);
  await adapter.request({ seq: 1, command: 'initialize' });
  assert.equal(messages.at(-1).body.supportsCancelRequest, true);
  let resolveRead;
  adapter.gdb = {
    command(command, options = {}) {
      if (command.startsWith('-stack-list-variables')) return new Promise((resolve, reject) => {
        resolveRead = resolve;
        options.signal.addEventListener('abort', () => reject(new Error('已取消')), { once: true });
      });
      return Promise.resolve({});
    },
    fail() {}, process: { exitCode: 0, signalCode: null },
  };
  adapter.state = 'stopped';
  await adapter.request({ seq: 20, command: 'launch', arguments: { program: 'duplicate.chl' } });
  assert.equal(messages.at(-1).success, false);
  assert.equal(adapter.state, 'stopped');
  assert(adapter.gdb);
  await adapter.request({ seq: 21, command: 'stackTrace', arguments: { threadId: '1\n-exec-run' } });
  assert.equal(messages.at(-1).success, false);
  adapter.frames.set(10, { thread: 1, level: 0 });
  adapter.dispatch({ seq: 2, command: 'evaluate', arguments: { frameId: 10, expression: 'x' } });
  await adapter.request({ seq: 3, command: 'continue' });
  resolveRead({ value: 'old' }); await tick();
  assert.equal(messages.find((value) => value.request_seq === 2).success, false);
  adapter.state = 'stopped';
  await adapter.request({ seq: 4, command: 'scopes', arguments: { frameId: 10 } });
  assert.equal(messages.at(-1).success, false);
  adapter.frames.set(11, { thread: 1, level: 0 });
  adapter.dispatch({ seq: 5, command: 'evaluate', arguments: { frameId: 11, expression: 'x' } });
  adapter.dispatch({ seq: 6, command: 'cancel', arguments: { requestId: 5 } });
  await tick();
  assert.equal(messages.find((value) => value.request_seq === 5).success, false);
  await adapter.request({ seq: 7, command: 'disconnect' });
  await adapter.request({ seq: 8, command: 'terminate' });
  assert.equal(messages.filter((value) => value.event === 'terminated').length, 1);
  assert.equal(adapter.requests.size, 0);
}

/** 配置请求即使连在一个包中到达，也必须先完成断点安装再启动。 */
async function ordering() {
  const adapter = new Adapter('unused', new PassThrough(), new PassThrough());
  adapter.send = () => {};
  adapter.state = 'configuring';
  const calls = [];
  let installed;
  adapter.gdb = {
    command(command) {
      calls.push(command);
      if (command.startsWith('-break-insert')) return new Promise((resolve) => { installed = resolve; });
      return Promise.resolve({});
    },
    fail() {}, process: { exitCode: 0, signalCode: null },
  };
  adapter.reader.push(Buffer.concat([
    packet(1, 'setBreakpoints', { source: { path: 'source.chl' }, breakpoints: [{ line: 4 }] }),
    packet(2, 'configurationDone'),
  ]));
  await tick();
  assert.equal(calls.length, 1);
  installed({ bkpt: { number: '1', line: '4' } });
  await adapter.control;
  assert.equal(calls[1], '-exec-run');
  await adapter.shutdown();
}

/** 信号栈查询迟到时不能向已关闭或已切换的会话发送停点。 */
async function signals() {
  const adapter = new Adapter('unused', new PassThrough(), new PassThrough());
  const events = [];
  adapter.event = (name, body) => events.push({ name, body });
  let resolveFrames;
  adapter.gdb = { command: () => new Promise(resolve => { resolveFrames = resolve; }) };
  const stale = adapter.reportSignal({ 'thread-id': '1', 'signal-name': 'SIGILL' });
  ++adapter.stopSerial;
  adapter.invalidate();
  resolveFrames({ stack: [{ frame: { fullname: 'stale.chl', line: '2' } }] });
  await stale;
  assert.deepEqual(events, []);
  adapter.gdb = { command: async () => ({ stack: [{ frame: { fullname: 'main.chl', line: '7' } }] }) };
  await adapter.reportSignal({ 'thread-id': '2', 'signal-name': 'SIGILL' });
  const stop = events.find(event => event.name === 'stopped');
  assert.equal(stop.body.threadId, 2);
  assert.match(stop.body.text, /main.chl:7/);
}

async function main() {
  framing(); await mi(); await session(); await ordering(); await signals();
  console.log('DAP/MI：消息预算、分片、迟到响应、取消、暂停代次及幂等清理通过');
}
main().catch((error) => { console.error(error); process.exitCode = 1; });
