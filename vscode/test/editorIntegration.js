'use strict';
const assert = require('node:assert/strict');
const { registerCodeActions, workspaceEdit } = require('../src/codeActions');
const { registerStandardSources } = require('../src/standardSources');
const { registerProjectTools } = require('../src/projectTools');

/** 验证修复、只读源码与项目配置的真实扩展 API 转换和过期编辑拒绝。 */
async function main() {
  const uri = value => ({ scheme: value.split(':')[0], fsPath: value.replace('file://', ''), toString: () => value });
  const sourceUri = uri('file:///project/main.chl'), configUri = uri('file:///project/chali.toml');
  const document = { uri: sourceUri, languageId: 'chali', version: 4, isDirty: false };
  const manifest = { uri: configUri, languageId: 'toml', version: 7, isDirty: true,
    text: '# unsaved comment\n[aot]\ntarget="windows-x64"\n', getText() { return this.text; } };
  const commands = new Map(), errors = [], statusItems = [];
  let codeActions, standardProvider, applyCount = 0, race = false;
  const disposable = { dispose() {} };
  class WorkspaceEdit {
    constructor() { this.changes = []; this.creates = []; }
    set(target, edits) { this.changes.push({ target, edits }); }
    createFile(target, options) { this.creates.push({ target, options }); }
  }
  class Position { constructor(line, character) { Object.assign(this, { line, character }); } }
  class Range { constructor(start, end) { Object.assign(this, { start, end }); } }
  class TextEdit { constructor(range, newText) { Object.assign(this, { range, newText }); } }
  class CodeAction { constructor(title, kind) { Object.assign(this, { title, kind }); } }
  class EventEmitter { constructor() { this.event = () => disposable; this.fired = []; } fire(value) { this.fired.push(value); } dispose() {} }
  const vscode = {
    WorkspaceEdit, Position, Range, TextEdit, CodeAction, EventEmitter,
    Uri: { parse: uri }, CodeActionKind: { QuickFix: { value: 'quickfix' } },
    StatusBarAlignment: { Right: 1 }, env: { language: 'en' },
    languages: { registerCodeActionsProvider(_selector, provider) { codeActions = provider; return disposable; } },
    workspace: {
      textDocuments: [document, manifest],
      registerTextDocumentContentProvider(scheme, provider) { assert.equal(scheme, 'chali-stdlib'); standardProvider = provider; return disposable; },
      async openTextDocument(target) { assert.equal(target.toString(), configUri.toString()); return manifest; },
      async applyEdit(edit) { ++applyCount; manifest.text = edit.changes[0].edits[0].newText; ++manifest.version; manifest.isDirty = true; return true; },
      onDidSaveTextDocument: () => disposable, onDidChangeTextDocument: () => disposable,
    },
    window: {
      activeTextEditor: { document },
      createStatusBarItem() { const item = { ...disposable, show() {}, hide() {} }; statusItems.push(item); return item; },
      onDidChangeActiveTextEditor: () => disposable,
      async showQuickPick(choices) { return choices[1]; },
      async showTextDocument(current) { vscode.window.activeTextEditor = { document: current }; },
      async showErrorMessage(message) { errors.push(message); },
    },
    commands: { registerCommand(name, callback) { commands.set(name, callback); return disposable; } },
  };
  const context = { subscriptions: [] };
  const range = { start: { line: 1, character: 0 }, end: { line: 1, character: 0 } };
  const editFor = (target, version, newText) => ({ documentChanges: [{
    textDocument: { uri: target.toString(), version }, edits: [{ range, newText }],
  }] });
  registerCodeActions(vscode, context, {
    selector: [], async request(method, params, _token, current) {
      assert.equal(method, 'textDocument/codeAction'); assert.equal(current, document);
      assert.deepEqual(params.context.only, ['quickfix']);
      return [{ title: 'Suppress CHL2201', kind: 'quickfix', edit: editFor(sourceUri, 4, '// chali: allow-next-line CHL2201\n') }];
    },
  });
  const actions = await codeActions.provideCodeActions(document, range, { only: vscode.CodeActionKind.QuickFix });
  assert.equal(actions[0].edit.changes[0].edits[0].newText, '// chali: allow-next-line CHL2201\n');
  document.version = 5;
  await assert.rejects(codeActions.provideCodeActions(document, range, { only: vscode.CodeActionKind.QuickFix }), /文档已更新/);
  document.version = 4;
  assert.throws(() => workspaceEdit(vscode, editFor(uri('chali-stdlib:/std/parse.chl'), null, 'bad')), /只读/);
  const client = {
    running: true,
    async request(method, params) {
      if (method === 'chali/standardSource') return { uri: params.uri, readonly: true, text: 'module std::parse;\n' };
      if (method === 'chali/projectInfo') return { valid: true, exists: true, manifestUri: configUri.toString(),
        compilerVersion: '1.0.0', llvm: 'static', output: 'object', target: 'windows-x64', targets: [
          { label: 'windows-x64', target: 'windows-x64' }, { label: 'linux-arm64', target: 'linux-arm64' }],
      };
      assert.equal(method, 'chali/projectEdit');
      assert.equal(params.text, manifest.getText()); assert.equal(params.version, manifest.version);
      const result = editFor(configUri, params.version, `# unsaved comment\n[aot]\ntarget="${params.value}"\n`);
      if (race) ++manifest.version;
      return result;
    },
  };
  const standard = registerStandardSources(vscode, context, { getClient: () => client });
  assert.equal(await standardProvider.provideTextDocumentContent(uri('chali-stdlib:/std/parse.chl')), 'module std::parse;\n');
  standard.refresh();
  const project = registerProjectTools(vscode, context, { getClient: () => client });
  await commands.get('chali.configureTarget')();
  assert.equal(applyCount, 1); assert.equal(errors.length, 0);
  assert.ok(manifest.getText().includes('linux-arm64') && manifest.getText().includes('# unsaved comment'));
  race = true;
  await commands.get('chali.configureTarget')();
  assert.equal(applyCount, 1); assert.equal(errors.length, 1);
  project.refresh();
  await new Promise(resolve => setTimeout(resolve, 300));
  assert.match(statusItems[0].text, /unsaved configuration/);
  for (const subscription of context.subscriptions) subscription.dispose();
  const { ServerLifecycle } = require('../src/serverLifecycle');
  const starts = []; let stopped = 0, release;
  const lifecycle = new ServerLifecycle({
    async start(value) { starts.push(value); if (value === 'active') await new Promise(resolve => { release = resolve; }); },
    async stop() { stopped++; },
  });
  await Promise.all([lifecycle.restart('old'), lifecycle.restart('new')]);
  assert.deepEqual(starts, ['new']);
  const active = lifecycle.restart('active'); await Promise.resolve(); await Promise.resolve();
  const queued = lifecycle.restart('never');
  const closing = lifecycle.dispose(); release();
  await Promise.all([active, queued, closing]);
  assert.deepEqual(starts, ['new', 'active']); assert.equal(stopped, 1);
  console.log('诊断修复、只读源码、配置版本保护、项目状态及串行服务切换通过。');
}

main().catch(error => { console.error(error); process.exitCode = 1; });
