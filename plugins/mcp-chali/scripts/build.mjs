#!/usr/bin/env node
import { copyFile, mkdir, readFile, stat, writeFile } from 'node:fs/promises'
import { createHash } from 'node:crypto'
import { dirname, join, resolve, isAbsolute, delimiter } from 'node:path'
import { spawnSync } from 'node:child_process'
import { fileURLToPath } from 'node:url'

const root = resolve(dirname(fileURLToPath(import.meta.url)), '..')
const repo = resolve(root, '..', '..')
const source = process.env.CHALI_SKILL_PATH ? resolve(process.env.CHALI_SKILL_PATH) : join(repo, 'skills', 'chali-programming', 'SKILL.md')
await mkdir(join(root, 'skill-source'), { recursive: true })
await copyFile(source, join(root, 'skill-source', 'SKILL.md'))
const raw = await readFile(source, 'utf8')
const match = raw.match(/^---\r?\n([\s\S]*?)\r?\n---\r?\n?([\s\S]*)$/)
if (!match) throw new Error(`${source}: missing YAML frontmatter`)
const desc = match[1].split(/\r?\n/).find((line) => line.startsWith('description:'))?.slice(12).trim().replace(/^['"]|['"]$/g, '')
if (!desc) throw new Error(`${source}: missing description`)
await mkdir(join(root, 'generated'), { recursive: true })
await writeFile(join(root, 'generated', 'skill-content.mjs'), `// Generated from skill-source/SKILL.md.\nexport const skillDescription = ${JSON.stringify(desc)}\nexport const skillContent = ${JSON.stringify(match[2].trim() + '\\n')}\n`)
console.log('generated skill-content.mjs and copied skill-source/SKILL.md')
