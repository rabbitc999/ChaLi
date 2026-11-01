'use strict';

/**
 * 转换带版本保护的工作区编辑，不允许旧结果覆盖新缓冲区。
 * @param {object} vscode 编辑器 API。
 * @param {object} result 服务端 WorkspaceEdit。
 * @returns {object} 编辑器工作区编辑。
 */
function workspaceEdit(vscode, result) {
  const edit = new vscode.WorkspaceEdit();
  for (const change of result.documentChanges || []) {
    const uri = vscode.Uri.parse(change.uri || change.textDocument?.uri);
    if (uri.scheme !== 'file' && uri.scheme !== 'untitled') throw new Error('目标文档只读。');
    if (change.kind === 'create') { edit.createFile(uri, change.options); continue; }
    const open = vscode.workspace.textDocuments.find(item => item.uri.toString() === uri.toString());
    if (change.textDocument.version !== null && change.textDocument.version !== undefined
        && (!open || open.version !== change.textDocument.version)) throw new Error('文档已更新，请重新执行。');
    edit.set(uri, change.edits.map(item => new vscode.TextEdit(
      new vscode.Range(new vscode.Position(item.range.start.line, item.range.start.character),
        new vscode.Position(item.range.end.line, item.range.end.character)), item.newText)));
  }
  return edit;
}

/**
 * 注册由编译器重新核对的诊断修复。
 * @param {object} vscode 编辑器 API。
 * @param {object} context 插件上下文。
 * @param {object} deps 请求与语言选择器。
 * @returns {void} 无。
 */
function registerCodeActions(vscode, context, deps) {
  context.subscriptions.push(vscode.languages.registerCodeActionsProvider(deps.selector, {
    async provideCodeActions(document, range, diagnostics, token) {
      const result = await deps.request('textDocument/codeAction', {
        textDocument: { uri: document.uri.toString() },
        range: { start: { line: range.start.line, character: range.start.character },
          end: { line: range.end.line, character: range.end.character } },
        context: { diagnostics: [], only: diagnostics.only ? [diagnostics.only.value] : undefined },
      }, token, document);
      return (result || []).map(item => {
        const action = new vscode.CodeAction(item.title, vscode.CodeActionKind.QuickFix);
        action.edit = workspaceEdit(vscode, item.edit);
        action.isPreferred = !!item.isPreferred;
        return action;
      });
    },
  }, { providedCodeActionKinds: [vscode.CodeActionKind.QuickFix] }));
}

module.exports = { registerCodeActions, workspaceEdit };
