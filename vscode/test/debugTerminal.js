'use strict';

/** 核对真实双向终端、EOF、关闭回收，以及旧 Node 的文件端点兼容性。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const net = require('node:net');
const vm = require('node:vm');
const { once } = require('node:events');
const { DebugTerminal } = require('../src/debugTerminal');
const root = path.resolve(__dirname, '../..');
const directory = fs.mkdtempSync(path.join(root, 'tmp/debug-terminal-'));

/** 等待异步输出，超时保留实际结果。 */
async function until(predicate) {
  for (let attempt = 0; attempt < 500; ++attempt) {
    if (predicate()) return;
    await new Promise(resolve => setTimeout(resolve, 10));
  }
  assert.fail('终端通道等待超时');
}

/** 在不启动监听的情况下核对旧宿主选择，不更改真实 process。 */
function endpointFor(platform, nodeVersion) {
  const module = { exports: {} };
  vm.runInNewContext(fs.readFileSync(path.join(root, 'vscode/src/debugTerminal.js'), 'utf8'), {
    require, module, process: { platform, versions: { node: nodeVersion } }, Buffer, setTimeout, clearTimeout
  });
  return new module.exports.DebugTerminal(directory, () => {}).endpoint;
}

/** 等两个监听器实际关闭，避免退出时遗留会话。 */
async function closeTerminal(terminal) {
  const closed = terminal.servers.map(server => once(server, 'close'));
  terminal.close();
  await Promise.all(closed);
}

/** 使用平台实际支持的端点传输 UTF-8，并核对会话隔离和 EOF。 */
async function main() {
  for (const version of ['16.20.2', '18.20.0', '20.7.0']) {
    assert.equal(endpointFor('linux', version), path.join(directory, 'console.sock'));
  }
  for (const version of ['20.8.0', '22.23.4', '26.5.0']) {
    assert.match(endpointFor('linux', version), /^@chali-debug-[0-9a-f]{32}$/);
  }
  assert.match(endpointFor('win32', '16.20.2'), /^\\\\\.\\pipe\\chali-debug-[0-9a-f]{32}$/);
  assert.equal(endpointFor('darwin', '22.23.4'), path.join(directory, 'console.sock'));
  let output = '', received = '';
  const terminal = new DebugTerminal(directory, text => output += text);
  const sockets = [];
  try {
    assert.throws(() => terminal.input('before connect'), /尚未连接/);
    await terminal.listen();
    const endpoint = terminal.endpoint.startsWith('@') ? '\0' + terminal.endpoint.slice(1) : terminal.endpoint;
    const input = net.createConnection(endpoint + '-stdin');
    const outgoing = net.createConnection(endpoint + '-stdout');
    sockets.push(input, outgoing);
    input.setEncoding('utf8');
    input.on('data', text => received += text);
    await Promise.all([once(input, 'connect'), once(outgoing, 'connect')]);
    await until(() => terminal.socket && terminal.outputSocket);
    assert.throws(() => terminal.input('x'.repeat(65537)), /限制/);
    outgoing.end('程序输出🙂\n');
    await until(() => output === '程序输出🙂\n');
    const ended = once(input, 'end');
    terminal.input('输入你好🙂\n', true);
    await ended;
    assert.equal(received, '输入你好🙂\n');
    await terminal.drain();
    for (const socket of sockets) socket.destroy();
    await closeTerminal(terminal);
    assert.throws(() => terminal.input('closed'), /尚未连接|关闭/);
    const next = new DebugTerminal(directory, () => {});
    if (terminal.endpoint.startsWith('@') || process.platform === 'win32') assert.notEqual(next.endpoint, terminal.endpoint);
    await next.listen();
    await closeTerminal(next);
    console.log('真实终端：双通道 UTF-8、EOF、输入预算、关闭与重建、旧 Node 文件端点及 Linux 抽象端点通过');
  } finally {
    for (const socket of sockets) socket.destroy();
    if (!terminal.closed) terminal.close();
  }
}

main().catch(error => { console.error(error); process.exitCode = 1; });
