'use strict';

/**
 * 注册标准库只读虚拟文档；源码通过当前编译器取得，不落盘。
 * @param {object} vscode 编辑器 API。
 * @param {object} context 插件上下文。
 * @param {object} deps 当前语言服务客户端。
 * @returns {object} 重启后的刷新入口。
 */
function registerStandardSources(vscode, context, deps) {
  const changed = new vscode.EventEmitter();
  const provider = {
    onDidChange: changed.event,
    async provideTextDocumentContent(uri, token) {
      const client = deps.getClient();
      if (!client?.running) throw new Error('ChaLi 语言服务尚未就绪。');
      const result = await client.request('chali/standardSource', { uri: uri.toString() }, token);
      if (!result || result.readonly !== true || result.uri !== uri.toString()) throw new Error('标准库源码请求未完成。');
      return result.text;
    },
  };
  context.subscriptions.push(changed, vscode.workspace.registerTextDocumentContentProvider('chali-stdlib', provider));
  return {
    refresh() {
      for (const document of vscode.workspace.textDocuments) {
        if (document.uri.scheme === 'chali-stdlib') changed.fire(document.uri);
      }
    },
  };
}

module.exports = { registerStandardSources };
