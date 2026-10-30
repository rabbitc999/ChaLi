# 学生信息管理系统

<!-- LANGUAGE NAVIGATION -->
[English](README.en.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

ChaLi 教务应用示例，包含登录、权限、学生、班级、成绩、科目和教师七个板块。浏览器页面与服务端共用三角色权限模型，后端独立检查每个请求。

| 角色 | 允许操作 |
| --- | --- |
| 管理员 | 管理基础资料、账号、授课授权和成绩 |
| 教师 | 读取授课班级资料，维护授权科目的成绩 |
| 学生 | 只读自己的资料、成绩及相关班级、科目、教师 |

完整教程、运行截图和架构图见[第 25 章](../../doc/book/25-学生信息管理系统.md)。

## 配置与启动

实际验收平台为 Windows x64、MySQL 8 和 Edge。程序需要可用的 MySQL 客户端动态库。认证模块直接使用 Windows CNG，当前声明的系统库路径为 C:/Windows/System32/bcrypt.dll；不同 Windows 安装位置需调整，其他系统需要替换相应认证实现。

复制 config.example.json 为 config.local.json，填写自己的数据库凭据。数据库名只允许 1–64 个 ASCII 字母、数字或下划线。配置文件不入库。账号须有建表权限；仅在数据库不存在且账号允许时自动创建数据库。

在仓库根执行：

```sh
chali check example/students-server/src/main.chl
chali run example/students-server/src/main.chl -- 8787 example/students-server/web example/students-server/config.local.json
```

三个程序参数分别是端口、页面目录、数据库配置文件，均需提供。访问 http://127.0.0.1:8787。首次空库启动生成 admin 的随机初始口令并在控制台显示一次；已有账号不重置。没有内置固定口令或业务演示记录。

原生导出：

```sh
chali aot example/students-server/src/main.chl --out build/students-server
```

运行导出的程序时传入相同三个参数。网页、配置及数据库客户端依赖仍需准备；aot 默认 unchecked，开发核查可以显式 --checked。连接默认只监听 127.0.0.1，不提供 TLS；跨机器部署需由 HTTPS 入口保护传输。本例为单进程串行服务，没有密码找回、分布式会话或后台任务。

## 建立业务数据

按以下顺序操作：

1. 登录管理员，建立班级和科目。
2. 建立教师、学生账号，并选择正确角色。
3. 建立教师与学生资料，关联账号及学生班级。
4. 在权限管理的授课授权页指定教师、班级、科目。
5. 教师登录录入成绩，学生登录查询本人记录。

编号分别唯一；成绩按学生、科目、学期唯一，分数为 0–100。已有引用的记录不能直接删除。班级或授课关系变更后重新计算访问范围，保留历史成绩。旧版 students 表保持原样，不自动迁移到新的 school_ 表。

## 认证与数据边界

密码使用每账号随机盐与 PBKDF2-HMAC-SHA256，600000 次迭代。系统随机数生成 256 位令牌。令牌只保留在当前页面与服务端内存，刷新页面或重启服务后重新登录。会话有效期 30 分钟，主动退出撤销令牌；停用账号、更改角色或口令会使已有会话失效。五次失败登录暂时锁定账号 15 分钟。

API 使用 Authorization: Bearer 令牌，不使用浏览器自动附带的会话 Cookie。请求头上限 64 KiB，请求体上限 1 MiB，不接受重复 Content-Length 或 Transfer-Encoding。列表上限 10000 条，筛选、排序及每页 10 条分页在浏览器进行；超出上限返回错误。

SQL 表名与列名来自固定白名单，MySQL 文本值由 Database.quote 转义。页面用 textContent 显示用户数据。密码摘要和盐不对外返回。

## 代码位置

| 文件 | 职责 |
| --- | --- |
| src/main.chl | 配置、连接、初始化与接受连接 |
| src/httpd.chl | HTTP 请求和响应 |
| src/school_api.chl | 路由与认证入口 |
| src/school_auth.chl | 登录、会话、账号状态 |
| src/school_accounts.chl | 账号管理与口令更新 |
| src/school_data.chl | 教务表、校验与角色数据范围 |
| src/crypto.chl | 系统随机数与 PBKDF2 C ABI |
| src/store.chl | MySQL 配置与连接 |
| src/api.chl | 响应与编号解析 |
| web/index.html | 登录、导航、表格与表单 |

## 验证

项目验证入口为 [tests/students_server.py](../../tests/students_server.py)、[协议与密码派生](../../tests/students_server_protocol.py)和 [UI 隔离验收](../../tests/students_server_ui.py)。完整项目验收需要 pymysql、Playwright、Edge 和可创建独立测试库的 MySQL 账号。

通过 CHALI_TEST_MYSQL_PASSWORD 环境变量提供测试凭据，执行：

```sh
python tests/students_server.py --app build/students-server.exe
python tests/students_server_protocol.py
python tests/students_server_ui.py
```

项目验收只创建唯一命名的 chali_school_test_ 库，结束后关闭服务并删除该库，不清空业务库。运行截图来自真实服务的隔离测试数据；UI 隔离测试另外验证分页、筛选、文本注入防护、表单、错误恢复与窄屏布局。
