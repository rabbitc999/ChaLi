# Appendix B Standard-library reference

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../B-标准库速查.md)
<!-- END LANGUAGE NAVIGATION -->

The maintained [API index](../../library/README.md) is the authoritative inventory of modules and signatures. The compiler embeds that index, so `chali man api` works without a source checkout. API pages may retain their source documentation language even when the terminal interface is English.

```sh
chali man api
chali man api std::text
chali man api std::text::Text
chali man api --search deserialize
```

| Need | Start with |
| --- | --- |
| Core values, collections, results and common errors | `std::lang` |
| Iteration and adapters | `std::iter` |
| Text operations and construction | `std::text` |
| Unicode validation and traversal | `std::utf8` |
| Binary encodings | `std::encoding`, `std::binary` |
| Structured data | `std::json`, `std::toml`, `std::csv` |
| Files and terminal input/output | `std::io` |
| Command-line parsing | `std::cli` |
| Time measurement and dates | `std::clock`, `std::datetime` |
| Numeric calculations | `std::math` and the numeric modules in the API index |
| Type metadata and serialization support | `std::reflect` |
| Database access and model annotations | `std::sql` |
| Network services | `std::net` |
| Images | `std::image` |
| Regular expressions | `std::regex` |

Use the index to discover the remaining data structures, algorithms, concurrency and system services; this overview does not maintain a separate module count. Check each API's declared result and resource behavior. Standard error types implement `Error` while preserving module-specific data. Use `MessageError` for message-only application failures.

Host services require the corresponding operating-system facilities and external dependencies. A package for a CPU architecture does not by itself validate every host service or AOT target on that CPU. Performance measurements and supported capabilities have separate documentation.
