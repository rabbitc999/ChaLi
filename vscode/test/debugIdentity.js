'use strict';

/** 真实本机子进程的映像与创建身份核对，不附加、不向目标写入或调用函数。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const crypto = require('node:crypto');
const { spawn } = require('node:child_process');
const { processId, imageHash, prepareLocalIdentity } = require('../src/debugIdentity');
const { stopProcessTree } = require('../src/debugProtocol');
const root = path.resolve(__dirname, '../..'), temporary = path.join(root, 'tmp');
const work = fs.mkdtempSync(path.join(temporary, 'debug-identity-'));
const wrong = path.join(work, 'wrong-image'); fs.writeFileSync(wrong, 'wrong image\n');

async function main() {
  assert.equal(processId('123'), 123);
  for (const value of [0, -1, '01', '1;run', '1\n-exec-run', 1.5, null, 2147483648]) assert.throws(() => processId(value));
  assert.equal(await imageHash(wrong), crypto.createHash('sha256').update('wrong image\n').digest('hex'));
  const child = spawn(process.execPath, ['-e', 'setInterval(() => {}, 1000)'], { windowsHide: true, detached: process.platform !== 'win32' });
  let identity;
  try {
    const environment = { ...process.env, TMPDIR: temporary, TMP: temporary, TEMP: temporary };
    await assert.rejects(prepareLocalIdentity(child.pid, wrong, environment), /身份不匹配/);
    assert.equal(child.exitCode, null);
    identity = await prepareLocalIdentity(child.pid, process.execPath, environment);
    assert.equal(identity.pid, child.pid); assert(/^[0-9]+$/.test(identity.start));
    await identity.verify();
    await stopProcessTree(child);
    await assert.rejects(identity.verify());
  } finally { await identity?.release(); await stopProcessTree(child); }
  console.log('附加身份：PID 输入、实际映像 SHA256、创建身份复核、错误符号及目标退出拒绝通过');
}
main().catch(error => { console.error(error); process.exitCode = 1; });
