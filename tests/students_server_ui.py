"""用隔离响应验收重构后的学生系统 UI，后端权限由 students_server.py 验证。"""
from pathlib import Path
import json
from playwright.sync_api import sync_playwright

ROOT=Path(__file__).resolve().parents[1]


def main():
    """检查分页、筛选、文本输出、表单、失败恢复和响应式布局。"""
    html=(ROOT/'example/students-server/web/index.html').read_text(encoding='utf-8')
    rows=[dict(id=i,student_no='2026'+str(i),name=f'学生{i}',age=18,class_id=1,user_id=None) for i in range(1,24)]
    rows[-1]['name']='<img src=x onerror=alert(1)>'
    data={'students':rows,'classes':[dict(id=1,code='C1',name='一班')],'subjects':[],'teachers':[],'assignments':[],'grades':[],'accounts':[dict(id=1,username='admin',role='admin',enabled=1)]}
    state={'fail':False,'saved':[]}
    with sync_playwright() as playwright:
        browser=playwright.chromium.launch(channel='msedge',headless=True)
        page=browser.new_page(viewport={'width':1440,'height':1000})
        errors=[];page.on('pageerror',lambda error:errors.append(str(error)))

        def respond(route):
            request=route.request
            path=request.url.split('school.test',1)[1]
            if path=='/':route.fulfill(status=200,content_type='text/html',body=html);return
            if path=='/api/auth/login':result={'token':'a'*64,'user':dict(id=1,username='admin',role='admin')}
            elif request.method=='GET':
                if state['fail']:route.fulfill(status=503,content_type='application/json',body=json.dumps({'error':'数据库暂不可用'}));return
                result=data[path.removeprefix('/api/')]
            elif request.method=='POST' and path=='/api/students':
                value=json.loads(request.post_data);state['saved'].append(value);rows.append(dict(value,id=24));result={'ok':True}
            else:result={'ok':True}
            route.fulfill(status=200,content_type='application/json',body=json.dumps(result))

        page.route('http://school.test/**',respond)
        page.goto('http://school.test/')
        page.get_by_label('账号',exact=True).fill('admin')
        page.get_by_label('口令',exact=True).fill('test-password')
        page.get_by_role('button',name='登录工作台',exact=True).click()
        page.get_by_text('数据已更新',exact=True).wait_for()
        page.locator('#navigation').get_by_text('学生管理',exact=True).click()
        assert page.locator('#rows tr').count()==10
        assert page.locator('#stat-students').inner_text()=='23'
        assert page.locator('#rows img').count()==0
        assert '<img src=x' in page.locator('#rows').inner_text()
        page.get_by_role('button',name='下一页',exact=True).click()
        assert page.locator('#page-number').inner_text()=='2 / 3'
        page.get_by_role('button',name='下一页',exact=True).click()
        assert page.locator('#rows tr').count()==3
        page.get_by_role('button',name='姓名',exact=True).click()
        assert page.locator('th[aria-sort=ascending]').inner_text().startswith('姓名')
        page.get_by_role('button',name='姓名',exact=True).click()
        assert page.locator('th[aria-sort=descending]').inner_text().startswith('姓名')
        page.get_by_label('搜索当前列表',exact=True).fill('<img')
        page.get_by_role('button',name='搜索',exact=True).click()
        assert page.locator('#rows tr').count()==1 and '<img' in page.locator('#rows').inner_text()
        page.get_by_role('button',name='重置',exact=True).click()
        page.get_by_role('button',name='＋ 新增学生',exact=True).click()
        page.get_by_label('学号',exact=True).fill('202624')
        page.get_by_label('姓名',exact=True).fill('新同学')
        page.get_by_label('年龄',exact=True).fill('20')
        page.get_by_label('班级',exact=True).select_option('1')
        page.get_by_role('button',name='保存',exact=True).click()
        page.wait_for_function("!document.querySelector('#mask').classList.contains('show')")
        page.wait_for_function("document.querySelector('#stat-students').textContent === '24'")
        assert page.locator('#form-fields input').count()==0
        assert state['saved'][-1]['class_id']==1
        state['fail']=True
        page.get_by_role('button',name='↻ 刷新',exact=True).click()
        page.locator('#load-error.show').wait_for()
        assert '数据库暂不可用' in page.locator('#load-error').inner_text()
        state['fail']=False
        page.get_by_role('button',name='↻ 刷新',exact=True).click()
        page.get_by_text('数据已更新',exact=True).wait_for()
        for width in (1440,390):
            page.set_viewport_size({'width':width,'height':1000})
            assert page.evaluate('document.documentElement.scrollWidth <= innerWidth'),width
        assert not errors,errors
        browser.close()
    print('UI 隔离验收通过：登录、分页、排序、筛选、XSS 文本、关联表单、保存、错误恢复、桌面与窄屏')


if __name__=='__main__':main()
