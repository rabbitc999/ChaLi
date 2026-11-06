'use strict';

/** 使用真实 GDB 核对基础引用、循环引用与移动后存储状态。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const { spawnSync } = require('node:child_process');
const { DebugClient } = require('./debugClient');
const root = path.resolve(__dirname, '../..');
const compiler = process.env.CHALI_BIN || path.join(root, 'chali.exe');
const external = process.env.CHALI_DEBUG_REFERENCES_LLVM;
const work = fs.mkdtempSync(path.join(root, 'tmp/debug-references-'));
const source = path.join(work, 'main.chl');
const lines = [
  'int64 once(int64 value){return value+1;}',
  'struct Node {int64 value;Node const& next;};',
  'struct Item {int64 value;};',
  'void main(){',
  '  Node node;node.value=7;node.next=&node;',
  '  int64 number=9;int64 const& alias=&number;',
  '  uint8 bytes[]=new[2];bytes[1]=255;uint8 const& byteAlias=&bytes[1];',
  '  String text="hello";String const& textAlias=&text;',
  '  Item original;original.value=13;',
  '  Node tail;tail.value=8;',
  '  Node head;head.value=6;head.next=&tail;',
  '  int64 mut& released=new int64;released=17;',
  '  int64 const& oldReference=&released;',
  '  println("READY");',
  '  Item recipient=move original;',
  '  released.drop();',
  '  println("MOVED");',
  '  int64 total=0;for(int64 i in std::core::range(0,4)){total=total+once(i);}assert(total==10);',
  '  println("RESULT {} {} {}",node.next.value,alias,recipient.value);',
  '}',
];
fs.writeFileSync(source, lines.join('\n') + '\n');

/** 取得当前源码帧及其变量。 */
async function locals(client, thread) {
  const stack = await client.request('stackTrace', {threadId:thread});
  const frame = stack.stackFrames[0].id;
  const scopes = await client.request('scopes', {frameId:frame});
  const values = await client.request('variables', {variablesReference:scopes.scopes[0].variablesReference});
  return {frame, values:Object.fromEntries(values.variables.map(item => [item.name,item]))};
}

async function main() {
  for (const allocator of ['malloc', 'pool']) {
    const prefix = path.join(work, allocator);
    const built = spawnSync(compiler, ['build',source,'--debug','--checked','--allocator',allocator,'--out',prefix,...(external?['--llvm',external]:[])],
      {cwd:root,env:process.env,encoding:'utf8',windowsHide:true,timeout:240000});
    assert.equal(built.status,0,built.stdout+built.stderr);
    const client = new DebugClient(root,compiler);
    try {
      await client.request('initialize',{adapterID:'chali'});
      await client.request('launch',{executable:prefix+(process.platform==='win32'?'.exe':''),cwd:root});
      await client.event('initialized');
      const points = ['READY','MOVED'].map(text=>({line:lines.findIndex(line=>line.includes('println("'+text))+1}));
      points.push({line:lines.findIndex(line=>line.startsWith('int64 once'))+1,hitCondition:'2',condition:'value >= 2'});
      const installed = await client.request('setBreakpoints',{source:{path:source},breakpoints:points});
      assert(installed.breakpoints.every(point=>point.verified));
      await client.request('configurationDone');
      let stop = await client.event('stopped');
      let current = await locals(client,stop.threadId);
      assert.equal(current.values.alias.value,'&9');
      assert.equal(current.values.byteAlias.value,'&255');
      assert.equal(current.values.textAlias.value,'&"hello"');
      assert.equal((await client.request('evaluate',{frameId:current.frame,expression:'alias+1'})).result,'10');
      assert(!Object.keys(current.values).some(name=>name.startsWith('__chali_context_')));
      const children = await client.request('variables',{variablesReference:current.values.node.variablesReference});
      const cycle = children.variables.find(item=>item.name==='next');
      assert(cycle,JSON.stringify(children));
      assert.match(cycle.value,/循环引用/);
      assert.equal(cycle.variablesReference,0);
      const head = await client.request('variables',{variablesReference:current.values.head.variablesReference});
      const next = head.variables.find(item=>item.name==='next');
      assert(next.variablesReference>0);
      const tail = await client.request('variables',{variablesReference:next.variablesReference});
      assert.equal(tail.variables.find(item=>item.name==='value').value,'8');
      assert.equal(tail.variables.find(item=>item.name==='next').value,'null');
      await client.request('continue',{threadId:stop.threadId});
      stop = await client.event('stopped'); current = await locals(client,stop.threadId);
      assert.match(current.values.original.value,/不可读.*移动或销毁/);
      assert.equal(current.values.original.variablesReference,0);
      assert.equal(current.values.released.value,'null');
      assert.equal(current.values.released.variablesReference,0);
      assert.match(current.values.oldReference.value,/不可读.*(?:移动或销毁|释放|存活分配)/);
      assert.equal(current.values.oldReference.variablesReference,0);
      const recipient = await client.request('variables',{variablesReference:current.values.recipient.variablesReference});
      assert.equal(recipient.variables.find(item=>item.name==='value').value,'13');
      await client.request('continue',{threadId:stop.threadId});
      stop = await client.event('stopped'); current = await locals(client,stop.threadId);
      assert.equal(current.values.value.value,'3');
      await client.request('continue',{threadId:stop.threadId});
      await client.event('terminated');
      assert(client.messages.some(message=>message.event==='output' && /RESULT 7 9 13/.test(message.body?.output || '')));
    } finally {
      fs.writeFileSync(path.join(work,allocator+'-messages.json'),JSON.stringify({messages:client.messages,stderr:client.stderr},null,2));
      await client.close();
    }
  }
  console.log('原生引用：基础引用实际值、私有身份隐藏、循环终止、移动源拒绝及接收方值，malloc／pool 通过');
}
main().catch(error=>{console.error(error);process.exitCode=1;});
