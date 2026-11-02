'use strict';

/** 用实际 O0/O3 机器码核对 DWARF 内联栈、优化掉的值和只读调试，不以选项字符串代替执行。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { spawnSync } = require('node:child_process');
const { DebugClient } = require('./debugClient');
const root = path.resolve(__dirname, '../..'), compiler = process.env.CHALI_BIN;
const external = process.env.CHALI_DEBUG_OPTIMIZED_LLVM;
assert(compiler, '需要 CHALI_BIN');
const work = fs.mkdtempSync(path.join(root, 'tmp/debug-optimized-'));
const source = path.join(work, 'main.chl');
const text = [
  'import std::clock;',
  'int64 leaf(int64 input){',
  ' int64 gone=std::clock::nowNanos();',
  ' int64 visible=input+5;',
  ' println("INNER {}",visible);',
  ' return visible*2;',
  '}',
  'int64 outer(int64 input){',
  ' int64 forwarded=input+2;',
  ' return leaf(forwarded)+1;',
  '}',
  'void main(List<String> args){',
  ' int64 source=len(args);',
  ' int64 result=outer(source);',
  ' println("RESULT {}",result);',
  ' assert(result==21);',
  '}',
].join('\n');
fs.writeFileSync(source, text);

async function main() {
  for (const allocator of ['malloc', 'pool']) for (const optimized of [false, true]) {
    const prefix = path.join(work, allocator + (optimized ? '-optimized' : '-development'));
    const built = spawnSync(compiler, ['aot', source, '--checked', optimized ? '--debug-optimized' : '--debug', '--allocator', allocator, '--out', prefix,
      ...(external ? ['--llvm', external] : [])],
      { cwd: root, env: process.env, encoding: 'utf8', windowsHide: true, timeout: 180000 });
    assert.equal(built.status, 0, built.stdout + built.stderr);
    const llvm = fs.readFileSync(prefix + '.ll', 'utf8');
    assert.match(llvm, new RegExp('producer: "ChaLi", isOptimized: ' + optimized));
    if (optimized) { assert(!llvm.includes('ptr blockaddress(')); assert.match(llvm, /DISPFlagOptimized/); }
    else assert(llvm.includes('ptr blockaddress('));
    const client = new DebugClient(root, compiler);
    try {
      await client.request('initialize', { adapterID: 'chali' });
      await client.request('launch', { ...(optimized && allocator === 'malloc' && !external
        ? { program: source, debugMode: 'optimized' }
        : { executable: prefix + (process.platform === 'win32' ? '.exe' : '') }), cwd: root, args: ['a', 'b', 'c'] });
      await client.event('initialized');
      const points = await client.request('setBreakpoints', { source: { path: source }, breakpoints: [{ line: 5 }] });
      assert(points.breakpoints[0].verified, JSON.stringify(points));
      await client.request('configurationDone');
      const stop = await client.event('stopped');
      const frames = (await client.request('stackTrace', { threadId: stop.threadId })).stackFrames;
      fs.writeFileSync(prefix + '-stack.json', JSON.stringify(frames, null, 2));
      const leaf = frames.find(frame => /(?:^|::)leaf$/.test(frame.name));
      const outer = frames.find(frame => /(?:^|::)outer$/.test(frame.name));
      assert(leaf && outer, JSON.stringify(frames)); assert.equal(leaf.line, 5);
      assert(frames.some(frame => /(?:^|::)main$/.test(frame.name)), JSON.stringify(frames));
      const scopes = (await client.request('scopes', { frameId: leaf.id })).scopes;
      const rows = (await client.request('variables', { variablesReference: scopes[0].variablesReference })).variables;
      fs.writeFileSync(prefix + '-variables.json', JSON.stringify(rows, null, 2));
      assert.equal(rows.find(row => row.name === 'visible')?.value, '10');
      assert.equal((await client.request('evaluate', { frameId: leaf.id, expression: 'visible+1' })).result, '11');
      const disassembly = await client.request('disassemble', { memoryReference: leaf.instructionPointerReference, offset: 0, instructionOffset: 0, instructionCount: 3 });
      fs.writeFileSync(prefix + '-disassembly.json', JSON.stringify(disassembly, null, 2));
      if (optimized) {
        assert.match(scopes[0].name, /O3/);
        assert.match(rows.find(row => row.name === 'gone')?.value || '', /已优化/);
        assert(rows.every(row => row.presentationHint?.attributes.includes('readOnly')));
        await assert.rejects(client.request('evaluate', { frameId: leaf.id, expression: 'gone' }), /已被优化/);
        await assert.rejects(client.request('setVariable', { variablesReference: scopes[0].variablesReference, name: 'visible', value: '11' }), /优化调试不修改/);
        assert.equal(leaf.instructionPointerReference, outer.instructionPointerReference, '内联调用帧必须对应相同机器位置');
        assert(disassembly.instructions.some(instruction => instruction.symbol && !/leaf/.test(instruction.symbol)), JSON.stringify(disassembly));
      } else {
        assert(!/已优化/.test(rows.find(row => row.name === 'gone')?.value || ''));
        assert.notEqual(leaf.instructionPointerReference, outer.instructionPointerReference);
      }
      await client.request('continue', { threadId: stop.threadId });
      assert.equal((await client.event('exited')).exitCode, 0); await client.event('terminated');
      console.log('真实 GDB 优化调试：' + allocator + '/' + (optimized ? 'O3 内联栈、已优化值及只读' : 'O0 独立调用帧与存活值') + '，运行结果一致通过');
    } finally { await client.close(); }
  }
}
main().catch(error => { console.error(error); process.exitCode = 1; });
