'use strict';

/**
 * 打 VSIX：内嵌编译器 → 清单自检 → 冒烟测试 → `vsce package`，产物落在 `dist/`。
 *
 * 用法（在 vscode/ 里跑）：
 *   npm run package                 # 全套
 *   npm run package -- --skip-tests # 只内嵌 + 打包
 *   node tools/package.js --dry-run # 只打印要跑的命令
 *   node tools/package.js --out other-dir
 *
 * `vsce` 优先用 PATH 上的那份；没有就退回 `npx --yes @vscode/vsce`（需要网络）。
 * 产物名与 `package.json` 的 name/version 对齐：`dist/chali-editor-<版本>.vsix`。
 */

const fs = require('node:fs');
const path = require('node:path');
const { spawnSync } = require('node:child_process');

const EXT_ROOT = path.join(__dirname, '..');
const manifest = require(path.join(EXT_ROOT, 'package.json'));

/** 取 `--xxx` 后面的值。 */
function option(name, fallback) {
  const index = process.argv.indexOf(name);
  if (index < 0) return fallback;
  const value = process.argv[index + 1];
  if (!value || value.startsWith('--')) {
    console.error(`${name} 后面要跟一个值`);
    process.exit(2);
  }
  return value;
}

const DRY_RUN = process.argv.includes('--dry-run');
const SKIP_TESTS = process.argv.includes('--skip-tests');
const outDir = path.resolve(EXT_ROOT, option('--out', 'dist'));
const variant = option('--llvm-variant', 'external');
if (!['static', 'external'].includes(variant)) {
  throw new Error('--llvm-variant 必须为 static 或 external');
}
const compiler = option('--compiler', '');
if (compiler) process.env.CHALI_BIN = path.resolve(EXT_ROOT, compiler);
process.env.CHALI_LLVM_VARIANT = variant;
const target = option('--target', '');
const suffix = `-llvm-${variant}${target ? '-' + target : ''}`;
const vsix = path.join(outDir, `${manifest.name}-${manifest.version}${suffix}.vsix`);

/**
 * 跑一条命令，失败就退出。
 *
 * @param {string} command 程序。
 * @param {string[]} args 参数。
 * @param {boolean} [shell] 是否经系统 shell（Windows 上的 vsce / npx 需要）。
 * @returns {void}
 */
function run(command, args, shell = false) {
  console.log(`\n$ ${command} ${args.join(' ')}`);
  if (DRY_RUN) return;
  // node 用绝对路径直连（`shell: true` 会把带空格的 "C:\\Program Files\\..." 拆坏）；
  // `vsce` / `npx` 在 Windows 上是 .cmd 垫片，才需要 shell。
  // 走 shell 时把命令与参数拼成一行（带空格的参数加引号）—— 给 spawnSync 同时传
  // args 数组和 shell:true 会触发 Node 的 DEP0190 警告，这样写就没有了。
  const quote = (value) => (/[\s"]/.test(value) ? `"${value.replace(/"/g, '\\"')}"` : value);
  const result = shell
    ? spawnSync([command, ...args.map(quote)].join(' '), {
        cwd: EXT_ROOT,
        stdio: 'inherit',
        shell: true,
      })
    : spawnSync(command, args, {
        cwd: EXT_ROOT,
        stdio: 'inherit',
      });
  if (result.status !== 0) {
    const error = new Error(`失败（退出码 ${result.status ?? 'null'}）：${command}`);
    error.exitCode = result.status || 1;
    throw error;
  }
}

/** PATH 上有没有这个程序。 */
function onPath(command) {
  const probe = process.platform === 'win32' ? 'where' : 'which';
  return spawnSync(probe, [command], { stdio: 'ignore' }).status === 0;
}

/** 挑一个能用的 vsce。 */
function vsce() {
  if (onPath('vsce')) return ['vsce', []];
  console.log('（PATH 上没有 vsce，退回 npx --yes @vscode/vsce）');
  return ['npx', ['--yes', '@vscode/vsce']];
}

function main() {
  console.log(`== 打包 ${manifest.displayName || manifest.name} ${manifest.version} ==`);
  run(process.execPath, [path.join('tools', 'embed-compiler.js')]);
  const manifestPath = path.join(EXT_ROOT, 'package.json');
  const original = fs.readFileSync(manifestPath);
  try {
    const releaseManifest = require('../src/llvmConfiguration').variantManifest(JSON.parse(original), variant);
    if (!DRY_RUN) fs.writeFileSync(manifestPath, JSON.stringify(releaseManifest, null, 2) + '\n');
    run(process.execPath, [path.join('tools', 'check-manifest.js')]);
    if (!SKIP_TESTS) {
      // 业务回归核对原有输出；完成提示由编译器专项独立验证默认与静默两种行为。
      const originalQuiet = process.env.CHALI_QUIET;
      process.env.CHALI_QUIET = '1';
      try {
        run(process.execPath, [path.join('test', 'debugConfiguration.js')]);
        run(process.execPath, [path.join('test', 'debugIntegration.js')]);
        run(process.execPath, [path.join('test', 'debugMetadata.js')]);
        run(process.execPath, [path.join('test', 'debugMemory.js')]);
        run(process.execPath, [path.join('test', 'debugFrameContext.js')]);
        run(process.execPath, [path.join('test', 'debugReferencesNative.js')]);
        run(process.execPath, [path.join('test', 'debugStorageLifetimeNative.js')]);
        run(process.execPath, [path.join('test', 'debugValue.js')]);
        run(process.execPath, [path.join('test', 'debugObjectMemory.js')]);
        run(process.execPath, [path.join('test', 'debugContainers.js')]);
        run(process.execPath, [path.join('test', 'debugContainerMetadata.js')]);
        run(process.execPath, [path.join('test', 'debugContainersNative.js')]);
        run(process.execPath, [path.join('test', 'debugCallbackContext.js')]);
        run(process.execPath, [path.join('test', 'debugDynamicValues.js')]);
        run(process.execPath, [path.join('test', 'debugInterfacesNative.js')]);
        run(process.execPath, [path.join('test', 'debugClosureValues.js')]);
        run(process.execPath, [path.join('test', 'debugClosuresNative.js')]);
        run(process.execPath, [path.join('test', 'debugExpression.js')]);
        run(process.execPath, [path.join('test', 'debugExpressionNative.js')]);
        run(process.execPath, [path.join('test', 'debugAssignment.js')]);
        run(process.execPath, [path.join('test', 'debugStringAssignment.js')]);
        run(process.execPath, [path.join('test', 'debugStringAssignmentNative.js')]);
        run(process.execPath, [path.join('test', 'debugAssignmentNative.js')]);
        run(process.execPath, [path.join('test', 'debugMirrorAssignment.js')]);
        run(process.execPath, [path.join('test', 'debugMirrorAssignmentNative.js')]);
        run(process.execPath, [path.join('test', 'debugThreads.js')]);
        run(process.execPath, [path.join('test', 'debugThreadsNative.js')]);
        run(process.execPath, [path.join('test', 'debugIdentity.js')]);
        run(process.execPath, [path.join('test', 'debugSourceMap.js')]);
        run(process.execPath, [path.join('test', 'debugSourceMapNative.js')]);
        run(process.execPath, [path.join('test', 'debugAttach.js')]);
        run(process.execPath, [path.join('test', 'debugAttachNative.js')]);
        run(process.execPath, [path.join('test', 'debugRemote.js')]);
        run(process.execPath, [path.join('test', 'debugOptimized.js')]);
        run(process.execPath, [path.join('test', 'debugOptimizedNative.js')]);
        run(process.execPath, [path.join('test', 'debugLibraries.js')]);
        run(process.execPath, [path.join('test', 'debugFrames.js')]);
        run(process.execPath, [path.join('test', 'debugDisassemble.js')]);
        run(process.execPath, [path.join('test', 'debugFailure.js')]);
        run(process.execPath, [path.join('test', 'debugBreakpoints.js')]);
        run(process.execPath, [path.join('test', 'debugProtocol.js')]);
        run(process.execPath, [path.join('test', 'debugTerminal.js')]);
        run(process.execPath, [path.join('test', 'smoke.js')]);
        run(process.execPath, [path.join('test', 'dependencies.js')]);
        run(process.execPath, [path.join('test', 'macros.js')]);
        run(process.execPath, [path.join('test', 'lspCancellation.js')]);
        run(process.execPath, [path.join('test', 'semanticProviders.js')]);
        run(process.execPath, [path.join('test', 'editorIntegration.js')]);
        run(process.execPath, [path.join('test', 'macroHighlighting.js')]);
        run(process.execPath, [path.join('test', 'llvmConfiguration.js')]);
        run(process.execPath, [path.join('test', 'build.js')]);
        run(process.execPath, [path.join('tools', 'test-extension-host.js')]);
      } finally {
        if (originalQuiet === undefined) delete process.env.CHALI_QUIET;
        else process.env.CHALI_QUIET = originalQuiet;
      }
    }
    const [command, prefix] = vsce();
    fs.mkdirSync(outDir, { recursive: true });
    run(command, [...prefix, 'package', '--out', vsix, ...(target ? ['--target', target] : []), '--allow-missing-repository',
      '--baseContentUrl', 'https://github.com/rabbitc999/ChaLi/blob/master/vscode',
      '--baseImagesUrl', 'https://github.com/rabbitc999/ChaLi/raw/master/vscode'],
        process.platform === 'win32');
  } finally {
    fs.writeFileSync(manifestPath, original);
  }
  if (DRY_RUN) {
    console.log(`\n（dry-run：没有真的产出 ${path.relative(EXT_ROOT, vsix)}）`);
    return;
  }
  const size = fs.statSync(vsix).size;
  console.log(`\n已打包：${path.relative(EXT_ROOT, vsix)}（${(size / 1048576).toFixed(1)} MB）`);
}

try { main(); } catch (error) { console.error(error.message); process.exitCode = error.exitCode || 1; }
