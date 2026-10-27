'use strict';

/** ChaLi 原生 DAP：编译带 DWARF 的程序，通过 GDB/MI 调试真实机器码。 */
const fs = require('node:fs');
const path = require('node:path');
const { spawn } = require('node:child_process');
const { EventEmitter } = require('node:events');

/** 解析 GDB/MI 的字符串、元组和结果列表，保留重复键列表。 */
function parseMi(text) {
  let at = 0;
  function string() {
    at++;
    const bytes = [];
    while (at < text.length) {
      const c = text[at++];
      if (c === '"') break;
      if (c !== '\\') { bytes.push(...Buffer.from(c)); continue; }
      const escaped = text[at++];
      if (/[0-7]/.test(escaped)) {
        let digits = escaped;
        while (digits.length < 3 && /[0-7]/.test(text[at] || 'x')) digits += text[at++];
        bytes.push(parseInt(digits, 8));
      } else bytes.push(...Buffer.from(({ n: '\n', r: '\r', t: '\t', b: '\b', f: '\f' })[escaped] || escaped));
    }
    return Buffer.from(bytes).toString('utf8');
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
      if (text[at] === close) at++;
      return result;
    }
    const start = at;
    while (at < text.length && !',]}'.includes(text[at])) at++;
    return text.slice(start, at);
  }
  return item();
}

/** 为一次调试会话管理 MI 请求、异步停止和目标输出。 */
class Gdb extends EventEmitter {
  constructor(executable, cwd, env) {
    super();
    this.pending = new Map();
    this.serial = 1;
    this.buffer = '';
    this.process = spawn(executable, ['--quiet', '--nx', '--interpreter=mi2'], { cwd, env, windowsHide: true });
    this.process.stdin.on('error', (error) => this.fail(error));
    this.process.stdout.setEncoding('utf8');
    this.process.stdout.on('data', (chunk) => {
      this.buffer += chunk;
      let end;
      while ((end = this.buffer.indexOf('\n')) >= 0) {
        const line = this.buffer.slice(0, end).replace(/\r$/, '');
        this.buffer = this.buffer.slice(end + 1);
        this.line(line);
      }
    });
    this.process.stderr.on('data', (chunk) => this.emit('output', chunk.toString(), 'stderr'));
    this.process.on('error', (error) => { this.closed = true; this.fail(new Error(`无法启动原生调试器 ${executable}：${error.message}`)); this.emit('exit'); });
    this.process.on('exit', () => { this.closed = true; this.fail(new Error('GDB 已退出')); this.emit('exit'); });
  }
  fail(error) {
    for (const entry of this.pending.values()) { clearTimeout(entry.timer); entry.reject(error); }
    this.pending.clear();
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
        if (status === 'error') entry.reject(new Error(result.msg || 'GDB 请求失败'));
        else entry.resolve(result);
      }
    } else if (kind === '*' && status === 'stopped') this.emit('stopped', result);
  }
  command(command) {
    if (this.closed) return Promise.reject(new Error('GDB 已退出'));
    return new Promise((resolve, reject) => {
      const token = this.serial++;
      const timer = setTimeout(() => { this.pending.delete(token); reject(new Error('GDB 请求超时：' + command)); }, 20000);
      this.pending.set(token, { resolve, reject, timer });
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
    this.buffer = Buffer.alloc(0);
    this.sequence = 1;
    this.handles = new Map();
    this.frames = new Map();
    this.breakpoints = new Map();
    this.nextHandle = 1;
    this.terminated = false;
    input.on('data', (chunk) => {
      this.buffer = Buffer.concat([this.buffer, chunk]);
      while (true) {
        const end = this.buffer.indexOf('\r\n\r\n');
        if (end < 0) break;
        const match = /Content-Length:\s*(\d+)/i.exec(this.buffer.subarray(0, end).toString());
        if (!match) { this.buffer = Buffer.alloc(0); break; }
        const size = Number(match[1]);
        if (this.buffer.length < end + 4 + size) break;
        const request = JSON.parse(this.buffer.subarray(end + 4, end + 4 + size).toString());
        this.buffer = this.buffer.subarray(end + 4 + size);
        void this.request(request);
      }
    });
    input.on('end', () => { if (this.gdb) void this.gdb.command('-gdb-exit').catch(() => {}); });
  }
  send(value) {
    const data = Buffer.from(JSON.stringify({ seq: this.sequence++, ...value }));
    this.output.write(`Content-Length: ${data.length}\r\n\r\n`); this.output.write(data);
  }
  event(event, body = {}) { this.send({ type: 'event', event, body }); }
  end() {
    if (this.terminated) return;
    this.terminated = true;
    this.event('terminated');
  }
  handle(value) { const id = this.nextHandle++; this.handles.set(id, value); return id; }
  async launch(config) {
    const cwd = config.cwd || path.dirname(config.program);
    const root = temporaryRoot(cwd);
    fs.mkdirSync(root, { recursive: true });
    this.directory = fs.mkdtempSync(path.join(root, 'native-debug-'));
    const env = { ...process.env, ...config.env, TMP: root, TEMP: root, TMPDIR: root };
    const prefix = path.join(this.directory, 'program');
    await new Promise((resolve, reject) => {
      this.compilerProcess = spawn(this.compiler, ['aot', config.program, '--checked', '--debug', '--out', prefix], { cwd, env, windowsHide: true, detached: process.platform !== 'win32' });
      this.compilerProcess.stdout.on('data', (chunk) => this.event('output', { category: 'console', output: chunk.toString() }));
      this.compilerProcess.stderr.on('data', (chunk) => this.event('output', { category: 'stderr', output: chunk.toString() }));
      this.compilerProcess.on('error', reject);
      this.compilerProcess.on('exit', (code) => code === 0 ? resolve() : reject(new Error(`AOT 调试构建失败，退出码 ${code}`)));
    });
    this.gdb = new Gdb(config.debuggerPath || process.env.CHALI_GDB || 'gdb', cwd, env);
    this.gdb.on('output', (output, category) => this.event('output', { output, category }));
    this.gdb.on('exit', () => this.end());
    this.gdb.on('stopped', (data) => {
      if ((data.reason || '').startsWith('exited')) {
        this.event('exited', { exitCode: parseInt(data['exit-code'] || '0', 8) }); this.end(); return;
      }
      this.frames.clear(); this.handles.clear();
      this.event('stopped', { reason: data.reason === 'breakpoint-hit' ? 'breakpoint' : data.reason === 'signal-received' ? 'exception' : 'step',
        threadId: Number(data['thread-id']) || 1, allThreadsStopped: true, description: data['signal-meaning'] });
    });
    await this.gdb.command('-gdb-set pagination off');
    await this.gdb.command('-gdb-set confirm off');
    await this.gdb.command('-gdb-set mi-async on');
    await this.gdb.command('-file-exec-and-symbols ' + JSON.stringify(prefix + (process.platform === 'win32' ? '.exe' : '')));
    if (Array.isArray(config.args) && config.args.length) await this.gdb.command('-exec-arguments ' + config.args.map((value) => JSON.stringify(String(value))).join(' '));
    if (config.stopOnEntry) await this.gdb.command('-break-insert -t f_main');
    this.event('initialized');
  }
  async request(request) {
    const args = request.arguments || {};
    let body = {};
    try {
      switch (request.command) {
        case 'initialize': body = { supportsConfigurationDoneRequest: true, supportsEvaluateForHovers: true, supportsTerminateRequest: true }; break;
        case 'launch': await this.launch(args); break;
        case 'setBreakpoints': {
          const source = path.resolve(args.source.path);
          const previous = this.breakpoints.get(source) || [];
          if (previous.length) await this.gdb.command('-break-delete ' + previous.join(' '));
          const ids = [];
          body.breakpoints = [];
          for (const point of args.breakpoints || []) {
            try {
              const result = await this.gdb.command('-break-insert -f ' + JSON.stringify(source.replace(/\\/g, '/') + ':' + point.line));
              const bp = result.bkpt;
              ids.push(bp.number);
              body.breakpoints.push({ id: Number(bp.number), verified: !bp.pending, line: Number(bp.line) || point.line, source: args.source });
            } catch (error) { body.breakpoints.push({ verified: false, line: point.line, message: error.message }); }
          }
          this.breakpoints.set(source, ids);
          break;
        }
        case 'setExceptionBreakpoints': break;
        case 'configurationDone': await this.gdb.command('-exec-run'); break;
        case 'threads': {
          const data = await this.gdb.command('-thread-info');
          body.threads = (data.threads || []).map((thread) => ({ id: Number(thread.id), name: thread.name || thread['target-id'] || `线程 ${thread.id}` }));
          break;
        }
        case 'stackTrace': {
          const data = await this.gdb.command(`-stack-list-frames --thread ${args.threadId || 1} ${args.startFrame || 0} ${(args.startFrame || 0) + (args.levels || 50) - 1}`);
          body.stackFrames = (data.stack || []).map((entry) => {
            const frame = entry.frame || entry;
            const id = this.nextHandle++;
            this.frames.set(id, { thread: args.threadId || 1, level: Number(frame.level) });
            return { id, name: frame.func || '<native>', line: Number(frame.line) || 1, column: 1,
              source: frame.fullname || frame.file ? { path: frame.fullname || frame.file, name: path.basename(frame.fullname || frame.file) } : undefined,
              instructionPointerReference: frame.addr };
          });
          body.totalFrames = body.stackFrames.length;
          break;
        }
        case 'scopes': {
          const frame = this.frames.get(args.frameId);
          body.scopes = frame ? [{ name: '局部变量与参数', presentationHint: 'locals', expensive: false, variablesReference: this.handle(frame) }] : [];
          break;
        }
        case 'variables': {
          const frame = this.handles.get(args.variablesReference);
          if (!frame) { body.variables = []; break; }
          const data = await this.gdb.command(`-stack-list-variables --thread ${frame.thread} --frame ${frame.level} --simple-values`);
          body.variables = (data.variables || []).map((variable) => ({ name: variable.name, value: variable.value || '<不可用>', type: variable.type, evaluateName: variable.name, variablesReference: 0 }));
          break;
        }
        case 'evaluate': {
          const frame = this.frames.get(args.frameId);
          const selector = frame ? ` --thread ${frame.thread} --frame ${frame.level}` : '';
          const data = await this.gdb.command('-data-evaluate-expression' + selector + ' ' + JSON.stringify(args.expression));
          body = { result: data.value || '', variablesReference: 0 };
          break;
        }
        case 'continue': await this.gdb.command('-exec-continue'); body.allThreadsContinued = true; break;
        case 'next': await this.gdb.command('-exec-next'); break;
        case 'stepIn': await this.gdb.command('-exec-step'); break;
        case 'stepOut': await this.gdb.command('-exec-finish'); break;
        case 'pause': await this.gdb.command('-exec-interrupt'); break;
        case 'disconnect':
        case 'terminate':
          if (this.compilerProcess && this.compilerProcess.exitCode === null) {
            if (process.platform === 'win32') spawn('taskkill', ['/PID', String(this.compilerProcess.pid), '/T', '/F'], { windowsHide: true, stdio: 'ignore' });
            else { try { process.kill(-this.compilerProcess.pid, 'SIGTERM'); } catch { this.compilerProcess.kill(); } }
          }
          if (this.gdb) await this.gdb.command('-gdb-exit').catch(() => {});
          this.end();
          break;
        default: throw new Error('原生调试暂不支持请求：' + request.command);
      }
      this.send({ type: 'response', request_seq: request.seq, command: request.command, success: true, body });
    } catch (error) {
      this.send({ type: 'response', request_seq: request.seq, command: request.command, success: false, message: error.message });
      if (request.command === 'launch') this.end();
    }
  }
}

if (require.main === module) new Adapter(process.argv[2]);
module.exports = { Adapter, Gdb, parseMi, temporaryRoot };
