# Chapter 25 A student information system

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../25-学生信息管理系统.md)
<!-- END LANGUAGE NAVIGATION -->

This project combines a reflected student model, JSON, SQL, TCP and result-based error handling. The browser sends HTTP requests to a ChaLi service, which validates input and stores records in MySQL. A single model declaration drives field metadata used by serialization and database helpers.

The source is in [example/students-server](../../../example/students-server/README.md). Its five modules separate the model, persistence, HTTP parsing, routing and entry point. The web page uses browser-native fetch and DOM APIs.

```sh
cd example/students-server
chali src/main.chl
chali src/main.chl 9000
```

The default development configuration connects to MySQL at `127.0.0.1:3306`, with the configured database and credentials in the store configuration. Use a dedicated database when experimenting. The page is loaded from `web/index.html`; starting from a different directory requires the correct web-root argument.

| Method | Route | Operation |
| --- | --- | --- |
| GET | `/api/students` | List, search and sort |
| GET | `/api/students/<id>` | Read one student |
| POST | `/api/students` | Create a student |
| PUT | `/api/students/<id>` | Update a student |
| DELETE | `/api/students/<id>` | Delete a student |
| GET | `/api/schema` | Read reflected model fields |

Errors use an `error` JSON field and an appropriate HTTP status. Database text values are quoted through the database interface; user input must not become a raw SQL identifier. Business validation checks names, class names, ages and scores before persistence.

## Exporting the native service

From the project directory, export through LLVM AOT:

```sh
chali aot src/main.chl --out build/students-server
```

On Windows, run the native service with its port and web-root arguments:

```powershell
.\build\students-server.exe 9000 web
```

A supported Linux x64 build normally has no `.exe` suffix and runs as `./build/students-server 9000 web`. MySQL, its required client library and `web/index.html` remain application dependencies. AOT does not embed the database or application page automatically.

Windows x64 acceptance built the original project and verified startup plus 18 HTTP business checks, including Chinese and non-BMP text, CRUD, search/sort, invalid input, error statuses and injection text. The acceptance copy changed only the database name, used an isolated database and removed it afterwards. Do not run destructive checks against an existing business database.

The smaller [SQLite command-line project](../../../example/reflection-students/README.en.md) demonstrates the same model without an HTTP server.

Exercise: add a model field, follow its path through schema and serialization, and identify the remaining explicit validation changes. Use [appendix A](A-common-errors.md) when diagnosing failures.
