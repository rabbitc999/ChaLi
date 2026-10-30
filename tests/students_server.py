"""在独立 MySQL 库运行学生系统 HTTP、三角色权限和浏览器验收。"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import socket
import subprocess
import time
import urllib.error
import urllib.request
import uuid
import pymysql
from playwright.sync_api import sync_playwright

ROOT = Path(__file__).resolve().parents[1]


def main():
    """只创建并删除本次唯一命名的测试库，保留报告而不接触业务库。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--app', required=True, type=Path)
    parser.add_argument('--mysql-host', default='127.0.0.1')
    parser.add_argument('--mysql-user', default='root')
    parser.add_argument('--password-env', default='CHALI_TEST_MYSQL_PASSWORD')
    parser.add_argument('--output', type=Path, default=ROOT / 'tmp/students-server')
    args = parser.parse_args()
    password = os.environ.get(args.password_env)
    if password is None:
        raise SystemExit(f'请通过 {args.password_env} 提供测试库凭据')
    work = args.output.resolve()
    assert work.is_relative_to(ROOT / 'tmp')
    work.mkdir(parents=True, exist_ok=True)
    database = 'chali_school_test_' + uuid.uuid4().hex
    assert re.fullmatch(r'chali_school_test_[0-9a-f]{32}', database)
    connection = pymysql.connect(host=args.mysql_host, user=args.mysql_user, password=password, autocommit=True)
    process = None
    log = (work / 'server.log').open('wb')
    checks = []

    def check(condition, label):
        assert condition, label
        checks.append(label)
        print('通过', label, flush=True)

    try:
        with connection.cursor() as cursor:
            cursor.execute('CREATE DATABASE `' + database + '` CHARACTER SET utf8mb4')
        config = dict(host=args.mysql_host,user=args.mysql_user,password=password,database=database,port=3306)
        config_path = work / 'config.json'
        config_path.write_text(json.dumps(config), encoding='utf-8')
        invalid=subprocess.run([str(args.app.resolve()),'4294976083',str(ROOT/'example/students-server/web'),str(config_path)],cwd=ROOT,capture_output=True,text=True,encoding='utf-8',timeout=15)
        check('1–65535' in invalid.stderr,'监听端口拒绝整数截断后伪装的合法值')
        config_path.write_text(json.dumps(dict(config,port=65536)),encoding='utf-8')
        invalid=subprocess.run([str(args.app.resolve()),'8787',str(ROOT/'example/students-server/web'),str(config_path)],cwd=ROOT,capture_output=True,text=True,encoding='utf-8',timeout=15)
        check('数据库端口必须在' in invalid.stderr,'数据库端口范围检查')
        config_path.write_text(json.dumps(config),encoding='utf-8')
        with socket.socket() as available:
            available.bind(('127.0.0.1', 0))
            port = available.getsockname()[1]
        base = f'http://127.0.0.1:{port}'
        env = dict(os.environ, TMPDIR=str(ROOT/'tmp'),TMP=str(ROOT/'tmp'),TEMP=str(ROOT/'tmp'))
        process = subprocess.Popen([str(args.app.resolve()),str(port),str(ROOT/'example/students-server/web'),str(config_path)],cwd=ROOT,env=env,stdout=log,stderr=subprocess.STDOUT,creationflags=subprocess.CREATE_NO_WINDOW if os.name=='nt' else 0)

        def http(method,path,body=None,token=None):
            headers={'Content-Type':'application/json'}
            if token: headers['Authorization']='Bearer '+token
            request=urllib.request.Request(base+path,method=method,headers=headers,data=None if body is None else json.dumps(body).encode())
            try:
                with urllib.request.urlopen(request,timeout=40) as response:
                    payload=response.read().decode('utf-8')
                    return response.status,json.loads(payload) if response.headers.get_content_type()=='application/json' else payload
            except urllib.error.HTTPError as error:
                return error.code,json.loads(error.read().decode('utf-8'))

        for _ in range(120):
            if process.poll() is not None: raise AssertionError((work/'server.log').read_text(encoding='utf-8',errors='replace'))
            try:
                if http('GET','/')[0]==200: break
            except OSError: pass
            time.sleep(.25)
        else: raise AssertionError('服务未就绪')
        connection.select_db(database)
        test_password='School-test-password-2026'
        with connection.cursor() as cursor:
            cursor.execute('SELECT COUNT(*) FROM school_users')
            check(cursor.fetchone()[0]==1,'空库只初始化管理员，不插入业务演示数据')
        bootstrap_log=(work/'server.log').read_text(encoding='utf-8',errors='replace')
        match=re.search(r'一次性初始口令：([0-9a-f]{64})',bootstrap_log)
        check(match is not None,'重定向日志中也及时显示随机初始口令')
        bootstrap_password=match[1]
        initial_status,initial=http('POST','/api/auth/login',dict(username='admin',password=bootstrap_password))
        check(initial_status==200,'随机初始口令可登录')
        check(http('POST','/api/auth/password',dict(oldPassword=bootstrap_password,password=test_password),initial['token'])[0]==200,'初次登录后更换随机口令')
        with connection.cursor() as cursor:
            cursor.execute('SELECT salt,digest FROM school_users WHERE username=%s',('admin',))
            salt,digest=cursor.fetchone()
            check(digest==hashlib.pbkdf2_hmac('sha256',test_password.encode(),salt.encode(),600000).hex(),'真实账号摘要与独立 PBKDF2 实现一致')

        def login(name):
            status,value=http('POST','/api/auth/login',dict(username=name,password=test_password))
            check(status==200 and len(value.get('token',''))==64,'登录 '+name)
            return value['token']

        admin=login('admin')
        for entity in ('students','classes','grades','subjects','teachers','accounts','assignments'):
            check(http('GET','/api/'+entity)[0]==401,'未登录拒绝 '+entity)

        def listing(entity,token=admin):
            status,value=http('GET','/api/'+entity,token=token)
            assert status==200,(entity,status,value)
            return value

        def create(entity,values,match,token=admin):
            status,value=http('POST','/api/'+entity,values,token)
            assert status==200,(entity,status,value)
            return next(row for row in listing(entity,token) if all(row[key]==item for key,item in match.items()))

        classes=[create('classes',dict(code='C'+str(i),name='一班' if i==1 else '二班'),dict(code='C'+str(i))) for i in (1,2)]
        subjects=[create('subjects',dict(code='S'+str(i),name='数学' if i==1 else '物理'),dict(code='S'+str(i))) for i in (1,2)]
        users={}
        for name,role in [('teacher_a','teacher'),('teacher_b','teacher'),('student_a','student'),('student_b','student')]:
            users[name]=create('accounts',dict(username=name,role=role,enabled=1,password=test_password),dict(username=name))
        teachers=[create('teachers',dict(teacher_no='T'+str(i),name='林老师' if i==1 else '周老师',user_id=users['teacher_'+letter]['id']),dict(teacher_no='T'+str(i))) for i,letter in [(1,'a'),(2,'b')]]
        students=[create('students',dict(student_no='202600'+str(i),name='陈同学' if i==1 else '许同学',age=18,class_id=classes[i-1]['id'],user_id=users['student_'+letter]['id']),dict(student_no='202600'+str(i))) for i,letter in [(1,'a'),(2,'b')]]
        for i in range(2):create('assignments',dict(teacher_id=teachers[i]['id'],class_id=classes[i]['id'],subject_id=subjects[i]['id']),dict(teacher_id=teachers[i]['id']))
        grade_bodies=[dict(student_id=students[i]['id'],subject_id=subjects[i]['id'],teacher_id=teachers[i]['id'],term='2026 秋季',score=91.5+i) for i in range(2)]
        grades=[create('grades',body,dict(student_id=body['student_id'])) for body in grade_bodies]
        # 各板块独立的有效更新与删除，不依靠失败路径代替 CRUD 验收。
        fixtures=[
            ('classes',dict(code='TEMP_C',name='临时班级'),'code'),
            ('subjects',dict(code='TEMP_S',name='临时科目'),'code'),
            ('teachers',dict(teacher_no='TEMP_T',name='临时教师',user_id=0),'teacher_no'),
            ('students',dict(student_no='TEMP_ST',name='临时学生',age=19,class_id=classes[0]['id'],user_id=0),'student_no'),
            ('accounts',dict(username='temp_account',role='student',enabled=1,password=test_password),'username'),
        ]
        for entity,values,key in fixtures:
            row=create(entity,values,{key:values[key]})
            changed=dict(values)
            if 'name' in changed:changed['name']="O'Reilly \\ 中文 <img>"
            elif entity=='accounts':changed['username']='temp_account_changed'
            check(http('PUT','/api/'+entity+'/'+str(row['id']),changed,admin)[0]==200,'管理员有效更新 '+entity)
            if 'name' in changed:
                check(next(item for item in listing(entity) if item['id']==row['id'])['name']==changed['name'],'真实 MySQL 转义往返 '+entity)
            check(http('DELETE','/api/'+entity+'/'+str(row['id']),token=admin)[0]==200,'管理员有效删除 '+entity)
        extra_assignment=dict(teacher_id=teachers[0]['id'],class_id=classes[0]['id'],subject_id=subjects[1]['id'])
        assigned=create('assignments',extra_assignment,extra_assignment)
        check(http('PUT','/api/assignments/'+str(assigned['id']),extra_assignment,admin)[0]==200,'不变授课授权更新成功')
        extra_grade=dict(student_id=students[0]['id'],subject_id=subjects[1]['id'],teacher_id=teachers[0]['id'],term='2026 补考',score=88)
        extra=create('grades',extra_grade,dict(term='2026 补考'))
        check(http('PUT','/api/grades/'+str(extra['id']),dict(extra_grade,score=89),admin)[0]==200,'管理员有效更新成绩')
        check(http('DELETE','/api/grades/'+str(extra['id']),token=admin)[0]==200,'管理员有效删除成绩')
        check(http('DELETE','/api/assignments/'+str(assigned['id']),token=admin)[0]==200,'管理员有效删除授课授权')
        teacher=login('teacher_a'); student=login('student_a')
        check([row['id'] for row in listing('students',teacher)]==[students[0]['id']],'教师仅见授课班级学生')
        check([row['id'] for row in listing('grades',teacher)]==[grades[0]['id']],'教师仅见授权科目成绩')
        check([row['id'] for row in listing('students',student)]==[students[0]['id']],'学生仅见本人资料')
        check([row['id'] for row in listing('grades',student)]==[grades[0]['id']],'学生仅见本人成绩')
        for token,name in [(teacher,'教师'),(student,'学生')]:
            check(http('GET','/api/accounts',token=token)[0]==403,name+'不可读取账号')
            check(http('POST','/api/classes',dict(code='NO',name='不允许'),token)[0]==403,name+'不可写基础资料')
        check(http('POST','/api/grades',grade_bodies[1],teacher)[0]==403,'教师不可跨授课范围写成绩')
        check(http('PUT','/api/grades/'+str(grades[1]['id']),grade_bodies[1],teacher)[0]==404,'教师不可按编号修改他人成绩')
        check(http('DELETE','/api/grades/'+str(grades[1]['id']),token=teacher)[0]==404,'教师不可按编号删除他人成绩')
        check(http('POST','/api/grades',grade_bodies[0],student)[0]==403,'学生不可录入成绩')
        own=dict(grade_bodies[0],score=95.5)
        check(http('PUT','/api/grades/'+str(grades[0]['id']),own,teacher)[0]==200,'教师可更正授权成绩')
        check(listing('grades',student)[0]['score']==95.5,'学生看到教师更新结果')
        check(http('POST','/api/classes',dict(code='C1',name='重复'),admin)[0]==400,'重复编号拒绝')
        check(http('DELETE','/api/classes/'+str(classes[0]['id']),token=admin)[0]==400,'有关联学生的班级拒绝删除')
        check(http('POST','/api/grades',dict(grade_bodies[0],score=101),admin)[0]==400,'成绩越界拒绝')
        check(http('DELETE','/api/accounts/1',token=admin)[0]==400,'当前管理员不可删除自己')

        with sync_playwright() as playwright:
            browser=playwright.chromium.launch(channel='msedge',headless=True)
            for name,role in [('admin','admin'),('teacher_a','teacher'),('student_a','student')]:
                page=browser.new_page(viewport={'width':1440,'height':840})
                errors=[];page.on('pageerror',lambda error:errors.append(str(error)))
                page.goto(base)
                if role=='admin':page.screenshot(path=str(work/'login.png'),full_page=True)
                page.get_by_label('账号',exact=True).fill(name)
                page.get_by_label('口令',exact=True).fill(test_password)
                page.get_by_role('button',name='登录工作台',exact=True).click()
                page.get_by_text('数据已更新',exact=True).wait_for(timeout=60000)
                check(page.get_by_text('权限管理',exact=True).count()==(1 if role=='admin' else 0),'浏览器角色导航 '+role)
                page.screenshot(path=str(work/(role+'-overview.png')),full_page=True)
                page.locator('#navigation').get_by_text('学生管理',exact=True).click()
                check(page.locator('#rows tr').count()==(2 if role=='admin' else 1),'浏览器学生数据范围 '+role)
                check(page.locator('#add-button').is_visible()==(role=='admin'),'浏览器写入操作范围 '+role)
                page.screenshot(path=str(work/(role+'-students.png')),full_page=True)
                if role=='admin':
                    page.locator('#navigation').get_by_text('权限管理',exact=True).click()
                    page.get_by_role('tab',name='授课授权',exact=True).click()
                    page.screenshot(path=str(work/'admin-permissions.png'),full_page=True)
                if role=='teacher':
                    page.locator('#navigation').get_by_text('成绩管理',exact=True).click()
                    page.get_by_role('button',name='编辑',exact=True).click()
                    page.get_by_label('成绩',exact=True).fill('96.5')
                    page.get_by_role('button',name='保存',exact=True).click()
                    page.wait_for_function("!document.querySelector('#mask').classList.contains('show')")
                    page.get_by_text('96.5',exact=True).wait_for()
                    page.wait_for_function("!document.querySelector('#toast').classList.contains('show')")
                    page.screenshot(path=str(work/'teacher-grades.png'),full_page=True)
                page.set_viewport_size({'width':390,'height':844})
                check(page.evaluate('document.documentElement.scrollWidth <= innerWidth'),'窄屏无横向页面溢出 '+role)
                page.screenshot(path=str(work/(role+'-mobile.png')),full_page=True)
                check(not errors,'浏览器无脚本错误 '+role)
                page.get_by_role('button',name='退出',exact=True).click()
                page.get_by_role('button',name='登录工作台',exact=True).wait_for()
                page.close()
            browser.close()

        account=users['teacher_a']
        check(http('PUT','/api/accounts/'+str(account['id']),dict(username='teacher_a',role='teacher',enabled=1.5,password=''),admin)[0]==400,'账号启用状态拒绝小数')
        check(http('PUT','/api/accounts/'+str(account['id']),dict(username='teacher_a',role='teacher',enabled=1,password=123),admin)[0]==400,'账号口令拒绝非文本')
        check(http('PUT','/api/accounts/'+str(account['id']),dict(username='teacher_a',role='teacher',enabled=0,password=''),admin)[0]==200,'管理员停用教师账号')
        check(http('GET','/api/students',token=teacher)[0]==401,'停用账号的旧会话立即拒绝')
        check(http('POST','/api/auth/logout',token=student)[0]==200,'主动退出')
        check(http('GET','/api/grades',token=student)[0]==401,'退出令牌不可复用')
        check(http('POST','/api/auth/password',dict(oldPassword=test_password,password='new-school-test-password'),admin)[0]==200,'本人修改口令')
        check(http('GET','/api/auth/me',token=admin)[0]==401,'修改口令撤销既有会话')
        rejected=[http('POST','/api/auth/login',dict(username='student_b',password='incorrect-password'))[0] for _ in range(5)]
        check(rejected==[401]*5,'连续五次错误口令均拒绝')
        check(http('POST','/api/auth/login',dict(username='student_b',password=test_password))[0]==429,'锁定期间正确口令也不能登录')
        with connection.cursor() as cursor:
            cursor.execute('SELECT locked_until,CAST(UNIX_TIMESTAMP()*1000 AS SIGNED) FROM school_users WHERE username=%s',('student_b',))
            locked,now=cursor.fetchone()
            check(0<locked-now<=900000,'锁定期限持久化为数据库挂钟时间')
        report={'checks':checks,'count':len(checks),'application_sha256':hashlib.sha256(args.app.read_bytes()).hexdigest(),'database':'isolated-and-dropped','browser':'Edge headless','roles':['admin','teacher','student']}
        (work/'report.json').write_text(json.dumps(report,ensure_ascii=False,indent=2),encoding='utf-8')
        print('项目模块验收通过',len(checks),flush=True)
    finally:
        if process is not None:
            process.terminate()
            try:process.wait(timeout=10)
            except subprocess.TimeoutExpired:process.kill();process.wait(timeout=10)
        log.close()
        with connection.cursor() as cursor:cursor.execute('DROP DATABASE IF EXISTS `'+database+'`')
        connection.close()
        (work/'config.json').unlink(missing_ok=True)


if __name__=='__main__':main()
