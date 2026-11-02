'use strict';

/** 独立目录必须使用同一总预算，保持混合优化状态并拒绝身份或布局冲突。 */
const assert = require('node:assert/strict');
const { createHash } = require('node:crypto');
const path = require('node:path');
const { PassThrough } = require('node:stream');
const { Adapter } = require('../src/nativeDebugAdapter');
const { DebugSourceMap } = require('../src/debugSourceMap');
const { DebugMetadata, visibleBindings, LIMIT } = require('../src/debugMetadata');
const prefix = 'chali_debug_' + createHash('sha256').update('demo::math').digest('hex');
const descriptor = { format: 1, prefix, module: 'demo::math', version: '1.0.0', optimized: true,
  implementation: 'native', implementationSha256: 'b'.repeat(64), bitcodeSha256: '' };

/** 仅封装实际只读符号和连续字节协议；修改目录以触发各个独立拒绝路径。 */
function fixture(change = () => {}, sizes = {}) {
  const variable = { name: 'value', binding: 'value', type: 'int64', file: '/source.chl', line: 2, column: 2,
    scope: 0, readOnly: false, layout: 0, nativeStorage: 'i64' };
  const fn = { name: 'twice', linkage: 'library_twice', file: '/source.chl', line: 1, column: 1,
    scopes: [{ id: 0, parent: 0 }], variables: [variable] };
  const main = { version: 1, pointerBytes: 8, byteOrder: 'little', optimized: false,
    functions: [], positions: [], layouts: ['main'], libraries: [{ ...descriptor }] };
  const library = { version: 1, pointerBytes: 8, byteOrder: 'little', optimized: true, unit: descriptor.module,
    functions: [fn], positions: [], layouts: ['library'] };
  change(main, library);
  const entries = [main, library].map((catalog, index) => ({ prefix: index ? prefix : 'chali_debug',
    base: BigInt(65536 + index * LIMIT * 2), bytes: Buffer.from(JSON.stringify(catalog)) }));
  const identity = { base: BigInt(LIMIT * 4), bytes: Buffer.from('b'.repeat(64) + '\0') };
  const commands = [], output = [];
  const reader = new DebugMetadata(async command => {
    commands.push(command);
    if (command.startsWith('-data-evaluate-expression ')) {
      const expression = JSON.parse(command.slice('-data-evaluate-expression '.length));
      if (expression === `(unsigned long long)&${prefix}_implementation_sha256`) return { value: String(identity.base) };
      for (const entry of entries) {
        if (expression === `*(unsigned long long*)&${entry.prefix}_catalog_size`) return { value: String(sizes[entry.prefix] || entry.bytes.length) };
        if (expression === `(unsigned long long)&${entry.prefix}_catalog`) return { value: String(entry.base) };
      }
      assert.fail('意外的符号查询：' + expression);
    }
    const match = /^-data-read-memory-bytes (0x[0-9a-f]+) ([0-9]+)$/.exec(command);
    assert(match, command);
    const address = BigInt(match[1]), length = Number(match[2]);
    const entry = [...entries, identity].find(item => address >= item.base && address < item.base + BigInt(item.bytes.length));
    assert(entry, '不得读取未声明地址');
    const start = Number(address - entry.base), bytes = entry.bytes.subarray(start, start + length);
    return { memory: [{ begin: match[1], contents: bytes.toString('hex') }] };
  }, value => output.push(value));
  return { reader, commands, output };
}

async function main() {
  const valid = fixture(), result = await valid.reader.load();
  assert.equal(result.catalog.optimized, false);
  assert.deepEqual(result.catalog.layouts, ['main', 'library']);
  const frame = valid.reader.frame({ func: 'twice', fullname: '/source.chl', line: 2 });
  assert.equal(frame.optimized, true); assert.equal(frame.function.library.module, 'demo::math');
  assert.equal(visibleBindings(frame).get('value').layout, 1);
  for (const change of [
    main => { main.libraries[0].prefix = 'user()'; },
    main => { main.libraries.push({ ...main.libraries[0] }); },
    main => { main.libraries[0].implementationSha256 = ''; },
    main => { main.libraries[0].implementationSha256 = 'c'.repeat(64); },
    (_, library) => { library.unit = 'other::math'; },
    (_, library) => { library.optimized = false; },
    (_, library) => { library.libraries = [{ ...descriptor }]; },
    (main, library) => { main.functions = [JSON.parse(JSON.stringify(library.functions[0]))]; },
    (main, library) => { main.dynamicTypes = [{ id: 3, layout: 0 }]; library.dynamicTypes = [{ id: 3, layout: 0 }]; },
  ]) {
    const invalid = fixture(change);
    assert.equal(await invalid.reader.load(), null);
    assert.match(invalid.output.join(''), /不可用/);
  }
  const budget = fixture(() => {}, { [prefix]: LIMIT });
  assert.equal(await budget.reader.load(), null);
  assert.match(budget.output.join(''), /总字节/);
  for (const behavior of ['resolved', 'pending', 'failure']) {
    const local = path.resolve('tmp/library-source');
    const adapter = new Adapter('unused', new PassThrough(), new PassThrough());
    adapter.sourceMap = new DebugSourceMap({ '/builder': local });
    const commands = [];
    adapter.gdb = { command: async command => {
      commands.push(command);
      if (command.startsWith('-break-insert') && command.includes('/builder/')) return { bkpt: { number: '1', pending: '/builder/math.chl:4' } };
      if (command.startsWith('-break-insert')) {
        if (behavior === 'failure') throw new Error('mapped lookup failed');
        return { bkpt: { number: '2', ...(behavior === 'pending' ? { pending: 'math.chl:4' } : { line: '4' }) } };
      }
      assert(command.startsWith('-break-delete'), command); return {};
    } };
    const result = await adapter.setSourceBreakpoints({ source: { path: path.join(local, 'math.chl') }, breakpoints: [{ line: 4 }] }, new AbortController().signal);
    assert.equal(result.breakpoints[0].verified, behavior === 'resolved');
    assert(commands.includes('-break-delete ' + (behavior === 'pending' ? '2' : '1')));
    if (behavior === 'failure') assert.equal(adapter.breakpoints.get(path.join(local, 'math.chl')).length, 0);
    else assert.deepEqual(adapter.breakpoints.get(path.join(local, 'math.chl')), [behavior === 'resolved' ? '2' : '1']);
  }
  console.log('库调试目录：混合优化、布局重定位、身份/重名/动态类型/嵌套拒绝与累计预算通过');
}
main().catch(error => { console.error(error); process.exitCode = 1; });
