'use strict';
const assert = require('node:assert/strict');
const { registerMacroPreview } = require('../src/macros');

async function main() {
  let command, provider, closed, flushed = false, shown, result = { text: 'int64 answer=4;', diagnostics: [] };
  const errors = [];
  const uri = (value) => ({ scheme: value.split(':')[0], toString: () => value });
  const document = { languageId: 'magel', uri: uri('file:///project/main.mgl') };
  const vscode = {
    Uri: { parse: uri }, ViewColumn: { Beside: 2 },
    workspace: {
      registerTextDocumentContentProvider(scheme, value) { assert.equal(scheme, 'magel-expanded'); provider = value; return { dispose() {} }; },
      onDidCloseTextDocument(callback) { closed = callback; return { dispose() {} }; },
      async openTextDocument(value) { return { uri: value, text: provider.provideTextDocumentContent(value) }; },
    },
    commands: { registerCommand(name, callback) { assert.equal(name, 'magel.previewMacroExpansion'); command = callback; return { dispose() {} }; } },
    languages: { async setTextDocumentLanguage(value, language) { assert.equal(language, 'magel'); return value; } },
    window: {
      activeTextEditor: { document }, async showInformationMessage() {},
      async showErrorMessage(message) { errors.push(message); },
      async showTextDocument(value, options) { shown = value; assert.equal(options.viewColumn, 2); },
    },
  };
  const context = { subscriptions: [] };
  registerMacroPreview(vscode, context, {
    flushDocument(value) { assert.equal(value, document); flushed = true; },
    getClient: () => ({ running: true, async request(method, params) {
      assert.ok(flushed); assert.equal(method, 'magel/expandMacro');
      assert.equal(params.textDocument.uri, document.uri.toString()); return result;
    } }),
  });
  await command(); assert.equal(shown.text, result.text); assert.deepEqual(errors, []);
  closed(shown); assert.equal(provider.provideTextDocumentContent(shown.uri), '');
  result = { diagnostics: ['宏实参数量错误'], text: '' };
  await command(); assert.match(errors[0], /宏实参数量错误/);
  for (const item of context.subscriptions) item.dispose();
  console.log('宏展开插件验证通过：同步未保存内容、只读预览、关闭清理与错误展示。');
}
main().catch((error) => { console.error(error); process.exitCode = 1; });
