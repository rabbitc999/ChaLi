'use strict';

/** 会话专属标准流：Windows 命名管道或 Linux 本地套接字，不接收调试控制命令。 */
const net = require('node:net');
const path = require('node:path');
const crypto = require('node:crypto');

class DebugTerminal {
  constructor(directory, output) {
    const [major, minor] = process.versions.node.split('.').map(Number);
    const abstract = process.platform === 'linux' && (major > 20 || (major === 20 && minor >= 8));
    const name = 'chali-debug-' + crypto.randomBytes(16).toString('hex');
    this.endpoint = process.platform === 'win32' ? '\\\\.\\pipe\\' + name : abstract ? '@' + name : path.join(directory, 'console.sock');
    this.output = output;
    this.servers = ['stdin', 'stdout'].map(direction => {
      let connected = false;
      return net.createServer(socket => {
        if (connected || this.closed) { socket.destroy(); return; }
        connected = true;
        if (direction === 'stdin') this.socket = socket;
        else this.outputSocket = socket;
        socket.setEncoding('utf8');
        socket.on('data', text => { if (direction === 'stdout') output(text); });
        socket.on('error', error => output('\r\n终端通道错误：' + error.message + '\r\n'));
        socket.on('close', () => { if (this.socket === socket) this.socket = null; });
      });
    });
  }
  /** 建立监听后才允许被调试程序执行用户代码。 */
  async listen() {
    try {
      await Promise.all(this.servers.map((server, index) => new Promise((resolve, reject) => {
        server.once('error', reject);
        const endpoint = this.endpoint + (index ? '-stdout' : '-stdin');
        server.listen(endpoint.startsWith('@') ? '\0' + endpoint.slice(1) : endpoint, () => { server.removeListener('error', reject); resolve(); });
      })));
    } catch (error) { this.close(); throw error; }
    for (const server of this.servers) server.on('error', error => this.output('\r\n终端监听错误：' + error.message + '\r\n'));
  }
  /** 输入受单次大小及写入排队预算约束；EOF 只关闭输入半边。 */
  input(text, end = false) {
    if (!this.socket || this.closed) throw new Error('程序尚未连接终端，或终端已经关闭');
    if (typeof text !== 'string' || Buffer.byteLength(text) > 65536) throw new Error('终端输入超过限制');
    if (this.socket.writableEnded || this.socket.writableLength > 262144) throw new Error('终端输入已结束或等待写入');
    this.socket.write(text);
    if (end) {
      const socket = this.socket;
      socket.end(() => socket.destroy());
    }
  }
  /** 结束会话时回收本会话连接和监听，不保留重启前的输入。 */
  async drain() {
    const socket = this.outputSocket;
    if (!socket || socket.destroyed || socket.readableEnded) return;
    await new Promise(resolve => {
      const done = () => { clearTimeout(timer); socket.removeListener('end', done); socket.removeListener('close', done); resolve(); };
      const timer = setTimeout(done, 250);
      socket.once('end', done); socket.once('close', done);
    });
  }
  /** 立即关闭本会话拥有的两个方向及监听器。 */
  close() {
    this.closed = true;
    this.socket?.destroy(); this.socket = null;
    this.outputSocket?.destroy(); this.outputSocket = null;
    for (const server of this.servers) server.close();
  }
}

module.exports = { DebugTerminal };
