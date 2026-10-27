'use strict';

/**
 * 把 chali 编译器内嵌进插件包（复制到本扩展的 bin/ 下）。
 *
 * `vsce package` 会通过 `vscode:prepublish` 自动跑它，所以打出来的 .vsix 自带编译器：
 * 装完就能用诊断 / 补全，不需要用户先构建工具链。
 *
 * 用法（在 vscode/ 里或任意目录都行）：
 *   node tools/embed-compiler.js                 # 自动找：--from → CHALI_BIN → ../chali[.exe] → PATH
 *   node tools/embed-compiler.js --from ../ChaLi.exe
 *
 * 找不到就退出 2，并说明怎么构建（在项目根目录跑 make）。
 */

const fs = require('node:fs');
const path = require('node:path');

const locator = require('../src/locator');

const EXT_ROOT = path.join(__dirname, '..');
const TARGET_DIR = path.join(EXT_ROOT, 'bin');
const TARGET = path.join(TARGET_DIR, 'chali' + locator.EXE);
const SKILL_SOURCE = path.join(EXT_ROOT, '..', 'skills', 'chali-programming', 'SKILL.md');
const SKILL_TARGET = path.join(EXT_ROOT, 'skills', 'chali-programming', 'SKILL.md');

/**
 * 取 `--from <路径>`。
 *
 * @returns {string} 参数值；没给返回空串。
 */
function fromArgument() {
  const index = process.argv.indexOf('--from');
  if (index < 0) return '';
  const value = process.argv[index + 1];
  if (!value) {
    console.error('--from 后面要跟编译器路径');
    process.exit(2);
  }
  return value;
}

/**
 * 按顺序找源编译器。
 *
 * @returns {{file: string|null, from: string}} 源文件与来源说明。
 */
function findSource() {
  const explicit = fromArgument() || (process.env.CHALI_BIN || '').trim();
  if (explicit) {
    if (!locator.isFile(explicit)) {
      console.error(`找不到指定的编译器：${explicit}`);
      process.exit(2);
    }
    return { file: explicit, from: '参数 / CHALI_BIN' };
  }
  const projectRoot = path.join(EXT_ROOT, '..');
  const candidates = [
    ...['static', 'external'].map((variant) => path.join(projectRoot,
      `chali-${require('../package.json').chaliCompilerVersion}-llvm-${variant}-windows-x64.exe`)).filter(() => process.platform === 'win32'),
    path.join(projectRoot, 'chali' + locator.EXE),
    path.join(projectRoot, 'build', 'bin', 'chali' + locator.EXE),
  ];
  for (const candidate of candidates) {
    if (locator.isFile(candidate)) return { file: candidate, from: '项目根目录' };
  }
  const onPath = locator.findOnPath('chali');
  if (onPath) return { file: onPath, from: 'PATH' };
  return { file: null, from: '' };
}

/** 复制（内容一致就跳过），并打印来源与大小。 */
function main() {
  fs.mkdirSync(path.dirname(SKILL_TARGET), { recursive: true });
  fs.copyFileSync(SKILL_SOURCE, SKILL_TARGET);
  const llvm = path.join(EXT_ROOT, '..', 'library', 'llvm');
  const notices = path.join(EXT_ROOT, 'third-party', 'llvm');
  fs.mkdirSync(notices, { recursive: true });
  fs.copyFileSync(path.join(llvm, 'LICENSE.TXT'), path.join(notices, 'LICENSE.TXT'));
  fs.cpSync(path.join(llvm, 'licenses'), path.join(notices, 'licenses'), { recursive: true });
  const runtimeNotices = path.join(EXT_ROOT, 'third-party', 'runtime');
  fs.mkdirSync(runtimeNotices, { recursive: true });
  for (const name of ['miniz', 'ipage', 'caspp']) {
    fs.copyFileSync(path.join(EXT_ROOT, '..', name === 'miniz' ? 'licenses' : 'library', name, 'LICENSE'),
      path.join(runtimeNotices, `${name}-LICENSE.txt`));
  }
  const source = findSource();
  if (!source.file) {
    console.error('找不到 chali 编译器：先在项目根目录跑 make（产出 ./chali[.exe]），');
    console.error('或用 `--from <路径>` / 环境变量 CHALI_BIN 指定。');
    process.exit(2);
  }
  const report = require('node:child_process').spawnSync(source.file, ['--build-info'], { encoding: 'utf8', windowsHide: true });
  if (report.status !== 0) throw new Error('内嵌编译器缺少可验证的 --build-info：' + report.stderr);
  const buildInfo = JSON.parse(report.stdout);
  const manifest = require(path.join(EXT_ROOT, 'package.json'));
  if (buildInfo.version !== manifest.version || buildInfo.version !== manifest.chaliCompilerVersion)
    throw new Error('VS Code 扩展版本必须与实际编译器版本一致');
  if (process.env.CHALI_LLVM_VARIANT && buildInfo.llvm !== process.env.CHALI_LLVM_VARIANT)
    throw new Error('内嵌编译器的 LLVM 链接方式与 VSIX 变体不一致');
  fs.mkdirSync(TARGET_DIR, { recursive: true });
  fs.writeFileSync(path.join(TARGET_DIR, 'chali-build-info.json'), JSON.stringify(buildInfo, null, 2) + '\n');
  const content = fs.readFileSync(source.file);
  const before = fs.existsSync(TARGET) ? fs.readFileSync(TARGET) : null;
  if (before && before.equals(content)) {
    console.log(`内嵌编译器已是最新：${path.relative(EXT_ROOT, TARGET)}`
      + `（${content.length} 字节，来自${source.from}）`);
    return;
  }
  fs.writeFileSync(TARGET, content);
  if (process.platform !== 'win32') fs.chmodSync(TARGET, 0o755);
  console.log(`已内嵌编译器：${path.relative(EXT_ROOT, TARGET)}`
    + `（${content.length} 字节，来自${source.from}：${source.file}）`);
}

main();
