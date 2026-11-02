'use strict';

/** 验证只读内存目录、线程身份、代次、公开边界和读取预算。 */
const assert = require('node:assert/strict');
const { DebugMemory, systemThread } = require('../src/debugMemory');
const { LIMIT } = require('../src/debugMetadata');
const logical = 0x4000000000001000n;

/** 以真实 MI 字节格式模拟固定布局目录，只允许内部符号和只读线程查询。 */
function target(change = () => {}, options = {}) {
  const header = Buffer.alloc(24), node = Buffer.alloc(96), rows = Buffer.alloc(48), revision = Buffer.alloc(8), context = Buffer.alloc(8);
  const data = Buffer.alloc(64), heap = Buffer.alloc(options.large ? 1048576 : 16), restricted = Buffer.from([9,8,7,6]);
  header.writeBigUInt64LE(1n); header.writeBigUInt64LE(8n,8); header.writeBigUInt64LE(8192n,16);
  [42n,2n,7n,16384n,20480n,64n,12288n,2n,1n,0n].forEach((value,index) => node.writeBigUInt64LE(value,index*8));
  node.writeBigUInt64LE(32768n,80);
  [logical,BigInt(heap.length),1048576n,logical+64n,4n,2097152n].forEach((value,index) => rows.writeBigUInt64LE(value,index*8));
  if (options.large) node.writeBigUInt64LE(1n,56);
  revision.writeBigUInt64LE(7n); data.writeBigUInt64LE(123n,8); heap.writeBigUInt64LE(777n);
  const state = {header,node,rows,revision,data,heap,context}; change(state);
  const regions = [[4096n,header],[8192n,node],[12288n,rows],[16384n,revision],[20480n,data],[32768n,context],[1048576n,heap],[2097152n,restricted]];
  const calls = [];
  const reader = new DebugMemory(async command => {
    calls.push(command);
    if (command === '-thread-info 1') return {threads:[{id:'1','target-id':'Thread 100.0x2a',state:options.running?'running':'stopped'}]};
    if (command === '-data-evaluate-expression "(unsigned long long)&chali_debug_memory"') return {value:'4096'};
    const match = /^-data-read-memory-bytes (0x[0-9a-f]+) ([0-9]+)$/.exec(command); assert(match,command);
    const address=BigInt(match[1]), count=Number(match[2]);
    const region=regions.find(([base,buffer]) => address>=base && address+BigInt(count)<=base+BigInt(buffer.length));
    assert(region,'不能读取目录之外的物理存储：'+command);
    const contents=region[1].subarray(Number(address-region[0]),Number(address-region[0])+count).toString('hex');
    if (options.changeDuringRead && region[1]===rows) node.writeBigUInt64LE(4n,8);
    if (options.changeContextDuringRead && region[1]===heap) context.writeBigUInt64LE(1n);
    return {memory:[{begin:match[1],contents}]};
  });
  return {reader,calls};
}

async function main() {
  assert.equal(systemThread('Thread 100.0x2a'),42n); assert.equal(systemThread('Thread 100.42'),42n);
  assert.equal(systemThread('Thread 0x1234 (LWP 42)'),42n); assert.equal(systemThread('process 42'),42n);
  assert.throws(()=>systemThread('Thread 1'),/身份/);
  const healthy=target(), snapshot=await healthy.reader.snapshot(1);
  assert.equal((await snapshot.read(8n,8)).readBigUInt64LE(),123n);
  assert.equal((await snapshot.read(logical,8)).readBigUInt64LE(),777n);
  assert.deepEqual(await snapshot.read(logical+64n,4),Buffer.from([9,8,7,6]));
  assert.equal(await snapshot.address(8n,8),20488n);
  assert.equal(await snapshot.address(logical,8),1048576n);
  assert.equal(await snapshot.address(logical+15n,1),1048591n);
  assert.equal(await snapshot.address(logical+64n,1),2097152n);
  for (const [address,count] of [[0n,8],[logical-1n,1],[logical+15n,8],[logical+64n,8],[logical+68n,1],[logical,0],[logical,2]])
    await assert.rejects(snapshot.address(address,count),/地址|存储|越界|宽度|释放/);
  for (const [address,count] of [[0n,8],[logical-1n,1],[logical+15n,2],[logical+63n,1],[logical+68n,1],[logical, LIMIT+1]])
    await assert.rejects(snapshot.read(address,count),/地址|区间|越界|长度/);
  for (const change of [
    state=>state.header.writeBigUInt64LE(99n),
    state=>state.header.writeBigUInt64LE(4n,8),
    state=>state.node.writeBigUInt64LE(3n,8),
    state=>state.node.writeBigUInt64LE(0n,64),
    state=>state.node.writeBigUInt64LE(65537n,56),
    state=>state.rows.writeBigUInt64LE(logical+8n,24),
    state=>{state.node.writeBigUInt64LE(43n);state.node.writeBigUInt64LE(8192n,72);},
  ]) await assert.rejects(target(change).reader.snapshot(1),/版本|表示|发布|完整|预算|重叠|循环/);
  await assert.rejects(target(undefined,{changeDuringRead:true}).reader.snapshot(1),/状态已经变化/);
  await assert.rejects(target(undefined,{running:true}).reader.snapshot(1),/尚未暂停/);
  await assert.rejects(target(state=>state.context.writeBigUInt64LE(1n)).reader.snapshot(1),/上下文已经变化/);
  const changedContext=await target(undefined,{changeContextDuringRead:true}).reader.snapshot(1);
  await assert.rejects(changedContext.read(logical,8),/上下文已经变化/);
  await assert.rejects(changedContext.address(logical,8),/上下文已经变化/);
  const stale=await target(state=>state.revision.writeBigUInt64LE(8n)).reader.snapshot(1);
  await assert.rejects(stale.read(logical,8),/映射已经失效/);
  await assert.rejects(stale.address(logical,8),/映射已经失效/);
  const wrapped=await target(state=>{state.node.writeBigUInt64LE(0n,16);state.revision.writeBigUInt64LE(0n);}).reader.snapshot(1);
  await assert.rejects(wrapped.read(logical,8),/映射已经失效/);
  assert.equal((await stale.read(8n,8)).readBigUInt64LE(),123n,'堆失效不影响静态数据的有界读取');
  const large=await target(undefined,{large:true}).reader.snapshot(1);
  for(let index=0;index<3;++index) assert.equal((await large.read(logical,1048576)).length,1048576);
  await assert.rejects(large.read(logical,1048576),/预算/);
  const cancelled=new AbortController();cancelled.abort();
  await assert.rejects(target().reader.snapshot(1,cancelled.signal),/取消/);
  const addressCancel=new AbortController(), cancellableAddress=await target().reader.snapshot(1,addressCancel.signal);
  addressCancel.abort(); await assert.rejects(cancellableAddress.address(logical,8),/取消/);
  assert(!healthy.calls.some(command=>/exec-|call |set variable/.test(command)));
  console.log('调试内存：只读 C 布局、真实线程身份、公开边界、失效代次、发布竞态、循环、取消及预算通过');
}
main().catch(error=>{console.error(error);process.exitCode=1;});
