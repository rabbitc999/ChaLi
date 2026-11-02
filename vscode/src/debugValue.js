'use strict';

/**
 * 版本化调试布局的只读解码：按逻辑地址读取目标内存，不调用目标函数或用户方法。
 * 布局文本与编译器 NativeDebugInfo 登记、运行时 value_text 使用同一格式：
 * 类别、名称长度、名称、偏移、数量、宽度、回调占位、子节点数量，子节点按声明顺序递归。
 */

const MAX_DEPTH = 32;
const MAX_NODES = 65536;
const MAX_TEXT = 4096;
const MAX_PAGE = 1000;
const DEFAULT_PAGE = 100;

/** 顺序解析布局文本的游标，拒绝截断、溢出和超出预算的节点。 */
class Cursor {
  constructor(text) {
    if (typeof text !== 'string' || text.length > 1048576) throw new Error('调试布局超过预算');
    this.text = Buffer.from(text, 'utf8'); this.at = 0; this.nodes = 0;
  }
  /** 读取一个非负整数及分隔冒号。 */
  number() {
    const start = this.at;
    let value = 0;
    while (this.at < this.text.length && this.text[this.at] >= 0x30 && this.text[this.at] <= 0x39) {
      value = value * 10 + (this.text[this.at] - 0x30);
      if (!Number.isSafeInteger(value) || value > 0x7fffffff) throw new Error('调试布局数值溢出');
      ++this.at;
    }
    if (this.at === start || this.text[this.at] !== 0x3a) throw new Error('调试布局缺少长度前缀');
    ++this.at;
    return value;
  }
  /** 读取按 UTF-8 字节长度前缀编码的名称。 */
  name(length) {
    if (length > this.text.length - this.at) throw new Error('调试布局名称被截断');
    const value = this.text.subarray(this.at, this.at + length).toString('utf8');
    this.at += length;
    return value;
  }
}

/**
 * 解析一份布局文本。
 * @param {string} text 编译器登记的布局文本。
 * @returns {{kind:string,name:string,offset:number,count:number,width:number,children:object[]}}
 */
function parseLayout(text) {
  const cursor = new Cursor(text);
  const parse = (depth) => {
    if (depth > MAX_DEPTH || ++cursor.nodes > MAX_NODES) throw new Error('调试布局超过深度或节点预算');
    if (cursor.at >= cursor.text.length) throw new Error('调试布局被截断');
    const kind = String.fromCharCode(cursor.text[cursor.at++]);
    if (!/^[iufbcsRSTAaNVLEMXFHID]$/.test(kind)) throw new Error('调试布局节点类别无效');
    const name = cursor.name(cursor.number());
    const offset = cursor.number(), count = cursor.number(), width = cursor.number();
    cursor.number();  // 自定义文本回调在调试路径不使用。
    const children = cursor.number();
    if (children > MAX_NODES) throw new Error('调试布局子节点超过预算');
    const node = { kind, name, offset, count, width, children: [] };
    for (let index = 0; index < children; ++index) node.children.push(parse(depth + 1));
    return node;
  };
  const layout = parse(0);
  if (cursor.at !== cursor.text.length) throw new Error('调试布局有多余字节');
  return layout;
}

/** 聚合布局需要真实内存地址才能展开。 */
function expandable(layout) {
  return Boolean(layout) && ['S', 'T', 'N', 'a', 'A', 'R', 's'].includes(layout.kind);
}

/** 按 UTF-8 解码展示文本，超长时截断并标记。 */
function quoted(bytes) {
  const value = new TextDecoder('utf-8', { fatal: false }).decode(bytes);
  return JSON.stringify(value.length > MAX_TEXT ? value.slice(0, MAX_TEXT) + '…' : value);
}

/** 读取一个标量并按布局格式化。 */
async function scalar(layout, address, memory) {
  if (![1, 2, 4, 8].includes(layout.width)) throw new Error('标量布局宽度无效');
  const bytes = await memory.read(address, layout.width);
  if (layout.kind === 'f') return String(layout.width === 4 ? bytes.readFloatLE(0) : bytes.readDoubleLE(0));
  if (layout.kind === 'b') return bytes.readUIntLE(0, layout.width) !== 0 ? 'true' : 'false';
  if (layout.kind === 'c') return JSON.stringify(String.fromCharCode(bytes[0]));
  const unsigned = layout.width === 8 ? bytes.readBigUInt64LE(0) : BigInt(bytes.readUIntLE(0, layout.width));
  if (layout.kind === 'u') return unsigned.toString();
  const sign = 1n << BigInt(layout.width * 8 - 1);
  return (unsigned & sign ? unsigned - (sign << 1n) : unsigned).toString();
}

/** 读取语言字符串：静态数据以 NUL 结尾，堆内先读内联或带缓冲头。 */
async function string(address, memory) {
  if (address === 0n) return '""';
  if (address < memory.dataSize) {
    const size = Number(memory.dataSize - address);
    const bytes = await memory.read(address, Math.min(size, MAX_TEXT));
    const end = bytes.indexOf(0);
    return quoted(end < 0 ? bytes : bytes.subarray(0, end));
  }
  const head = await memory.read(address, 8);
  const length = head.readBigInt64LE(0);
  if (length >= 0n) {
    const size = Number(length > BigInt(MAX_TEXT) ? BigInt(MAX_TEXT) : length);
    return quoted(await memory.read(address + 8n, size)) + (length > BigInt(size) ? '…' : '');
  }
  if (length !== -1n) throw new Error('字符串描述头无效');
  const header = await memory.read(address, 32);
  const lengthValue = header.readBigInt64LE(8), data = header.readBigUInt64LE(24);
  if (lengthValue < 0n || data === 0n) throw new Error('字符串描述无效');
  const size = Number(lengthValue > BigInt(MAX_TEXT) ? BigInt(MAX_TEXT) : lengthValue);
  return quoted(await memory.read(data, size)) + (lengthValue > BigInt(size) ? '…' : '');
}

/** 只生成聚合摘要，不读取全部子项。 */
async function summary(layout, address, memory) {
  if (layout.kind === 'R') return address === 0n ? 'null' : '&<' + (layout.name || '引用') + '>';
  if (layout.kind === 'a' || layout.kind === 'A') {
    let count = layout.count;
    if (layout.kind === 'a') {
      if (address < 8n) throw new Error('数组句柄缺少长度头');
      const header = await memory.read(address - 8n, 8);
      count = Number(header.readBigInt64LE(0));
      if (!Number.isSafeInteger(count) || count < 0) throw new Error('数组长度无效');
    }
    return '[' + count + ']';
  }
  if (layout.kind === 'N') {
    const tag = (await memory.read(address, 4)).readInt32LE(0);
    const variant = layout.children.find((item) => item.count === tag);
    return variant ? variant.name + '(' + (variant.children.length ? '…' : '') + ')' : '<未知变体 ' + tag + '>';
  }
  if (layout.kind === 'S' || layout.kind === 'T') return layout.kind === 'T' ? '(' + (layout.children.length ? '…' : '') + ')' : '{' + (layout.children.length ? '…' : '') + '}';
  return '<' + (layout.name || 'ChaLi 值') + '>';
}

/**
 * 解码一个布局节点。
 * @param {object} layout 解析后的布局。
 * @param {bigint} address 值所在的逻辑地址。
 * @param {{dataSize:bigint,read:Function}} memory 当前暂停的内存快照。
 * @param {{start?:number,count?:number}} [page] 子项分页；count 为零时只取摘要。
 * @returns {Promise<{text:string,children:Array}>} 展示文本与可继续展开的子项。
 */
async function decode(layout, address, memory, page = {}) {
  const start = Number.isSafeInteger(page.start) && page.start > 0 ? page.start : 0;
  const limit = Math.min(MAX_PAGE, Number.isSafeInteger(page.count) ? Math.max(0, page.count) : DEFAULT_PAGE);
  switch (layout.kind) {
    case 'i': case 'u': case 'f': case 'b': case 'c':
      return { text: await scalar(layout, address, memory), children: [] };
    case 's':
      return { text: await string(address, memory), children: [] };
    case 'R': {
      if (address === 0n) return { text: 'null', children: [] };
      if (!layout.children.length) return { text: '&<' + (layout.name || '引用') + '>', children: [] };
      const target = await decode(layout.children[0], address, memory, page);
      return { text: '&' + target.text, children: target.children };
    }
    case 'N': {
      const tag = (await memory.read(address, 4)).readInt32LE(0);
      const variant = layout.children.find((item) => item.count === tag);
      if (!variant) return { text: '<未知变体 ' + tag + '>', children: [] };
      const children = [];
      if (limit > 0) {
        const end = Math.min(variant.children.length, start + limit);
        for (let index = start; index < end; ++index) {
          const payload = variant.children[index];
          children.push(await child(payload, address + BigInt(payload.offset), payload.name, memory));
        }
      }
      const more = variant.children.length > start + limit ? '…' : '';
      return { text: variant.name + '(' + more + ')', children };
    }
    case 'S': case 'T': {
      const children = [];
      if (limit > 0) {
        const end = Math.min(layout.children.length, start + limit);
        for (let index = start; index < end; ++index) {
          const field = layout.children[index];
          children.push(await child(field, address + BigInt(field.offset), field.name, memory));
        }
      }
      const more = layout.children.length > start + limit || limit === 0 ? '…' : '';
      return { text: (layout.kind === 'T' ? '(' : '{') + more + '}', children };
    }
    case 'a': case 'A': {
      if (!layout.children.length || (layout.width !== 1 && layout.width !== 8)) throw new Error('数组布局无效');
      const count = layout.kind === 'A' ? layout.count : Number((await memory.read(address - 8n, 8)).readBigInt64LE(0));
      if (!Number.isSafeInteger(count) || count < 0) throw new Error('数组长度无效');
      const children = [];
      if (limit > 0) {
        const end = Math.min(count, start + limit);
        for (let index = start; index < end; ++index) {
          const element = address + BigInt(index) * BigInt(layout.width);
          children.push(await child(layout.children[0], element, String(index), memory));
        }
      }
      return { text: '[' + count + (limit > 0 && count > start + limit ? '…' : '') + ']', children };
    }
    default:
      return { text: '<' + (layout.name || 'ChaLi 值') + '>', children: [] };
  }
}

/** 按布局推导子项的类型拼写：字段名不能冒充类型名。 */
function typeName(layout) {
  switch (layout.kind) {
    case 'i': return 'int' + layout.width * 8;
    case 'u': return 'uint' + layout.width * 8;
    case 'f': return layout.width === 4 ? 'float32' : 'float64';
    case 'b': return 'bool';
    case 'c': return 'char';
    case 's': return 'String';
    case 'R': return layout.name || '引用';
    case 'a': case 'A': return layout.name || '数组';
    default: return layout.name || '记录';
  }
}

/** 读取并格式化一个子项，标量直接展示，聚合给出摘要并保留展开地址。 */
async function child(layout, address, name, memory) {
  try {
    if (layout.kind === 'R') {
      const pointer = (await memory.read(address, 8)).readBigUInt64LE(0);
      return { name, type: typeName(layout), value: pointer === 0n ? 'null' : '&<' + (layout.name || '引用') + '>', childLayout: pointer === 0n ? null : layout, address: pointer };
    }
    const value = layout.kind === 's' ? await string(address, memory) :
      (['i', 'u', 'f', 'b', 'c'].includes(layout.kind) ? await scalar(layout, address, memory) : await summary(layout, address, memory));
    return { name, type: typeName(layout), value, childLayout: expandable(layout) ? layout : null, address };
  } catch (error) {
    return { name, type: typeName(layout), value: '<不可读：' + error.message + '>', childLayout: null, address: 0n };
  }
}

module.exports = { parseLayout, decode, expandable, typeName, MAX_PAGE };
