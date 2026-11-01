'use strict';

/** 调试配置规范化：只处理路径和工程入口，语言配置仍由编译器读取。 */
const fs = require('node:fs');
const path = require('node:path');
const { findProject } = require('./dependencies');

/** 保留未指定环境项，null 明确删除继承的环境变量。 */
function debugEnvironment(base, overrides = {}) {
  if (!overrides || typeof overrides !== 'object' || Array.isArray(overrides)) throw new Error('env 必须是对象');
  const result = { ...base };
  for (const [name, value] of Object.entries(overrides)) {
    if (!name || name.includes('=') || name.includes('\0') || (value !== null && (typeof value !== 'string' || value.includes('\0')))) throw new Error('env 包含无效名称或值');
    const actual = process.platform === 'win32' ? Object.keys(result).find(key => key.toLowerCase() === name.toLowerCase()) || name : name;
    if (value === null) delete result[actual];
    else result[actual] = value;
  }
  return result;
}

/** 将单次构建的原始诊断转为编辑器位置；不再触发第二次检查。 */
function buildDiagnostics(output, cwd) {
  const diagnostics = [];
  for (const line of output.replace(/\x1b\[[0-9;]*m/g, '').split(/\r?\n/)) {
    const match = /^(?:chali:\s*)?(.+?):([0-9]+):([0-9]+):\s*(.+)$/.exec(line);
    if (match) diagnostics.push({ path: path.resolve(cwd, match[1]), line: Math.max(0, Number(match[2]) - 1),
      column: Math.max(0, Number(match[3]) - 1), message: match[4] });
  }
  return diagnostics;
}

/** 按选定工作区解析入口，保存相关文档，并拒绝不能在宿主执行的项目目标。 */
async function resolveConfiguration(vscode, folder, configuration, projectInfo) {
  if (vscode.workspace.isTrusted === false) throw new Error('ChaLi 调试需要受信任的工作区');
  const config = { type: 'chali', request: 'launch', name: 'ChaLi', ...configuration };
  const active = vscode.window.activeTextEditor?.document;
  const directory = folder?.uri.fsPath || (active?.uri.scheme === 'file' ? path.dirname(active.uri.fsPath) : null);
  if (!directory) throw new Error('请选择工作区或已保存的 ChaLi 文件');
  if (config.args !== undefined && (!Array.isArray(config.args) || config.args.length > 65536 || config.args.some(value => typeof value !== 'string' || value.includes('\0')))) throw new Error('args 必须是无 NUL 的字符串数组，且数量不得超过限制');
  debugEnvironment({}, config.env);
  if (config.executable) {
    config.executable = path.resolve(directory, config.executable);
    if (!fs.statSync(config.executable).isFile()) throw new Error('executable 不是普通文件');
    config.cwd = path.resolve(directory, config.cwd || path.dirname(config.executable));
    return config;
  }
  let program = config.program || (config.entry !== 'project' && active?.languageId === 'chali' ? active.uri.fsPath : '');
  const candidate = program ? path.resolve(directory, program) : directory;
  const projectRoot = findProject(candidate);
  const manifest = projectRoot && path.join(projectRoot, 'chali.toml');
  for (const document of vscode.workspace.textDocuments) {
    if (document.uri.scheme === 'file' && document.isDirty && (document.uri.fsPath === manifest || document.languageId === 'chali')) {
      if (!(await document.save())) throw new Error('文档保存失败，已取消调试');
    }
  }
  const project = projectInfo ? await projectInfo(vscode.Uri.file(candidate)) : null;
  if (projectRoot && !project?.valid) throw new Error(project?.error || '项目配置尚未就绪');
  if (project?.exists) {
    if (!project.native || project.output !== 'executable') throw new Error('调试要求宿主目标及可执行输出');
    if (config.entry === 'project' || !program) {
      if (!project.main) throw new Error('项目没有配置 main 入口');
      program = path.resolve(path.dirname(vscode.Uri.parse(project.manifestUri).fsPath), project.main);
    }
  }
  if (!program) throw new Error('没有 ChaLi 源文件或项目入口');
  config.program = path.resolve(directory, program);
  if (path.extname(config.program).toLowerCase() !== '.chl' || !fs.statSync(config.program).isFile()) throw new Error('program 必须是已保存的 .chl 文件');
  config.cwd = path.resolve(directory, config.cwd || projectRoot || path.dirname(config.program));
  return config;
}

module.exports = { debugEnvironment, buildDiagnostics, resolveConfiguration };
