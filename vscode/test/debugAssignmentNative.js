'use strict';

/** 真实 GDB 修改基础值后恢复执行，核对物理槽、类型、可变性、作用域和快照失效。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { spawnSync } = require('node:child_process');
const { DebugClient } = require('./debugClient');
const root = path.resolve(__dirname, '../..'), compiler = process.env.CHALI_BIN;
assert(compiler);
const work = fs.mkdtempSync(path.join(root, 'tmp/debug-assignment-'));
const source = path.join(work, 'main.chl');
const lines = [
  'void inspect(int64 argument){',
  ' println("PARAMETER");',
  ' assert(argument==12);',
  '}',
  'void main(){',
  ' int8 signed8=-7;int16 signed16=-300;int32 signed32=-70000;int64 signed64=-9000000000;',
  ' uint8 unsigned8=255;uint16 unsigned16=65000;uint32 unsigned32=(uint32)4000000000;uint64 unsigned64=(uint64)-1;',
  ' float32 single=(float32)1.5;float64 real=2.25;bool truth=true;char letter=\'A\';',
  ' const int64 fixed=8;String owned="kept";',
  ' int64 borrowed=5;int64& alias=&borrowed;',
  ' int64 values[]=new[2];values[0]=123;',
  ' int64 visible=41;',
  ' println("READY");',
  ' assert(signed8==-9 && signed16==6 && signed32==7 && signed64==-9000000001);',
  ' assert(unsigned8==4 && unsigned16==8 && unsigned32==9 && unsigned64==(uint64)-2);',
  ' assert(single==(float32)0.1 && real==2.5 && !truth && letter==\'Z\');',
  ' assert(fixed==8 && owned=="kept" && borrowed==5 && alias==5 && values[0]==123);',
  ' {int64 visible=42;',
  '  println("INNER");',
  '  assert(visible==99);',
  ' }',
  ' println("AFTER");',
  ' assert(visible==77);',
  ' inspect(11);',
  '}',
];
fs.writeFileSync(source, lines.join('\n'));

async function view(client, threadId) {
  const frameId = (await client.request('stackTrace', { threadId })).stackFrames[0].id;
  const scope = (await client.request('scopes', { frameId })).scopes[0].variablesReference;
  const values = Object.fromEntries((await client.request('variables', { variablesReference: scope })).variables.map(value => [value.name, value]));
  return { frameId, scope, values };
}

async function main() {
  for (const allocator of ['malloc', 'pool']) {
    const prefix = path.join(work, allocator);
    const built = spawnSync(compiler, ['aot', source, '--debug', '--checked', '--allocator', allocator, '--out', prefix],
      { cwd: root, env: process.env, encoding: 'utf8', windowsHide: true, timeout: 180000 });
    assert.equal(built.status, 0, built.stdout + built.stderr);
    const client = new DebugClient(root, compiler);
    try {
      const capabilities = await client.request('initialize', { adapterID: 'chali', supportsInvalidatedEvent: true });
      assert.equal(capabilities.supportsSetVariable, true);
      await client.request('launch', { executable: prefix + (process.platform === 'win32' ? '.exe' : ''), cwd: root });
      await client.event('initialized');
      const installed = await client.request('setBreakpoints', { source: { path: source }, breakpoints: [' println("READY");', '  println("INNER");', ' println("AFTER");', ' println("PARAMETER");'].map(text => ({ line: lines.indexOf(text) + 1 })) });
      assert(installed.breakpoints.every(point => point.verified)); await client.request('configurationDone');
      let stop = await client.event('stopped'), data = await view(client, stop.threadId);
      const originalScope = data.scope, oldValue = data.values.values.variablesReference, oldMemory = data.values.values.memoryReference;
      assert(oldValue && oldMemory);
      const set = (name, value) => client.request('setVariable', { variablesReference: data.scope, name, value });
      for (const name of ['fixed', 'owned', 'borrowed', 'alias', 'values']) assert(data.values[name].presentationHint.attributes.includes('readOnly'), name);
      for (const [name, value] of [['fixed', '9'], ['owned', '0'], ['borrowed', '9'], ['alias', '0'], ['values', '0'], ['signed8', '128'], ['unsigned8', '-1'], ['signed32', '1.5'], ['truth', '1'], ['single', '1e39'], ['signed64', 'inspect(9)'], ['missing', '1']]) await assert.rejects(set(name, value));
      for (const [name, value] of [['signed8', '3'], ['signed8', '-9'], ['signed16', '6'], ['signed32', '7'], ['signed64', '-9000000001'], ['unsigned8', '4'], ['unsigned16', '8'], ['unsigned32', '9'], ['unsigned64', '18446744073709551614'], ['single', '0.1'], ['real', '2.5'], ['truth', 'false'], ['letter', "'Z'"]]) {
        await set(name, value);
        const event = await client.event('invalidated'); assert.deepEqual(event.areas, ['variables']);
      }
      assert.equal((await client.request('evaluate', { frameId: data.frameId, expression: 'signed8 + signed16 + signed32' })).result, '4');
      await assert.rejects(client.request('variables', { variablesReference: oldValue }), /过期/);
      await assert.rejects(client.request('readMemory', { memoryReference: oldMemory, count: 8 }), /过期|无效/);
      assert.equal((await view(client, stop.threadId)).values.single.value, String(Math.fround(0.1)));
      await client.request('continue', { threadId: stop.threadId }); stop = await client.event('stopped'); data = await view(client, stop.threadId);
      await assert.rejects(client.request('setVariable', { variablesReference: originalScope, name: 'visible', value: '0' }), /有效|过期/);
      assert.equal(data.values.visible.value, '42'); await set('visible', '99');
      await client.request('continue', { threadId: stop.threadId }); stop = await client.event('stopped'); data = await view(client, stop.threadId);
      assert.equal(data.values.visible.value, '41'); await set('visible', '77');
      await client.request('continue', { threadId: stop.threadId }); stop = await client.event('stopped'); data = await view(client, stop.threadId);
      assert.equal(data.values.argument.value, '11'); await set('argument', '12');
      await client.request('continue', { threadId: stop.threadId });
      assert.equal((await client.event('exited')).exitCode, 0); await client.event('terminated');
    } finally { await client.close(); }
  }
  console.log('基础变量修改真实 GDB：malloc/pool、全部基础类型、完整槽恢复执行、参数/遮蔽、只读/镜像/拥有值拒绝与引用失效通过');
}
main().catch(error => { console.error(error); process.exitCode = 1; });
