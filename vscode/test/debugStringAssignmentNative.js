'use strict';

/** 用真实 GDB 修改拥有 String，保持描述符、句柄、借用及独立副本。 */
const assert=require('node:assert/strict');
const fs=require('node:fs');
const path=require('node:path');
const {spawnSync}=require('node:child_process');
const {DebugClient}=require('./debugClient');
const root=path.resolve(__dirname,'../..'),compiler=process.env.CHALI_BIN;
const external=process.env.CHALI_DEBUG_STRING_LLVM;
assert(compiler);
const work=fs.mkdtempSync(path.join(root,'tmp/debug-string-assignment-')),source=path.join(work,'main.chl');
const lines=[
  'void inspect(String value){',
  ' println("PARAMETER");',
  ' assert(value=="world");',
  '}',
  'void main(){',
  ' String text="hello";String independent=text;String const& alias=&text;',
  ' String direct="plain";String longText="abcdefghijklmnopqrstuvwx";',
  ' String unicode="中文";String empty="";const String fixed="fixed";',
  ' List<String> copies;copies.push(text);',
  ' String moved="gone";String recipient=move moved;',
  ' println("READY");',
  ' assert(text=="world" && alias=="world" && independent=="hello" && copies[0]=="hello");',
  ' assert(direct=="other" && longText=="ABCDEFGHIJKLMNOPQRSTUVWX" && unicode=="世界");',
  ' assert(empty=="" && fixed=="fixed" && recipient=="gone");',
  ' direct=direct+"!";longText=longText+"!";',
  ' assert(direct=="other!" && longText=="ABCDEFGHIJKLMNOPQRSTUVWX!");',
  ' {String text="inner";String const& innerAlias=&text;',
  '  println("INNER");',
  '  assert(text=="child" && innerAlias=="child");',
  ' }',
  ' println("AFTER");',
  ' assert(text=="world" && alias=="world");',
  ' inspect(text);',
  ' println("RESULT {} {} {} {}",text,independent,direct,unicode);',
  '}',
];
fs.writeFileSync(source,lines.join('\n')+'\n');

/** 取得当前源码帧、作用域及局部值。 */
async function view(client,thread,level=0) {
  const frames=(await client.request('stackTrace',{threadId:thread})).stackFrames,frame=frames[level].id;
  const scope=(await client.request('scopes',{frameId:frame})).scopes[0].variablesReference;
  const values=(await client.request('variables',{variablesReference:scope})).variables;
  return {frame,scope,values:Object.fromEntries(values.map(value=>[value.name,value]))};
}

/** 在两种分配器下验证直接与镜像绑定、独立外置缓冲区和恢复执行。 */
async function main() {
  for(const allocator of ['malloc','pool']) {
    const prefix=path.join(work,allocator);
    const built=spawnSync(compiler,['build',source,'--debug','--checked','--allocator',allocator,'--out',prefix,...(external?['--llvm',external]:[])],
      {cwd:root,env:process.env,encoding:'utf8',windowsHide:true,timeout:240000});
    assert.equal(built.status,0,built.stdout+built.stderr);
    const client=new DebugClient(root,compiler);
    try {
      await client.request('initialize',{adapterID:'chali',supportsInvalidatedEvent:true});
      await client.request('launch',{executable:prefix+(process.platform==='win32'?'.exe':''),cwd:root});
      await client.event('initialized');
      const points=['READY','INNER','AFTER','PARAMETER'].map(word=>({line:lines.findIndex(line=>line.includes('println("'+word))+1}));
      const installed=await client.request('setBreakpoints',{source:{path:source},breakpoints:points});
      assert(installed.breakpoints.every(point=>point.verified));await client.request('configurationDone');
      let stop=await client.event('stopped'),current=await view(client,stop.threadId);
      const set=(name,value)=>client.request('setVariable',{variablesReference:current.scope,name,value});
      for(const name of ['text','direct','longText','unicode','empty']) assert(!current.values[name].presentationHint?.attributes.includes('readOnly'),name);
      for(const name of ['fixed','alias','copies','moved']) assert(current.values[name].presentationHint.attributes.includes('readOnly'),name);
      const before={};
      for(const name of ['text','longText']) before[name]=await client.request('readMemory',{memoryReference:current.values[name].memoryReference,count:32});
      const obsolete=current.values.text.memoryReference;
      assert.equal((await set('text','"world"')).value,'"world"');
      await assert.rejects(client.request('readMemory',{memoryReference:obsolete,count:8}),/过期/);
      await set('direct','"other"');await set('longText','"ABCDEFGHIJKLMNOPQRSTUVWX"');await set('unicode','"世界"');await set('empty','""');
      for(const [name,value] of [['text','"different length"'],['text','0'],['text','text'],['text','"\\ud800"'],['fixed','"other"'],['alias','"world"'],['copies','0'],['moved','"safe"']])
        await assert.rejects(set(name,value));
      current=await view(client,stop.threadId);
      for(const [name,text] of [['text','"world"'],['direct','"other"'],['unicode','"世界"'],['independent','"hello"'],['recipient','"gone"']]) {
        assert.equal(current.values[name].value,text,name);
        assert.equal((await client.request('evaluate',{frameId:current.frame,expression:name})).result,text);
      }
      assert.equal(current.values.alias.value,'&"world"');
      assert.equal((await client.request('evaluate',{frameId:current.frame,expression:'copies[0]'})).result,'"hello"');
      for(const name of ['text','longText']) {
        const after=await client.request('readMemory',{memoryReference:current.values[name].memoryReference,count:32});
        assert.equal(after.address,before[name].address);assert.equal(after.data,before[name].data,'不得覆盖 '+name+' 描述符或拥有句柄');
      }
      const oldScope=current.scope;
      await client.request('continue',{threadId:stop.threadId});stop=await client.event('stopped');current=await view(client,stop.threadId);
      await assert.rejects(client.request('setVariable',{variablesReference:oldScope,name:'text',value:'"stale"'}),/过期|有效/);
      await set('text','"child"');
      await client.request('continue',{threadId:stop.threadId});stop=await client.event('stopped');current=await view(client,stop.threadId);
      assert.equal(current.values.text.value,'"world"');
      await client.request('continue',{threadId:stop.threadId});stop=await client.event('stopped');current=await view(client,stop.threadId);
      assert.equal(current.values.value.value,'"world"');
      await assert.rejects(set('value','"other"'),/拥有关系/);
      const caller=await view(client,stop.threadId,1);
      await assert.rejects(client.request('setVariable',{variablesReference:caller.scope,name:'text',value:'"other"'}),/栈顶/);
      await client.request('continue',{threadId:stop.threadId});
      assert.equal((await client.event('exited')).exitCode,0);await client.event('terminated');
      assert(client.messages.some(message=>message.event==='output'&&/RESULT world hello other! 世界/.test(message.body?.output||'')));
    } finally {fs.writeFileSync(prefix+'-messages.json',JSON.stringify(client.messages,null,2));await client.close();}
  }
  const prefix=path.join(work,'optimized');
  const built=spawnSync(compiler,['build',source,'--debug-optimized','--checked','--out',prefix,...(external?['--llvm',external]:[])],
    {cwd:root,env:process.env,encoding:'utf8',windowsHide:true,timeout:240000});
  assert.equal(built.status,0,built.stdout+built.stderr);
  const client=new DebugClient(root,compiler);
  try {
    await client.request('initialize',{adapterID:'chali'});
    await client.request('launch',{executable:prefix+(process.platform==='win32'?'.exe':''),cwd:root});await client.event('initialized');
    const installed=await client.request('setBreakpoints',{source:{path:source},breakpoints:[{line:lines.findIndex(line=>line.includes('println("READY'))+1}]});
    assert(installed.breakpoints[0].verified);await client.request('configurationDone');
    const stop=await client.event('stopped'),current=await view(client,stop.threadId);
    assert(Object.values(current.values).every(value=>value.presentationHint?.attributes.includes('readOnly')));
    await assert.rejects(client.request('setVariable',{variablesReference:current.scope,name:'direct',value:'"other"'}),/优化调试不修改/);
  } finally {fs.writeFileSync(prefix+'-messages.json',JSON.stringify(client.messages,null,2));await client.close();}
  console.log('拥有 String 真实修改：malloc／pool、内联／外置缓冲区、UTF-8、镜像、独立副本、借用、作用域及恢复执行通过');
}
main().catch(error=>{console.error(error);process.exitCode=1;});
