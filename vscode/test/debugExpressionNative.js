'use strict';

/** 真实 GDB 核对源码名称、嵌套字段、索引、作用域、精度与无副作用拒绝。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { spawnSync } = require('node:child_process');
const { DebugClient } = require('./debugClient');
const root = path.resolve(__dirname, '../..'), compiler = process.env.CHALI_BIN;
assert(compiler);
const work = fs.mkdtempSync(path.join(root, 'tmp/debug-expression-'));
const source = path.join(work, 'main.chl');
const lines = [
  'struct Pair { int64 first; uint8 tag; };',
  'struct Line { Pair start; Pair end; };',
  'int64 forbidden(){println("FORBIDDEN EXECUTION");return 99;}',
  'void main(){',
  ' Pair row;row.first=7;row.tag=3;',
  ' Line line;line.start.first=11;line.end.first=19;',
  ' int64 values[]=new[2048];values[1700]=23;',
  ' Pair const& alias=&row;',
  ' uint64 large=(uint64)-1;',
  ' float32 decimal=(float32)1.5;bool flag=true;',
  ' List<int64> numbers;numbers.push(31);List<Pair> rows;rows.push(row);(int64,int64) pair=(17,29);',
  ' int64 visible=41;',
  ' println("OUTER");',
  ' {int64 visible=42;',
  '  println("INNER");',
  ' }',
  ' println("AFTER");',
  ' assert(row.first==7 && values[1700]==23 && visible==41);',
  '}',
];
fs.writeFileSync(source, lines.join('\n'));

async function main() {
  for (const allocator of ['malloc', 'pool']) {
    const prefix = path.join(work, allocator);
    const built = spawnSync(compiler, ['aot', source, '--debug', '--checked', '--allocator', allocator, '--out', prefix],
      { cwd: root, env: process.env, encoding: 'utf8', windowsHide: true, timeout: 180000 });
    assert.equal(built.status, 0, built.stdout + built.stderr);
    const client = new DebugClient(root, compiler);
    try {
      await client.request('initialize', { adapterID: 'chali' });
      await client.request('launch', { executable: prefix + (process.platform === 'win32' ? '.exe' : ''), cwd: root });
      await client.event('initialized');
      const installed = await client.request('setBreakpoints', { source: { path: source }, breakpoints: [' println("OUTER");', '  println("INNER");', ' println("AFTER");'].map(text => ({ line: lines.indexOf(text) + 1 })) });
      assert(installed.breakpoints.every(point => point.verified));
      await client.request('configurationDone');
      let oldFrame, oldValue;
      for (const visible of [41, 42, 41]) {
        const stop = await client.event('stopped');
        const frameId = (await client.request('stackTrace', { threadId: stop.threadId })).stackFrames[0].id;
        const evaluate = expression => client.request('evaluate', { frameId, expression, context: 'watch' });
        if (oldFrame) {
          await assert.rejects(client.request('evaluate', { frameId: oldFrame, expression: 'row' }), /过期|有效/);
          await assert.rejects(client.request('variables', { variablesReference: oldValue }), /过期/);
        }
        for (const [expression, result] of [['row.first + row.tag', '10'], ['line.start.first*2+line.end.first', '41'], ['alias.first', '7'], ['values[1700]+values[0]', '23'], ['large-1', '18446744073709551614'], ['large>9007199254740993', 'true'], ['decimal+2', '3.5'], ['flag && row.first==7', 'true'], ['visible', String(visible)], ['false && absent', 'false']]) assert.equal((await evaluate(expression)).result, result, expression);
        assert.equal((await evaluate('numbers[0]+pair[1]')).result, '60');
        assert.equal((await evaluate('rows[0].first+rows[0].tag')).result, '10');
        assert.equal((await evaluate('pair.0+pair.1')).result, '46');
        await assert.rejects(evaluate('numbers[1]')); await assert.rejects(evaluate('pair[2]'));
        const aggregate = await evaluate('line.start'); assert(aggregate.variablesReference);
        assert.equal((await client.request('variables', { variablesReference: aggregate.variablesReference })).variables[0].value, '11');
        for (const expression of ['forbidden()', 'row.first=99', 'row.first++', '(int64)row', '*(int64*)0', 'values[-1]', 'values[2048]', 'row.missing', 'absent', 'large+1', '1/0']) await assert.rejects(evaluate(expression));
        assert.equal((await evaluate('row.first')).result, '7');
        oldFrame = frameId; oldValue = aggregate.variablesReference;
        await client.request('continue', { threadId: stop.threadId });
      }
      assert.equal((await client.event('exited')).exitCode, 0); await client.event('terminated');
      assert(!client.messages.some(message => message.event === 'output' && message.body.output.includes('FORBIDDEN EXECUTION')));
    } finally { await client.close(); }
  }
  console.log('只读表达式真实 GDB：malloc/pool、字段/引用/大索引、64 位精度、遮蔽、过期、调用与写入拒绝通过');
}
main().catch(error => { console.error(error); process.exitCode = 1; });
