'use strict';

/** 对真实注册函数注入编辑器 API，核对终端重启与 Problems 的转换。 */
const assert = require('node:assert/strict');
const Module = require('node:module');
let trackerFactory, provider, terminalOptions, closed = 0;
const requests = [], emissions = [], diagnostics = [];
const disposable = { dispose() {} };
class Emitter { constructor() { this.event = () => disposable; } fire(text) { emissions.push(text); } dispose() {} }
class Range { constructor(...values) { this.values = values; } }
class Diagnostic { constructor(range, message, severity) { Object.assign(this, { range, message, severity }); } }
const vscode = {
  EventEmitter: Emitter, Range, Diagnostic, DiagnosticSeverity: { Error: 0 }, Uri: { file: path => path },
  languages: { createDiagnosticCollection() { return { ...disposable, clear() { diagnostics.length = 0; }, set(file, values) { diagnostics.push({ file, values }); } }; } },
  debug: {
    registerDebugConfigurationProvider(_type, value) { provider = value; return disposable; },
    registerDebugAdapterTrackerFactory(_type, value) { trackerFactory = value; return disposable; },
    registerDebugAdapterDescriptorFactory: () => disposable, onDidTerminateDebugSession: () => disposable,
    async stopDebugging() { ++closed; },
  },
  window: { createTerminal(options) { terminalOptions = options; return { show() {}, dispose() { options.pty.close(); } }; } },
  commands: { registerCommand() { return disposable; } },
};
const load = Module._load;
Module._load = function(name, ...args) { return name === 'vscode' ? vscode : load.call(this, name, ...args); };
let registerDebugger;
try { ({ registerDebugger } = require('../src/debugger')); } finally { Module._load = load; }
async function main() {
  registerDebugger({ subscriptions: [] }, {});
  assert(provider.provideDebugConfigurations().some(item => item.entry === 'project'));
  const tracker = trackerFactory.createDebugAdapterTracker({ async customRequest(command, arguments_) { requests.push({ command, arguments_ }); } });
  const event = (name, body) => tracker.onDidSendMessage({ type: 'event', event: name, body });
  event('chali/terminal', { name: 'example' });
  terminalOptions.pty.handleInput('ab\x7f中\r');
  assert.deepEqual(requests.pop(), { command: 'chali/input', arguments_: { text: 'a中\n' } });
  terminalOptions.pty.handleInput('\x1b[Aline\r\n');
  assert.deepEqual(requests.pop(), { command: 'chali/input', arguments_: { text: 'line\n' } });
  assert.equal(requests.length, 0);
  terminalOptions.pty.handleInput('\x03'); assert.equal(requests.pop().command, 'pause');
  terminalOptions.pty.handleInput('\x04'); assert.deepEqual(requests.pop().arguments_, { text: '', end: true });
  event('chali/terminalOutput', { output: 'hello\n' }); assert.equal(emissions.at(-1), 'hello\r\n');
  event('chali/terminal', { name: 'restart' }); assert.equal(closed, 0, '重启释放旧终端不能终止新会话');
  terminalOptions.pty.close(); assert.equal(closed, 1);
  event('chali/buildDiagnostics', { diagnostics: [{ path: 'main.chl', line: 2, column: 4, message: 'bad variable' }] });
  assert.equal(diagnostics[0].file, 'main.chl'); assert.deepEqual(diagnostics[0].values[0].range.values, [2, 4, 2, 5]);
  event('chali/buildDiagnostics', { diagnostics: [] }); assert.equal(diagnostics.length, 0);
  tracker.onExit();
  console.log('调试器编辑器集成：终端输入编辑、暂停、EOF、输出、重启与 Problems 转换通过');
}
main().catch(error => { console.error(error); process.exitCode = 1; });
