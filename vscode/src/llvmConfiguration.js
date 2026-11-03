'use strict';

const fs = require('node:fs');
const path = require('node:path');

/** 根据包内真实构建信息解析 LLVM 变体，并核对 external 的必填配置。 */
function llvmOptions(extensionPath, configuration) {
  const manifest = JSON.parse(fs.readFileSync(path.join(extensionPath, 'package.json'), 'utf8'));
  const info = JSON.parse(fs.readFileSync(path.join(extensionPath, 'bin/chali-build-info.json'), 'utf8'));
  const variant = manifest.chaliLLVMVariant || info.llvm;
  if (!['static', 'external'].includes(variant) || variant !== info.llvm)
    throw new Error('ChaLi LLVM 变体与内嵌编译器不一致 / LLVM variant does not match the embedded compiler');
  if (variant === 'static') return { llvmPath: '' };
  const llvmPath = configuration.get('build.llvmPath', '').trim();
  if (!llvmPath) throw new Error('external 模式必须设置 chali.build.llvmPath / Configure chali.build.llvmPath for the external variant');
  return { llvmPath };
}

/** 生成当前发布变体的设置清单，static 包不暴露外置 LLVM 路径选项。 */
function variantManifest(manifest, variant) {
  if (!['static', 'external'].includes(variant)) throw new Error('Unknown LLVM variant');
  const result = structuredClone(manifest);
  result.chaliLLVMVariant = variant;
  const properties = result.contributes.configuration.properties;
  if (variant === 'static') delete properties['chali.build.llvmPath'];
  else properties['chali.build.llvmPath'] = { type: 'string', default: '',
    description: '%chali.build.llvmPath.description%', minLength: 1, pattern: '\\S' };
  return result;
}

module.exports = { llvmOptions, variantManifest };
