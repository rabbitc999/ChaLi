'use strict';

/** 远程地址仅允许 TCP，身份读取只使用已核对 PID 与适配器自身路径。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { remoteEndpoint, remoteImage } = require('../src/debugRemote');

async function main() {
  assert.equal(remoteEndpoint(), null);
  assert.equal(remoteEndpoint({ host: '127.0.0.1', port: 2345 }), '127.0.0.1:2345');
  assert.equal(remoteEndpoint({ host: '::1', port: 2345 }), '[::1]:2345');
  assert.equal(remoteEndpoint({ host: 'target.example', port: 2345 }), 'target.example:2345');
  for (const value of [null, [], 'localhost:1', { host: '|run', port: 1 }, { host: 'a\nrun', port: 1 },
    { host: '-device', port: 1 }, { host: 'a..b', port: 1 }, { host: 'localhost', port: '1' },
    { host: 'localhost', port: 0 }, { host: 'localhost', port: 65536 }, { host: 'localhost', port: 1, command: 'run' }])
    assert.throws(() => remoteEndpoint(value));
  if (process.platform === 'linux') assert.equal((await remoteImage(process.execPath)).argument, process.arch === 'arm64' ? '$x0' : '$rdi');
  else await assert.rejects(remoteImage(process.execPath), /ELF/);
  const directory = fs.mkdtempSync(path.resolve(__dirname, '../../tmp/debug-remote-format-'));
  const file = path.join(directory, 'image');
  const valid = Buffer.alloc(20); valid.set([127, 69, 76, 70, 2, 1, 1]);
  valid.writeUInt16LE(3, 16); valid.writeUInt16LE(62, 18);
  fs.writeFileSync(file, valid); assert.equal((await remoteImage(file)).argument, '$rdi');
  const arm = Buffer.from(valid); arm.writeUInt16LE(183, 18);
  fs.writeFileSync(file, arm); assert.equal((await remoteImage(file)).argument, '$x0');
  for (const [offset, value] of [[0, 0], [4, 1], [5, 2], [7, 9], [16, 1], [18, 3]]) {
    const wrong = Buffer.from(valid); wrong[offset] = value;
    fs.writeFileSync(file, wrong); await assert.rejects(remoteImage(file), /ELF/);
  }
  console.log('远程 TCP 配置与目标格式约束通过');
}
main().catch(error => { console.error(error); process.exitCode = 1; });
