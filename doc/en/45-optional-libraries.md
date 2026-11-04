# Optional host libraries

<!-- LANGUAGE NAVIGATION -->
[English](45-optional-libraries.md) · [简体中文](../45-optional-libraries.md)
<!-- END LANGUAGE NAVIGATION -->

Build these modules on demand with CMake, Ninja and a C/C++ toolchain. Put Windows executables next to the DLL they use. On Linux keep the corresponding shared library with its module. Supply `--llvm` when using the external LLVM variant. The build outputs include upstream license files.

## Desktop windows and audio

```powershell
python tools/setup_desktop_gui.py
chali aot example/desktop-gui/demo.chl -I build/desktop_gui --out build/desktop_gui/demo.exe
```
raylib supplies the window, events and drawing. raygui supplies the widgets. The default build disables audio, 3D models and icons. The demo prints `clicks=N` after the window closes. Call the frame and close operations on the thread that created the window.

```powershell
python tools/setup_desktop_gui.py --audio --out build/desktop_audio
chali run example/desktop-gui/audio.chl -I build/desktop_audio --out build/desktop_audio/audio.exe -- path/to/tone.wav
```
`--audio` enables `desktop_audio` and `--icons` includes raygui icons. `--offline` uses previously verified archives. The audio demo prints `playback finished` on completion or `audio device unavailable` if device initialization fails. Closing the device releases registered sounds.

## XML

```powershell
python tools/setup_xml.py
chali run example/xml/demo.chl -I build/xml --out build/xml/demo.exe
```

```text
XML OK
```
`xml::parse` reads one UTF-8 document and returns its root. Attribute and element names preserve their qualified source spelling. Element children and text fragments retain mixed-content order. The escaping functions distinguish text content from quoted attributes. The parser bounds input, nesting and output, returns one-based error positions and rejects document type declarations and external entities.

## Cryptography

```powershell
python tools/setup_crypto.py
chali run example/crypto/demo.chl -I build/crypto --out build/crypto/std/demo.exe
```

```text
crypto OK
```
`std::crypto` exposes secure random bytes, SHA-256, HMAC-SHA-256 and detached Ed25519 signature verification. Random bytes and digests use lowercase hexadecimal. Verification takes a fixed-length hexadecimal public key and signature. Windows x64 uses a pinned libsodium archive. Linux uses the system development package.

## Streaming HTTP

```powershell
python tools/setup_http_transfer.py
```
Import `std::net::http` and open an `HttpClient` session. `download_to` streams GET responses to a new file. `upload_file` streams a file using PUT. Requests reuse the same session and eligible connections. The deadline covers the entire request. A synchronous callback can cancel the transfer by returning a nonzero value. HTTP status is separate from transport errors. A download keeps its new file only after a completed 2xx response. Close the session to release connections.

The extensions use [raylib](https://github.com/raysan5/raylib), [raygui](https://github.com/raysan5/raygui), [Expat](https://github.com/libexpat/libexpat), [libsodium](https://doc.libsodium.org/) and [libcurl](https://curl.se/libcurl/).

[Download example](../../example/http-transfer/download.chl)。Pass a URL and a new destination path. A successful 200 response prints HTTP 200. A transfer failure prints its error.
