'use strict';

/** 在已安装的 VSCode 扩展宿主运行真实调试测试，配置与日志全部隔离到仓库 tmp。 */
const fs = require('node:fs');
const path = require('node:path');
const { spawn } = require('node:child_process');
const { stopProcessTree } = require('../src/debugProtocol');
const root = path.resolve(__dirname, '../..');
const code = process.env.CHALI_VSCODE_EXECUTABLE || (process.platform === 'win32'
  ? path.join(process.env.ProgramFiles || 'C:/Program Files', 'Microsoft VS Code/Code.exe') : 'code');
const compiler = path.resolve(process.env.CHALI_BIN || path.join(root, 'vscode/bin', process.platform === 'win32' ? 'chali.exe' : 'chali'));
fs.mkdirSync(path.join(root, 'tmp'), { recursive: true });
const work = fs.mkdtempSync(path.join(root, 'tmp/vscode-extension-host-'));
const workspace = path.join(work, 'workspace');
fs.mkdirSync(path.join(workspace, '.vscode'), { recursive: true });
fs.writeFileSync(path.join(workspace, '.vscode/settings.json'), JSON.stringify({
  'chali.path': compiler, 'chali.debuggerPath': process.env.CHALI_GDB || 'gdb',
  'update.mode': 'none', 'extensions.autoUpdate': false,
}));
const env = { ...process.env, TMPDIR: path.join(root, 'tmp'), TMP: path.join(root, 'tmp'), TEMP: path.join(root, 'tmp'),
  CHALI_EXTENSION_TEST_WORK: work, CHALI_BIN: compiler };
delete env.ELECTRON_RUN_AS_NODE;
/** 超时或输出超预算时回收本次拥有的宿主进程树，保留完整退出证据。 */
async function main() {
  const child = spawn(code, [...(process.argv.includes('--debug') ? ['--inspect-brk-extensions=9333'] : []),
  '--wait', '--disable-gpu', '--disable-extensions', '--skip-welcome', '--skip-release-notes',
  '--disable-workspace-trust', '--user-data-dir=' + path.join(work, 'user'), '--extensions-dir=' + path.join(work, 'extensions'),
  '--extensionDevelopmentPath=' + path.join(root, 'vscode'),
  '--extensionTestsPath=' + path.join(root, 'vscode/test/extensionHost.js'), workspace],
  { cwd: root, env, windowsHide: true, detached: process.platform !== 'win32' });
  let output = '', failure;
  const stop = error => { failure ||= error; void stopProcessTree(child).catch(cleanup => { failure ||= cleanup; }); };
  const collect = chunk => {
    if (Buffer.byteLength(output) + chunk.length > 4194304) stop(new Error('扩展宿主输出超过预算'));
    else output += chunk.toString('utf8');
  };
  child.stdout.on('data', collect); child.stderr.on('data', collect);
  const timer = setTimeout(() => stop(new Error('扩展宿主超时')), 240000);
  let status;
  try {
    status = await new Promise((resolve, reject) => { child.once('error', reject); child.once('close', resolve); });
  } finally {
    clearTimeout(timer);
    await stopProcessTree(child);
    fs.writeFileSync(path.join(work, 'host.log'), output + (failure ? '\n' + failure.stack : ''));
  }
  console.log('真实 VSCode 扩展测试证据：' + work);
  if (failure || status !== 0) throw failure || new Error('扩展宿主退出码 ' + status + '\n' + output);
  const evidence = JSON.parse(fs.readFileSync(path.join(work, 'result.json'), 'utf8'));
  console.log(JSON.stringify(evidence, null, 2));
}
main().catch(error => { console.error(error); process.exitCode = 1; });
