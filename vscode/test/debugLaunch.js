'use strict';

/** 使用真实编译器及 GDB 核对已有产物、独立终端与单次构建诊断。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { spawnSync } = require('node:child_process');
const { DebugClient } = require('./debugClient');
const root = path.resolve(__dirname, '../..');
const compiler = process.env.CHALI_BIN || path.join(root, 'chali.exe');
const work = fs.mkdtempSync(path.join(root, 'tmp/debug-launch-'));

/** 读取终端输出，避免把 MI 输出当作应用标准流。 */
async function outputUntil(client, expression) {
  let output = '';
  for (let count = 0; count < 100; ++count) {
    const message = await client.event('chali/terminalOutput', 20000).catch(error => { throw new Error('等待终端内容 ' + expression + '：' + error.message + '\n已收到：' + output); });
    output += message.output;
    if (expression.test(output)) return output;
  }
  assert.fail('终端输出未包含预期内容：' + output);
}

async function main() {
  const source = path.join(work, 'input.chl');
  fs.writeFileSync(source, 'void main(List<String> args) {\nfor(String item in args){println("ARG [{}]",item);}\nprintln("READY {}",len(args));\nString value=std::core::readLine();\nprintln("INPUT [{}]",value);\nassert(std::core::readLine()=="");\nprintln("INPUT EOF");\n}\n');
  const prefix = path.join(work, 'input');
  const build = spawnSync(compiler, ['build', source, '--checked', '--debug', '--out', prefix], { cwd: root, env: process.env, encoding: 'utf8', timeout: 180000, windowsHide: true });
  assert.equal(build.status, 0, build.stdout + build.stderr);
  const executable = prefix + (process.platform === 'win32' ? '.exe' : '');
  const client = new DebugClient(root, path.join(work, 'compiler-must-not-be-called'));
  try {
    await client.request('initialize', { adapterID: 'chali', supportsProgressReporting: true });
    const args = ['a b', '中文', '', '学生🙂', 'x"y', 'C:\\tail\\'];
    await client.request('launch', { executable, cwd: work, console: 'integratedTerminal', args, env: { CHALI_DEBUG_TEST: 'yes' }, stopOnEntry: true });
    await client.event('initialized'); await client.event('chali/terminal');
    await client.request('configurationDone');
    let stop = await client.event('stopped'); assert.equal(stop.reason, 'breakpoint');
    await client.request('continue', { threadId: stop.threadId });
    const output = await outputUntil(client, /READY 6/);
    for (const value of args) assert(output.includes('ARG [' + value + ']'), output);
    await client.request('chali/input', { text: 'discard-on-restart' });
    await client.request('restart');
    await client.event('chali/terminal');
    stop = await client.event('stopped');
    await client.request('continue', { threadId: stop.threadId });
    await outputUntil(client, /READY 6/);
    await assert.rejects(client.request('chali/input', { text: 'x'.repeat(65537) }), /限制/);
    await client.request('chali/input', { text: '你好 ChaLi🙂\n' });
    assert.match(await outputUntil(client, /INPUT \[你好 ChaLi🙂\]/), /INPUT/);
    await client.request('chali/input', { text: '', end: true });
    await outputUntil(client, /INPUT EOF/);
    await client.event('terminated');
    assert(!client.messages.some(message => message.event === 'progressStart'), '已有产物不能再次构建');
  } finally {
    fs.writeFileSync(path.join(work, 'messages.json'), JSON.stringify({ messages: client.messages, stderr: client.stderr }, null, 2));
    await client.close();
  }
  const failed = new DebugClient(root, compiler);
  try {
    const bad = path.join(work, 'bad.chl'); fs.writeFileSync(bad, 'void main(){int64 value=unknownName;}\n');
    await failed.request('initialize', { adapterID: 'chali' });
    await assert.rejects(failed.request('launch', { program: bad, cwd: root }), /构建失败/);
    const diagnostics = failed.messages.filter(message => message.event === 'chali/buildDiagnostics').flatMap(message => message.body.diagnostics);
    assert(diagnostics.some(item => item.path === bad && item.line === 0 && /unknownName/.test(item.message)), JSON.stringify(diagnostics));
    await failed.event('terminated');
  } finally { await failed.close(); }
  console.log('DAP 工程启动：已有产物不重编译、UTF-8 参数、独立标准输入、输入预算及单次构建诊断通过');
}
main().catch(error => { console.error(error); process.exitCode = 1; });
