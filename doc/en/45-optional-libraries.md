# Optional host libraries

<!-- LANGUAGE NAVIGATION -->
[English](45-optional-libraries.md) · [简体中文](../45-optional-libraries.md)
<!-- END LANGUAGE NAVIGATION -->

The compiler embeds the declarations for `std::gui`, `std::audio` and `std::xml`. No `-I` is needed. A compiler with the native archives links the C code used by the application into the output. On Windows, distribute the resulting EXE without ChaLi, raylib or Expat DLLs. Graphics and audio still use OS services; any font, image or audio asset that the application opens remains an application resource.

To build the Windows x64 compiler with static LLVM, prepare CMake, Ninja and the SDK and run:

```powershell
mingw32-make CHALI_STATIC_LLVM=1 native-extensions
mingw32-make CHALI_STATIC_LLVM=1 -j8
```

On Linux, run `make native-extensions` followed by `make` with the raylib desktop development headers installed. Building an application that uses strings, structs or other complex C ABI values also requires a same-architecture C++ compiler; set `CHALI_ABI_CXX` to select it. The generated application does not need the toolchain. For the external LLVM variant, supply `--llvm` as configured.

For external builds and testing, run `tools/setup_desktop_gui.py` or `tools/setup_xml.py` without `--static`. An explicit `-I` can point to the installation root containing `std/gui.chl`, `std/audio.chl` or `std/xml.chl`. Keep the shared library paths specified by those declarations when deploying such a build. Cryptography and HTTP streaming still use their own setup scripts.

## Desktop windows and audio

```powershell
chali build example/desktop-gui/demo.chl --out build/gui-demo
```
raylib supplies the window, events and drawing. raygui supplies the widgets. The default build disables audio, 3D models and icons. The demo prints `clicks=N` after the window closes. Call the frame and close operations on the thread that created the window.

```powershell
chali run example/desktop-gui/audio.chl --out build/audio-demo -- path/to/tone.wav
```
`--audio` enables `std::audio` and `--icons` includes raygui icons. `--offline` uses previously verified archives. The audio demo prints `playback finished` on completion or `audio device unavailable` if device initialization fails. Closing the device releases registered sounds.


### Controls and callbacks

The binding covers all **34 controls** in pinned raygui 4.0. Each control has a lowercase factory. `controlCount()` returns 34 and `controlName(index)` returns its factory name.

| ChaLi factory | raygui control |
| --- | --- |
| `windowBox` | `GuiWindowBox` |
| `groupBox` | `GuiGroupBox` |
| `line` | `GuiLine` |
| `panel` | `GuiPanel` |
| `tabBar` | `GuiTabBar` |
| `scrollPanel` | `GuiScrollPanel` |
| `label` | `GuiLabel` |
| `button` | `GuiButton` |
| `labelButton` | `GuiLabelButton` |
| `toggle` | `GuiToggle` |
| `toggleGroup` | `GuiToggleGroup` |
| `toggleSlider` | `GuiToggleSlider` |
| `checkBox` | `GuiCheckBox` |
| `comboBox` | `GuiComboBox` |
| `dropdownBox` | `GuiDropdownBox` |
| `spinner` | `GuiSpinner` |
| `valueBox` | `GuiValueBox` |
| `textBox` | `GuiTextBox` |
| `slider` | `GuiSlider` |
| `sliderBar` | `GuiSliderBar` |
| `progressBar` | `GuiProgressBar` |
| `statusBar` | `GuiStatusBar` |
| `dummyRec` | `GuiDummyRec` |
| `grid` | `GuiGrid` |
| `listView` | `GuiListView` |
| `listViewEx` | `GuiListViewEx` |
| `messageBox` | `GuiMessageBox` |
| `textInputBox` | `GuiTextInputBox` |
| `colorPicker` | `GuiColorPicker` |
| `colorPanel` | `GuiColorPanel` |
| `colorBarAlpha` | `GuiColorBarAlpha` |
| `colorBarHue` | `GuiColorBarHue` |
| `colorPickerHsv` | `GuiColorPickerHSV` |
| `colorPanelHsv` | `GuiColorPanelHSV` |

Each factory takes text and returns a `Widget`. `draw(Rect::at(x, y, width, height))` renders it and returns an event, processing a model at most once per frame. Register synchronous ChaLi callbacks with `onEvent`, `onClick`, `onChange`, `onSubmit` and `onClose`. Language objects own the callbacks; the C library retains only text and control state. `clearCallbacks()` releases captured values. `close()` destroys the model and clears callbacks. Dispatch stops if a callback destroys the widget or its window.

`textBox` and `textInputBox` use C-allocated editable UTF-8 buffers. `setTextCapacity` changes capacity transactionally. Use `checked` for boolean state, `selected` for selections, `number` for integer editors, `value` and `setRange` for sliders, and `color` or HSV for colors. `setItems(List<String>)` accepts up to 128 entries; entries cannot contain semicolons or newlines because raygui uses them as separators. `setContent`, `viewport`, `scrollOffset` and `beginContent` support scroll panels and clipping.

`run(draw, onEvent)` manages frames and shutdown. Window events are polled after controls; text controls consume characters first, so window `TextEntered` events contain remaining characters. Events cover opening, closing, resizing, focus, keys, characters, mouse buttons, movement and wheel input. Use `requestClose()` from a close button to dispatch one `Closing` event before cleanup. `close()` releases the window immediately. Window shutdown frees all native controls; reopening never makes old IDs valid. Language objects and `clearCallbacks` control the lifetime of callback captures.

Perform all UI operations on the creating thread; operations from other threads are rejected. `openWindow(width, height, title, options)` accepts bit flags 1 for resizing, 2 for a hidden window and 4 for vsync. `loadFont(path, size, glyphs)` loads a font with the required characters; the default font does not guarantee Chinese coverage. The original `open`, immediate `label` and immediate `button` interfaces remain available.

The [callback demo](../../example/desktop-gui/demo.chl) shows buttons, a checkbox, text editing, a slider and a progress bar. The native host uses C11 and platform C APIs without requiring the C++ runtime. Audio remains an optional separate build.

### Drawing, custom widgets, and named events

Draw between `beginFrame()` and `endFrame()` or inside the draw callback passed to `run`. Colors use `0xRRGGBBAA`. The drawing API includes pixels, lines, rounded or outlined rectangles, circles, ellipses, triangles, polygons, cubic Bezier curves, rings, and UTF-8 text. `measureText` returns text dimensions. Clipping is nested; `clipBounds()` returns the current intersection.

`loadTexture(path)` loads a PNG into a window-owned `Texture`. `draw` and `drawRegion` support destination rectangles, rotation, and tint. Closing a texture invalidates its handle. Closing the window releases remaining textures, and reopening cannot reuse old identities. Windows UTF-8 font and image paths use C/WinAPI file access.

`customWidget()` adds custom drawing and input without changing the 34 built-in control kinds. `onDraw(fn(Rect))` receives screen bounds. `onEvent(fn(Event))` and `on(name, fn(Event))` receive events routed through `handle(event)`. Pointer coordinates become widget-local, with pointer capture and focus handling. Names include `pointer.down`, `pointer.up`, `pointer.move`, `pointer.wheel`, `key.down`, `key.up`, `text.input`, `focus.gained`, `focus.lost`, and window events. Callbacks may close their own widget or window safely.

`customEvent(name, text, code, value)` owns its text payload. `EventBus` provides `on`, `once`, `off`, synchronous `emit`, and queued `post` / `pump`. One-shot listeners are removed before invocation. Listeners added during dispatch wait for the next dispatch; events posted by queued callbacks wait for the next pump. The queue holds at most 4096 events, a pump processes 256 by default, and recursive dispatch is bounded to depth 64. Check operation results.

See the [custom slider](../../example/desktop-gui/custom.chl) and [Game of Life](../../example/desktop-gui/life.chl). [Book chapter 30](../book/en/30-standard-library-tour.md) contains the complete model, paused left-click toggling, right-button panning, simulation speed control, and actual screenshots.

## XML

```powershell
chali run example/xml/demo.chl --out build/xml-demo
```

```text
XML OK
```
`std::xml::parse` reads one UTF-8 document and returns its root. Attribute and element names preserve their qualified source spelling. Element children and text fragments retain mixed-content order. The escaping functions distinguish text content from quoted attributes. The parser bounds input, nesting and output, returns one-based error positions and rejects document type declarations and external entities.

## Cryptography

```powershell
chali run example/crypto/demo.chl --out build/crypto-demo
```

```text
crypto OK
```
Distributions with native services embed the `std::crypto` module and static archives; no `-I` or separate DLL deployment is required. `randomHex`, `sha256`, `hmacSha256` and `verifyEd25519` expose secure random bytes, SHA-256, HMAC-SHA-256 and detached Ed25519 verification. Random byte counts range from 1 to 65536. Random bytes and digests use lowercase hexadecimal. Verification takes fixed-length hexadecimal public keys and signatures. Windows x64 builds use a pinned libsodium archive; Linux builds use system static development packages. See the [cryptography API reference](../library/std-crypto.md).

## Streaming HTTP

```powershell
python tools/setup_http_transfer.py
```
Import `std::net::http` and open an `HttpClient` session. `downloadTo` streams GET responses to a new file. `uploadFile` streams a file using PUT. Requests reuse the same session and eligible connections. The deadline covers the entire request. A synchronous callback can cancel the transfer by returning a nonzero value. HTTP status is separate from transport errors. A download keeps its new file only after a completed 2xx response. Close the session to release connections.

The extensions use [raylib](https://github.com/raysan5/raylib), [raygui](https://github.com/raysan5/raygui), [Expat](https://github.com/libexpat/libexpat), [libsodium](https://doc.libsodium.org/) and [libcurl](https://curl.se/libcurl/).

[Download example](../../example/http-transfer/download.chl)。Pass a URL and a new destination path. A successful 200 response prints HTTP 200. A transfer failure prints its error.
