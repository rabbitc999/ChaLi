'use strict';

/** 真实 GDB 验证记录、数组、字符串与引用的结构化查看，以及分页和过期拒绝。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { spawnSync } = require('node:child_process');
const { DebugClient } = require('./debugClient');
const root = path.resolve(__dirname, '../..');
const compiler = process.env.CHALI_BIN || path.join(root, 'chali.exe');
const work = fs.mkdtempSync(path.join(root, 'tmp/debug-value-'));
const source = path.join(work, 'main.chl');
const lines = [
  'struct Pair { int64 first; uint8 tag; };',
  'struct Line { Pair start; Pair end; int64 count; };',
  'enum Shape { Circle(int64 radius), Empty };',
  'void main() {',
  '    Pair row; row.first=7; row.tag=3;',
  '    String text="hello";',
  '    int64 values[]=new[12];',
  '    for (int64 i in std::core::range(0, 12)) { values[i]=i*2; }',
  '    Pair const& alias=&row;',
  '    Line line; line.start.first=1; line.start.tag=2; line.end.first=9; line.end.tag=8; line.count=5;',
  '    Shape shape=Shape::Circle(4);',
  '    Shape empty=Shape::Empty;',
  '    Option<int64> maybe=Option<int64>::Value(3);',
  '    Option<int64> none=Option<int64>::None;',
  '    Result<int64, int64> ok=Result<int64, int64>::Ok(9);',
  '    Result<int64, int64> err=Result<int64, int64>::Fail(5);',
  '    println("READY {}", row.first);',
  '    int64 keep=1;',
  '    println("KEEP {}", keep);',
  '}',
];
fs.writeFileSync(source, lines.join('\n') + '\n');
const ready = lines.indexOf('    println("READY {}", row.first);') + 1;
const keep = lines.indexOf('    println("KEEP {}", keep);') + 1;
const built = spawnSync(compiler, ['build', source, '--debug', '--checked', '--allocator', 'malloc', '--out', path.join(work, 'program')], { cwd: root, env: process.env, encoding: 'utf8', windowsHide: true, timeout: 180000 });
assert.equal(built.status, 0, built.stdout + built.stderr);

/** 取得当前帧的可见变量，保留展开引用。 */
async function variables(client, thread) {
  const stack = await client.request('stackTrace', { threadId: thread });
  const scopes = await client.request('scopes', { frameId: stack.stackFrames[0].id });
  const data = await client.request('variables', { variablesReference: scopes.scopes[0].variablesReference });
  return Object.fromEntries(data.variables.map((value) => [value.name, value]));
}

/** 展开一个结构化值并按名字索引子项。 */
async function expand(client, value, start, count) {
  assert(value?.variablesReference, '需要可展开的结构化引用：' + JSON.stringify(value));
  const args = { variablesReference: value.variablesReference };
  if (start !== undefined) args.start = start;
  if (count !== undefined) args.count = count;
  const data = await client.request('variables', args);
  return Object.fromEntries(data.variables.map((item) => [item.name, item]));
}

async function main() {
  const client = new DebugClient(root, compiler);
  try {
    await client.request('initialize', { adapterID: 'chali' });
    await client.request('launch', { executable: path.join(work, 'program' + (process.platform === 'win32' ? '.exe' : '')), cwd: root });
    await client.event('initialized');
    const points = await client.request('setBreakpoints', { source: { path: source }, breakpoints: [{ line: ready }, { line: keep }] });
    assert(points.breakpoints.every((point) => point.verified));
    await client.request('configurationDone');
    let stop = await client.event('stopped');
    let values = await variables(client, stop.threadId);

    // 记录：摘要文本、字段顺序、字段类型和内存引用。
    assert.equal(values.row.value, '{…}');
    assert.equal(values.row.type, 'Pair');
    const row = await expand(client, values.row);
    assert.deepEqual(Object.keys(row), ['first', 'tag']);
    assert.equal(row.first.value, '7');
    assert.equal(row.first.type, 'int64');
    assert.equal(row.tag.value, '3');
    assert.equal(row.tag.type, 'uint8');
    const raw = await client.request('readMemory', { memoryReference: values.row.memoryReference, count: 9 });
    const bytes = Buffer.from(raw.data, 'base64');
    assert.equal(bytes.readBigInt64LE(0), 7n);
    assert.equal(bytes[8], 3);

    // 字符串按语言存储解出内容，不再显示句柄。
    assert.equal(values.text.value, '"hello"');
    assert.equal(values.text.variablesReference, 0);

    // 数组长度、元素、分页与越界页。
    assert.equal(values.values.value, '[12]');
    const all = await expand(client, values.values, 5, 3);
    assert.deepEqual(Object.keys(all), ['5', '6', '7']);
    assert.equal(all['5'].value, '10');
    assert.equal(all['7'].value, '14');
    const tail = await expand(client, values.values, 10, 10);
    assert.deepEqual(Object.keys(tail), ['10', '11']);

    // 引用：展开目标是记录本身。
    assert.equal(values.alias.value, '&{…}');
    const alias = await expand(client, values.alias);
    assert.equal(alias.first.value, '7');
    assert.equal(alias.tag.value, '3');

    // 源码布局保留嵌套记录，按每层声明顺序和实际偏移展开。
    const line = await expand(client, values.line);
    assert.deepEqual(Object.keys(line), ['start', 'end', 'count']);
    const start = await expand(client,line.start), end = await expand(client,line.end);
    assert.equal(start.first.value, '1');
    assert.equal(start.tag.value, '2');
    assert.equal(end.first.value, '9');
    assert.equal(end.tag.value, '8');
    assert.equal(line.count.value, '5');

    // 载荷枚举、Option 与 Result 按标签显示变体，载荷字段只列真实名字。
    assert.equal(values.shape.value, 'Circle(…)');
    const circle = await expand(client, values.shape);
    assert.deepEqual(Object.keys(circle), ['radius']);
    assert.equal(circle.radius.value, '4');
    assert.equal(circle.radius.type, 'int64');
    assert.equal(values.empty.value, 'Empty()');
    assert.equal(values.maybe.value, 'Value(…)');
    const some = await expand(client, values.maybe);
    assert.deepEqual(Object.keys(some), ['value']);
    assert.equal(some.value.value, '3');
    assert.equal(values.none.value, 'None()');
    assert.equal(values.ok.value, 'Ok(…)');
    assert.equal((await expand(client, values.ok)).value.value, '9');
    assert.equal(values.err.value, 'Fail(…)');
    assert.equal((await expand(client, values.err)).error.value, '5');

    // 继续后旧的展开引用必须失效。
    const stale = values.row.variablesReference;
    await client.request('continue', { threadId: stop.threadId });
    stop = await client.event('stopped');
    values = await variables(client, stop.threadId);
    assert.equal(values.text.value, '"hello"');
    await assert.rejects(client.request('variables', { variablesReference: stale }), /过期|无效/);
    await client.request('continue', { threadId: stop.threadId });
    await client.event('terminated');
  } finally {
    fs.writeFileSync(path.join(work, 'messages.json'), JSON.stringify({ messages: client.messages, stderr: client.stderr }, null, 2));
    await client.close();
  }
  console.log('原生调试取值：记录、嵌套记录、数组分页、字符串、引用展开、载荷枚举、Option／Result、内存引用及过期拒绝通过');
}
main().catch((error) => { console.error(error); process.exitCode = 1; });
