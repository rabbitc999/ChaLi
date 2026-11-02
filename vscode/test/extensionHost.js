'use strict';

/** 在真实 VSCode API 与扩展宿主中检查原生调试、异常、取消和会话结束。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { spawnSync } = require('node:child_process');

/** 只记录当前扩展宿主下的 GDB、被调试程序及适配器，不触碰其他会话。 */
function debugProcesses() {
  const windows = process.platform === 'win32';
  const result = spawnSync(windows ? 'powershell.exe' : 'ps', windows ? ['-NoProfile', '-Command',
    'Get-CimInstance Win32_Process | Select-Object ProcessId,ParentProcessId,Name | ConvertTo-Json -Compress'] : ['-eo', 'pid=,ppid=,comm='],
  { encoding: 'utf8', windowsHide: true, timeout: 10000 });
  assert.equal(result.status, 0, result.stderr);
  const rows = windows ? JSON.parse(result.stdout) : result.stdout.trim().split('\n').map(line => {
    const match = /^\s*(\d+)\s+(\d+)\s+(.+)$/.exec(line); assert(match, line);
    return { ProcessId: Number(match[1]), ParentProcessId: Number(match[2]), Name: path.basename(match[3]) };
  });
  const byId = new Map(rows.map(row => [row.ProcessId, row]));
  const belongs = id => {
    const seen = new Set();
    while (id && !seen.has(id)) {
      if (id === process.pid) return true;
      seen.add(id); id = byId.get(id)?.ParentProcessId;
    }
    return false;
  };
  const debuggerRow = rows.find(row => /^gdb(?:\.exe)?$/i.test(row.Name) && belongs(row.ProcessId));
  assert(debuggerRow, '当前扩展宿主下必须存在真实 GDB');
  const pids = new Set([debuggerRow.ProcessId, debuggerRow.ParentProcessId]);
  for (const row of rows) if (row.ParentProcessId === debuggerRow.ProcessId) pids.add(row.ProcessId);
  assert(pids.size >= 3, '暂停时必须能观察到适配器、GDB 和被调试进程');
  return [...pids];
}

/** 等待本场景记录的进程自然退出；只查询存活状态，不按旧 PID 强制终止。 */
async function exited(pids) {
  const alive = pid => { try { process.kill(pid, 0); return true; } catch (error) { if (error.code !== 'ESRCH') throw error; return false; } };
  for (let attempt = 0; attempt < 100; ++attempt) {
    if (!pids.some(alive)) return;
    await new Promise(resolve => setTimeout(resolve, 50));
  }
  assert.deepEqual(pids.filter(alive), [], '原生调试进程必须全部退出');
}

/** 对协议事件使用有界等待，结束后释放定时器。 */
function bounded(promise, label) {
  let timer;
  return Promise.race([promise, new Promise((_, reject) => { timer = setTimeout(() => reject(new Error(label + ' 超时')), 60000); })])
    .finally(() => clearTimeout(timer));
}

exports.run = async function () {
  const vscode = require('vscode');
  const work = process.env.CHALI_EXTENSION_TEST_WORK;
  const folder = vscode.workspace.workspaceFolders[0];
  const extension = vscode.extensions.getExtension('rabbitc999.chali-editor');
  assert(extension); await extension.activate(); assert(extension.isActive);
  const commands = await vscode.commands.getCommands(true);
  assert(commands.includes('chali.checkDebugEnvironment'));
  const scenarios = [];
  const tracker = vscode.debug.registerDebugAdapterTrackerFactory('chali', {
    createDebugAdapterTracker(session) {
      const state = { session, messages: [], waiters: [] }; scenarios.push(state);
      return { onDidSendMessage(message) {
        state.messages.push(message);
        const matching = state.waiters.filter(waiter => waiter.event === message.event && message.type === 'event');
        for (const waiter of matching) { state.waiters.splice(state.waiters.indexOf(waiter), 1); waiter.resolve(message.body || {}); }
      } };
    },
  });
  const event = (state, name) => {
    const found = state.messages.find(message => message.type === 'event' && message.event === name);
    return found ? Promise.resolve(found.body || {}) : bounded(new Promise(resolve => state.waiters.push({ event: name, resolve })), name);
  };
  let active;
  try {
    for (const mode of ['values', 'cancel', 'exception']) {
      const source = path.join(folder.uri.fsPath, 'test_' + mode + '.chl');
      fs.writeFileSync(source, mode === 'exception' ? 'void main(){\n panic("extension-host-failure");\n}\n'
        : 'void main(){\n int64 number=42;\n int64 data[]=new[2]; data[0]=73;\n println("{}",number);\n}\n');
      const document = await vscode.workspace.openTextDocument(source);
      assert.equal(document.languageId, 'chali');
      await vscode.window.showTextDocument(document);
      const breakpoint = new vscode.SourceBreakpoint(new vscode.Location(document.uri, new vscode.Position(mode === 'exception' ? 1 : 3, 0)));
      if (mode !== 'exception') vscode.debug.addBreakpoints([breakpoint]);
      let terminatedSubscription;
      const ended = new Promise(resolve => {
        terminatedSubscription = vscode.debug.onDidTerminateDebugSession(session => {
          if (session.name === 'ChaLi extension ' + mode) { terminatedSubscription.dispose(); resolve(); }
        });
      });
      const launched = await vscode.debug.startDebugging(folder, { type: 'chali', request: 'launch', name: 'ChaLi extension ' + mode,
        program: source, cwd: folder.uri.fsPath, console: 'internalConsole' });
      assert.equal(launched, true);
      const state = scenarios.at(-1); assert(state); active = state.session;
      const stop = await event(state, 'stopped');
      state.processes = debugProcesses();
      assert.equal(stop.reason, mode === 'exception' ? 'exception' : 'breakpoint');
      const stack = await active.customRequest('stackTrace', { threadId: stop.threadId });
      assert(stack.stackFrames.some(frame => path.resolve(frame.source?.path || '') === source));
      if (mode === 'values') {
        const scopes = await active.customRequest('scopes', { frameId: stack.stackFrames[0].id });
        const values = await active.customRequest('variables', { variablesReference: scopes.scopes[0].variablesReference });
        const named = Object.fromEntries(values.variables.map(value => [value.name, value]));
        assert.equal(named.number.value, '42');
        const items = await active.customRequest('variables', { variablesReference: named.data.variablesReference, count: 1 });
        assert.equal(items.variables[0].value, '73');
        await active.customRequest('continue', { threadId: stop.threadId });
        assert.equal((await event(state, 'exited')).exitCode, 0);
      } else if (mode === 'exception') {
        assert.match(stop.text, /extension-host-failure/);
        await active.customRequest('continue', { threadId: stop.threadId });
        assert.notEqual((await event(state, 'exited')).exitCode, 0);
      } else {
        await vscode.debug.stopDebugging(active);
      }
      await event(state, 'terminated');
      await bounded(ended, 'VSCode 会话回收');
      await exited(state.processes);
      vscode.debug.removeBreakpoints([breakpoint]);
      active = null;
    }
    fs.writeFileSync(path.join(work, 'result.json'), JSON.stringify({ vscode: vscode.version, extension: extension.packageJSON.version,
      activated: true, scenarios: scenarios.map(state => ({ name: state.session.name,
        stopped: state.messages.filter(message => message.event === 'stopped').length,
        processesExited: state.processes.length,
        terminated: state.messages.some(message => message.event === 'terminated') })) }, null, 2));
  } catch (error) {
    fs.writeFileSync(path.join(work, 'failure.json'), JSON.stringify({ error: error.stack,
      sessions: scenarios.map(state => ({ name: state.session.name, messages: state.messages })) }, null, 2));
    throw error;
  } finally {
    if (active) await vscode.debug.stopDebugging(active);
    tracker.dispose();
    vscode.debug.removeBreakpoints(vscode.debug.breakpoints);
  }
};
