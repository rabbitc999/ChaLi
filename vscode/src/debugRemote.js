'use strict';

/** 连接已由用户启动的 Linux gdbserver，并核对暂停进程的实际映像。 */
const fs = require('node:fs');
const path = require('node:path');
const net = require('node:net');
const { imageHash, processId } = require('./debugIdentity');

/** 只允许明确的 TCP 主机和端口，不把地址解释成 GDB 设备、管道或命令。 */
function remoteEndpoint(value) {
  if (value === undefined) return null;
  if (!value || typeof value !== 'object' || Array.isArray(value)
      || Object.keys(value).some(key => !['host', 'port'].includes(key))) throw new Error('remote 必须只包含 host 和 port');
  const { host, port } = value;
  if (typeof host !== 'string' || host.length > 253 || !(net.isIP(host)
      || /^(?=.{1,253}$)[a-z0-9](?:[a-z0-9.-]*[a-z0-9])?$/i.test(host))
      || host.includes('..')) throw new Error('remote.host 必须是 IP 地址或主机名');
  if (!Number.isInteger(port) || port < 1 || port > 65535) throw new Error('remote.port 必须在 1 到 65535 之间');
  return (net.isIP(host) === 6 ? '[' + host + ']' : host) + ':' + port;
}

/** 只接受已有的 64 位小端 ELF 调试产物，避免以宿主格式猜测远程 ABI。 */
async function remoteImage(executable, signal) {
  const file = await fs.promises.open(executable, 'r');
  const header = Buffer.alloc(20);
  try { if ((await file.read(header, 0, header.length, 0)).bytesRead !== header.length) throw new Error('远程符号映像过短'); }
  finally { await file.close(); }
  if (!header.subarray(0, 4).equals(Buffer.from([127, 69, 76, 70])) || header[4] !== 2 || header[5] !== 1
      || ![0, 3].includes(header[7]) || ![2, 3].includes(header.readUInt16LE(16))
      || ![62, 183].includes(header.readUInt16LE(18))) throw new Error('远程调试要求 x64 或 ARM64 的 64 位小端 Linux ELF 产物');
  return { sha256: await imageHash(executable, signal), argument: header.readUInt16LE(18) === 183 ? '$x0' : '$rdi' };
}

/** 从暂停且 PID 已核对的 gdbserver 读取真实 exe；失败时禁止查看并由调用者断开。 */
async function verifyRemoteImage(gdb, pid, executable, expected, directory, signal) {
  processId(pid);
  const destination = path.join(directory, 'remote-image');
  const command = 'remote get /proc/' + pid + '/exe ' + JSON.stringify(destination.replace(/\\/g, '/'));
  await gdb.command('-interpreter-exec console ' + JSON.stringify(command), { signal, timeoutMs: 60000 });
  if (await imageHash(destination, signal) !== expected.sha256 || await imageHash(executable, signal) !== expected.sha256)
    throw new Error('远程目标映像与所选符号产物身份不匹配');
}

module.exports = { remoteEndpoint, remoteImage, verifyRemoteImage };
