'use strict';

const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { llvmOptions, variantManifest } = require('../src/llvmConfiguration');
const { buildRunCommand } = require('../src/runner');
const { buildArguments } = require('../src/build');

const directory = fs.mkdtempSync(path.join(require('./projectTemp').root, 'llvm-configuration-'));
try {
  const original = JSON.parse(fs.readFileSync(path.join(__dirname, '../package.json'), 'utf8'));
  const unchanged = JSON.stringify(original);
  fs.mkdirSync(path.join(directory, 'bin'));
  for (const variant of ['static', 'external']) {
    const manifest = variantManifest(original, variant);
    const properties = manifest.contributes.configuration.properties;
    assert.equal(!!properties['chali.build.llvmPath'], variant === 'external');
    if (variant === 'external') {
      assert.equal(properties['chali.build.llvmPath'].minLength, 1);
      assert.equal(properties['chali.build.llvmPath'].pattern, '\\S');
    }
    fs.writeFileSync(path.join(directory, 'package.json'), JSON.stringify(manifest));
    fs.writeFileSync(path.join(directory, 'bin/chali-build-info.json'), JSON.stringify({ llvm: variant }));
    const config = value => ({ get: () => value });
    if (variant === 'static') {
      assert.deepEqual(llvmOptions(directory, config('ignored clang')), { llvmPath: '' });
      assert.deepEqual(llvmOptions(directory, config('')), { llvmPath: '' });
    } else {
      assert.throws(() => llvmOptions(directory, config('  ')), /chali.build.llvmPath/);
      const options = llvmOptions(directory, config(' clang with space '));
      assert.equal(options.llvmPath, 'clang with space');
      assert.equal(buildRunCommand('chali', 'source.chl', options), 'chali run source.chl --llvm "clang with space"');
      assert.deepEqual(buildArguments('exe', 'source.chl', undefined, options),
        ['aot', 'source.chl', '--llvm', 'clang with space']);
    }
    fs.writeFileSync(path.join(directory, 'bin/chali-build-info.json'), JSON.stringify({ llvm: variant === 'static' ? 'external' : 'static' }));
    assert.throws(() => llvmOptions(directory, config('clang')), /不一致/);
  }
  assert.equal(JSON.stringify(original), unchanged);
  console.log('LLVM variants: static setting absent, external required, build/run arguments and payload identity passed');
} finally {
  fs.rmSync(directory, { recursive: true, force: true });
}
