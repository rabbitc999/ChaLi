# HTTP file server

The Chapter 28 project develops a request, parser, route selection, response, and connection state machine in small steps.

From this directory:

```sh
chali run src/main.chl -- 8080 public
```

Open `http://127.0.0.1:8080/`, then `/notes.txt`. Port zero asks the system for a port, reported on stderr. An optional third argument limits total accepted connections, allowing deterministic shutdown in tests.

Startup loads three explicitly named text assets, each limited to one MiB. Request targets select preloaded routes and are never concatenated with disk paths. Unknown routes return 404. This teaching server does not provide arbitrary directory browsing, binary files, TLS, persistent connections, request bodies, Range, or conditional caching.

It supports GET and HEAD in the documented HTTP/1.0 and HTTP/1.1 forms, requiring a single nonempty Host for HTTP/1.1. Each connection handles one request and closes. Headers are limited to eight KiB, with two-second read and write deadlines. A single thread advances eight nonblocking connection slots; sockets stay in their owning runtime. Excess connections close.

Run `python tests/chali_book_projects.py` for requests, byte lengths, Unicode text, HEAD, errors, path boundaries, fragmented input, timeouts, and concurrent waiting. See [RFC 9112](https://www.rfc-editor.org/rfc/rfc9112.html) and [HTTP semantics](https://httpwg.org/specs/rfc9110.html) for the wider protocols; the implementation intentionally covers the book's stated subset.
