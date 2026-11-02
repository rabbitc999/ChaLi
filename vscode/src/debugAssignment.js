'use strict';

/** 基础变量修改的宿主类型核对和物理槽编码；不接受拥有值或原生表达式。 */
const { parseExpression, evaluateExpression } = require('./debugExpression');

/** 解析单字节 char，支持 JSON 字符串或单引号和基础转义，不执行输入文本。 */
function character(text) {
  if (typeof text !== 'string' || text.length > 32) throw new Error('char 修改需要单字节字符');
  let value;
  if (text.startsWith('"')) {
    try { value = JSON.parse(text); } catch { throw new Error('char 字面量无效'); }
  } else {
    const match = /^'([^'\\]|\\(?:[nrt0'"\\]|x[0-9a-fA-F]{2}|u[0-9a-fA-F]{4}))'$/u.exec(text);
    if (!match) throw new Error('char 修改需要单字符字面量');
    const item = match[1];
    if (!item.startsWith('\\')) value = item;
    else if (item[1] === 'x' || item[1] === 'u') value = String.fromCharCode(parseInt(item.slice(2), 16));
    else value = ({ n: '\n', r: '\r', t: '\t', '0': '\0', "'": "'", '"': '"', '\\': '\\' })[item[1]];
  }
  if (typeof value !== 'string' || [...value].length !== 1 || value.codePointAt(0) > 255) throw new Error('char 超过单字节范围');
  return BigInt(value.codePointAt(0));
}

/** 按源码类型和明确登记的真实 64 位帧槽生成完整字节，避免窄类型写入留下旧高位。 */
async function encodeAssignment(binding, layout, text) {
  if (binding.readOnly) throw new Error('不能修改不可变绑定');
  if (binding.storage || binding.boxed) throw new Error('该变量使用真实绑定镜像，暂不支持安全修改');
  if (binding.address || !layout || !['i', 'u', 'f', 'b', 'c'].includes(layout.kind)) throw new Error('只支持基础变量修改；引用、拥有值和句柄不能直接覆盖');
  const type = binding.type;
  const floating = /^float(?:32|64)$/.test(type || '');
  if (binding.nativeStorage !== (floating ? 'double' : 'i64')) throw new Error('缺少匹配的真实帧槽描述，不能安全修改');
  let value;
  if (type === 'char') value = character(typeof text === 'string' ? text.trim() : text);
  else {
    const unsupported = async () => { throw new Error('修改值只接受字面量及其基础运算，不接受变量或字段'); };
    const result = await evaluateExpression(parseExpression(text), unsupported, unsupported);
    value = result.scalar;
  }
  const bytes = Buffer.alloc(8);
  if (floating) {
    if (layout.kind !== 'f' || layout.width !== 8 || !['bigint', 'number'].includes(typeof value)) throw new Error('浮点变量需要匹配布局的数值');
    if (typeof value === 'bigint' && (value < BigInt(Number.MIN_SAFE_INTEGER) || value > BigInt(Number.MAX_SAFE_INTEGER))) throw new Error('整数转浮点会丢失精度');
    value = Number(value);
    if (type === 'float32') value = Math.fround(value);
    if (!Number.isFinite(value)) throw new Error('浮点修改超出类型范围');
    bytes.writeDoubleLE(value);
  } else if (type === 'bool') {
    if (layout.kind !== 'b' || layout.width !== 1 || typeof value !== 'boolean') throw new Error('bool 变量只接受 bool 值');
    bytes.writeBigUInt64LE(value ? 1n : 0n);
  } else {
    const match = /^(u?int)(8|16|32|64)$/.exec(type || '');
    if ((!match && type !== 'char') || typeof value !== 'bigint') throw new Error('整数变量需要匹配类型的整数');
    const signed = match && match[1] === 'int', bits = BigInt(match ? match[2] : 8);
    if ((type === 'char' ? !['c', 'u'].includes(layout.kind) : layout.kind !== (signed ? 'i' : 'u')) || layout.width !== Number(bits / 8n)) throw new Error('变量类型与调试布局不匹配');
    const low = signed ? -(1n << (bits - 1n)) : 0n, high = (1n << (signed ? bits - 1n : bits)) - 1n;
    if (value < low || value > high) throw new Error('修改值超过 ' + type + ' 范围');
    bytes.writeBigUInt64LE(BigInt.asUintN(64, value));
  }
  return { bytes, text: String(value), type };
}

module.exports = { encodeAssignment };
