// dsh-chali — DeepSeek Harness plugin for writing ChaLi programs.
//
// The plugin registers one embedded skill (chali-programming) and four
// host-plane tools. Tools are registered on the global tools layer, so every
// agent preset inherits them; the skill is registered on the global skills
// layer for the same reason. The module deliberately imports only Node
// built-ins: it can be loaded from a profile link, an absolute path, or an
// installed package without a peer-resolution setup.

import { spawn } from 'node:child_process'
import { lstat, mkdir, readdir, stat, writeFile } from 'node:fs/promises'
import {
  delimiter,
  dirname,
  isAbsolute,
  join,
  normalize,
  relative,
  resolve,
} from 'node:path'
import { fileURLToPath } from 'node:url'

import { skillContent, skillDescription } from './skill-content.js'

export const name = 'dsh-chali'
export const inject = ['tools', 'skills', 'workingDirectory']

const PLUGIN_ROOT = dirname(dirname(fileURLToPath(import.meta.url)))

const DEFAULT_TIMEOUT_MS = 120_000
const MAX_TIMEOUT_MS = 900_000
const DEFAULT_MAX_OUTPUT_CHARS = 12_000

const COMPILER_RELATIVE_CANDIDATES = [
  join('vscode', 'bin', 'chali.exe'),
  join('vscode', 'bin', 'chali'),
  'chali.exe',
  'ChaLi.exe',
  'chali',
  join('build', 'chali.exe'),
  join('build', 'chali'),
  join('dist', 'chali.exe'),
  join('dist', 'chali'),
]

/**
 * Register the ChaLi skill and tools on the calling (host) context.
 * @param ctx - Cordis context carrying `tools`, `skills`, and `workingDirectory`.
 * @param config - optional row configuration (compiler, workspaceRoot, limits).
 */
export function apply(ctx, config = {}) {
  const settings = normalizeSettings(config)

  ctx.skills.register({
    name: 'chali-programming',
    description: skillDescription,
    content: skillContent,
    invocation: { modelInvocable: true, userInvocable: true },
    provider: 'dsh-chali',
    source: 'runtime',
  })

  for (const definition of createTools(ctx, settings)) ctx.tools.register(definition)
}

/**
 * Validate row configuration once at mount time.
 * @param config - raw row configuration.
 * @returns normalized settings used by every tool call.
 */
function normalizeSettings(config) {
  const compiler =
    typeof config.compiler === 'string' && config.compiler.trim() !== ''
      ? config.compiler.trim()
      : undefined
  const workspaceRoot =
    typeof config.workspaceRoot === 'string' && config.workspaceRoot.trim() !== ''
      ? resolve(config.workspaceRoot.trim())
      : undefined
  return {
    compiler,
    workspaceRoot,
    allowOutsideWorkspace: config.allowOutsideWorkspace === true,
    checkOnWrite: config.checkOnWrite !== false,
    timeoutMs: positiveInteger(config.timeoutMs, DEFAULT_TIMEOUT_MS, MAX_TIMEOUT_MS),
    maxOutputChars: positiveInteger(config.maxOutputChars, DEFAULT_MAX_OUTPUT_CHARS, 200_000),
  }
}

/**
 * Build the model-facing tool definitions.
 * @param ctx - host context.
 * @param settings - normalized row configuration.
 * @returns tool definitions to register.
 */
function createTools(ctx, settings) {
  return [
    writeTool(ctx, settings),
    runTool(ctx, settings),
    manTool(ctx, settings),
    newTool(ctx, settings),
  ]
}

function writeTool(ctx, settings) {
  return {
    name: 'chali_write',
    description:
      'Write a complete ChaLi (.chl) source file into the session workspace and, by default, immediately type-check it with the real ChaLi compiler. Use this instead of the generic write tool when creating ChaLi code: it uses the compiler bundled in this plugin (bin/chali.exe) unless CHALI_COMPILER or the row compiler field overrides it, writes UTF-8, and returns the compiler diagnostics. Paths are relative to the session working directory unless allowOutsideWorkspace is configured.',
    parameters: {
      type: 'object',
      additionalProperties: false,
      properties: {
        path: {
          type: 'string',
          description: 'Target file, usually ending in .chl. Relative paths resolve against the session working directory.',
        },
        code: {
          type: 'string',
          description: 'Complete ChaLi source text. Do not include Markdown fences.',
        },
        check: {
          type: 'boolean',
          description: 'Run "chali check" after writing. Defaults to true; pass false only when the toolchain is unavailable.',
        },
      },
      required: ['path', 'code'],
    },
    output: {
      schema: {
        type: 'object',
        additionalProperties: false,
        properties: {
          path: { type: 'string' },
          bytes: { type: 'integer' },
          compiler: { type: 'string' },
          checked: { type: 'boolean' },
          ok: { type: 'boolean' },
          exitCode: { type: 'integer' },
          timedOut: { type: 'boolean' },
          stdout: { type: 'string' },
          stderr: { type: 'string' },
        },
        required: ['path', 'bytes', 'checked'],
      },
      render: (_args, value) => [{ type: 'text', text: renderWriteResult(value) }],
    },
    async execute(args, exec) {
      const cwd = await sessionCwd(ctx, exec, settings)
      const file = targetPath(args.path, cwd, settings)
      const bytes = Buffer.byteLength(args.code, 'utf8')
      await mkdir(dirname(file), { recursive: true })
      await assertWritableFile(file)
      await writeFile(file, args.code, 'utf8')

      if (args.check === false || settings.checkOnWrite === false) {
        return {
          path: file,
          bytes,
          compiler: '',
          checked: false,
          ok: false,
          exitCode: -1,
          timedOut: false,
          stdout: '',
          stderr: '',
        }
      }

      const check = await checkFile(file, cwd, settings, exec.signal)
      return {
        path: file,
        bytes,
        compiler: check.compiler ?? '',
        checked: true,
        ok: check.ok,
        exitCode: check.exitCode,
        timedOut: check.timedOut,
        stdout: check.stdout,
        stderr: check.stderr,
      }
    },
  }
}

function runTool(ctx, settings) {
  return {
    name: 'chali_run',
    description:
      'Check, run, or build an existing ChaLi (.chl) file with the real toolchain. mode=check type-checks only; mode=run compiles checked native code and captures stdout/stderr; mode=build produces a native executable (unchecked by default) and reports the artifact when --out names one. Use it after chali_write, and after changing code, to verify behavior instead of guessing.',
    parameters: {
      type: 'object',
      additionalProperties: false,
      properties: {
        path: {
          type: 'string',
          description: 'Existing .chl file, relative to the session working directory or absolute inside it.',
        },
        mode: {
          type: 'string',
          enum: ['check', 'run', 'build'],
          description: 'Toolchain scenario. Defaults to run.',
        },
        args: {
          type: 'array',
          items: { type: 'string' },
          description: 'Program arguments for mode=run. Passed after the source file.',
        },
        out: {
          type: 'string',
          description: 'Output prefix for mode=build, as accepted by "chali build --out".',
        },
        timeoutMs: {
          type: 'integer',
          description: 'Wall-clock limit in milliseconds, at most 900000.',
        },
      },
      required: ['path'],
    },
    output: {
      schema: {
        type: 'object',
        additionalProperties: false,
        properties: {
          mode: { type: 'string' },
          path: { type: 'string' },
          compiler: { type: 'string' },
          ok: { type: 'boolean' },
          exitCode: { type: 'integer' },
          timedOut: { type: 'boolean' },
          artifact: { type: 'string' },
          stdout: { type: 'string' },
          stderr: { type: 'string' },
        },
        required: ['mode', 'path', 'ok', 'exitCode'],
      },
      render: (_args, value) => [{ type: 'text', text: renderRunResult(value) }],
    },
    async execute(args, exec) {
      const cwd = await sessionCwd(ctx, exec, settings)
      const file = targetPath(args.path, cwd, settings)
      const info = await stat(file).catch(() => undefined)
      if (info === undefined || !info.isFile()) throw new Error(`ChaLi source not found: ${file}`)

      const mode = args.mode ?? 'run'
      const compiler = await findCompiler(settings, cwd)
      if (compiler === undefined) throw new Error(compilerNotFoundMessage(cwd))

      const timeoutMs = clamp(
        positiveInteger(args.timeoutMs, mode === 'build' ? 300_000 : settings.timeoutMs, MAX_TIMEOUT_MS),
        1_000,
        MAX_TIMEOUT_MS,
      )
      const command = mode === 'build' ? ['build', file] : [mode, file]
      if (mode === 'build' && args.out) {
        const out = targetPath(args.out, cwd, settings)
        command.push('--out', out)
      }
      if (mode === 'run' && Array.isArray(args.args) && args.args.length > 0) {
        command.push('--', ...args.args.map(String))
      }

      const result = await runProcess(compiler, command, {
        cwd,
        timeoutMs,
        signal: exec.signal,
        maxOutputChars: settings.maxOutputChars,
      })
      const artifact = mode === 'build' && args.out ? await findArtifact(args.out, cwd, settings) : undefined
      return {
        mode,
        path: file,
        compiler,
        ok: result.exitCode === 0 && !result.timedOut,
        exitCode: result.exitCode,
        timedOut: result.timedOut,
        ...(artifact === undefined ? {} : { artifact }),
        stdout: result.stdout,
        stderr: result.stderr,
      }
    },
  }
}

function manTool(ctx, settings) {
  return {
    name: 'chali_man',
    description:
      'Query the ChaLi compiler\'s embedded manual. Use it before writing unfamiliar syntax or standard-library calls: it covers keywords, book chapters, and the complete stdlib API (for example ["keyword","break"], ["book","6"], ["api","std::text"], or ["api","std::text::Text::split"]). The result is the compiler\'s own text, not a paraphrase.',
    parameters: {
      type: 'object',
      additionalProperties: false,
      properties: {
        query: {
          type: 'array',
          items: { type: 'string' },
          description: 'Arguments after "chali man"; defaults to ["help"].',
        },
      },
    },
    output: {
      schema: {
        type: 'object',
        additionalProperties: false,
        properties: {
          query: { type: 'string' },
          compiler: { type: 'string' },
          ok: { type: 'boolean' },
          exitCode: { type: 'integer' },
          stdout: { type: 'string' },
          stderr: { type: 'string' },
        },
        required: ['query', 'ok', 'exitCode'],
      },
      render: (_args, value) => [{ type: 'text', text: renderManResult(value) }],
    },
    async execute(args, exec) {
      const cwd = await sessionCwd(ctx, exec, settings)
      const compiler = await findCompiler(settings, cwd)
      if (compiler === undefined) throw new Error(compilerNotFoundMessage(cwd))
      const query = Array.isArray(args.query) && args.query.length > 0 ? args.query.map(String) : ['help']
      const result = await runProcess(compiler, ['man', ...query], {
        cwd,
        timeoutMs: 60_000,
        signal: exec.signal,
        maxOutputChars: settings.maxOutputChars,
      })
      return {
        query: query.join(' '),
        compiler,
        ok: result.exitCode === 0 && !result.timedOut,
        exitCode: result.exitCode,
        stdout: result.stdout,
        stderr: result.stderr,
      }
    },
  }
}

function newTool(ctx, settings) {
  return {
    name: 'chali_new',
    description:
      'Create a new ChaLi application project with "chali new". The target directory must not exist. Use it when the user asks for a runnable project rather than a single source file; then write sources with chali_write and verify with chali_run.',
    parameters: {
      type: 'object',
      additionalProperties: false,
      properties: {
        dir: {
          type: 'string',
          description: 'New project directory, relative to the session working directory or absolute inside it.',
        },
        name: {
          type: 'string',
          description: 'Optional project name passed as "chali new DIR name NAME".',
        },
      },
      required: ['dir'],
    },
    output: {
      schema: {
        type: 'object',
        additionalProperties: false,
        properties: {
          path: { type: 'string' },
          compiler: { type: 'string' },
          ok: { type: 'boolean' },
          exitCode: { type: 'integer' },
          stdout: { type: 'string' },
          stderr: { type: 'string' },
        },
        required: ['path', 'ok', 'exitCode'],
      },
      render: (_args, value) => [{ type: 'text', text: renderNewResult(value) }],
    },
    async execute(args, exec) {
      const cwd = await sessionCwd(ctx, exec, settings)
      const dir = targetPath(args.dir, cwd, settings)
      const compiler = await findCompiler(settings, cwd)
      if (compiler === undefined) throw new Error(compilerNotFoundMessage(cwd))
      const command = ['new', dir]
      if (typeof args.name === 'string' && args.name.trim() !== '') command.push('name', args.name.trim())
      const result = await runProcess(compiler, command, {
        cwd,
        timeoutMs: settings.timeoutMs,
        signal: exec.signal,
        maxOutputChars: settings.maxOutputChars,
      })
      return {
        path: dir,
        compiler,
        ok: result.exitCode === 0 && !result.timedOut,
        exitCode: result.exitCode,
        stdout: result.stdout,
        stderr: result.stderr,
      }
    },
  }
}

async function checkFile(file, cwd, settings, signal) {
  const compiler = await findCompiler(settings, cwd)
  if (compiler === undefined) {
    return {
      compiler: undefined,
      ok: false,
      exitCode: -1,
      timedOut: false,
      stdout: '',
      stderr: compilerNotFoundMessage(cwd),
    }
  }
  const result = await runProcess(compiler, ['check', file], {
    cwd,
    timeoutMs: settings.timeoutMs,
    signal,
    maxOutputChars: settings.maxOutputChars,
  })
  return {
    compiler,
    ok: result.exitCode === 0 && !result.timedOut,
    exitCode: result.exitCode,
    timedOut: result.timedOut,
    stdout: result.stdout,
    stderr: result.stderr,
  }
}

async function sessionCwd(ctx, exec, settings) {
  if (exec?.agent !== undefined) {
    return await ctx.workingDirectory.ensure(exec.agent, exec.signal)
  }
  return settings.workspaceRoot ?? process.cwd()
}

function targetPath(raw, cwd, settings) {
  if (typeof raw !== 'string' || raw.trim() === '') throw new Error('path must be a non-empty string')
  const absolute = isAbsolute(raw) ? normalize(raw) : resolve(cwd, raw)
  if (!settings.allowOutsideWorkspace && !isInside(cwd, absolute)) {
    throw new Error(
      `path must stay inside the session working directory (${cwd}): ${absolute}. ` +
        'Set allowOutsideWorkspace: true on the dsh-chali row to permit paths elsewhere.',
    )
  }
  return absolute
}

function isInside(root, target) {
  const rel = relative(lower(root), lower(target))
  return rel === '' || (!rel.startsWith('..') && !isAbsolute(rel))
}

function lower(value) {
  return process.platform === 'win32' ? value.toLowerCase() : value
}

async function assertWritableFile(file) {
  try {
    const info = await lstat(file)
    if (info.isSymbolicLink()) throw new Error(`refusing to overwrite a symbolic link: ${file}`)
    if (info.isDirectory()) throw new Error(`cannot write a directory: ${file}`)
  } catch (error) {
    if (error?.code !== 'ENOENT') throw error
  }
}

async function findCompiler(settings, cwd) {
  const explicit = settings.compiler ?? process.env.CHALI_COMPILER ?? process.env.CHALI_BIN
  if (typeof explicit === 'string' && explicit.trim() !== '') {
    const candidate = isAbsolute(explicit) ? explicit : resolve(cwd, explicit)
    if (await isFile(candidate)) return candidate
    throw new Error(`ChaLi compiler not found at configured path: ${candidate}`)
  }

  // The package carries the real ChaLi compiler under bin/. Prefer it so the
  // plugin keeps working outside this repository and without a system chali.
  const bundled = await bundledCompilers()
  if (bundled.length > 0) return bundled[0]

  for (const root of ancestorRoots(cwd)) {
    for (const relativePath of COMPILER_RELATIVE_CANDIDATES) {
      const candidate = join(root, relativePath)
      if (await isFile(candidate)) return candidate
    }
    let entries
    try {
      entries = await readdir(root, { withFileTypes: true })
    } catch {
      entries = []
    }
    const release = entries
      .filter((entry) => entry.isFile() && /^chali-.*-(?:windows|linux|darwin)-.*(?:\.exe)?$/i.test(entry.name))
      .map((entry) => join(root, entry.name))
      .sort()
    for (const candidate of release) if (await isFile(candidate)) return candidate
  }

  const names = process.platform === 'win32' ? ['chali.exe', 'ChaLi.exe'] : ['chali', 'ChaLi']
  for (const directory of (process.env.PATH ?? '').split(delimiter)) {
    if (directory === '') continue
    for (const executable of names) {
      const candidate = join(directory, executable)
      if (await isFile(candidate)) return candidate
    }
  }
  return undefined
}

async function bundledCompilers() {
  const directory = join(PLUGIN_ROOT, 'bin')
  const candidates = [
    join(directory, process.platform === 'win32' ? 'chali.exe' : 'chali'),
    join(directory, 'chali.exe'),
    join(directory, 'chali'),
  ]
  try {
    const entries = await readdir(directory, { withFileTypes: true })
    for (const entry of entries) {
      if (entry.isFile() && /^chali(?:-.*)?(?:\.exe)?$/i.test(entry.name)) candidates.push(join(directory, entry.name))
    }
  } catch {
    // No bundled compiler; fall through to the repository and PATH lookup.
  }
  const found = []
  for (const candidate of candidates) if (await isFile(candidate)) found.push(candidate)
  return found
}

function ancestorRoots(cwd) {
  const roots = []
  let current = resolve(cwd)
  for (let depth = 0; depth < 6; depth += 1) {
    roots.push(current)
    const parent = dirname(current)
    if (parent === current) break
    current = parent
  }
  return roots
}

async function isFile(file) {
  try {
    return (await stat(file)).isFile()
  } catch {
    return false
  }
}

function compilerNotFoundMessage(cwd) {
  return (
    `ChaLi compiler not found. Run "npm run package" in the dsh-chali plugin to copy the real compiler into bin/, ` +
    'or set CHALI_COMPILER / configure the dsh-chali row\'s compiler field. ' +
    `The search also covered ${cwd} and its ancestors.`
  )
}

async function runProcess(command, args, options) {
  return await new Promise((resolvePromise, rejectPromise) => {
    const child = spawn(command, args, {
      cwd: options.cwd,
      env: { ...process.env, CHALI_LANG: process.env.CHALI_LANG ?? 'zh' },
      windowsHide: true,
      stdio: ['ignore', 'pipe', 'pipe'],
      signal: options.signal,
    })
    let stdout = ''
    let stderr = ''
    let timedOut = false
    let settled = false
    const finish = (result) => {
      if (settled) return
      settled = true
      clearTimeout(timer)
      resolvePromise({
        ...result,
        stdout: clip(result.stdout, options.maxOutputChars),
        stderr: clip(result.stderr, options.maxOutputChars),
      })
    }
    const fail = (error) => {
      if (settled) return
      settled = true
      clearTimeout(timer)
      rejectPromise(error)
    }
    const timer = setTimeout(() => {
      timedOut = true
      killTree(child)
    }, options.timeoutMs)
    child.stdout.setEncoding('utf8')
    child.stderr.setEncoding('utf8')
    child.stdout.on('data', (chunk) => {
      stdout += chunk
    })
    child.stderr.on('data', (chunk) => {
      stderr += chunk
    })
    child.on('error', fail)
    child.on('close', (exitCode) => {
      finish({
        exitCode: Number.isInteger(exitCode) ? exitCode : -1,
        stdout,
        stderr,
        timedOut,
      })
    })
  })
}

function killTree(child) {
  try {
    child.kill('SIGKILL')
  } catch {
    // The child may already be gone; taskkill below covers descendants.
  }
  if (process.platform === 'win32' && Number.isInteger(child.pid)) {
    try {
      const killer = spawn('taskkill', ['/pid', String(child.pid), '/T', '/F'], {
        windowsHide: true,
        stdio: 'ignore',
      })
      killer.on('error', () => {})
    } catch {
      // Best-effort cleanup only.
    }
  }
}

function clip(text, maxChars) {
  if (typeof text !== 'string' || text.length <= maxChars) return text
  const head = Math.floor(maxChars * 0.6)
  const tail = maxChars - head
  return `${text.slice(0, head)}\n...[truncated ${text.length - maxChars} characters]...\n${text.slice(-tail)}`
}

function positiveInteger(value, fallback, max) {
  if (!Number.isInteger(value) || value <= 0) return fallback
  return Math.min(value, max)
}

function clamp(value, min, max) {
  return Math.min(Math.max(value, min), max)
}

async function findArtifact(out, cwd, settings) {
  const prefix = targetPath(out, cwd, settings)
  const candidates = process.platform === 'win32' ? [prefix, `${prefix}.exe`] : [prefix]
  for (const candidate of candidates) if (await isFile(candidate)) return candidate
  return undefined
}

function renderWriteResult(value) {
  const lines = [`Wrote ${value.path} (${value.bytes} bytes).`]
  if (value.checked) {
    lines.push(`Check: ${value.ok ? 'passed' : 'failed'} (exit ${value.exitCode}${value.timedOut ? ', timed out' : ''})`)
  } else {
    lines.push('Check: skipped.')
  }
  if (value.compiler) lines.push(`Compiler: ${value.compiler}`)
  if (value.stdout) lines.push(`stdout:\n${value.stdout}`)
  if (value.stderr) lines.push(`stderr:\n${value.stderr}`)
  return lines.join('\n')
}

function renderRunResult(value) {
  const lines = [
    `${value.mode}: ${value.ok ? 'ok' : 'failed'} (exit ${value.exitCode}${value.timedOut ? ', timed out' : ''})`,
    `Source: ${value.path}`,
    `Compiler: ${value.compiler}`,
  ]
  if (value.artifact) lines.push(`Artifact: ${value.artifact}`)
  if (value.stdout) lines.push(`stdout:\n${value.stdout}`)
  if (value.stderr) lines.push(`stderr:\n${value.stderr}`)
  return lines.join('\n')
}

function renderManResult(value) {
  const lines = [`chali man ${value.query} (exit ${value.exitCode})`]
  if (value.stdout) lines.push(value.stdout)
  if (value.stderr) lines.push(`stderr:\n${value.stderr}`)
  return lines.join('\n')
}

function renderNewResult(value) {
  const lines = [`chali new ${value.path}: ${value.ok ? 'ok' : 'failed'} (exit ${value.exitCode})`]
  if (value.stdout) lines.push(value.stdout)
  if (value.stderr) lines.push(`stderr:\n${value.stderr}`)
  return lines.join('\n')
}
