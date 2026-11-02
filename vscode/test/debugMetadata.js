'use strict';

/** 核对只读映射的完整性、地址精度、预算和取消，不执行任何目标函数。 */
const assert = require('node:assert/strict');
const path = require('node:path');
const { DebugMetadata, LIMIT, visibleBindings } = require('../src/debugMetadata');
const source = path.resolve(__dirname, 'catalog-source.chl');
const start = 0x123456789abcdef0n;

/** 模拟 GDB 的真实十六进制内存封装，只允许三个内部符号查询。 */
function target(change = value => value, options = {}) {
  const catalog = change({ version: 1, pointerBytes: 8, byteOrder: 'little',
    functions: [{ name: 'identity<int64>', linkage: 'f_identity_i64', file: source, line: 1, column: 1,
      scopes: [{ id: 0, parent: 0 }, { id: 2, parent: 0 }],
      variables: [{ name: 'value', binding: 'value$1', type: 'int64', file: source, line: 1, column: 24, scope: 0, readOnly: false }] }],
    positions: [{ function: 0, file: source, line: 2, column: 9, scope: 2 }, { function: 0, file: source, line: 2, column: 13, scope: 2 }] });
  const bytes = Buffer.from(JSON.stringify(catalog)), addresses = Buffer.alloc(16);
  addresses.writeBigUInt64LE(start, 0); addresses.writeBigUInt64LE(start + 8n, 8);
  const calls = [], output = [];
  const reader = new DebugMetadata(async (command, { signal }) => {
    calls.push(command);
    if (command.startsWith('-data-evaluate-expression ')) {
      const expression = JSON.parse(command.slice('-data-evaluate-expression '.length));
      if (expression === '*(unsigned long long*)&chali_debug_catalog_size') return { value: String(options.size || bytes.length) };
      if (expression === '(unsigned long long)&chali_debug_catalog') return { value: '65536' };
      if (expression === '(unsigned long long)&chali_debug_positions') return { value: '268435456' };
      assert.fail('不允许查询目标表达式：' + expression);
    }
    const match = /^-data-read-memory-bytes (0x[0-9a-f]+) ([0-9]+)$/.exec(command); assert(match, command);
    if (options.pause) await options.pause(signal);
    const address = BigInt(match[1]), count = Number(match[2]);
    const data = address >= 268435456n ? addresses : bytes, base = address >= 268435456n ? 268435456n : 65536n;
    const offset = Number(address - base);
    return { memory: [{ begin: '0x' + (address + BigInt(options.gap || 0)).toString(16), contents: data.subarray(offset, offset + count - (options.truncate || 0)).toString('hex') }] };
  }, message => output.push(message));
  return { reader, calls, output };
}

async function main() {
  const healthy = target(); await healthy.reader.load();
  let frame = healthy.reader.frame({ func: 'f_identity_i64', fullname: source, line: '2', addr: '0x' + (start + 10n).toString(16) });
  assert.equal(frame.name, 'identity<int64>'); assert.equal(frame.column, 13); assert.equal(frame.position.scope, 2);
  frame.function.variables.push({name:'value',dwarf:'value',scope:2,address:true});
  assert.equal(visibleBindings(frame).get('value').address,true);
  assert.equal(visibleBindings({...frame,position:{scope:0}}).get('value').scope,0);
  assert.equal(visibleBindings({...frame,position:undefined}).size,0);
  frame = healthy.reader.frame({ func: 'f_identity_i64', fullname: source, line: '3', addr: '0x' + (start + 10n).toString(16) });
  assert.equal(frame.column, 1, '不能把其他源码行的列号冒充当前列');
  const expanded = target(value => {
    const location = { file: source, line: 1, column: 2 };
    value.macros = [ { id: 8, parent: -1, name: 'Outer', call: location, invocation: location, definition: location },
      { id: 9, parent: 8, name: 'Inner', call: location, invocation: location, definition: location } ];
    value.positions[1].macro = 9; value.positions[1].definition = location;
    Object.assign(value.functions[0].variables[0], { dwarf: '__chali_macro_debug_9_1', macro: 9 });
    return value;
  });
  assert(await expanded.reader.load());
  const macroFrame = expanded.reader.frame({ func: 'f_identity_i64', fullname: source, line: '2', addr: '0x' + (start + 10n).toString(16) });
  assert.deepEqual(macroFrame.macros.map(item => item.name), ['Inner', 'Outer']);
  const count = healthy.calls.length; healthy.reader.cancel(); await healthy.reader.load(); assert.equal(healthy.calls.length, count);
  const large = target(value => ({ ...value, padding: 'x'.repeat(131072) }));
  assert(await large.reader.load()); assert(large.calls.filter(command => command.startsWith('-data-read-memory-bytes ')).length >= 4);
  const optimized = target(value => ({ ...value, optimized: true, positions: [] }));
  assert(await optimized.reader.load());
  const optimizedFrame = optimized.reader.frame({ func: 'f_identity_i64', fullname: source, line: '2', addr: '0x' + start.toString(16) });
  assert.equal(optimizedFrame.optimized, true); assert.equal(optimizedFrame.position, undefined);
  assert.equal(visibleBindings(optimizedFrame).get('value').type, 'int64');
  assert(!optimized.calls.some(command => command.includes('&chali_debug_positions')));
  assert.match(optimized.output[0], /O3 优化调试/);
  const dynamic = target(value => ({ ...value, layouts: ['O0:0:0:8:0:0:'], dynamicTypes: [{ id: 42, layout: 0 }] }));
  assert.equal((await dynamic.reader.load()).dynamicTypes.get(42n), 0);
  const closure = target(value => ({ ...value, layouts: ['x'], closures: [{ address: 24, name: 'counter',
    captures: [{ name: 'n', layout: 0, mode: 2, address: false }] }] }));
  assert.equal((await closure.reader.load()).closures.get(24n).captures[0].name, 'n');
  for (const invalid of [
    () => target(value => ({ ...value, optimized: 'O3' })),
    () => target(value => ({ ...value, optimized: true })),
    () => target(value => { value.functions[0].variables[0].nativeStorage = 'ptr'; return value; }),
    () => target(value => ({ ...value, closures: [{ address: 24, name: 'bad', captures: [{ name: 'n', layout: 0, mode: 4, address: true }] }] })),
    () => target(value => ({ ...value, layouts: ['x'], closures: [{ address: 24, name: 'bad', captures: [{ name: 'n', layout: 0, mode: 2, address: true }] }] })),
    () => target(value => ({ ...value, dynamicTypes: [{ id: 42, layout: 0 }] })),
    () => target(value => ({ ...value, layouts: ['x'], dynamicTypes: [{ id: 42, layout: 0 }, { id: 42, layout: 0 }] })),
    () => target(value => ({ ...value, version: 99 })),
    () => target(value => ({ ...value, pointerBytes: 4 })),
    () => target(value => { value.functions[0].scopes.push({ id: 3, parent: 3 }); return value; }),
    () => target(value => { value.positions[0].function = 3; return value; }),
    () => target(value => { value.functions[0].variables[0].scope = 99; return value; }),
    () => target(value => { value.positions[0].macro = 99; return value; }),
    () => target(value => { value.macros = [{ id: 2, parent: 1, name: 'Broken', call: {file: source,line: 1,column: 1}, invocation: {file: source,line: 1,column: 1}, definition: {file: source,line: 1,column: 1} }]; return value; }),
    () => target(undefined, { size: LIMIT + 1 }),
    () => target(undefined, { gap: 1 }),
    () => target(undefined, { truncate: 1 }),
  ]) {
    const test = invalid(); assert.equal(await test.reader.load(), null); assert.equal(test.output.length, 1);
    const before = test.calls.length; assert.equal(await test.reader.load(), null); assert.equal(test.calls.length, before);
  }
  let release;
  const wait = new Promise(resolve => { release = resolve; });
  const shared = target(undefined, { pause: () => wait });
  const cancelled = new AbortController();
  const first = shared.reader.load(cancelled.signal), second = shared.reader.load();
  const rejected = assert.rejects(first, /取消/); cancelled.abort(); await rejected; release();
  assert(await second); assert.equal(shared.output.length, 0);
  const interrupted = target(undefined, { pause: signal => new Promise((resolve, reject) => {
    signal.addEventListener('abort', () => reject(new Error('读取被停止')), { once: true });
    if (signal.aborted) reject(new Error('读取被停止'));
  }) });
  const pending = interrupted.reader.load();
  await new Promise(resolve => setImmediate(resolve)); interrupted.reader.cancel();
  await assert.rejects(pending, /停止/); assert.equal(interrupted.reader.unavailable, false);
  console.log('调试映射：只读符号、64 位地址、真实列、缓存、损坏／预算拒绝及独立取消通过');
}
main().catch(error => { console.error(error); process.exitCode = 1; });
