'use strict';

/** 工程入口、环境和已有产物的实际配置转换验收。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { pathToFileURL, fileURLToPath } = require('node:url');
const { resolveConfiguration, debugEnvironment, buildDiagnostics } = require('../src/debugConfiguration');
const root = path.resolve(__dirname, '../..');
const work = fs.mkdtempSync(path.join(root, 'tmp/debug-config-'));
const uri = file => ({ scheme: 'file', fsPath: path.resolve(file), toString: () => pathToFileURL(file).href });

async function main() {
  const project = path.join(work, 'project'); fs.mkdirSync(project);
  const main = path.join(project, 'main.chl'), other = path.join(project, 'other.chl');
  for (const file of [main, other]) fs.writeFileSync(file, 'void main(){}');
  fs.writeFileSync(path.join(project, 'chali.toml'), '[chali]\nmain="main.chl"\n');
  let saved = 0;
  const document = { uri: uri(other), languageId: 'chali', isDirty: true, async save() { ++saved; this.isDirty = false; return true; } };
  const vscode = { workspace: { isTrusted: true, textDocuments: [document] }, window: { activeTextEditor: { document } },
    Uri: { file: uri, parse: value => uri(fileURLToPath(value)) } };
  const folder = { uri: uri(project) };
  const info = { valid: true, exists: true, main: 'main.chl', native: true, output: 'executable', manifestUri: uri(path.join(project, 'chali.toml')).toString() };
  const provider = async () => info;
  let config = await resolveConfiguration(vscode, folder, { entry: 'project' }, provider);
  assert.equal(config.program, main); assert.equal(config.cwd, project); assert.equal(saved, 1);
  config = await resolveConfiguration(vscode, folder, {}, provider); assert.equal(config.program, other);
  config = await resolveConfiguration(vscode, folder, { executable: path.relative(project, process.execPath), env: { X: null }, args: ['a b', '"'] }, provider);
  assert.equal(config.executable, process.execPath); assert.equal(saved, 1);
  await assert.rejects(resolveConfiguration(vscode, folder, { entry: 'project' }, async () => ({ ...info, native: false })), /宿主/);
  await assert.rejects(resolveConfiguration(vscode, folder, { entry: 'project' }, async () => ({ ...info, output: 'object' })), /可执行/);
  await assert.rejects(resolveConfiguration(vscode, folder, { args: [1] }, provider), /args/);
  vscode.workspace.isTrusted = false; await assert.rejects(resolveConfiguration(vscode, folder, {}, provider), /受信任/);
  assert.deepEqual(debugEnvironment({ A: 'a', B: 'b' }, { A: null, B: 'new', C: '' }), { B: 'new', C: '' });
  assert.throws(() => debugEnvironment({}, { A: 3 }), /无效/);
  assert.throws(() => debugEnvironment({}, { A: 'x\0y' }), /无效/);
  if (process.platform === 'win32') assert.deepEqual(debugEnvironment({ Path: 'old' }, { PATH: 'new' }), { Path: 'new' });
  const diagnostics = buildDiagnostics('chali: '+other+':2:3: unknown name\n 2 | ignored\n', root);
  assert.deepEqual(diagnostics, [{ path: other, line: 1, column: 2, message: 'unknown name' }]);
  assert.deepEqual(buildDiagnostics('chali: '+other+' 第 4 行第 5 列：引用了没声明的变量\n 4 | main\n', root),
    [{ path: other, line: 3, column: 4, message: '引用了没声明的变量' }]);
  assert.deepEqual(buildDiagnostics('chali: '+other+' 第 7 行：整行诊断\n', root),
    [{ path: other, line: 6, column: 0, message: '整行诊断' }]);
  assert.deepEqual(buildDiagnostics('chali: 没有位置的错误\n', root), []);
  console.log('调试配置：当前文件、项目入口、多工作区路径、保存、目标拒绝、已有产物、环境和构建诊断通过');
}
main().catch(error => { console.error(error); process.exitCode = 1; });
