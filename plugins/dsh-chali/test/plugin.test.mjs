// Registration and toolchain smoke tests for dsh-chali.
//
// Run from the plugin directory: node --test test/plugin.test.mjs
// The toolchain cases call the bundled compiler (bin/chali.exe), so they also
// prove the package is self-contained after `npm run package`.

import assert from 'node:assert/strict'
import { mkdtemp, readFile, rm, stat } from 'node:fs/promises'
import { tmpdir } from 'node:os'
import { dirname, join, resolve } from 'node:path'
import test from 'node:test'
import { fileURLToPath, pathToFileURL } from 'node:url'

const pluginRoot = resolve(dirname(fileURLToPath(import.meta.url)), '..')
const repoRoot = resolve(pluginRoot, '..', '..')
const plugin = await import(pathToFileURL(join(pluginRoot, 'lib', 'index.js')).href)

function fakeContext() {
  const skills = []
  const tools = []
  return {
    skills: { register: (definition) => skills.push(definition) },
    tools: { register: (definition) => tools.push(definition) },
    workingDirectory: { ensure: async () => repoRoot },
    skillsList: skills,
    toolsList: tools,
  }
}

function tool(ctx, name) {
  const found = ctx.toolsList.find((definition) => definition.name === name)
  assert.ok(found, `missing ${name}`)
  return found
}

test('registers the ChaLi skill and four tools on the host layer', () => {
  const ctx = fakeContext()
  plugin.apply(ctx, { workspaceRoot: repoRoot })

  assert.equal(plugin.name, 'dsh-chali')
  assert.deepEqual(plugin.inject, ['tools', 'skills', 'workingDirectory'])
  assert.equal(ctx.skillsList.length, 1)
  assert.equal(ctx.skillsList[0].name, 'chali-programming')
  assert.match(ctx.skillsList[0].content, /dsh-chali/)
  assert.deepEqual(
    ctx.toolsList.map((definition) => definition.name),
    ['chali_write', 'chali_run', 'chali_man', 'chali_new'],
  )
})

test('bundles the real compiler', async () => {
  const binary = join(pluginRoot, 'bin', 'chali.exe')
  const info = await stat(binary)
  assert.ok(info.size > 100 * 1024 * 1024, `compiler too small: ${info.size}`)
  const metadata = JSON.parse(await readFile(join(pluginRoot, 'bin', 'compiler.json'), 'utf8'))
  assert.equal(metadata.version, '1.0.0')
  assert.equal(metadata.bytes, info.size)
  assert.match(metadata.sha256, /^[0-9a-f]{64}$/)
})

test('writes, checks, and runs a ChaLi program with the bundled compiler', async () => {
  const ctx = fakeContext()
  plugin.apply(ctx, { workspaceRoot: repoRoot })
  const workDir = await mkdtemp(join(repoRoot, 'tmp', 'dsh-chali-test-'))
  try {
    const relative = join('tmp', workDir.split(/[\\/]/).pop(), 'hello.chl')
    const source = [
      'void main() {',
      '    int64 total = 0;',
      '    int64 i = 1;',
      '    while (i <= 10) {',
      '        total = total + i;',
      '        i = i + 1;',
      '    }',
      '    assert(total == 55);',
      '    println("total={}", total);',
      '}',
      '',
    ].join('\n')

    const write = tool(ctx, 'chali_write')
    const written = await write.execute({ path: relative, code: source }, { agent: undefined, signal: undefined })
    assert.equal(written.checked, true)
    assert.equal(written.ok, true, written.stderr)
    assert.equal(written.bytes, Buffer.byteLength(source))
    assert.equal(resolve(written.compiler), resolve(pluginRoot, 'bin', 'chali.exe'))

    const run = tool(ctx, 'chali_run')
    const result = await run.execute({ path: relative, mode: 'run' }, { agent: undefined, signal: undefined })
    assert.equal(result.ok, true, result.stderr)
    assert.match(result.stdout, /total=55/)
  } finally {
    await rm(workDir, { recursive: true, force: true })
  }
})

test('chali_man reads the embedded manual', async () => {
  const ctx = fakeContext()
  plugin.apply(ctx, { workspaceRoot: repoRoot })
  const man = tool(ctx, 'chali_man')
  const result = await man.execute({ query: ['keyword', 'break'] }, { agent: undefined, signal: undefined })
  assert.equal(result.ok, true, result.stderr)
  assert.match(result.stdout, /break|退出|循环/)
})
