'use strict';

/** 真实 GDB 验证标准 DAP 反汇编请求：向前、向后、分页参数与无效输入拒绝。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { spawnSync } = require('node:child_process');
const { DebugClient } = require('./debugClient');
const root = path.resolve(__dirname, '../..');
const compiler = process.env.CHALI_BIN || path.join(root, 'chali.exe');
const work = fs.mkdtempSync(path.join(root, 'tmp/debug-disassemble-'));
const source = path.join(work, 'main.chl');
fs.writeFileSync(source, 'int64 add(int64 a, int64 b) { int64 sum = a + b; return sum; }\nvoid main() { int64 v = add(2, 3); println("{}", v); }\n');
const built = spawnSync(compiler, ['build', source, '--debug', '--checked', '--allocator', 'malloc', '--out', path.join(work, 'program')], { cwd: root, env: process.env, encoding: 'utf8', windowsHide: true, timeout: 180000 });
assert.equal(built.status, 0, built.stdout + built.stderr);

/** 反汇编返回的地址按数值比较。 */
const addresses = (instructions) => instructions.map((item) => BigInt(item.address));

async function main() {
  const client = new DebugClient(root, compiler);
  try {
    const capabilities = await client.request('initialize', { adapterID: 'chali' });
    assert.equal(capabilities.supportsDisassembleRequest, true);
    await client.request('launch', { executable: path.join(work, 'program' + (process.platform === 'win32' ? '.exe' : '')), cwd: root });
    await client.event('initialized');
    await client.request('setBreakpoints', { source: { path: source }, breakpoints: [{ line: 1 }] });
    await client.request('configurationDone');
    const stop = await client.event('stopped');
    const stack = await client.request('stackTrace', { threadId: stop.threadId });
    const frame = stack.stackFrames[0];
    assert.match(frame.name, /add/);
    const pc = frame.instructionPointerReference;
    assert.match(pc, /^0x[0-9a-f]+$/i);

    // 从当前指令开始的反汇编：地址自 PC 起递增、文本非空。
    const forward = await client.request('disassemble', { memoryReference: pc, offset: 0, instructionOffset: 0, instructionCount: 6, resolveSymbols: true });
    assert.equal(forward.instructions.length, 6);
    const forwardAddresses = addresses(forward.instructions);
    assert.equal(forwardAddresses[0], BigInt(pc));
    for (let index = 1; index < forwardAddresses.length; ++index) assert(forwardAddresses[index] > forwardAddresses[index - 1]);
    for (const item of forward.instructions) {
      assert.match(item.address, /^0x[0-9a-f]+$/);
      assert.equal(typeof item.instruction, 'string');
      assert(item.instruction.length > 0);
      assert.equal(item.presentationHint, 'normal');
    }
    assert.equal(forward.memoryReference, pc);

    // 负指令偏移返回 PC 之前的指令，PC 仍在窗口内。
    const backward = await client.request('disassemble', { memoryReference: pc, offset: 0, instructionOffset: -2, instructionCount: 4, resolveSymbols: false });
    assert.equal(backward.instructions.length, 4);
    const backwardAddresses = addresses(backward.instructions);
    assert(backwardAddresses.includes(BigInt(pc)), JSON.stringify(backward.instructions));
    for (let index = 1; index < backwardAddresses.length; ++index) assert(backwardAddresses[index] > backwardAddresses[index - 1]);

    // 地址偏移按字节加到起始地址。
    const shifted = await client.request('disassemble', { memoryReference: pc, offset: 1, instructionOffset: 0, instructionCount: 2, resolveSymbols: false });
    assert.equal(shifted.instructions.length, 2);
    assert(addresses(shifted.instructions)[0] >= BigInt(pc) + 1n);

    // 无效地址与数量必须在读取前拒绝。
    await assert.rejects(client.request('disassemble', { memoryReference: 'main', offset: 0, instructionOffset: 0, instructionCount: 4 }), /地址/);
    await assert.rejects(client.request('disassemble', { memoryReference: pc, offset: 0, instructionOffset: 0, instructionCount: 0 }), /数量/);
    await assert.rejects(client.request('disassemble', { memoryReference: pc, offset: 0, instructionOffset: 0, instructionCount: 4097 }), /数量/);

    // 反汇编不能恢复目标执行。
    const afterStop = client.messages.slice(client.messages.findIndex((message) => message.event === 'stopped') + 1);
    assert(!afterStop.some((message) => message.event === 'continued'), '反汇编不能恢复目标执行');
    await client.request('continue', { threadId: stop.threadId });
    await client.event('terminated');
  } finally {
    fs.writeFileSync(path.join(work, 'messages.json'), JSON.stringify({ messages: client.messages, stderr: client.stderr }, null, 2));
    await client.close();
  }
  console.log('原生调试反汇编：DAP 请求、前后指令窗口、字节偏移、无效输入拒绝及不恢复执行通过');
}
main().catch((error) => { console.error(error); process.exitCode = 1; });
