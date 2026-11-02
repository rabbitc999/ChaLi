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
const { parseLayout, decode, expandable } = require('./debugValue');

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
        else result[key] = value;
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
    this.stepAttempts = 0;        ///< 连续跳过的内部停点计数。
    this.suppressContinued = false;  ///< 过滤内部停点时不为每次恢复上报 continued。
    this.panicPoint = null;          ///< 隐藏的 panic 入口断点编号。
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
    if (request.command === 'restart') {
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
    const snapshot = await memory.snapshot(thread, signal);
    this.valueMemories.set(thread, snapshot);
    return snapshot;
  }
  /** 保证查看请求属于当前暂停代次。 */
  stopped(epoch = this.epoch) {
    if (this.state !== 'stopped' || epoch !== this.epoch) throw new Error('程序没有暂停，或调试引用已经过期');
  }
  /** 回收本会话的构建进程和 GDB 进程树，重复调用共享同一次清理。 */
  async shutdown() {
    if (this.shutdownPromise) return this.shutdownPromise;
    this.invalidate();
    const gdb = this.gdb;
    this.metadata?.cancel(); this.metadata = null;
    const compiler = this.compilerProcess;
    const terminal = this.terminal;
    this.terminal = null;
    this.gdb = null;
    this.compilerProcess = null;
    this.shutdownPromise = (async () => {
      try {
        if (gdb) {
          gdb.fail(new Error('调试会话已关闭'));
          await gdb.command('-gdb-exit', { timeoutMs: 1000 }).catch(() => {});
          await stopProcessTree(gdb.process);
        }
        await stopProcessTree(compiler);
      } finally {
        await terminal?.drain();
        terminal?.close();
      }
    })();
    try { await this.shutdownPromise; } finally { this.shutdownPromise = null; }
  }
  handle(value) { const id = this.nextHandle++; this.handles.set(id, value); return id; }
  async launch(config, signal, restart = false, requestId) {
    if (signal.aborted) throw new Error('调试请求已取消');
    if ((!config.program || typeof config.program !== 'string') && (!config.executable || typeof config.executable !== 'string')) throw new Error('缺少调试入口 program 或 executable');
    if (config.args !== undefined && (!Array.isArray(config.args) || config.args.length > 65536 || config.args.some(value => typeof value !== 'string' || value.includes('\0')))) throw new Error('args 必须是无 NUL 的字符串数组，且数量不得超过限制');
    this.state = 'building';
    this.configuration = { ...config };
    this.terminated = false;
    const cwd = path.resolve(config.cwd || path.dirname(config.executable || config.program));
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
      this.compilerProcess = spawn(this.compiler, ['aot', config.program, '--checked', '--debug', '--out', prefix], { cwd, env, windowsHide: true, detached: process.platform !== 'win32' });
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
    this.gdb = new Gdb(config.debuggerPath || process.env.CHALI_GDB || 'gdb', cwd, env);
    const gdb = this.gdb;
    this.metadata = new DebugMetadata((text, options) => gdb.command(text, options), output => this.event('output', { category: 'console', output }));
    this.gdb.on('output', (output, category) => this.event('output', { output, category }));
    this.gdb.on('exit', () => { if (this.gdb === gdb) this.end(); });
    this.gdb.on('fault', (error) => { if (this.gdb === gdb) { this.event('output', { category: 'stderr', output: error.message + '\n' }); this.end(); } });
    this.gdb.on('running', (data) => {
      if (this.gdb !== gdb || this.terminated) return;
      this.invalidate();
      this.state = 'running';
      if (!this.suppressContinued) this.event('continued', { threadId: Number(data['thread-id']) || 1, allThreadsContinued: true });
    });
    this.gdb.on('stopped', (data) => {
      if (this.gdb !== gdb || this.terminated) return;
      if ((data.reason || '').startsWith('exited')) {
        const exitCode = data.reason === 'exited-signalled' ? 128 + (osConstants.signals[data['signal-name']] || 1) : parseInt(data['exit-code'] || '0', 8);
        this.event('exited', { exitCode: Number.isFinite(exitCode) ? exitCode : 1 }); this.end(); return;
      }
      if (data.reason === 'breakpoint-hit' && data.bkptno !== undefined && this.logPoints.has(Number(data.bkptno))) {
        void this.runLogPoint(this.logPoints.get(Number(data.bkptno))).catch((error) => this.event('output', { category: 'stderr', output: error.message + '\n' }));
        return;
      }
      if (this.panicPoint !== null && data.reason === 'breakpoint-hit' && Number(data.bkptno) === this.panicPoint) {
        void this.reportPanic(Number(data['thread-id']) || 1).catch((error) => {
          this.event('output', { category: 'stderr', output: error.message + '\n' });
          this.invalidate(); this.state = 'stopped';
          this.event('stopped', { reason: 'exception', threadId: Number(data['thread-id']) || 1, allThreadsStopped: true, description: 'panic' });
        });
        return;
      }
      const stepping = data.reason === 'end-stepping-range' || data.reason === 'function-finished';
      if (stepping && this.stepCommand) {
        const frame = data.frame || {};
        const file = String(frame.fullname || frame.file || '');
        const internal = !file || file.startsWith('<');
        if (internal) {
          if (this.stepAttempts >= 64) {
            this.event('output', { category: 'console', output: '内部代码步进超过限制，停在当前内部位置\n' });
          } else {
            this.stepAttempts++;
            this.suppressContinued = true;
            void gdb.command('-exec-finish').catch((error) => this.event('output', { category: 'stderr', output: error.message + '\n' }));
            return;
          }
        }
      }
      this.stepCommand = null;
      this.stepAttempts = 0;
      this.suppressContinued = false;
      this.invalidate();
      this.state = 'stopped';
      this.event('stopped', { reason: data.reason === 'breakpoint-hit' ? 'breakpoint' : data.reason === 'signal-received' ? 'exception' : 'step',
        threadId: Number(data['thread-id']) || 1, allThreadsStopped: true, description: data['signal-meaning'] });
    });
    this.gdb.on('notify', (status, data) => {
      if (this.gdb !== gdb || this.terminated || status !== 'breakpoint-modified' || !data.bkpt) return;
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
    await command('-gdb-set charset UTF-8');
    await command('-file-exec-and-symbols ' + JSON.stringify(executable));
    // 隐藏的 panic 断点让运行期失败停在调用现场，而不是只留下退出码。
    try { const inserted = await command('-break-insert -f chali_panic'); this.panicPoint = Number(inserted.bkpt.number); }
    catch { this.panicPoint = null; }
    if (this.terminal) {
      try { await command('-data-evaluate-expression "&chali_debug_console"'); }
      catch { throw new Error('集成终端要求带终端支持的 ChaLi --debug 产物'); }
    }
    if (Array.isArray(config.args) && config.args.length) await command('-exec-arguments ' + config.args.map((value) => JSON.stringify(String(value))).join(' '));
    if (config.stopOnEntry) await command('-break-insert -t f_main');
    this.state = 'configuring';
    if (!restart) this.event('initialized');
  }
  /**
   * @brief 在 panic 入口读取消息与调用方源码位置，报告异常停点而不继续执行。
   * @param thread 发生失败的目标线程。
   * @returns {Promise<void>} 停点事件发送完成。
   */
  async reportPanic(thread) {
    let message = '';
    // 运行期 IR 没有 DWARF 参数信息，按各平台 x64/ARM64 的第一个整型参数寄存器取消息指针。
    for (const register of ['$rcx', '$rdi', '$x0']) {
      try {
        const result = await this.gdb.command(`-data-evaluate-expression (char*)${register}`);
        const quoted = /^0x[0-9a-f]+\s+"(.*)"$/i.exec(String(result.value || ''));
        if (quoted) { message = quoted[1]; break; }
      } catch { /* 尝试下一个寄存器。 */ }
    }
    if (!message) message = '运行期失败';
    let location = '';
    try {
      const data = await this.gdb.command(`-stack-list-frames --thread ${thread} 0 4`);
      for (const entry of data.stack || []) {
        const frame = entry.frame || entry;
        const file = String(frame.fullname || frame.file || '');
        const line = Number(frame.line);
        if (file && !file.startsWith('<') && Number.isSafeInteger(line) && line > 0) { location = file + ':' + line; break; }
      }
    } catch { /* 没有调用方源码时只报告消息。 */ }
    const text = '运行期失败：' + message + (location ? '（' + location + '）' : '');
    this.event('output', { category: 'console', output: text + '\n' });
    this.invalidate();
    this.state = 'stopped';
    this.event('stopped', { reason: 'exception', threadId: thread, allThreadsStopped: true, description: 'panic', text });
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
        const result = await this.gdb.command('-break-insert -f ' + JSON.stringify(source.replace(/\\/g, '/') + ':' + point.line), { signal });
        const bp = result.bkpt, number = Number(bp.number);
        try {
          if (condition) await this.gdb.command(`-break-condition ${number} ${condition}`, { signal });
          if (hits > 1) await this.gdb.command(`-break-after ${number} ${hits - 1}`, { signal });
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
    this.logging = true;
    try {
      const text = await this.interpolate(message);
      this.event('output', { category: 'stdout', output: text.endsWith('\n') ? text : text + '\n' });
    } finally {
      this.logging = false;
    }
    try { await this.gdb.command('-exec-continue'); }
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
      const independent = ['initialize', 'launch', 'cancel', 'disconnect', 'terminate', 'restart'];
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
          body = { supportsConfigurationDoneRequest: true, supportsEvaluateForHovers: true, supportsTerminateRequest: true, supportsCancelRequest: true, supportsRestartRequest: true, supportsReadMemoryRequest: true,
            supportsConditionalBreakpoints: true, supportsHitConditionalBreakpoints: true, supportsFunctionBreakpoints: true, supportsLogPoints: true };
          break;
        case 'launch':
          if (this.state !== 'ready') throw new Error('调试会话不能重复 launch');
          startsSession = true;
          await this.launch(args, signal, false, request.seq);
          break;
        case 'cancel': {
          const task = this.requests.get(args.requestId === undefined ? this.progressRequests.get(args.progressId) : args.requestId);
          if (task && task !== controller) task.abort();
          break;
        }
        case 'restart': {
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
          if ((args.filters || []).length) throw new Error('运行期错误过滤器尚未接入');
          break;
        case 'configurationDone':
          if (this.state !== 'configuring') throw new Error('调试配置已经完成或当前不能启动程序');
          this.state = 'running';
          this.invalidate();
          try { await this.gdb.command('-exec-run', { signal }); }
          catch (error) { if (error.gdbError) this.state = 'configuring'; else this.end(); throw error; }
          break;
        case 'threads': {
          const data = await this.gdb.command('-thread-info', { signal });
          body.threads = (data.threads || []).map((thread) => ({ id: Number(thread.id), name: thread.name || thread['target-id'] || `线程 ${thread.id}` }));
          break;
        }
        case 'stackTrace': {
          await this.metadata?.load(signal);
          this.stopped(epoch);
          const thread = args.threadId === undefined ? 1 : args.threadId;
          const start = args.startFrame === undefined ? 0 : args.startFrame;
          const levels = args.levels === undefined || args.levels === 0 ? 50 : args.levels;
          if (!Number.isSafeInteger(thread) || thread < 1 || !Number.isSafeInteger(start) || start < 0 || !Number.isSafeInteger(levels) || levels < 1 || levels > 1000 || !Number.isSafeInteger(start + levels)) throw new Error('调用栈线程或分页参数无效');
          const data = await this.gdb.command(`-stack-list-frames --thread ${thread} ${start} ${start + levels - 1}`, { signal });
          this.stopped(epoch);
          body.stackFrames = (data.stack || []).map((entry) => {
            const frame = entry.frame || entry;
            const id = this.nextHandle++;
            const sourceInfo = this.metadata?.frame(frame);
            this.frames.set(id, { thread, level: Number(frame.level), native: frame, metadata: sourceInfo });
            return { id, name: sourceInfo?.name || frame.func || '<native>', line: Number(frame.line) || 1, column: sourceInfo?.column || Number(frame.col) || 1,
              source: frame.fullname || frame.file ? { path: frame.fullname || frame.file, name: path.basename(frame.fullname || frame.file) } : undefined,
              instructionPointerReference: frame.addr };
          });
          body.totalFrames = body.stackFrames.length;
          break;
        }
        case 'scopes': {
          const frame = this.frames.get(args.frameId);
          if (!frame) throw new Error('调用帧已经过期');
          body.scopes = [{ name: '局部变量与参数', presentationHint: 'locals', expensive: false, variablesReference: this.handle(frame) }];
          for (const macro of frame.metadata?.macros || []) {
            body.scopes.push({ name: '$' + macro.name, expensive: false,
              variablesReference: this.handle({ ...frame, macro: macro.id }),
              source: { path: macro.definition.file, name: path.basename(macro.definition.file) },
              line: macro.definition.line || 1, column: macro.definition.column || 1 });
          }
          break;
        }
        case 'variables': {
          const target = this.handles.get(args.variablesReference);
          if (!target || target.kind === 'memory') throw new Error('变量引用已经过期或类别无效');
          if (target.kind === 'value') {
            const memory = await this.memoryFor(target.thread, signal);
            const decoded = await decode(target.layout, target.address, memory, { start: args.start, count: args.count === undefined ? 100 : args.count });
            body.variables = decoded.children.map((item) => {
              const reference = item.childLayout && item.address > 0n
                ? this.handle({ kind: 'value', thread: target.thread, address: item.address, layout: item.childLayout }) : 0;
              const memoryReference = item.childLayout && item.address > 0n
                ? 'chali-memory:' + this.handle({ kind: 'memory', thread: target.thread, address: item.address }) : undefined;
              return { name: item.name, value: item.value, type: item.type, variablesReference: reference, memoryReference };
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
            if (!binding && variable.name.startsWith('__chali_macro_debug_')) return false;
            if ((binding?.macro ?? -1) !== (frame.macro ?? -1)) return false;
            if (visible.has(variable.name)) return false;
            visible.add(variable.name); return true;
          });
          body.variables = await Promise.all(rows.map(async (variable) => {
            const value = /^(?:int8|uint8)$/.test(variable.type || '') ? (variable.value || '').replace(/^(-?[0-9]+)\s+'.*'$/, '$1') : variable.value;
            const binding = bindings.get(variable.name);
            const numeric = /^(?:0x[0-9a-f]+|[0-9]+)$/i.test(value || '') ? BigInt(value) : null;
            const addressable = frame.level === 0 && binding?.address === true && numeric !== null && numeric > 0n;
            const layout = addressable ? this.layoutAt(binding.layout) : null;
            let display = value || '<不可用>';
            let memoryReference;
            let variablesReference = 0;
            if (addressable) {
              memoryReference = 'chali-memory:' + this.handle({ kind: 'memory', thread: frame.thread, address: numeric });
              if (layout) {
                try {
                  const memory = await this.memoryFor(frame.thread, signal);
                  display = (await decode(layout, numeric, memory, { count: 0 })).text;
                } catch { /* 目录不可用时保留原始槽值，内存引用仍可单独读取。 */ }
                if (layout && expandable(layout) && layout.kind !== 's')
                  variablesReference = this.handle({ kind: 'value', thread: frame.thread, address: numeric, layout });
              }
            }
            return { name: binding?.name || variable.name, value: display, type: variable.type, evaluateName: variable.name, variablesReference, memoryReference };
          }));
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
          const snapshot = await memory.snapshot(reference.thread, signal);
          const address = reference.address + BigInt(offset);
          const data = await snapshot.read(address, args.count);
          body = { address: '0x' + address.toString(16), data: data.toString('base64'), unreadableBytes: 0 };
          break;
        }
        case 'evaluate': {
          const frame = this.frames.get(args.frameId);
          if (args.frameId !== undefined && !frame) throw new Error('调用帧已经过期');
          const selector = frame ? ` --thread ${frame.thread} --frame ${frame.level}` : '';
          const data = await this.gdb.command('-data-evaluate-expression' + selector + ' ' + JSON.stringify(args.expression), { signal });
          body = { result: data.value || '', variablesReference: 0 };
          break;
        }
        case 'continue':
        case 'next':
        case 'stepIn':
        case 'stepOut': {
          this.stopped();
          this.state = 'running'; this.invalidate();
          const commands = { continue: '-exec-continue', next: '-exec-next', stepIn: '-exec-step', stepOut: '-exec-finish' };
          this.stepCommand = request.command === 'continue' ? null : commands[request.command];
          this.stepAttempts = 0;
          this.suppressContinued = false;
          try { await this.gdb.command(commands[request.command], { signal }); }
          catch (error) {
            if (error.gdbError && this.state === 'running') this.state = 'stopped';
            else if (!this.restartPending) this.end();
            throw error;
          }
          if (request.command === 'continue') body.allThreadsContinued = true;
          break;
        }
        case 'pause':
          if (this.state !== 'running') throw new Error('程序当前没有运行');
          await this.gdb.command('-exec-interrupt', { signal });
          break;
        case 'disconnect':
        case 'terminate':
          this.state = 'ending';
          for (const task of this.requests.values()) if (task !== controller) task.abort();
          await this.shutdown();
          this.end();
          break;
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
