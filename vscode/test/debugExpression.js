'use strict';

/** 宿主只读表达式的语法、安全边界、精度与短路；无目标进程参与。 */
const assert = require('node:assert/strict');
const { parseExpression, evaluateExpression } = require('../src/debugExpression');

async function main() {
  const reads = [];
  const run = async source => evaluateExpression(parseExpression(source), async name => {
    reads.push(name);
    const values = { 大数: { text: '18446744073709551615', type: 'uint64' }, negative: { text: '-7', type: 'int8' }, decimal: { text: '1.5', type: 'float32' }, flag: { text: 'true', type: 'bool' }, row: { text: '{…}', type: 'Row' } };
    if (!values[name]) throw new Error('未知变量'); return values[name];
  }, async (target, key, indexed) => {
    reads.push([target.type, key, indexed]); return { text: '9', type: 'int64' };
  });
  for (const [source, expected] of [['2+3*4', '14'], ['(2+3)*4', '20'], ['negative / 2', '-3'], ['negative % 2', '-1'], ['大数-1', '18446744073709551614'], ['大数>9007199254740993', 'true'], ['decimal+2', '3.5'], ['flag && !false', 'true'], ['row.field+row[2+1]', '18']]) assert.equal((await run(source)).text, expected, source);
  const before = reads.length;
  assert.equal((await run('false && unknown')).text, 'false');
  assert.equal((await run('true || unknown')).text, 'true');
  assert.equal(reads.length, before);
  for (const source of ['foo()', 'row.method()', 'x=2', 'x++', '(int64)x', '*pointer', 'a::b', 'x;bad()', 'new[2]', '"text"', 'x[', 'x.', 'x constructor x', '']) {
    const count = reads.length;
    await assert.rejects(run(source), /表达式|字段|括号/); assert.equal(reads.length, count, source);
  }
  for (const source of ['1/0', '1%0', 'flag+1', '!2', '大数+1', '大数+0.5', 'row+1', 'row[-1]', 'row[0.5]', '1e999']) await assert.rejects(run(source));
  assert.throws(() => parseExpression('('.repeat(100) + '1' + ')'.repeat(100)), /预算/);
  assert.throws(() => parseExpression('a'.repeat(4097)), /预算/);
  console.log('只读表达式：预解析拒绝调用/写入、中文名称、精确整数、浮点、字段/索引、短路与预算通过');
}
main().catch(error => { console.error(error); process.exitCode = 1; });
