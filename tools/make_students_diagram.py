"""生成学生系统架构图的 SVG 图源及供书稿排版的 PNG。"""
from pathlib import Path
from html import escape
from playwright.sync_api import sync_playwright

ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'doc/book/images'


def render(english=False):
    """按当前代码中的请求边界绘图，不把页面隐藏操作作为权限边界。"""
    labels=(['Student management system','Requests, authorization and persistent data','Browser · three role views','Login / overview / six management views','Safe text output · filters · pagination','HTTP entry · main.chl / httpd.chl','Parse bounded requests / serve HTML / return JSON','Router · school_api.chl','Authenticate before business API dispatch','Authentication · school_auth.chl','PBKDF2 verification / token / expiry / account version','Business · school_data / school_accounts','Role checks / teaching scope / validation / CRUD','Windows CNG · bcrypt.dll','System random / PBKDF2-HMAC-SHA256','MySQL · school_* tables','Users / classes / subjects / teachers / students','Teaching assignments / grades / foreign keys','Bearer token / JSON','System C ABI','Quoted literals / fixed identifiers','Sessions are held in this process; restart requires login. Existing students data is left intact.'] if english else ['学生信息管理系统','请求、权限与持久化数据的边界','浏览器 · 三角色页面','登录／总览／六类管理视图','文本安全输出 · 筛选 · 分页','HTTP 入口 · main.chl / httpd.chl','有界读取请求／返回首页与 JSON','路由 · school_api.chl','业务 API 分派前先验证会话','认证 · school_auth.chl','口令派生验证／令牌／到期／账号版本','业务 · school_data / school_accounts','角色检查／授课范围／字段校验／增删改查','Windows CNG · bcrypt.dll','系统随机数／PBKDF2-HMAC-SHA256','MySQL · school_* 表','账号／班级／科目／教师／学生','授课授权／成绩／外键约束','Bearer 令牌／JSON','系统 C ABI','值转义／固定表名列名','会话保存在当前进程中，服务重启后重新登录；保留既有 students 表及其数据。'])
    parts=['<svg xmlns="http://www.w3.org/2000/svg" width="1200" height="880" viewBox="0 0 1200 880">','<defs><marker id="arrow" markerWidth="10" markerHeight="8" refX="9" refY="4" orient="auto"><path d="M0 0L10 4L0 8" fill="none" stroke="#6d86b2" stroke-width="1.5"/></marker></defs>','<rect width="1200" height="880" fill="#f5f7fb"/>','<style>text{font-family:"Microsoft YaHei","Segoe UI",sans-serif;fill:#182c49}.title{font-size:28px;font-weight:650}.sub{font-size:15px;fill:#64748b}.heading{font-size:18px;font-weight:650}.body{font-size:14px;fill:#50657f}.label{font-size:13px;fill:#466293}.arrow{stroke:#6d86b2;stroke-width:2;fill:none;marker-end:url(#arrow)}</style>']

    def text(x,y,value,kind='body'):
        parts.append(f'<text x="{x}" y="{y}" class="{kind}">{escape(value)}</text>')

    def box(x,y,w,h,title,lines,dark=False):
        parts.append(f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="13" fill="'+('#172945' if dark else '#fff')+'" stroke="#d6e0ef"/>')
        if dark:
            parts.append(f'<text x="{x+22}" y="{y+34}" style="font-size:18px;font-weight:650;fill:white">{escape(title)}</text>')
            for i,line in enumerate(lines):parts.append(f'<text x="{x+22}" y="{y+63+i*25}" style="font-size:14px;fill:#c8d8ed">{escape(line)}</text>')
        else:
            text(x+22,y+34,title,'heading')
            for i,line in enumerate(lines):text(x+22,y+63+i*25,line)

    text(48,55,labels[0],'title');text(48,85,labels[1],'sub')
    box(50,120,360,123,labels[2],labels[3:5],True)
    box(490,120,660,123,labels[5],[labels[6]])
    parts.append('<path class="arrow" d="M410 181H490"/>');text(418,166,'HTTP','label')
    box(490,285,660,103,labels[7],[labels[8]])
    parts.append('<path class="arrow" d="M820 243V285"/>');text(835,270,labels[18],'label')
    box(50,438,510,120,labels[9],[labels[10]])
    box(640,438,510,120,labels[11],[labels[12]])
    parts.append('<path class="arrow" d="M760 388V410H305V438"/><path class="arrow" d="M820 388V410H895V438"/>')
    parts.append('<path class="arrow" d="M560 497H640"/>')
    text(571,485,'User','label')
    box(50,655,510,115,labels[13],[labels[14]])
    box(640,640,510,145,labels[15],labels[16:18])
    parts.append('<path class="arrow" d="M305 558V655"/><path class="arrow" d="M895 558V640"/><path class="arrow" d="M490 558V598H700V640"/>')
    text(320,611,labels[19],'label');text(910,608,labels[20],'label')
    text(50,835,labels[21],'sub');parts.append('</svg>')
    path=OUT/('students-architecture-en.svg' if english else 'students-architecture.svg')
    path.write_text('\n'.join(parts),encoding='utf-8')
    return path


def main():
    """通过浏览器渲染图源，保持 SVG 与 PNG 尺寸一致。"""
    OUT.mkdir(parents=True,exist_ok=True)
    paths=[render(),render(True)]
    with sync_playwright() as playwright:
        browser=playwright.chromium.launch(channel='msedge',headless=True)
        page=browser.new_page(viewport={'width':1200,'height':880},device_scale_factor=1.5)
        for path in paths:
            page.goto(path.as_uri())
            page.locator('svg').screenshot(path=str(path.with_suffix('.png')))
        browser.close()
    print('学生系统中英文架构图 SVG / PNG 已生成')


if __name__=='__main__':main()
