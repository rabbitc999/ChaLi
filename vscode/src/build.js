'use strict';

const fs = require('node:fs');
const path = require('node:path');
const { spawn } = require('node:child_process');
const locator = require('./locator');
const { findProject } = require('./dependencies');
const { llvmOptions } = require('./llvmConfiguration');

/**
 * 构造构建参数，保持文件名和工具路径为独立 argv，不进入 shell。
 * @param {string} format exe、object、assembly 或 llvm。
 * @param {string} source 源文件绝对路径。
 * @param {string} prefix 输出前缀。
 * @param {object} options LLVM 路径及检查开关。
 * @returns {string[]} 编译器实参。
 */
function buildArguments(format, source, prefix, options = {}) {
  if (!['exe', 'object', 'assembly', 'llvm'].includes(format)) throw new Error('未知构建格式');
  const args = ['build', source];
  if (prefix) args.push('--out', prefix);
  if (options.llvmPath) args.push('--llvm', options.llvmPath);
  if (options.checked) args.push('--checked');
  if (options.checked === false) args.push('--unchecked');
  if (format === 'assembly') args.push('--emit-asm');
  if (format === 'object') args.push('--emit-obj');
  if (format === 'llvm') args.push('--emit-llvm');
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
 * 注册格式选择、LLVM 构建命令。
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
      const explicitExecutable = format === 'exe';
      const document = vscode.workspace.textDocuments.find((item) => item.uri.scheme === 'file'
        && path.resolve(item.uri.fsPath) === path.resolve(selected.fsPath));
      if (document?.isDirty && !(await document.save())) throw new Error(text('源文件保存失败，构建已取消', 'The source could not be saved; build cancelled'));
      const cwd = findProject(selected.fsPath) || path.dirname(selected.fsPath);
      const manifest = vscode.workspace.textDocuments.find(item => item.uri.scheme === 'file'
        && path.resolve(item.uri.fsPath) === path.join(cwd, 'chali.toml'));
      if (manifest?.isDirty && !(await manifest.save())) throw new Error(text('项目配置保存失败，构建已取消', 'Project configuration could not be saved; build cancelled'));
      const project = deps.projectInfo ? await deps.projectInfo(selected) : null;
      if (deps.projectInfo && !project?.valid && (project || fs.existsSync(path.join(cwd, 'chali.toml')))) throw new Error(project?.error || text('无法确认项目目标，请等待语言服务就绪', 'Cannot confirm project target; wait for the language server'));
      if (explicitExecutable && project && project.output !== 'executable') throw new Error(text('项目当前导出对象、汇编或 LLVM IR，请使用“构建当前文件”或修改输出设置。', 'The project exports an object, assembly or LLVM IR; use Build Current File or change the output setting.'));
      format = explicitExecutable ? 'exe' : ({ object: 'object', assembly: 'assembly', llvm: 'llvm', executable: 'exe' }[project?.output] || 'exe');
      const compiler = locator.resolveChaLi(cwd, deps.readConfig(), context.extensionPath).chali;
      if (!compiler) throw new Error(text('未找到 ChaLi 编译器，请设置 chali.path', 'ChaLi compiler not found; configure chali.path'));
      const config = vscode.workspace.getConfiguration('chali', selected);
      const configuredDirectory = config.get('build.outputDirectory', '').trim();
      const directory = configuredDirectory ? path.resolve(cwd, configuredDirectory) : path.dirname(selected.fsPath);
      fs.mkdirSync(directory, { recursive: true });
      const prefix = path.join(directory, path.basename(selected.fsPath, path.extname(selected.fsPath)));
      const args = buildArguments(format, selected.fsPath, configuredDirectory ? prefix : undefined, {
        ...llvmOptions(context.extensionPath, config), checked: config.get('build.checked', false),
      });
      const product = prefix + (format === 'assembly' ? '.s' : format === 'object' ? '.o' : format === 'llvm' ? '.ll' : (project ? project.windows : process.platform === 'win32') ? '.exe' : '');
      output.show(true);
      output.appendLine(text(`编译器：${compiler}\n源文件：${selected.fsPath}\n产物：${product}`, `Compiler: ${compiler}\nSource: ${selected.fsPath}\nOutput: ${product}`));
      await vscode.window.withProgress({ location: vscode.ProgressLocation.Notification,
        title: text('ChaLi：发布构建', 'ChaLi: Release build'), cancellable: false },
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
