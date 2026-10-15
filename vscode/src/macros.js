'use strict';

/**
 * 注册只读宏展开预览，使用服务器的真实预处理器，支持未保存源码。
 * @param {object} vscode VSCode API。
 * @param {object} context 扩展上下文。
 * @param {object} deps 当前语言客户端与即时文档同步回调。
 * @returns {void} 无。
 */
function registerMacroPreview(vscode, context, deps) {
  const contents = new Map();
  let serial = 0;
  context.subscriptions.push(vscode.workspace.registerTextDocumentContentProvider('magel-expanded', {
    provideTextDocumentContent(uri) { return contents.get(uri.toString()) || ''; },
  }));
  context.subscriptions.push(vscode.workspace.onDidCloseTextDocument((document) => {
    if (document.uri.scheme === 'magel-expanded') contents.delete(document.uri.toString());
  }));
  context.subscriptions.push({ dispose() { contents.clear(); } });
  context.subscriptions.push(vscode.commands.registerCommand('magel.previewMacroExpansion', async () => {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'magel' || editor.document.uri.scheme !== 'file') {
      await vscode.window.showInformationMessage('Magel：请先打开一个 .mgl 源文件。');
      return;
    }
    const client = deps.getClient();
    if (!client || !client.running) {
      await vscode.window.showErrorMessage('Magel：语言服务器尚未启动。');
      return;
    }
    try {
      deps.flushDocument(editor.document);
      const result = await client.request('magel/expandMacro', {
        textDocument: { uri: editor.document.uri.toString() },
      });
      if (!result || (result.diagnostics && result.diagnostics.length)) {
        throw new Error(result && result.diagnostics ? result.diagnostics.join('\n') : '服务器未返回展开结果');
      }
      const uri = vscode.Uri.parse(`magel-expanded:/宏展开-${++serial}.mgl`);
      contents.set(uri.toString(), result.text);
      const document = await vscode.workspace.openTextDocument(uri);
      await vscode.languages.setTextDocumentLanguage(document, 'magel');
      await vscode.window.showTextDocument(document, { viewColumn: vscode.ViewColumn.Beside, preview: true });
    } catch (error) {
      await vscode.window.showErrorMessage(`Magel：宏展开失败：${error.message}`);
    }
  }));
}

module.exports = { registerMacroPreview };
