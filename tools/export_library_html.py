#!/usr/bin/env python3
"""导出完整离线参考手册；依赖 tools/requirements_chm.txt。

用法：python tools/export_library_html.py --output dist/ChaLi-1.0.0-api-reference.html
默认含语法、书稿、示例与 API；--api-only 可只导出 API。与 CHM 脚本共享原文，
图片内嵌在单个 HTML 中；发行时使用 --zip-output 压缩，不提供独立 HTML 附件。
"""
from pathlib import Path
import argparse
import base64
import collections
import html
import json
import re
import tempfile
import zipfile
from urllib.parse import unquote, urljoin

from bs4 import BeautifulSoup
from markdown_it import MarkdownIt

ROOT = Path(__file__).resolve().parents[1]

STYLE = r"""
:root{color-scheme:light;--ink:#233247;--muted:#627087;--line:#dce4ec;--blue:#096789;--paper:#fff}
*{box-sizing:border-box}body{margin:0;height:100vh;height:100dvh;display:grid;grid-template-rows:auto minmax(0,1fr);overflow:hidden;color:var(--ink);background:#f4f7fa;font:15px/1.75 system-ui,"Microsoft YaHei",sans-serif}
[hidden]{display:none!important}a{color:var(--blue);text-underline-offset:3px}button,input{font:inherit}button{cursor:pointer}a:focus-visible,button:focus-visible,input:focus-visible,summary:focus-visible{outline:3px solid #52a5c3;outline-offset:3px}
.site-header{display:flex;align-items:center;gap:16px;padding:17px 26px;background:#15354a;color:white;border-bottom:1px solid #28495c}.site-header h1{font-size:21px;line-height:1.4;margin:0}.site-header p{font-size:12px;color:#d1e4ee;margin:3px 0 0}.menu-toggle{display:none;background:transparent;color:white;border:1px solid #7796a9;border-radius:5px;padding:5px 10px}
.workspace{display:grid;grid-template-columns:280px minmax(0,1fr);min-height:0}.sidebar{min-height:0;display:flex;flex-direction:column;background:white;border-right:1px solid var(--line);padding:18px 12px 12px}.search-box{padding:0 8px 12px;border-bottom:1px solid var(--line)}.search-box label{display:block;font-size:12px;font-weight:650;color:var(--muted);margin-bottom:6px}.search-box input{width:100%;min-width:0;border:1px solid #b9c8d5;border-radius:6px;padding:8px 10px;background:#fff;color:var(--ink)}.search-status{font-size:12px;color:var(--muted);margin:7px 0 0;min-height:21px}.module-list{overflow:auto;min-height:0;padding-top:10px;scrollbar-gutter:stable}.module-link{display:block;padding:8px 11px;margin:2px 0;border-radius:5px;text-decoration:none;line-height:1.5}.module-link .module-name{display:block;font:600 13px/1.6 Consolas,monospace;overflow-wrap:anywhere}.module-link .module-description{display:block;font-size:12px;color:var(--muted)}.module-link:hover{background:#f0f6fa}.module-link[aria-current=page]{background:#e7f2f8;box-shadow:inset 3px 0 #16789a}
main{min-width:0;min-height:0;overflow:auto;padding:26px 30px 60px;scrollbar-gutter:stable}.page-location{max-width:1120px;margin:0 auto 12px;color:var(--muted);font-size:12px}article{max-width:1120px;margin:auto;padding:26px 32px 38px;background:var(--paper);border:1px solid var(--line);border-radius:8px;overflow-wrap:anywhere}article h1{font-size:28px;line-height:1.45;color:#125f80;margin:0 0 18px;letter-spacing:-.3px}h2{font-size:21px;line-height:1.5;border-bottom:1px solid var(--line);padding-bottom:9px;margin:34px 0 16px}h3{font-size:18px;line-height:1.5;margin:30px 0 14px;color:#174c68}h4{font-size:16px}p{margin:12px 0}li{margin:4px 0}ul,ol{padding-left:25px}code{font:13px/1.7 Consolas,"Cascadia Code",monospace;overflow-wrap:anywhere}p code,li code,td code{background:#f0f4f7;border-radius:3px;padding:2px 4px}pre{max-width:100%;padding:15px 18px;background:#f0f5f8;border:1px solid #dce6ed;border-left:3px solid #3a8ca9;border-radius:4px;white-space:pre-wrap;overflow-wrap:anywhere;tab-size:4;line-height:1.7}pre code{font-size:13px;background:none;padding:0;white-space:inherit}details{border:1px solid var(--line);border-radius:5px;padding:10px 14px;margin:16px 0}summary{cursor:pointer;color:#166585;font-weight:600}details[open]>summary{margin-bottom:12px}blockquote{margin:16px 0;padding:1px 18px;border-left:3px solid #9fb8c8;color:var(--muted);background:#f7f9fb}
.table-wrap{max-width:100%;margin:18px 0;overflow:auto;border:1px solid var(--line);border-radius:5px}table{border-collapse:collapse;table-layout:fixed;width:100%;font-size:13px;line-height:1.75}th,td{padding:10px 12px;text-align:left;vertical-align:top;border-bottom:1px solid var(--line);border-right:1px solid var(--line);overflow-wrap:anywhere;word-break:normal}th{background:#edf3f7;color:#35536a;font-weight:650}td:last-child,th:last-child{border-right:0}tbody tr:last-child td{border-bottom:0}tbody tr:nth-child(even){background:#fafcfd}td p:first-child{margin-top:0}td p:last-child{margin-bottom:0}td code{font-size:12px;white-space:normal}td::before{display:none}[id]{scroll-margin-top:20px}.empty-state{max-width:1120px;margin:auto;background:white;border:1px solid var(--line);padding:30px;border-radius:8px}.backdrop{display:none}
@media(min-width:1600px){main{padding-left:48px;padding-right:48px}}
@media(max-width:900px){.workspace{grid-template-columns:240px minmax(0,1fr)}main{padding:20px 16px 40px}article{padding:22px}article h1{font-size:25px}th,td{padding:9px}}
@media(max-width:700px){.site-header{padding:12px 14px;gap:11px}.site-header h1{font-size:17px}.site-header p{font-size:11px}.menu-toggle{display:block;flex:none}.workspace{grid-template-columns:minmax(0,1fr)}.sidebar{position:fixed;inset:0 auto 0 0;width:min(86vw,310px);z-index:30;transform:translateX(-101%);visibility:hidden;padding-top:24px;box-shadow:8px 0 30px #10263622}.nav-open .sidebar{transform:translateX(0);visibility:visible}.nav-open .backdrop{display:block;position:fixed;inset:0;z-index:25;border:0;background:#142d4580}main{padding:15px 12px 36px;scrollbar-gutter:auto}article{padding:20px 17px;border-radius:6px}article h1{font-size:23px;line-height:1.5}h2{font-size:19px;margin-top:28px}h3{font-size:17px}pre{padding:12px;font-size:12px}pre code{font-size:12px}.page-location{display:none}.table-wrap{border:0;overflow:visible;margin:18px 0}table,tbody{display:block;width:100%}colgroup,thead{display:none}tr{display:block;border:1px solid var(--line);border-radius:5px;margin:12px 0;padding:3px 12px;background:white!important}td{display:block;width:auto!important;border:0;border-bottom:1px solid #e9eef3;padding:9px 0;line-height:1.8}td:last-child{border:0}td::before{display:block;content:attr(data-label);font-size:11px;line-height:1.6;color:var(--muted);font-weight:650;margin-bottom:4px}td code{font-size:12px}}
@media(prefers-reduced-motion:reduce){.sidebar{transition:none}}
"""

SCRIPT = r"""
const articles=[...document.querySelectorAll('article')], links=[...document.querySelectorAll('.module-link')];
const main=document.querySelector('main'), input=document.querySelector('#search'), status=document.querySelector('#search-status'), empty=document.querySelector('#empty-state'), toggle=document.querySelector('#menu-toggle');
const texts=new Map(articles.map(a=>[a.id,a.textContent.toLocaleLowerCase()]));
function closeMenu(){document.body.classList.remove('nav-open');toggle.setAttribute('aria-expanded','false')}
function showArticle(article){for(const a of articles)a.hidden=a!==article;empty.hidden=!!article;for(const link of links){if(article&&link.dataset.target===article.id)link.setAttribute('aria-current','page');else link.removeAttribute('aria-current')}document.querySelector('#current-module').textContent=article?article.dataset.module:'搜索结果';}
function clearSearch(){input.value='';for(const link of links)link.hidden=false;status.textContent='50 个模块 · 支持函数名与说明搜索'}
function route(){let id;try{id=decodeURIComponent(location.hash.slice(1))||'catalog'}catch{id='catalog'}const target=document.getElementById(id), article=target?.closest('article')||document.getElementById('catalog');clearSearch();showArticle(article);closeMenu();main.scrollTop=0;if(target&&target!==article){for(let node=target.parentElement;node&&node!==article;node=node.parentElement)if(node.tagName==='DETAILS')node.open=true;requestAnimationFrame(()=>target.scrollIntoView({block:'start'}))}}
input.addEventListener('input',()=>{const value=input.value.toLocaleLowerCase().trim();let matches=[];for(const link of links){const id=link.dataset.target;const match=!value||((id==='catalog'||id==='symbols')?link.textContent.toLocaleLowerCase().includes(value):texts.get(id).includes(value));link.hidden=!match;if(match)matches.push(document.getElementById(id))}status.textContent=value?(matches.length+' 个页面匹配'):'50 个模块 · 支持函数名与说明搜索';if(!matches.some(a=>!a.hidden)){showArticle(matches[0]||null);main.scrollTop=0}});
document.addEventListener('click',event=>{const anchor=event.target.closest('a[href^="#"]');if(!anchor)return;event.preventDefault();history.pushState(null,'',anchor.getAttribute('href'));route()});
toggle.addEventListener('click',()=>{const open=document.body.classList.toggle('nav-open');toggle.setAttribute('aria-expanded',String(open));if(open)input.focus()});document.querySelector('.backdrop').addEventListener('click',closeMenu);document.addEventListener('keydown',event=>{if(event.key==='Escape')closeMenu()});window.addEventListener('hashchange',route);window.addEventListener('popstate',route);route();
"""

SCRIPT = SCRIPT.replace("'50 个模块 · 支持函数名与说明搜索'", 'document.body.dataset.searchHint')
SCRIPT = SCRIPT.replace("function closeMenu()", "function syncGroups(){const searching=!!input.value.trim();for(const group of document.querySelectorAll('.nav-group')){const visible=[...group.querySelectorAll('.module-link')].some(link=>!link.hidden);group.hidden=!visible;group.open=searching||!!group.querySelector('[aria-current=page]')}}\nfunction closeMenu()")
SCRIPT = SCRIPT.replace("document.querySelector('#current-module').textContent=article?article.dataset.module:'搜索结果';", "document.querySelector('#current-module').textContent=article?article.dataset.module:'搜索结果';syncGroups();")
SCRIPT = SCRIPT.replace("if(!matches.some(a=>!a.hidden)){showArticle(matches[0]||null);main.scrollTop=0}", "if(!matches.some(a=>!a.hidden)){showArticle(matches[0]||null);main.scrollTop=0}syncGroups();")
STYLE += '\n.module-list .nav-group{border:0;padding:0;margin:4px 0}.module-list .nav-group>summary{font-size:13px;padding:8px;color:#35536a}.module-list .nav-group .module-link{margin-left:9px}.module-list .nav-group[open]>summary{margin-bottom:0}\n'
STYLE += 'article img{display:block;max-width:100%;height:auto;margin:18px 0}.example{border:1px solid #dce4ec;padding:12px 16px;margin:16px 0}.example-title{font-weight:600;color:#166585}\n'


def render_reference(work, manifest):
    """将共享章节合并到一个 HTML，重写跨页锚点并将插图内嵌为数据地址。"""
    keys = {}
    for topic in manifest['topics']:
        key = Path(topic['file']).stem
        if topic['file'] == 'index.html': key = 'catalog'
        elif topic['category'] == '库函数 API':
            name = topic['title'].split(' — ')[0]
            key = name.replace('::', '-') if name.startswith('std') else ('api-catalog' if topic['file']=='api-00.html' else 'symbols')
        keys[topic['file']] = key
    sections = []
    groups = collections.OrderedDict()
    for topic in manifest['topics']:
        key = keys[topic['file']]
        soup = BeautifulSoup((work / topic['file']).read_text(encoding='utf-8'), 'html.parser').body
        breadcrumb = soup.find(class_='breadcrumb')
        if breadcrumb: breadcrumb.decompose()
        for node in soup.find_all(id=True): node['id'] = key + '--' + node['id']
        for node in soup.find_all('a', attrs={'name': True}):
            node['name'] = key + '--' + node['name']
            if not node.get('id'): node['id'] = node['name']
        for anchor in soup.find_all('a', href=True):
            target = anchor['href']
            if re.match(r'[a-zA-Z][a-zA-Z0-9+.-]*:', target): continue
            filename, separator, fragment = target.partition('#')
            filename = filename or topic['file']
            if filename in keys: anchor['href'] = '#' + keys[filename] + ('--' + fragment if separator and fragment else '')
        for image in soup.find_all('img', src=True):
            source = work / image['src']
            if source.is_file():
                subtype = {'.png':'png','.jpg':'jpeg','.jpeg':'jpeg','.svg':'svg+xml','.gif':'gif'}.get(source.suffix,'png')
                image['src'] = 'data:image/' + subtype + ';base64,' + base64.b64encode(source.read_bytes()).decode('ascii')
        for table in soup.find_all('table'):
            labels = [node.get_text(strip=True) for node in table.select('thead th')]
            for row in table.select('tbody tr'):
                for index, cell in enumerate(row.find_all('td', recursive=False)):
                    cell['data-label'] = labels[index] if index < len(labels) else '内容'
            wrapper = BeautifulSoup('<div class="table-wrap"></div>', 'html.parser').div
            table.wrap(wrapper)
        content = ''.join(str(node) for node in soup.contents)
        sections.append('<article id="' + key + '" data-module="' + html.escape(topic['title'],quote=True) + '" hidden>' + content + '</article>')
        name, _, description = topic['title'].partition(' — ')
        link = '<a class="module-link" data-target="' + key + '" href="#' + key + '"><span class="module-name">' + html.escape(name) + '</span>'
        if description: link += '<span class="module-description">' + html.escape(description) + '</span>'
        groups.setdefault(topic['category'], []).append(link + '</a>')
    navigation = ['<details class="nav-group"><summary>' + html.escape(name) + '</summary>' + ''.join(links) + '</details>' for name, links in groups.items()]
    return sections, navigation


def render_page(path, renderer):
    """转换页面、锚点和表格，为每个模块保留独立且可跳转的作用域。"""
    key = 'catalog' if path.stem == 'README' else path.stem
    source = path.read_text(encoding='utf-8')
    title = source.splitlines()[0].removeprefix('# ')
    source = re.sub(r'<!-- LANGUAGE NAVIGATION -->.*?<!-- END LANGUAGE NAVIGATION -->', '', source, flags=re.S)
    soup = BeautifulSoup(renderer.render(source), 'html.parser')
    slugs = {}
    for heading in soup.find_all(re.compile(r'^h[1-6]$')):
        slug = re.sub(r'[^\w\s-]', '', heading.get_text().lower()).strip().replace(' ', '-')
        count = slugs.get(slug, 0)
        slugs[slug] = count + 1
        if not heading.get('id'):
            heading['id'] = slug + ('-' + str(count) if count else '')
    for node in soup.find_all(id=True):
        node['id'] = key + '--' + node['id']
    for anchor in soup.find_all('a', href=True):
        target = html.unescape(anchor['href'])
        if target.startswith('#'):
            target = '#' + key + '--' + unquote(target[1:])
        elif not re.match(r'[a-zA-Z]+:', target):
            file, separator, fragment = target.partition('#')
            destination = (path.parent / file).resolve()
            if destination.parent == ROOT / 'doc/library' and destination.suffix == '.md':
                destination_key = 'catalog' if destination.stem == 'README' else destination.stem
                target = '#' + destination_key + ('--' + unquote(fragment) if separator else '')
            else:
                target = urljoin('https://github.com/rabbitc999/ChaLi/blob/master/' + path.relative_to(ROOT).as_posix(), target)
        anchor['href'] = target
    for table in soup.find_all('table'):
        labels = [cell.get_text(strip=True) for cell in table.select('thead th')]
        count = len(labels)
        widths = [100 / count] * count if count else []
        if labels == ['所属类型', '声明', '说明']:
            widths = [15, 43, 42]
        elif count == 4 and labels[0] == '参数':
            widths = [18, 33, 36, 13]
        elif count == 4 and labels[0] == '模块':
            widths = [27, 43, 16, 14]
        elif count == 2:
            widths = [30, 70]
        group = soup.new_tag('colgroup')
        for width in widths:
            column = soup.new_tag('col', style=f'width:{width:g}%')
            group.append(column)
        table.insert(0, group)
        for row in table.select('tbody tr'):
            for index, cell in enumerate(row.find_all('td', recursive=False)):
                cell['data-label'] = labels[index] if index < count else '内容'
        wrapper = soup.new_tag('div', attrs={'class': 'table-wrap'})
        table.wrap(wrapper)
    article = '<article id="' + key + '" data-module="' + html.escape(title, quote=True) + '" hidden>' + str(soup) + '</article>'
    name, _, description = title.partition(' — ')
    navigation = '<a class="module-link" data-target="' + key + '" href="#' + key + '"><span class="module-name">' + html.escape(name) + '</span>'
    if description:
        navigation += '<span class="module-description">' + html.escape(description) + '</span>'
    return article, navigation + '</a>'


def main():
    """按现有 API 清单生成可离线使用的自适应 HTML 手册。"""
    global ROOT
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--repo', type=Path, default=ROOT, help='ChaLi 仓库根目录；脚本单独下载时指定')
    parser.add_argument('--api-only', action='store_true', help='仅导出 API，默认包含语法、书稿和示例')
    parser.add_argument('--zip-output', type=Path, help='同时生成只含该 HTML 的压缩附件')
    args = parser.parse_args()
    ROOT = args.repo.resolve()
    if not (ROOT / 'doc/library/README.md').is_file(): parser.error('--repo 必须指向完整 ChaLi 仓库')
    version = (ROOT / 'VERSION').read_text(encoding='utf-8').strip()
    inventory = json.loads((ROOT / 'tools/chali_stdlib_api.json').read_text(encoding='utf-8'))
    module_count, function_count = len(inventory['modules']), len(inventory['functions'])
    renderer = MarkdownIt('commonmark', {'html': True}).enable('table')
    if args.api_only:
        pages = [ROOT / 'doc/library/README.md', *sorted((ROOT / 'doc/library').glob('std*.md')), ROOT / 'doc/library/symbols.md']
        sections, navigation = zip(*(render_page(path, renderer) for path in pages))
        hint = str(module_count) + ' 个模块 · 搜索 API 全文'
    else:
        import build_reference_chm as reference
        reference.ROOT = ROOT
        (ROOT / 'tmp').mkdir(exist_ok=True)
        work = Path(tempfile.mkdtemp(prefix='reference-html-', dir=ROOT / 'tmp'))
        manifest = reference.prepare(work)
        sections, navigation = render_reference(work, manifest)
        hint = str(len(sections)) + ' 页 · 搜索语法、书稿、示例与 API'
    document = '<!doctype html><html lang="zh-CN"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">'
    document += '<title>ChaLi ' + version + ' 语言与标准库参考手册</title><style>' + STYLE + '</style></head><body data-search-hint="' + html.escape(hint,quote=True) + '">'
    document += '<header class="site-header"><button class="menu-toggle" id="menu-toggle" aria-controls="module-navigation" aria-expanded="false" type="button">目录</button><div><h1>ChaLi ' + version + ' · 语言与标准库参考手册</h1><p>' + str(module_count) + ' 个模块 · ' + str(function_count) + ' 个函数重载 · 离线阅读 · 源码链接指向项目仓库</p></div></header>'
    document += '<div class="workspace"><button class="backdrop" aria-label="关闭目录" type="button"></button><nav class="sidebar" id="module-navigation" aria-label="参考目录"><div class="search-box"><label for="search">搜索语法、书稿、示例或 API</label><input id="search" type="search" placeholder="例如 verifyEd25519" autocomplete="off"><p class="search-status" id="search-status" role="status"></p></div><div class="module-list">' + ''.join(navigation) + '</div></nav>'
    document += '<main id="content"><div class="page-location">参考手册 / <span id="current-module"></span></div><div id="empty-state" class="empty-state" hidden>没有找到匹配的内容。请尝试其他函数名或关键词。</div>' + ''.join(sections) + '</main></div><noscript><style>body{height:auto;overflow:auto;display:block}.workspace{display:block}.sidebar{display:none}main{overflow:visible}article[hidden]{display:block!important;margin-bottom:24px}</style></noscript><script>' + SCRIPT.replace('50 个模块', str(module_count) + ' 个模块') + '</script></body></html>'
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(document, encoding='utf-8')
    if args.zip_output:
        args.zip_output.parent.mkdir(parents=True,exist_ok=True)
        with zipfile.ZipFile(args.zip_output,'w',zipfile.ZIP_DEFLATED,compresslevel=9) as archive:
            archive.write(args.output,arcname=args.output.name)
    print(f'已导出 {module_count} 个模块、{function_count} 个重载：{args.output}')


if __name__ == '__main__':
    main()
