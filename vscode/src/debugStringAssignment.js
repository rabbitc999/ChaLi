'use strict';

/** 拥有型 String 的受限内容修改：只改变等长 UTF-8 载荷，不覆盖句柄或分配目标对象。 */
const MAX_BYTES = 4096;

/** 核对明确的局部拥有关系并解析 JSON 字符串，不接受表达式或隐式转换。 */
function encodeStringAssignment(binding, layout, text) {
  if (binding.readOnly) throw new Error('不能修改不可变绑定');
  if (binding.stringOwner !== true || !['String','std::core::String'].includes(binding.type) || !binding.address
    || binding.nativeStorage !== 'i64' || layout?.kind !== 's' || layout.width !== 8)
    throw new Error('缺少可核对的局部 String 拥有关系；引用、参数、模式绑定和旧构件不能直接修改');
  if (typeof text !== 'string' || text.length > 32768) throw new Error('拥有 String 修改超过输入预算');
  let value;
  try { value = JSON.parse(text); } catch { throw new Error('拥有 String 只接受 JSON 字符串字面量'); }
  if (typeof value !== 'string') throw new Error('拥有 String 只接受 JSON 字符串字面量');
  for (const point of value) if (point.codePointAt(0) >= 0xd800 && point.codePointAt(0) <= 0xdfff)
    throw new Error('String 修改不能包含未配对的 Unicode 代理项');
  const bytes = Buffer.from(value, 'utf8');
  if (bytes.length > MAX_BYTES) throw new Error('String 修改超过 4096 字节预算');
  return { bytes, text: JSON.stringify(value), type: binding.type };
}

/** 核对当前拥有描述符、独立缓冲区及完整等长载荷，返回单次写入的物理地址。 */
async function stringWriteAddress(handle, bytes, memory) {
  if (typeof handle !== 'bigint' || handle <= 0n || handle < memory.dataSize
    || !memory.context || !memory.validateStorage || !Array.isArray(memory.regions))
    throw new Error('String 没有可核对的拥有堆存储；空句柄、静态数据和旧观察目录只读');
  if (!Buffer.isBuffer(bytes) || bytes.length > MAX_BYTES) throw new Error('String 修改载荷超过预算');
  await memory.validateStorage();
  const descriptor = memory.regions.find(region => region.begin === handle);
  if (!descriptor || descriptor.end - handle < 8n) throw new Error('String 描述符不是完整的存活分配');
  await memory.readable(handle, 8);
  const marker = (await memory.read(handle, 8)).readBigInt64LE();
  let length = marker, data = handle + 8n, capacity = descriptor.end - data;
  if (marker < 0n) {
    if (marker !== -1n || descriptor.end - handle < 32n) throw new Error('String 拥有描述符无效');
    await memory.readable(handle, 32);
    const header = await memory.read(handle, 32);
    length = header.readBigInt64LE(8); capacity = header.readBigInt64LE(16); data = header.readBigUInt64LE(24);
    const allocation = data === handle + 32n ? descriptor : memory.regions.find(region => region.begin === data);
    if (length < 0n || capacity < length || !allocation || data < allocation.begin
      || data >= allocation.end || capacity + 1n > allocation.end - data)
      throw new Error('String 容量或独立缓冲区身份无效');
  }
  if (length < 0n || length > BigInt(MAX_BYTES) || capacity < length + (marker >= 0n ? 1n : 0n))
    throw new Error('String 长度或存活分配范围无效，或超过修改预算');
  if (length !== BigInt(bytes.length)) throw new Error('安全 String 修改要求 UTF-8 字节数相同；不改变长度、容量或拥有句柄');
  await memory.readable(data, bytes.length + 1);
  if ((await memory.read(data + length, 1))[0] !== 0) throw new Error('String 终止字节无效');
  await memory.validateStorage();
  return bytes.length ? memory.address(data, 1) : null;
}

module.exports = { encodeStringAssignment, stringWriteAddress };
