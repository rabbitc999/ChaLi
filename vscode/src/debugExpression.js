'use strict';

/** 有界的 ChaLi 调试表达式；只生成宿主运算树，绝不把用户输入发给 GDB。 */
const PRECEDENCE = { '||': 1, '&&': 2, '==': 3, '!=': 3, '<': 4, '<=': 4, '>': 4, '>=': 4, '+': 5, '-': 5, '*': 6, '/': 6, '%': 6 };

/** 在任何目标读取之前完整解析，拒绝调用、赋值、转换及超出预算的输入。 */
function parseExpression(source) {
  if (typeof source !== 'string' || source.length > 4096) throw new Error('调试表达式超过长度预算');
  const tokens = []; let at = 0;
  while (at < source.length) {
    if (/\s/u.test(source[at])) { ++at; continue; }
    const rest = source.slice(at);
    const match = /^(?:0[xX][0-9a-fA-F]+|(?:[0-9]+\.[0-9]+|[0-9]+)(?:[eE][+-]?[0-9]+)?|[\p{L}_][\p{L}\p{N}_]*|&&|\|\||==|!=|<=|>=|[().\[\]+*/%<>!\-])/u.exec(rest);
    if (!match) throw new Error('不支持的调试表达式，位置 ' + (at + 1));
    tokens.push(match[0]); at += match[0].length;
    if (tokens.length > 512) throw new Error('调试表达式超过节点预算');
  }
  let cursor = 0;
  function expression(minimum = 0, depth = 0) {
    if (depth > 64) throw new Error('调试表达式超过嵌套预算');
    const token = tokens[cursor++]; let node;
    if (['+', '-', '!'].includes(token)) node = { kind: 'unary', op: token, value: expression(7, depth + 1) };
    else if (token === '(') {
      node = expression(0, depth + 1);
      if (tokens[cursor++] !== ')') throw new Error('调试表达式缺少右括号');
    } else if (token === 'true' || token === 'false') node = { kind: 'literal', value: token === 'true' };
    else if (/^[0-9]/.test(token || '')) {
      const floating = !/^0[xX]/.test(token) && /[.eE]/.test(token);
      const value = floating ? Number(token) : BigInt(token);
      if (floating && !Number.isFinite(value)) throw new Error('浮点字面量超出范围');
      node = { kind: 'literal', value };
    } else if (/^[\p{L}_][\p{L}\p{N}_]*$/u.test(token || '') && !['new', 'delete', 'move', 'sizeof'].includes(token)) node = { kind: 'name', name: token };
    else throw new Error('不支持的调试表达式');
    while (cursor < tokens.length) {
      const next = tokens[cursor];
      if (next === '.') {
        ++cursor; const name = tokens[cursor++];
        if (/^[0-9]+$/.test(name || '')) {
          node = { kind: 'index', target: node, index: { kind: 'literal', value: BigInt(name) }, tupleOnly: true }; continue;
        }
        if (!/^[\p{L}_][\p{L}\p{N}_]*$/u.test(name || '')) throw new Error('字段名称无效');
        node = { kind: 'field', target: node, name }; continue;
      }
      if (next === '[') {
        ++cursor; const index = expression(0, depth + 1);
        if (tokens[cursor++] !== ']') throw new Error('调试表达式缺少右方括号');
        node = { kind: 'index', target: node, index }; continue;
      }
      const precedence = Object.hasOwn(PRECEDENCE, next) ? PRECEDENCE[next] : undefined;
      if (precedence === undefined || precedence < minimum) break;
      ++cursor; node = { kind: 'binary', op: next, left: node, right: expression(precedence + 1, depth + 1) };
    }
    return node;
  }
  const tree = expression();
  if (cursor !== tokens.length) throw new Error('不支持的调试表达式：函数调用、转换和赋值不可用');
  return tree;
}

/** 从经过类型核对的展示值提取基础标量，禁止对聚合摘要或截断文本运算。 */
function scalar(value) {
  if (Object.hasOwn(value, 'scalar')) return value.scalar;
  const type = value.type || '', text = value.text || '';
  if (/^(?:u?int)(?:8|16|32|64)$/.test(type) && /^-?[0-9]+$/.test(text)) return BigInt(text);
  if (type === 'bool' && /^(?:true|false|0|1)$/.test(text)) return text === 'true' || text === '1';
  if (/^float(?:32|64)$/.test(type) && text.trim() && Number.isFinite(Number(text))) return Number(text);
  throw new Error('该值不支持基础标量运算：' + type);
}

/** 保留整数精度；混合整数和浮点数只接受可精确表示的整数。 */
function numbers(left, right) {
  if (!['bigint', 'number'].includes(typeof left) || !['bigint', 'number'].includes(typeof right)) throw new Error('算术和顺序比较需要数值');
  if (typeof left !== typeof right) {
    for (const value of [left, right]) if (typeof value === 'bigint' && (value > BigInt(Number.MAX_SAFE_INTEGER) || value < BigInt(Number.MIN_SAFE_INTEGER))) throw new Error('整数转浮点会丢失精度');
    return [Number(left), Number(right)];
  }
  return [left, right];
}

/** 只在宿主计算基础运算；名称和子值由当前暂停的只读解析器提供。 */
async function evaluateExpression(tree, resolve, project) {
  const wrap = scalar => {
    if (typeof scalar === 'number' && !Number.isFinite(scalar)) throw new Error('浮点运算结果非有限值');
    if (typeof scalar === 'bigint' && (scalar < -(1n << 63n) || scalar >= (1n << 64n))) throw new Error('整数运算超过 64 位范围');
    return { scalar, text: String(scalar), type: typeof scalar === 'boolean' ? 'bool' : typeof scalar === 'number' ? 'float64' : scalar >= (1n << 63n) ? 'uint64' : 'int64' };
  };
  async function visit(node) {
    if (node.kind === 'literal') return wrap(node.value);
    if (node.kind === 'name') return resolve(node.name);
    if (node.kind === 'field') return project(await visit(node.target), node.name, false);
    if (node.kind === 'index') {
      const index = scalar(await visit(node.index));
      if (typeof index !== 'bigint' || index < 0n || index > 2147483647n) throw new Error('索引必须是范围内的非负整数');
      return project(await visit(node.target), Number(index), true, node.tupleOnly);
    }
    const left = scalar(await visit(node.kind === 'unary' ? node.value : node.left));
    if (node.kind === 'unary') {
      if (node.op === '!') { if (typeof left !== 'boolean') throw new Error('逻辑非需要 bool'); return wrap(!left); }
      if (!['bigint', 'number'].includes(typeof left)) throw new Error('一元算术需要数值');
      return wrap(node.op === '-' ? -left : left);
    }
    if (['&&', '||'].includes(node.op)) {
      if (typeof left !== 'boolean') throw new Error('逻辑运算需要 bool');
      if ((node.op === '&&' && !left) || (node.op === '||' && left)) return wrap(left);
      const right = scalar(await visit(node.right));
      if (typeof right !== 'boolean') throw new Error('逻辑运算需要 bool');
      return wrap(right);
    }
    let right = scalar(await visit(node.right)); let a = left;
    if (['==', '!='].includes(node.op) && typeof a === 'boolean' && typeof right === 'boolean') return wrap(node.op === '==' ? a === right : a !== right);
    [a, right] = numbers(a, right);
    switch (node.op) {
      case '+': return wrap(a + right);
      case '-': return wrap(a - right);
      case '*': return wrap(a * right);
      case '/': case '%':
        if (right === 0n || right === 0) throw new Error('除数不能为零');
        return wrap(node.op === '/' ? a / right : a % right);
      case '<': return wrap(a < right);
      case '<=': return wrap(a <= right);
      case '>': return wrap(a > right);
      case '>=': return wrap(a >= right);
      case '==': return wrap(a === right);
      case '!=': return wrap(a !== right);
      default: throw new Error('不支持的运算符');
    }
  }
  return visit(tree);
}

module.exports = { parseExpression, evaluateExpression };
