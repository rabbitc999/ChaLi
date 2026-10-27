'use strict';

const fs = require('node:fs');
const path = require('node:path');
const { spawn } = require('node:child_process');
const locator = require('./locator');
const { findProject } = require('./dependencies');

/**
 * 构造构建参数，保持文件名和工具路径为独立 argv，不进入 shell。
 * @param {string} format 仅支持 exe。
 * @param {string} source 源文件绝对路径。
 * @param {string} prefix 输出前缀。
 * @param {object} options LLVM 路径及检查开关。
 * @returns {string[]} 编译器实参。
 */
function buildArguments(format, source, prefix, options = {}) {
  if (format !== 'exe') throw new Error('未知构建格式');
  const args = ['aot', source, '--out', prefix];
  if (format === 'exe' && options.llvmPath) args.push('--llvm', options.llvmPath);
  if (format === 'exe' && options.checked) args.push('--checked');
  if (format === 'exe' && options.checked === false) args.push('--unchecked');
  return args;
}

/**
 * 运行一个构建进程，完整转交编译器诊断，失败时保留退出码。
 * @param {string} compiler 编译器路径。
 * @param {string[]} args 独立实参。
 * @param {string} cwd 工程工作目录。
 * @param {Function} append 输出回调。
 * @returns {Promise<void>} 成功退出时完成。
 */
function runBuild(compiler, args, cwd, append, language = 'zh') {
  return new Promise((resolve, reject) => {
    const english = String(language).toLowerCase().startsWith('en');
    const child = spawn(compiler, args, { cwd, windowsHide: true, shell: false, stdio: ['ignore', 'pipe', 'pipe'],
      env: { ...process.env, CHALI_LANG: english ? 'en' : 'zh' } });
    child.stdout.setEncoding('utf8'); child.stderr.setEncoding('utf8');
    child.stdout.on('data', append); child.stderr.on('data', append);
    child.once('error', reject);
    child.once('close', (code, signal) => {
      if (code === 0) resolve();
      else reject(new Error(`${english ? 'Build failed, exit code: ' : '构建失败，退出码：'}${code === null ? signal : code}`));
    });
  });
}

/**
 * 注册格式选择、LLVM AOT 构建命令。
 * @param {object} vscode 编辑器 API。
 * @param {object} context 扩展上下文。
 * @param {object} deps 编译器配置提供方。
 * @returns {void} 无。
 */
function registerBuild(vscode, context, deps) {
  const language = vscode.env?.language || 'zh';
  const text = (zh, en) => language.toLowerCase().startsWith('en') ? en : zh;
  const output = vscode.window.createOutputChannel(text('ChaLi 构建', 'ChaLi Build'));
  let running = false;
  context.subscriptions.push(output);

  /** 保存目标文档，构建选定产物并在完成后提供定位入口。 */
  async function execute(format, uri) {
    if (vscode.workspace.isTrusted === false) {
      await vscode.window.showWarningMessage(text('ChaLi：构建需要在受信任的工作区中执行。', 'ChaLi: Building requires a trusted workspace.'));
      return;
    }
    if (running) {
      await vscode.window.showInformationMessage(text('ChaLi：已有构建正在运行，请等待完成。', 'ChaLi: A build is already running.'));
      return;
    }
    running = true;
    try {
      const selected = uri || vscode.window.activeTextEditor?.document.uri;
      if (!selected || selected.scheme !== 'file' || path.extname(selected.fsPath).toLowerCase() !== '.chl') {
        throw new Error(text('请选择已保存到磁盘的 .chl 源文件', 'Select a .chl source file saved on disk'));
      }
      if (!format) format = 'exe';
      if (format === 'exe' && process.platform !== 'win32') throw new Error(text('exe 构建入口当前面向 Windows x64', 'The exe build command currently targets Windows x64'));
      const document = vscode.workspace.textDocuments.find((item) => item.uri.scheme === 'file'
        && path.resolve(item.uri.fsPath) === path.resolve(selected.fsPath));
      if (document?.isDirty && !(await document.save())) throw new Error(text('源文件保存失败，构建已取消', 'The source could not be saved; build cancelled'));
      const cwd = findProject(selected.fsPath) || path.dirname(selected.fsPath);
      const compiler = locator.resolveChaLi(cwd, deps.readConfig(), context.extensionPath).chali;
      if (!compiler) throw new Error(text('未找到 ChaLi 编译器，请设置 chali.path', 'ChaLi compiler not found; configure chali.path'));
      const config = vscode.workspace.getConfiguration('chali', selected);
      const directory = path.resolve(cwd, config.get('build.outputDirectory', 'build'));
      fs.mkdirSync(directory, { recursive: true });
      const prefix = path.join(directory, path.basename(selected.fsPath, path.extname(selected.fsPath)));
      const args = buildArguments(format, selected.fsPath, prefix, {
        llvmPath: config.get('build.llvmPath', '').trim(), checked: config.get('build.checked', false),
      });
      const product = prefix + '.exe';
      output.show(true);
      output.appendLine(text(`编译器：${compiler}\n源文件：${selected.fsPath}\n产物：${product}`, `Compiler: ${compiler}\nSource: ${selected.fsPath}\nOutput: ${product}`));
      await vscode.window.withProgress({ location: vscode.ProgressLocation.Notification,
        title: text('ChaLi：构建 LLVM AOT 可执行文件', 'ChaLi: Build LLVM AOT executable'), cancellable: false },
      () => runBuild(compiler, args, cwd, (chunk) => output.append(chunk), language));
      if (!fs.existsSync(product) || !fs.statSync(product).size) throw new Error(text('编译器未生成目标产物', 'The compiler did not produce the requested output'));
      output.appendLine(text(`构建完成：${product}`, `Build completed: ${product}`));
      const reveal = text('在资源管理器中显示', 'Reveal in File Explorer');
      const action = await vscode.window.showInformationMessage(text(`ChaLi：构建完成 ${product}`, `ChaLi: Build completed ${product}`), reveal);
      if (action === reveal) await vscode.commands.executeCommand('revealFileInOS', vscode.Uri.file(product));
    } catch (error) {
      output.appendLine(error.message); output.show(true);
      await vscode.window.showErrorMessage(text(`ChaLi：${error.message}，详细诊断见“ChaLi 构建”输出。`, `ChaLi: ${error.message}. See the ChaLi Build output for diagnostics.`));
    } finally {
      running = false;
    }
  }

  for (const [command, format] of [['chali.build', null], ['chali.buildExe', 'exe']]) {
    context.subscriptions.push(vscode.commands.registerCommand(command, (uri) => execute(format, uri)));
  }
}

module.exports = { buildArguments, runBuild, registerBuild };
