'use strict';

/** 从真实编译器导出的 LLVM 模块核对容器及嵌套记录的调试类型，不执行目标程序。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { spawnSync } = require('node:child_process');
const { parseLayout } = require('../src/debugValue');
const root = path.resolve(__dirname, '../..');
const compiler = process.env.CHALI_BIN;
assert(compiler, '需要指定本阶段 CHALI_BIN');
const work = fs.mkdtempSync(path.join(root, 'tmp/debug-container-metadata-'));
const source = path.join(work, 'main.chl');
fs.writeFileSync(source, [
  'struct Pair { int64 first; String second; };',
  'void main(){',
  ' List<Pair> rows;',
  ' Map<String,Pair> lookup;',
  ' Set<int64> unique;',
  ' println("{} {} {}", rows.len(), lookup.len(), unique.len());',
  '}',
].join('\n'));
const prefix = path.join(work, 'program');
const built = spawnSync(compiler, ['build', source, '--debug', '--emit-llvm', '--out', prefix],
  { cwd: root, env: process.env, encoding: 'utf8', windowsHide: true, timeout: 180000 });
assert.equal(built.status, 0, built.stdout + built.stderr);
const llvm = fs.readFileSync(prefix + '.ll', 'utf8');
const literal = /^@chali_debug_catalog = .* c"(.*)"$/m.exec(llvm);
assert(literal, '缺少编译器调试目录');
const bytes = literal[1].replace(/\\([0-9a-f]{2})/gi, (_, hex) => String.fromCharCode(parseInt(hex, 16)));
const catalog = JSON.parse(Buffer.from(bytes, 'latin1').toString('utf8').replace(/\0$/, ''));
const bindings = catalog.functions.flatMap(fn => fn.variables);
const layout = name => {
  const variable = bindings.find(item => item.name === name);
  assert(variable, '缺少变量 ' + name);
  return parseLayout(catalog.layouts[variable.layout]);
};
const rows = layout('rows');
assert.equal(rows.kind, 'L');
assert.equal(rows.children[0].kind, 'S');
assert.deepEqual(rows.children[0].children.map(field => field.name), ['first', 'second']);
const lookup = layout('lookup');
assert.equal(lookup.kind, 'M');
assert.equal(lookup.children[0].kind, 's');
assert.equal(lookup.children[1].kind, 'S');
assert.equal(layout('unique').kind, 'E');
console.log('容器调试元数据：真实目录保留 List、Map、Set 及嵌套记录字段类型');
