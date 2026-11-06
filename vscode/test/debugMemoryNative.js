'use strict';

/** 通过真实 GDB 的 DAP 内存请求验证语言逻辑地址，不调用目标函数。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { spawnSync } = require('node:child_process');
const { DebugClient } = require('./debugClient');
const root = path.resolve(__dirname,'../..');
const compiler = process.env.CHALI_BIN || path.join(root,'chali.exe');
const work = fs.mkdtempSync(path.join(root,'tmp/debug-memory-'));
const source = path.join(work,'main.chl');
const lines = [
  'interface Reading {int64 read();};',
  'struct Pair:Reading {int64 first;int64 second;int64 read(){return self.first;}};',
  'Pair shared;',
  'void main(){',
  '    shared.first=101;shared.second=202;',
  '    Pair const& alias=&shared;',
  '    Pair row;row.first=7;row.second=9;',
  '    Reading object=row;',
  '    int64 values[]=new[3];values[0]=11;values[1]=22;values[2]=33;',
  '    uint8 bytes[]=new[4];bytes[0]=0;bytes[1]=255;bytes[2]=65;bytes[3]=9;',
  '    int64 shadow=123;',
  '    {',
  '        Pair shadow;shadow.first=17;shadow.second=19;',
  '        println("SHADOW {}",shadow.first);',
  '    }',
  '    println("READY {}",shadow);',
  '    int64 pressure[]=new[32768];pressure[0]=8;',
  '    values[0]=44;',
  '    println("UPDATED {}",pressure[0]);',
  '}',
];
fs.writeFileSync(source,lines.join('\n')+'\n');

/** 取得当前用户帧的变量，保持本次暂停专属的内存引用。 */
async function variables(client,thread) {
  const stack=await client.request('stackTrace',{threadId:thread});
  const scopes=await client.request('scopes',{frameId:stack.stackFrames[0].id});
  const values=await client.request('variables',{variablesReference:scopes.scopes[0].variablesReference});
  return Object.fromEntries(values.variables.map(value=>[value.name,value]));
}
/** 取一次引用对应的逻辑基址；聚合值的显示文本不再是地址。 */
async function addressOf(client,value) {
  const response=await client.request('readMemory',{memoryReference:value.memoryReference,count:0});
  return BigInt(response.address);
}
/** 只读指定逻辑值的字节，同时核对返回的逻辑地址。 */
async function bytes(client,value,count,offset=0) {
  assert(value?.memoryReference,'值需要本次暂停的逻辑内存引用：'+JSON.stringify(value));
  const base=await addressOf(client,value);
  const response=await client.request('readMemory',{memoryReference:value.memoryReference,count,offset});
  assert.equal(BigInt(response.address),base+BigInt(offset));
  assert.equal(response.unreadableBytes,0);
  const data=Buffer.from(response.data,'base64');assert.equal(data.length,count);return data;
}

async function main() {
  for(const allocator of ['malloc','pool']) {
    const prefix=path.join(work,allocator);
    const built=spawnSync(compiler,['build',source,'--debug','--checked','--allocator',allocator,'--out',prefix],{cwd:root,env:process.env,encoding:'utf8',windowsHide:true,timeout:180000});
    assert.equal(built.status,0,built.stdout+built.stderr);
    const client=new DebugClient(root,compiler);
    try {
      const capabilities=await client.request('initialize',{adapterID:'chali'});assert.equal(capabilities.supportsReadMemoryRequest,true);
      await client.request('launch',{executable:prefix+(process.platform==='win32'?'.exe':''),cwd:root});await client.event('initialized');
      const points=['SHADOW','READY','UPDATED'].map(text=>({line:lines.findIndex(line=>line.includes('println("'+text))+1}));
      const installed=await client.request('setBreakpoints',{source:{path:source},breakpoints:points});
      assert(installed.breakpoints.every(value=>value.verified));await client.request('configurationDone');
      let stop=await client.event('stopped'), values=await variables(client,stop.threadId);
      let data=await bytes(client,values.shadow,16);
      assert.equal(data.readBigInt64LE(),17n);assert.equal(data.readBigInt64LE(8),19n);
      await client.request('continue',{threadId:stop.threadId});stop=await client.event('stopped');values=await variables(client,stop.threadId);
      assert.equal(values.shadow.value,'123');assert.equal(values.shadow.memoryReference,undefined,'内层聚合退出后不能把同名整数当作地址');
      assert.equal(values.object.memoryReference,undefined,'接口对象编号不能作为数据地址');
      data=await bytes(client,values.values,24);
      assert.deepEqual([0,8,16].map(offset=>data.readBigInt64LE(offset)),[11n,22n,33n]);
      assert.equal((await bytes(client,values.values,8,-8)).readBigInt64LE(),3n);
      assert.deepEqual(await bytes(client,values.bytes,4),Buffer.from([0,255,65,9]));
      data=await bytes(client,values.row,16);assert.equal(data.readBigInt64LE(),7n);assert.equal(data.readBigInt64LE(8),9n);
      data=await bytes(client,values.alias,16);assert.equal(data.readBigInt64LE(),101n);assert.equal(data.readBigInt64LE(8),202n);
      await assert.rejects(client.request('readMemory',{memoryReference:values.values.memoryReference,count:25}),/区间|越界/);
      await assert.rejects(client.request('readMemory',{memoryReference:'0x1234',count:8}),/引用/);
      await assert.rejects(client.request('readMemory',{memoryReference:[values.values.memoryReference],count:8}),/引用/);
      const stale=values.values.memoryReference;
      await client.request('continue',{threadId:stop.threadId});stop=await client.event('stopped');
      await assert.rejects(client.request('readMemory',{memoryReference:stale,count:8}),/过期/);
      values=await variables(client,stop.threadId);
      assert.equal((await bytes(client,values.values,8)).readBigInt64LE(),44n);
      assert.deepEqual(await bytes(client,values.bytes,4),Buffer.from([0,255,65,9]));
      data=await bytes(client,values.row,16);assert.equal(data.readBigInt64LE(),7n);assert.equal(data.readBigInt64LE(8),9n);
      await client.request('continue',{threadId:stop.threadId});await client.event('terminated');
    } finally {
      fs.writeFileSync(path.join(work,allocator+'-messages.json'),JSON.stringify({messages:client.messages,stderr:client.stderr},null,2));
      await client.close();
    }
  }
  console.log('原生调试内存：malloc／pool、数组头及载荷、字节数组、记录／静态借用、池增长、越界拒绝及暂停引用隔离通过');
}
main().catch(error=>{console.error(error);process.exitCode=1;});
