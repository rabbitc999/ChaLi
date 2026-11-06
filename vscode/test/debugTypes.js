'use strict';

/** 用真实 GDB 核对全部基础值、声明范围和同名遮蔽。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { spawnSync } = require('node:child_process');
const { DebugClient } = require('./debugClient');
const root = path.resolve(__dirname, '../..');
const compiler = process.env.CHALI_BIN || path.join(root, 'chali.exe');
const work = fs.mkdtempSync(path.join(root, 'tmp/debug-types-'));
const lines = [
  'void inspect(uint64 number, bool flag, float32 decimal) {',
  '    println("PARAMETERS");',
  '}',
  'void main() {',
  '    int8 signed8=-7; uint8 unsigned8=255;',
  '    int16 signed16=-300; uint16 unsigned16=65000;',
  '    int32 signed32=-70000; uint32 unsigned32=(uint32)4000000000;',
  '    int64 signed64=-9000000000; uint64 unsigned64=(uint64)-1;',
  '    float32 single=(float32)1.5; float64 real=2.25;',
  "    bool truth=true; bool falsehood=false; char letter='A';",
  '    int64 visible=11;',
  '    println("OUTER");',
  '    {',
  '        int64 visible=22;',
  '        int64 innerOnly=33;',
  '        println("INNER");',
  '    }',
  '    println("AFTER");',
  '    for(int64 loopIndex in std::core::range(0,1)) {',
  '        println("LOOP");',
  '    }',
  '    println("LOOP AFTER");',
  '    inspect(unsigned64,truth,single);',
  '}',
];
const source = path.join(work, 'types.chl'); fs.writeFileSync(source, lines.join('\n') + '\n');

/** 查询当前源码帧和全部可见变量，保留重复名字供遮蔽验收。 */
async function view(client, stop) {
  const stack = await client.request('stackTrace', { threadId: stop.threadId });
  const top = stack.stackFrames[0];
  const scopes = await client.request('scopes', { frameId: top.id });
  const values = await client.request('variables', { variablesReference: scopes.scopes[0].variablesReference });
  return { top, list: values.variables, values: Object.fromEntries(values.variables.map(item => [item.name, item])) };
}

async function main() {
  const irPrefix = path.join(work, 'types');
  const emitted = spawnSync(compiler, ['build', source, '--checked', '--debug', '--emit-llvm', '--out', irPrefix], { cwd: root, env: process.env, encoding: 'utf8', timeout: 180000, windowsHide: true });
  assert.equal(emitted.status, 0, emitted.stdout + emitted.stderr);
  const ir = fs.readFileSync(irPrefix + '.ll', 'utf8');
  assert.match(ir, /DILexicalBlock\(/); assert.match(ir, /DILocation\(line: 14, column: [1-9][0-9]*/);
  assert.match(ir, /DILocalVariable\(name: "visible"[^\n]*line: 14/);
  assert.match(ir, /DILocalVariable\(name: "number", arg: 1[^\n]*line: 1/);
  for (const name of ['int8', 'uint8', 'int16', 'uint16', 'int32', 'uint32', 'int64', 'uint64', 'float32', 'float64', 'bool', 'char']) {
    assert(ir.includes('DIBasicType(name: "' + name + '"'), name);
  }
  const client = new DebugClient(root, compiler);
  try {
    await client.request('initialize', { adapterID: 'chali' });
    await client.request('launch', { program: source, cwd: root }); await client.event('initialized');
    const points = [12, 16, 18, 20, 22, 2];
    const installed = await client.request('setBreakpoints', { source: { path: source }, breakpoints: points.map(line => ({ line })) });
    assert(installed.breakpoints.every(point => point.verified), JSON.stringify(installed));
    await client.request('configurationDone');
    let stop = await client.event('stopped'), data = await view(client, stop);
    assert.equal(data.top.line, 12);
    assert.equal(data.top.name, 'main'); assert(data.top.column > 1, JSON.stringify(data.top));
    const expected = { signed8: '-7', unsigned8: '255', signed16: '-300', unsigned16: '65000', signed32: '-70000', unsigned32: '4000000000', signed64: '-9000000000', unsigned64: '18446744073709551615', single: '1.5', real: '2.25', truth: 'true', falsehood: 'false', visible: '11' };
    for (const [name, value] of Object.entries(expected)) assert.equal(data.values[name]?.value, value, JSON.stringify({ name, data }));
    assert.match(data.values.letter.value, /65.*A/); assert.equal(data.values.letter.type, 'char');
    assert.equal(data.values.unsigned64.type, 'uint64'); assert.equal(data.values.single.type, 'float32');
    assert(!data.values.innerOnly);
    await client.request('continue', { threadId: stop.threadId }); stop = await client.event('stopped'); data = await view(client, stop);
    assert.equal(data.top.line, 16); assert.equal(data.values.visible.value, '22'); assert.equal(data.values.innerOnly.value, '33');
    assert.equal(data.list.filter(item => item.name === 'visible').length, 1);
    await client.request('continue', { threadId: stop.threadId }); stop = await client.event('stopped'); data = await view(client, stop);
    assert.equal(data.top.line, 18); assert.equal(data.values.visible.value, '11'); assert(!data.values.innerOnly);
    await client.request('continue', { threadId: stop.threadId }); stop = await client.event('stopped'); data = await view(client, stop);
    assert.equal(data.top.line, 20); assert.equal(data.values.loopIndex.value, '0');
    await client.request('continue', { threadId: stop.threadId }); stop = await client.event('stopped'); data = await view(client, stop);
    assert.equal(data.top.line, 22); assert(!data.values.loopIndex);
    await client.request('continue', { threadId: stop.threadId }); stop = await client.event('stopped'); data = await view(client, stop);
    assert.equal(data.top.line, 2); assert.equal(data.values.number.value, '18446744073709551615'); assert.equal(data.values.flag.value, 'true'); assert.equal(data.values.decimal.value, '1.5');
    await client.request('continue', { threadId: stop.threadId }); await client.event('terminated');
    console.log('DWARF 基础类型、无符号边界、布尔、字符、浮点、参数、源码列及块遮蔽通过');
  } finally {
    fs.writeFileSync(path.join(work, 'messages.json'), JSON.stringify({ messages: client.messages, stderr: client.stderr }, null, 2));
    await client.close();
  }
}
main().catch(error => { console.error(error); process.exitCode = 1; });
