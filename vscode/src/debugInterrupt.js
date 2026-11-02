'use strict';

/** Windows 原生 GDB 的同步等待不处理 MI 中断时，通过已核对的目标句柄请求系统调试中断。 */
const path = require('node:path');
const { spawn } = require('node:child_process');
const { stopProcessTree } = require('./debugProtocol');

const SCRIPT = String.raw`
$ErrorActionPreference = 'Stop'
[Console]::OutputEncoding = [System.Text.UTF8Encoding]::new($false)
Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
public static class ChaLiDebugInterrupt {
  [DllImport("kernel32.dll", SetLastError=true)]
  public static extern bool DebugBreakProcess(IntPtr process);
}
'@
$chaliTargetId = [int]$env:CHALI_INTERRUPT_TARGET
$chaliDebuggerId = [int]$env:CHALI_INTERRUPT_DEBUGGER
$chaliProcess = [System.Diagnostics.Process]::GetProcessById($chaliTargetId)
$chaliAncestors = [System.Collections.Generic.List[System.Diagnostics.Process]]::new()
try {
  $chaliHandle = $chaliProcess.Handle
  $chaliRecord = Get-CimInstance Win32_Process -Filter ('ProcessId=' + $chaliTargetId)
  if (!$chaliRecord) { throw '目标已经退出' }
  if ($env:CHALI_INTERRUPT_START) {
    if ($chaliProcess.StartTime.ToFileTimeUtc().ToString([System.Globalization.CultureInfo]::InvariantCulture) -ne $env:CHALI_INTERRUPT_START) { throw '附加目标创建身份已改变' }
  } else {
    $chaliAncestorId = $chaliRecord.ParentProcessId
    $chaliChild = $chaliProcess
    $chaliMatched = $false
    for ($chaliDepth = 0; $chaliDepth -lt 8 -and $chaliAncestorId -gt 0; ++$chaliDepth) {
      $chaliAncestor = [System.Diagnostics.Process]::GetProcessById($chaliAncestorId)
      $chaliAncestors.Add($chaliAncestor)
      $null = $chaliAncestor.Handle
      if ($chaliAncestor.StartTime.ToFileTimeUtc() -gt $chaliChild.StartTime.ToFileTimeUtc()) { throw '目标祖先进程编号已重用' }
      if ($chaliAncestorId -eq $chaliDebuggerId) { $chaliMatched = $true; break }
      $chaliParent = Get-CimInstance Win32_Process -Filter ('ProcessId=' + $chaliAncestorId)
      if (!$chaliParent) { throw '目标祖先进程已经退出' }
      $chaliChild = $chaliAncestor
      $chaliAncestorId = $chaliParent.ParentProcessId
    }
    if (!$chaliMatched) { throw '目标不属于当前调试器进程链' }
  }
  $chaliActual = [System.IO.Path]::GetFullPath($chaliProcess.MainModule.FileName)
  $chaliExpected = [System.IO.Path]::GetFullPath($env:CHALI_INTERRUPT_IMAGE)
  if (![System.StringComparer]::OrdinalIgnoreCase.Equals($chaliActual, $chaliExpected)) { throw '目标映像身份已改变' }
  if ($chaliProcess.HasExited) { throw '目标已经退出' }
  if (![ChaLiDebugInterrupt]::DebugBreakProcess($chaliHandle)) {
    throw [System.ComponentModel.Win32Exception]::new([System.Runtime.InteropServices.Marshal]::GetLastWin32Error())
  }
} finally { foreach ($chaliAncestor in $chaliAncestors) { $chaliAncestor.Dispose() }; $chaliProcess.Dispose() }
`;

/** 对当前调试器进程链启动且映像相同的目标暂停；保留句柄并核对创建时间，防止 PID 重用。 */
async function interruptWindows(target, debuggerId, executable, environment, signal, started = () => {}, creation = null) {
  if (![target, debuggerId].every(value => Number.isSafeInteger(value) && value > 0) || typeof executable !== 'string' || !executable) throw new Error('缺少可核对的原生调试进程身份');
  if (signal?.aborted) throw new Error('暂停请求已取消');
  if (creation !== null && (typeof creation !== 'string' || !/^[0-9]+$/.test(creation))) throw new Error('附加目标创建身份无效');
  const temporary = environment?.TMPDIR;
  if (typeof temporary !== 'string' || !temporary || environment.TMP !== temporary || environment.TEMP !== temporary) throw new Error('暂停辅助进程缺少统一临时目录');
  const powershell = path.join(process.env.SystemRoot || 'C:\\Windows', 'System32/WindowsPowerShell/v1.0/powershell.exe');
  const child = spawn(powershell, ['-NoProfile', '-NonInteractive', '-Command', SCRIPT], { windowsHide: true,
    env: { ...process.env, TMPDIR: temporary, TMP: temporary, TEMP: temporary,
      CHALI_INTERRUPT_TARGET: String(target), CHALI_INTERRUPT_DEBUGGER: String(debuggerId), CHALI_INTERRUPT_IMAGE: executable,
      CHALI_INTERRUPT_START: creation || '' } });
  started(child);
  let failure, output = '';
  const stop = error => { failure ||= error; void stopProcessTree(child).catch(error => { failure ||= error; }); };
  const abort = () => stop(new Error('暂停请求已取消'));
  const collect = bytes => { if (output.length + bytes.length > 16384) stop(new Error('暂停辅助进程输出超过预算')); else output += bytes.toString('utf8'); };
  child.stdout.on('data', collect); child.stderr.on('data', collect);
  signal?.addEventListener('abort', abort, { once: true });
  const timer = setTimeout(() => stop(new Error('暂停辅助进程超时')), 10000);
  try {
    const status = await new Promise((resolve, reject) => { child.once('error', reject); child.once('close', resolve); });
    if (failure) throw failure;
    if (status !== 0) throw new Error('无法暂停原生目标：' + output.trim().slice(0, 4096));
  } finally {
    clearTimeout(timer); signal?.removeEventListener('abort', abort);
    await stopProcessTree(child);
  }
}

module.exports = { interruptWindows };
