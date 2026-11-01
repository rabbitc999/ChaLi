'use strict';

/** 有界 DAP 消息读取与原生调试进程清理。 */
const { spawn } = require('node:child_process');
const HEADER_LIMIT = 8192;
const BODY_LIMIT = 16 * 1024 * 1024;
const stopping = new WeakMap();

/** 按字节解析分片或连续的 DAP 请求；错误后不再接受输入。 */
class RequestReader {
  constructor(receive, failure) {
    this.receive = receive;
    this.failure = failure;
    this.buffer = Buffer.alloc(0);
    this.failed = false;
  }
  /** 校验请求封装和长度，不把非法消息交给会话状态机。 */
  push(chunk) {
    if (this.failed) return;
    try {
      this.buffer = Buffer.concat([this.buffer, chunk]);
      while (this.buffer.length) {
        const end = this.buffer.indexOf('\r\n\r\n');
        if (end < 0) {
          if (this.buffer.length > HEADER_LIMIT) throw new Error('DAP 消息头超过限制');
          return;
        }
        if (end > HEADER_LIMIT) throw new Error('DAP 消息头超过限制');
        const fields = this.buffer.subarray(0, end).toString('ascii').split('\r\n');
        const lengths = fields.filter((line) => /^Content-Length:/i.test(line));
        if (lengths.length !== 1 || !/^Content-Length:\s*[0-9]+\s*$/i.test(lengths[0])) throw new Error('DAP Content-Length 无效');
        const size = Number(lengths[0].split(':')[1].trim());
        if (!Number.isSafeInteger(size) || size <= 0 || size > BODY_LIMIT) throw new Error('DAP 消息长度超过限制或为零');
        if (this.buffer.length < end + 4 + size) return;
        const request = JSON.parse(this.buffer.subarray(end + 4, end + 4 + size).toString('utf8'));
        this.buffer = this.buffer.subarray(end + 4 + size);
        if (!request || request.type !== 'request' || !Number.isSafeInteger(request.seq) || request.seq < 1 || typeof request.command !== 'string') throw new Error('DAP 请求封装无效');
        this.receive(request);
      }
    } catch (error) {
      this.failed = true;
      this.buffer = Buffer.alloc(0);
      this.failure(error);
    }
  }
  /** 输入结束时报告尚未完成的封装，不把截断消息当作正常断开。 */
  finish() {
    if (!this.failed && this.buffer.length) {
      this.failed = true;
      this.buffer = Buffer.alloc(0);
      this.failure(new Error('DAP 消息被截断'));
    }
  }
}

/** 等待属于本会话的进程树停止；已退出的根不再按旧 PID 执行清理。 */
function stopProcessTree(child) {
  if (!child) return Promise.resolve();
  if (stopping.has(child)) return stopping.get(child);
  const operation = stopProcessTreeNow(child);
  stopping.set(child, operation);
  return operation;
}

/** 对同一个子进程只执行一次系统清理，防止并发取消重复使用旧 PID。 */
async function stopProcessTreeNow(child) {
  if (!child || child.exitCode !== null || child.signalCode !== null || !child.pid) return;
  if (process.platform === 'win32') {
    await new Promise((resolve) => {
      const killer = spawn('taskkill', ['/PID', String(child.pid), '/T', '/F'], { windowsHide: true, stdio: 'ignore' });
      killer.once('error', () => { child.kill(); resolve(); });
      killer.once('exit', resolve);
    });
  } else {
    try { process.kill(-child.pid, 'SIGKILL'); } catch { child.kill('SIGKILL'); }
  }
  if (child.exitCode !== null || child.signalCode !== null) return;
  await new Promise((resolve, reject) => {
    const timer = setTimeout(() => reject(new Error('原生调试子进程未能退出：' + child.pid)), 2000);
    child.once('exit', () => { clearTimeout(timer); resolve(); });
  });
}

module.exports = { RequestReader, stopProcessTree, HEADER_LIMIT, BODY_LIMIT };
