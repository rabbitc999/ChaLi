'use strict';

/** 从目标的只读映射读取源码信息；不调用目标函数或用户代码。 */
const path = require('node:path');
const LIMIT = 4 * 1024 * 1024, POINT_LIMIT = 65536, CHUNK = 65536;

/** 校验来自目标的整数，不把精度丢失或负数用于地址／预算计算。 */
function integer(value, maximum = Number.MAX_SAFE_INTEGER) {
  if (!Number.isSafeInteger(value) || value < 0 || value > maximum) throw new Error('调试映射整数无效');
  return value;
}
/** 校验目标中的 UTF-8 名称和路径。 */
function text(value) {
  if (typeof value !== 'string' || value.length > 8192 || value.includes('\0')) throw new Error('调试映射文本无效');
  return value;
}
/** 保留真实文件身份；Windows 路径比较忽略大小写。 */
function sourcePath(value) {
  const result = path.resolve(value).replace(/\\/g, '/');
  return process.platform === 'win32' ? result.toLowerCase() : result;
}

class DebugMetadata {
  constructor(command, report) {
    this.command = command; this.report = report;
    this.cache = null; this.pending = null; this.unavailable = false;
  }
  /** 恢复运行、重启或断开时取消尚未完成的目标读取。 */
  cancel() { this.controller?.abort(); }
  /** 对调用者单独取消等待，不取消其他查询共享的只读加载。 */
  async load(signal) {
    if (signal?.aborted) throw new Error('调试映射读取已取消');
    if (this.cache || this.unavailable) return this.cache;
    if (!this.pending) {
      const controller = new AbortController(); this.controller = controller;
      const operation = this.read(controller.signal).then(value => { this.cache = value; return value; }).catch(error => {
        if (controller.signal.aborted) throw error;
        this.unavailable = true; this.report('调试源映射不可用，继续使用 DWARF：' + error.message + '\n');
        return null;
      }).finally(() => { if (this.pending === operation) { this.pending = null; this.controller = null; } });
      this.pending = operation;
    }
    const pending = this.pending;
    if (!signal) return pending;
    return new Promise((resolve, reject) => {
      const abort = () => reject(new Error('调试映射读取已取消'));
      signal.addEventListener('abort', abort, { once: true });
      pending.then(resolve, reject).finally(() => signal.removeEventListener('abort', abort));
      if (signal.aborted) abort();
    });
  }
  /** 只读取明确的内部符号或已验证的地址，不接受用户表达式。 */
  async scalar(expression, signal) {
    const result = await this.command('-data-evaluate-expression ' + JSON.stringify(expression), { signal });
    if (typeof result.value !== 'string' || !/^(?:0x[0-9a-f]+|[0-9]+)$/i.test(result.value)) throw new Error('调试映射符号地址无效');
    return BigInt(result.value);
  }
  /** 分块读取并核对连续地址与真实字节数量，不补造不可读内存。 */
  async memory(address, size, signal) {
    integer(size, LIMIT);
    if (address <= 0n || address + BigInt(size) > 0xffffffffffffffffn) throw new Error('调试映射地址越界');
    const output = Buffer.alloc(size);
    for (let offset = 0; offset < size; offset += CHUNK) {
      if (signal.aborted) throw new Error('调试映射读取已取消');
      const length = Math.min(CHUNK, size - offset), begin = address + BigInt(offset);
      const result = await this.command(`-data-read-memory-bytes 0x${begin.toString(16)} ${length}`, { signal });
      let written = 0;
      for (const row of result.memory || []) {
        if (!/^(?:[0-9a-f]{2})*$/i.test(row.contents || '') || !/^0x[0-9a-f]+$/i.test(row.begin || '')) throw new Error('调试映射内存响应无效');
        const bytes = Buffer.from(row.contents, 'hex');
        if (BigInt(row.begin) !== begin + BigInt(written) || written + bytes.length > length) throw new Error('调试映射内存不连续');
        bytes.copy(output, offset + written); written += bytes.length;
      }
      if (written !== length) throw new Error('调试映射内存被截断');
    }
    return output;
  }
  /** 验证版本、目标表示及所有引用，然后建立物理地址到源码的索引。 */
  async read(signal) {
    const size = await this.scalar('*(unsigned long long*)&chali_debug_catalog_size', signal);
    if (size < 2n || size > BigInt(LIMIT)) throw new Error('调试映射超过读取预算');
    const address = await this.scalar('(unsigned long long)&chali_debug_catalog', signal);
    const bytes = await this.memory(address, Number(size), signal);
    const catalog = JSON.parse(new TextDecoder('utf-8', { fatal: true }).decode(bytes));
    if (catalog.version !== 1 || catalog.pointerBytes !== 8 || catalog.byteOrder !== 'little'
      || !Array.isArray(catalog.functions) || !Array.isArray(catalog.positions)
      || catalog.functions.length > POINT_LIMIT || catalog.positions.length > POINT_LIMIT) throw new Error('调试映射版本、目标或条目预算不匹配');
    const macros = new Map();
    if (catalog.layouts !== undefined && (!Array.isArray(catalog.layouts) || catalog.layouts.length > POINT_LIMIT)) throw new Error('调试布局超过预算');
    for (const layout of catalog.layouts || [])
      if (typeof layout !== 'string' || layout.length > 1048576 || layout.includes('\0')) throw new Error('调试布局文本无效');
    const origin = value => { if (!value) throw new Error('宏来源缺失'); text(value.file); integer(value.line); integer(value.column); };
    if (catalog.macros !== undefined && (!Array.isArray(catalog.macros) || catalog.macros.length > POINT_LIMIT)) throw new Error('宏来源超过预算');
    for (const macro of catalog.macros || []) {
      integer(macro.id); text(macro.name); origin(macro.call); origin(macro.invocation); origin(macro.definition);
      if (macros.has(macro.id) || !Number.isSafeInteger(macro.parent) || macro.parent < -1 || macro.parent >= macro.id) throw new Error('宏来源树无效');
      macros.set(macro.id, macro);
    }
    for (const macro of macros.values()) {
      let cursor = macro, depth = 0;
      while (cursor.parent !== -1) {
        cursor = macros.get(cursor.parent);
        if (!cursor || ++depth > 256) throw new Error('宏来源父级无效');
      }
    }
    const macroId = value => { if (value !== undefined && value !== -1 && !macros.has(value)) throw new Error('宏来源引用无效'); };
    const byLinkage = new Map();
    for (const fn of catalog.functions) {
      text(fn.name); text(fn.linkage); text(fn.file); integer(fn.line); integer(fn.column);
      if (byLinkage.has(fn.linkage) || !Array.isArray(fn.scopes) || !Array.isArray(fn.variables) || fn.scopes.length > POINT_LIMIT || fn.variables.length > POINT_LIMIT) throw new Error('调试函数映射重复或超过预算');
      fn.parents = new Map([[0, 0]]); const seen = new Set();
      for (const scope of fn.scopes) {
        integer(scope.id); integer(scope.parent);
        if (seen.has(scope.id) || (scope.id && scope.parent >= scope.id) || !fn.parents.has(scope.parent)) throw new Error('调试词法树无效');
        seen.add(scope.id); fn.parents.set(scope.id, scope.parent);
      }
      for (const variable of fn.variables) {
        text(variable.name); text(variable.binding); text(variable.type); text(variable.file); integer(variable.line); integer(variable.column); integer(variable.scope);
        if (variable.storage !== undefined) text(variable.storage);
        if (variable.dwarf !== undefined) text(variable.dwarf);
        macroId(variable.macro);
        if (variable.boxed !== undefined && typeof variable.boxed !== 'boolean') throw new Error('调试绑定存储标记无效');
        if (variable.address !== undefined && typeof variable.address !== 'boolean') throw new Error('调试地址类别无效');
        if (variable.layout !== undefined && variable.layout !== -1) integer(variable.layout);
        if (!fn.parents.has(variable.scope) || typeof variable.readOnly !== 'boolean') throw new Error('调试变量来源无效');
      }
      fn.positions = []; byLinkage.set(fn.linkage, fn);
    }
    let addresses = Buffer.alloc(0);
    if (catalog.positions.length) {
      const pointAddress = await this.scalar('(unsigned long long)&chali_debug_positions', signal);
      addresses = await this.memory(pointAddress, catalog.positions.length * 8, signal);
    }
    for (let index = 0; index < catalog.positions.length; ++index) {
      const position = catalog.positions[index]; integer(position.function); integer(position.line); integer(position.column); integer(position.scope); text(position.file);
      const fn = catalog.functions[position.function];
      if (!fn || !fn.parents.has(position.scope)) throw new Error('调试位置引用无效');
      macroId(position.macro);
      if (position.definition !== undefined) origin(position.definition);
      fn.positions.push({ ...position, address: addresses.readBigUInt64LE(index * 8), ordinal: index });
    }
    for (const fn of catalog.functions) fn.positions.sort((a, b) => a.address < b.address ? -1 : a.address > b.address ? 1 : a.ordinal - b.ordinal);
    return { byLinkage, catalog, macros };
  }
  /** 用当前机器 PC 选择同函数、同文件和同源码行的位置，不猜测源文件缩进。 */
  frame(frame) {
    const fn = this.cache?.byLinkage.get(frame.func);
    if (!fn) return null;
    const result = { name: fn.name, column: Number(frame.col) || 1, function: fn };
    if (!/^0x[0-9a-f]+$/i.test(frame.addr || '') || !(frame.fullname || frame.file)) return result;
    const pc = BigInt(frame.addr), file = sourcePath(frame.fullname || frame.file);
    let low = 0, high = fn.positions.length;
    while (low < high) { const middle = (low + high) >>> 1; if (fn.positions[middle].address <= pc) low = middle + 1; else high = middle; }
    for (let at = low - 1; at >= 0; --at) {
      const position = fn.positions[at];
      if (position.line === Number(frame.line) && sourcePath(position.file) === file) {
        result.column = position.column || result.column; result.position = position;
        result.macros = [];
        let macro = this.cache.macros.get(position.macro);
        while (macro) { result.macros.push(macro); macro = this.cache.macros.get(macro.parent); }
        break;
      }
    }
    return result;
  }
}

/** 按实际 PC 的词法祖先选择绑定，不把已经退出的同名聚合误认作当前标量。 */
function visibleBindings(metadata) {
  const result = new Map(), ranks = new Map(), distances = new Map();
  if (!metadata?.position || !metadata.function?.parents) return result;
  let scope = metadata.position.scope, distance = 0;
  while (true) {
    if (distances.has(scope)) return new Map();
    distances.set(scope, distance++);
    if (scope === 0) break;
    scope = metadata.function.parents.get(scope);
    if (scope === undefined) return new Map();
  }
  for (const variable of metadata.function.variables || []) {
    const rank = distances.get(variable.scope), name = variable.dwarf || variable.name;
    if (rank !== undefined && (!ranks.has(name) || rank < ranks.get(name))) {
      result.set(name, variable); ranks.set(name, rank);
    }
  }
  return result;
}

module.exports = { DebugMetadata, LIMIT, POINT_LIMIT, visibleBindings };
