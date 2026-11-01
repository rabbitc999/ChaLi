'use strict';

/** 真实 DAP 集成测试共用客户端，失败时回收适配器进程树。 */
const path = require('node:path');
const { spawn } = require('node:child_process');
const { stopProcessTree } = require('../src/debugProtocol');

class DebugClient {
  constructor(root, compiler) {
    this.child = spawn(process.execPath, [path.join(root, 'vscode/src/nativeDebugAdapter.js'), compiler], { cwd: root, env: process.env, windowsHide: true, detached: process.platform !== 'win32' });
    this.sequence = 1; this.buffer = Buffer.alloc(0); this.stderr = '';
    this.pending = new Map(); this.events = []; this.waiters = []; this.messages = [];
    this.child.stderr.on('data', (chunk) => { this.stderr += chunk; });
    this.child.stdout.on('data', (chunk) => this.receive(chunk));
    this.child.on('error', (error) => this.fail(error));
    this.child.on('exit', () => this.fail(new Error('DAP 适配器已退出：' + this.stderr)));
  }
  /** 解析响应和事件，事件先到时保留以供后续等待。 */
  receive(chunk) {
    this.buffer = Buffer.concat([this.buffer, chunk]);
    while (true) {
      const end = this.buffer.indexOf('\r\n\r\n');
      if (end < 0) return;
      const match = /Content-Length:\s*(\d+)/i.exec(this.buffer.subarray(0, end).toString());
      if (!match) return this.fail(new Error('DAP 响应没有长度'));
      const size = Number(match[1]);
      if (this.buffer.length < end + 4 + size) return;
      const message = JSON.parse(this.buffer.subarray(end + 4, end + 4 + size).toString());
      this.buffer = this.buffer.subarray(end + 4 + size); this.messages.push(message);
      if (message.type === 'response') {
        const task = this.pending.get(message.request_seq);
        if (task) {
          this.pending.delete(message.request_seq); clearTimeout(task.timer);
          message.success ? task.resolve(message.body || {}) : task.reject(new Error(message.message));
        }
      } else if (message.type === 'event') {
        const at = this.waiters.findIndex((task) => task.event === message.event);
        if (at < 0) this.events.push(message);
        else { const [task] = this.waiters.splice(at, 1); clearTimeout(task.timer); task.resolve(message.body); }
      }
    }
  }
  /** 发送请求，并保留 requestId 供取消验收使用。 */
  request(command, args = {}, timeout = 180000) {
    const seq = this.sequence++;
    const data = Buffer.from(JSON.stringify({ seq, type: 'request', command, arguments: args }));
    const result = new Promise((resolve, reject) => {
      const timer = setTimeout(() => { this.pending.delete(seq); reject(new Error('DAP 请求超时：' + command + '\n' + this.stderr)); }, timeout);
      this.pending.set(seq, { resolve, reject, timer });
      this.child.stdin.write(`Content-Length: ${data.length}\r\n\r\n`); this.child.stdin.write(data);
    });
    result.requestId = seq;
    return result;
  }
  /** 等待指定协议事件。 */
  event(name, timeout = 180000) {
    const index = this.events.findIndex((message) => message.event === name);
    if (index >= 0) return Promise.resolve(this.events.splice(index, 1)[0].body);
    return new Promise((resolve, reject) => {
      const task = { event: name, resolve, reject };
      task.timer = setTimeout(() => { this.waiters = this.waiters.filter((entry) => entry !== task); reject(new Error('DAP 事件超时：' + name)); }, timeout);
      this.waiters.push(task);
    });
  }
  /** 子进程退出后终结全部等待，避免测试计时器留存。 */
  fail(error) {
    for (const task of this.pending.values()) { clearTimeout(task.timer); task.reject(error); }
    for (const task of this.waiters) { clearTimeout(task.timer); task.reject(error); }
    this.pending.clear(); this.waiters = [];
  }
  /** 先发送关闭请求；超时仍回收测试拥有的适配器进程树。 */
  async close() {
    if (this.child.exitCode !== null || this.child.signalCode !== null) return;
    await this.request('disconnect', {}, 5000).catch(() => {});
    this.child.stdin.end();
    await new Promise((resolve) => {
      if (this.child.exitCode !== null || this.child.signalCode !== null) return resolve();
      const timer = setTimeout(() => { void stopProcessTree(this.child).finally(resolve); }, 3000);
      this.child.once('exit', () => { clearTimeout(timer); resolve(); });
    });
  }
}

module.exports = { DebugClient };
