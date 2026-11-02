'use strict';

/** 本机附加前后核对进程创建身份和映像真实字节；不向目标调用函数或写入数据。 */
const fs = require('node:fs');
const path = require('node:path');
const crypto = require('node:crypto');
const { spawn } = require('node:child_process');
const { stopProcessTree } = require('./debugProtocol');
const MAX_IMAGE = 512 * 1024 * 1024;

/** 解析明确提供的本机 PID，不接受表达式、零或命令片段。 */
function processId(value) {
  if (typeof value === 'string' && /^[1-9][0-9]*$/.test(value)) value = Number(value);
  if (!Number.isSafeInteger(value) || value < 1 || value > 2147483647) throw new Error('processId 必须是有效的正整数 PID');
  return value;
}

/** 对已打开的普通映像流式核对，拒绝读取途中变化和超出预算的文件。 */
async function imageHash(file, signal) {
  const handle = await fs.promises.open(file, 'r');
  try {
    const before = await handle.stat({ bigint: true });
    if (!before.isFile() || before.size <= 0n || before.size > BigInt(MAX_IMAGE)) throw new Error('目标映像不是预算内的普通文件');
    const hash = crypto.createHash('sha256'); let size = 0;
    for await (const bytes of handle.createReadStream({ autoClose: false, signal })) {
      size += bytes.length;
      if (size > MAX_IMAGE) throw new Error('目标映像超过身份核对预算');
      hash.update(bytes);
    }
    const after = await handle.stat({ bigint: true });
    if (BigInt(size) !== before.size || before.size !== after.size || before.mtimeNs !== after.mtimeNs || before.ctimeNs !== after.ctimeNs) throw new Error('身份核对期间映像已改变');
    return hash.digest('hex');
  } finally { await handle.close(); }
}

const WINDOWS_IDENTITY = String.raw`
$ErrorActionPreference = 'Stop'
[Console]::OutputEncoding = [System.Text.UTF8Encoding]::new($false)
$chaliIdentityProcess = [System.Diagnostics.Process]::GetProcessById([int]$env:CHALI_IDENTITY_PID)
try {
  $null = $chaliIdentityProcess.Handle
  $chaliIdentity = [pscustomobject]@{
    pid = $chaliIdentityProcess.Id
    image = $chaliIdentityProcess.MainModule.FileName
    start = $chaliIdentityProcess.StartTime.ToFileTimeUtc().ToString([System.Globalization.CultureInfo]::InvariantCulture)
  }
  if ($chaliIdentityProcess.HasExited) { throw '目标进程已经退出' }
  $chaliIdentity | ConvertTo-Json -Compress
} finally { $chaliIdentityProcess.Dispose() }
`;

/** Windows 查询只运行固定宿主脚本，进程句柄覆盖一次原子身份采集。 */
async function windowsIdentity(pid, environment, signal, started) {
  if (signal?.aborted) throw new Error('附加身份核对已取消');
  const temporary = environment?.TMPDIR;
  if (!temporary || environment.TMP !== temporary || environment.TEMP !== temporary) throw new Error('身份核对缺少统一临时目录');
  const powershell = path.join(process.env.SystemRoot || 'C:\\Windows', 'System32/WindowsPowerShell/v1.0/powershell.exe');
  const child = spawn(powershell, ['-NoProfile', '-NonInteractive', '-Command', WINDOWS_IDENTITY], { windowsHide: true,
    env: { ...process.env, TMPDIR: temporary, TMP: temporary, TEMP: temporary, CHALI_IDENTITY_PID: String(pid) } });
  started?.(child);
  let failure, output = '';
  const stop = error => { failure ||= error; void stopProcessTree(child).catch(error => { failure ||= error; }); };
  const abort = () => stop(new Error('附加身份核对已取消'));
  const collect = data => { if (output.length + data.length > 65536) stop(new Error('进程身份响应超过预算')); else output += data.toString('utf8'); };
  child.stdout.on('data', collect); child.stderr.on('data', collect);
  signal?.addEventListener('abort', abort, { once: true });
  const timer = setTimeout(() => stop(new Error('进程身份查询超时')), 10000);
  try {
    const status = await new Promise((resolve, reject) => { child.once('error', reject); child.once('close', resolve); });
    if (failure) throw failure;
    if (status !== 0) throw new Error('无法核对目标进程：' + output.trim().slice(0, 4096));
    const result = JSON.parse(output.trim().replace(/^\uFEFF/, ''));
    if (result.pid !== pid || typeof result.image !== 'string' || !result.image || !/^[0-9]+$/.test(result.start)) throw new Error('目标进程身份响应无效');
    return result;
  } finally {
    clearTimeout(timer); signal?.removeEventListener('abort', abort); await stopProcessTree(child);
  }
}

/** Linux 保留 proc 目录描述符，使 PID 退出后不能通过同名新目录替代原进程。 */
async function linuxIdentity(directory, pid) {
  const base = '/proc/self/fd/' + directory.fd;
  const text = await fs.promises.readFile(base + '/stat', 'utf8');
  const end = text.lastIndexOf(')'), fields = text.slice(end + 2).trim().split(/\s+/);
  if (end < 0 || Number(text.slice(0, text.indexOf(' '))) !== pid || !/^[0-9]+$/.test(fields[19] || '')) throw new Error('Linux 进程创建身份无效');
  return { pid, start: fields[19], image: await fs.promises.readlink(base + '/exe'), hashPath: base + '/exe' };
}

/** 创建附加身份租约；准备失败保持目标运行，附加后再次核对创建身份和两份映像。 */
async function prepareLocalIdentity(value, executable, environment, signal, started) {
  const pid = processId(value), selected = await fs.promises.realpath(executable);
  const expected = await imageHash(selected, signal); let directory;
  try {
    if (process.platform === 'linux') directory = await fs.promises.open('/proc/' + pid, fs.constants.O_RDONLY | fs.constants.O_DIRECTORY);
    else if (process.platform !== 'win32') throw new Error('当前宿主不支持本机附加身份核对');
    const read = () => process.platform === 'win32' ? windowsIdentity(pid, environment, signal, started) : linuxIdentity(directory, pid);
    const identity = await read();
    if (await imageHash(identity.hashPath || identity.image, signal) !== expected) throw new Error('目标映像与所选符号产物身份不匹配');
    return { ...identity, sha256: expected, async verify() {
      const current = await read();
      if (current.start !== identity.start || current.image !== identity.image) throw new Error('附加期间进程身份已改变，可能已退出或 PID 被重用');
      if (await imageHash(current.hashPath || current.image, signal) !== expected || await imageHash(selected, signal) !== expected) throw new Error('附加期间目标或符号产物映像已改变');
    }, async release() { await directory?.close(); directory = null; } };
  } catch (error) { await directory?.close(); throw error; }
}

module.exports = { processId, imageHash, prepareLocalIdentity };
