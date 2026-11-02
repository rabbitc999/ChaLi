'use strict';

/** 真实 GDB 核对拥有接口、借用接口与容器内接口的实际类型，不调用用户方法。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { spawnSync } = require('node:child_process');
const { DebugClient } = require('./debugClient');
const root = path.resolve(__dirname, '../..'), compiler = process.env.CHALI_BIN;
assert(compiler);
const work = fs.mkdtempSync(path.join(root, 'tmp/debug-interfaces-native-'));
async function expand(client, value, start = 0, count = 100) {
  assert(value?.variablesReference, JSON.stringify(value));
  return (await client.request('variables', { variablesReference: value.variablesReference, start, count })).variables;
}
async function locals(client, threadId) {
  const stack = await client.request('stackTrace', { threadId });
  const scopes = await client.request('scopes', { frameId: stack.stackFrames[0].id });
  return Object.fromEntries((await expand(client, scopes.scopes[0])).map(value => [value.name, value]));
}
async function main() {
  for (const allocator of ['malloc', 'pool']) {
    const directory = path.join(work, allocator); fs.mkdirSync(directory);
    const source = path.join(directory, 'main.chl');
    const lines = [
      'int64 formatCalls=0;',
      'interface Read { int64 read(); };',
      'struct Inner { int64 number; };',
      'struct Row : Read { Inner nested; String text; int64 read(){return self.nested.number;} String toString(){formatCalls=formatCalls+1;return "formatter";} };',
      'void main(){',
      ' Row row; row.nested.number=41; row.text="世界";',
      ' Read owned=row; Read const& borrowed=&row; Read mut& writable=&row;',
      ' List<Read> items; items.push(owned);',
      ' println("READY");',
      ' row.nested.number=99;',
      ' println("KEEP");',
      ' assert(formatCalls==0 && owned.read()==41 && borrowed.read()==99 && writable.read()==99);',
      '}',
    ];
    fs.writeFileSync(source, lines.join('\n'));
    const prefix = path.join(directory, 'program');
    const result = spawnSync(compiler, ['aot', source, '--debug', '--checked', '--allocator', allocator, '--out', prefix],
      { cwd: root, env: process.env, encoding: 'utf8', windowsHide: true, timeout: 180000 });
    assert.equal(result.status, 0, result.stdout + result.stderr);
    const client = new DebugClient(root, compiler);
    try {
      await client.request('initialize', { adapterID: 'chali' });
      await client.request('launch', { executable: prefix + (process.platform === 'win32' ? '.exe' : ''), cwd: root });
      await client.event('initialized');
      await client.request('setBreakpoints', { source: { path: source }, breakpoints: [' println("READY");', ' println("KEEP");'].map(text => ({ line: lines.indexOf(text) + 1 })) });
      await client.request('configurationDone');
      let stop = await client.event('stopped');
      const before = client.messages.length;
      const values = await locals(client, stop.threadId);
      for (const name of ['owned', 'borrowed', 'writable']) {
        assert.match(values[name].value, /Row/, JSON.stringify(values[name]));
        assert.equal(values[name].memoryReference, undefined);
        const fields = await expand(client, values[name]);
        assert.deepEqual(fields.map(item => item.name), ['nested', 'text'], JSON.stringify({ name, value: values[name], fields }));
        assert.equal(fields[1].value, '"世界"');
        assert.equal((await expand(client, fields[0]))[0].value, '41');
        assert.deepEqual((await expand(client, values[name], 1, 1)).map(item => item.name), ['text']);
      }
      const item = (await expand(client, values.items))[0];
      assert.match(item.value, /Row/);
      assert.equal((await expand(client, (await expand(client, item))[0]))[0].value, '41');
      assert(!client.messages.slice(before).some(message => ['continued', 'stopped'].includes(message.event)));
      await client.request('continue', { threadId: stop.threadId });
      stop = await client.event('stopped');
      await assert.rejects(expand(client, values.owned), /过期/);
      const changed = await locals(client, stop.threadId);
      assert.equal((await expand(client, (await expand(client, changed.borrowed))[0]))[0].value, '99');
      assert.equal((await expand(client, (await expand(client, changed.owned))[0]))[0].value, '41');
      await client.request('continue', { threadId: stop.threadId });
      assert.equal((await client.event('exited')).exitCode, 0);
      await client.event('terminated');
    } finally { await client.close(); }
  }
  console.log('真实接口调试：malloc/pool、拥有／只读／可写借用、实际类型、嵌套／文本、容器、分页与失效拒绝通过');
}
main().catch(error => { console.error(error); process.exitCode = 1; });
