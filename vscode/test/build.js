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
    // append 接收任意管道分块，不能在块间插入换行拆开诊断中的标识符。
    assert.ok(logs.join('').includes('missing_symbol'), logs.join(''));
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
    // 通过真实 LSP 解析清单；构建按钮先保存脏配置，并核对交叉对象／LLVM IR 的实际后缀。
    const { LspClient } = require('../src/lspClient');
    const { pathToFileURL } = require('node:url');
    const client = new LspClient(compiler, ['lsp'], { cwd: directory });
    const manifestPath = path.join(directory, 'chali.toml');
    let configSaved = 0;
    const manifest = { uri: uri(manifestPath), isDirty: true,
      text: '[aot]\ntarget="linux-arm64"\noutput="object"\n',
      async save() { ++configSaved; fs.writeFileSync(manifestPath, this.text); this.isDirty = false; return true; } };
    fs.writeFileSync(manifestPath, '');
    vscode.workspace.textDocuments.push(manifest);
    settings['build.llvmPath'] = '';
    try {
      await client.start(); await client.initialize();
      registerBuild(vscode, context, { readConfig: () => ({ chaliPath: compiler }),
        projectInfo: target => client.request('chali/projectInfo', { uri: pathToFileURL(target.fsPath).toString() }) });
      await commands.get('chali.build')(uri(source));
      assert.equal(errors.length, 2, errors.join('\n')); assert.equal(configSaved, 1);
      const object = fs.readFileSync(prefix + '.o');
      assert.equal(object.subarray(0, 4).toString('hex'), '7f454c46'); assert.equal(object.readUInt16LE(18), 183);
      await commands.get('chali.buildExe')(uri(source));
      assert.equal(errors.length, 3); assert.match(errors[2], /object or LLVM IR/);
      manifest.text = '[aot]\ntarget="linux-arm64"\noutput="llvm"\n'; manifest.isDirty = true;
      await commands.get('chali.build')(uri(source));
      assert.equal(errors.length, 3, errors.join('\n')); assert.equal(configSaved, 2);
      assert.ok(fs.readFileSync(prefix + '.ll', 'utf8').includes('aarch64-unknown-linux-gnu'));
    } finally { await client.stop(); }
    console.log('VS Code 构建：原生产物、配置保存、ARM64 对象、LLVM IR、错误输出模式及既有交互通过');
  } finally {
    for (const item of context.subscriptions) item.dispose();
    fs.rmSync(directory, { recursive: true, force: true, maxRetries: 5, retryDelay: 100 });
  }
}

main().catch((error) => { console.error(error); process.exitCode = 1; });
