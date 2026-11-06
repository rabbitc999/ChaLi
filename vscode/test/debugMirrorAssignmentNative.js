'use strict';

/** 所有基础类型取引用后经真实 GDB 修改拥有绑定，再从语言引用及恢复执行核对。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { spawnSync } = require('node:child_process');
const { DebugClient } = require('./debugClient');
const root = path.resolve(__dirname, '../..'), compiler = process.env.CHALI_BIN;
const external = process.env.CHALI_DEBUG_MIRROR_LLVM;
assert(compiler, '需要 CHALI_BIN');
const work = fs.mkdtempSync(path.join(root, 'tmp/debug-mirror-assignment-'));
const source = path.join(work, 'main.chl');
const cases = [
  ['int8', 'small', '-7', '-9'], ['int16', 'shortValue', '-300', '6'], ['int32', 'wide', '-70000', '7'],
  ['int64', 'integer', '-9000000000', '-9000000001'], ['uint8', 'byteValue', '255', '230'],
  ['uint16', 'unsignedShort', '65000', '8'], ['uint32', 'unsignedWide', '(uint32)4000000000', '9'],
  ['uint64', 'unsignedInteger', '(uint64)-1', '18446744073709551614'],
  ['float32', 'single', '(float32)1.5', '0.1'], ['float64', 'real', '2.25', '2.5'],
  ['bool', 'truth', 'true', 'false'], ['char', 'letter', "'A'", "'Z'"],
];
const lines = [
  'void nested(int64 argument){', ' int64& argumentView=&argument;', ' println("PARAMETER");',
  ' assert(argument==12 && argumentView==12);', '}',
  'void child(){', ' println("CALLER");', '}', 'void main(){',
  ...cases.flatMap(([type, name, initial]) => [` ${type} ${name}=${initial};`, ` ${type}& ${name}View=&${name};`]),
  ' const int64 fixed=8;int64 const& fixedView=&fixed;',
  ' println("READY");',
  ...cases.map(([type, name, , value]) => {
    const expected = type === 'uint64' ? '(uint64)-2' : type === 'float32' ? '(float32)0.1' : value;
    return ` assert(${name}==${expected} && ${name}View==${expected});`;
  }),
  ' assert(fixed==8 && fixedView==8);',
  ' int64 shadow=1;int64& outerShadow=&shadow;',
  ' {int64 shadow=2;int64& innerShadow=&shadow;', ' println("SHADOW");', ' assert(shadow==7 && innerShadow==7);', ' }',
  ' println("AFTER SHADOW");', ' assert(shadow==8 && outerShadow==8);',
  ' child();', ' nested(11);', '}',
];
fs.writeFileSync(source, lines.join('\n'));

/** 取得所选源码帧的实际变量作用域。 */
async function view(client, thread, name) {
  const frames = (await client.request('stackTrace', { threadId: thread })).stackFrames;
  const frame = name ? frames.find(item => item.name === name) : frames[0]; assert(frame, JSON.stringify(frames));
  const scope = (await client.request('scopes', { frameId: frame.id })).scopes[0].variablesReference;
  const variables = (await client.request('variables', { variablesReference: scope })).variables;
  assert(!variables.some(item => item.name.startsWith('__chali_storage_')));
  return { frame, scope, variables };
}

async function main() {
  for (const allocator of ['malloc', 'pool']) {
    const prefix = path.join(work, allocator);
    const built = spawnSync(compiler, ['build', source, '--debug', '--checked', '--allocator', allocator, '--out', prefix,
      ...(external ? ['--llvm', external] : [])],
      { cwd: root, env: process.env, encoding: 'utf8', windowsHide: true, timeout: 180000 });
    assert.equal(built.status, 0, built.stdout + built.stderr);
    const client = new DebugClient(root, compiler);
    try {
      await client.request('initialize', { adapterID: 'chali' });
      await client.request('launch', { executable: prefix + (process.platform === 'win32' ? '.exe' : ''), cwd: root });
      await client.event('initialized');
      const points = await client.request('setBreakpoints', { source: { path: source },
        breakpoints: [' println("READY");', ' println("SHADOW");', ' println("AFTER SHADOW");', ' println("CALLER");', ' println("PARAMETER");'].map(text => ({ line: lines.indexOf(text) + 1 })) });
      assert(points.breakpoints.every(point => point.verified)); await client.request('configurationDone');
      let stop = await client.event('stopped'), data = await view(client, stop.threadId);
      for (const [type, name, , value] of cases) {
        assert(!data.variables.find(item => item.name === name)?.presentationHint?.attributes?.includes('readOnly'), name);
        await client.request('setVariable', { variablesReference: data.scope, name, value });
        if (!['char'].includes(type)) {
          const expected = type === 'float32' ? String(Math.fround(0.1)) : value;
          assert.equal((await client.request('evaluate', { frameId: data.frame.id, expression: name })).result, expected);
        }
      }
      await assert.rejects(client.request('setVariable', { variablesReference: data.scope, name: 'fixed', value: '9' }), /不可变/);
      await client.request('continue', { threadId: stop.threadId }); stop = await client.event('stopped');
      data = await view(client, stop.threadId);
      assert.equal(data.variables.find(item => item.name === 'shadow').value, '2');
      const innerScope = data.scope;
      await client.request('setVariable', { variablesReference: data.scope, name: 'shadow', value: '7' });
      await client.request('continue', { threadId: stop.threadId }); stop = await client.event('stopped'); data = await view(client, stop.threadId);
      assert.equal(data.variables.find(item => item.name === 'shadow').value, '1');
      await assert.rejects(client.request('setVariable', { variablesReference: innerScope, name: 'shadow', value: '0' }), /过期|有效/);
      await client.request('setVariable', { variablesReference: data.scope, name: 'shadow', value: '8' });
      await client.request('continue', { threadId: stop.threadId }); stop = await client.event('stopped');
      const caller = await view(client, stop.threadId, 'main');
      assert(caller.variables.find(item => item.name === 'integer').presentationHint.attributes.includes('readOnly'));
      await assert.rejects(client.request('setVariable', { variablesReference: caller.scope, name: 'integer', value: '1' }), /栈顶/);
      await client.request('continue', { threadId: stop.threadId }); stop = await client.event('stopped'); data = await view(client, stop.threadId);
      await client.request('setVariable', { variablesReference: data.scope, name: 'argument', value: '12' });
      assert.equal((await client.request('evaluate', { frameId: data.frame.id, expression: 'argument' })).result, '12');
      await client.request('continue', { threadId: stop.threadId });
      assert.equal((await client.event('exited')).exitCode, 0); await client.event('terminated');
      console.log(`真实基础镜像修改：${allocator}、全部基础类型、只读约束、调用方拒绝、参数与语言引用恢复执行通过`);
    } finally {
      fs.writeFileSync(prefix + '-protocol.json', JSON.stringify(client.messages, null, 2)); await client.close();
    }
  }
}
main().catch(error => { console.error(error); process.exitCode = 1; });
