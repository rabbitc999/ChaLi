'use strict';

/** 只读解析源码停点发布的 C 目录，不调用目标函数或依赖 C++ 容器布局。 */
const { DebugMetadata, LIMIT } = require('./debugMetadata');
const MAXIMUM = 0xffffffffffffffffn;

/** 从 GDB 的真实线程身份取得操作系统线程号，拒绝猜测适配器线程编号。 */
function systemThread(value) {
  const match = /\bLWP\s+(\d+)\b/.exec(value || '')
    || /\bThread\s+(?:0x[0-9a-f]+|\d+)\.(0x[0-9a-f]+|\d+)(?=\s|$)/i.exec(value || '')
    || /\bprocess\s+(\d+)\b/i.exec(value || '');
  if (!match) throw new Error('无法核对目标操作系统线程身份');
  return BigInt(match[1]);
}

class DebugMemory {
  constructor(command) { this.transport = new DebugMetadata(command, () => {}); this.command = command; }
  /** 在一次有界读取中核对线程、目录状态、完整区间及映射代次。 */
  async snapshot(thread, signal = new AbortController().signal) {
    if (signal.aborted) throw new Error('内存查看已经取消');
    if (!Number.isSafeInteger(thread) || thread < 1) throw new Error('内存查看线程无效');
    let budget = LIMIT;
    const bytes = async (address, length) => {
      if (signal.aborted) throw new Error('内存查看已经取消');
      if (!Number.isSafeInteger(length) || length < 0 || length > budget) throw new Error('内存查看超过读取预算');
      budget -= length;
      return this.transport.memory(address, length, signal);
    };
    const threads = await this.command('-thread-info ' + thread, { signal });
    const selected = (threads.threads || []).find(item => Number(item.id) === thread);
    if (!selected) throw new Error('目标线程已经退出');
    if (selected.state && selected.state !== 'stopped') throw new Error('目标线程尚未暂停');
    const identity = systemThread(selected['target-id']);
    const base = await this.transport.scalar('(unsigned long long)&chali_debug_memory', signal);
    const header = await bytes(base, 24);
    const version = header.readBigUInt64LE(0);
    if (![1n, 2n, 3n, 4n].includes(version) || header.readBigUInt64LE(8) !== 8n) throw new Error('内存目录版本或目标表示不匹配');
    const visited = new Set(); let address = header.readBigUInt64LE(16), record;
    while (address) {
      if (visited.has(address) || visited.size >= 4096) throw new Error('内存线程目录循环或超过预算');
      visited.add(address);
      const row = await bytes(address, version >= 2n ? 160 : 96);
      const state = row.readBigUInt64LE(8);
      if (row.readBigUInt64LE(0) === identity && state !== 0n) { record = { address, row, state }; break; }
      address = row.readBigUInt64LE(72);
    }
    if (!record) throw new Error('当前线程尚未发布源码停点内存目录');
    const { row, state } = record;
    if (state & 1n) throw new Error('内存目录正在发布，请在源码停点重试');
    if (row.readBigUInt64LE(64) !== 1n) throw new Error('内存目录不完整或超过观察预算');
    const count = row.readBigUInt64LE(56);
    if (count > 65536n) throw new Error('内存区间数量超过预算');
    const raw = count ? await bytes(row.readBigUInt64LE(48), Number(count) * 24) : Buffer.alloc(0);
    const regions = [];
    for (let offset = 0; offset < raw.length; offset += 24) {
      const begin = raw.readBigUInt64LE(offset), size = raw.readBigUInt64LE(offset + 8), physical = raw.readBigUInt64LE(offset + 16);
      if (!begin || !size || !physical || begin + size > MAXIMUM || physical + size > MAXIMUM
        || (regions.length && regions[regions.length - 1].end > begin)) throw new Error('内存区间无效、重叠或乱序');
      regions.push({ begin, end: begin + size, physical });
    }
    const stamp = row.readBigUInt64LE(16), revision = row.readBigUInt64LE(24);
    const checkState = async () => {
      if ((await bytes(record.address + 8n, 8)).readBigUInt64LE() !== state) throw new Error('内存目录暂停状态已经变化');
      if ((await bytes(row.readBigUInt64LE(80), 8)).readBigUInt64LE() !== row.readBigUInt64LE(88)) throw new Error('原生执行上下文已经变化，请执行到源码停点');
    };
    await checkState();
    const data = row.readBigUInt64LE(32), dataSize = row.readBigUInt64LE(40);
    if ((dataSize && !data) || data + dataSize > MAXIMUM) throw new Error('静态数据区描述无效');
    if (regions.length && regions[0].begin < dataSize) throw new Error('堆区间与静态数据区重叠');
    let objectTables;
    const checkObjects = async () => {
      await checkState();
      if (!row.readBigUInt64LE(152) || !row.readBigUInt64LE(144)
        || (await bytes(row.readBigUInt64LE(144), 8)).readBigUInt64LE() !== row.readBigUInt64LE(152)) throw new Error('对象快照已经失效');
    };
    const objects = async handle => {
      if (version < 2n) throw new Error('目标未发布对象快照');
      await checkObjects();
      if (!objectTables) objectTables = (async () => {
        const objectCount = row.readBigUInt64LE(104), valueCount = row.readBigUInt64LE(120), textSize = row.readBigUInt64LE(136);
        if (objectCount > 4096n || valueCount > 32768n || textSize > 2097152n
          || objectCount * 32n + valueCount * 48n + textSize > 2097152n) throw new Error('对象快照超过预算');
        const load = async (pointer, size) => {
          if (size && (!pointer || pointer + size > MAXIMUM)) throw new Error('对象快照物理范围无效');
          return size ? bytes(pointer, Number(size)) : Buffer.alloc(0);
        };
        const rawObjects = await load(row.readBigUInt64LE(96), objectCount * 32n);
        const rawValues = await load(row.readBigUInt64LE(112), valueCount * 48n);
        const text = await load(row.readBigUInt64LE(128), textSize);
        const values = [];
        for (let at = 0; at < rawValues.length; at += 48) {
          const flags = rawValues.readBigUInt64LE(at), first = rawValues.readBigUInt64LE(at + 16), count = rawValues.readBigUInt64LE(at + 24);
          const begin = rawValues.readBigUInt64LE(at + 32), length = rawValues.readBigUInt64LE(at + 40);
          if (!(version >= 4n ? [0n, 1n, 2n, 4n, 8n] : [0n, 1n, 2n, 4n]).includes(flags)
            || (flags === 8n && (first || count || begin || length)) || first + count > valueCount || begin + length > textSize
            || ((flags & 1n) && count && first <= BigInt(values.length))) throw new Error('对象值快照范围或递归无效');
          values.push({ flags: Number(flags), bits: rawValues.readBigUInt64LE(at + 8), first: Number(first), count: Number(count),
            text: text.subarray(Number(begin), Number(begin + length)) });
        }
        const records = new Map();
        for (let at = 0; at < rawObjects.length; at += 32) {
          const id = rawObjects.readBigUInt64LE(at), rawKind = rawObjects.readBigUInt64LE(at + 8);
          if (rawKind > 127n) throw new Error('对象种类编码无效');
          const kind = String.fromCharCode(Number(rawKind));
          const count = rawObjects.readBigUInt64LE(at + 16), first = rawObjects.readBigUInt64LE(at + 24);
          if (!id || records.has(id) || !(version >= 4n ? ['L', 'M', 'E', 'O', 'F'] : version >= 3n ? ['L', 'M', 'E', 'O'] : ['L', 'M', 'E']).includes(kind)
            || (kind === 'O' && count !== 2n) || (kind === 'F' && count < 1n)
            || first + count * (kind === 'M' ? 2n : 1n) > valueCount) throw new Error('对象快照身份或范围无效');
          records.set(id, { kind, count: Number(count), first: Number(first), values });
        }
        await checkObjects();
        return records;
      })();
      const records = await objectTables;
      await checkObjects();
      const object = records.get(handle);
      if (!object) throw new Error('对象已经释放、未同步或超过快照预算');
      return object;
    };
    return {
      regions, dataSize, object: objects, validateObjects: checkObjects,
      /** 为基础值修改核对一个完整存活区间，只返回真实地址，不在此执行写入。 */
      address: async (logical, length) => {
        if (typeof logical !== 'bigint' || logical <= 0n || logical > MAXIMUM || ![1, 8].includes(length)) throw new Error('真实存储地址或宽度无效');
        await checkState();
        if (logical < dataSize && BigInt(length) <= dataSize - logical) {
          await checkState(); return data + logical;
        }
        if (!stamp || (await bytes(revision, 8)).readBigUInt64LE() !== stamp) throw new Error('真实存储的堆映射已经失效');
        let low = 0, high = regions.length;
        while (low < high) { const middle = (low + high) >>> 1; if (regions[middle].begin <= logical) low = middle + 1; else high = middle; }
        const region = regions[low - 1];
        if (!region || logical >= region.end || BigInt(length) > region.end - logical) throw new Error('真实存储越界或已经释放');
        await checkState();
        if ((await bytes(revision, 8)).readBigUInt64LE() !== stamp) throw new Error('定位期间真实存储的堆映射已经变化');
        return region.physical + logical - region.begin;
      },
      read: async (logical, length) => {
        if (typeof logical !== 'bigint' || logical <= 0n || logical > MAXIMUM || !Number.isSafeInteger(length) || length < 0 || length > LIMIT) throw new Error('逻辑地址或读取长度无效');
        await checkState();
        if (logical < dataSize && BigInt(length) <= dataSize - logical) {
          const value = await bytes(data + logical, length); await checkState(); return value;
        }
        if (!stamp || (await bytes(revision, 8)).readBigUInt64LE() !== stamp) throw new Error('堆映射已经失效，请执行到下一个源码停点');
        let low = 0, high = regions.length;
        while (low < high) { const middle = (low + high) >>> 1; if (regions[middle].begin <= logical) low = middle + 1; else high = middle; }
        const region = regions[low - 1];
        if (!region || logical >= region.end || BigInt(length) > region.end - logical) throw new Error('逻辑区间无效、越界或已经释放');
        const value = await bytes(region.physical + logical - region.begin, length);
        await checkState();
        if ((await bytes(revision, 8)).readBigUInt64LE() !== stamp) throw new Error('读取期间堆映射已经失效');
        return value;
      },
    };
  }
}

module.exports = { DebugMemory, systemThread };
