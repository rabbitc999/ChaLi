'use strict';

/** 按路径边界和最长前缀双向映射源码；保持调试目录和 GDB 内部身份使用原始路径。 */
const path = require('node:path');

/** 统一比较形式，不把 Linux 路径按 Windows 大小写规则折叠。 */
function normalized(value) {
  const unix = value.replace(/\\/g, '/'), network = unix.startsWith('//');
  let result = path.posix.normalize(unix);
  if (network && !result.startsWith('//')) result = '/' + result;
  return result === '/' || /^[a-z]:\/$/i.test(result) ? result : result.replace(/\/$/, '');
}

/** 路径前缀匹配必须落在完整目录边界。 */
function prefix(file, root, insensitive) {
  if (insensitive) { file = file.toLowerCase(); root = root.toLowerCase(); }
  return file === root || file.startsWith(root.endsWith('/') ? root : root + '/');
}

class DebugSourceMap {
  /** 校验映射表；本地相对目录以配置工作目录为基准。 */
  constructor(input = {}, base = process.cwd()) {
    if (!input || typeof input !== 'object' || Array.isArray(input) || Object.keys(input).length > 128) throw new Error('sourceFileMap 必须是最多 128 项的路径映射表');
    this.rules = Object.entries(input).map(([from, to]) => {
      if (![from, to].every(value => typeof value === 'string' && value.length > 0 && value.length <= 4096 && !/[\0\r\n]/.test(value))) throw new Error('源码映射路径无效');
      from = normalized(from);
      if (!from.startsWith('/') && !/^[a-z]:\//i.test(from)) throw new Error('源码映射要求绝对目标路径前缀');
      const local = path.resolve(base, to);
      return { from, local, compare: normalized(local), targetCase: /^[a-z]:\//i.test(from) || from.startsWith('//') };
    });
    const seen = new Set(), targets = new Set();
    for (const rule of this.rules) {
      const target = rule.targetCase ? rule.from.toLowerCase() : rule.from;
      if (targets.has(target)) throw new Error('目标源码映射前缀不唯一');
      targets.add(target);
      const key = process.platform === 'win32' ? rule.compare.toLowerCase() : rule.compare;
      if (seen.has(key)) throw new Error('本地源码映射前缀不唯一');
      seen.add(key);
    }
    this.forward = [...this.rules].sort((left, right) => right.from.length - left.from.length);
    this.reverse = [...this.rules].sort((left, right) => right.compare.length - left.compare.length);
  }
  /** 将目标中记录的原始路径映射到本地；未匹配时保留原始名字。 */
  toLocal(file) {
    if (typeof file !== 'string' || file.length > 16384 || /[\0\r\n]/.test(file)) throw new Error('源码路径无效或超过预算');
    const value = normalized(file);
    const rule = this.forward.find(rule => prefix(value, rule.from, rule.targetCase));
    if (!rule) return file;
    const suffix = value.slice(rule.from.length).replace(/^\//, '');
    return path.resolve(rule.local, ...suffix.split('/'));
  }
  /** 将编辑器断点反向映射给 GDB，拒绝被更具体规则覆盖的歧义路径。 */
  toTarget(file) {
    if (typeof file !== 'string' || file.length > 16384 || /[\0\r\n]/.test(file)) throw new Error('源码路径无效或超过预算');
    if (file.startsWith('<')) return file;
    const value = normalized(path.resolve(file));
    const rule = this.reverse.find(rule => prefix(value, rule.compare, process.platform === 'win32'));
    if (!rule) return file;
    const suffix = value.slice(rule.compare.length).replace(/^\//, '');
    const result = rule.from + (suffix ? (rule.from.endsWith('/') ? '' : '/') + suffix : '');
    const roundTrip = normalized(this.toLocal(result));
    if ((process.platform === 'win32' ? roundTrip.toLowerCase() : roundTrip) !== (process.platform === 'win32' ? value.toLowerCase() : value)) throw new Error('本地源码路径被更具体的映射覆盖');
    return result;
  }
}

module.exports = { DebugSourceMap };
