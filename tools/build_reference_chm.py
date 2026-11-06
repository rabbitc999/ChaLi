#!/usr/bin/env python3
"""生成包含语言参考、常见用法、中文书稿和标准库 API 的 CHM。

依赖：Python 3、tools/requirements_chm.txt，以及 Microsoft HTML Help Workshop
的 hhc.exe。可用 --prepare-only 在任意平台生成工程，再在 Windows 上编译。
所有构建中间文件位于仓库 tmp/，不会将 HTML 作为独立发行附件。
"""
from pathlib import Path
import argparse
import collections
import html
import hashlib
import json
import os
import re
import shutil
import subprocess
import tempfile
from urllib.parse import unquote, urljoin

from bs4 import BeautifulSoup
from markdown_it import MarkdownIt

ROOT = Path(__file__).resolve().parents[1]
STYLE = """
body{font-family:'Microsoft YaHei',Segoe UI,Arial,sans-serif;font-size:15px;line-height:1.8;color:#243648;background:#fff;margin:24px 30px;word-wrap:break-word}
a{color:#096789}h1{font-size:26px;line-height:1.5;color:#125f80;margin:18px 0}h2{font-size:21px;border-bottom:1px solid #dce4ec;padding-bottom:8px;margin-top:30px}h3{font-size:18px;margin-top:26px}h4{font-size:16px}.breadcrumb{font-size:12px;color:#627087;border-bottom:1px solid #dce4ec;padding-bottom:10px}
code{font-family:Consolas,'Courier New',monospace;font-size:13px;word-wrap:break-word}pre{background:#f0f5f8;border:1px solid #dce6ed;border-left:3px solid #3a8ca9;padding:14px 16px;white-space:pre-wrap;word-wrap:break-word;line-height:1.65}pre code{white-space:pre-wrap}table{border-collapse:collapse;table-layout:fixed;width:100%;font-size:13px;line-height:1.8;margin:18px 0}th,td{border:1px solid #dce4ec;padding:9px 11px;text-align:left;vertical-align:top;word-wrap:break-word}th{background:#edf3f7}td code{font-size:12px}td p{margin:4px 0}.example{border:1px solid #dce4ec;padding:12px 16px;margin:16px 0}.example-title{font-weight:bold;color:#166585}blockquote{border-left:3px solid #9fb8c8;padding:1px 16px;margin:16px 0;background:#f7f9fb}img{max-width:100%;height:auto}li{margin:5px 0}
"""


def slug(text):
    """生成与常见 Markdown 锚点兼容的章节标识。"""
    return re.sub(r'[^\w\s-]', '', text.lower()).strip().replace(' ', '-')


def document(title, body):
    """使用帮助查看器兼容的 HTML 和 CSS 包装正文，不依赖脚本。"""
    return ('<!DOCTYPE html><html><head><meta http-equiv="Content-Type" content="text/html; charset=utf-8">'
            '<meta http-equiv="X-UA-Compatible" content="IE=edge"><title>' + html.escape(title) +
            '</title><link rel="stylesheet" type="text/css" href="style.css"></head><body>'
            '<div class="breadcrumb"><a href="index.html">手册首页</a> / ' + html.escape(title) + '</div>' + body + '</body></html>')


def sitemap_item(name, local, children=''):
    """输出 CHM 目录或索引节点，名称使用字符引用以兼容编译器编码。"""
    node = '<LI><OBJECT type="text/sitemap"><param name="Name" value="' + html.escape(name, quote=True) + '"><param name="Local" value="' + html.escape(local, quote=True) + '"></OBJECT>'
    return node + ('<UL>' + children + '</UL>' if children else '') + '</LI>'


def prepare(work):
    """从仓库原文构造帮助工程，并校验全部内部页面和片段链接。"""
    renderer = MarkdownIt('commonmark', {'html': True}).enable('table')
    api = json.loads((ROOT / 'tools/chali_stdlib_api.json').read_text(encoding='utf-8'))
    groups = collections.OrderedDict()
    groups['语言与工具参考'] = [ROOT / 'doc/README.md', *sorted(p for p in (ROOT / 'doc').glob('*.md') if p.name != 'README.md')]
    groups['库函数 API'] = [ROOT / 'doc/library/README.md', *sorted((ROOT / 'doc/library').glob('std*.md')), ROOT / 'doc/library/symbols.md']
    groups['中文书稿'] = [ROOT / 'doc/book/README.md', *sorted(p for p in (ROOT / 'doc/book').glob('*.md') if p.name != 'README.md')]
    pages = {}
    for category, paths in groups.items():
        prefix = {'语言与工具参考': 'guide', '库函数 API': 'api', '中文书稿': 'book'}[category]
        for number, path in enumerate(paths):
            title = path.read_text(encoding='utf-8').splitlines()[0].lstrip('# ')
            pages[path.resolve()] = dict(file=f'{prefix}-{number:02d}.html', title=title, category=category)
    resources = {}
    sources = {}
    headings = {}
    for path, page in list(pages.items()):
        text = re.sub(r'<!-- LANGUAGE NAVIGATION -->.*?<!-- END LANGUAGE NAVIGATION -->', '', path.read_text(encoding='utf-8'), flags=re.S)
        rendered = renderer.render(text)
        allowed = set('a p h1 h2 h3 h4 h5 h6 div span pre code table colgroup col thead tbody tfoot tr th td ul ol li blockquote hr br em strong s del sup sub img details summary dl dt dd kbd samp var'.split())
        # 普通段落中的 List<T> 等类型写法不是 HTML 标签，避免旧 HTML 引擎吞掉它们。
        rendered = re.sub(r'<!--.*?-->', '', rendered, flags=re.S)
        rendered = re.sub(r'</?([^>]*?)>', lambda match: match[0] if (match[1].split(None, 1) or [''])[0].rstrip('/').lower() in allowed else html.escape(match[0]), rendered)
        soup = BeautifulSoup(rendered, 'html.parser')
        counts = {}
        outline = []
        for heading in soup.find_all(re.compile(r'^h[1-6]$')):
            key = slug(heading.get_text())
            number = counts.get(key, 0)
            counts[key] = number + 1
            if not heading.get('id'):
                heading['id'] = key + ('-' + str(number) if number else '')
            if heading.name == 'h2': outline.append((heading.get_text(), heading['id']))
        headings[page['file']] = outline
        for details in list(soup.find_all('details')):
            details.name = 'div'
            details['class'] = 'example'
            summary = details.find('summary')
            if summary: summary.name = 'p'; summary['class'] = 'example-title'
        for table in soup.find_all('table'):
            labels = [cell.get_text(strip=True) for cell in table.select('thead th')]
            widths = [100 / len(labels)] * len(labels) if labels else []
            if labels == ['所属类型', '声明', '说明']: widths = [15, 43, 42]
            elif len(labels) == 4 and labels[0] == '参数': widths = [18, 33, 36, 13]
            elif len(labels) == 4 and labels[0] == '模块': widths = [27, 43, 16, 14]
            elif len(labels) == 2: widths = [30, 70]
            group = soup.new_tag('colgroup')
            for width in widths: group.append(soup.new_tag('col', width=f'{width:g}%'))
            table.insert(0, group)
        for node in soup.find_all(['a', 'img']):
            attribute = 'href' if node.name == 'a' else 'src'
            target = node.get(attribute)
            if not target or re.match(r'[a-zA-Z][a-zA-Z0-9+.-]*:', target): continue
            if target.startswith('#'):
                node[attribute] = '#' + unquote(target[1:]); continue
            filename, separator, fragment = target.partition('#')
            destination = (path.parent / unquote(filename)).resolve()
            if destination == ROOT / 'doc/book/latex/book.pdf':
                destination = (ROOT / 'doc/book/README.md').resolve()
                node.string = 'CHM 内书稿'
            suffix = '#' + unquote(fragment) if separator else ''
            if destination in pages:
                node[attribute] = pages[destination]['file'] + suffix
            elif destination.is_file() and destination.is_relative_to(ROOT) and destination.suffix == '.chl':
                if destination not in sources: sources[destination] = f'source-{len(sources):03d}.html'
                node[attribute] = sources[destination] + suffix
            elif node.name == 'img' and destination.is_file() and destination.is_relative_to(ROOT):
                if destination not in resources: resources[destination] = f'image-{len(resources):03d}' + destination.suffix
                node[attribute] = resources[destination]
            else:
                node[attribute] = urljoin('https://github.com/rabbitc999/ChaLi/blob/master/' + path.relative_to(ROOT).as_posix(), target)
        (work / page['file']).write_text(document(page['title'], str(soup)), encoding='utf-8')
    for path, filename in resources.items(): shutil.copy2(path, work / filename)
    for path, filename in sources.items():
        source = path.read_text(encoding='utf-8')
        lines = ''.join('<span id="L' + str(index) + '">' + html.escape(line) + '</span>\n' for index, line in enumerate(source.splitlines(), 1))
        title = path.relative_to(ROOT).as_posix()
        (work / filename).write_text(document(title, '<h1>' + html.escape(title) + '</h1><pre><code>' + lines + '</code></pre>'), encoding='utf-8')
    (work / 'style.css').write_text(STYLE, encoding='utf-8')
    lookup = lambda path: pages[(ROOT / path).resolve()]['file']
    quick_links = [('第一个程序', 'doc/book/01-第一个程序.md'), ('常见语法与完整小程序', 'doc/02-language-tour.md'), ('语言参考', 'doc/04-language-reference.md'), ('数组、元组与容器', 'doc/book/13-容器.md'), ('声明简写', 'doc/29-declaration-sugars.md'), ('常见错误速查', 'doc/book/A-常见错误速查.md')]
    usage_links = [('字符串与文本', 'doc/book/12-字符串与文本.md'), ('数组、列表与元组', 'doc/book/13-容器.md'), ('错误处理', 'doc/book/14-错误处理.md'), ('迭代器与管线', 'doc/book/20-迭代器.md'), ('文件与命令行：日志检索项目', 'doc/book/16-日志检索工具.md'), ('标准库常用程序', 'doc/book/30-标准库初探.md'), ('GUI', 'doc/library/std-gui.md'), ('音频', 'doc/library/std-audio.md'), ('XML', 'doc/library/std-xml.md'), ('密码学', 'doc/library/std-crypto.md')]
    def link_list(items):
        """将常用入口映射到手册中的实际页面。"""
        return '<ul>' + ''.join('<li><a href="' + lookup(path) + '">' + html.escape(title) + '</a></li>' for title, path in items) + '</ul>'
    version = (ROOT / 'VERSION').read_text(encoding='utf-8').strip()
    title = 'ChaLi ' + version + ' 语言与标准库参考手册'
    landing = '<h1>' + title + '</h1><p>按语法、用法、库函数与书稿查阅。示例、插图、目录与索引均随手册保存，可离线阅读。</p><p>覆盖 ' + str(len(api['modules'])) + ' 个标准库模块、' + str(len(api['functions'])) + ' 个函数重载，以及完整中文书稿。</p><h2 id="syntax">常见语法</h2>' + link_list(quick_links) + '<h2 id="usage">常见用法</h2>' + link_list(usage_links) + '<h2 id="reference">完整参考</h2>' + link_list([('语言与工具参考目录', 'doc/README.md'), ('标准库 API 目录', 'doc/library/README.md'), ('中文书稿目录', 'doc/book/README.md')])
    (work / 'index.html').write_text(document(title, landing), encoding='utf-8')
    toc = sitemap_item('手册首页', 'index.html')
    toc += sitemap_item('常见语法', 'index.html#syntax', ''.join(sitemap_item(name, lookup(path)) for name, path in quick_links))
    toc += sitemap_item('常见用法', 'index.html#usage', ''.join(sitemap_item(name, lookup(path)) for name, path in usage_links))
    for category, paths in groups.items():
        children = ''
        for path in paths:
            page = pages[path.resolve()]
            sections = ''.join(sitemap_item(name, page['file'] + '#' + anchor) for name, anchor in headings[page['file']])
            children += sitemap_item(page['title'], page['file'], sections)
        toc += sitemap_item(category, pages[paths[0].resolve()]['file'], children)
    if sources:
        toc += sitemap_item('示例源码', 'index.html#usage', ''.join(sitemap_item(path.relative_to(ROOT).as_posix(), filename) for path, filename in sources.items()))
    keywords = []
    for row in [*api['functions'], *api['symbols']]:
        local = lookup('doc/library/' + row['page']) + '#' + row['anchor']
        full_name = row['module'] + '::' + row['name']
        keywords.append((full_name, local))
        keywords.append((row['name'].split('::')[-1], local))
    for page in pages.values(): keywords.append((page['title'], page['file']))
    index = ''.join(sitemap_item(name, local) for name, local in sorted(set(keywords)))
    for filename, content in [('contents.hhc', toc), ('keywords.hhk', index)]:
        markup = '<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML//EN"><HTML><HEAD><meta http-equiv="Content-Type" content="text/html; charset=utf-8"></HEAD><BODY><UL>' + content + '</UL></BODY></HTML>'
        (work / filename).write_text(markup, encoding='utf-8')
    topic_files = sorted(p.name for p in work.iterdir() if p.suffix in ('.html', '.css', '.png', '.jpg', '.jpeg', '.svg', '.gif'))
    project = '[OPTIONS]\nCompatibility=1.1 or later\nCompiled file=reference.chm\nContents file=contents.hhc\nIndex file=keywords.hhk\nDefault topic=index.html\nDefault Window=main\nTitle=' + title + '\nLanguage=0x804 Chinese (PRC)\nFull-text search=Yes\nBinary TOC=No\nBinary Index=Yes\nDisplay compile progress=No\n\n[WINDOWS]\nmain="' + title + '","contents.hhc","keywords.hhk","index.html","index.html",,,,,0x63520,280,0x104e,[0,0,1200,860],0xcf0000,,,,,0\n\n[FILES]\n' + '\n'.join(topic_files) + '\n'
    (work / 'reference.hhp').write_text(project, encoding='utf-8')
    documents = {path.name: BeautifulSoup(path.read_text(encoding='utf-8'), 'html.parser') for path in work.glob('*.html')}
    mappings = {}
    for filename, parsed in documents.items():
        original = {node['id'] for node in parsed.find_all(id=True)} | {node['name'] for node in parsed.find_all('a', attrs={'name': True})}
        mappings[filename] = {key: key if key.isascii() else 'section-' + hashlib.sha256(key.encode('utf-8')).hexdigest()[:16] for key in original}
        for node in parsed.find_all(id=True): node['id'] = mappings[filename][node['id']]
        for node in parsed.find_all('a', attrs={'name': True}): node['name'] = mappings[filename][node['name']]
    # CHM 的 Local 路径和锚点使用 ASCII，中文仍保留在正文、目录标题和索引中。
    for path in [*work.glob('*.html'), work / 'contents.hhc', work / 'keywords.hhk']:
        parsed = documents.get(path.name) or BeautifulSoup(path.read_text(encoding='utf-8'), 'html.parser')
        nodes = [(node, 'href') for node in parsed.find_all('a', href=True)]
        nodes += [(node, 'value') for node in parsed.find_all('param', attrs={'name': 'Local'})]
        for node, attribute in nodes:
            target = node[attribute]
            if re.match(r'[a-zA-Z][a-zA-Z0-9+.-]*:', target): continue
            filename, separator, fragment = unquote(target).partition('#')
            if separator and fragment in mappings.get(filename or path.name, {}):
                node[attribute] = filename + '#' + mappings[filename or path.name][fragment]
        path.write_text(str(parsed), encoding='utf-8')
    ids = {filename: set(mapping.values()) for filename, mapping in mappings.items()}
    broken = []
    for path in [*work.glob('*.html'), work / 'contents.hhc', work / 'keywords.hhk']:
        parsed = BeautifulSoup(path.read_text(encoding='utf-8'), 'html.parser')
        links = [node.get('href', node.get('src')) for node in parsed.find_all(['a', 'img'])]
        links += [node.get('value') for node in parsed.find_all('param', attrs={'name': 'Local'})]
        for target in links:
            if not target or re.match(r'[a-zA-Z][a-zA-Z0-9+.-]*:', target): continue
            filename, separator, fragment = unquote(target).partition('#')
            filename = filename or path.name
            if not (work / filename).is_file() or (separator and fragment and fragment not in ids.get(filename, set())): broken.append((path.name, target))
    if broken: raise ValueError('CHM 内部链接无效：' + repr(broken[:20]))
    topics = [dict(file='index.html', title='手册首页', category='开始阅读'), *pages.values()]
    topics += [dict(file=filename, title=path.relative_to(ROOT).as_posix(), category='示例源码') for path, filename in sources.items()]
    manifest = dict(version=version,modules=len(api['modules']),overloads=len(api['functions']),guidePages=len(groups['语言与工具参考']),bookPages=len(groups['中文书稿']),apiPages=len(groups['库函数 API']),sourceExamples=len(sources),images=len(resources),htmlPages=len(ids),keywordTargets=len(set(keywords)),internalLinksValid=True,compilerFiles=topic_files,topics=topics)
    (work / 'manifest.json').write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    return manifest


def portable_compiler(compiler, work):
    """在工程内准备免注册 COM 编译器，避免修改系统注册表或请求管理员权限。"""
    required = ['hhc.exe', 'hha.dll', 'itcc.dll']
    if not all((compiler.parent / name).is_file() for name in required):
        return compiler
    folder = work / 'compiler'
    folder.mkdir()
    for name in required: shutil.copy2(compiler.parent / name, folder / name)
    classes = {'4662DAA2-D393-11D0-9A56-00C04FB68BF7': 'ITIR.CmdInt', '4662DAA3-D393-11D0-9A56-00C04FB68BF7': 'ITIR.SvMgr', '4662DAA4-D393-11D0-9A56-00C04FB68BF7': 'ITIR.GroupBuild', '8FA0D5A9-DEDF-11D0-9A61-00C04FB68BF7': 'ITIR.SystemWordSink', '8FA0D5AA-DEDF-11D0-9A61-00C04FB68BF7': 'ITIR.FTIBuild', '8FA0D5AB-DEDF-11D0-9A61-00C04FB68BF7': 'ITIR.WWFilterBuild'}
    # 全文索引器还按 ProgID 查找组件，仅声明 CLSID 会生成缺少搜索数据的 CHM。
    manifest = '<assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0"><assemblyIdentity type="win32" name="ChaLi.HtmlHelpCompiler" version="1.0.0.0" processorArchitecture="x86"/><file name="itcc.dll">' + ''.join('<comClass clsid="{' + identifier + '}" threadingModel="Both" progid="' + name + '"><progid>' + name + '.4</progid></comClass>' for identifier, name in classes.items()) + '</file></assembly>'
    (folder / 'hhc.exe.manifest').write_text(manifest, encoding='utf-8')
    return folder / 'hhc.exe'


def encode_chm_project(work):
    """按中文 CHM 的 CP936 编码正文与工程，保证旧全文索引器正确处理中文。"""
    for project in [*work.glob('*.html'), work / 'reference.hhp', work / 'contents.hhc', work / 'keywords.hhk']:
        project.write_bytes(project.read_text(encoding='utf-8').replace('charset=utf-8', 'charset=gb2312').encode('gbk', errors='xmlcharrefreplace'))


def main():
    """生成工程并调用可用编译器，将通过检查的 CHM 复制到交付位置。"""
    global ROOT
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--repo', type=Path, default=ROOT, help='ChaLi 仓库根目录；脚本放在仓库外时指定')
    parser.add_argument('--compiler', help='Microsoft HTML Help Workshop hhc.exe 的路径')
    parser.add_argument('--work-dir', type=Path, help='空的工程目录，默认在仓库 tmp 内新建唯一目录')
    parser.add_argument('--prepare-only', action='store_true')
    args = parser.parse_args()
    ROOT = args.repo.resolve()
    if not (ROOT / 'doc/library/README.md').is_file(): parser.error('--repo 必须指向完整 ChaLi 仓库')
    (ROOT / 'tmp').mkdir(exist_ok=True)
    work = args.work_dir.resolve() if args.work_dir else Path(tempfile.mkdtemp(prefix='reference-chm-',dir=ROOT / 'tmp'))
    if not work.is_relative_to((ROOT / 'tmp').resolve()): parser.error('--work-dir 必须位于仓库 tmp 内')
    if work.exists() and any(work.iterdir()): parser.error('构建目录非空，请使用新的 --work-dir 或清理上次的目录')
    work.mkdir(parents=True, exist_ok=True)
    environment = dict(os.environ,TMPDIR=str(ROOT / 'tmp'),TMP=str(ROOT / 'tmp'),TEMP=str(ROOT / 'tmp'))
    manifest = prepare(work)
    if args.prepare_only:
        encode_chm_project(work)
        print(json.dumps(manifest, ensure_ascii=False, indent=2)); print('工程目录：'+str(work)); return
    candidates = [ROOT / 'build/dependencies/html-help-workshop/hhc.exe']
    candidates += [Path(os.environ.get(name, 'C:/Program Files (x86)')) / 'HTML Help Workshop/hhc.exe' for name in ('ProgramFiles(x86)', 'ProgramFiles')]
    compiler = args.compiler or shutil.which('hhc.exe') or next((str(path) for path in candidates if path.is_file()), None)
    if not compiler: parser.error('未找到 hhc.exe；可用 --compiler 指定，或先用 --prepare-only 生成工程')
    if os.name != 'nt': parser.error('实际编译需要 Windows；当前平台可使用 --prepare-only')
    compiler = Path(shutil.which(compiler) or compiler).resolve()
    if not compiler.is_file(): parser.error('编译器不存在：'+str(compiler))
    compiler = portable_compiler(compiler, work)
    command = [str(compiler), 'reference.hhp']
    encode_chm_project(work)
    result = subprocess.run(command,cwd=work,env=environment,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
    (work / 'compile.log').write_bytes(result.stdout)
    binary = work / 'reference.chm'
    # hhc.exe 成功返回 1；仍须同时核对编译诊断和 CHM 文件头。
    compile_text = result.stdout.decode('utf-8', errors='replace')
    data = binary.read_bytes() if binary.is_file() else b''
    if result.returncode != 1 or re.search(r'(?im)^\s*(?:Error:|Fatal:|HHC\d+:\s*Error:|HHC3004:\s*Warning:)', compile_text) or data[:4] != b'ITSF' or b'/$FIftiMain' not in data:
        raise RuntimeError('CHM 编译失败：' + result.stdout.decode('utf-8', errors='replace')[-4000:])
    args.output.parent.mkdir(parents=True,exist_ok=True)
    shutil.copy2(binary,args.output)
    manifest.update(compiled=True, compiler='Microsoft HTML Help Compiler', fullTextSearch=True, sha256=hashlib.sha256(data).hexdigest())
    (work / 'manifest.json').write_text(json.dumps(manifest,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    print(json.dumps({key:value for key,value in manifest.items() if key not in ('compilerFiles','topics')},ensure_ascii=False,indent=2))
    print('工程目录：'+str(work))
    print('已编译：'+str(args.output))


if __name__ == '__main__':
    main()
