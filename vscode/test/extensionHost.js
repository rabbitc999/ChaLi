'use strict';

/** 在真实 VSCode API 与扩展宿主中检查原生调试、异常、取消和会话结束。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { spawn, spawnSync } = require('node:child_process');
const { stopProcessTree } = require('../src/debugProtocol');

/** 读取 Windows 进程身份，区分 PID 重用及仍带旧父 PID 的进程。 */
function windowsProcesses() {
  const result = spawnSync('powershell.exe', ['-NoProfile', '-Command',
    'Get-CimInstance Win32_Process | Select-Object ProcessId,ParentProcessId,Name,@{Name="Started";Expression={if ($_.CreationDate) { ([DateTimeOffset]$_.CreationDate).ToUnixTimeMilliseconds() }}} | ConvertTo-Json -Compress'],
  { encoding: 'utf8', windowsHide: true, timeout: 10000 });
  assert.equal(result.status, 0, result.stderr);
  return JSON.parse(result.stdout);
}

/** 只记录当前扩展宿主下的 GDB、被调试程序及适配器，不触碰其他会话。 */
function debugProcesses(attaching = false) {
  const windows = process.platform === 'win32';
  let rows;
  if (windows) {
    rows = windowsProcesses();
  } else {
    const result = spawnSync('ps', ['-eo', 'pid=,ppid=,comm='], { encoding: 'utf8', timeout: 10000 });
    assert.equal(result.status, 0, result.stderr);
    rows = result.stdout.trim().split('\n').map(line => {
      const match = /^\s*(\d+)\s+(\d+)\s+(.+)$/.exec(line); assert(match, line);
      return { ProcessId: Number(match[1]), ParentProcessId: Number(match[2]), Name: path.basename(match[3]) };
    });
  }
  const byId = new Map(rows.map(row => [row.ProcessId, row]));
  const parentOf = row => {
    const parent = byId.get(row.ParentProcessId);
    if (windows && parent && parent.Started > row.Started) return undefined;
    return parent;
  };
  const belongs = row => {
    const seen = new Set();
    while (row && !seen.has(row.ProcessId)) {
      if (row.ProcessId === process.pid) return true;
      seen.add(row.ProcessId); row = parentOf(row);
    }
    return false;
  };
  const debuggerRow = rows.find(row => /^gdb(?:\.exe)?$/i.test(row.Name) && belongs(row));
  assert(debuggerRow, '当前扩展宿主下必须存在真实 GDB');
  const pids = new Set([debuggerRow.ProcessId, parentOf(debuggerRow).ProcessId]);
  let changed;
  do {
    changed = false;
    for (const row of rows) {
      if (pids.has(parentOf(row)?.ProcessId) && !pids.has(row.ProcessId)) {
        pids.add(row.ProcessId); changed = true;
      }
    }
  } while (changed);
  assert(pids.size >= (attaching ? 2 : 3), '暂停时必须能观察到本会话的完整调试器进程树');
  return [...pids].map(pid => byId.get(pid));
}

/** 等待本场景记录的进程自然退出；只查询存活状态，不按旧 PID 强制终止。 */
async function exited(processes) {
  const pids = processes.map(row => row.ProcessId);
  const alive = pid => { try { process.kill(pid, 0); return true; } catch (error) { if (error.code !== 'ESRCH') throw error; return false; } };
  for (let attempt = 0; attempt < 100; ++attempt) {
    if (!pids.some(alive)) return;
    await new Promise(resolve => setTimeout(resolve, 50));
  }
  const remaining = process.platform === 'win32'
    ? windowsProcesses().filter(current => processes.some(original =>
      current.ProcessId === original.ProcessId && current.Started === original.Started)).map(row => row.ProcessId)
    : pids.filter(alive);
  assert.deepEqual(remaining, [], '原生调试进程必须全部退出');
}

/** 对协议事件使用有界等待，结束后释放定时器。 */
function bounded(promise, label) {
  let timer;
  return Promise.race([promise, new Promise((_, reject) => { timer = setTimeout(() => reject(new Error(label + ' 超时')), 60000); })])
    .finally(() => clearTimeout(timer));
}

/** 为真实编辑器创建独立标准流的外部目标，失败时也回收本测试自建的进程。 */
async function externalAttachTarget(source, work) {
  const compiler = process.env.CHALI_BIN; assert(compiler);
  const prefix = path.join(work, 'attach-program'), env = { ...process.env };
  if (process.platform === 'linux') {
    const input = path.join(work, 'allow_attach.c'), library = path.join(work, 'allow_attach.so');
    fs.writeFileSync(input, '#include <sys/prctl.h>\n#include <unistd.h>\n/**\n * @brief 仅允许本专项目标被同一用户附加，不修改系统 ptrace 设置。\n * @return 无；设置失败以 97 退出。\n */\n__attribute__((constructor)) static void chali_test_allow_attach(void){if(prctl(PR_SET_PTRACER,PR_SET_PTRACER_ANY,0,0,0))_exit(97);}\n');
    const host = spawnSync(process.env.CC || 'cc', ['-shared', '-fPIC', input, '-o', library], { encoding: 'utf8', timeout: 30000 });
    assert.equal(host.status, 0, host.stdout + host.stderr); env.LD_PRELOAD = library;
  }
  const compiled = spawnSync(compiler, ['build', source, '--checked', '--debug', '--out', prefix], { env: process.env, encoding: 'utf8', windowsHide: true, timeout: 180000 });
  assert.equal(compiled.status, 0, compiled.stdout + compiled.stderr);
  const child = spawn(prefix + (process.platform === 'win32' ? '.exe' : ''), [], { env, windowsHide: true, detached: process.platform !== 'win32' });
  try {
    await bounded(new Promise((resolve, reject) => {
      let text = ''; child.stdout.resume(); child.stderr.setEncoding('utf8');
      child.stderr.on('data', part => { text += part; if (text.includes('READY')) resolve(); });
      child.once('error', reject); child.once('exit', () => reject(new Error('外部附加目标提前退出：' + text)));
    }), '外部目标就绪');
    return { child, executable: prefix + (process.platform === 'win32' ? '.exe' : '') };
  } catch (error) { await stopProcessTree(child); throw error; }
}

exports.run = async function () {
  const vscode = require('vscode');
  const work = process.env.CHALI_EXTENSION_TEST_WORK;
  const folder = vscode.workspace.workspaceFolders[0];
  const startupUri = vscode.Uri.joinPath(folder.uri, 'first-open.chl');
  const startupDocument = await vscode.workspace.openTextDocument(startupUri);
  await vscode.window.showTextDocument(startupDocument);
  const macroLine = 2;
  const startupLine = startupDocument.lineAt(macroLine).text;
  const macroColumn = startupLine.indexOf('Equal');
  const hoverPosition = new vscode.Position(macroLine, macroColumn + 1);
  await bounded((async () => {
    for (let attempt = 0; attempt < 100; attempt++) {
      const tokens = await vscode.commands.executeCommand('vscode.provideDocumentSemanticTokens', startupUri);
      let line = 0;
      let column = 0;
      for (let i = 0; i < (tokens?.data.length || 0); i += 5) {
        column = tokens.data[i] ? tokens.data[i + 1] : column + tokens.data[i + 1];
        line += tokens.data[i];
        if (line === macroLine && column === macroColumn && tokens.data[i + 2] === 'Equal'.length && tokens.data[i + 3] === 6) return;
      }
      await new Promise(resolve => setTimeout(resolve, 100));
    }
    assert.fail('首次打开文档时限定宏的语义高亮未就绪');
  })(), '首次打开语义高亮');
  assert.equal(startupDocument.isDirty, false);
  const hovers = await vscode.commands.executeCommand('vscode.executeHoverProvider', startupUri, hoverPosition);
  assert(hovers?.some(hover => hover.contents.some(content => content.value?.includes('$macro Equal'))));
  const completions = await vscode.commands.executeCommand('vscode.executeCompletionItemProvider',
    startupUri, new vscode.Position(macroLine, macroColumn + 2));
  assert(completions?.items.some(item => item.label === '$Equal' && item.insertText === 'Equal' && item.filterText === 'Equal'));
  const argumentsColumn = startupLine.indexOf('leftEqual') + 1;
  const signature = await vscode.commands.executeCommand('vscode.executeSignatureHelpProvider',
    startupUri, new vscode.Position(macroLine, argumentsColumn));
  assert.deepEqual(signature?.signatures.map(item => item.label.match(/\$macro Equal/)?.[0]), ['$macro Equal']);
  const extension = vscode.extensions.getExtension('rabbitc999.chali-editor');
  assert(extension); await extension.activate(); assert(extension.isActive);
  const commands = await vscode.commands.getCommands(true);
  assert(commands.includes('chali.checkDebugEnvironment'));
  assert(commands.includes('chali.previewTypeAlias'));
  const aliasSource = path.join(folder.uri.fsPath, 'alias-preview.chl');
  fs.writeFileSync(aliasSource, 'type alias Count<T = int64> = T;\nvoid main() { Count number = 3; assert(number == 3); }\n');
  const aliasDocument = await vscode.workspace.openTextDocument(aliasSource);
  const aliasEditor = await vscode.window.showTextDocument(aliasDocument);
  aliasEditor.selection = new vscode.Selection(1, 16, 1, 16);
  await bounded((async () => {
    for (let attempt = 0; attempt < 100; attempt++) {
      const hovers = await vscode.commands.executeCommand('vscode.executeHoverProvider', aliasDocument.uri, aliasEditor.selection.active);
      if (hovers?.some(hover => hover.contents.some(content => content.value?.includes('type alias Count')))) return;
      await new Promise(resolve => setTimeout(resolve, 100));
    }
    assert.fail('类型别名悬停未就绪');
  })(), '类型别名语言服务就绪');
  const expandedAlias = await bounded(vscode.commands.executeCommand('chali.previewTypeAlias'), '类型别名展开');
  assert.equal(expandedAlias?.spelling, 'Count');
  assert.equal(expandedAlias?.type, 'int64');
  const interpolationSource = path.join(folder.uri.fsPath, 'interpolation-links.chl');
  const interpolationText = 'String dumpNode(int64 expr, int64 pool) { return "node"; }\n'
    + 'void main() { String name = "test"; int64 expr = 1; int64 pool = 2;\n'
    + '    println(f"😀 let {name} = {dumpNode(expr, pool)}"); }\n';
  fs.writeFileSync(interpolationSource, interpolationText);
  const interpolationDocument = await vscode.workspace.openTextDocument(interpolationSource);
  await vscode.window.showTextDocument(interpolationDocument);
  for (const [word, declaration] of [['name', 'name ='], ['dumpNode', 'dumpNode(int64']]) {
    const at = interpolationText.lastIndexOf(word);
    const definitions = await bounded(vscode.commands.executeCommand('vscode.executeDefinitionProvider',
      interpolationDocument.uri, interpolationDocument.positionAt(at + 1)), '插值定义链接');
    const link = definitions?.find(item => item.targetUri?.toString() === interpolationDocument.uri.toString());
    assert(link?.originSelectionRange?.start.isEqual(interpolationDocument.positionAt(at)));
    assert(link.originSelectionRange.end.isEqual(interpolationDocument.positionAt(at + word.length)));
    assert(link.targetSelectionRange.start.isEqual(interpolationDocument.positionAt(interpolationText.indexOf(declaration))));
  }
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
  let active, external;
  try {
    for (const mode of ['values', 'cancel', 'exception', 'attach']) {
      const source = path.join(folder.uri.fsPath, 'test_' + mode + '.chl');
      fs.writeFileSync(source, mode === 'attach' ? 'import std::thread;\nvoid main(){\n int64 number=42;int64 data[]=new[2];data[0]=73;\n eprintln("READY");\n while(number==42){std::thread::sleepFor(10);}\n}\n'
        : mode === 'exception' ? 'void main(){\n panic("extension-host-failure");\n}\n'
        : 'void main(){\n int64 number=42;\n int64 data[]=new[2]; data[0]=73;\n println("{}",number);\n assert(number==43);\n}\n');
      const document = await vscode.workspace.openTextDocument(source);
      assert.equal(document.languageId, 'chali');
      await vscode.window.showTextDocument(document);
      const breakpoint = new vscode.SourceBreakpoint(new vscode.Location(document.uri, new vscode.Position(mode === 'exception' ? 1 : mode === 'attach' ? 4 : 3, 0)));
      if (mode !== 'exception') vscode.debug.addBreakpoints([breakpoint]);
      let terminatedSubscription;
      const ended = new Promise(resolve => {
        terminatedSubscription = vscode.debug.onDidTerminateDebugSession(session => {
          if (session.name === 'ChaLi extension ' + mode) { terminatedSubscription.dispose(); resolve(); }
        });
      });
      if (mode === 'attach') external = await externalAttachTarget(source, work);
      const launched = await vscode.debug.startDebugging(folder, { type: 'chali', name: 'ChaLi extension ' + mode,
        ...(mode === 'attach' ? { request: 'attach', executable: external.executable, processId: external.child.pid, stopOnAttach: false } : { request: 'launch', program: source }),
        cwd: folder.uri.fsPath, console: 'internalConsole' });
      assert.equal(launched, true);
      const state = scenarios.at(-1); assert(state); active = state.session;
      const stop = await event(state, 'stopped');
      const processes = debugProcesses(mode === 'attach');
      state.processes = processes.map(row => row.ProcessId);
      if (external) assert(!state.processes.includes(external.child.pid), '外部目标不属于调试器进程树');
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
        assert.equal((await active.customRequest('evaluate', { frameId: stack.stackFrames[0].id, expression: 'number+data[0]' })).result, '115');
        assert.equal((await active.customRequest('setVariable', { variablesReference: scopes.scopes[0].variablesReference, name: 'number', value: '43' })).value, '43');
        assert.equal((await active.customRequest('evaluate', { frameId: stack.stackFrames[0].id, expression: 'number' })).result, '43');
        state.modifiedVariables = 1;
        await active.customRequest('continue', { threadId: stop.threadId });
        assert.equal((await event(state, 'exited')).exitCode, 0);
      } else if (mode === 'attach') {
        assert.equal((await active.customRequest('evaluate', { frameId: stack.stackFrames[0].id, expression: 'number' })).result, '42');
        await active.customRequest('setBreakpoints', { source: { path: source }, breakpoints: [] });
        await active.customRequest('continue', { threadId: stop.threadId });
        await vscode.debug.stopDebugging(active);
      } else if (mode === 'exception') {
        assert.match(stop.text, /extension-host-failure/);
        await active.customRequest('continue', { threadId: stop.threadId });
        assert.notEqual((await event(state, 'exited')).exitCode, 0);
      } else {
        await vscode.debug.stopDebugging(active);
      }
      await event(state, 'terminated');
      await bounded(ended, 'VSCode 会话回收');
      await exited(processes);
      if (external) {
        await new Promise(resolve => setTimeout(resolve, 100));
        assert.equal(external.child.exitCode, null); assert.equal(external.child.signalCode, null);
        state.detachedTarget = external.child.pid;
        await stopProcessTree(external.child); external = null;
        state.detachedTargetPreserved = true;
      }
      vscode.debug.removeBreakpoints([breakpoint]);
      active = null;
    }
    fs.writeFileSync(path.join(work, 'result.json'), JSON.stringify({ vscode: vscode.version, extension: extension.packageJSON.version,
      activated: true, interpolationDefinitionLinks: true, scenarios: scenarios.map(state => ({ name: state.session.name,
        stopped: state.messages.filter(message => message.event === 'stopped').length,
        processesExited: state.processes.length,
        processes: state.processes,
        detachedTarget: state.detachedTarget,
        detachedTargetPreserved: state.detachedTargetPreserved === true,
        modifiedVariables: state.modifiedVariables || 0,
        terminated: state.messages.some(message => message.event === 'terminated') })) }, null, 2));
  } catch (error) {
    fs.writeFileSync(path.join(work, 'failure.json'), JSON.stringify({ error: error.stack,
      sessions: scenarios.map(state => ({ name: state.session.name, messages: state.messages })) }, null, 2));
    throw error;
  } finally {
    if (active) await vscode.debug.stopDebugging(active);
    await stopProcessTree(external?.child);
    tracker.dispose();
    vscode.debug.removeBreakpoints(vscode.debug.breakpoints);
  }
};
