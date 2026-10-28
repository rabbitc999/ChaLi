# 学生信息管理系统（前后端 · MySQL · 反射 · 多模块）

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](README.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

带 Web 界面的学生信息管理：ChaLi 多模块工程做后端（HTTP 服务 + MySQL 存储 + 反射驱动），
单页 HTML 做前端，JSON 通信。

## 运行

前置条件：本机有 MySQL 8.0（或 5.7+），账号默认取 `root/123456@127.0.0.1:3306`
（与 `std::sql::Database::local` 同一套默认值；改连接参数见 `src/main.chl` 的
`defaultServerConfig`）。库和表不存在时启动过程自动创建。

```sh
cd example/students-server
chali src/main.chl            # 默认 127.0.0.1:8787
chali src/main.chl 9000       # 指定端口
```

浏览器打开 <http://127.0.0.1:8787> 即可。

## AOT 导出

在工程目录执行 `chali aot src/main.chl --out build/students-server`。Windows 下运行：

```powershell
.\build\students-server.exe 9000 web
```

原生产物仍需要 MySQL、相应客户端库与 `web/index.html`，不会自动内嵌应用页面或数据库。Windows x64 已核对原始工程构建、服务启动及 18 项 HTTP 业务流程；验收使用独立数据库，未修改已有业务库。详细步骤见[书稿第 25 章](../../doc/book/25-学生信息管理系统.md)。

## 功能

- 学生增删查改：学号（可自动分配）、姓名、年龄、班级、成绩；
- 业务校验：学号正整数、姓名 / 班级非空限长、年龄 1–150、成绩 0–100；
- 按姓名模糊搜索、按任意模型字段排序；
- 模型字段清单接口 `/api/schema`（反射读出，前端统计卡直接用）。

## REST 接口

| 方法 | 路径 | 说明 |
| --- | --- | --- |
| GET | `/api/students?search=&order=&desc=1` | 列表，可选姓名模糊搜索与排序 |
| GET | `/api/students/<学号>` | 查一个学生 |
| POST | `/api/students` | 新增（JSON，学号可省） |
| PUT | `/api/students/<学号>` | 更新 |
| DELETE | `/api/students/<学号>` | 删除 |
| GET | `/api/schema` | 模型字段清单（反射） |

错误统一为 `{"error":"…"}`，HTTP 状态码 400 / 404 / 405。

## 模块划分

```text
src/
├── main.chl     入口：参数解析、连库建表、读首页、accept 循环
├── api.chl      REST 路由：HTTP 语义 ↔ 存取调用
├── store.chl    MySQL 存取：建表、增删查改、结果集 → JSON
├── httpd.chl    极简 HTTP：读请求（非阻塞轮询）、发响应
└── model.chl    学生模型：@Reflect + SQL 注解、校验、字段清单
```

模型声明只在 `model.chl` 出现一次：建表列（`mysqlColumns`）、结果集解码
（`encodeRows`）、排序字段白名单（`isField`）、JSON 键全部由
`std::reflect::getStructInfo<Student>()` 派生。加一个字段只需改模型一处，
再按 `std::sql` 文档同步数据库列类型即可。

## 与书稿的关系

入门讲解见书稿[第 24 章 学生信息管理系统](../../doc/book/25-学生信息管理系统.md)。
命令行 + SQLite 的最小版本见 [`example/reflection-students`](../reflection-students)；
SQL 模型与反射序列化的完整说明见
[动态 SQL 说明](../../doc/33-reflection-serialization-sql.md)。

## 边界说明

- `std::sql` 的参数化绑定目前由 SQLite 后端实现；MySQL 后端按文档口径使用
  `Database::quote` 转义文本值。列名来自编译期模型字段名，不拼任何用户输入。
- HTTP 服务是单线程顺序处理，够课堂演示与课内实验；并发能力不是本示例的目标。
- 前端只依赖原生浏览器能力（fetch / DOM），无任何外部资源。
