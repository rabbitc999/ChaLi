# Log search

The Chapter 16 project searches text line by line, preserving original text and one-based line numbers. Matching is case-sensitive by default.

From this directory:

```sh
chali run src/main.chl -- ERROR sample.log
chali run src/main.chl -- --ignore-case ERROR sample.log
chali run src/main.chl -- --ignore-case --count ERROR sample.log
```

The final command prints 2. An application-level `--` stops option recognition. Empty queries are rejected; text files are limited to four MiB and cannot contain zero bytes. LF and CRLF are supported, and case folding is ASCII-only. No matches is a successful empty result, or zero in count mode. Library errors use Result and the entry reports an unsuccessful invocation through panic; this example does not implement the complete Unix grep exit-status convention.

Search, option parsing, and file access are separate. Run `python tests/chali_book_projects.py` for the project checks.

On Windows, use path characters supported by the current file-opening implementation. UTF-8 Chinese file content and queries remain supported.
