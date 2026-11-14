#!/usr/bin/env node
// Prepare the installable plugin: regenerate the embedded skill and copy the
// real ChaLi compiler into bin/ so the installed package carries its own
// toolchain. Run from the plugin directory:
//   node scripts/package.mjs
//
// The compiler is a large binary and stays ignored by Git; the generated
// bin/compiler.json records what was copied for local verification.

import { createHash } from 'node:crypto'
import { createReadStream } from 'node:fs'
import { copyFile, mkdir, readFile, readdir, stat, writeFile, chmod } from 'node:fs/promises'
import { delimiter, dirname, isAbsolute, join, resolve } from 'node:path'
import { fileURLToPath } from 'node:url'
import { spawnSync } from 'node:child_process'

const here = dirname(fileURLToPath(import.meta.url))
const pluginRoot = resolve(here, '..')
const repoRoot = resolve(pluginRoot, '..', '..')

const build = spawnSync(process.execPath, [join(here, 'build.mjs')], { stdio: 'inherit' })
if (build.status !== 0) process.exit(build.status ?? 1)

const source = await locateCompiler()
if (source === undefined) {
  console.error('dsh-chali: no ChaLi compiler found. Set CHALI_COMPILER or build vscode/bin/chali.exe first.')
  process.exit(1)
}

const binDir = join(pluginRoot, 'bin')
await mkdir(binDir, { recursive: true })
const filename = process.platform === 'win32' ? 'chali.exe' : 'chali'
const destination = join(binDir, filename)

const sourceInfo = await stat(source)
const existing = await stat(destination).catch(() => undefined)
if (existing === undefined || existing.size !== sourceInfo.size || existing.mtimeMs < sourceInfo.mtimeMs) {
  await copyFile(source, destination)
  if (process.platform !== 'win32') await chmod(destination, 0o755)
  console.log(`copied ${source} -> ${destination} (${formatBytes(sourceInfo.size)})`)
} else {
  console.log(`kept ${destination} (${formatBytes(existing.size)})`)
}

const sha256 = await hashFile(destination)
const version = await readVersion()
const metadata = {
  name: 'chali',
  version,
  source,
  file: `bin/${filename}`,
  bytes: (await stat(destination)).size,
  sha256,
  packagedAt: new Date().toISOString(),
}
await writeFile(join(binDir, 'compiler.json'), JSON.stringify(metadata, undefined, 2) + '\n', 'utf8')
console.log(`bundled ChaLi ${version} (${formatBytes(metadata.bytes)}, sha256 ${sha256})`)

async function locateCompiler() {
  for (const name of ['CHALI_COMPILER', 'CHALI_BIN']) {
    const value = process.env[name]
    if (typeof value === 'string' && value.trim() !== '') {
      const candidate = isAbsolute(value) ? value : resolve(process.cwd(), value)
      if (await isFile(candidate)) return candidate
      throw new Error(`${name} points to a missing file: ${candidate}`)
    }
  }

  const version = await readVersion()
  const platform = process.platform === 'win32' ? 'windows' : process.platform === 'darwin' ? 'darwin' : 'linux'
  const arch = process.arch === 'x64' ? 'x64' : process.arch === 'arm64' ? 'arm64' : process.arch
  const direct = [
    join(repoRoot, 'vscode', 'bin', 'chali.exe'),
    join(repoRoot, 'vscode', 'bin', 'chali'),
    join(repoRoot, 'chali.exe'),
    join(repoRoot, 'ChaLi.exe'),
    join(repoRoot, 'chali'),
    join(repoRoot, `chali-${version}-llvm-static-${platform}-${arch}${platform === 'windows' ? '.exe' : ''}`),
    join(repoRoot, `chali-${version}-llvm-external-${platform}-${arch}${platform === 'windows' ? '.exe' : ''}`),
  ]
  for (const candidate of direct) if (await isFile(candidate)) return candidate

  try {
    const entries = await readdir(repoRoot, { withFileTypes: true })
    const matches = entries
      .filter((entry) => entry.isFile() && /^chali-.*-(?:windows|linux|darwin)-.*(?:\.exe)?$/i.test(entry.name))
      .map((entry) => join(repoRoot, entry.name))
      .sort()
    for (const candidate of matches) if (await isFile(candidate)) return candidate
  } catch {
    // Fall through to PATH.
  }

  const names = process.platform === 'win32' ? ['chali.exe', 'ChaLi.exe'] : ['chali', 'ChaLi']
  for (const directory of (process.env.PATH ?? '').split(delimiter)) {
    if (directory === '') continue
    for (const name of names) {
      const candidate = join(directory, name)
      if (await isFile(candidate)) return candidate
    }
  }
  return undefined
}

async function readVersion() {
  try {
    return (await readFile(join(repoRoot, 'VERSION'), 'utf8')).trim()
  } catch {
    return 'unknown'
  }
}

async function isFile(file) {
  try {
    return (await stat(file)).isFile()
  } catch {
    return false
  }
}

async function hashFile(file) {
  const hash = createHash('sha256')
  for await (const chunk of createReadStream(file, { highWaterMark: 1024 * 1024 })) hash.update(chunk)
  return hash.digest('hex')
}

function formatBytes(bytes) {
  return `${(bytes / (1024 * 1024)).toFixed(1)} MiB`
}
