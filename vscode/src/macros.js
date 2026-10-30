'use strict';

/** 按编辑器语言选择界面文字，缺省面向中文开发者。 */
function ui(language, chinese, english) {
  return String(language || '').toLowerCase().startsWith('en') ? english : chinese;
}

/** AST 展示名称只翻译节点种类，源码中的名字和值始终原样保留。 */
const NODE_LABELS = {
  template: '模板', block: '语句块', call: '调用', identifier: '标识符',
  integer: '整数', real: '浮点数', float: '浮点数', double: '浮点数', string: '字符串',
  character: '字符', boolean: '布尔值', binary: '二元运算', unary: '一元运算',
  declaration: '声明', variable: '变量', assignment: '赋值', function: '函数',
  parameter: '参数', argument: '实参', member: '成员访问', index: '下标访问',
  if: '条件分支', while: '循环', for: '遍历', return: '返回', break: '退出循环',
  continue: '继续循环', cast: '类型转换', struct: '结构体', enum: '枚举',
  match: '模式匹配', array: '数组', list: '列表', null: '空值', node: '节点',
  macro_call: '宏调用', source_fragment: '源码片段', sequence: '语句序列',
  expression_statement: '表达式语句', value_block: '值块', closure: '闭包',
  qualified_member: '限定成员', truncated: '已截断',
};

/**
 * 把展开结果里的 AST 序列化成可读的树状结构。
 * @param {string|object} payload 语言服务器返回的 `text`（JSON 文本）或已解析对象。
 * @returns {object} 含 `expansions` 数组的展开结果。
 */
function parsePayload(payload) {
  if (typeof payload !== 'string') return payload || {};
  try {
    return JSON.parse(payload);
  } catch (error) {
    throw new Error(`展开结果不是合法的 AST 描述：${error.message}`);
  }
}

/**
 * 转义 HTML 文本与属性。
 * @param {*} value 待转义的值。
 * @returns {string} 转义后的字符串。
 */
function escapeHtml(value) {
  return String(value === undefined || value === null ? '' : value)
    .replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;').replace(/'/g, '&#39;');
}

/**
 * 把 AST 节点渲染成可折叠的树。
 * @param {object|null} node 节点；编译器用 `null` 表示被裁剪的空槽。
 * @param {number} depth 当前深度，用于控制默认展开层级。
 * @returns {string} 节点对应的 HTML。
 */
function renderNode(node, depth, language) {
  if (!node || typeof node !== 'object') return '';
  const rawKind = node.kind || 'node';
  const kind = escapeHtml(ui(language, NODE_LABELS[rawKind] || rawKind, rawKind));
  const name = node.name === undefined || node.name === null ? '' : String(node.name);
  const details = [];
  if (node.member) details.push(`<span class="member">.${escapeHtml(node.member)}</span>`);
  if (node.value !== undefined && node.value !== null && String(node.value) !== name)
    details.push(`<span class="value">= ${escapeHtml(node.value)}</span>`);
  if (node.type) details.push(`<span class="type">: ${escapeHtml(node.type)}</span>`);
  if (node.symbol) details.push(`<button class="symbol" data-binding="${escapeHtml(node.symbol)}">${escapeHtml(node.symbol)}</button>`);
  if (node.parameterCache) details.push(`<span class="type">${escapeHtml(ui(language, '参数缓存', 'parameter cache'))}: ${escapeHtml(node.parameterCache)}</span>`);
  for (const [key, title] of [['source', ui(language, '来源', 'Source')], ['definition', ui(language, '定义', 'Definition')]]) {
    const location = node[key];
    if (location && typeof location.file === 'string' && location.file && !location.file.startsWith('<') && Number.isInteger(location.line) && location.line > 0 && Number.isInteger(location.column) && location.column > 0)
      details.push(`<button data-node-location="${escapeHtml(JSON.stringify(location))}">${escapeHtml(title)}</button>`);
  }
  const origin = node.origin === 'argument' ? 'argument' : 'definition';
  const label = `<span class="kind">${kind}</span>`
    + (name ? `<span class="name">${escapeHtml(name)}</span>` : '')
    + details.join('');
  const children = Array.isArray(node.children)
    ? node.children.filter((child) => child && typeof child === 'object') : [];
  const binding = node.symbol ? ` data-binding-node="${escapeHtml(node.symbol)}"` : '';
  if (!children.length) return `<div class="leaf ${origin}"><span class="node-card"${binding}>${label}</span></div>`;
  const body = children.map((child) => renderNode(child, depth + 1, language)).join('');
  return `<details class="branch ${origin}"${depth < 3 ? ' open' : ''}>`
    + `<summary${binding}>${label}<span class="count">${children.length}</span></summary>`
    + `<div class="children">${body}</div></details>`;
}

/**
 * 单张预览图共用的样式：颜色全部带兜底色，脱离编辑器直接放进网页也能读。
 * @returns {string} `<style>` 片段。
 */
function pageStyle() {
  return `<style>
  :root { color-scheme: light dark; }
  * { box-sizing: border-box; }
  body {
    margin: 0; padding: 20px 22px 26px;
    background: var(--vscode-editor-background, #ffffff);
    color: var(--vscode-editor-foreground, #1f2328);
    font-family: var(--vscode-font-family, -apple-system, "Segoe UI", "Microsoft YaHei", sans-serif);
    font-size: var(--vscode-font-size, 13px); line-height: 1.5;
  }
  figure.macro { margin: 0; }
  [data-binding-node].binding-selected { outline: 2px solid var(--vscode-focusBorder, #007acc); }
  button.symbol, button[data-node-location] { font: inherit; color: inherit; background: transparent; border: 1px solid var(--vscode-panel-border, #888); border-radius: 3px; cursor: pointer; }
  figcaption { display: flex; flex-wrap: wrap; align-items: baseline; gap: 8px; margin-bottom: 12px; }
  figcaption .macro { font-size: 1.35em; font-weight: 600; color: var(--vscode-symbolIcon-functionForeground, #8250df); }
  figcaption .site { color: var(--vscode-descriptionForeground, #57606a); font-size: .92em; }
  figcaption .toolbar { margin-left: auto; display: flex; gap: 6px; }
  figcaption button {
    font: inherit; color: inherit; cursor: pointer; padding: 2px 9px; border-radius: 4px;
    border: 1px solid var(--vscode-panel-border, #d0d7de);
    background: var(--vscode-button-secondaryBackground, #f6f8fa);
  }
  figcaption button:hover { background: var(--vscode-button-secondaryHoverBackground, #eaeef2); }
  .tree-viewport { overflow: auto; padding: 18px 12px 28px; border: 1px solid var(--vscode-panel-border, #d0d7de); border-radius: 8px; min-height: 200px; }
  div.tree { width: max-content; min-width: 100%; font-family: var(--vscode-editor-font-family, ui-monospace, Consolas, monospace); text-align: center; }
  details.branch { position: relative; width: max-content; margin: 0 auto; }
  summary, .node-card { list-style: none; display: flex; align-items: baseline; justify-content: center; flex-wrap: wrap; gap: 7px; padding: 9px 12px;
    width: max-content; max-width: 360px; margin: 0 auto; border: 1px solid var(--vscode-panel-border, #d0d7de); border-radius: 7px;
    background: var(--vscode-editorWidget-background, #f6f8fa); box-shadow: 0 2px 4px #0000000b; text-align: left; overflow-wrap: anywhere; }
  summary { cursor: pointer; }
  summary::-webkit-details-marker { display: none; }
  summary::before { content: '▸'; color: var(--vscode-descriptionForeground, #8b949e); width: .9em; }
  details[open] > summary::before { content: '▾'; }
  div.children { display: flex; justify-content: center; align-items: flex-start; position: relative; padding-top: 28px; }
  div.children::before { content: ''; position: absolute; top: 0; left: 50%; height: 14px; border-left: 1px solid var(--vscode-descriptionForeground, #8b949e); }
  div.children > * { flex: 0 0 auto; padding: 0 12px; margin: 0; position: relative; }
  div.children > *::before, div.children > *::after { content: ''; position: absolute; top: -14px; right: 50%; width: 50%; height: 14px;
    border-top: 1px solid var(--vscode-descriptionForeground, #8b949e); }
  div.children > *::after { right: auto; left: 50%; border-left: 1px solid var(--vscode-descriptionForeground, #8b949e); }
  div.children > *:first-child::before { border-top: 0; }
  div.children > *:last-child::after { border-top: 0; }
  details.branch:not([open]) > div.children { display: none; }
  .argument > summary, .leaf.argument > .node-card { border-color: var(--vscode-symbolIcon-variableForeground, #9a6700); }
  span.kind { color: var(--vscode-symbolIcon-keywordForeground, #0550ae); }
  span.member, span.value { color: var(--vscode-symbolIcon-propertyForeground, #953800); }
  span.type { color: var(--vscode-symbolIcon-classForeground, #116329); }
  span.count {
    color: var(--vscode-descriptionForeground, #57606a); font-size: .8em; padding: 0 5px;
    border-radius: 8px; background: var(--vscode-badge-background, #eaeef2);
  }
  .argument > summary > span.kind, .leaf.argument .kind {
    color: var(--vscode-symbolIcon-variableForeground, #9a6700);
  }
  @media (prefers-color-scheme: dark) {
    body { background: var(--vscode-editor-background, #1e1e1e); color: var(--vscode-editor-foreground, #d4d4d4); }
    span.kind { color: var(--vscode-symbolIcon-keywordForeground, #79c0ff); }
    span.member, span.value { color: var(--vscode-symbolIcon-propertyForeground, #ffa657); }
    span.type { color: var(--vscode-symbolIcon-classForeground, #7ee787); }
    figcaption .macro { color: var(--vscode-symbolIcon-functionForeground, #d2a8ff); }
    figcaption .site, span.count { color: var(--vscode-descriptionForeground, #9da7b3); }
    summary, .node-card { background: var(--vscode-editorWidget-background, #252a32); }
  }
</style>`;
}

/**
 * 渲染一处展开的独立 AST 视图，供单图导出和 Tab 容器共用。
 * @param {object} expansion 展开记录。
 * @param {number} index 调用序号。
 * @param {number} total 调用总数。
 * @returns {string} figure 片段。
 */
function renderFigure(expansion, index, total, language) {
  const line = Number(expansion.line) || 1;
  const column = Number(expansion.column) || 0;
  const name = escapeHtml(expansion.name || ui(language, '宏', 'Macro'));
  const file = escapeHtml(String(expansion.file || '').split(/[\\/]/).pop() || ui(language, '当前文件', 'Current file'));
  return `<figure class="macro" data-index="${index}" data-macro="${name}" data-line="${line}" data-column="${column}">
  <figcaption>
    <span class="macro">${name}</span>
    <span class="site">${file} · ${ui(language, `第 ${line} 行 · 第 ${column + 1} 列`, `Line ${line} · Column ${column + 1}`)}</span>
    ${total > 1 ? `<span class="site">${ui(language, `第 ${index + 1} / ${total} 处`, `${index + 1} / ${total}`)}</span>` : ''}
    <span class="toolbar">
      <button type="button" data-action="expand">${ui(language, '全部展开', 'Expand all')}</button>
      <button type="button" data-action="collapse">${ui(language, '全部折叠', 'Collapse all')}</button>
      <label>${ui(language, '缩放', 'Zoom')} <input aria-label="${ui(language, '树形图缩放', 'Tree zoom')}" type="range" min="30" max="150" value="100" data-zoom></label>
      <button type="button" data-action="fit">${ui(language, '适应宽度', 'Fit width')}</button>
    </span>
  </figcaption>
  <div class="tree-viewport"><div class="tree">${renderNode(expansion.tree, 0, language) || `<div class="leaf">${ui(language, '（空模板）', '(Empty template)')}</div>`}</div></div>
</figure>`;
}

/**
 * 生成单张宏展开预览图（一个宏一张，自包含 HTML，可直接放进网页）。
 * @param {object} expansion 编译器给出的一次展开记录。
 * @param {number} index 展开序号，从 0 开始。
 * @param {object} [options] 可选设置；`total` 为同伴数量，`interactive` 为是否注入跳转脚本。
 * @returns {string} 完整 HTML 文档。
 */
function renderPage(expansion, index, options) {
  const settings = options || {};
  const total = Number(settings.total) || 1;
  const interactive = settings.interactive !== false;
  const line = Number(expansion.line) || 1;
  const column = Number(expansion.column) || 0;
  const name = escapeHtml(expansion.name || '宏');
  const file = escapeHtml(String(expansion.file || '').split(/[\\/]/).pop() || '当前文件');
  const heading = `${ui(settings.language, '宏展开', 'Macro expansion')} · ${name}`;
  return `<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>${escapeHtml(heading)}（${file} 第 ${line} 行）</title>
${pageStyle()}
</head>
<body>
${renderFigure(expansion, index, total, settings.language)}
${interactive ? `<script>
  document.querySelectorAll('button[data-action]').forEach((button) => {
    button.addEventListener('click', () => {
      if (button.dataset.action === 'fit') {
        const tree = document.querySelector('.tree'); tree.style.zoom = '1';
        const scale = Math.max(.3, Math.min(1, (document.querySelector('.tree-viewport').clientWidth - 24) / tree.scrollWidth));
        tree.style.zoom = String(scale); document.querySelector('[data-zoom]').value = String(Math.round(scale * 100)); return;
      }
      const open = button.dataset.action === 'expand';
      document.querySelectorAll('details').forEach((details) => { details.open = open; });
    });
  });
  const figure = document.querySelector('figure.macro');
  const zoom = document.querySelector('[data-zoom]');
  if (zoom) zoom.addEventListener('input', () => { document.querySelector('.tree').style.zoom = String(Number(zoom.value) / 100); });
  if (typeof acquireVsCodeApi === 'function' && figure) {
    const api = acquireVsCodeApi();
    figure.addEventListener('dblclick', () => api.postMessage({
      type: 'reveal', line: Number(figure.dataset.line), column: Number(figure.dataset.column),
    }));
  }
</script>` : ''}
</body>
</html>`;
}

/**
 * 把本次所有宏调用放进一个自包含的多 Tab 页面，切换时保留各图的折叠状态。
 * @param {object[]} expansions 宏展开列表。
 * @returns {string} 单个完整 HTML 文档。
 */
function renderTabs(expansions, options = {}) {
  const language = options.language;
  const tabs = expansions.map((item, index) => `<button type="button" role="tab" id="tab-${index}" aria-controls="expansion-${index}" aria-selected="${index === 0}" tabindex="${index === 0 ? 0 : -1}" data-tab="${index}">${escapeHtml(item.name || ui(language, '宏', 'Macro'))} · ${ui(language, `第 ${Number(item.line) || 1} 行`, `Line ${Number(item.line) || 1}`)}</button>`).join('');
  const views = expansions.map((item, index) => `<section role="tabpanel" id="expansion-${index}" aria-labelledby="tab-${index}"${index ? ' hidden' : ''}>${renderFigure(item, index, expansions.length, language)}</section>`).join('\n');
  return `<!DOCTYPE html>
<html lang="${ui(language, 'zh-CN', 'en')}"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1">
<title>${ui(language, `宏展开（${expansions.length} 处）`, `Macro expansions (${expansions.length})`)}</title>${pageStyle()}
<style>
  .search { display: flex; gap: 10px; align-items: center; margin-bottom: 8px; }
  .search input { flex: 1; max-width: 480px; min-width: 100px; padding: 8px 10px; border-radius: 5px;
    font: inherit; border: 1px solid var(--vscode-input-border, #8b949e); background: var(--vscode-input-background, #ffffff); color: inherit; }
  .search [role="status"] { color: var(--vscode-descriptionForeground, #57606a); }
  figcaption .toolbar { flex-wrap: wrap; align-items: center; }
  figcaption input[type="range"] { width: 100px; vertical-align: middle; }
  .tabs { position: sticky; top: 0; z-index: 1; display: flex; overflow-x: auto; gap: 4px; padding: 8px 0 12px;
    background: var(--vscode-editor-background, #ffffff); border-bottom: 1px solid var(--vscode-panel-border, #d0d7de); margin-bottom: 18px; }
  .tabs button { flex: 0 0 auto; cursor: pointer; padding: 7px 12px; font: inherit; color: inherit;
    border: 1px solid var(--vscode-panel-border, #d0d7de); border-radius: 5px; background: var(--vscode-button-secondaryBackground, #f6f8fa); }
  .tabs button[aria-selected="true"] { border-color: var(--vscode-focusBorder, #0969da); box-shadow: inset 0 -2px var(--vscode-focusBorder, #0969da); }
  [role="tabpanel"][hidden] { display: none; }
  @media (prefers-color-scheme: dark) { .tabs { background: var(--vscode-editor-background, #1e1e1e); }
    .search input { background: var(--vscode-input-background, #252a32); }
    .tabs button { background: var(--vscode-button-secondaryBackground, #31363d); } }
</style></head><body>
<div class="search"><label for="macro-search">${ui(language, '搜索宏', 'Search macros')}</label> <input id="macro-search" type="search" placeholder="${ui(language, '宏名称、文件或调用行号', 'Macro name, file or call line')}" autocomplete="off"><span id="match-count" role="status"></span></div>
<nav class="tabs" role="tablist" aria-label="${ui(language, '宏展开调用', 'Macro expansion calls')}">${tabs}</nav>
<p id="no-matches" role="status" hidden>${ui(language, '没有匹配的宏调用。', 'No matching macro calls.')}</p>
${views}
<script>
  const api = typeof acquireVsCodeApi === 'function' ? acquireVsCodeApi() : null;
  const tabs = [...document.querySelectorAll('[role="tab"]')];
  const views = [...document.querySelectorAll('[role="tabpanel"]')];
  const search = document.getElementById('macro-search');
  const count = document.getElementById('match-count');
  const empty = document.getElementById('no-matches');
  let selected = 0;
  function select(index, focus) {
    selected = index;
    tabs.forEach((tab, at) => { const active = at === index; tab.setAttribute('aria-selected', String(active)); tab.tabIndex = active ? 0 : -1; views[at].hidden = !active; });
    if (focus) { tabs[index].focus(); tabs[index].scrollIntoView({ block: 'nearest', inline: 'nearest' }); }
    if (api) api.setState({ selected: index, search: search.value });
  }
  tabs.forEach((tab, index) => {
    tab.addEventListener('click', () => select(index, false));
    tab.addEventListener('keydown', (event) => {
      const visible = tabs.map((item, at) => at).filter((at) => !tabs[at].hidden);
      const position = visible.indexOf(index);
      let next = index;
      if (event.key === 'ArrowRight') next = visible[(position + 1) % visible.length];
      else if (event.key === 'ArrowLeft') next = visible[(position + visible.length - 1) % visible.length];
      else if (event.key === 'Home') next = visible[0];
      else if (event.key === 'End') next = visible[visible.length - 1];
      else return;
      event.preventDefault(); select(next, true);
    });
  });
  function filter() {
    const query = search.value.trim().toLocaleLowerCase();
    const visible = [];
    tabs.forEach((tab, index) => { const figure = views[index].querySelector('figure');
      tab.hidden = !((figure.dataset.macro + ' ' + figure.querySelector('.site').textContent).toLocaleLowerCase().includes(query));
      if (!tab.hidden) visible.push(index);
    });
    empty.hidden = visible.length > 0;
    count.textContent = visible.length + ' / ' + tabs.length + ${JSON.stringify(ui(language, ' 处', ' matches'))};
    if (visible.length) select(visible.includes(selected) ? selected : visible[0], false);
    else { views.forEach((view) => { view.hidden = true; }); if (api) api.setState({ selected, search: search.value }); }
  }
  search.addEventListener('input', filter);
  const previous = api && api.getState();
  if (previous && Number.isInteger(previous.selected) && previous.selected >= 0 && previous.selected < tabs.length) select(previous.selected, false);
  if (previous && typeof previous.search === 'string') search.value = previous.search;
  filter();
  document.querySelectorAll('figure.macro').forEach((figure) => {
    const tree = figure.querySelector('.tree');
    const zoom = figure.querySelector('[data-zoom]');
    figure.querySelectorAll('[data-binding]').forEach((button) => button.addEventListener('click', (event) => {
      event.stopPropagation();
      figure.querySelectorAll('[data-binding-node]').forEach((node) => node.classList.toggle('binding-selected', node.dataset.bindingNode === button.dataset.binding));
    }));
    figure.querySelectorAll('[data-node-location]').forEach((button) => button.addEventListener('click', (event) => {
      event.stopPropagation();
      if (api) api.postMessage({ type: 'revealNode', index: Number(figure.dataset.index), location: JSON.parse(button.dataset.nodeLocation) });
    }));
    zoom.addEventListener('input', () => { tree.style.zoom = String(Number(zoom.value) / 100); });
    figure.querySelectorAll('button[data-action]').forEach((button) => button.addEventListener('click', () => {
      if (button.dataset.action === 'fit') {
        tree.style.zoom = '1';
        const scale = Math.max(.3, Math.min(1, (figure.querySelector('.tree-viewport').clientWidth - 24) / tree.scrollWidth));
        tree.style.zoom = String(scale); zoom.value = String(Math.round(scale * 100)); return;
      }
      figure.querySelectorAll('details').forEach((details) => { details.open = button.dataset.action === 'expand'; });
    }));
    figure.querySelector('figcaption').addEventListener('dblclick', () => {
      if (api) api.postMessage({ type: 'reveal', index: Number(figure.dataset.index) });
    });
  });
</script></body></html>`;
}

/**
 * 注册 AST 展开预览：所有调用在一个页面的多个 Tab 中展示，支持未保存源码。
 * @param {object} vscode VSCode API。
 * @param {object} context 扩展上下文。
 * @param {object} deps 当前语言客户端与即时文档同步回调。
 * @returns {void} 无。
 */
function registerMacroPreview(vscode, context, deps) {
  const language = vscode.env?.language || 'zh';
  let panel = null;
  let panelExpansions = [];
  let panelLocations = new Set();
  let latestRequest = 0;
  let source = null;
  const close = () => {
    if (panel) panel.dispose();
    panel = null;
  };
  context.subscriptions.push({ dispose() { close(); source = null; } });
  const showPreview = async (mode = 'all', atCursor = false) => {
    const editor = vscode.window.activeTextEditor;
    if (!editor || editor.document.languageId !== 'chali' || editor.document.uri.scheme !== 'file') {
      await vscode.window.showInformationMessage(ui(language, 'ChaLi：请先打开一个 .chl 源文件。', 'ChaLi: Open a .chl source file first.'));
      return;
    }
    const client = deps.getClient();
    if (!client || !client.running) {
      await vscode.window.showErrorMessage(ui(language, 'ChaLi：语言服务器尚未启动。', 'ChaLi: The language server is not running.'));
      return;
    }
    const document = editor.document;
    const request = ++latestRequest;
    try {
      deps.flushDocument(document);
      const version = document.version;
      const params = {
        textDocument: { uri: document.uri.toString() },
        mode,
      };
      if (atCursor && editor.selection?.active) params.position = { line: editor.selection.active.line, character: editor.selection.active.character };
      const result = await client.request('chali/expandMacro', params);
      if (request !== latestRequest) return;
      if (document.version !== version) throw new Error(ui(language, '预览期间源码已更新，请重新展开。', 'The source changed during preview. Expand it again.'));
      if (!result || (result.diagnostics && result.diagnostics.length)) {
        throw new Error(result && result.diagnostics ? result.diagnostics.join('\n') : '服务器未返回展开结果');
      }
      const payload = parsePayload(result.text);
      const expansions = Array.isArray(payload.expansions) ? payload.expansions : [];
      if (!expansions.length) {
        await vscode.window.showInformationMessage(ui(language, 'ChaLi：当前文件没有可展开的宏调用。', 'ChaLi: No macro calls to expand in this file.'));
        return;
      }
      source = document.uri;
      if (!panel) {
        panel = vscode.window.createWebviewPanel(
          'chali.macroExpansion',
          ui(language, '宏展开', 'Macro expansions'),
          vscode.ViewColumn.Beside,
          { enableScripts: true, retainContextWhenHidden: true, localResourceRoots: [] },
        );
        panel.webview.onDidReceiveMessage(async (message) => {
          if (!message || !['reveal', 'revealNode'].includes(message.type) || !source) return;
          const expansion = panelExpansions[message.index];
          if (!Number.isInteger(message.index) || !expansion) return;
          let target = source;
          let location = { line: expansion.line, column: expansion.column };
          if (message.type === 'revealNode') {
            if (!panelLocations.has(JSON.stringify(message.location))) return;
            location = message.location;
            target = vscode.Uri.file(location.file);
          }
          const open = await vscode.workspace.openTextDocument(target);
          const line = Math.max(0, Number(location.line) - 1);
          let character = Math.max(0, Number(location.column) - 1);
          if (typeof open.lineAt === 'function') character = Buffer.from(open.lineAt(line).text, 'utf8').subarray(0, character).toString('utf8').length;
          const position = new vscode.Position(
            line, character);
          const shown = await vscode.window.showTextDocument(open, { viewColumn: vscode.ViewColumn.One, preview: true });
          if (shown && shown.selection !== undefined) shown.selection = new vscode.Selection(position, position);
          if (shown && shown.revealRange) shown.revealRange(new vscode.Range(position, position));
        }, undefined, context.subscriptions);
        panel.onDidDispose(() => { panel = null; }, undefined, context.subscriptions);
      } else {
        panel.reveal(vscode.ViewColumn.Beside);
      }
      panelExpansions = expansions;
      panelLocations = new Set();
      const collectLocations = (node) => {
        if (!node || typeof node !== 'object') return;
        for (const key of ['source', 'definition']) {
          const location = node[key];
          if (location && typeof location.file === 'string' && location.file && !location.file.startsWith('<') && Number.isInteger(location.line) && location.line > 0 && Number.isInteger(location.column) && location.column > 0)
            panelLocations.add(JSON.stringify(location));
        }
        for (const child of node.children || []) collectLocations(child);
      };
      for (const expansion of expansions) collectLocations(expansion.tree);
      panel.title = ui(language, `宏展开（${expansions.length} 处）`, `Macro expansions (${expansions.length})`);
      panel.webview.html = renderTabs(expansions, { language });
    } catch (error) {
      await vscode.window.showErrorMessage(ui(language, `ChaLi：宏展开失败：${error.message}`, `ChaLi: Macro expansion failed: ${error.message}`));
    }
  };
  context.subscriptions.push(vscode.commands.registerCommand('chali.previewMacroExpansion', () => showPreview()));
  context.subscriptions.push(vscode.commands.registerCommand('chali.previewMacroExpansionOnce', () => showPreview('one', true)));
  context.subscriptions.push(vscode.commands.registerCommand('chali.previewMacroExpansionAtCursor', () => showPreview('all', true)));
}

module.exports = { registerMacroPreview, renderPage, renderTabs, renderNode, parsePayload };
