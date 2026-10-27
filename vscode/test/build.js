'use strict';

/** 构建命令集成回归：模拟编辑器 API，使用真实编译器验收 exe、保存、右键与失败诊断。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const os = require('node:os');
const { spawnSync } = require('node:child_process');
const { buildArguments, registerBuild } = require('../src/build');

/** 执行真实构建并核对原生产物及各失败分支。 */
async function main() {
  const root = path.resolve(__dirname, '../..');
  const compiler = path.resolve(process.env.CHALI_BIN || path.join(root, process.platform === 'win32' ? 'ChaLi.exe' : 'chali'));
  const directory = fs.mkdtempSync(path.join(require('./projectTemp').root, 'chali build space '));
  const source = path.join(directory, 'hello & 中文.chl');
  fs.writeFileSync(source, 'void main() { println("build {}", 42); }\n');
  const commands = new Map(); const errors = []; const messages = []; const logs = [];
  let saved = 0; let cancel = false;
  const uri = (fsPath) => ({ scheme: 'file', fsPath });
  const document = { uri: uri(source), isDirty: true, async save() { ++saved; this.isDirty = false; return true; } };
  const settings = { 'build.outputDirectory': 'output & space', 'build.llvmPath': process.platform === 'win32'
    ? path.join(root, 'build/dependencies/llvm/toolchain/bin/clang.exe') : '', 'build.checked': true };
  const disposable = { dispose() {} };
  const context = { extensionPath: path.join(root, 'vscode'), subscriptions: [] };
  const vscode = {
    Uri: { file: uri }, ProgressLocation: { Notification: 1 },
    commands: { registerCommand(name, callback) { commands.set(name, callback); return disposable; }, async executeCommand() {} },
    window: {
      activeTextEditor: { document },
      createOutputChannel: () => ({ ...disposable, append: (s) => logs.push(s), appendLine: (s) => logs.push(s), show() {} }),
      async showInformationMessage(s) { messages.push(s); }, async showWarningMessage(s) { messages.push(s); },
      async showErrorMessage(s) { errors.push(s); },
      async showQuickPick(choices) { return cancel ? undefined : choices[1]; },
      async withProgress(_options, task) { return task(); },
    },
    workspace: { isTrusted: true, textDocuments: [document],
      getConfiguration: () => ({ get: (key, fallback) => settings[key] ?? fallback }) },
  };
  try {
    registerBuild(vscode, context, { readConfig: () => ({ chaliPath: compiler }) });
    assert.equal(commands.size, 2);
    assert.equal(commands.has('chali.buildSvmb'), false);
    assert.throws(() => buildArguments('svmb', source, 'prefix'), /未知构建格式/);
    assert.deepEqual(buildArguments('exe', source, 'prefix', { llvmPath: 'clang space', checked: true }), ['aot', source, '--out', 'prefix', '--llvm', 'clang space', '--checked']);
    assert.deepEqual(buildArguments('exe', source, 'prefix', { checked: false }), ['aot', source, '--out', 'prefix', '--unchecked']);
    await commands.get('chali.build')();
    assert.deepEqual(errors, []); assert.equal(saved, 1);
    const prefix = path.join(directory, settings['build.outputDirectory'], 'hello & 中文');
    assert.ok(fs.statSync(prefix + '.exe').size > 0);
    let result;
    if (process.platform === 'win32') {
      // 显式右键目标应优先于当前不相关的编辑器。
      vscode.window.activeTextEditor = null;
      await commands.get('chali.buildExe')(uri(source));
      assert.deepEqual(errors, []);
      assert.ok(fs.statSync(prefix + '.exe').size > 0);
      result = spawnSync(prefix + '.exe', [], { cwd: directory, encoding: 'utf8' });
      assert.equal(result.status, 0, result.stderr); assert.equal(result.stdout.replace(/\r\n/g, '\n'), 'build 42\n');
    }
    const successes = messages.filter((s) => s.includes('构建完成')).length;
    fs.writeFileSync(source, 'void main() { missing_symbol(); }\n');
    await commands.get('chali.buildExe')(uri(source));
    assert.equal(errors.length, 1);
    assert.equal(messages.filter((s) => s.includes('构建完成')).length, successes);
    assert.ok(logs.join('\n').includes('missing_symbol'));
    const count = logs.length;
    vscode.workspace.isTrusted = false;
    await commands.get('chali.buildExe')(uri(source));
    assert.equal(logs.length, count);
    vscode.workspace.isTrusted = true;
    await commands.get('chali.buildExe')({ scheme: 'untitled', fsPath: source });
    assert.equal(errors.length, 2);
    // 英语编辑器使用英文界面与编译器语言，不翻译源码中的文件名和值。
    vscode.env = { language: 'en' }; cancel = false;
    fs.writeFileSync(source, 'void main() { println("build {}", 42); }\n');
    registerBuild(vscode, context, { readConfig: () => ({ chaliPath: compiler }) });
    await commands.get('chali.buildExe')(uri(source));
    assert.equal(errors.length, 2);
    assert.ok(messages.some((s) => s.includes('Build completed')));
    console.log('VS Code 构建：原生产物、已删除字节码入口、保存、右键目标、空格与中文路径、取消、信任和诊断全部通过');
  } finally {
    for (const item of context.subscriptions) item.dispose();
    fs.rmSync(directory, { recursive: true, force: true });
  }
}

main().catch((error) => { console.error(error); process.exitCode = 1; });
