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
    if (header.readBigUInt64LE(0) !== 1n || header.readBigUInt64LE(8) !== 8n) throw new Error('内存目录版本或目标表示不匹配');
    const visited = new Set(); let address = header.readBigUInt64LE(16), record;
    while (address) {
      if (visited.has(address) || visited.size >= 4096) throw new Error('内存线程目录循环或超过预算');
      visited.add(address);
      const row = await bytes(address, 96);
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
    return {
      regions, dataSize,
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
