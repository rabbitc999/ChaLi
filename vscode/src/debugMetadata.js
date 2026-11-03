'use strict';
const { createHash } = require('node:crypto');

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
      const operation = this.read(controller.signal).then(value => {
        this.cache = value;
        if (value.catalog.optimized) this.report('O3 优化调试：变量可能被优化或内联，变量修改不可用；源码位置以 DWARF 为准。\n');
        return value;
      }).catch(error => {
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
  /** 从已验证包描述加载独立目录，统一限制总字节及条目，不按每个库放大预算。 */
  async read(signal) {
    const budget = { bytes: 0, points: 0 };
    const main = await this.readCatalog(signal, 'chali_debug', budget);
    const descriptors = main.catalog.libraries || [];
    if (!Array.isArray(descriptors) || descriptors.length > 128) throw new Error('库调试目录超过预算');
    const seen = new Set();
    for (const descriptor of descriptors) {
      text(descriptor.module); text(descriptor.version);
      if (!/^chali_debug_[a-f0-9]{64}$/.test(descriptor.prefix || '') || seen.has(descriptor.prefix)
        || descriptor.format !== 1 || typeof descriptor.optimized !== 'boolean'
        || !['native', 'bitcode'].includes(descriptor.implementation)
        || !/^(?:[a-f0-9]{64})?$/.test(descriptor.implementationSha256 ?? '')
        || !/^(?:[a-f0-9]{64})?$/.test(descriptor.bitcodeSha256 ?? '')) throw new Error('库调试身份无效');
      if (descriptor.prefix !== 'chali_debug_' + createHash('sha256').update(descriptor.module).digest('hex')) throw new Error('库调试模块与符号身份不符');
      const expected = descriptor.implementation === 'bitcode' ? descriptor.bitcodeSha256 : descriptor.implementationSha256;
      if (!/^[a-f0-9]{64}$/.test(expected)) throw new Error('库调试所选实现摘要缺失');
      seen.add(descriptor.prefix);
      const library = await this.readCatalog(signal, descriptor.prefix, budget);
      if (library.catalog.unit !== descriptor.module || !!library.catalog.optimized !== descriptor.optimized
        || (library.catalog.libraries || []).length) throw new Error('库符号与已验证实现身份不符');
      budget.bytes += 65;
      if (budget.bytes > LIMIT) throw new Error('库调试身份超过读取总预算');
      const identityAddress = await this.scalar(`(unsigned long long)&${descriptor.prefix}_implementation_sha256`, signal);
      const identity = await this.memory(identityAddress, 65, signal);
      if (identity[64] !== 0 || identity.subarray(0, 64).toString('ascii') !== expected) throw new Error('库符号与最终链接实现 SHA-256 不匹配；请重新编译消费者对象');
      const layoutOffset = (main.catalog.layouts || []).length;
      let macroOffset = 0;
      for (const id of main.macros.keys()) macroOffset = Math.max(macroOffset, id + 1);
      if (!Number.isSafeInteger(macroOffset)) throw new Error('跨库宏身份越界');
      const macroId = id => {
        if (id === undefined || id === -1) return id;
        const relocated = id + macroOffset;
        if (!Number.isSafeInteger(relocated)) throw new Error('跨库宏身份越界');
        return relocated;
      };
      for (const [id, macro] of library.macros) main.macros.set(macroId(id),
        { ...macro, id: macroId(id), parent: macroId(macro.parent) });
      for (const fn of library.catalog.functions) {
        if (main.byLinkage.has(fn.linkage)) throw new Error('跨库调试函数身份冲突');
        fn.library = descriptor;
        for (const variable of fn.variables) {
          if (variable.layout !== undefined && variable.layout !== -1) variable.layout += layoutOffset;
          variable.macro = macroId(variable.macro);
        }
        for (const position of fn.positions) position.macro = macroId(position.macro);
        main.byLinkage.set(fn.linkage, fn); main.catalog.functions.push(fn);
      }
      for (const [id, layout] of library.dynamicTypes) {
        const prior = main.dynamicTypes.get(id);
        if (prior !== undefined && main.catalog.layouts[prior] !== library.catalog.layouts[layout]) throw new Error('跨库动态类型身份冲突');
        main.dynamicTypes.set(id, layout + layoutOffset);
      }
      for (const [id, closure] of library.closures) {
        if (main.closures.has(id)) throw new Error('跨库闭包身份冲突');
        for (const capture of closure.captures) capture.layout += layoutOffset;
        main.closures.set(id, closure);
      }
      main.catalog.layouts = [...(main.catalog.layouts || []), ...(library.catalog.layouts || [])];
    }
    return main;
  }
  /** 验证一个编译单元的版本、目标和引用，再建立物理地址到源码的索引。 */
  async readCatalog(signal, prefix, budget) {
    const size = await this.scalar(`*(unsigned long long*)&${prefix}_catalog_size`, signal);
    if (size < 2n || size > BigInt(LIMIT)) throw new Error('调试映射超过读取预算');
    budget.bytes += Number(size);
    if (budget.bytes > LIMIT) throw new Error('调试映射总字节超过读取预算');
    const address = await this.scalar(`(unsigned long long)&${prefix}_catalog`, signal);
    const bytes = await this.memory(address, Number(size), signal);
    const catalog = JSON.parse(new TextDecoder('utf-8', { fatal: true }).decode(bytes));
    if (catalog.version !== 1 || catalog.pointerBytes !== 8 || catalog.byteOrder !== 'little'
      || !Array.isArray(catalog.functions) || !Array.isArray(catalog.positions)
      || catalog.functions.length > POINT_LIMIT || catalog.positions.length > POINT_LIMIT) throw new Error('调试映射版本、目标或条目预算不匹配');
    if (catalog.optimized !== undefined && typeof catalog.optimized !== 'boolean') throw new Error('调试优化模式无效');
    if (catalog.optimized && catalog.positions.length) throw new Error('优化调试不能依赖固定基本块地址');
    if (catalog.unit !== undefined && typeof catalog.unit !== 'string') throw new Error('调试编译单元身份无效');
    budget.points += catalog.functions.length + catalog.positions.length + (catalog.macros || []).length + (catalog.layouts || []).length;
    budget.bytes += catalog.positions.length * 8;
    if (budget.bytes > LIMIT || budget.points > POINT_LIMIT) throw new Error('跨库调试目录超过总预算');
    const macros = new Map();
    if (catalog.layouts !== undefined && (!Array.isArray(catalog.layouts) || catalog.layouts.length > POINT_LIMIT)) throw new Error('调试布局超过预算');
    for (const layout of catalog.layouts || [])
      if (typeof layout !== 'string' || layout.length > 1048576 || layout.includes('\0')) throw new Error('调试布局文本无效');
    const dynamicTypes = new Map();
    if (catalog.dynamicTypes !== undefined && (!Array.isArray(catalog.dynamicTypes) || catalog.dynamicTypes.length > POINT_LIMIT)) throw new Error('动态类型目录超过预算');
    for (const entry of catalog.dynamicTypes || []) {
      integer(entry.id); integer(entry.layout);
      if (!entry.id || dynamicTypes.has(BigInt(entry.id)) || entry.layout >= (catalog.layouts || []).length) throw new Error('动态类型布局引用无效');
      dynamicTypes.set(BigInt(entry.id), entry.layout);
    }
    const closures = new Map(); let captures = 0;
    if (catalog.closures !== undefined && (!Array.isArray(catalog.closures) || catalog.closures.length > POINT_LIMIT)) throw new Error('闭包目录超过预算');
    for (const closure of catalog.closures || []) {
      integer(closure.address); text(closure.name);
      if (!closure.address || closures.has(BigInt(closure.address)) || !Array.isArray(closure.captures)
        || (captures += closure.captures.length) > POINT_LIMIT) throw new Error('闭包目录身份或预算无效');
      for (const capture of closure.captures) {
        text(capture.name); integer(capture.layout); integer(capture.mode, 3);
        if (capture.layout >= (catalog.layouts || []).length || typeof capture.address !== 'boolean'
          || (capture.mode >= 2 && capture.address)) throw new Error('闭包捕获布局或方式无效');
      }
      closures.set(BigInt(closure.address), closure);
    }
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
      if (fn.context !== undefined && (!fn.context || typeof fn.context !== 'object'
        || Object.keys(fn.context).length !== 3
        || ['heap', 'data', 'objects'].some(key => fn.context[key] !== '__chali_context_' + key))) throw new Error('调试帧上下文描述无效');
      if (byLinkage.has(fn.linkage) || !Array.isArray(fn.scopes) || !Array.isArray(fn.variables) || fn.scopes.length > POINT_LIMIT || fn.variables.length > POINT_LIMIT) throw new Error('调试函数映射重复或超过预算');
      budget.points += fn.scopes.length + fn.variables.length;
      if (budget.points > POINT_LIMIT) throw new Error('跨库变量与作用域超过总预算');
      fn.parents = new Map([[0, 0]]); const seen = new Set();
      for (const scope of fn.scopes) {
        integer(scope.id); integer(scope.parent);
        if (seen.has(scope.id) || (scope.id && scope.parent >= scope.id) || !fn.parents.has(scope.parent)) throw new Error('调试词法树无效');
        seen.add(scope.id); fn.parents.set(scope.id, scope.parent);
      }
      for (const variable of fn.variables) {
        text(variable.name); text(variable.binding); text(variable.type); text(variable.file); integer(variable.line); integer(variable.column); integer(variable.scope);
        if (variable.storage !== undefined) text(variable.storage);
        if (variable.nativeStorage !== undefined && !['', 'i64', 'double'].includes(variable.nativeStorage)) throw new Error('调试变量原生槽表示无效');
        if (variable.storageDwarf !== undefined && variable.storageDwarf !== '') {
          if (!/^__chali_storage_[0-9]{1,6}$/.test(variable.storageDwarf) || !variable.storage
            || ![1, 8].includes(variable.storageBytes)) throw new Error('调试镜像的真实存储描述无效');
        } else if (variable.storageBytes !== undefined && variable.storageBytes !== 0) throw new Error('调试镜像缺少真实存储身份');
        if (variable.availabilityDwarf !== undefined && variable.availabilityDwarf !== '' && !/^__chali_available_[0-9]{1,6}$/.test(variable.availabilityDwarf)) throw new Error('调试绑定可用性描述无效');
        if (variable.dwarf !== undefined) text(variable.dwarf);
        macroId(variable.macro);
        if (variable.boxed !== undefined && typeof variable.boxed !== 'boolean') throw new Error('调试绑定存储标记无效');
        if (variable.address !== undefined && typeof variable.address !== 'boolean') throw new Error('调试地址类别无效');
        if (variable.layout !== undefined && variable.layout !== -1) integer(variable.layout);
        if (!fn.parents.has(variable.scope) || typeof variable.readOnly !== 'boolean') throw new Error('调试变量来源无效');
      }
      fn.optimized = catalog.optimized === true;
      fn.positions = []; byLinkage.set(fn.linkage, fn);
    }
    let addresses = Buffer.alloc(0);
    if (catalog.positions.length) {
      const pointAddress = await this.scalar(`(unsigned long long)&${prefix}_positions`, signal);
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
    return { byLinkage, catalog, macros, dynamicTypes, closures };
  }
  /** 用当前机器 PC 选择同函数、同文件和同源码行的位置，不猜测源文件缩进。 */
  frame(frame) {
    let fn = this.cache?.byLinkage.get(frame.func);
    if (!fn && this.cache) {
      const candidates = this.cache.catalog.functions.filter(item => item.optimized && item.name === frame.func);
      if (candidates.length === 1) fn = candidates[0];
    }
    if (!fn) return null;
    const result = { name: fn.name === '__chali_main_arguments' ? 'main' : fn.name,
      column: Number(frame.col) || 1, function: fn, optimized: fn.optimized === true };
    if (result.optimized) return result;
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
  if (metadata?.optimized) {
    // DWARF 负责实际可见性；名字有多个候选时不猜词法范围或聚合布局。
    const ambiguous = new Set();
    for (const variable of metadata.function?.variables || []) {
      if ((variable.macro ?? -1) !== -1) continue;
      const name = variable.dwarf || variable.name;
      if (ambiguous.has(name)) continue;
      if (result.has(name)) { result.delete(name); ambiguous.add(name); }
      else result.set(name, variable);
    }
    return result;
  }
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
