'use strict';

/** ChaLi 原生 DAP：编译带 DWARF 的程序，通过 GDB/MI 调试真实机器码。 */
const fs = require('node:fs');
const path = require('node:path');
const { spawn } = require('node:child_process');
const { EventEmitter } = require('node:events');
const { constants: osConstants } = require('node:os');
const { RequestReader, stopProcessTree, BODY_LIMIT } = require('./debugProtocol');
const { debugEnvironment, buildDiagnostics } = require('./debugConfiguration');
const { DebugTerminal } = require('./debugTerminal');
const { DebugMetadata, visibleBindings, LIMIT: MEMORY_LIMIT } = require('./debugMetadata');
const { DebugMemory } = require('./debugMemory');
const { parseLayout, decode, expandable, typeName } = require('./debugValue');
const { parseExpression, evaluateExpression } = require('./debugExpression');
const { encodeAssignment } = require('./debugAssignment');
const { DebugThreads } = require('./debugThreads');
const { interruptWindows } = require('./debugInterrupt');
const { DebugSourceMap } = require('./debugSourceMap');
const { processId, prepareLocalIdentity } = require('./debugIdentity');
const { remoteEndpoint, remoteImage, verifyRemoteImage } = require('./debugRemote');

/** 识别内部源码标记，包括 GDB 给合成 ABI 文件补上编译目录后的形式。 */
function internalSource(file) {
  return !file || file.startsWith('<') || ['<native-library-entry>', '<native-debug-context>'].includes(path.posix.basename(file.replace(/\\/g, '/')));
}

/** 解析 GDB/MI 的字符串、元组和结果列表，保留重复键列表。 */
function parseMi(text) {
  let at = 0;
  function string() {
    at++;
    const bytes = [];
    while (at < text.length) {
      const c = String.fromCodePoint(text.codePointAt(at));
      at += c.length;
      if (c === '"') return Buffer.from(bytes).toString('utf8');
      if (c !== '\\') { bytes.push(...Buffer.from(c)); continue; }
      const escaped = text[at++];
      if (/[0-7]/.test(escaped)) {
        let digits = escaped;
        while (digits.length < 3 && /[0-7]/.test(text[at] || 'x')) digits += text[at++];
        bytes.push(parseInt(digits, 8));
      } else bytes.push(...Buffer.from(({ n: '\n', r: '\r', t: '\t', b: '\b', f: '\f' })[escaped] || escaped));
    }
    throw new Error('GDB/MI 字符串未结束');
  }
  function item() {
    if (text[at] === '"') return string();
    if (text[at] === '{' || text[at] === '[') {
      const array = text[at++] === '[';
      const close = array ? ']' : '}';
      const result = array ? [] : {};
      while (at < text.length && text[at] !== close) {
        const start = at;
        while (/[A-Za-z0-9_-]/.test(text[at] || '!')) at++;
        let key;
        if (text[at] === '=') { key = text.slice(start, at++); } else at = start;
        const value = item();
        if (array) result.push(key ? { [key]: value } : value);
        else if (key !== undefined) result[key] = value;
        else (result.$values ||= []).push(value);
        if (text[at] !== ',') break;
        at++;
      }
      if (text[at] !== close) throw new Error('GDB/MI 容器未结束');
      at++;
      return result;
    }
    const start = at;
    while (at < text.length && !',]}'.includes(text[at])) at++;
    return text.slice(start, at);
  }
  const result = item();
  if (at !== text.length) throw new Error('GDB/MI 记录存在多余内容');
  return result;
}

/** 为一次调试会话管理 MI 请求、异步停止和目标输出。 */
class Gdb extends EventEmitter {
  constructor(executable, cwd, env, options = {}) {
    super();
    this.pending = new Map();
    this.serial = 1;
    this.buffer = '';
    this.process = (options.spawn || spawn)(executable, ['--quiet', '--nx', '--interpreter=mi2'], { cwd, env, windowsHide: true, detached: process.platform !== 'win32' });
    this.process.stdin.on('error', (error) => this.fault(error));
    this.process.stdout.setEncoding('utf8');
    this.process.stdout.on('data', (chunk) => {
      this.buffer += chunk;
      let end;
      while ((end = this.buffer.indexOf('\n')) >= 0) {
        const line = this.buffer.slice(0, end).replace(/\r$/, '');
        this.buffer = this.buffer.slice(end + 1);
        try {
          if (Buffer.byteLength(line) > BODY_LIMIT) throw new Error('GDB/MI 记录超过限制');
          this.line(line);
        } catch (error) { this.fault(error); }
      }
      if (Buffer.byteLength(this.buffer) > BODY_LIMIT) { this.buffer = ''; this.fault(new Error('GDB/MI 记录超过限制')); }
    });
    this.process.stderr.on('data', (chunk) => this.emit('output', chunk.toString(), 'stderr'));
    this.process.on('error', (error) => { this.closed = true; this.fail(new Error(`无法启动原生调试器 ${executable}：${error.message}`)); this.emit('exit'); });
    this.process.on('exit', () => { this.closed = true; this.fail(new Error('GDB 已退出')); this.emit('exit'); });
  }
  fail(error) {
    for (const entry of this.pending.values()) { clearTimeout(entry.timer); entry.reject(error); }
    this.pending.clear();
  }
  /** 协议或管道故障终结等待，并明确报告进程树清理失败。 */
  fault(error) {
    this.fail(error);
    this.emit('fault', error);
    void stopProcessTree(this.process).catch((failure) => this.emit('output', failure.message + '\n', 'stderr'));
  }
  line(line) {
    if (!line || line.startsWith('(gdb)')) return;
    const record = /^(\d*)([\^*+=~@&])(.*)$/.exec(line);
    if (!record) { this.emit('output', line + '\n', 'stdout'); return; }
    const [, token, kind, body] = record;
    if ('~@&'.includes(kind)) { this.emit('output', parseMi(body), kind === '@' ? 'stdout' : 'console'); return; }
    const comma = body.indexOf(',');
    const result = comma < 0 ? {} : parseMi('{' + body.slice(comma + 1) + '}');
    const status = comma < 0 ? body : body.slice(0, comma);
    if (kind === '^') {
      const entry = this.pending.get(Number(token));
      if (entry) {
        this.pending.delete(Number(token)); clearTimeout(entry.timer);
        if (status === 'error') entry.reject(Object.assign(new Error(result.msg || 'GDB 请求失败'), { gdbError: true }));
        else entry.resolve(result);
      }
    } else if (kind === '*' && status === 'stopped') this.emit('stopped', result);
    else if (kind === '*' && status === 'running') this.emit('running', result);
    else if (kind === '=') this.emit('notify', status, result);
  }
  command(command, options = {}) {
    if (this.closed) return Promise.reject(new Error('GDB 已退出'));
    if (this.pending.size >= 256) return Promise.reject(new Error('GDB 并发请求超过限制'));
    if (options.signal && options.signal.aborted) return Promise.reject(new Error('调试请求已取消'));
    return new Promise((resolve, reject) => {
      const token = this.serial++;
      const finish = (callback) => (value) => {
        clearTimeout(timer);
        this.pending.delete(token);
        if (options.signal) options.signal.removeEventListener('abort', abort);
        callback(value);
      };
      const abort = () => finish(reject)(new Error('调试请求已取消'));
      const timer = setTimeout(() => finish(reject)(new Error('GDB 请求超时：' + command)), options.timeoutMs || 20000);
      this.pending.set(token, { resolve: finish(resolve), reject: finish(reject), timer });
      if (options.signal) options.signal.addEventListener('abort', abort, { once: true });
      this.process.stdin.write(token + command + '\n');
    });
  }
}

/** 查找工程临时目录，所有编译及调试子进程继承相同临时路径。 */
function temporaryRoot(cwd) {
  let current = path.resolve(cwd);
  while (true) {
    if (fs.existsSync(path.join(current, 'source', 'chali.gram'))) return path.join(current, 'tmp');
    const parent = path.dirname(current);
    if (parent === current) return path.join(path.resolve(cwd), 'tmp');
    current = parent;
  }
}

/** 将 DAP 请求映射到原生调试器操作，不运行 VM 或字节码。 */
class Adapter {
  constructor(compiler, input = process.stdin, output = process.stdout) {
    this.compiler = compiler;
    this.output = output;
    this.sequence = 1;
    this.handles = new Map();
    this.frames = new Map();
    this.breakpoints = new Map();
    this.functionBreakpoints = [];
    this.stepCommand = null;      ///< 最近一次单步命令，用于跳过内部代码。
    this.stepThread = null;
    this.stopSerial = 0;
    this.pauseRequested = false;
    this.pauseThread = null;
    this.inferiorPid = null;
    this.sourceMap = new DebugSourceMap();
    this.stepAttempts = 0;        ///< 连续跳过的内部停点计数。
    this.suppressContinued = false;  ///< 过滤内部停点时不为每次恢复上报 continued。
    this.panicPoint = null;          ///< 隐藏的 panic 入口断点编号。
    this.runtimeFailures = true;     ///< 是否在运行期失败入口暂停，重启时保留选择。
    this.breakpointInfo = new Map();
    this.logPoints = new Map();
    this.nextHandle = 1;
    this.terminated = false;
    this.state = 'created';
    this.epoch = 0;
    this.requests = new Map();
    this.control = Promise.resolve();
    this.progressRequests = new Map();
    this.nextProgress = 1;
    this.resetThreads();
    this.reader = new RequestReader((request) => this.dispatch(request), (error) => {
      this.event('output', { category: 'stderr', output: error.message + '\n' });
      this.end();
    });
    input.on('data', (chunk) => this.reader.push(chunk));
    input.on('end', () => { this.reader.finish(); this.end(); });
    input.on('error', () => this.end());
    output.on('error', () => this.end());
  }
  send(value) {
    const data = Buffer.from(JSON.stringify({ seq: this.sequence++, ...value }));
    this.output.write(`Content-Length: ${data.length}\r\n\r\n`); this.output.write(data);
  }
  event(event, body = {}) { this.send({ type: 'event', event, body }); }
  end() {
    if (this.terminated) return;
    this.terminated = true;
    this.state = 'ended';
    for (const task of this.requests.values()) task.abort();
    void this.shutdown().catch((error) => this.event('output', { category: 'stderr', output: error.message + '\n' }))
      .finally(() => this.event('terminated'));
  }
  /** 控制请求顺序执行，查询可并行；停止和取消不等待正在进行的构建。 */
  dispatch(request) {
    if (this.requests.has(request.seq)) throw new Error('DAP 请求编号重复');
    if (this.requests.size >= 256) throw new Error('DAP 并发请求超过限制');
    const controller = new AbortController();
    if (request.command === 'restart' && !this.attached) {
      this.restartPending = true;
      for (const task of this.requests.values()) task.abort();
    }
    this.requests.set(request.seq, controller);
    const run = () => this.request(request, controller).finally(() => this.requests.delete(request.seq));
    const immediate = ['cancel', 'disconnect', 'terminate', 'chali/input'];
    const reads = ['threads', 'stackTrace', 'scopes', 'variables', 'evaluate', 'readMemory'];
    if (immediate.includes(request.command) || reads.includes(request.command)) void run();
    else this.control = this.control.then(run, run);
  }
  /** 暂停状态变化后废弃全部帧及变量句柄。 */
  invalidate() {
    this.metadata?.cancel();
    this.epoch++;
    this.frames.clear();
    this.handles.clear();
    this.valueMemories?.clear();
    this.layoutCache?.clear();
  }
  /** 基础值修改后废弃读取与聚合快照，保留仍处于相同暂停位置的源码帧和局部作用域。 */
  invalidateValues() {
    this.epoch++;
    this.metadata?.cancel();
    for (const [id, value] of this.handles) if (['value', 'memory'].includes(value.kind)) this.handles.delete(id);
    this.valueMemories?.clear();
  }
  /** 新原生会话清空线程身份，退出事件立即使已有帧和读取失效。 */
  resetThreads() {
    if (this.threads) for (const id of [...this.threads.active.keys()]) this.threads.exit(id);
    this.threads = new DebugThreads((reason, threadId) => {
      if (reason === 'exited') {
        this.invalidate();
        if (this.stepThread === threadId) { this.stepThread = null; this.stepCommand = null; this.suppressContinued = false; }
        if (this.invalidatedEvents && this.state === 'stopped') this.event('invalidated', { areas: ['stacks', 'variables'] });
      }
      this.event('thread', { reason, threadId });
    });
  }
  /** 停止线程已退出时重新核对活动列表，避免用迟到的帧继续单步或伪造线程编号。 */
  async recoverThreadStop(gdb, data) {
    const serial = this.stopSerial, generation = this.threads.generation;
    this.invalidate(); this.state = 'stopped';
    try {
      const result = await gdb.command('-thread-info');
      if (this.gdb !== gdb || this.terminated || serial !== this.stopSerial) return;
      this.threads.sync(result.threads || [], result['current-thread-id'], generation);
      const threadId = this.threads.selected;
      if (!threadId) throw new Error('停止线程已退出，目标没有可用的活动线程');
      this.event('stopped', { reason: data.pause ? 'pause' : data.reason === 'signal-received' ? 'exception' : 'step', threadId,
        allThreadsStopped: true, description: '原停止线程已退出，已选择活动线程' });
    } catch (error) {
      if (this.gdb !== gdb || this.terminated || serial !== this.stopSerial) return;
      this.event('output', { category: 'stderr', output: error.message + '\n' }); this.end();
    }
  }
  /**
   * @brief 解析并缓存调试目录里的版本化布局；缺失或损坏只影响结构化展示。
   * @param index 变量登记的非负布局索引。
   * @returns {object|null} 解析后的布局。
   */
  layoutAt(index) {
    if (!Number.isSafeInteger(index) || index < 0) return null;
    if (!this.layoutCache) this.layoutCache = new Map();
    if (this.layoutCache.has(index)) return this.layoutCache.get(index);
    const raw = this.metadata?.cache?.catalog?.layouts?.[index];
    let parsed = null;
    try { parsed = typeof raw === 'string' ? parseLayout(raw) : null; } catch { parsed = null; }
    this.layoutCache.set(index, parsed);
    return parsed;
  }
  /**
   * @brief 取得本暂停代次共享的只读内存快照，供结构化变量展开使用。
   * @param thread 目标线程。
   * @param signal 取消信号。
   * @returns {Promise<object>} 目录快照。
   */
  async memoryFor(thread, signal) {
    if (!this.valueMemories) this.valueMemories = new Map();
    if (this.valueMemories.has(thread)) return this.valueMemories.get(thread);
    const memory = new DebugMemory(async (text, options) => {
      this.stopped();
      const result = await this.gdb.command(text, options);
      this.stopped();
      return result;
    });
    const pending = memory.snapshot(thread, signal).then(snapshot => {
      snapshot.dynamicLayout = type => this.layoutAt(this.metadata?.cache?.dynamicTypes?.get(type));
      snapshot.closureInfo = address => this.metadata?.cache?.closures?.get(address);
      snapshot.layout = index => this.layoutAt(index);
      snapshot.referenceLayout = name => {
        let found, encoded;
        for (let index = 0; index < (this.metadata?.cache?.catalog?.layouts || []).length; ++index) {
          const layout = this.layoutAt(index);
          if (!layout || !['S', 'T', 'N'].includes(layout.kind) || layout.name !== name) continue;
          const text = this.metadata.cache.catalog.layouts[index];
          if (found && text !== encoded) throw new Error('引用目标类型布局存在歧义：' + name);
          found = layout; encoded = text;
        }
        return found;
      };
      return snapshot;
    });
    this.valueMemories.set(thread, pending);
    try { return await pending; }
    catch (error) {
      if (this.valueMemories.get(thread) === pending) this.valueMemories.delete(thread);
      throw error;
    }
  }
  /** 只在活动源码栈顶定位基础镜像的真实存储，避免把调用方的逻辑地址用于另一个原生上下文。 */
  async mirrorStorage(frame, binding, signal) {
    if (frame.level !== 0 || frame.metadata?.optimized) throw new Error('真实绑定镜像仅支持栈顶开发源码帧');
    if (!/^__chali_storage_[0-9]{1,6}$/.test(binding.storageDwarf || '') || ![1, 8].includes(binding.storageBytes)) throw new Error('缺少真实存储描述');
    const expression = '(unsigned long long)(' + binding.storageDwarf + ')';
    const result = await this.gdb.command(`-data-evaluate-expression --thread ${frame.thread} --frame ${frame.level} ` + JSON.stringify(expression), { signal });
    if (!/^(?:0x[0-9a-f]+|[0-9]+)$/i.test(result.value || '') || BigInt(result.value) === 0n) throw new Error('变量没有有效的真实存储地址');
    return { logical: BigInt(result.value), memory: await this.memoryForFrame(frame, signal) };
  }
  /** 核对源码帧保存的内存身份，禁止将其他原生上下文的逻辑地址用于活动快照。 */
  async memoryForFrame(frame, signal, rows) {
    const memory = await this.memoryFor(frame.thread, signal);
    const descriptor = frame.metadata?.function?.context;
    if (!descriptor || !memory.context) {
      if (frame.level !== 0) throw new Error('该构件没有可核对的调用帧内存身份');
      return memory;
    }
    if (!rows) rows = (await this.gdb.command(`-stack-list-variables --thread ${frame.thread} --frame ${frame.level} --simple-values`, { signal })).variables || [];
    for (const key of ['heap', 'data', 'objects']) {
      const matches = rows.filter(row => row.name === descriptor[key]);
      if (matches.length !== 1 || !/^(?:0x[0-9a-f]+|[0-9]+)$/i.test(matches[0].value || '')
        || BigInt(matches[0].value) !== memory.context[key]) throw new Error('源码帧不属于活动原生上下文，或帧身份已优化不可读');
    }
    return memory;
  }
  /** 保证查看请求属于当前暂停代次。 */
  stopped(epoch = this.epoch) {
    if (this.state !== 'stopped' || epoch !== this.epoch) throw new Error('程序没有暂停，或调试引用已经过期');
    if (this.attached && !this.identityVerified) throw new Error('附加目标身份尚未核对');
  }
  /** 对已绑定的本机目标请求中断；附加会话使用核对过的创建身份，而非父子进程关系。 */
  async interruptTarget(signal) {
    let interrupt;
    try {
      if (process.platform === 'win32' && !this.remote) await interruptWindows(this.inferiorPid, this.gdb.process.pid,
        this.attachedIdentity?.image || this.debugExecutable, this.debugEnvironment, signal,
        child => { interrupt = child; this.interruptProcess = child; }, this.attachedIdentity?.start || null);
      else await this.gdb.command('-exec-interrupt', { signal });
    } finally { if (this.interruptProcess === interrupt) this.interruptProcess = null; }
  }
  /** 在断开前确认真实全停；尚在附加中的目标等待 GDB 自己完成停止，避免重复中断。 */
  async stopForClose(gdb, initiate) {
    const controller = new AbortController(); let timer;
    let stopped, exited;
    const waiting = new Promise((resolve, reject) => {
      stopped = () => { this.closeStopped = true; resolve(); };
      exited = () => reject(new Error('调试器退出，无法确认目标的断开状态'));
      gdb.once('stopped', stopped); gdb.once('exit', exited);
      timer = setTimeout(() => reject(new Error('无法在断开前确认目标暂停')), 10000);
    });
    try { await Promise.all([waiting, initiate ? this.interruptTarget(controller.signal) : Promise.resolve()]); }
    finally { clearTimeout(timer); gdb.off('stopped', stopped); gdb.off('exit', exited); controller.abort(); }
  }
  /** 回收本会话的构建进程和 GDB 进程树，重复调用共享同一次清理。 */
  async shutdown(options) {
    const explicit = options && Object.hasOwn(options, 'keepTarget');
    const keepTarget = explicit ? options.keepTarget : this.attached === true;
    if (this.shutdownPromise) {
      if (explicit && this.closingMode !== (keepTarget ? 'detach' : 'terminate')) throw new Error('已有不同的关闭操作正在执行');
      return this.shutdownPromise;
    }
    if (keepTarget && this.terminal) throw new Error('集成终端会话不能保留目标；请在外部启动后附加');
    if (keepTarget && !this.attached && this.sourceBuilt) throw new Error('临时源码启动产物不能在断开后保留；请启动已有产物或从外部附加');
    const previousState = this.state;
    this.closeStopped = previousState === 'stopped' || Boolean(this.attachPending && this.pendingAttachStop) || (this.attached && previousState === 'configuring');
    this.closingMode = keepTarget ? 'detach' : 'terminate';
    this.closingTarget = keepTarget || this.attached;
    if (!this.terminated) this.state = 'ending';
    this.invalidate();
    ++this.stopSerial;
    const gdb = this.gdb;
    this.metadata?.cancel();
    const compiler = this.compilerProcess;
    const interrupt = this.interruptProcess;
    this.interruptProcess = null;
    const terminal = this.terminal;
    this.compilerProcess = null;
    this.shutdownPromise = (async () => {
      try {
        await stopProcessTree(interrupt);
        await stopProcessTree(this.identityProcess);
        await this.identityLease?.release(); this.identityLease = null;
        if (gdb) {
          gdb.fail(new Error('调试会话已关闭'));
          if (this.closingTarget && (this.attachCommandSent || (this.inferiorPid !== null && this.inferiorPid !== undefined)) && !this.targetDetached) {
            if (!this.closeStopped) {
              try { await this.stopForClose(gdb, !this.attachPending); }
              catch (error) { if (this.inferiorPid !== null) throw error; }
            }
            if (this.inferiorPid !== null) {
              await gdb.command(keepTarget ? '-target-detach' : '-interpreter-exec console "kill"', { timeoutMs: 5000 });
              if (keepTarget) this.targetDetached = true;
            }
          }
          gdb.fail(new Error('调试会话已关闭'));
          await gdb.command('-gdb-exit', { timeoutMs: 1000 }).catch(() => {});
          if (keepTarget) {
            const child = gdb.process;
            if (child?.exitCode === null && child?.signalCode === null) await new Promise((resolve, reject) => {
              const timer = setTimeout(() => { child.off('exit', done); reject(new Error('目标已保留，但调试器未退出；请重试断开，禁止结束目标进程树')); }, 2000);
              const done = () => { clearTimeout(timer); resolve(); }; child.once('exit', done);
            });
          } else await stopProcessTree(gdb.process);
        }
        this.gdb = null; this.metadata = null; this.terminal = null;
        await stopProcessTree(compiler);
      } catch (error) {
        if (!this.terminated) {
          this.state = this.targetDetached ? 'detached' : this.closeStopped ? 'stopped' : previousState;
          if (this.state === 'stopped' && this.threads.active.size) this.event('stopped', { reason: 'pause', threadId: this.threads.selected || this.threads.active.keys().next().value,
            allThreadsStopped: true, description: '关闭未完成，目标保持暂停' });
        }
        throw error;
      } finally {
        if (!this.gdb) { await terminal?.drain(); terminal?.close(); }
      }
    })();
    try { await this.shutdownPromise; } finally { this.shutdownPromise = null; this.closingTarget = false; this.closingMode = null; }
  }
  handle(value) { const id = this.nextHandle++; this.handles.set(id, value); return id; }
  async launch(config, signal, restart = false, requestId) {
    if (signal.aborted) throw new Error('调试请求已取消');
    const attaching = config.request === 'attach';
    if (config.debugMode !== undefined && !['development', 'optimized'].includes(config.debugMode)) throw new Error('debugMode 必须为 development 或 optimized');
    const endpoint = remoteEndpoint(config.remote);
    if (endpoint && !attaching) throw new Error('remote 只用于附加已有 Linux gdbserver');
    const attachPid = attaching ? processId(config.processId) : null;
    if (attaching && (!config.executable || typeof config.executable !== 'string' || config.program)) throw new Error('附加需要已有 executable，不执行 program 构建');
    if (attaching && ((config.args || []).length || config.console === 'integratedTerminal' || config.stopOnEntry)) throw new Error('附加不修改目标参数、不接管集成终端；入口暂停使用 stopOnAttach');
    if (config.stopOnAttach !== undefined && typeof config.stopOnAttach !== 'boolean') throw new Error('stopOnAttach 必须是布尔值');
    if ((!config.program || typeof config.program !== 'string') && (!config.executable || typeof config.executable !== 'string')) throw new Error('缺少调试入口 program 或 executable');
    if (config.args !== undefined && (!Array.isArray(config.args) || config.args.length > 65536 || config.args.some(value => typeof value !== 'string' || value.includes('\0')))) throw new Error('args 必须是无 NUL 的字符串数组，且数量不得超过限制');
    this.state = 'building';
    this.configuration = { ...config };
    this.terminated = false;
    this.attached = attaching; this.identityVerified = !attaching; this.attachPending = attaching;
    this.remote = endpoint;
    this.panicArgument = process.arch === 'arm64' ? '$x0' : process.platform === 'win32' ? '$rcx' : '$rdi';
    this.attachCommandSent = false;
    this.pendingAttachStop = null; this.targetDetached = false; this.attachedIdentity = null;
    this.sourceBuilt = !config.executable;
    const cwd = path.resolve(config.cwd || path.dirname(config.executable || config.program));
    this.sourceMap = new DebugSourceMap(config.sourceFileMap, cwd);
    const root = temporaryRoot(cwd);
    fs.mkdirSync(root, { recursive: true });
    this.directory = fs.mkdtempSync(path.join(root, 'native-debug-'));
    const env = debugEnvironment(debugEnvironment(process.env, config.env), { TMP: root, TEMP: root, TMPDIR: root });
    for (const name of Object.keys(env)) {
      const key = process.platform === 'win32' ? name.toUpperCase() : name;
      if (key === 'CHALI_DEBUG_CONSOLE' || key === 'CHALI_DEBUG_ARGUMENTS') delete env[name];
    }
    if (process.platform === 'win32' && Array.isArray(config.args)) {
      if (config.args.length > 65536 || config.args.some(value => typeof value !== 'string' || value.includes('\0'))) throw new Error('args 参数数量或内容无效');
      const chunks = [Buffer.alloc(4)]; chunks[0].writeUInt32LE(config.args.length);
      let bytes = 4;
      for (const value of config.args) {
        const data = Buffer.from(value, 'utf8'), header = Buffer.alloc(4);
        header.writeUInt32LE(data.length); bytes += 4 + data.length;
        if (bytes > BODY_LIMIT) throw new Error('调试参数超过限制');
        chunks.push(header, data);
      }
      env.CHALI_DEBUG_ARGUMENTS = path.join(this.directory, 'arguments.bin');
      fs.writeFileSync(env.CHALI_DEBUG_ARGUMENTS, Buffer.concat(chunks));
    }
    if (config.console && !['internalConsole', 'integratedTerminal'].includes(config.console)) throw new Error('console 必须是 internalConsole 或 integratedTerminal');
    const prefix = path.join(this.directory, 'program');
    let executable = config.executable && path.resolve(cwd, config.executable);
    if (executable && !fs.statSync(executable).isFile()) throw new Error('executable 不是普通文件');
    if (endpoint) {
      this.remoteIdentity = await remoteImage(executable, signal);
      this.panicArgument = this.remoteIdentity.argument;
    } else if (attaching) {
      this.identityLease = await prepareLocalIdentity(attachPid, executable, env, signal, child => { this.identityProcess = child; });
      const identity = this.identityLease;
      this.attachedIdentity = { pid: identity.pid, image: identity.image, start: identity.start, sha256: identity.sha256 };
    }
    const progressId = 'build-' + this.nextProgress++;
    const diagnosticChunks = [];
    let diagnosticBytes = 0;
    const collect = (chunk, category) => {
      this.event('output', { category, output: chunk.toString() });
      if (diagnosticBytes < BODY_LIMIT) {
        const part = chunk.subarray(0, BODY_LIMIT - diagnosticBytes);
        diagnosticChunks.push(part); diagnosticBytes += part.length;
      }
    };
    this.event('chali/buildDiagnostics', { diagnostics: [] });
    if (!executable) try { await new Promise((resolve, reject) => {
      this.compilerProcess = spawn(this.compiler, ['aot', config.program, '--checked', config.debugMode === 'optimized' ? '--debug-optimized' : '--debug', '--out', prefix], { cwd, env, windowsHide: true, detached: process.platform !== 'win32' });
      this.compilerProcess.stdout.on('data', (chunk) => collect(chunk, 'console'));
      this.compilerProcess.stderr.on('data', (chunk) => collect(chunk, 'stderr'));
      const buildChild = this.compilerProcess;
      const abort = () => {
        void stopProcessTree(buildChild).catch((error) => this.event('output', { category: 'stderr', output: error.message + '\n' }));
        reject(new Error('调试构建已取消'));
      };
      signal.addEventListener('abort', abort, { once: true });
      buildChild.on('error', (error) => { signal.removeEventListener('abort', abort); reject(error); });
      buildChild.on('close', (code) => { signal.removeEventListener('abort', abort); code === 0 ? resolve() : reject(new Error(`AOT 调试构建失败，退出码 ${code}`)); });
      if (this.progressReporting) {
        this.progressRequests.set(progressId, requestId);
        this.event('progressStart', { progressId, title: '构建 ChaLi 调试程序', requestId, cancellable: true });
      }
    }); } finally {
      this.event('chali/buildDiagnostics', { diagnostics: buildDiagnostics(Buffer.concat(diagnosticChunks).toString('utf8'), cwd) });
      this.progressRequests.delete(progressId);
      if (this.progressReporting) this.event('progressEnd', { progressId });
    }
    executable = executable || prefix + (process.platform === 'win32' ? '.exe' : '');
    if (signal.aborted || this.terminated) throw new Error('调试请求已取消');
    if (config.console === 'integratedTerminal') {
      this.terminal = new DebugTerminal(this.directory, output => this.event('chali/terminalOutput', { output }));
      await this.terminal.listen();
      env.CHALI_DEBUG_CONSOLE = this.terminal.endpoint;
      this.event('chali/terminal', { name: config.name || 'ChaLi 调试' });
    }
    this.state = 'starting';
    this.resetThreads();
    this.inferiorPid = null;
    this.debugEnvironment = env;
    this.debugExecutable = fs.realpathSync(executable);
    this.gdb = new Gdb(config.debuggerPath || process.env.CHALI_GDB || 'gdb', cwd, env);
    const gdb = this.gdb;
    this.metadata = new DebugMetadata((text, options) => gdb.command(text, options), output => this.event('output', { category: 'console', output }));
    this.gdb.on('output', (output, category) => this.event('output', { output, category }));
    this.gdb.on('exit', () => { if (this.gdb === gdb && !this.closingMode) this.end(); });
    this.gdb.on('fault', (error) => { if (this.gdb === gdb) { this.event('output', { category: 'stderr', output: error.message + '\n' }); if (!this.closingMode) this.end(); } });
    this.gdb.on('running', (data) => {
      if (this.gdb !== gdb || this.terminated) return;
      if (this.closingMode) return;
      ++this.stopSerial;
      this.invalidate();
      this.state = 'running';
      if (!this.suppressContinued) this.event('continued', { threadId: this.threads.selected || 1, allThreadsContinued: true });
    });
    this.gdb.on('stopped', (data) => {
      if (this.gdb !== gdb) return;
      const serial = ++this.stopSerial;
      const exitedTarget = (data.reason || '').startsWith('exited');
      if (exitedTarget) this.inferiorPid = null;
      if (this.closingMode) {
        this.closeStopped = !exitedTarget; this.pendingCloseStop = data;
        if (!this.terminated && !exitedTarget && data['thread-id'] !== undefined) this.threads.stop(data['thread-id']);
        return;
      }
      if (this.terminated) return;
      if (this.attachPending && (this.state !== 'configuring' || !exitedTarget)) { this.pendingAttachStop = data; return; }
      const paused = this.pauseRequested; this.pauseRequested = false;
      const pauseThread = this.pauseThread; this.pauseThread = null;
      const pauseStop = paused && data.reason !== 'breakpoint-hit'
        && (data.reason !== 'signal-received' || ['SIGINT', 'SIGTRAP'].includes(data['signal-name']));
      if ((data.reason || '').startsWith('exited')) {
        const exitCode = data.reason === 'exited-signalled' ? 128 + (osConstants.signals[data['signal-name']] || 1) : parseInt(data['exit-code'] || '0', 8);
        this.event('exited', { exitCode: Number.isFinite(exitCode) ? exitCode : 1 }); this.end(); return;
      }
      const exitedThread = data['thread-id'] !== undefined && this.threads.dead.has(DebugThreads.id(data['thread-id']));
      let thread = this.threads.stop(data['thread-id']);
      if (!thread || exitedThread) { void this.recoverThreadStop(gdb, { ...data, pause: pauseStop }); return; }
      if (pauseStop && pauseThread && this.threads.active.has(pauseThread)) { thread = pauseThread; this.threads.selected = thread; }
      data = { ...data, 'thread-id': String(thread) };
      if (data.reason === 'breakpoint-hit' && data.bkptno !== undefined && this.logPoints.has(Number(data.bkptno))) {
        void this.runLogPoint(this.logPoints.get(Number(data.bkptno))).catch((error) => this.event('output', { category: 'stderr', output: error.message + '\n' }));
        return;
      }
      if (this.panicPoint !== null && data.reason === 'breakpoint-hit' && Number(data.bkptno) === this.panicPoint) {
        void this.reportPanic(Number(data['thread-id']) || 1).catch((error) => {
          if (this.gdb !== gdb || this.terminated || serial !== this.stopSerial || this.threads.dead.has(thread)) return;
          this.event('output', { category: 'stderr', output: error.message + '\n' });
          this.invalidate(); this.state = 'stopped';
          this.event('stopped', { reason: 'exception', threadId: Number(data['thread-id']) || 1, allThreadsStopped: true, description: 'panic' });
        });
        return;
      }
      const stepping = data.reason === 'end-stepping-range' || data.reason === 'function-finished' || (!data.reason && this.stepCommand);
      if (stepping && this.stepCommand) {
        const frame = data.frame || {};
        const file = String(frame.fullname || frame.file || '');
        const internal = internalSource(file);
        const mapped = this.metadata?.frame?.(frame);
        const observation = path.posix.basename(file.replace(/\\/g, '/')) === '<native-debug-context>';
        const entry = mapped && /^0x[0-9a-f]+$/i.test(frame.addr || '')
          ? mapped.function?.positions?.find(position => position.address > BigInt(frame.addr)) : null;
        const prologue = mapped && !mapped.optimized && mapped.function?.context && !mapped.position
          && (Number(frame.line) === mapped.function.line || observation) && entry;
        if (internal || prologue || observation) {
          if (this.stepAttempts >= 64) {
            this.event('output', { category: 'console', output: '内部代码步进超过限制，停在当前内部位置\n' });
          } else {
            this.stepAttempts++;
            this.suppressContinued = true;
            const command = prologue ? '-exec-until --thread ' + thread + ' *0x' + entry.address.toString(16)
              : (observation ? '-exec-next --thread ' : '-exec-finish --thread ') + thread;
            void gdb.command(command).catch((error) => {
              if (this.gdb !== gdb || this.terminated || serial !== this.stopSerial) return;
              this.stepCommand = null; this.stepThread = null; this.suppressContinued = false;
              this.event('output', { category: 'stderr', output: error.message + '\n' });
              void this.recoverThreadStop(gdb, data);
            });
            return;
          }
        }
      }
      this.stepCommand = null;
      this.stepThread = null;
      this.stepAttempts = 0;
      this.suppressContinued = false;
      if (data.reason === 'signal-received' && !pauseStop) {
        void this.reportSignal(data).catch(error => {
          if (this.gdb !== gdb || this.terminated || serial !== this.stopSerial || this.threads.dead.has(thread)) return;
          this.event('output', { category: 'stderr', output: error.message + '\n' });
          this.invalidate(); this.state = 'stopped';
          this.event('stopped', { reason: 'exception', threadId: Number(data['thread-id']) || 1, allThreadsStopped: true, description: data['signal-meaning'] || data['signal-name'] });
        });
        return;
      }
      this.invalidate();
      this.state = 'stopped';
      this.event('stopped', { reason: data.reason === 'breakpoint-hit' ? 'breakpoint' : pauseStop ? 'pause' : data.reason === 'signal-received' ? 'exception' : 'step',
        threadId: Number(data['thread-id']) || 1, allThreadsStopped: true, description: pauseStop ? undefined : data['signal-meaning'] });
    });
    this.gdb.on('notify', (status, data) => {
      if (this.gdb !== gdb) return;
      // 取消附加后仍须知道 GDB 实际接管了谁，才能在退出调试器之前安全分离。
      if (status === 'thread-group-started') { this.inferiorPid = DebugThreads.id(data.pid); return; }
      if (status === 'thread-group-exited') { this.inferiorPid = null; return; }
      if (this.terminated) return;
      if (status === 'thread-created') { this.threads.start(data.id); return; }
      if (status === 'thread-exited') { this.threads.exit(data.id); return; }
      if (status !== 'breakpoint-modified' || !data.bkpt) return;
      const known = this.breakpointInfo.get(Number(data.bkpt.number));
      if (!known) return;
      known.verified = !data.bkpt.pending;
      if (data.bkpt.line) known.line = Number(data.bkpt.line);
      this.event('breakpoint', { reason: 'changed', breakpoint: known });
    });
    const command = (text) => gdb.command(text, { signal });
    await command('-gdb-set pagination off');
    await command('-gdb-set confirm off');
    await command('-gdb-set mi-async on');
    await command('-gdb-set non-stop off');
    await command('-gdb-set charset UTF-8');
    await command('-file-exec-and-symbols ' + JSON.stringify(executable));
    for (const rule of this.sourceMap.forward)
      await command('-gdb-set substitute-path ' + JSON.stringify(rule.from) + ' ' + JSON.stringify(rule.local.replace(/\\/g, '/')));
    if (attaching) {
      if (signal.aborted) throw new Error('附加已取消');
      this.attachCommandSent = true;
      try { await command(endpoint ? '-target-select remote ' + endpoint : '-target-attach ' + attachPid); this.attachCommandSent = false; }
      catch (error) { if (error.gdbError) this.attachCommandSent = false; throw error; }
      if (this.inferiorPid !== attachPid) throw new Error('GDB 附加目标 PID 与已核对进程不一致');
      if (endpoint) await verifyRemoteImage(gdb, attachPid, executable, this.remoteIdentity, this.directory, signal);
      else {
        await this.identityLease.verify();
        await this.identityLease.release(); this.identityLease = null; this.identityProcess = null;
      }
      this.identityVerified = true;
    }
    // 隐藏的 panic 断点让运行期失败停在调用现场，而不是只留下退出码。
    try {
      const inserted = await command('-break-insert -f chali_panic');
      this.panicPoint = Number(inserted.bkpt.number);
    }
    catch { this.panicPoint = null; }
    if (this.panicPoint !== null && !this.runtimeFailures) await command('-break-disable ' + this.panicPoint);
    if (this.terminal) {
      try { await command('-data-evaluate-expression "&chali_debug_console"'); }
      catch { throw new Error('集成终端要求带终端支持的 ChaLi --debug 产物'); }
    }
    if (Array.isArray(config.args) && config.args.length) await command('-exec-arguments ' + config.args.map((value) => JSON.stringify(String(value))).join(' '));
    if (config.stopOnEntry) await command('-break-insert -t f_main');
    this.state = 'configuring';
    this.event('capabilities', { capabilities: { supportsRestartRequest: !attaching } });
    if (!restart) this.event('initialized');
  }
  /**
   * @brief 在 panic 入口读取消息与调用方源码位置，报告异常停点而不继续执行。
   * @param thread 发生失败的目标线程。
   * @returns {Promise<void>} 停点事件发送完成。
   */
  async reportPanic(thread) {
    const gdb = this.gdb, serial = this.stopSerial;
    let message = '';
    // 运行期 IR 没有 DWARF 参数信息，按各平台 x64/ARM64 的第一个整型参数寄存器取消息指针。
    for (const register of this.panicArgument ? [this.panicArgument] : ['$rcx', '$rdi', '$x0']) {
      try {
        const result = await gdb.command(`-data-evaluate-expression --thread ${thread} --frame 0 (char*)${register}`);
        const quoted = /^0x[0-9a-f]+\s+"(.*)"$/i.exec(String(result.value || ''));
        if (quoted) { message = quoted[1]; break; }
      } catch { /* 尝试下一个寄存器。 */ }
    }
    if (!message) message = '运行期失败';
    let location = '';
    try {
      const data = await gdb.command(`-stack-list-frames --thread ${thread} 0 4`);
      for (const entry of data.stack || []) {
        const frame = entry.frame || entry;
        const file = String(frame.fullname || frame.file || '');
        const line = Number(frame.line);
        if (!internalSource(file) && Number.isSafeInteger(line) && line > 0) { location = this.sourceMap.toLocal(this.sourceMap.toTarget(file)) + ':' + line; break; }
      }
    } catch { /* 没有调用方源码时只报告消息。 */ }
    if (this.gdb !== gdb || this.terminated || serial !== this.stopSerial) return;
    if (this.threads.dead.has(thread)) { await this.recoverThreadStop(gdb, { reason: 'signal-received' }); return; }
    const text = '运行期失败：' + message + (location ? '（' + location + '）' : '');
    this.event('output', { category: 'console', output: text + '\n' });
    this.invalidate();
    this.state = 'stopped';
    this.event('stopped', { reason: 'exception', threadId: thread, allThreadsStopped: true, description: 'panic', text });
  }

  /** 原生信号保留信号描述，并关联最近可用的源码调用帧；不恢复目标执行。 */
  async reportSignal(data) {
    const gdb = this.gdb;
    const serial = this.stopSerial;
    const thread = Number(data['thread-id']) || 1;
    let location = '';
    try {
      const frames = await gdb.command(`-stack-list-frames --thread ${thread} 0 63`);
      for (const entry of frames.stack || []) {
        const frame = entry.frame || entry;
        const file = String(frame.fullname || frame.file || '');
        const line = Number(frame.line);
        if (!internalSource(file) && Number.isSafeInteger(line) && line > 0) { location = this.sourceMap.toLocal(this.sourceMap.toTarget(file)) + ':' + line; break; }
      }
    } catch { /* 缺少调试信息时保留信号描述。 */ }
    if (this.gdb !== gdb || this.terminated || this.stopSerial !== serial) return;
    if (this.threads.dead.has(thread)) { await this.recoverThreadStop(gdb, data); return; }
    const description = [data['signal-name'], data['signal-meaning']].filter(Boolean).join(': ') || 'native signal';
    const text = description + (location ? '（' + location + '）' : '');
    this.event('output', { category: 'console', output: text + '\n' });
    this.invalidate(); this.state = 'stopped';
    this.event('stopped', { reason: 'exception', threadId: thread, allThreadsStopped: true, description, text });
  }

  /** 校验用户断点条件：只接受单行非空文本，避免 MI 命令注入。 */
  static conditionText(value) {
    if (value === undefined || value === null) return '';
    if (typeof value !== 'string' || !value.trim() || value.length > 4096 || /[\r\n]/.test(value)) throw new Error('断点条件必须是单行非空文本');
    return value;
  }
  /** 校验 DAP 命中次数，返回正整数或零。 */
  static hitCount(value) {
    if (value === undefined || value === null) return 0;
    const text = String(value).trim();
    if (!/^[1-9][0-9]{0,8}$/.test(text)) throw new Error('断点命中次数必须是正整数');
    return Number(text);
  }
  /** 撤销 GDB 断点登记的适配器状态，编号可被新断点复用。 */
  forgetBreakpoint(id) {
    const number = Number(id);
    this.breakpointInfo.delete(number);
    this.logPoints.delete(number);
  }
  /** 为一个源文件替换断点；重启时复用原始位置而不复用 GDB 编号。 */
  async setSourceBreakpoints(args, signal) {
    const source = path.resolve(args.source.path);
    const previous = this.breakpoints.get(source) || [];
    for (const id of previous) this.forgetBreakpoint(id);
    if (previous.length) await this.gdb.command('-break-delete ' + previous.join(' '), { signal });
    const ids = [], breakpoints = [];
    for (const point of args.breakpoints || []) {
      try {
        const condition = Adapter.conditionText(point.condition);
        const hits = Adapter.hitCount(point.hitCondition);
        const logMessage = point.logMessage === undefined ? '' : String(point.logMessage);
        if (logMessage.length > 8192) throw new Error('断点日志文本过长');
        const targetSource = this.sourceMap.toTarget(source).replace(/\\/g, '/');
        let result = await this.gdb.command('-break-insert -f ' + JSON.stringify(targetSource + ':' + point.line), { signal });
        // 某些 GDB 编译单元按 substitute-path 后的全名索引；只在原位置未解析时尝试已验证的本地映射。
        const localSource = source.replace(/\\/g, '/');
        if (result.bkpt.pending && targetSource !== localSource) {
          let mapped;
          try {
            mapped = await this.gdb.command('-break-insert -f ' + JSON.stringify(localSource + ':' + point.line), { signal });
            if (!mapped.bkpt.pending) {
              await this.gdb.command('-break-delete ' + result.bkpt.number, { signal });
              result = mapped;
            } else await this.gdb.command('-break-delete ' + mapped.bkpt.number, { signal });
          } catch (error) {
            for (const pending of [result, mapped])
              if (pending?.bkpt?.number) await this.gdb.command('-break-delete ' + pending.bkpt.number).catch(() => {});
            throw error;
          }
        }
        const bp = result.bkpt, number = Number(bp.number);
        try {
          const locations = bp.locations || result.$values || [];
          const observations = locations.filter(location => path.posix.basename(String(location.fullname || location.file || '').replace(/\\/g, '/')) === '<native-debug-context>');
          let effectiveCondition = condition, guarded = false;
          if (observations.length && observations.length < locations.length) {
            // Windows 重定位会重建子位置；按符号相对位置筛选，不能依赖子位置的禁用状态。
            const bases = new Map(), clauses = [];
            for (const location of locations.filter(location => !observations.includes(location))) {
              if (!/^[\p{L}_][\p{L}\p{N}_.$:]*$/u.test(location.func || '') || !/^0x[0-9a-f]+$/i.test(location.addr || '')) throw new Error('源码断点位置不能安全关联函数');
              if (!bases.has(location.func)) {
                const value = await this.gdb.command('-data-evaluate-expression ' + JSON.stringify('(unsigned long long)&' + location.func), { signal });
                if (!/^(?:0x[0-9a-f]+|[0-9]+)$/i.test(value.value || '')) throw new Error('源码断点函数地址不可读');
                bases.set(location.func, BigInt(value.value));
              }
              const offset = BigInt(location.addr) - bases.get(location.func);
              if (offset < 0n || offset > 16777216n) throw new Error('源码断点与函数地址不匹配');
              clauses.push('((unsigned long long)$pc == (unsigned long long)&' + location.func + ' + ' + offset + ')');
            }
            effectiveCondition = '(' + clauses.join(' || ') + ')';
            if (condition) effectiveCondition += ' && (' + condition + ')';
            if (hits > 1) {
              const counter = '$chali_debug_break_' + number;
              await this.gdb.command('-interpreter-exec console ' + JSON.stringify('set ' + counter + ' = (unsigned long long)0'), { signal });
              effectiveCondition += ' && (' + counter + ' >= ' + hits + ' || ++' + counter + ' >= ' + hits + ')';
            }
            guarded = true;
          }
          if (effectiveCondition) await this.gdb.command(`-break-condition ${number} ${effectiveCondition}`, { signal });
          if (hits > 1 && !guarded) await this.gdb.command(`-break-after ${number} ${hits - 1}`, { signal });
        } catch (error) {
          await this.gdb.command('-break-delete ' + bp.number, { signal }).catch(() => {});
          throw error;
        }
        ids.push(bp.number);
        const entry = { id: number, verified: !bp.pending, line: Number(bp.line) || point.line, source: args.source };
        if (logMessage) this.logPoints.set(number, logMessage);
        this.breakpointInfo.set(number, entry);
        breakpoints.push(entry);
      } catch (error) {
        if (signal.aborted) throw error;
        breakpoints.push({ verified: false, line: point.line, message: error.message });
      }
    }
    this.breakpoints.set(source, ids);
    if (!this.breakpointSpecs) this.breakpointSpecs = new Map();
    this.breakpointSpecs.set(source, { ...args, breakpoints: (args.breakpoints || []).map((point) => ({ ...point })) });
    return { breakpoints };
  }
  /** 按函数名替换函数断点；待解析的断点在符号可用后由 GDB 通知更新。 */
  async setFunctionBreakpoints(args, signal) {
    if (this.functionBreakpoints.length) {
      for (const id of this.functionBreakpoints) this.forgetBreakpoint(id);
      await this.gdb.command('-break-delete ' + this.functionBreakpoints.join(' '), { signal });
      this.functionBreakpoints = [];
    }
    // 源码函数名到实际链接名只在调试映射可用时翻译，缺失时交回 GDB 原生名称。
    let functions = [];
    try { await this.metadata?.load(signal); functions = this.metadata?.cache ? [...this.metadata.cache.byLinkage.values()] : []; }
    catch (error) { if (signal.aborted) throw error; }
    const breakpoints = [];
    for (const point of args.breakpoints || []) {
      try {
        const name = typeof point.name === 'string' ? point.name.trim() : '';
        if (!name || name.length > 1024 || /[\r\n]/.test(name)) throw new Error('函数断点名称无效');
        const condition = Adapter.conditionText(point.condition);
        const hits = Adapter.hitCount(point.hitCondition);
        const source = functions.find((entry) => entry.name === name) || functions.find((entry) => entry.name.endsWith('::' + name));
        const target = source ? source.linkage : name;
        const result = await this.gdb.command('-break-insert -f ' + JSON.stringify(target), { signal });
        const bp = result.bkpt, number = Number(bp.number);
        try {
          if (condition) await this.gdb.command(`-break-condition ${number} ${condition}`, { signal });
          if (hits > 1) await this.gdb.command(`-break-after ${number} ${hits - 1}`, { signal });
        } catch (error) {
          await this.gdb.command('-break-delete ' + bp.number, { signal }).catch(() => {});
          throw error;
        }
        this.functionBreakpoints.push(bp.number);
        const entry = { id: number, verified: !bp.pending, line: Number(bp.line) || 1, message: bp.pending ? '函数尚未加载，等待符号解析' : undefined };
        this.breakpointInfo.set(number, entry);
        breakpoints.push(entry);
      } catch (error) {
        if (signal.aborted) throw error;
        breakpoints.push({ verified: false, message: error.message });
      }
    }
    this.functionSpecs = { ...args, breakpoints: (args.breakpoints || []).map((point) => ({ ...point })) };
    return { breakpoints };
  }
  /** 展开日志断点中的 {表达式}，不中断目标程序。 */
  async interpolate(message) {
    const parts = [];
    let at = 0;
    while (at < message.length) {
      const open = message.indexOf('{', at);
      if (open < 0) { parts.push(message.slice(at)); break; }
      const close = message.indexOf('}', open + 1);
      if (close < 0) { parts.push(message.slice(at)); break; }
      parts.push(message.slice(at, open));
      const expression = message.slice(open + 1, close).trim();
      if (!expression) parts.push('{}');
      else {
        try {
          const result = await this.gdb.command('-data-evaluate-expression ' + JSON.stringify(expression));
          parts.push(String(result.value));
        } catch {
          parts.push('{' + expression + '}');
        }
      }
      at = close + 1;
    }
    return parts.join('');
  }
  /** 命中日志断点时输出展开文本并继续，不向客户端报告暂停。 */
  async runLogPoint(message) {
    if (this.logging) return;
    const gdb = this.gdb, serial = this.stopSerial;
    this.logging = true;
    try {
      const text = await this.interpolate(message);
      if (this.gdb !== gdb || this.terminated || this.stopSerial !== serial) return;
      this.event('output', { category: 'stdout', output: text.endsWith('\n') ? text : text + '\n' });
    } finally {
      this.logging = false;
    }
    if (this.gdb !== gdb || this.terminated || this.stopSerial !== serial) return;
    try { await gdb.command('-exec-continue'); }
    catch (error) { this.event('output', { category: 'stderr', output: error.message + '\n' }); }
  }
  async request(request, controller = new AbortController()) {
    const args = request.arguments || {};
    const signal = controller.signal;
    let body = {};
    let startsSession = false;
    try {
      if (signal.aborted) throw new Error('调试请求已取消');
      const inspecting = ['stackTrace', 'scopes', 'variables', 'evaluate', 'readMemory'].includes(request.command);
      const epoch = this.epoch;
      if (inspecting) this.stopped();
      const independent = ['initialize', 'launch', 'attach', 'cancel', 'disconnect', 'terminate', 'restart'];
      if (!independent.includes(request.command) && (!this.gdb || this.terminated)) throw new Error('原生调试器尚未启动或已经结束');
      switch (request.command) {
        case 'chali/input':
          if (!this.terminal) throw new Error('当前会话没有集成终端');
          this.terminal.input(args.text === undefined ? '' : args.text, args.end === true);
          break;
        case 'initialize':
          if (this.state !== 'created') throw new Error('调试会话已经初始化');
          this.state = 'ready';
          this.progressReporting = args.supportsProgressReporting === true;
          this.invalidatedEvents = args.supportsInvalidatedEvent === true;
          body = { supportsConfigurationDoneRequest: true, supportsEvaluateForHovers: true, supportsSetVariable: true, supportsTerminateRequest: true, supportsCancelRequest: true, supportsRestartRequest: true, supportsReadMemoryRequest: true, supportsDisassembleRequest: true,
            supportsConditionalBreakpoints: true, supportsHitConditionalBreakpoints: true, supportsFunctionBreakpoints: true, supportsLogPoints: true,
            exceptionBreakpointFilters: [{ filter: 'runtime', label: 'ChaLi runtime failures', description: 'panic, assertions and checked memory failures', default: true }] };
          break;
        case 'launch':
          if (this.state !== 'ready') throw new Error('调试会话不能重复 launch');
          startsSession = true;
          await this.launch(args, signal, false, request.seq);
          break;
        case 'attach':
          if (this.state !== 'ready') throw new Error('调试会话不能重复 attach');
          startsSession = true;
          await this.launch({ ...args, request: 'attach' }, signal, false, request.seq);
          break;
        case 'cancel': {
          const task = this.requests.get(args.requestId === undefined ? this.progressRequests.get(args.progressId) : args.requestId);
          if (task && task !== controller) task.abort();
          break;
        }
        case 'restart': {
          if (this.attached) { this.restartPending = false; throw new Error('附加会话不重启外部目标，请重新附加'); }
          if (!this.configuration) { this.restartPending = false; throw new Error('尚无可重启的调试配置'); }
          startsSession = true;
          await this.shutdown();
          this.restartPending = false;
          this.breakpoints.clear();
          this.functionBreakpoints = [];
          this.breakpointInfo.clear();
          this.logPoints.clear();
          await this.launch(this.configuration, signal, true, request.seq);
          for (const spec of this.breakpointSpecs ? [...this.breakpointSpecs.values()] : []) await this.setSourceBreakpoints(spec, signal);
          if (this.functionSpecs) await this.setFunctionBreakpoints(this.functionSpecs, signal);
          try { await this.gdb.command('-exec-run', { signal }); }
          catch (error) { if (error.gdbError) this.state = 'configuring'; else this.end(); throw error; }
          break;
        }
        case 'setBreakpoints': body = await this.setSourceBreakpoints(args, signal); break;
        case 'setFunctionBreakpoints': body = await this.setFunctionBreakpoints(args, signal); break;
        case 'setExceptionBreakpoints':
          if (!Array.isArray(args.filters) || args.filters.some(value => value !== 'runtime')) throw new Error('未知运行期错误过滤器');
          if ((args.filterOptions || []).length || (args.exceptionOptions || []).length) throw new Error('不支持运行期错误条件或异常路径选项');
          if (this.panicPoint !== null) await this.gdb.command((args.filters.includes('runtime') ? '-break-enable ' : '-break-disable ') + this.panicPoint, { signal });
          this.runtimeFailures = args.filters.includes('runtime');
          break;
        case 'configurationDone': {
          if (this.state !== 'configuring') throw new Error('调试配置已经完成或当前不能启动程序');
          if (this.attached) {
            if (!this.identityVerified || this.inferiorPid === null) throw new Error('附加目标身份未确认或已经退出');
            this.attachPending = false;
            this.pendingAttachStop = null;
            const generation = this.threads.generation;
            const data = await this.gdb.command('-thread-info', { signal });
            this.threads.sync(data.threads || [], data['current-thread-id'], generation);
            const threadId = this.threads.active.keys().next().value;
            if (!threadId) throw new Error('附加目标没有活动线程');
            this.threads.selected = threadId;
            this.invalidate(); this.state = 'stopped';
            if (this.configuration.stopOnAttach !== false) {
              this.event('stopped', { reason: 'entry', threadId, allThreadsStopped: true, description: '已附加到现有目标' }); break;
            }
          }
          this.state = 'running';
          ++this.stopSerial;
          this.invalidate();
          try { await this.gdb.command(this.attached ? '-exec-continue' : '-exec-run', { signal }); }
          catch (error) { if (error.gdbError) this.state = 'configuring'; else this.end(); throw error; }
          break;
        }
        case 'threads': {
          const generation = this.threads.generation;
          const data = await this.gdb.command('-thread-info', { signal });
          body.threads = this.threads.sync(data.threads || [], data['current-thread-id'], generation);
          break;
        }
        case 'stackTrace': {
          await this.metadata?.load(signal);
          this.stopped(epoch);
          const thread = this.threads.resolve(args.threadId);
          const start = args.startFrame === undefined ? 0 : args.startFrame;
          const levels = args.levels === undefined || args.levels === 0 ? 50 : args.levels;
          if (!Number.isSafeInteger(thread) || thread < 1 || !Number.isSafeInteger(start) || start < 0 || !Number.isSafeInteger(levels) || levels < 1 || levels > 1000 || !Number.isSafeInteger(start + levels)) throw new Error('调用栈线程或分页参数无效');
          const data = await this.gdb.command(`-stack-list-frames --thread ${thread} ${start} ${start + levels - 1}`, { signal });
          this.stopped(epoch);
          body.stackFrames = (data.stack || []).map((entry) => {
            const frame = entry.frame || entry;
            const id = this.nextHandle++;
            const original = frame.fullname ? { ...frame, fullname: this.sourceMap.toTarget(frame.fullname) } : frame;
            const sourceInfo = this.metadata?.frame(original);
            const observation = sourceInfo?.function && path.posix.basename(String(frame.fullname || frame.file || '').replace(/\\/g, '/')) === '<native-debug-context>';
            const sourceFile = observation ? sourceInfo.function.file : frame.fullname || frame.file;
            const localFile = sourceFile ? this.sourceMap.toLocal(this.sourceMap.toTarget(sourceFile)) : undefined;
            const library = sourceInfo?.function?.library;
            const implementationHash = library?.implementation === 'bitcode' ? library.bitcodeSha256 : library?.implementationSha256;
            const sourceOrigin = library ? `预编译库 ${library.module}@${library.version} · ${library.implementation} · SHA-256 ${implementationHash}`
              + (localFile && !fs.existsSync(localFile) ? ' · 源码不可用；仍可查看符号、位置、变量及反汇编' : '') : undefined;
            this.frames.set(id, { thread, level: Number(frame.level), native: frame, metadata: sourceInfo });
            return { id, name: sourceInfo?.name || frame.func || '<native>', line: (observation ? sourceInfo.function.line : Number(frame.line)) || 1, column: sourceInfo?.column || Number(frame.col) || 1,
              source: localFile ? { path: localFile, name: path.basename(localFile), ...(sourceOrigin ? { origin: sourceOrigin } : {}) } : undefined,
              instructionPointerReference: frame.addr };
          });
          body.totalFrames = body.stackFrames.length;
          break;
        }
        case 'scopes': {
          const frame = this.frames.get(args.frameId);
          if (!frame) throw new Error('调用帧已经过期');
          body.scopes = [{ name: frame.metadata?.optimized ? '局部变量与参数（O3 优化）' : '局部变量与参数', presentationHint: 'locals', expensive: false, variablesReference: this.handle(frame) }];
          for (const macro of frame.metadata?.macros || []) {
            body.scopes.push({ name: '$' + macro.name, expensive: false,
              variablesReference: this.handle({ ...frame, macro: macro.id }),
              source: { path: this.sourceMap.toLocal(macro.definition.file), name: path.basename(this.sourceMap.toLocal(macro.definition.file)) },
              line: macro.definition.line || 1, column: macro.definition.column || 1 });
          }
          break;
        }
        case 'variables': {
          const target = this.handles.get(args.variablesReference);
          if (!target || target.kind === 'memory') throw new Error('变量引用已经过期或类别无效');
          if (target.kind === 'value') {
            const memory = target.frame ? await this.memoryForFrame(target.frame, signal) : await this.memoryFor(target.thread, signal);
            const decoded = await decode(target.layout, target.address, memory, { start: args.start, count: args.count === undefined ? 100 : args.count }, target.stored);
            const identity = (layout, address) => {
              while (layout.kind === 'R' && layout.children.length) layout = layout.children[0];
              return layout.kind + ':' + layout.name + ':' + address;
            };
            const ancestors = [...(target.ancestors || []), identity(target.layout, target.address)];
            if (ancestors.length > 32) throw new Error('变量展开超过引用深度预算');
            body.variables = decoded.children.map((item) => {
              const cycle = item.childLayout && !item.stored && (ancestors.includes(identity(item.childLayout, item.address))
                || (item.childLayout.kind === 'R' && ancestors.some(key => key.endsWith(':' + item.address))));
              const reference = !cycle && item.childLayout && (item.stored || item.address > 0n)
                ? this.handle({ kind: 'value', thread: target.thread, frame: target.frame, address: item.address, layout: item.childLayout, stored: item.stored, ancestors }) : 0;
              const memoryReference = item.childLayout && !item.stored && !['L', 'M', 'E', 'O', 'F'].includes(item.childLayout.kind) && item.address > 0n
                ? 'chali-memory:' + this.handle({ kind: 'memory', thread: target.thread, frame: target.frame, address: item.address }) : undefined;
              return { name: item.name, value: cycle ? '<循环引用：' + item.value + '>' : item.value, type: item.type, variablesReference: reference, memoryReference,
                presentationHint: { attributes: ['readOnly'] } };
            });
            break;
          }
          const frame = target;
          const data = await this.gdb.command(`-stack-list-variables --thread ${frame.thread} --frame ${frame.level} --simple-values`, { signal });
          // GDB 从最内层逐块列举，保留首先遇到的绑定，隐藏被遮蔽的外层同名值。
          const visible = new Set();
          const bindings = visibleBindings(frame.metadata);
          const rows = (data.variables || []).filter(variable => {
            const binding = bindings.get(variable.name);
            if (!binding && (variable.name.startsWith('__chali_macro_debug_') || variable.name.startsWith('__chali_storage_') || variable.name.startsWith('__chali_context_'))) return false;
            if ((binding?.macro ?? -1) !== (frame.macro ?? -1)) return false;
            if (visible.has(variable.name)) return false;
            visible.add(variable.name); return true;
          });
          const formatVariable = async variable => {
            const value = /^(?:int8|uint8)$/.test(variable.type || '') ? (variable.value || '').replace(/^(-?[0-9]+)\s+'.*'$/, '$1') : variable.value;
            const binding = bindings.get(variable.name);
            const numeric = /^(?:0x[0-9a-f]+|[0-9]+)$/i.test(value || '') ? BigInt(value) : null;
            // 帧身份与活动内存目录分别核对，逻辑地址本身不能证明所属上下文。
            const declaredLayout = binding ? this.layoutAt(binding.layout) : null;
            const container = declaredLayout && ['L', 'M', 'E', 'O', 'F'].includes(declaredLayout.kind);
            const addressable = (binding?.address === true || container) && numeric !== null && numeric >= 0n;
            const layout = addressable ? declaredLayout : null;
            let display = value || '<不可用>';
            if (/<optimized out>/i.test(display)) display = '<已优化：当前位置没有可读值>';
            else if (frame.metadata?.optimized && !binding && !/^(?:u?int(?:8|16|32|64)|float(?:32|64)|bool|char)$/.test(variable.type || ''))
              display = '<布局不可用：无法唯一对应优化变量>';
            let memoryReference;
            let variablesReference = 0;
            let mirrorReadable = true;
            const mirror = binding?.storageDwarf && frame.level === 0 && !frame.metadata?.optimized
              && declaredLayout && ['i', 'u', 'f', 'b', 'c'].includes(declaredLayout.kind);
            if (mirror) {
              try {
                const storage = await this.mirrorStorage(frame, binding, signal);
                display = (await decode(declaredLayout, storage.logical, storage.memory, { count: 0 })).text;
              } catch (error) { display = '<不可读：' + error.message + '>'; mirrorReadable = false; }
            }
            if (addressable) {
              if (!container && numeric > 0n) memoryReference = 'chali-memory:' + this.handle({ kind: 'memory', thread: frame.thread, frame, address: numeric });
              if (layout) {
                try {
                  const memory = await this.memoryForFrame(frame, signal, data.variables || []);
                  display = (await decode(layout, numeric, memory, { count: 0 })).text;
                } catch (error) {
                  display = '<不可读：' + error.message + '>';
                  memoryReference = undefined;
                  // 失效或无法核对的帧不继续暴露类型化展开。
                }
                if (layout && numeric > 0n && expandable(layout) && layout.kind !== 's' && !display.startsWith('<不可读：'))
                  variablesReference = this.handle({ kind: 'value', thread: frame.thread, frame, address: numeric, layout });
              }
            }
            const readOnly = frame.metadata?.optimized || !binding || binding.readOnly || binding.address || !mirrorReadable || ((binding.storage || binding.boxed) && !mirror)
              || !['i64', 'double'].includes(binding.nativeStorage) || !declaredLayout || !['i', 'u', 'f', 'b', 'c'].includes(declaredLayout.kind);
            return { name: binding?.name || variable.name, value: display, type: variable.type, evaluateName: binding?.name || variable.name, variablesReference, memoryReference,
              presentationHint: readOnly ? { attributes: ['readOnly'] } : undefined };
          };
          body.variables = [];
          for (let start = 0; start < rows.length; start += 16)
            body.variables.push(...await Promise.all(rows.slice(start, start + 16).map(formatVariable)));
          break;
        }
        case 'disassemble': {
          if (typeof args.memoryReference !== 'string' || !/^0x[0-9a-f]+$/i.test(args.memoryReference)) throw new Error('反汇编地址无效');
          if (!Number.isSafeInteger(args.offset) || !Number.isSafeInteger(args.instructionOffset) || !Number.isSafeInteger(args.instructionCount)
            || args.instructionCount < 1 || args.instructionCount > 4096) throw new Error('反汇编偏移或数量无效');
          const base = BigInt(args.memoryReference) + BigInt(args.offset);
          // 先向后多取一段，按地址切片；GDB 只能从低地址向前反汇编。
          const span = 64n + BigInt(args.instructionCount + Math.abs(args.instructionOffset)) * 16n;
          const begin = base - BigInt(args.instructionOffset < 0 ? Math.min(256, -args.instructionOffset * 16) : 0);
          const listed = [];
          let from = begin;
          while (from < base + span && listed.length < args.instructionCount + Math.abs(args.instructionOffset) + 64) {
            const result = await this.gdb.command(`-data-disassemble -s 0x${from.toString(16)} -e 0x${(from + 1024n).toString(16)} -- 0`, { signal });
            const rows = result.asm_insns || [];
            if (!rows.length) break;
            for (const row of rows) {
              const address = BigInt(row.address);
              if (!listed.length || address > listed[listed.length - 1].address) listed.push({ address, text: String(row.inst || ''), symbol: row['func-name'], offset: Number(row.offset) || 0 });
            }
            const last = listed[listed.length - 1].address;
            if (last + 1n <= from) break;
            from = last + 1n;
          }
          if (!listed.length) throw new Error('反汇编区间不可读');
          let anchor = listed.findIndex((item) => item.address >= base);
          if (anchor < 0) anchor = listed.length - 1;
          const start = Math.max(0, anchor + args.instructionOffset);
          const chosen = listed.slice(start, start + args.instructionCount);
          if (!chosen.length) throw new Error('反汇编不超过可用区间');
          body = {
            memoryReference: args.memoryReference,
            instructions: chosen.map((item) => ({ address: '0x' + item.address.toString(16), instruction: item.text,
              presentationHint: 'normal', symbol: item.symbol })),
          };
          break;
        }
        case 'readMemory': {
          if (typeof args.memoryReference !== 'string') throw new Error('内存引用类别无效');
          const match = /^chali-memory:([1-9][0-9]{0,15})$/.exec(args.memoryReference || '');
          const id = match ? Number(match[1]) : 0;
          const reference = Number.isSafeInteger(id) && this.handles.get(id);
          if (!reference || reference.kind !== 'memory') throw new Error('内存引用已经过期或类别无效');
          const offset = args.offset === undefined ? 0 : args.offset;
          if (!Number.isSafeInteger(offset) || !Number.isSafeInteger(args.count) || args.count < 0 || args.count > MEMORY_LIMIT) throw new Error('内存偏移或读取预算无效');
          const memory = new DebugMemory(async (text, options) => {
            this.stopped(epoch);
            const result = await this.gdb.command(text, options);
            this.stopped(epoch);
            return result;
          });
          const snapshot = reference.frame ? await this.memoryForFrame(reference.frame, signal) : await memory.snapshot(reference.thread, signal);
          const address = reference.address + BigInt(offset);
          const data = await snapshot.read(address, args.count);
          body = { address: '0x' + address.toString(16), data: data.toString('base64'), unreadableBytes: 0 };
          break;
        }
        case 'setVariable': {
          this.stopped();
          const frame = this.handles.get(args.variablesReference);
          if (!frame || frame.kind) throw new Error('需要当前有效的局部变量作用域；不支持直接修改聚合、引用或拥有值');
          if (frame.metadata?.optimized) throw new Error('优化调试不修改变量：无法保证真实帧槽仍存在');
          if (typeof args.name !== 'string' || args.name.length > 1024) throw new Error('变量名称无效');
          const bindings = visibleBindings(frame.metadata);
          const entry = [...bindings].find(([, binding]) => binding.name === args.name && (binding.macro ?? -1) === (frame.macro ?? -1));
          if (!entry) throw new Error('当前作用域没有可修改变量：' + args.name);
          const [name, binding] = entry;
          if (!/^[\p{L}_][\p{L}\p{N}_]*$/u.test(name)) throw new Error('变量原生名称不可安全定位');
          if ((binding.storage || binding.boxed) && frame.level !== 0) throw new Error('真实绑定镜像仅支持栈顶开发源码帧修改');
          const value = await encodeAssignment(binding, this.layoutAt(binding.layout), args.value);
          this.stopped(epoch);
          const selector = ` --thread ${frame.thread} --frame ${frame.level}`;
          const rows = (await this.gdb.command('-stack-list-variables' + selector + ' --simple-values', { signal })).variables || [];
          const variable = rows.find(row => row.name === name);
          if (!variable || variable.type !== binding.type || /optimized out|不可用/.test(variable.value || '')) throw new Error('变量已经不可用或类型不匹配');
          let physical;
          if (binding.storage || binding.boxed) {
            const storage = await this.mirrorStorage(frame, binding, signal);
            physical = await storage.memory.address(storage.logical, value.bytes.length);
          } else {
            const result = await this.gdb.command('-data-evaluate-expression' + selector + ' ' + JSON.stringify('(unsigned long long)&(' + name + ')'), { signal });
            if (!/^(?:0x[0-9a-f]+|[0-9]+)$/i.test(result.value || '') || BigInt(result.value) === 0n) throw new Error('变量没有真实可写地址');
            physical = BigInt(result.value);
          }
          this.stopped(epoch);
          if (signal.aborted) throw new Error('变量修改已取消');
          this.invalidateValues();
          try {
            await this.gdb.command('-data-write-memory-bytes 0x' + physical.toString(16) + ' ' + value.bytes.toString('hex'), { signal });
            body = { value: value.text, type: value.type, variablesReference: 0 };
          } finally {
            this.invalidateValues();
            if (this.invalidatedEvents && this.state === 'stopped') this.event('invalidated', { areas: ['variables'], threadId: frame.thread });
          }
          break;
        }
        case 'evaluate': {
          const tree = parseExpression(args.expression);
          const frame = this.frames.get(args.frameId);
          if (!frame) throw new Error('只读表达式需要当前有效的源码调用帧');
          const bindings = visibleBindings(frame.metadata);
          let rows;
          const resolve = async name => {
            if (!rows) rows = (await this.gdb.command(`-stack-list-variables --thread ${frame.thread} --frame ${frame.level} --simple-values`, { signal })).variables || [];
            const variable = rows.find(row => {
              const binding = bindings.get(row.name);
              return binding && binding.name === name && (binding.macro ?? -1) === (frame.macro ?? -1);
            });
            if (!variable) throw new Error((frame.metadata?.optimized ? '当前优化作用域没有唯一可读绑定：' : '当前作用域没有可读变量：') + name);
            const binding = bindings.get(variable.name), layout = this.layoutAt(binding.layout);
            let text = variable.value || '<不可用>';
            if (binding.storageDwarf && frame.level === 0 && !frame.metadata?.optimized && layout && ['i', 'u', 'f', 'b', 'c'].includes(layout.kind)) {
              const storage = await this.mirrorStorage(frame, binding, signal);
              text = (await decode(layout, storage.logical, storage.memory, { count: 0 })).text;
            }
            if (/<optimized out>/i.test(text)) throw new Error('变量已被优化，当前位置没有可读值：' + name);
            if (/^(?:int8|uint8)$/.test(variable.type || '')) text = text.replace(/^(-?[0-9]+)\s+'.*'$/, '$1');
            const addressable = layout && (binding.address || ['L', 'M', 'E', 'O', 'F'].includes(layout.kind));
            const address = addressable && /^(?:0x[0-9a-f]+|[0-9]+)$/i.test(text) ? BigInt(text) : null;
            if (address !== null) text = (await decode(layout, address, await this.memoryForFrame(frame, signal), { count: 0 })).text;
            else if (addressable) throw new Error('变量没有可读取的地址：' + name);
            if (address !== null && address > 0n && layout.kind === 'R' && layout.children.length === 1 && ['i', 'u', 'f', 'b', 'c'].includes(layout.children[0].kind)) {
              const element = layout.children[0];
              return {text:(await decode(element,address,await this.memoryForFrame(frame,signal),{count:0})).text,type:typeName(element),layout:element,address};
            }
            return { text, type: variable.type, layout: address === null ? null : layout, address };
          };
          const project = async (target, key, indexed, tupleOnly = false) => {
            if (!target.layout) throw new Error('该值没有可读取的字段或索引');
            let layout = target.layout;
            while (layout.kind === 'R' && layout.children.length) layout = layout.children[0];
            const tuple = layout.kind === 'T' || (layout.kind === 'S' && /^\(/.test(target.type || ''));
            if (tupleOnly && !tuple) throw new Error('数字字段只支持元组');
            if (indexed ? !tuple && !['a', 'A', 'L'].includes(layout.kind) : !['S', 'O'].includes(layout.kind)) throw new Error('该类型不支持此字段或索引表达式：' + target.type);
            // 普通记录的现行布局把嵌套字段登记成 start.first；保留原偏移构造只读子视图。
            if (!indexed && layout.kind === 'S' && !layout.children.some(child => child.name === key)) {
              const prefix = key + '.';
              const children = layout.children.filter(child => child.name.startsWith(prefix)).map(child => ({ ...child, name: child.name.slice(prefix.length) }));
              if (children.length) {
                const nested = { ...layout, name: key, children };
                return { text: '{…}', type: '记录', layout: nested, address: target.address, stored: target.stored };
              }
            }
            const decoded = await decode(target.layout, target.address, await this.memoryForFrame(frame, signal),
              indexed ? { start: key, count: 1 } : { count: 1000 }, target.stored);
            const item = indexed ? decoded.children[0] : decoded.children.find(child => child.name === key);
            if (!item || (indexed && !tuple && item.name !== String(key))) throw new Error(indexed ? '索引越界或不可读' : '字段不存在或超过读取预算：' + key + '（可读字段：' + decoded.children.map(child => child.name).join(', ') + '）');
            if (item.value.startsWith('<不可读：')) throw new Error(item.value);
            return { text: item.value, type: item.type, layout: item.childLayout, address: item.address, stored: item.stored };
          };
          const value = await evaluateExpression(tree, resolve, project);
          const variablesReference = value.layout && expandable(value.layout) && value.layout.kind !== 's' && (value.stored || value.address > 0n)
            ? this.handle({ kind: 'value', thread: frame.thread, frame, address: value.address, layout: value.layout, stored: value.stored }) : 0;
          body = { result: value.text, type: value.type, variablesReference };
          break;
        }
        case 'continue':
        case 'next':
        case 'stepIn':
        case 'stepOut': {
          this.stopped();
          if (args.singleThread === true) throw new Error('当前仅支持全停调试模式');
          const thread = this.threads.resolve(args.threadId);
          this.threads.selected = thread;
          ++this.stopSerial;
          this.pauseRequested = false;
          this.state = 'running'; this.invalidate();
          const commands = { continue: '-exec-continue', next: '-exec-next', stepIn: '-exec-step', stepOut: '-exec-finish' };
          this.stepCommand = request.command === 'continue' ? null : commands[request.command];
          this.stepThread = request.command === 'continue' ? null : thread;
          this.stepAttempts = 0;
          this.suppressContinued = false;
          try {
            try { await this.gdb.command(commands[request.command] + (request.command === 'continue' ? '' : ' --thread ' + thread), { signal }); }
            catch (error) {
              // Linux 精简系统库可能没有函数边界；只对确认无源码的所选线程退到其调用方。
              if (!error.gdbError || !['next', 'stepIn'].includes(request.command)
                  || !/Cannot find bounds of current function/.test(error.message)) throw error;
              const gdb = this.gdb, serial = this.stopSerial;
              const frames = await gdb.command('-stack-list-frames --thread ' + thread + ' 0 0', { signal });
              const top = frames.stack?.[0]?.frame || frames.stack?.[0];
              const file = String(top?.fullname || top?.file || '');
              if (!top || !internalSource(file) || this.gdb !== gdb || serial !== this.stopSerial) throw error;
              this.threads.resolve(thread);
              await gdb.command('-exec-finish --thread ' + thread, { signal });
            }
          }
          catch (error) {
            this.stepCommand = null; this.stepThread = null;
            if (error.gdbError && this.state === 'running') this.state = 'stopped';
            else if (!this.restartPending) this.end();
            throw error;
          }
          if (request.command === 'continue') body.allThreadsContinued = true;
          break;
        }
        case 'pause': {
          if (this.state !== 'running') throw new Error('程序当前没有运行');
          this.pauseThread = this.threads.resolve(args.threadId);
          this.pauseRequested = true; this.stepCommand = null; this.stepThread = null; this.suppressContinued = false;
          try {
            await this.interruptTarget(signal);
          } catch (error) { this.pauseRequested = false; this.pauseThread = null; throw error; }
          break;
        }
        case 'disconnect':
        case 'terminate': {
          if (args.terminateDebuggee !== undefined && typeof args.terminateDebuggee !== 'boolean') throw new Error('terminateDebuggee 必须是布尔值');
          if (args.suspendDebuggee !== undefined && typeof args.suspendDebuggee !== 'boolean') throw new Error('suspendDebuggee 必须是布尔值');
          if (args.suspendDebuggee === true) throw new Error('不支持断开后保留暂停的目标');
          const keepTarget = request.command === 'terminate' ? false : args.terminateDebuggee === undefined ? this.attached === true : !args.terminateDebuggee;
          for (const task of this.requests.values()) if (task !== controller) task.abort();
          await this.shutdown({ keepTarget });
          this.end();
          break;
        }
        default: throw new Error('原生调试暂不支持请求：' + request.command);
      }
      if (inspecting) this.stopped(epoch);
      this.send({ type: 'response', request_seq: request.seq, command: request.command, success: true, body });
    } catch (error) {
      this.send({ type: 'response', request_seq: request.seq, command: request.command, success: false, message: error.message });
      if (startsSession) {
        try { await this.shutdown(); }
        catch (failure) { this.event('output', { category: 'stderr', output: failure.message + '\n' }); }
        if (!this.restartPending) this.end();
      }
    }
  }
}

if (require.main === module) new Adapter(process.argv[2]);
module.exports = { Adapter, Gdb, parseMi, temporaryRoot };
