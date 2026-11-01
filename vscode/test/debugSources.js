'use strict';

/** 已有原生产物的泛型名、源码列及宏调用位置验收，不依赖源旁临时映射文件。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { spawnSync } = require('node:child_process');
const { DebugClient } = require('./debugClient');
const root = path.resolve(__dirname, '../..');
const compiler = process.env.CHALI_BIN || path.join(root, 'chali.exe');
const work = fs.mkdtempSync(path.join(root, 'tmp/debug-sources-'));

/** 查询当前真实机器帧及其可见变量。 */
async function view(client, thread) {
  const stack = await client.request('stackTrace', { threadId: thread });
  const frame = stack.stackFrames[0];
  const scopes = await client.request('scopes', { frameId: frame.id });
  const values = await client.request('variables', { variablesReference: scopes.scopes[0].variablesReference });
  return { frame, values: Object.fromEntries(values.variables.map(value => [value.name, value])) };
}

/** 用编译后的孤立产物调试，编译器路径无效也不能触发额外构建。 */
async function scenario(name, lines, points, inspect, allocator = 'malloc') {
  const source = path.join(work, name + '.chl'); fs.writeFileSync(source, lines.join('\n') + '\n');
  const prefix = path.join(work, name);
  const build = spawnSync(compiler, ['aot', source, '--debug', '--checked', '--allocator', allocator, '--out', prefix], { cwd: root, env: process.env, encoding: 'utf8', windowsHide: true, timeout: 180000 });
  assert.equal(build.status, 0, build.stdout + build.stderr);
  if (name === 'macro-private') {
    const executable = fs.readFileSync(prefix + (process.platform === 'win32' ? '.exe' : ''));
    const begin = executable.indexOf(Buffer.from('{"version":1,"pointerBytes":8'));
    assert(begin >= 0, '调试产物必须内嵌来源目录');
    const catalog = JSON.parse(executable.subarray(begin, executable.indexOf(0, begin)).toString('utf8'));
    const inner = catalog.macros.find(macro => macro.name === 'Inner');
    const outer = catalog.macros.find(macro => macro.name === 'Outer');
    assert(inner && outer); assert.equal(inner.parent, outer.id);
    assert.equal(inner.invocation.line, 8); assert.equal(inner.definition.line, 2);
    assert.equal(outer.call.line, 10); assert.equal(outer.definition.line, 7);
    assert.equal(path.resolve(inner.definition.file), path.join(work, 'debug_macros.chl'));
    assert(catalog.positions.some(position => position.macro === inner.id && position.definition.line === 4));
  }
  if (name === 'macro-pool' && process.env.CHALI_RELEASE_COMPARE) {
    const outputs = [];
    for (const [label, executable] of [['before', process.env.CHALI_RELEASE_COMPARE], ['after', compiler]]) {
      const irPrefix = path.join(work, 'release-' + label);
      const emitted = spawnSync(executable, ['aot', source, '--unchecked', '--emit-llvm', '--allocator', allocator, '--out', irPrefix], { cwd: root, env: process.env, encoding: 'utf8', windowsHide: true, timeout: 180000 });
      assert.equal(emitted.status, 0, emitted.stdout + emitted.stderr);
      outputs.push(fs.readFileSync(irPrefix + '.ll', 'utf8').replace(/\r\n/g, '\n'));
    }
    assert.equal(outputs[0], outputs[1], '调试镜像不能改变该发布用例的 LLVM 生成代码');
    console.log('宏与扩容用例：刷新前后发布 LLVM 生成代码逐字一致（未计时）');
  }
  const client = new DebugClient(root, path.join(work, 'no-compiler'));
  try {
    await client.request('initialize', { adapterID: 'chali' });
    await client.request('launch', { executable: prefix + (process.platform === 'win32' ? '.exe' : ''), cwd: root });
    await client.event('initialized');
    const installed = await client.request('setBreakpoints', { source: { path: source }, breakpoints: points.map(line => ({ line })) });
    assert(installed.breakpoints.every(item => item.verified), JSON.stringify(installed));
    await client.request('configurationDone');
    await inspect(client, source);
    await client.request('continue', { threadId: 1 }); await client.event('terminated');
    const diagnostics = client.messages.filter(message => message.event === 'output').map(message => message.body.output).join('');
    assert(!diagnostics.includes('调试源映射不可用'), diagnostics);
  } finally {
    fs.writeFileSync(path.join(work, name + '-messages.json'), JSON.stringify({ messages: client.messages, stderr: client.stderr }, null, 2));
    await client.close();
  }
}

async function main() {
  await scenario('macro-export', [
    '$macro Declare(name:ident)',
    'int64 name=42;',
    '$end_macro',
    'void main() {',
    '    $Declare(exported);',
    '    println("EXPORTED {}",exported);',
    '}',
  ], [6], async client => {
    const stop = await client.event('stopped'), data = await view(client, stop.threadId);
    assert.equal(data.values.exported.value, '42', '名称参数生成的调用方声明仍应可见');
  });
  await scenario('interface-pattern', [
    'interface Base {int64 value();};',
    'struct First:Base {int64 value(){return 7;}};',
    'void main() {',
    '    First first; Base item=first;',
    '    match(item) {',
    '        First(found) => {',
    '            println("FOUND {}",found.value());',
    '        }',
    '        _ => {}',
    '    }',
    '    println("AFTER");',
    '}',
  ], [7,11], async client => {
    let stop = await client.event('stopped'), data = await view(client, stop.threadId);
    assert(data.values.found); assert.match(data.values.found.type, /First/);
    await client.request('continue', {threadId: stop.threadId});
    stop = await client.event('stopped'); data = await view(client, stop.threadId);
    assert(!data.values.found);
  });
  await scenario('owned-cleanup', [
    'void main() {',
    '    {',
    '        String message="scoped";',
    '        println("TEXT {}",message);',
    '    }',
    '    println("AFTER");',
    '}',
  ], [4], async client => {
    let stop = await client.event('stopped'), data = await view(client, stop.threadId);
    assert(data.values.message);
    for (let count=0; count<4 && data.frame.line!==6; ++count) {
      await client.request('next', {threadId: stop.threadId});
      stop = await client.event('stopped'); data = await view(client, stop.threadId);
      assert(data.frame.line >= 4 && data.frame.line <= 6, '清理步进必须保持用户源码位置');
    }
    assert.equal(data.frame.line, 6); assert(!data.values.message);
  });
  await scenario('enum-pattern', [
    'enum Choice { Empty, Number(int64 value), Pair(int64 left, int64 right) };',
    'void main() {',
    '    Choice choice=Choice::Pair(7,9);',
    '    match(choice) {',
    '        Choice::Pair(left,right) => {',
    '            println("PAIR {} {}",left,right);',
    '        }',
    '        Choice::Number(value) => { println("NUMBER {}",value); }',
    '        Choice::Empty => {}',
    '    }',
    '    println("AFTER");',
    '}',
  ], [6,11], async client => {
    let stop = await client.event('stopped'), data = await view(client, stop.threadId);
    assert.equal(data.values.left.value, '7'); assert.equal(data.values.right.value, '9');
    assert(!data.values.value);
    await client.request('continue', {threadId: stop.threadId});
    stop = await client.event('stopped'); data = await view(client, stop.threadId);
    assert(!data.values.left && !data.values.right && !data.values.value, '模式变量不能泄漏到分支外');
  });
  fs.writeFileSync(path.join(work, 'debug_macros.chl'), [
    'module debug_macros;',
    '$macro Inner(n)',
    'int64 scratch=n+1;',
    'println("MACRO {}",scratch);',
    'probe();',
    '$end_macro',
    '$macro Outer(n)',
    '$Inner(n);',
    '$end_macro',
    'void probe(){println("PROBE");}',
  ].join('\n')+'\n');
  await scenario('macro-private', [
    'import debug_macros;', '', '', '', '', '', '',
    'void main() {',
    '    int64 scratch=80;',
    '    $Outer(scratch);',
    '    println("CALLER {}",scratch);',
    '}',
  ], [10,11], async client => {
    let stop = await client.event('stopped'), data = await view(client, stop.threadId);
    assert.equal(data.values.scratch.value, '80', '宏内部局部不能遮蔽调用方展示');
    const scopes = await client.request('scopes', {frameId: data.frame.id});
    assert(scopes.scopes.some(scope => scope.name === '$Outer'), JSON.stringify(scopes));
    const installed = await client.request('setBreakpoints', {source: {path: path.join(work,'debug_macros.chl')}, breakpoints: [{line:10}]});
    assert(installed.breakpoints[0].verified);
    await client.request('continue', {threadId: stop.threadId});
    stop = await client.event('stopped');
    const stack = await client.request('stackTrace', {threadId:stop.threadId});
    const caller = stack.stackFrames.find(frame => frame.name === 'main'); assert(caller);
    const active = await client.request('scopes', {frameId:caller.id});
    const innerScope = active.scopes.find(scope => scope.name === '$Inner'); assert(innerScope,JSON.stringify(active));
    const innerValues = await client.request('variables', {variablesReference:innerScope.variablesReference});
    assert(innerValues.variables.some(value => value.name === 'scratch' && value.value === '81'), JSON.stringify(innerValues));
    await client.request('continue', {threadId: stop.threadId});
    stop = await client.event('stopped'); data = await view(client, stop.threadId);
    assert.equal(data.values.scratch.value, '80');
    const after = await client.request('scopes', {frameId: data.frame.id});
    assert.equal(after.scopes.length, 1, '宏展开结束后隐藏宏内部变量');
  });
  await scenario('generic', [
    'struct Holder<T> { T value; };',
    'T identity<T>(T value) {',
    '    println("GENERIC");',
    '    return value;',
    '}',
    'void main() {',
    '    Holder<int64> item; item.value=7;',
    '    int64 result=identity<int64>(item.value);',
    '    println("AFTER {}",result);',
    '}',
  ], [8, 3], async (client, source) => {
    let stop = await client.event('stopped'), data = await view(client, stop.threadId);
    assert.equal(data.frame.line, 8); assert.equal(data.frame.name, 'main'); assert(data.frame.column > 1);
    assert.match(data.values.item.type, /Holder<int64>/);
    await client.request('continue', { threadId: stop.threadId }); stop = await client.event('stopped'); data = await view(client, stop.threadId);
    assert.equal(data.frame.line, 3); assert.equal(data.frame.name, 'identity<int64>'); assert(data.frame.column > 1); assert.equal(data.values.value.value, '7');
    assert.equal(path.resolve(data.frame.source.path), source);
  });
  for (const allocator of ['malloc', 'pool']) await scenario('macro-' + allocator, [
    '$macro Increase(mut &value)',
    'value=value+1;',
    '$end_macro',
    'void main() {',
    '    int64 number=1;',
    '    $Increase(number);',
    '    int64 pressure[]=new[32768];',
    '    pressure[0]=9;',
    '    println("DONE {}",number);',
    '}',
  ], [6, 9], async client => {
    let stop = await client.event('stopped'), data = await view(client, stop.threadId);
    assert.equal(data.frame.line, 6); assert(data.frame.column > 1); assert.equal(data.values.number.value, '1');
    await client.request('next', { threadId: stop.threadId }); stop = await client.event('stopped'); data = await view(client, stop.threadId);
    assert.equal(data.frame.line, 7); assert.equal(data.values.number.value, '2');
    await client.request('continue', { threadId: stop.threadId }); stop = await client.event('stopped'); data = await view(client, stop.threadId);
    assert.equal(data.frame.line, 9); assert.equal(data.values.number.value, '2');
  }, allocator);
  await scenario('boxed-parameter', [
    '$macro Increase(mut &value)',
    'value=value+1;',
    '$end_macro',
    'int64 update(int64 parameter) {',
    '    $Increase(parameter);',
    '    return parameter;',
    '}',
    'void main() {',
    '    int64 local=4;',
    '    int64 result=update(local);',
    '    println("LOCAL {} RESULT {}",local,result);',
    '}',
  ], [6, 11], async client => {
    let stop = await client.event('stopped'), data = await view(client, stop.threadId);
    assert.equal(data.frame.line, 6); assert.equal(data.values.parameter.value, '5');
    await client.request('continue', { threadId: stop.threadId }); stop = await client.event('stopped'); data = await view(client, stop.threadId);
    assert.equal(data.frame.line, 11); assert.equal(data.values.local.value, '4'); assert.equal(data.values.result.value, '5');
  }, 'pool');
  await scenario('boxed-scalars', [
    'void change(bool mut& truth, float32 mut& decimal, uint8 mut& tiny) {',
    '    truth=true; decimal=(float32)2.5; tiny=(uint8)9;',
    '}',
    'void main() {',
    '    bool truth=false;',
    '    float32 decimal=(float32)1.5;',
    '    uint8 tiny=3;',
    '    change(&truth,&decimal,&tiny);',
    '    println("SCALARS {} {} {}",truth,decimal,tiny);',
    '}',
  ], [9], async client => {
    const stop = await client.event('stopped'), data = await view(client, stop.threadId);
    assert.equal(data.frame.line, 9); assert.equal(data.values.truth.value, 'true'); assert.equal(data.values.decimal.value, '2.5'); assert.equal(data.values.tiny.value, '9');
  }, 'pool');
  console.log('调试来源：已有产物内嵌映射、泛型函数／类型可读名、真实列及宏调用单步通过');
}
main().catch(error => { console.error(error); process.exitCode = 1; });
