'use strict';

/** 核对拥有证明、UTF-8 长度、存储身份、单次写入和拒绝路径。 */
const assert = require('node:assert/strict');
const {PassThrough} = require('node:stream');
const {Adapter} = require('../src/nativeDebugAdapter');
const {encodeStringAssignment, stringWriteAddress} = require('../src/debugStringAssignment');
const layout = {kind:'s',width:8};
const binding = {name:'text',type:'String',stringOwner:true,address:true,nativeStorage:'i64',readOnly:false,scope:0,layout:0};

/** 生成带真实描述符和可检查载荷的合成堆快照。 */
function fixture(buffered = true, external = false, content = 'hello') {
  const raw = Buffer.alloc(64), payload = Buffer.from(content), storage = Buffer.alloc(32);
  const handle = 100n, data = external ? 300n : handle + (buffered ? 32n : 8n);
  raw.writeBigInt64LE(buffered ? -1n : BigInt(payload.length));
  if (buffered) {raw.writeBigInt64LE(BigInt(payload.length),8);raw.writeBigInt64LE(16n,16);raw.writeBigUInt64LE(data,24);}
  payload.copy(external ? storage : raw,external ? 0 : Number(data-handle));
  const regions = [{begin:100n,end:164n,physical:4096n},...(external ? [{begin:300n,end:332n,physical:8192n}] : [])];
  const memory = {
    dataSize:16n,context:{heap:1n,data:2n,objects:3n},regions,
    validateStorage:async()=>{}, readable:async()=>{},
    read:async(address,length)=>{
      const region = regions.find(item=>address>=item.begin && address+BigInt(length)<=item.end);
      assert(region,'不能越过已验证分配读取');
      return Buffer.from((region.begin===100n?raw:storage).subarray(Number(address-region.begin),Number(address-region.begin)+length));
    },
    address:async(address,length)=>{assert.equal(length,1);const region=regions.find(item=>address>=item.begin&&address<item.end);assert(region);return region.physical+address-region.begin;},
  };
  return {raw,storage,memory,handle,data};
}

/** 核对类型与堆边界，并用适配器证明只写载荷且不执行目标表达式。 */
async function main() {
  assert.equal(encodeStringAssignment(binding,layout,'"中文"').bytes.length,6);
  assert.equal(encodeStringAssignment({...binding,type:'std::core::String'},layout,'"world"').type,'std::core::String');
  for (const text of ['null','12','text','"a"+"b"','"\\ud800"','"\\udfff"',JSON.stringify('a'.repeat(4097))])
    assert.throws(()=>encodeStringAssignment(binding,layout,text));
  for (const changed of [{stringOwner:false},{stringOwner:undefined},{readOnly:true},{type:'String const&'},{type:'custom::String'},{address:false},{nativeStorage:'double'}])
    assert.throws(()=>encodeStringAssignment({...binding,...changed},layout,'"world"'));
  for (const [buffered,external] of [[true,false],[true,true],[false,false]]) {
    const value=fixture(buffered,external);
    assert.equal(await stringWriteAddress(value.handle,Buffer.from('world'),value.memory),external?8192n:4096n+(buffered?32n:8n));
    await assert.rejects(stringWriteAddress(value.handle,Buffer.from('longer'),value.memory),/字节数相同/);
    await assert.rejects(stringWriteAddress(101n,Buffer.from('world'),value.memory),/完整.*分配/);
    await assert.rejects(stringWriteAddress(1n,Buffer.from('world'),value.memory),/静态/);
  }
  const empty=fixture(true,false,'');assert.equal(await stringWriteAddress(empty.handle,Buffer.alloc(0),empty.memory),null);
  const unicode=fixture(true,false,'é');assert.equal(await stringWriteAddress(unicode.handle,Buffer.from('hi'),unicode.memory),4128n);
  for (const corrupt of [value=>value.raw.writeBigInt64LE(-2n),value=>value.raw.writeBigInt64LE(4n,16),
    value=>value.raw.writeBigUInt64LE(133n,24),value=>value.raw.writeBigUInt64LE(301n,24),value=>{value.raw[37]=1;}]) {
    const value=fixture();corrupt(value);await assert.rejects(stringWriteAddress(value.handle,Buffer.from('world'),value.memory));
  }
  const adapter = new Adapter('unused',new PassThrough(),new PassThrough()),messages=[],commands=[];
  adapter.send=value=>messages.push(value);adapter.state='stopped';
  const frame={thread:1,level:0,metadata:{position:{scope:0},function:{parents:new Map([[0,0]]),variables:[binding]}}};
  adapter.handles.set(20,frame);adapter.frames.set(10,frame);
  adapter.metadata={cache:{catalog:{layouts:['s0:0:0:8:0:0:']}},cancel(){}};
  const value=fixture(),header=Buffer.from(value.raw.subarray(0,32));
  adapter.memoryForFrame=async()=>value.memory;
  adapter.gdb={command:async command=>{
    commands.push(command);
    if(command.startsWith('-stack-list-variables'))return {variables:[{name:'text',type:'String',value:'100'}]};
    assert.equal(command,'-data-write-memory-bytes 0x1020 776f726c64');Buffer.from('world').copy(value.raw,32);return {};
  }};
  await adapter.request({seq:1,command:'setVariable',arguments:{variablesReference:20,name:'text',value:'"world"'}});
  assert.equal(messages.find(message=>message.request_seq===1).success,true);
  assert.deepEqual(value.raw.subarray(0,32),header);assert.equal(value.raw[37],0);
  assert.equal(commands.filter(command=>command.startsWith('-data-write')).length,1);
  for(const [seq,update] of [[2,'"length changed"'],[3,'0'],[4,'"\\ud800"']])
    await adapter.request({seq,command:'setVariable',arguments:{variablesReference:20,name:'text',value:update}});
  assert(messages.filter(message=>[2,3,4].includes(message.request_seq)).every(message=>!message.success));
  assert.equal(commands.filter(command=>command.startsWith('-data-write')).length,1);
  value.memory.readable=async()=>{throw new Error('存储已经移动');};
  await adapter.request({seq:5,command:'setVariable',arguments:{variablesReference:20,name:'text',value:'"again"'}});
  assert.match(messages.find(message=>message.request_seq===5).message,/移动/);
  assert.equal(commands.filter(command=>command.startsWith('-data-write')).length,1);
  console.log('String 安全修改：拥有证明、UTF-8 等长、描述符与缓冲区、零写入拒绝及单次载荷写入通过');
}
main().catch(error=>{console.error(error);process.exitCode=1;});
