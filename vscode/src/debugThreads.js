'use strict';

/** 将 MI 线程事件与可能迟到的列表快照合并，退出墓碑防止旧响应重新显示已退出线程。 */
class DebugThreads {
  constructor(changed = () => {}) {
    this.active = new Map(); this.dead = new Set(); this.generation = 0;
    this.observed = false; this.selected = 0; this.changed = changed;
  }
  /** 解析原生协议的规范十进制线程号，不接受零、溢出或命令片段。 */
  static id(value) {
    if (typeof value === 'string' && /^[1-9][0-9]*$/.test(value)) value = Number(value);
    if (!Number.isSafeInteger(value) || value < 1) throw new Error('线程编号无效');
    return value;
  }
  /** 创建通知只发一次；显式新建通知允许真正的新线程重新使用编号。 */
  start(value, details = {}) {
    const id = DebugThreads.id(value); this.observed = true;
    if (this.active.has(id)) { this.active.set(id, { ...this.active.get(id), ...details, id }); return id; }
    if (this.active.size >= 4096) throw new Error('线程数量超过预算');
    this.dead.delete(id); this.active.set(id, { ...details, id }); ++this.generation;
    this.changed('started', id); return id;
  }
  /** 退出立即撤销当前编号，迟到快照不能恢复它；重复退出通知不重复发送事件。 */
  exit(value) {
    const id = DebugThreads.id(value); this.observed = true;
    if (this.dead.has(id)) return;
    if (this.dead.size >= 65536) throw new Error('线程历史超过预算');
    const known = this.active.delete(id); this.dead.add(id); ++this.generation;
    if (this.selected === id) this.selected = this.active.keys().next().value || 0;
    if (known) this.changed('exited', id);
  }
  /** 合并查询结果，仅在查询期间没有事件变化时以缺席判定退出。 */
  sync(rows, current, generation) {
    if (!Array.isArray(rows) || rows.length > 4096) throw new Error('线程列表超过预算或格式无效');
    const stable = generation === this.generation, seen = new Set();
    for (const row of rows) {
      const id = DebugThreads.id(row.id);
      if (seen.has(id)) throw new Error('线程列表有重复编号');
      seen.add(id);
      const name = row.name || row['target-id'] || `线程 ${id}`;
      if (typeof name !== 'string' || name.length > 4096) throw new Error('线程名称无效或超过预算');
      if (!this.dead.has(id) && (stable || !this.active.has(id))) this.start(id, { name });
    }
    this.observed = true;
    if (stable) for (const id of this.active.keys()) if (!seen.has(id)) this.exit(id);
    if (current !== undefined) {
      const id = DebugThreads.id(current);
      if (!this.active.has(this.selected) && this.active.has(id)) this.selected = id;
    }
    if (!this.active.has(this.selected)) this.selected = this.active.keys().next().value || 0;
    return [...this.active.values()].map(row => ({ id: row.id, name: row.name || `线程 ${row.id}` }));
  }
  /** DAP 请求只接受整数；未观察到线程的测试／兼容会话仍允许缺省主线程。 */
  resolve(value) {
    if (value === undefined) value = this.selected || this.active.keys().next().value || 1;
    if (typeof value !== 'number') throw new Error('线程编号必须是整数');
    const id = DebugThreads.id(value);
    if (this.dead.has(id) || (this.observed && !this.active.has(id))) throw new Error('线程已经退出或不存在：' + id);
    return id;
  }
  /** 停止通知是活动线程的直接证据；已退出编号改选仍存活线程，不撤销退出墓碑。 */
  stop(value) {
    if (value !== undefined) {
      const id = DebugThreads.id(value);
      if (!this.dead.has(id)) { this.start(id); this.selected = id; return id; }
    }
    this.selected = this.active.has(this.selected) ? this.selected : this.active.keys().next().value || (this.observed ? 0 : 1);
    return this.selected;
  }
}

module.exports = { DebugThreads };
