'use strict';

/** 真实 GDB 核对 malloc/pool 容器快照、分页、嵌套记录和暂停引用失效。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { spawnSync } = require('node:child_process');
const { DebugClient } = require('./debugClient');
const root = path.resolve(__dirname, '../..'), compiler = process.env.CHALI_BIN;
assert(compiler);
const work = fs.mkdtempSync(path.join(root, 'tmp/debug-containers-native-'));
async function expand(client, variable, start = 0, count = 100) {
  assert(variable?.variablesReference, JSON.stringify(variable));
  return (await client.request('variables', { variablesReference: variable.variablesReference, start, count })).variables;
}
async function locals(client, threadId) {
  const stack = await client.request('stackTrace', { threadId });
  const scopes = await client.request('scopes', { frameId: stack.stackFrames[0].id });
  return Object.fromEntries((await expand(client, scopes.scopes[0])).map(item => [item.name, item]));
}
async function main() {
  for (const allocator of ['malloc', 'pool']) {
    const directory = path.join(work, allocator); fs.mkdirSync(directory);
    const source = path.join(directory, 'main.chl');
    const lines = [
      'int64 formatCalls=0;',
      'struct Pair { int64 first; String second; String toString(){ formatCalls=formatCalls+1; return "user formatter"; } };',
      'struct Box { Pair pair; int64 tail; };',
      'void main(){',
      ' List<int64> numbers; int64 i=0; while(i<12){ numbers.push(i*2); i=i+1; }',
      ' List<String> words; words.push("世界");',
      ' Pair row; row.first=7; row.second="nested";',
      ' List<Pair> rows; rows.push(row);',
      ' Map<String,Pair> lookup; lookup["key"]=row;',
      ' Set<int64> unique; unique.add(9); unique.add(3);',
      ' List<List<int64>> nested; nested.push(numbers);',
      ' List<float64> decimals; decimals.push(1.25);',
      ' Box box; box.pair=row; box.tail=99; List<Box> boxes; boxes.push(box);',
      ' println("READY");',
      ' numbers.push(99);',
      ' println("KEEP");',
      ' assert(formatCalls==0);',
      '}',
    ];
    fs.writeFileSync(source, lines.join('\n'));
    const prefix = path.join(directory, 'program');
    const built = spawnSync(compiler, ['aot', source, '--debug', '--checked', '--allocator', allocator, '--out', prefix],
      { cwd: root, env: process.env, encoding: 'utf8', windowsHide: true, timeout: 180000 });
    assert.equal(built.status, 0, built.stdout + built.stderr);
    const client = new DebugClient(root, compiler);
    try {
      await client.request('initialize', { adapterID: 'chali' });
      await client.request('launch', { executable: prefix + (process.platform === 'win32' ? '.exe' : ''), cwd: root });
      await client.event('initialized');
      await client.request('setBreakpoints', { source: { path: source }, breakpoints: [{ line: lines.indexOf(' println("READY");') + 1 }, { line: lines.indexOf(' println("KEEP");') + 1 }] });
      await client.request('configurationDone');
      let stop = await client.event('stopped');
      const before = client.messages.length;
      const values = await locals(client, stop.threadId);
      assert.equal(values.numbers.value, 'List[12]', JSON.stringify(values.numbers));
      assert.equal(values.numbers.memoryReference, undefined);
      assert.deepEqual((await expand(client, values.numbers, 4, 2)).map(item => item.value), ['8', '10']);
      assert.equal((await expand(client, values.words))[0].value, '"世界"');
      const fields = await expand(client, (await expand(client, values.rows))[0]);
      assert.deepEqual(fields.map(item => [item.name, item.value]), [['first', '7'], ['second', '"nested"']]);
      const entry = await expand(client, (await expand(client, values.lookup))[0]);
      assert.equal(entry[0].value, '"key"');
      assert.equal((await expand(client, entry[1]))[0].value, '7');
      assert.deepEqual((await expand(client, values.unique)).map(item => item.value).sort(), ['3', '9']);
      const inner = (await expand(client, values.nested))[0];
      assert.equal(inner.value, 'List[12]');
      assert.equal((await expand(client, inner, 2, 1))[0].value, '4');
      assert.equal((await expand(client, values.decimals))[0].value, '1.25');
      const boxed = await expand(client, (await expand(client, values.boxes))[0]);
      const pairFields = await expand(client, boxed[0]);
      assert.deepEqual(pairFields.map(item => item.value), ['7', '"nested"']);
      assert.equal(boxed[1].value, '99');
      assert(!client.messages.slice(before).some(message => ['continued', 'stopped'].includes(message.event)));
      await client.request('continue', { threadId: stop.threadId });
      stop = await client.event('stopped');
      await assert.rejects(expand(client, values.numbers), /过期/);
      assert.equal((await locals(client, stop.threadId)).numbers.value, 'List[13]');
      await client.request('continue', { threadId: stop.threadId });
      assert.equal((await client.event('exited')).exitCode, 0, '查看不得调用用户格式化方法或改变程序结果');
      await client.event('terminated');
    } finally { await client.close(); }
  }
  console.log('真实容器调试：malloc/pool、分页、文本、记录、Map、Set、不恢复执行及过期拒绝通过');
}
main().catch(error => { console.error(error); process.exitCode = 1; });
