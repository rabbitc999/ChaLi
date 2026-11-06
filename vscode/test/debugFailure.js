'use strict';

/** 真实 GDB 验证内部代码步进过滤、panic／断言／受检查错误定位及 Result::Err 不视为异常。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { spawnSync } = require('node:child_process');
const { DebugClient } = require('./debugClient');
const root = path.resolve(__dirname, '../..');
const compiler = process.env.CHALI_BIN || path.join(root, 'chali.exe');
const work = fs.mkdtempSync(path.join(root, 'tmp/debug-failure-'));

/** 以调试模式构建一个独立程序并返回源码路径与可执行文件路径。 */
function build(name, lines, assembly) {
  const directory = path.join(work, name);
  fs.mkdirSync(directory, { recursive: true });
  const source = path.join(directory, 'main.chl');
  fs.writeFileSync(source, lines.join('\n') + '\n');
  const extra = [];
  if (assembly) {
    const native = path.join(directory, 'signal.s');
    fs.writeFileSync(native, assembly);
    const object = path.join(directory, 'signal.o');
    const assembled = spawnSync(compiler, ['asm', native, '--out', object], { cwd: root, env: process.env, encoding: 'utf8', windowsHide: true, timeout: 180000 });
    assert.equal(assembled.status, 0, assembled.stdout + assembled.stderr);
    extra.push('--link', object);
  }
  const built = spawnSync(compiler, ['build', source, '--debug', '--checked', '--allocator', 'malloc', '--out', path.join(directory, 'program'), ...extra], { cwd: root, env: process.env, encoding: 'utf8', windowsHide: true, timeout: 180000 });
  assert.equal(built.status, 0, built.stdout + built.stderr);
  return { source, executable: path.join(directory, 'program' + (process.platform === 'win32' ? '.exe' : '')) };
}

/** 启动一次会话并停在配置阶段。 */
async function start(target) {
  const client = new DebugClient(root, compiler);
  const capabilities = await client.request('initialize', { adapterID: 'chali' });
  assert(capabilities.exceptionBreakpointFilters.some(filter => filter.filter === 'runtime' && filter.default));
  await client.request('launch', { executable: target.executable, cwd: root });
  await client.event('initialized');
  return client;
}

/** 顶层帧的名字、源码与行号。 */
async function top(client, thread) {
  const stack = await client.request('stackTrace', { threadId: thread });
  return stack.stackFrames[0];
}

async function main() {
  // 单步进入内部标准模块时自动回到用户代码；用户函数仍按单步停住。
  const stepping = build('stepping', [
    'int64 add(int64 a, int64 b) { return a + b; }',
    'void main() {',
    '    int64 x = 1;',
    '    println("{}", x);',
    '    int64 y = add(2, 3);',
    '    println("{}", y);',
    '}',
  ]);
  const stepClient = await start(stepping);
  try {
    await stepClient.request('setBreakpoints', { source: { path: stepping.source }, breakpoints: [{ line: 4 }] });
    await stepClient.request('configurationDone');
    let stop = await stepClient.event('stopped');
    await stepClient.request('stepIn', { threadId: stop.threadId });
    stop = await stepClient.event('stopped');
    let frame = await top(stepClient, stop.threadId);
    assert.equal(frame.source?.path, stepping.source, '内部代码不能成为单步停点：' + JSON.stringify(frame));
    assert.equal(frame.line, 5);
    await stepClient.request('stepIn', { threadId: stop.threadId });
    stop = await stepClient.event('stopped');
    frame = await top(stepClient, stop.threadId);
    assert.match(frame.name, /add/);
    assert.equal(frame.source?.path, stepping.source);
    await stepClient.request('continue', { threadId: stop.threadId });
    await stepClient.event('terminated');
  } finally { await stepClient.close(); }

  // panic／断言／受检查错误都停在失败调用现场，并给出消息与用户源码位置。
  for (const [name, lines, match] of [
    ['panic', ['void main(){ panic("boom"); }'], /boom/],
    ['assert', ['void main(){ assert(false, "bad"); }'], /bad/],
    ['bounds', ['void main(){ int64 values[]=new[2]; values[0]=1; int64 keep=values[5]; println("{}", keep); }'], /下标越界/],
  ]) {
    const target = build(name, lines);
    const client = await start(target);
    try {
      await client.request('setExceptionBreakpoints', { filters: [] });
      await client.request('setExceptionBreakpoints', { filters: ['runtime'] });
      await assert.rejects(client.request('setExceptionBreakpoints', { filters: ['unknown'] }), /过滤器/);
      await assert.rejects(client.request('setExceptionBreakpoints', { filters: ['runtime'], filterOptions: [{ filterId: 'runtime', condition: 'false' }] }), /条件/);
      await client.request('configurationDone');
      const stop = await client.event('stopped');
      assert.equal(stop.reason, 'exception', name + '：' + JSON.stringify(stop));
      assert.match(stop.text || '', match, name + '：' + JSON.stringify(stop));
      assert.match(stop.text || '', new RegExp(target.source.replace(/[.*+?^${}()|[\]\\]/g, '\\$&') + ':1'), name + '：' + JSON.stringify(stop));
      const stack = await client.request('stackTrace', { threadId: stop.threadId });
      assert(stack.stackFrames.some((item) => item.source?.path === target.source && item.line === 1), name + ' 调用栈缺少用户源码帧：' + JSON.stringify(stack.stackFrames));
      await client.request('continue', { threadId: stop.threadId });
      await client.event('terminated');
    } finally { await client.close(); }
  }

  // 关闭运行期过滤器后正常走失败退出；重启也必须保留关闭状态。
  const filteredTarget = build('filtered', ['void main(){ panic("filtered boom"); }']);
  for (const restart of [false, true]) {
    const client = await start(filteredTarget);
    try {
      await client.request('setExceptionBreakpoints', { filters: [] });
      await client.request(restart ? 'restart' : 'configurationDone');
      await client.event('terminated');
      assert(!client.messages.some(message => message.event === 'stopped'), '关闭过滤器仍产生停点');
      const exited = client.messages.find(message => message.event === 'exited');
      assert(exited && exited.body.exitCode !== 0, '过滤器不应改变原有失败退出');
    } finally { await client.close(); }
  }

  // 无源码汇编中的非法指令触发真实机器异常，关联用户调用行。
  const signalTarget = build('signal', ['#abi("C","@link","debug_signal") void raiseSignal();', 'void main(){ raiseSignal(); }'],
    '.text\n.globl debug_signal\ndebug_signal:\n' + (process.arch === 'arm64' ? '.inst 0x00000000\n' : 'ud2\n'));
  const signalClient = await start(signalTarget);
  try {
    await signalClient.request('setExceptionBreakpoints', { filters: [] });
    await signalClient.request('configurationDone');
    const stop = await signalClient.event('stopped', 15000).catch(error => { throw new Error(error.message + '\n' + JSON.stringify(signalClient.messages)); });
    assert.equal(stop.reason, 'exception');
    assert.match(stop.text || '', /SIG/);
    assert((stop.text || '').includes(signalTarget.source + ':2'), JSON.stringify(stop));
  } finally { await signalClient.close(); }

  // Result::Err 是普通值，不产生异常停点，程序正常结束。
  const resultTarget = build('result', [
    'Result<int64, int64> fail() { return Result<int64, int64>::Err(3); }',
    'void main() { Result<int64, int64> value = fail(); println("done"); }',
  ]);
  const resultClient = await start(resultTarget);
  try {
    await resultClient.request('configurationDone');
    const exited = await resultClient.event('exited');
    assert.equal(exited.exitCode, 0);
    assert(!resultClient.messages.some((message) => message.event === 'stopped' && message.body?.reason === 'exception'));
    await resultClient.event('terminated');
  } finally { await resultClient.close(); }

  console.log('原生调试失败定位：内部代码步进过滤、panic／断言／越界消息与源码位置、Result::Err 不作异常通过');
}
main().catch((error) => { console.error(error); process.exitCode = 1; });
