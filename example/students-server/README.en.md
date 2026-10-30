# Student information management system

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](README.md)
<!-- END LANGUAGE NAVIGATION -->

A ChaLi school application with login, permissions, students, classes, grades, subjects and teachers. Administrators manage accounts and records, teachers maintain grades in their assigned class/subject scope, and students read their own profiles and grades. Every business API enforces permissions independently of visible browser actions.

See [Chapter 29](../../doc/book/en/29-student-management.md) for architecture, actual screenshots and the complete walkthrough.

## Configure and start

The verified platform is Windows x64, MySQL 8 and Edge. Prepare a MySQL client library. Authentication directly uses Windows CNG at C:/Windows/System32/bcrypt.dll; adjust for another Windows installation or implement the corresponding provider on another system.

Copy config.example.json to the untracked config.local.json and provide database credentials. Database names allow 1–64 ASCII letters, digits or underscores. The database account needs schema creation permissions; database creation is attempted only when necessary and permitted.

From the repository root:

```sh
chali check example/students-server/src/main.chl
chali run example/students-server/src/main.chl -- 8787 example/students-server/web example/students-server/config.local.json
```

All three application arguments are required: port, web directory and database configuration file. Visit http://127.0.0.1:8787. An empty database gets one admin account with a randomly generated password printed once. Existing accounts are not reset, and no business demonstration records are inserted.

Export with chali aot example/students-server/src/main.chl --out build/students-server. Pass the same arguments to the executable. Deliver web assets, configuration and database client dependencies separately. AOT defaults to unchecked; use --checked for development checks. The example listens on local HTTP; cross-machine deployments require an HTTPS entry. It is a single-process serial example without password recovery, distributed sessions or background tasks.

## Data and permissions

Create classes and subjects first, then role-specific accounts, teachers and students with account links, and teaching assignments. Teachers can then enter grades and students can inspect their own records. Identifiers are individually unique. Grades are unique by student, subject and term, with scores from 0 to 100. Foreign keys protect referenced records. Current teaching assignments determine access while historical grades remain. The old students table is left intact without automatic migration to school_ tables.

Passwords use independent random salts and PBKDF2-HMAC-SHA256 with 600000 iterations. System random bytes produce 256-bit Bearer tokens. Tokens remain in current-page and server memory, with 30-minute expiry. Page refresh or service restart requires login. Logout revokes a token, and account disabling, role changes or password updates invalidate existing sessions. Five failed logins lock the account for 15 minutes.

Authorization uses a Bearer header rather than an automatically attached session cookie. Requests allow 64 KiB of headers and 1 MiB of body, rejecting duplicate Content-Length or Transfer-Encoding. Lists allow 10000 rows; browser filtering, sorting and pagination use those loaded records, with 10 rows per page.

Fixed SQL identifiers and Database.quote handle MySQL values; textContent displays user data. API output excludes password hashes and salts.

## Modules

main.chl initializes the service; httpd.chl handles HTTP; school_api.chl dispatches authenticated routes; school_auth.chl owns login and sessions; school_accounts.chl handles accounts and passwords; school_data.chl owns business tables and scopes; crypto.chl uses system C ABI; store.chl connects MySQL; api.chl provides responses and ID parsing. web/index.html contains the frontend.

## Acceptance

Use [students_server.py](../../tests/students_server.py), [students_server_protocol.py](../../tests/students_server_protocol.py) and [students_server_ui.py](../../tests/students_server_ui.py). Full acceptance requires pymysql, Playwright, Edge and a MySQL account allowed to create an isolated database. Provide its password through CHALI_TEST_MYSQL_PASSWORD:

```sh
python tests/students_server.py --app build/students-server.exe
python tests/students_server_protocol.py
python tests/students_server_ui.py
```

The test creates and drops only its unique chali_school_test_ database and closes the service afterward. Screenshots come from the real service and isolated acceptance data. Separate frontend tests cover pagination, filtering, safe text output, forms, failure recovery and narrow screens.
