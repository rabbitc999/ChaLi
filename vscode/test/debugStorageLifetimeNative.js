'use strict';

/** 使用真实 GDB 验证非聚合移动、闭包绑定移动和重复声明后的恢复。 */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const {spawnSync} = require('node:child_process');
const {DebugClient} = require('./debugClient');
const root = path.resolve(__dirname, '../..');
const compiler = process.env.CHALI_BIN || path.join(root, 'chali.exe');
const work = fs.mkdtempSync(path.join(root, 'tmp/debug-storage-lifetime-'));
const source = path.join(work, 'main.chl');
const lines = [
  'struct Holder {int64 number;String text;};',
  'int64 inspect(int64 parameter){',
  ' int64 received=move parameter;',
  ' println("PARAMETER");',
  ' return received;',
  '}',
  'void main(){',
  ' int64 old=7;int64 current=move old;',
  ' String before="hello";String after=move before;',
  ' Holder row;row.number=13;row.text="field";',
  ' int64 field=move row.number;String fieldText=move row.text;',
  ' int64 values[]=new[2];values[0]=17;int64 element=move values[0];',
  ' int64 number=19;int64 mut& reference=&number;',
  ' let capture=int64 fn move[reference](){reference};',
  ' int64 owned=23;let captured=int64 fn move[owned](){owned};',
  ' int64 branchSource=47;int64 branchResult=0;',
  ' if(current==7){branchResult=move branchSource;}',
  ' int64 notMoved=53;if(current==0){int64 unused=move notMoved;}',
  ' {int64 old=41;',
  '  println("SHADOW");',
  ' }',
  ' println("MOVED");',
  ' for(int64 i in std::core::range(0,2)){',
  '  int64 roundValue=i+29;int64 mut& roundRef=&roundValue;',
  '  println("LOOPREADY");',
  '  let roundClosure=int64 fn move[roundRef](){roundRef};',
  '  println("LOOPMOVED");',
  '  assert(roundClosure()==i+29);',
  ' }',
  ' int64 parameterResult=inspect(37);assert(parameterResult==37);',
  ' println("RESULT {} {} {} {} {} {} {}",current,after,field,fieldText,element,capture(),captured());',
  '}',
];
fs.writeFileSync(source,lines.join('\n')+'\n');
/** 读取当前源码帧及局部变量。 */
async function locals(client,thread) {
  const stack=await client.request('stackTrace',{threadId:thread});
  const frame=stack.stackFrames[0].id;
  const scopes=await client.request('scopes',{frameId:frame});
  const values=await client.request('variables',{variablesReference:scopes.scopes[0].variablesReference});
  return {frame,scope:scopes.scopes[0].variablesReference,values:Object.fromEntries(values.variables.map(value=>[value.name,value]))};
}
/** 拒绝调试器对移动后的绑定求值或写入。 */
async function unavailable(client,current,name) {
  assert.match(current.values[name]?.value||'',/不可读.*(?:移动|销毁)/,name);
  assert.equal(current.values[name].variablesReference,0,name);
  await assert.rejects(client.request('evaluate',{frameId:current.frame,expression:name}),/移动|销毁/);
  if (current.values[name].type === 'int64')
    await assert.rejects(client.request('setVariable',{variablesReference:current.scope,name,value:'99'}),/移动|销毁/);
}
/** 构建并执行两种分配器的真实调试会话。 */
async function main() {
  for(const allocator of ['malloc','pool']) {
    const prefix=path.join(work,allocator);
    const build=spawnSync(compiler,['build',source,'--debug','--checked','--allocator',allocator,'--out',prefix],{cwd:root,env:process.env,encoding:'utf8',windowsHide:true,timeout:240000});
    assert.equal(build.status,0,build.stdout+build.stderr);
    const client=new DebugClient(root,compiler);
    try {
      await client.request('initialize',{adapterID:'chali'});
      await client.request('launch',{executable:prefix+(process.platform==='win32'?'.exe':''),cwd:root});
      await client.event('initialized');
      const points=['SHADOW','MOVED','LOOPREADY','LOOPMOVED','PARAMETER'].map(word=>({line:lines.findIndex(line=>line.includes('println("'+word))+1}));
      const installed=await client.request('setBreakpoints',{source:{path:source},breakpoints:points});
      assert(installed.breakpoints.every(point=>point.verified));
      await client.request('configurationDone');
      let stop=await client.event('stopped');let current=await locals(client,stop.threadId);
      assert.equal(current.values.old.value,'41');
      assert.equal((await client.request('evaluate',{frameId:current.frame,expression:'old+1'})).result,'42');
      await client.request('continue',{threadId:stop.threadId});
      stop=await client.event('stopped');current=await locals(client,stop.threadId);
      for(const name of ['old','before','reference','owned','branchSource']) await unavailable(client,current,name);
      assert.equal(current.values.branchResult.value,'47');assert.equal(current.values.notMoved.value,'53');
      assert.equal(current.values.current.value,'7');assert.equal(current.values.after.value,'"hello"');
      for(const expression of ['row.number','row.text','values[0]']) await assert.rejects(client.request('evaluate',{frameId:current.frame,expression}),/移动|销毁/);
      assert(!Object.keys(current.values).some(name=>/^__chali_(?:storage|available|context)_/.test(name)));
      for(let round=0;round<2;round++) {
        await client.request('continue',{threadId:stop.threadId});
        stop=await client.event('stopped');current=await locals(client,stop.threadId);
        assert.equal(current.values.roundRef.value,'&'+(29+round));
        assert.equal((await client.request('evaluate',{frameId:current.frame,expression:'roundRef+1'})).result,String(30+round));
        await client.request('continue',{threadId:stop.threadId});
        stop=await client.event('stopped');current=await locals(client,stop.threadId);
        await unavailable(client,current,'roundRef');
        assert.equal(current.values.roundValue.value,String(29+round));
      }
      await client.request('continue',{threadId:stop.threadId});
      stop=await client.event('stopped');current=await locals(client,stop.threadId);
      await unavailable(client,current,'parameter');assert.equal(current.values.received.value,'37');
      await client.request('continue',{threadId:stop.threadId});await client.event('terminated');
      assert(client.messages.some(message=>message.event==='output'&&/RESULT 7 hello 13 field 17 19 23/.test(message.body?.output||'')));
    } finally {fs.writeFileSync(path.join(work,allocator+'-messages.json'),JSON.stringify(client.messages,null,2));await client.close();}
  }
  for(const allocator of ['malloc','pool']) {
    const prefix=path.join(work,allocator+'-optimized');
    const build=spawnSync(compiler,['build',source,'--debug-optimized','--checked','--allocator',allocator,'--out',prefix],{cwd:root,env:process.env,encoding:'utf8',windowsHide:true,timeout:240000});
    assert.equal(build.status,0,build.stdout+build.stderr);
    const client=new DebugClient(root,compiler);
    try {
      await client.request('initialize',{adapterID:'chali'});
      await client.request('launch',{executable:prefix+(process.platform==='win32'?'.exe':''),cwd:root});
      await client.event('initialized');
      const installed=await client.request('setBreakpoints',{source:{path:source},breakpoints:[{line:lines.findIndex(line=>line.includes('println("MOVED'))+1}]});
      assert(installed.breakpoints[0].verified);
      await client.request('configurationDone');
      const stop=await client.event('stopped'),current=await locals(client,stop.threadId);
      let inspected=0;
      for(const name of ['old','before','reference','owned','branchSource']) if(current.values[name]) {
        inspected++;
        assert.match(current.values[name].value,/不可读.*(?:移动|销毁|优化|核对)|已优化|布局不可用/,name);
        assert.equal(current.values[name].variablesReference,0,name);
        assert(current.values[name].presentationHint?.attributes.includes('readOnly'));
        await assert.rejects(client.request('evaluate',{frameId:current.frame,expression:name}),/移动|销毁|优化|不可读|核对/);
      }
      assert(inspected>0,'优化调试必须实际核对至少一个移动源');
      await client.request('continue',{threadId:stop.threadId});await client.event('terminated');
      assert(client.messages.some(message=>message.event==='output'&&/RESULT 7 hello 13 field 17 19 23/.test(message.body?.output||'')));
    } finally {fs.writeFileSync(prefix+'-messages.json',JSON.stringify(client.messages,null,2));await client.close();}
  }
  console.log('原生存储生命周期：标量、字符串、字段、索引、闭包绑定移动和重复声明恢复，O0／O3、malloc／pool 通过');
}
main().catch(error=>{console.error(error);process.exitCode=1;});
