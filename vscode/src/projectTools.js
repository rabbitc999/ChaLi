'use strict';
const path = require('node:path');
const { workspaceEdit } = require('./codeActions');

/**
 * 注册项目配置、目标选择与状态栏，项目语义由编译器读取。
 * @param {object} vscode 编辑器 API。
 * @param {object} context 扩展上下文。
 * @param {object} deps 当前语言服务客户端。
 * @returns {object} 项目查询及服务重启刷新入口。
 */
function registerProjectTools(vscode, context, deps) {
  const english = String(vscode.env?.language || 'zh').startsWith('en');
  const text = (zh, en) => english ? en : zh;
  const status = vscode.window.createStatusBarItem(vscode.StatusBarAlignment.Right, 89);
  status.command = 'chali.configureTarget';
  let timer, disposed = false, serial = 0;
  const source = () => {
    const document = vscode.window.activeTextEditor?.document;
    return document?.uri.scheme === 'file' && (document.languageId === 'chali'
      || path.basename(document.uri.fsPath).toLowerCase() === 'chali.toml') ? document.uri : null;
  };

  /** 从当前编译器查询已保存清单，不用扩展端的正则猜测配置。 */
  async function info(uri) {
    const client = deps.getClient();
    if (!uri || !client?.running) return null;
    return client.request('chali/projectInfo', { uri: uri.toString() });
  }

  /** 刷新当前编辑器的项目状态，旧请求不能覆盖新编辑器。 */
  function refresh() {
    const version = ++serial;
    clearTimeout(timer);
    timer = setTimeout(async () => {
      const uri = source();
      if (!uri) { status.hide(); return; }
      const active = vscode.window.activeTextEditor?.document;
      if (active?.isDirty && path.basename(active.uri.fsPath).toLowerCase() === 'chali.toml') {
        status.text = text('$(edit) ChaLi 配置未保存', '$(edit) ChaLi unsaved configuration');
        status.tooltip = text('保存后生效；构建会先保存配置。', 'Save to apply; builds save the configuration first.');
        status.show(); return;
      }
      try {
        const result = await info(uri);
        if (disposed || version !== serial || !result) return;
        if (!result.valid) {
          status.text = text('$(warning) ChaLi 配置错误', '$(warning) ChaLi configuration error');
          status.tooltip = result.error;
        } else {
          const target = result.targets.find(item => item.target === result.target)?.label || result.target;
          const format = { object: text('对象文件', 'Object'), assembly: text('机器汇编', 'Assembly'), llvm: 'LLVM IR', executable: text('可执行文件', 'Executable') }[result.output];
          status.text = `$(target) ChaLi ${target} · ${format}`;
          status.tooltip = `ChaLi ${result.compilerVersion} · LLVM ${result.llvm}\n${result.manifestUri}\n${text('点击选择目标', 'Click to select target')}`;
        }
        status.show();
      } catch (error) {
        if (!disposed && version === serial && error.code !== -32800) {
          status.text = text('$(warning) ChaLi 项目状态不可用', '$(warning) ChaLi project status unavailable');
          status.tooltip = error.message; status.show();
        }
      }
    }, 250);
  }

  /** 按当前 TOML 缓冲区生成版本化编辑，保留其他设置及注释。 */
  async function configure(key) {
    try {
      const uri = source();
      if (!uri) throw new Error(text('请先打开 .chl 文件或 chali.toml。', 'Open a .chl file or chali.toml first.'));
      const project = await info(uri);
      if (!project?.valid) throw new Error(project?.error || text('语言服务尚未就绪。', 'Language server is not ready.'));
      const choices = key === 'target' ? project.targets : [
        { label: text('可执行文件', 'Executable'), target: 'executable' },
        { label: text('对象文件', 'Object file'), target: 'object' },
        { label: text('机器汇编', 'Assembly'), target: 'assembly' },
        { label: 'LLVM IR', target: 'llvm' },
      ];
      const selected = await vscode.window.showQuickPick(choices, { placeHolder: text('选择项目构建配置', 'Select project build configuration') });
      if (!selected) return;
      const manifest = vscode.Uri.parse(project.manifestUri);
      const document = project.exists ? await vscode.workspace.openTextDocument(manifest) : null;
      const before = document?.getText() || '';
      const version = document?.version ?? null;
      const edit = await deps.getClient().request('chali/projectEdit', {
        uri: uri.toString(), text: before, version, key, value: selected.target,
      });
      if (!edit || (document && (document.version !== version || document.getText() !== before))) {
        throw new Error(text('配置已更新，请重新执行。', 'Configuration changed; try again.'));
      }
      if (!(await vscode.workspace.applyEdit(workspaceEdit(vscode, edit)))) throw new Error(text('配置编辑未应用。', 'Configuration edit was not applied.'));
      const updated = await vscode.workspace.openTextDocument(manifest);
      await vscode.window.showTextDocument(updated);
      // 工作区编辑保留为可撤销的缓冲区变更；构建时和源文件一起保存。
      refresh();
    } catch (error) { if (error.code !== -32800) await vscode.window.showErrorMessage(error.message); }
  }

  context.subscriptions.push(status, { dispose() { disposed = true; clearTimeout(timer); } },
    vscode.window.onDidChangeActiveTextEditor(refresh),
    vscode.workspace.onDidChangeTextDocument(event => { if (path.basename(event.document.uri.fsPath || '') === 'chali.toml') refresh(); }),
    vscode.workspace.onDidSaveTextDocument(document => { if (path.basename(document.uri.fsPath || '') === 'chali.toml') refresh(); }),
    vscode.commands.registerCommand('chali.configureTarget', () => configure('target')),
    vscode.commands.registerCommand('chali.configureOutput', () => configure('output')),
    vscode.commands.registerCommand('chali.openProjectConfig', async () => {
      try {
        const uri = source(), project = await info(uri);
        if (!project) throw new Error(text('请先打开 .chl 文件或 chali.toml。', 'Open a .chl file or chali.toml first.'));
        const target = vscode.Uri.parse(project.manifestUri);
        if (!project.exists) {
          const edit = new vscode.WorkspaceEdit(); edit.createFile(target, { ignoreIfExists: true });
          if (!(await vscode.workspace.applyEdit(edit))) return;
        }
        await vscode.window.showTextDocument(await vscode.workspace.openTextDocument(target));
      } catch (error) { if (error.code !== -32800) await vscode.window.showErrorMessage(error.message); }
    }));
  refresh();
  return { info, refresh };
}

module.exports = { registerProjectTools };
