'use strict';

/** 真实 GDB 只读查看借用、copy、move 与克隆闭包，禁止调用用户代码。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { spawnSync } = require('node:child_process');
const { DebugClient } = require('./debugClient');
const root = path.resolve(__dirname, '../..'), compiler = process.env.CHALI_BIN;
assert(compiler);
const work = fs.mkdtempSync(path.join(root, 'tmp/debug-closures-native-'));
async function expand(client, value, start = 0, count = 100) {
  assert(value?.variablesReference, JSON.stringify(value));
  return (await client.request('variables', { variablesReference: value.variablesReference, start, count })).variables;
}
async function locals(client, threadId) {
  const stack = await client.request('stackTrace', { threadId });
  const scopes = await client.request('scopes', { frameId: stack.stackFrames[0].id });
  return Object.fromEntries((await expand(client, scopes.scopes[0])).map(value => [value.name, value]));
}
async function fields(client, closure) {
  assert.match(closure.value, /闭包/, JSON.stringify(closure));
  assert.equal(closure.memoryReference, undefined);
  return Object.fromEntries((await expand(client, closure)).map(value => [value.name, value]));
}
async function main() {
  for (const allocator of ['malloc', 'pool']) {
    const directory = path.join(work, allocator); fs.mkdirSync(directory);
    const source = path.join(directory, 'main.chl');
    const lines = [
      'int64 formatCalls=0;',
      'struct Row { int64 number; String text; String toString(){formatCalls+=1;return "formatter";} };',
      'int64 plain(int64 value){return value+1;}',
      'void main(){',
      ' int64 n=4; String text="origin"; Row row;row.number=10;row.text="row";',
      ' let borrowed=int64 fn const(){return n+row.number+text.length();};',
      ' let writable=int64 fn mut(){n+=1;return n;};',
      ' let copied=int64 fn copy(){n+=1;row.number+=1;return n+row.number;};',
      ' let cloned=copied; assert(copied()==16);assert(cloned()==16);assert(cloned()==18);',
      ' Row transfer;transfer.number=30;transfer.text="moved";',
      ' let moved=int64 fn move(){transfer.number+=2;return transfer.number;};assert(moved()==32);',
      ' let function=plain;',
      ' float32 fraction=(float32)1.25;let decimals=float32 fn copy(){fraction+=(float32)0.5;return fraction;};assert(decimals()==(float32)1.75);',
      ' Row mut& alias=&row;let pointer=int64 fn(){return alias.number;};',
      ' int64 data[3];data[1]=7;let array=int64 fn(){return data[1];};',
      ' let nested=int64 fn copy(){return function(2);};',
      ' let stored=copied;let wrapper=int64 fn copy(){return stored();};',
      ' println("READY");',
      ' n=9;row.number=99;text="changed";',
      ' println("KEEP");',
      ' assert(formatCalls==0 && writable()==10 && function(4)==5);',
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
      const borrowed = await fields(client, values.borrowed);
      assert.deepEqual(Object.keys(borrowed).sort(), ['n', 'row', 'text']);
      assert.equal(borrowed.n.value, '4');assert.equal(borrowed.text.value, '"origin"');
      assert(borrowed.n.presentationHint?.attributes.includes('readOnly'));
      assert.equal((await expand(client, borrowed.row))[0].value, '10');
      const whole = await expand(client, values.borrowed);
      assert.deepEqual((await expand(client, values.borrowed, 1, 1)).map(value => value.name), whole.slice(1, 2).map(value => value.name));
      const copied = await fields(client, values.copied), cloned = await fields(client, values.cloned);
      assert.equal(copied.n.value, '5');assert.equal(cloned.n.value, '6');
      assert.equal((await expand(client, copied.row))[0].value, '11');
      assert.equal((await expand(client, cloned.row))[0].value, '12');
      assert.equal((await expand(client, (await fields(client, values.moved)).transfer))[0].value, '32');
      assert.equal((await fields(client, values.writable)).n.value, '4');
      const fraction = (await fields(client, values.decimals)).fraction;
      assert.equal(fraction.type, 'float32');assert.equal(fraction.value, '1.75');
      assert.equal((await expand(client, (await fields(client, values.pointer)).alias))[0].value, '10');
      assert.equal((await expand(client, (await fields(client, values.array)).data))[1].value, '7');
      assert.match((await fields(client, values.nested)).function.value, /函数.*plain/);
      const nestedClosure = (await fields(client, values.wrapper)).stored;
      assert.equal((await fields(client, nestedClosure)).n.value, '5');
      assert.match(values.function.value, /函数.*plain/);
      assert.equal((await expand(client, values.function)).length, 0);
      assert(!client.messages.slice(before).some(message => ['continued', 'stopped'].includes(message.event)));
      await client.request('continue', { threadId: stop.threadId });stop = await client.event('stopped');
      await assert.rejects(expand(client, values.borrowed), /过期/);
      const changed = await locals(client, stop.threadId), current = await fields(client, changed.borrowed);
      assert.equal(current.n.value, '9');assert.equal(current.text.value, '"changed"');
      assert.equal((await expand(client, current.row))[0].value, '99');
      assert.equal((await fields(client, changed.copied)).n.value, '5');
      await client.request('continue', { threadId: stop.threadId });
      assert.equal((await client.event('exited')).exitCode, 0);await client.event('terminated');
    } finally { await client.close(); }
  }
  console.log('真实闭包调试：malloc/pool、只读／可写借用、copy／move、克隆、浮点／文本／记录／数组／引用、嵌套闭包、分页及失效拒绝通过');
}
main().catch(error => { console.error(error); process.exitCode = 1; });
