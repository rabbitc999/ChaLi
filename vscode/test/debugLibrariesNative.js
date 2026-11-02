'use strict';

/** 使用真实预编译库、已移动／删除源码和 GDB 核对目录、值、位置及机器码。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { spawnSync } = require('node:child_process');
const { DebugClient } = require('./debugClient');
const root = path.resolve(__dirname, '../..'), compiler = process.env.CHALI_BIN;
assert(compiler, '需要 CHALI_BIN');
const work = fs.mkdtempSync(path.join(root, 'tmp/debug-libraries-'));
const external = process.env.CHALI_DEBUG_OPTIMIZED_LLVM;

/** 每次调用真实编译器，任何诊断或失败保留给调用方。 */
function compile(args) {
  const result = spawnSync(compiler, [...args, ...(external ? ['--llvm', external] : [])],
    { cwd: work, env: process.env, encoding: 'utf8', windowsHide: true, timeout: 180000 });
  assert.equal(result.status, 0, result.stdout + result.stderr);
}

async function main() {
  for (const optimized of [false, true]) {
    const caseRoot = path.join(work, optimized ? 'optimized' : 'development');
    const original = path.join(caseRoot, 'original'), relocated = path.join(caseRoot, '映射 源码');
    fs.mkdirSync(original, { recursive: true });
    fs.mkdirSync(path.join(work, 'modules/demo'), { recursive: true });
    const provider = path.join(original, 'math.chl'), mapped = path.join(relocated, 'math.chl');
    const extra = path.join(original, 'extra.chl'), mappedExtra = path.join(relocated, 'extra.chl');
    const text = 'module demo::math;\nint64 twice(int64 input){\n int64 visible=input+5;\n return visible*2;\n}\n';
    const extraText = 'module demo::extra;\nstruct Row{int64 value;};\nbool valid(Row const& input){\n bool matched=input.value==16;\n return matched;\n}\n';
    fs.writeFileSync(provider, text);
    compile(['lib', 'build', provider, '--module', 'demo::math', '--package-version', '1.0.0', '--with-bitcode',
      optimized ? '--debug-optimized' : '--debug', '--out', path.join(work, 'modules/demo/math.chlib')]);
    fs.writeFileSync(extra, extraText);
    compile(['lib', 'build', extra, '--module', 'demo::extra', '--package-version', '1.0.0', '--with-bitcode',
      optimized ? '--debug-optimized' : '--debug', '--out', path.join(work, 'modules/demo/extra.chlib')]);
    fs.renameSync(original, relocated);
    const consumer = path.join(work, 'main.chl');
    fs.writeFileSync(consumer, 'import demo::math;\nimport demo::extra;\nvoid main(List<String> args){\n int64 result=demo::math::twice(len(args));\n demo::extra::Row row;row.value=result;\n assert(demo::extra::valid(&row));println("LIB {}",result);\n}\n');
    for (const allocator of ['malloc', 'pool']) for (const implementation of ['native', 'bitcode']) {
      const missing = allocator === 'pool';
      if (missing && fs.existsSync(mapped)) fs.unlinkSync(mapped);
      if (missing && fs.existsSync(mappedExtra)) fs.unlinkSync(mappedExtra);
      if (!missing) { fs.writeFileSync(mapped, text); fs.writeFileSync(mappedExtra, extraText); }
      const prefix = path.join(caseRoot, allocator + '-' + implementation);
      compile(['aot', consumer, implementation === 'native' && missing ? '--debug-optimized' : '--debug', '--checked', '--allocator', allocator,
        '--library-implementation', implementation, '--lto-mode', implementation === 'bitcode' ? 'thin' : 'off', '--out', prefix]);
      const client = new DebugClient(root, compiler);
      try {
        await client.request('initialize', { adapterID: 'chali' });
        await client.request('launch', { executable: prefix + (process.platform === 'win32' ? '.exe' : ''), cwd: work,
          args: ['a', 'b', 'c'], sourceFileMap: { [original]: relocated } });
        await client.event('initialized');
        const points = await client.request('setBreakpoints', { source: { path: mapped }, breakpoints: [{ line: 4 }] });
        assert(points.breakpoints[0].verified, JSON.stringify(points));
        const extraPoints = await client.request('setBreakpoints', { source: { path: mappedExtra }, breakpoints: [{ line: 5 }] });
        assert(extraPoints.breakpoints[0].verified, JSON.stringify(extraPoints));
        await client.request('configurationDone');
        const stop = await client.event('stopped');
        const frames = (await client.request('stackTrace', { threadId: stop.threadId })).stackFrames;
        fs.writeFileSync(prefix + '-stack.json', JSON.stringify(frames, null, 2));
        const frame = frames.find(row => /(?:^|::)twice$/.test(row.name));
        assert(frame, JSON.stringify(frames)); assert.equal(frame.line, 4);
        if (optimized) {
          const caller = frames[frames.indexOf(frame) + 1];
          assert(caller, JSON.stringify(frames));
          assert.equal(frame.instructionPointerReference, caller.instructionPointerReference, 'O3 库函数应保留在 ABI 入口内联后的同地址源码帧');
        }
        assert.equal(path.normalize(frame.source.path), path.normalize(mapped));
        assert.match(frame.source.origin, /demo::math@1\.0\.0/);
        assert(frame.source.origin.includes(implementation)); assert.match(frame.source.origin, /SHA-256 [a-f0-9]{64}/);
        assert.equal(frame.source.origin.includes('源码不可用'), missing);
        const scopes = (await client.request('scopes', { frameId: frame.id })).scopes;
        assert.equal(scopes[0].name.includes('O3'), optimized);
        const values = (await client.request('variables', { variablesReference: scopes[0].variablesReference })).variables;
        fs.writeFileSync(prefix + '-variables.json', JSON.stringify(values, null, 2));
        assert.equal(values.find(row => row.name === 'visible')?.value, '8');
        assert.equal((await client.request('evaluate', { frameId: frame.id, expression: 'visible+1' })).result, '9');
        const assembly = await client.request('disassemble', { memoryReference: frame.instructionPointerReference, offset: 0, instructionOffset: 0, instructionCount: 3 });
        assert.equal(assembly.instructions.length, 3);
        if (optimized) await assert.rejects(client.request('setVariable', { variablesReference: scopes[0].variablesReference, name: 'visible', value: '9' }), /优化调试不修改/);
        await client.request('stepOut', { threadId: stop.threadId });
        const callerStop = await client.event('stopped');
        const callerFrames = (await client.request('stackTrace', { threadId: callerStop.threadId })).stackFrames;
        assert.equal(path.normalize(callerFrames[0].source.path), path.normalize(consumer), JSON.stringify(callerFrames));
        assert.equal(callerFrames[0].name, 'main');
        await client.request('continue', { threadId: callerStop.threadId });
        const extraStop = await client.event('stopped');
        const extraFrames = (await client.request('stackTrace', { threadId: extraStop.threadId })).stackFrames;
        const extraFrame = extraFrames.find(row => /(?:^|::)valid$/.test(row.name));
        assert(extraFrame, JSON.stringify(extraFrames)); assert.equal(extraFrame.line, 5);
        assert.match(extraFrame.source.origin, /demo::extra@1\.0\.0/);
        assert.equal(extraFrame.source.origin.includes('源码不可用'), missing);
        assert.equal((await client.request('evaluate', { frameId: extraFrame.id, expression: 'matched' })).result, 'true');
        if (!optimized) assert.equal((await client.request('evaluate', { frameId: extraFrame.id, expression: 'input.value' })).result, '16');
        await client.request('continue', { threadId: extraStop.threadId });
        await client.event('terminated');
        const output = client.messages.filter(row => row.event === 'output').map(row => row.body.output).join('');
        assert.match(output, /LIB 16/);
        console.log(`真实预编译库调试：${optimized ? 'O3' : 'O0'}/${allocator}/${implementation}，源码${missing ? '缺失' : '映射'}、变量、反汇编与结果通过`);
      } finally {
        fs.writeFileSync(prefix + '-protocol.json', JSON.stringify(client.messages, null, 2));
        await client.close();
      }
    }
  }
  const consumer = path.join(work, 'main.chl'), objectPrefix = path.join(work, 'old-consumer');
  compile(['aot', consumer, '--debug', '--emit-obj', '--out', objectPrefix]);
  const original = path.join(work, 'optimized/original');
  fs.mkdirSync(original, { recursive: true });
  const replaced = path.join(original, 'math.chl');
  fs.writeFileSync(replaced, 'module demo::math;\nint64 twice(int64 input){\n int64 visible=input+6;\n return visible*2;\n}\n');
  compile(['lib', 'build', replaced, '--module', 'demo::math', '--package-version', '1.0.0', '--debug-optimized',
    '--with-bitcode', '--out', path.join(work, 'modules/demo/math.chlib')]);
  const executable = path.join(work, 'stale-consumer' + (process.platform === 'win32' ? '.exe' : ''));
  compile(['link', objectPrefix + '.o', '--debug', '--package', path.join(work, 'modules/demo/math.chlib'),
    '--package', path.join(work, 'modules/demo/extra.chlib'), '--out', executable]);
  const stale = new DebugClient(root, compiler);
  try {
    await stale.request('initialize', { adapterID: 'chali' });
    await stale.request('launch', { executable, cwd: work }); await stale.event('initialized');
    const points = await stale.request('setBreakpoints', { source: { path: consumer }, breakpoints: [{ line: 4 }] });
    assert(points.breakpoints[0].verified);
    await stale.request('configurationDone'); const stop = await stale.event('stopped');
    const frames = (await stale.request('stackTrace', { threadId: stop.threadId })).stackFrames;
    const output = stale.messages.filter(row => row.event === 'output').map(row => row.body.output).join('');
    assert.match(output, /最终链接实现 SHA-256 不匹配/);
    await assert.rejects(stale.request('evaluate', { frameId: frames[0].id, expression: 'result' }), /当前作用域没有可读变量/);
    console.log('真实库身份：旧消费者对象重新链接同 ABI 新实现，目录核对拒绝旧身份通过');
  } finally {
    fs.writeFileSync(path.join(work, 'stale-protocol.json'), JSON.stringify(stale.messages, null, 2));
    await stale.close();
  }
}
main().catch(error => { console.error(error); process.exitCode = 1; });
