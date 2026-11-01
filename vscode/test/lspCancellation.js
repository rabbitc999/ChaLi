'use strict';

/** @brief 检查客户端并行响应配对、请求取消、监听释放与诊断版本透传。 */
const assert = require('node:assert/strict');
const { LspClient } = require('../src/lspClient');

/** @brief 用内存帧代替传输，确定性模拟乱序和过期响应。 */
async function main() {
  const client = new LspClient('unused');
  client.process = { exitCode: null }; client.stopped = false;
  const sent = []; client._send = message => sent.push(message);
  const params = { textDocument: { uri: 'file:///sample.chl' } };
  const first = client.request('textDocument/hover', params);
  const hoverId = sent.at(-1).id;
  const completion = client.request('textDocument/completion', params);
  const completionId = sent.at(-1).id;
  const newer = client.request('textDocument/hover', params);
  const newerId = sent.at(-1).id;
  assert.equal(await first, null);
  assert.ok(sent.some(frame => frame.method === '$/cancelRequest' && frame.params.id === hoverId));
  client._handle({ id: hoverId, result: 'stale' });
  client._handle({ id: completionId, result: ['completion'] });
  client._handle({ id: newerId, result: 'fresh' });
  assert.deepEqual(await completion, ['completion']); assert.equal(await newer, 'fresh');

  let cancel; let disposed = 0;
  const token = { isCancellationRequested: false,
    onCancellationRequested(callback) { cancel = callback; return { dispose() { disposed++; } }; } };
  const pending = client.request('textDocument/signatureHelp', params, token);
  cancel(); assert.equal(await pending, null); assert.equal(disposed, 1);
  assert.equal(client.pending.size, 0);
  const count = sent.length;
  assert.equal(await client.request('textDocument/hover', params, { isCancellationRequested: true }), null);
  assert.equal(sent.length, count);
  const normal = client.request('textDocument/hover', params, token);
  client._handle({ id: sent.at(-1).id, result: 'done' });
  assert.equal(await normal, 'done'); assert.equal(disposed, 2);
  let diagnostic;
  client.on('diagnostics', value => { diagnostic = value; });
  client._handle({ method: 'textDocument/publishDiagnostics', params: { uri: params.textDocument.uri, version: 7, diagnostics: [] } });
  assert.equal(diagnostic.version, 7);
  assert.equal(client.pending.size, 0);
  const sourceA = client.request('chali/standardSource', { uri: 'chali-stdlib:/std/parse.chl' });
  const sourceAId = sent.at(-1).id;
  const sourceB = client.request('chali/standardSource', { uri: 'chali-stdlib:/std/io.chl' });
  const sourceBId = sent.at(-1).id;
  client._handle({ id: sourceAId, result: 'parse' }); client._handle({ id: sourceBId, result: 'io' });
  assert.equal(await sourceA, 'parse'); assert.equal(await sourceB, 'io');
  // exit 之后管道仍可能未关闭；stop 必须等待 close 才允许回收工作目录。
  const { EventEmitter } = require('node:events');
  const closing = new LspClient('unused');
  const child = new EventEmitter(); child.exitCode = null;
  let killed = 0, closed = false, finished = false;
  child.kill = () => { killed++; };
  closing.process = child;
  closing.closePromise = new Promise(resolve => child.once('close', () => { closed = true; resolve(); }));
  closing.request = async method => { assert.equal(method, 'shutdown'); };
  closing.notify = method => {
    assert.equal(method, 'exit'); child.exitCode = 0; child.emit('exit', 0);
    setTimeout(() => child.emit('close'), 30);
  };
  const stopping = closing.stop().then(() => { finished = true; });
  await new Promise(resolve => setTimeout(resolve, 10)); assert.equal(finished, false);
  await stopping; assert.equal(closed, true); assert.equal(killed, 0);
  console.log('LSP 乱序响应、取消、独立源码请求、诊断版本和真实关闭等待通过。');
}

main().catch(error => { console.error(error); process.exitCode = 1; });
