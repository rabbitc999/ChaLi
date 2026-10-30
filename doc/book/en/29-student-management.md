# Chapter 29 Student information management system

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../29-学生信息管理系统.md)
<!-- END LANGUAGE NAVIGATION -->

Bring types, borrows, errors and modules into one request: authenticate a user, determine access scope, read or change school data and display the outcome. Correct behavior also requires valid relations, explainable failures, successful startup and enforced authorization.

This chapter follows the repository's real student system: login, permissions, students, classes, grades, subjects and teachers. Administrator, teacher and student roles connect configuration, authentication, data relations, UI and delivery. Screenshots and historical acceptance describe their actual environment rather than universal platform validation.

## 29.1 Start with actions and data boundaries

Start with one observable action: a teacher signs in and corrects a grade for a class they teach. The application must identify the account, class/subject assignment, student relation and valid score. The page initiates the action while the backend checks those relationships.

Administrators manage accounts, reference data and assignments; teachers work within assigned scope; students read their own records. Establish these rules before arranging pages and routes instead of retrofitting authorization after building buttons.

| Area | Main actions | Boundary |
| --- | --- | --- |
| Login | Sign in, sign out, change password | Business APIs require authentication |
| Permissions | Roles, enabled status, teaching assignments | Administrator writes only |
| Students | ID, name, age, class and account link | Administrator writes; teachers read assigned classes; students read self |
| Classes | Code and name | Administrator writes; others read related classes |
| Grades | Student, subject, teacher, term and score | Teachers write assigned scope; students read self |
| Subjects | Code and name | Administrator writes; others read related subjects |
| Teachers | Staff number, name and account link | Administrator writes; others read related teachers |

The browser displays appropriate actions, but the backend enforces the boundary even for manually constructed HTTP requests.

## 29.2 Trace a request through the modules

![Architecture: browser, authentication, data scopes and MySQL](../images/students-architecture-en.png)

The project lives in [example/students-server](../../../example/students-server/). main.chl connects the database and accepts TCP connections. httpd.chl reads bounded requests and returns HTML or JSON. school_api.chl dispatches routes; school_auth.chl verifies sessions; school_data.chl handles school records and teaching scope; school_accounts.chl handles accounts and passwords.

store.chl owns database connection configuration and api.chl provides responses and ID parsing. crypto.chl calls system C ABI for random bytes and password derivation. The frontend is web/index.html, with its styles and script served in the same page without an external CDN.

## 29.3 Configure and start the application

The verified environment is Windows x64, the external LLVM compiler, MySQL 8 and Edge. The authentication example binds C:/Windows/System32/bcrypt.dll directly. Adjust declarations for a different Windows installation; another operating system requires a corresponding authentication implementation. Changing a target triple alone does not establish application portability.

Copy config.example.json to an untracked local configuration and provide a database account permitted to use the test database. Keep real database passwords out of source control.

```json
{
  "host": "127.0.0.1",
  "user": "school_app",
  "password": "replace-with-local-secret",
  "database": "chali_school",
  "port": 3306
}
```

Host and port identify the database server, serving a different purpose from the web listener's port 8787. User and password authenticate the application's MySQL connection; database selects its schema. These are not credentials for browser login to the student system.

The password is placeholder text for a local test configuration, not a secret to retain in the book or Git. JSON quotes delimit keys and string values, while port is numeric. Successful parsing only validates readable configuration; schema existence and account privileges still require a real connection.


Run from the repository root. The three application arguments are the port, web directory and database configuration:

```sh
chali run example/students-server/src/main.chl -- 8787 example/students-server/web example/students-server/config.local.json
```

Chali run compiles and executes the project entry. The double dash separates compiler options from application arguments. The following three values specify the web port, static-page directory and MySQL configuration file in that order. Reordering them changes their interpretation.

These paths assume the documented repository-root working directory. Copying the command after changing into src would alter resolution. Observe database connection and initialization before opening the browser. Compilation success alone does not establish listening, readable assets or working authentication. The database's 3306 port is not the web entry.


Open http://127.0.0.1:8787. An empty database gets school_-prefixed tables and a randomly generated admin password printed once to the console. Existing accounts are not reset. No demonstration records are inserted, and an existing students table is neither migrated nor deleted.

## 29.4 Login, sessions and password updates

![Actual application: login](../images/students-login.png)

Login verifies PBKDF2-HMAC-SHA256 with a random per-account salt and 600000 iterations. Session tokens come from system random bytes and remain in server process memory and current page memory, not localStorage. Refreshing the page or restarting the service requires login again.

Requests send a Bearer token in Authorization. Sessions last 30 minutes; every request rechecks account status and version. Disabling an account or changing its role or password invalidates old sessions. Five failed passwords lock an account for 15 minutes. Persistent lockout uses the database wall clock; process-local session expiry uses a monotonic clock.

The example listens on local HTTP. Cross-machine deployment needs HTTPS and operational work appropriate to concurrency, account recovery and auditing. Password recovery, distributed sessions and background tasks are not implemented. See [Windows CNG](https://learn.microsoft.com/en-us/windows/win32/api/bcrypt/) and [OWASP password storage](https://cheatsheetseries.owasp.org/cheatsheets/Password_Storage_Cheat_Sheet.html).

## 29.5 Create reference data in dependency order

Create classes and subjects first, then accounts, teachers and students, and finally teaching assignments. An account role is distinct from a person’s name: a teacher user_id must link to a teacher account and a student link to a student account. Administrators may maintain person records without a login account.

![Actual application: administrator overview with two classes, two students and two grade records](../images/students-admin-overview.png)

Class codes, subject codes, student numbers, staff numbers and usernames are individually unique. Students reference classes; assignments reference teachers, classes and subjects; grades reference students, subjects and teachers. Foreign keys protect referenced records. Duplicate identifiers, invalid links and out-of-range grades return failures shown by the page.

## 29.6 Teaching assignments and grade writes

![Actual application: administrator assigns teacher, class and subject](../images/students-admin-permissions.png)

A teaching assignment combines teacher_id, class_id and subject_id. Grade writes verify an assignment for the student’s current class, selected subject and teacher, and verify that teacher belongs to the logged-in account. Knowing another grade ID does not grant access.

![Actual application: teacher updates an authorized mathematics grade](../images/students-teacher-grades.png)

Grades are unique by student, subject and term, with scores from 0 to 100. PUT treats unchanged data as a valid update of an existing record rather than inferring absence from MySQL affected=0. Authorization follows current assignments while existing grades remain. A student can still resolve teachers and subjects referenced by their historical grades.

## 29.7 Student read-only and narrow-screen views

![Actual application: a student sees only their own profile](../images/students-student-students.png)

Student views have no create, edit or delete actions, and the backend rejects those writes too. Overview counts describe data visible to the current account: a student count of one means one accessible profile, not the school population.

![Actual application: the student view at 390 pixels, with navigation and table scrolling inside their own regions](../images/students-student-mobile.png)

Views share navigation, headers, statistics, filtering and pagination. Narrow screens use horizontal navigation and internal table scrolling without page overflow. Dialogs support keyboard focus, Escape and cancellation, with duplicate submission prevented while saving.

## 29.8 Input, SQL and browser output

The backend accepts fixed table and column names; paths cannot become arbitrary SQL identifiers. JSON fields receive separate text, integer or score validation, followed by length, range and relationship checks. The current MySQL interface uses Database.quote for text literals; SQLite parameter binding does not imply the same MySQL API exists.

The browser assigns user data through textContent rather than interpreting names as innerHTML. Filtering and pagination operate on loaded records. List queries allow at most 10000 records and return an error beyond that; cursor pagination for larger datasets is not implemented.

| Route | Method | Meaning |
| --- | --- | --- |
| /api/auth/login | POST | Sign in |
| /api/auth/me | GET | Current account |
| /api/auth/logout | POST | Revoke current token |
| /api/auth/password | POST | Verify old password and replace it |
| /api/accounts | GET/POST | Administrator lists or creates accounts |
| /api/accounts/id | PUT/DELETE | Update or delete an account |
| /api/students and other entity routes | GET/POST | Scoped list or create |
| /api/students/id and other entity routes | PUT/DELETE | Check data scope before update or delete |

Other entity routes are classes, grades, subjects, teachers and assignments. Error responses consistently contain an error field.

## 29.9 Export and acceptance

A native export still needs the web directory, database configuration and MySQL client dependencies. Process-local sessions are not part of a database backup; verify account links and foreign keys when backing up business data. Chapter 24 covers deployment and cross-compilation boundaries.

Acceptance uses an isolated temporary database for valid CRUD, duplicate and foreign-key rejection, role isolation, unauthorized teacher writes, session revocation and browser interactions. It closes the service and drops only that test database afterward. The entry points are [students_server.py](../../../tests/students_server.py) and [students_server_ui.py](../../../tests/students_server_ui.py).

Screenshots come from the real Windows x64 service, an isolated MySQL database and Edge. Names, classes and scores are acceptance data, and the shown roles and counts reflect actual access results.

### Summary

Follow a teacher's grade correction end to end: the browser submits a token and data, the server validates session/account state, checks fields and teaching scope, writes the database and returns an outcome for the page. Testing only buttons or only writes does not cover that chain.

Structures organize values, enums and Result express states, modules separate responsibilities, C ABI connects system capabilities and AOT defines delivery. Preserve these boundaries and add positive and negative acceptance cases when extending the application rather than generalizing local results to arbitrary deployment.

## 29.10 Follow a Request Through the Router

```chali
api::Reply handle(std::sql::Database mut& database, school_auth::State mut& state, httpd::Request const& request)
{
    if (request.path == "/api/auth/login")
    {
        if (request.method != "POST") { return api::errorReply(405, "登录需要 POST"); }
        return school_auth::login(&database, &state, request.body);
    }
    let identity = school_auth::authenticate(&database, &state, &request);
    if (identity.isNone()) { return api::errorReply(401, "请登录，会话已失效或账号已更新"); }
    let user = identity.unwrap();
    if (request.path == "/api/auth/me" && request.method == "GET") { return api::jsonReply(200, school_auth::userJson(user)); }
    if (request.path == "/api/auth/logout" && request.method == "POST")
    {
        school_auth::logout(&state, &request);
        return api::jsonReply(200, "{\"ok\":true}");
    }
    if (request.path == "/api/auth/password" && request.method == "POST") { return school_accounts::changePassword(&database, user, request.body); }
    if (!std::text::Text::startsWith(request.path, "/api/")) { return api::errorReply(404, "接口不存在"); }
    String entity = request.path[5:];
    int64 id = 0;
    int64 separator = std::text::Text::find(entity, "/");
    if (separator >= 0)
    {
        id = api::parseId(entity[(separator + 1):]);
        entity = entity[0:separator];
        if (id <= 0) { return api::errorReply(400, "记录编号无效"); }
    }
    if (entity == "accounts")
    {
        if (request.method == "GET" && id == 0) { return school_accounts::list(&database, user); }
        return school_accounts::write(&database, user, request.method, id, request.body);
    }
    if (request.method == "GET" && id == 0) { return school_data::list(&database, entity, user); }
    return school_data::write(&database, entity, user, request.method, id, request.body);
}
```

Read the function in four sections. Login is the only unauthenticated route and requires POST. Every other request passes authenticate; None returns 401 before routing, and user comes from server-side validation.

Next, handle identity, logout and password routes, then split the remaining API target into entity and optional id. Reject invalid identifiers. Finally dispatch account operations separately from school data. List and write functions receive the authenticated user and enforce role and teaching scope again. Authentication alone does not authorize arbitrary grade updates. Trace one teacher request through identity, id parsing and the school_data authorization check.

## 29.11 From a Connection to Business Logic

```chali
void serve(std::sql::Database mut& database, school_auth::State mut& state, std::net::TcpSocket mut& peer, String page)
{
    let read = httpd::readRequest(peer, 10000);
    if (read.isErr())
    {
        /* 空连接 / 超时是常态（浏览器预连接），安静地关掉即可。 */
        return;
    }
    httpd::Request request = read.unwrap();
    if ((request.path == "/" || request.path == "/index.html") && request.method == "GET")
    {
        let sent = httpd::sendResponse(peer, 200, "OK", "text/html; charset=utf-8", page);
        if (sent.isErr()) { eprintln("发送首页失败：{}", sent.error().toString()); }
        return;
    }
    api::Reply reply = school_api::handle(&database, &state, &request);
    String reason = "OK";
    if (reply.status == 400) { reason = "Bad Request"; }
    elif (reply.status == 401) { reason = "Unauthorized"; }
    elif (reply.status == 403) { reason = "Forbidden"; }
    elif (reply.status == 429) { reason = "Too Many Requests"; }
    elif (reply.status == 503) { reason = "Service Unavailable"; }
    elif (reply.status == 404) { reason = "Not Found"; }
    elif (reply.status == 405) { reason = "Method Not Allowed"; }
    elif (reply.status == 500) { reason = "Internal Server Error"; }
    let sent = httpd::sendResponse(peer, reply.status, reason, reply.contentType, reply.body);
    if (sent.isErr()) { eprintln("发送响应失败：{}", sent.error().toString()); }
    eprintln("{} {} -> {}", request.method, request.path, reply.status);
}
```

The mutable references refer to the service's database, session state and current socket; page is preloaded HTML. The first section reads a request and handles the homepage without entering API routing. A failed read returns for outer resource cleanup, not as business success.

The second section invokes school_api::handle, maps Reply status to reason text and sends the common response. Logging records method, path and status, without passwords or full tokens. Trace GET /api/grades through these handoffs.

## 29.12 Login: Validate Input and Create a Session

```chali
api::Reply login(std::sql::Database mut& database, State mut& state, String body)
{
    prune(&state);
    let parsed = std::json::parse(body);
    if (parsed.isErr()) { return api::errorReply(400, "登录数据必须是 JSON 对象"); }
    let value = parsed.unwrap();
    String username = stringField(&value, "username");
    String password = stringField(&value, "password");
    if (username.isEmpty() || len(username) > 64 || len(password) < 1 || len(password) > 1024)
    {
        return api::errorReply(400, "请填写有效账号与口令");
    }
    let rows = database.exec("SELECT id,username,role,version,enabled,salt,digest,failed_count,locked_until,CAST(UNIX_TIMESTAMP()*1000 AS SIGNED) AS now_ms FROM school_users WHERE username=" + database.quote(username));
    if (!rows.ok) { return api::errorReply(503, "账号服务暂不可用"); }
    if (rows.rowCount() != 1)
    {
        let ignored = crypto::derive(password, "school-unknown-account", (uint64)600000);
        return api::errorReply(401, "账号、口令无效或账号暂不可用");
    }
    int64 now = rows.intAt(0, 9);
    if (rows.intAt(0, 8) > now) { return api::errorReply(429, "登录尝试过多，请稍后再试"); }
    let digest = crypto::derive(password, rows.textAt(0, 5), (uint64)600000);
    if (digest.isErr()) { return api::errorReply(503, "认证服务暂不可用"); }
    int64 id = rows.intAt(0, 0);
    if (rows.intAt(0, 4) != 1 || !crypto::equalDigest(digest.unwrap(), rows.textAt(0, 6)))
    {
        int64 attempts = rows.intAt(0, 7) + 1;
        int64 until = 0;
        if (attempts >= 5) { until = now + 900000; attempts = 0; }
        let failed = database.exec("UPDATE school_users SET failed_count=" + attempts.toString() + ",locked_until=" + until.toString() + " WHERE id=" + id.toString());
        return api::errorReply(401, "账号、口令无效或账号暂不可用");
    }
    if (len(state.sessions) >= 4096) { return api::errorReply(503, "会话数量已达上限"); }
    let random = crypto::token();
    if (random.isErr()) { return api::errorReply(503, "无法创建会话"); }
    Session session;
    session.token = random.unwrap(); session.userId = id; session.version = rows.intAt(0, 3); session.expires = std::clock::nowMs() + 1800000;
    state.sessions.push(session);
    let reset = database.exec("UPDATE school_users SET failed_count=0,locked_until=0 WHERE id=" + id.toString());
    User user;
    user.id = id;
    user.username = rows.textAt(0, 1);
    user.role = rows.textAt(0, 2);
    user.version = session.version;
    return api::jsonReply(200,
        "{\"token\":" + std::reflect::__serdeQuote(session.token)
        + ",\"user\":" + userJson(user) + "}");
}
```

Read login in three sections. First prune expired sessions, parse JSON, read actual string fields and validate lengths. Next query by a quoted username, check lockout, derive the candidate digest with the stored salt and compare it. An unknown account still performs a derivation before returning the common invalid-credential response.

Only success creates a random token, retaining user id, account version and a monotonic expiry. Sessions contain no password. The response exposes only token and public user data; it does not grant the browser control over roles.

Locate input errors (400), invalid credentials (401) and unavailable services (503). These imply different recovery actions. Database write results also matter; stricter deployment needs should tighten transaction boundaries around session establishment and failure-counter updates.

## 29.13 Return with a Token and Revalidate the Account

```chali
Option<User> authenticate(std::sql::Database mut& database, State mut& state, httpd::Request const& request)
{
    prune(&state);
    String token = bearer(&request);
    if (token.isEmpty()) { return Option<User>::None; }
    for (Session session in state.sessions)
    {
        if (!crypto::equalDigest(session.token, token)) { continue; }
        let rows = database.exec("SELECT id,username,role,version FROM school_users WHERE enabled=1 AND id=" + session.userId.toString());
        if (!rows.ok || rows.rowCount() != 1 || rows.intAt(0, 3) != session.version) { return Option<User>::None; }
        User user;
        user.id = rows.intAt(0, 0);
        user.username = rows.textAt(0, 1);
        user.role = rows.textAt(0, 2);
        user.version = rows.intAt(0, 3);
        return Option<User>::Value(user);
    }
    return Option<User>::None;
}
```

First prune sessions and extract one Bearer token; malformed or empty input returns None. Next compare against server-side tokens, then query an enabled account and require the stored version to match. Disabling or updating an account invalidates an old session.

Finally construct User from id, username, role and version. Value(user) proves authentication, not authorization to modify a particular grade. This teaching interface also maps database lookup failures to None rather than exposing a distinct infrastructure error.

## 29.14 Why a Teacher Cannot Modify Any Grade

```chali
bool relations(std::sql::Database mut& database, String entity, std::json::JsonValue const& body, school_auth::User user)
{
    if (entity == "students" || entity == "teachers")
    {
        int64 account = integer(&body, "user_id");
        if (account > 0)
        {
            String role = "student";
            if (entity == "teachers") { role = "teacher"; }
            let rows = database.exec("SELECT id FROM school_users WHERE id=" + account.toString() + " AND role=" + database.quote(role));
            if (!rows.ok || rows.rowCount() != 1) { return false; }
        }
    }
    if (entity == "grades")
    {
        String sql = "SELECT a.id FROM school_assignments a JOIN school_students s ON s.class_id=a.class_id JOIN school_teachers t ON t.id=a.teacher_id WHERE s.id=" + integer(&body,"student_id").toString()
            + " AND a.subject_id=" + integer(&body,"subject_id").toString() + " AND a.teacher_id=" + integer(&body,"teacher_id").toString();
        if (user.role == "teacher") { sql += " AND t.user_id=" + user.id.toString(); }
        let rows = database.exec(sql);
        if (!rows.ok || rows.rowCount() != 1) { return false; }
    }
    return true;
}
```

The first section verifies that linked student and teacher records use accounts with the corresponding role. Matching names do not establish account ownership.

For grades, join the student's class, teaching assignment and teacher, requiring all submitted identifiers to agree. A teacher additionally needs t.user_id=user.id, preventing substitution of another teacher's id.

Only one successful result authorizes the target association. Updating an existing row also requires access to the original record; write checks that separately.

## 29.15 Validate Before Writing

```chali
api::Reply write(std::sql::Database mut& database, String entity, school_auth::User user, String method, int64 id, String text)
{
    String source = table(entity);
    if (source.isEmpty()) { return api::errorReply(404, "板块不存在"); }
    if (user.role != "admin" && !(user.role == "teacher" && entity == "grades")) { return api::errorReply(403, "没有此操作权限"); }
    if (method != "POST" && method != "PUT" && method != "DELETE") { return api::errorReply(405, "请求方法不支持"); }
    if ((method == "POST" && id != 0) || (method != "POST" && id <= 0)) { return api::errorReply(400, "记录编号无效"); }
    String condition = "id=" + id.toString() + " AND (" + rowFilter(entity,user) + ")";
    if (id > 0)
    {
        let existing = database.exec("SELECT id FROM " + source + " WHERE " + condition);
        if (!existing.ok) { return api::errorReply(503, "读取记录失败"); }
        if (existing.rowCount() != 1) { return api::errorReply(404, "记录不存在或无权访问"); }
    }
    String sql = "DELETE FROM " + source + " WHERE " + condition;
    if (method != "DELETE")
    {
        let parsed = std::json::parse(text);
        if (parsed.isErr() || !parsed.unwrap().isObject()) { return api::errorReply(400, "记录必须是 JSON 对象"); }
        let body = parsed.unwrap();
        List<String> names = fields(entity);
        String columns = "";
        String values = "";
        String updates = "";
        for (String name in names)
        {
            let item = literal(&database, &body, name);
            if (item.isErr()) { return api::errorReply(400, item.error().toString()); }
            if (!columns.isEmpty()) { columns += ","; values += ","; updates += ","; }
            columns += name; values += item.unwrap(); updates += name + "=" + item.unwrap();
        }
        if (!relations(&database, entity, &body, user)) { return api::errorReply(403, "账号角色不匹配，或教师未获该班级与科目授权"); }
        sql = "INSERT INTO " + source + "(" + columns + ") VALUES (" + values + ")";
        if (method == "PUT") { sql = "UPDATE " + source + " SET " + updates + " WHERE " + condition; }
    }
    let result = database.exec(sql);
    if (!result.ok) { return api::errorReply(400, "保存失败：编号重复、关联不存在，或记录仍被其他数据使用"); }
    return api::jsonReply(200, "{\"ok\":true}");
}
```

The first section validates entity, role, method and identifier shape. Students cannot write; teachers write only grades. Existing rows must match both id and rowFilter, so knowing an identifier is insufficient.

The second parses JSON and iterates only server-approved column names. literal validates each value, quotes text and restricts scores to zero through one hundred. It builds columns, values and assignments in matching order.

The third checks relations and selects INSERT, UPDATE or DELETE. Only successful execution returns {"ok":true}. Uniqueness, foreign keys and referenced-record constraints remain database checks.

Read backward from exec and list every preceding identity, scope, field and association check. A teacher submitting a grade for an unauthorized class should fail here even when manually sending the HTTP request.

## 29.16 Exercises

1. Create a teacher, class and subject and assign teaching scope.
2. Try changing another class’s grade using a teacher token.
3. Reuse a student token after disabling the account.
4. Explain browser-memory loss, service restart and database backup.

### Answers and checks

1. Create records in dependency order and inspect the teacher’s visible scope.
2. The backend rejects it and preserves the target; hidden buttons alone are insufficient.
3. The old token is rejected because account status and version are rechecked.
4. Refresh loses the token, restart loses sessions, and database backups retain accounts and business records.
