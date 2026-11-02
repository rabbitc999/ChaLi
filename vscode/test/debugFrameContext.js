'use strict';

/** 核对源码帧身份、调用方兼容边界和跨上下文拒绝。 */
const assert = require('node:assert/strict');
const { PassThrough } = require('node:stream');
const { Adapter, parseMi } = require('../src/nativeDebugAdapter');
const { parseLayout } = require('../src/debugValue');

async function main() {
  const multiple = parseMi('{bkpt={number="2",addr="<MULTIPLE>"},{number="2.1",file="<native-debug-context>"},{number="2.2",file="main.chl"}}');
  assert.deepEqual(multiple.$values.map(location=>location.number),['2.1','2.2']);
  const adapter = new Adapter('unused',new PassThrough(),new PassThrough());
  const memory = {context:{heap:100n,data:101n,objects:102n}};
  adapter.memoryFor = async () => memory;
  const frame = {thread:1,level:1,metadata:{function:{context:{heap:'__chali_context_heap',data:'__chali_context_data',objects:'__chali_context_objects'}}}};
  const rows = Object.entries(memory.context).map(([key,value])=>({name:'__chali_context_'+key,value:String(value)}));
  let reads = 0;
  adapter.gdb = {command:async command=>{assert.equal(command,'-stack-list-variables --thread 1 --frame 1 --simple-values');++reads;return {variables:rows};}};
  assert.equal(await adapter.memoryForFrame(frame,undefined,rows),memory);
  assert.equal(reads,0);
  assert.equal(await adapter.memoryForFrame(frame),memory);
  assert.equal(reads,1);
  await assert.rejects(adapter.memoryForFrame(frame,undefined,rows.slice(1)),/帧身份/);
  await assert.rejects(adapter.memoryForFrame(frame,undefined,[...rows,rows[0]]),/帧身份/);
  await assert.rejects(adapter.memoryForFrame(frame,undefined,rows.map((row,index)=>index?row:{...row,value:'999'})),/活动原生上下文/);
  await assert.rejects(adapter.memoryForFrame(frame,undefined,rows.map((row,index)=>index?row:{...row,value:'<optimized out>'})),/优化不可读/);
  delete frame.metadata.function.context;
  await assert.rejects(adapter.memoryForFrame(frame),/没有可核对/);
  frame.level=0;assert.equal(await adapter.memoryForFrame(frame),memory);
  adapter.state='stopped';
  const messages=[];adapter.send=message=>messages.push(message);
  const self=Buffer.alloc(8);self.writeBigUInt64LE(128n);
  memory.read=async (address,size)=>{assert.equal(address,128n);assert.equal(size,8);return self;};
  const recursive=parseLayout('S4:Node0:0:8:0:1:R4:next0:0:8:0:1:X4:Node0:0:8:0:0:');
  memory.referenceLayout=name=>name==='Node'?recursive:undefined;
  adapter.handles.set(50,{kind:'value',thread:1,frame,layout:recursive,address:128n});
  await adapter.request({seq:1,command:'variables',arguments:{variablesReference:50}});
  const response=messages.find(message=>message.request_seq===1);
  assert.equal(response.success,true,JSON.stringify(response));
  assert.match(response.body.variables[0].value,/循环引用/);
  assert.equal(response.body.variables[0].variablesReference,0);
  console.log('帧内存身份：实际三元组、跨上下文拒绝、重复或优化槽拒绝及旧构件兼容边界通过');
}
main().catch(error=>{console.error(error);process.exitCode=1;});
