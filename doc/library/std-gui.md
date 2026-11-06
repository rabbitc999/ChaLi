# std::gui — 桌面图形界面

<!-- LANGUAGE NAVIGATION -->
[English（中文原文）](std-gui.md) · **简体中文**
<!-- END LANGUAGE NAVIGATION -->

[返回库目录](README.md) · [全部符号](symbols.md)

## 引入

```chali
import std::gui;
```

## 公共约定与错误边界

窗口、控件、绘图和事件操作在创建窗口的线程执行。用 beginFrame/endFrame 配对组织一帧；Widget.draw 每帧对同一控件调用一次并同步派发回调。窗口拥有控件和纹理，关闭窗口后旧句柄失效；用 isValid 检查创建结果。无图形会话时不能运行交互示例。原生服务随支持它的发行版内嵌，无需另行部署共享库。

## 参考代码

完整程序：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

```powershell
chali run example/desktop-gui/demo.chl
```

交互示例需要图形会话或音频设备；音频示例的输入文件及参数见程序入口。

<details>
<summary>展开完整参考代码</summary>

```chali
import std::gui;

void main() {
    assert(std::gui::openWindow(560, 380, "ChaLi desktop", 1));
    let count = std::gui::button("Count");
    let quit = std::gui::button("Close");
    let enabled = std::gui::checkBox("Enable counter");
    let input = std::gui::textBox("Edit this text");
    let slider = std::gui::slider("Value");
    let progress = std::gui::progressBar("");
    assert(enabled.setChecked(true));
    assert(slider.setRange(0, 100)); assert(progress.setRange(0, 100));
    int64 clicks = 0;
    count.onClick(void fn mut(std::gui::Event event) { clicks += 1; });
    quit.onClick(void fn(std::gui::Event event) { std::gui::requestClose(); });
    std::gui::run(void fn mut() {
        std::gui::label((int32)24, (int32)20, (int32)500, (int32)28, "Clicks: " + clicks.toString());
        enabled.draw(std::gui::Rect::at(24, 64, 24, 24));
        count.setEnabled(enabled.checked());
        count.draw(std::gui::Rect::at(24, 108, 152, 36));
        quit.draw(std::gui::Rect::at(200, 108, 152, 36));
        input.draw(std::gui::Rect::at(24, 168, 328, 32));
        slider.draw(std::gui::Rect::at(80, 228, 272, 24));
        progress.setValue(slider.value());
        progress.draw(std::gui::Rect::at(24, 284, 328, 24));
    });
    count.close(); quit.close(); enabled.close(); input.close(); slider.close(); progress.close();
    println("clicks={}", clicks);
}
```

</details>

聚合入口的具体操作参见对应子模块；本页类型/方法的调用形式列在下方。

## 类型、枚举、常量与字段

公开字段按源码原样列出；句柄和游标字段用于包装器状态，通常通过方法操作。枚举的括号表示变体载荷。

| 所属类型 | 声明 | 说明 |
| --- | --- | --- |
| 模块 | <a id="symbol-525"></a>`enum EventKind` | 窗口、输入、控件和自定义事件的分类。 |
| EventKind | `None, Opened, Closing, Resized, Focused, Blurred, KeyPressed, KeyReleased, TextEntered, MousePressed, MouseReleased, MouseMoved, MouseWheel, Control, Custom` | 窗口、输入、控件和自定义事件的分类。 变体及载荷见声明。 |
| 模块 | <a id="symbol-526"></a>`struct Event` | 独立拥有的事件快照；控件 code 保存组合标志。 |
| Event | <a id="symbol-527"></a>`int64 source` | 事件来源控件的身份编号。 |
| Event | <a id="symbol-528"></a>`int64 code` | 事件码；控件事件为组合标志，窗口输入为键码或按钮号。 |
| Event | <a id="symbol-529"></a>`float64 x` | 逻辑像素横坐标。 |
| Event | <a id="symbol-530"></a>`float64 y` | 逻辑像素纵坐标。 |
| Event | <a id="symbol-531"></a>`float64 delta_x` | 横向移动或滚轮增量。 |
| Event | <a id="symbol-532"></a>`float64 delta_y` | 纵向移动或滚轮增量。 |
| Event | <a id="symbol-533"></a>`String name` | 事件名称。 |
| Event | <a id="symbol-534"></a>`String text` | 独立拥有的事件文本。 |
| Event | <a id="symbol-535"></a>`float64 value` | 事件携带的浮点值。 |
| 模块 | <a id="symbol-536"></a>`struct Rect` | 使用逻辑像素坐标的矩形。 |
| Rect | <a id="symbol-537"></a>`float32 x` | 逻辑像素横坐标。 |
| Rect | <a id="symbol-538"></a>`float32 y` | 逻辑像素纵坐标。 |
| Rect | <a id="symbol-539"></a>`float32 width` | 逻辑像素宽度。 |
| Rect | <a id="symbol-540"></a>`float32 height` | 逻辑像素高度。 |
| 模块 | <a id="symbol-541"></a>`struct Point` | 位置、偏移或二维尺寸。 |
| Point | <a id="symbol-542"></a>`float64 x` | 逻辑像素横坐标。 |
| Point | <a id="symbol-543"></a>`float64 y` | 逻辑像素纵坐标。 |
| 模块 | <a id="symbol-544"></a>`struct Widget` | 由窗口拥有的控件句柄与同步回调模型。 |
| 模块 | <a id="symbol-545"></a>`struct Texture` | 由窗口拥有的 GPU 纹理句柄。 |
| 模块 | <a id="symbol-546"></a>`struct EventBus` | 同一线程中的事件订阅与延迟派发队列。 |
| 模块 | <a id="symbol-547"></a>`struct CustomWidget` | 持有绘图、命中检测与输入回调的自绘控件。 |

## 函数索引

| 名称 | 声明 |
| --- | --- |
| [shouldClose](#api-0) | `bool shouldClose()` |
| [beginFrame](#api-1) | `bool beginFrame()` |
| [endFrame](#api-2) | `void endFrame()` |
| [close](#api-3) | `void close()` |
| [open](#api-4) | `bool open(int32 width, int32 height, String title)` |
| [label](#api-5) | `void label(int32 x, int32 y, int32 width, int32 height, String text)` |
| [button](#api-6) | `bool button(int32 x, int32 y, int32 width, int32 height, String text)` |
| [Event::isValid](#api-7) | `bool isValid()` |
| [Event::kind](#api-8) | `EventKind kind()` |
| [Event::changed](#api-9) | `bool changed()` |
| [Event::clicked](#api-10) | `bool clicked()` |
| [Event::submitted](#api-11) | `bool submitted()` |
| [Event::editingChanged](#api-12) | `bool editingChanged()` |
| [Event::closed](#api-13) | `bool closed()` |
| [Event::selectionChanged](#api-14) | `bool selectionChanged()` |
| [Event::scrolled](#api-15) | `bool scrolled()` |
| [Event::hovered](#api-16) | `bool hovered()` |
| [Event::focusChanged](#api-17) | `bool focusChanged()` |
| [Rect::at](#api-18) | `static Rect at(float64 x, float64 y, float64 width, float64 height)` |
| [Rect::contains](#api-19) | `bool contains(float64 x, float64 y)` |
| [Rect::intersection](#api-20) | `Rect intersection(Rect other)` |
| [Rect::isValid](#api-21) | `bool isValid()` |
| [Point::at](#api-22) | `static Point at(float64 x, float64 y)` |
| [requestClose](#api-23) | `void requestClose()` |
| [setBackground](#api-24) | `void setBackground(uint32 rgba)` |
| [endClip](#api-25) | `void endClip()` |
| [setStyle](#api-26) | `bool setStyle(int32 control, int32 property, int32 value)` |
| [getStyle](#api-27) | `int32 getStyle(int32 control, int32 property)` |
| [loadFont](#api-28) | `bool loadFont(String path, int32 size, String glyphs)` |
| [openWindow](#api-29) | `bool openWindow(int64 width, int64 height, String title, int64 options)` |
| [open](#api-30) | `bool open(int64 width, int64 height, String title)` |
| [setTitle](#api-31) | `bool setTitle(String text)` |
| [setTargetFps](#api-32) | `bool setTargetFps(int64 fps)` |
| [setExitKey](#api-33) | `bool setExitKey(int64 key)` |
| [setMinSize](#api-34) | `bool setMinSize(int64 width, int64 height)` |
| [setOpacity](#api-35) | `bool setOpacity(float64 value)` |
| [width](#api-36) | `int64 width()` |
| [height](#api-37) | `int64 height()` |
| [frameTime](#api-38) | `float64 frameTime()` |
| [mousePosition](#api-39) | `Point mousePosition()` |
| [mouseWheel](#api-40) | `Point mouseWheel()` |
| [dpiScale](#api-41) | `Point dpiScale()` |
| [focused](#api-42) | `bool focused()` |
| [time](#api-43) | `float64 time()` |
| [beginClip](#api-44) | `bool beginClip(Rect bounds)` |
| [enable](#api-45) | `bool enable(bool value)` |
| [lock](#api-46) | `bool lock(bool value)` |
| [setAlpha](#api-47) | `bool setAlpha(float64 value)` |
| [setState](#api-48) | `bool setState(int64 value)` |
| [enableTooltips](#api-49) | `bool enableTooltips(bool value)` |
| [resetStyle](#api-50) | `bool resetStyle()` |
| [setFontSize](#api-51) | `bool setFontSize(int64 value)` |
| [setTooltip](#api-52) | `bool setTooltip(String text)` |
| [rgba](#api-53) | `uint32 rgba(int64 red, int64 green, int64 blue, int64 alpha)` |
| [pollEvent](#api-54) | `Event pollEvent()` |
| [Widget::isValid](#api-55) | `bool isValid()` |
| [Widget::id](#api-56) | `int64 id()` |
| [Widget::clearCallbacks](#api-57) | `void clearCallbacks()` |
| [Widget::close](#api-58) | `void close()` |
| [Widget::onEvent](#api-59) | `void onEvent(void fn(Event) callback)` |
| [Widget::onClick](#api-60) | `void onClick(void fn(Event) callback)` |
| [Widget::onChange](#api-61) | `void onChange(void fn(Event) callback)` |
| [Widget::onSubmit](#api-62) | `void onSubmit(void fn(Event) callback)` |
| [Widget::onClose](#api-63) | `void onClose(void fn(Event) callback)` |
| [Widget::draw](#api-64) | `Event draw(Rect bounds)` |
| [Widget::setText](#api-65) | `bool setText(String text)` |
| [Widget::text](#api-66) | `String text()` |
| [Widget::setLeftText](#api-67) | `bool setLeftText(String text)` |
| [Widget::setRightText](#api-68) | `bool setRightText(String text)` |
| [Widget::setMessage](#api-69) | `bool setMessage(String text)` |
| [Widget::setButtons](#api-70) | `bool setButtons(String text)` |
| [Widget::setItems](#api-71) | `bool setItems(List<String> items)` |
| [Widget::setTextCapacity](#api-72) | `bool setTextCapacity(int64 capacity)` |
| [Widget::setChecked](#api-73) | `bool setChecked(bool value)` |
| [Widget::checked](#api-74) | `bool checked()` |
| [Widget::setEditing](#api-75) | `bool setEditing(bool value)` |
| [Widget::editing](#api-76) | `bool editing()` |
| [Widget::setSecret](#api-77) | `bool setSecret(bool value)` |
| [Widget::secret](#api-78) | `bool secret()` |
| [Widget::setVisible](#api-79) | `bool setVisible(bool value)` |
| [Widget::visible](#api-80) | `bool visible()` |
| [Widget::setEnabled](#api-81) | `bool setEnabled(bool value)` |
| [Widget::enabled](#api-82) | `bool enabled()` |
| [Widget::setSelected](#api-83) | `bool setSelected(int64 value)` |
| [Widget::selected](#api-84) | `int64 selected()` |
| [Widget::setNumber](#api-85) | `bool setNumber(int64 value)` |
| [Widget::number](#api-86) | `int64 number()` |
| [Widget::result](#api-87) | `int64 result()` |
| [Widget::setScrollIndex](#api-88) | `bool setScrollIndex(int64 value)` |
| [Widget::scrollIndex](#api-89) | `int64 scrollIndex()` |
| [Widget::focusIndex](#api-90) | `int64 focusIndex()` |
| [Widget::setRange](#api-91) | `bool setRange(float64 minimum, float64 maximum)` |
| [Widget::setValue](#api-92) | `bool setValue(float64 value)` |
| [Widget::value](#api-93) | `float64 value()` |
| [Widget::setColor](#api-94) | `bool setColor(uint32 rgba)` |
| [Widget::color](#api-95) | `uint32 color()` |
| [Widget::setHsv](#api-96) | `bool setHsv(float64 hue, float64 saturation, float64 brightness)` |
| [Widget::hue](#api-97) | `float64 hue()` |
| [Widget::saturation](#api-98) | `float64 saturation()` |
| [Widget::brightness](#api-99) | `float64 brightness()` |
| [Widget::setSpacing](#api-100) | `bool setSpacing(float64 spacing, int64 subdivisions)` |
| [Widget::setContent](#api-101) | `bool setContent(Rect bounds)` |
| [Widget::viewport](#api-102) | `Rect viewport()` |
| [Widget::scrollOffset](#api-103) | `Point scrollOffset()` |
| [Widget::setScrollOffset](#api-104) | `bool setScrollOffset(float64 x, float64 y)` |
| [Widget::beginContent](#api-105) | `bool beginContent()` |
| [createControl](#api-106) | `Widget createControl(int64 kind, String text)` |
| [controlCount](#api-107) | `int64 controlCount()` |
| [windowBox](#api-108) | `Widget windowBox(String text)` |
| [groupBox](#api-109) | `Widget groupBox(String text)` |
| [line](#api-110) | `Widget line(String text)` |
| [panel](#api-111) | `Widget panel(String text)` |
| [tabBar](#api-112) | `Widget tabBar(String text)` |
| [scrollPanel](#api-113) | `Widget scrollPanel(String text)` |
| [label](#api-114) | `Widget label(String text)` |
| [button](#api-115) | `Widget button(String text)` |
| [labelButton](#api-116) | `Widget labelButton(String text)` |
| [toggle](#api-117) | `Widget toggle(String text)` |
| [toggleGroup](#api-118) | `Widget toggleGroup(String text)` |
| [toggleSlider](#api-119) | `Widget toggleSlider(String text)` |
| [checkBox](#api-120) | `Widget checkBox(String text)` |
| [comboBox](#api-121) | `Widget comboBox(String text)` |
| [dropdownBox](#api-122) | `Widget dropdownBox(String text)` |
| [spinner](#api-123) | `Widget spinner(String text)` |
| [valueBox](#api-124) | `Widget valueBox(String text)` |
| [textBox](#api-125) | `Widget textBox(String text)` |
| [slider](#api-126) | `Widget slider(String text)` |
| [sliderBar](#api-127) | `Widget sliderBar(String text)` |
| [progressBar](#api-128) | `Widget progressBar(String text)` |
| [statusBar](#api-129) | `Widget statusBar(String text)` |
| [dummyRec](#api-130) | `Widget dummyRec(String text)` |
| [grid](#api-131) | `Widget grid(String text)` |
| [listView](#api-132) | `Widget listView(String text)` |
| [listViewEx](#api-133) | `Widget listViewEx(String text)` |
| [messageBox](#api-134) | `Widget messageBox(String text)` |
| [textInputBox](#api-135) | `Widget textInputBox(String text)` |
| [colorPicker](#api-136) | `Widget colorPicker(String text)` |
| [colorPanel](#api-137) | `Widget colorPanel(String text)` |
| [colorBarAlpha](#api-138) | `Widget colorBarAlpha(String text)` |
| [colorBarHue](#api-139) | `Widget colorBarHue(String text)` |
| [colorPickerHsv](#api-140) | `Widget colorPickerHsv(String text)` |
| [colorPanelHsv](#api-141) | `Widget colorPanelHsv(String text)` |
| [controlName](#api-142) | `String controlName(int64 index)` |
| [frameActive](#api-143) | `bool frameActive()` |
| [inputEnabled](#api-144) | `bool inputEnabled()` |
| [frameCount](#api-145) | `uint64 frameCount()` |
| [clipBounds](#api-146) | `Rect clipBounds()` |
| [drawPixel](#api-147) | `bool drawPixel(Point point, uint32 color)` |
| [drawLine](#api-148) | `bool drawLine(Point start, Point end, float64 thickness, uint32 color)` |
| [drawRectangle](#api-149) | `bool drawRectangle(Rect bounds, uint32 color, float64 thickness = 0, float64 roundness = 0)` |
| [drawCircle](#api-150) | `bool drawCircle(Point center, float64 radius, uint32 color, float64 thickness = 0)` |
| [drawEllipse](#api-151) | `bool drawEllipse(Point center, Point radius, uint32 color, bool outline = false)` |
| [drawTriangle](#api-152) | `bool drawTriangle(Point a, Point b, Point c, uint32 color, bool outline = false)` |
| [drawPolygon](#api-153) | `bool drawPolygon(Point center, int64 sides, float64 radius, float64 rotation, uint32 color, float64 thickness = 0)` |
| [drawBezier](#api-154) | `bool drawBezier(Point start, Point first_control, Point second_control, Point end, float64 thickness, uint32 color)` |
| [drawRing](#api-155) | `bool drawRing(Point center, float64 inner_radius, float64 outer_radius, float64 start_angle, float64 end_angle, uint32 color)` |
| [drawText](#api-156) | `bool drawText(String text, Point position, float64 size, uint32 color, float64 spacing = 1)` |
| [measureText](#api-157) | `Point measureText(String text, float64 size, float64 spacing = 1)` |
| [Texture::isValid](#api-158) | `bool isValid()` |
| [Texture::close](#api-159) | `void close()` |
| [Texture::size](#api-160) | `Point size()` |
| [Texture::draw](#api-161) | `bool draw(Rect destination, uint32 tint = (uint32)4294967295)` |
| [Texture::drawRegion](#api-162) | `bool drawRegion(Rect source, Rect destination, Point origin, float64 rotation, uint32 tint = (uint32)4294967295)` |
| [loadTexture](#api-163) | `Texture loadTexture(String path)` |
| [customEvent](#api-164) | `Event customEvent(String name, String text = "", int64 code = 0, float64 value = 0)` |
| [EventBus::on](#api-165) | `int64 on(String name, void fn(Event) callback)` |
| [EventBus::once](#api-166) | `int64 once(String name, void fn(Event) callback)` |
| [EventBus::off](#api-167) | `bool off(int64 id)` |
| [EventBus::clear](#api-168) | `void clear()` |
| [EventBus::emit](#api-169) | `int64 emit(Event event)` |
| [EventBus::post](#api-170) | `bool post(Event event)` |
| [EventBus::pending](#api-171) | `int64 pending()` |
| [EventBus::pump](#api-172) | `int64 pump(int64 maximum = 256)` |
| [CustomWidget::isValid](#api-173) | `bool isValid()` |
| [CustomWidget::id](#api-174) | `int64 id()` |
| [CustomWidget::onDraw](#api-175) | `void onDraw(void fn(Rect) callback)` |
| [CustomWidget::onEvent](#api-176) | `void onEvent(void fn(Event) callback)` |
| [CustomWidget::on](#api-177) | `int64 on(String name, void fn(Event) callback)` |
| [CustomWidget::once](#api-178) | `int64 once(String name, void fn(Event) callback)` |
| [CustomWidget::off](#api-179) | `bool off(int64 id)` |
| [CustomWidget::clearCallbacks](#api-180) | `void clearCallbacks()` |
| [CustomWidget::close](#api-181) | `void close()` |
| [CustomWidget::focus](#api-182) | `bool focus()` |
| [CustomWidget::blur](#api-183) | `void blur()` |
| [CustomWidget::hasFocus](#api-184) | `bool hasFocus()` |
| [CustomWidget::setVisible](#api-185) | `bool setVisible(bool visible)` |
| [CustomWidget::setEnabled](#api-186) | `bool setEnabled(bool enabled)` |
| [CustomWidget::visible](#api-187) | `bool visible()` |
| [CustomWidget::enabled](#api-188) | `bool enabled()` |
| [CustomWidget::setText](#api-189) | `bool setText(String text)` |
| [CustomWidget::text](#api-190) | `String text()` |
| [CustomWidget::setRange](#api-191) | `bool setRange(float64 minimum, float64 maximum)` |
| [CustomWidget::setValue](#api-192) | `bool setValue(float64 value)` |
| [CustomWidget::value](#api-193) | `float64 value()` |
| [CustomWidget::setNumber](#api-194) | `bool setNumber(int64 value)` |
| [CustomWidget::number](#api-195) | `int64 number()` |
| [CustomWidget::setChecked](#api-196) | `bool setChecked(bool value)` |
| [CustomWidget::checked](#api-197) | `bool checked()` |
| [CustomWidget::bounds](#api-198) | `Rect bounds()` |
| [CustomWidget::setClipping](#api-199) | `void setClipping(bool enabled)` |
| [CustomWidget::draw](#api-200) | `bool draw(Rect bounds)` |
| [CustomWidget::handle](#api-201) | `bool handle(Event event)` |
| [CustomWidget::emit](#api-202) | `bool emit(Event event)` |
| [customWidget](#api-203) | `CustomWidget customWidget()` |
| [run](#api-204) | `void run(void fn() draw, void fn(Event) onEvent)` |
| [run](#api-205) | `void run(void fn() draw)` |

<a id="api-0"></a>

### shouldClose（1）

**声明**

```chali
bool shouldClose();
```

**行为**

查询窗口是否已收到关闭请求。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。查询窗口是否已收到关闭请求。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::shouldClose();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:8](../../library/desktop_gui/desktop_gui.chl.in#L8)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-1"></a>

### beginFrame（2）

**声明**

```chali
bool beginFrame();
```

**行为**

开始一帧，失败时返回 false。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。开始一帧，失败时返回 false。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::beginFrame();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:11](../../library/desktop_gui/desktop_gui.chl.in#L11)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-2"></a>

### endFrame（3）

**声明**

```chali
void endFrame();
```

**行为**

提交当前帧并结束绘制。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::endFrame();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:14](../../library/desktop_gui/desktop_gui.chl.in#L14)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-3"></a>

### close（4）

**声明**

```chali
void close();
```

**行为**

释放当前窗口及它拥有的控件、字体与纹理资源。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::close();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:17](../../library/desktop_gui/desktop_gui.chl.in#L17)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-4"></a>

### open（5）

**声明**

```chali
bool open(int32 width, int32 height, String title);
```

**行为**

按指定逻辑尺寸和标题打开窗口，成功返回 true。

保留原即时接口，并允许与控件工厂重载。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `width` | `int32 width` | 逻辑像素宽度。 | 无 |
| `height` | `int32 height` | 逻辑像素高度。 | 无 |
| `title` | `String title` | 窗口标题。 | 无 |

**返回值**

`bool`。按指定逻辑尺寸和标题打开窗口，成功返回 true。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::open(width, height, title);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:26](../../library/desktop_gui/desktop_gui.chl.in#L26)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-5"></a>

### label（6）

**声明**

```chali
void label(int32 x, int32 y, int32 width, int32 height, String text);
```

**行为**

在指定逻辑像素矩形内绘制即时标签。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `x` | `int32 x` | 逻辑像素横坐标。 | 无 |
| `y` | `int32 y` | 逻辑像素纵坐标。 | 无 |
| `width` | `int32 width` | 逻辑像素宽度。 | 无 |
| `height` | `int32 height` | 逻辑像素高度。 | 无 |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::label(x, y, width, height, text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:27](../../library/desktop_gui/desktop_gui.chl.in#L27)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-6"></a>

### button（7）

**声明**

```chali
bool button(int32 x, int32 y, int32 width, int32 height, String text);
```

**行为**

在指定逻辑像素矩形内绘制即时按钮，返回本帧是否点击。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `x` | `int32 x` | 逻辑像素横坐标。 | 无 |
| `y` | `int32 y` | 逻辑像素纵坐标。 | 无 |
| `width` | `int32 width` | 逻辑像素宽度。 | 无 |
| `height` | `int32 height` | 逻辑像素高度。 | 无 |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`bool`。在指定逻辑像素矩形内绘制即时按钮，返回本帧是否点击。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::button(x, y, width, height, text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:28](../../library/desktop_gui/desktop_gui.chl.in#L28)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-7"></a>

### Event::isValid（8）

**声明**

```chali
bool isValid();
```

**行为**

检查事件是否为非空事件。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。检查事件是否为非空事件。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isValid();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:52](../../library/desktop_gui/desktop_gui.chl.in#L52)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-8"></a>

### Event::kind（9）

**声明**

```chali
EventKind kind();
```

**行为**

将宿主事件码转换为 EventKind。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`EventKind`。将宿主事件码转换为 EventKind。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.kind();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:53](../../library/desktop_gui/desktop_gui.chl.in#L53)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-9"></a>

### Event::changed（10）

**声明**

```chali
bool changed();
```

**行为**

检查控件事件是否包含 值变化 标志。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。检查控件事件是否包含 值变化 标志。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.changed();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:70](../../library/desktop_gui/desktop_gui.chl.in#L70)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-10"></a>

### Event::clicked（11）

**声明**

```chali
bool clicked();
```

**行为**

检查控件事件是否包含 点击 标志。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。检查控件事件是否包含 点击 标志。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.clicked();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:71](../../library/desktop_gui/desktop_gui.chl.in#L71)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-11"></a>

### Event::submitted（12）

**声明**

```chali
bool submitted();
```

**行为**

检查控件事件是否包含 提交 标志。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。检查控件事件是否包含 提交 标志。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.submitted();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:72](../../library/desktop_gui/desktop_gui.chl.in#L72)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-12"></a>

### Event::editingChanged（13）

**声明**

```chali
bool editingChanged();
```

**行为**

检查控件事件是否包含 编辑变化 标志。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。检查控件事件是否包含 编辑变化 标志。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.editingChanged();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:73](../../library/desktop_gui/desktop_gui.chl.in#L73)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-13"></a>

### Event::closed（14）

**声明**

```chali
bool closed();
```

**行为**

检查控件事件是否包含 关闭 标志。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。检查控件事件是否包含 关闭 标志。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.closed();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:74](../../library/desktop_gui/desktop_gui.chl.in#L74)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-14"></a>

### Event::selectionChanged（15）

**声明**

```chali
bool selectionChanged();
```

**行为**

检查控件事件是否包含 选择变化 标志。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。检查控件事件是否包含 选择变化 标志。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.selectionChanged();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:75](../../library/desktop_gui/desktop_gui.chl.in#L75)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-15"></a>

### Event::scrolled（16）

**声明**

```chali
bool scrolled();
```

**行为**

检查控件事件是否包含 滚动 标志。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。检查控件事件是否包含 滚动 标志。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.scrolled();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:76](../../library/desktop_gui/desktop_gui.chl.in#L76)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-16"></a>

### Event::hovered（17）

**声明**

```chali
bool hovered();
```

**行为**

检查控件事件是否包含 悬停 标志。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。检查控件事件是否包含 悬停 标志。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.hovered();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:77](../../library/desktop_gui/desktop_gui.chl.in#L77)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-17"></a>

### Event::focusChanged（18）

**声明**

```chali
bool focusChanged();
```

**行为**

检查控件事件是否包含 焦点变化 标志。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。检查控件事件是否包含 焦点变化 标志。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.focusChanged();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:78](../../library/desktop_gui/desktop_gui.chl.in#L78)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-18"></a>

### Rect::at（19）

**声明**

```chali
static Rect at(float64 x, float64 y, float64 width, float64 height);
```

**行为**

按指定分量构造几何值。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `x` | `float64 x` | 逻辑像素横坐标。 | 无 |
| `y` | `float64 y` | 逻辑像素纵坐标。 | 无 |
| `width` | `float64 width` | 逻辑像素宽度。 | 无 |
| `height` | `float64 height` | 逻辑像素高度。 | 无 |

**返回值**

`Rect`。按指定分量构造几何值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::Rect::at(x, y, width, height);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:86](../../library/desktop_gui/desktop_gui.chl.in#L86)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-19"></a>

### Rect::contains（20）

**声明**

```chali
bool contains(float64 x, float64 y);
```

**行为**

检查给定点是否在矩形内。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `x` | `float64 x` | 逻辑像素横坐标。 | 无 |
| `y` | `float64 y` | 逻辑像素纵坐标。 | 无 |

**返回值**

`bool`。检查给定点是否在矩形内。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.contains(x, y);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:90](../../library/desktop_gui/desktop_gui.chl.in#L90)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-20"></a>

### Rect::intersection（21）

**声明**

```chali
Rect intersection(Rect other);
```

**行为**

返回两个矩形的相交区域。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `other` | `Rect other` | 参与相交运算的另一个矩形。 | 无 |

**返回值**

`Rect`。返回两个矩形的相交区域。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.intersection(other);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:91](../../library/desktop_gui/desktop_gui.chl.in#L91)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-21"></a>

### Rect::isValid（22）

**声明**

```chali
bool isValid();
```

**行为**

检查矩形宽度和高度是否为正。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。检查矩形宽度和高度是否为正。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isValid();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:101](../../library/desktop_gui/desktop_gui.chl.in#L101)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-22"></a>

### Point::at（23）

**声明**

```chali
static Point at(float64 x, float64 y);
```

**行为**

按指定分量构造几何值。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `x` | `float64 x` | 逻辑像素横坐标。 | 无 |
| `y` | `float64 y` | 逻辑像素纵坐标。 | 无 |

**返回值**

`Point`。按指定分量构造几何值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::Point::at(x, y);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:106](../../library/desktop_gui/desktop_gui.chl.in#L106)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-23"></a>

### requestClose（24）

**声明**

```chali
void requestClose();
```

**行为**

向当前窗口提交关闭请求。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::requestClose();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:116](../../library/desktop_gui/desktop_gui.chl.in#L116)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-24"></a>

### setBackground（25）

**声明**

```chali
void setBackground(uint32 rgba);
```

**行为**

设置帧背景颜色，编码为 RRGGBBAA。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `rgba` | `uint32 rgba` | RRGGBBAA 颜色编码。 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::setBackground(rgba);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:126](../../library/desktop_gui/desktop_gui.chl.in#L126)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-25"></a>

### endClip（26）

**声明**

```chali
void endClip();
```

**行为**

结束当前裁剪区域，与 beginClip 配对。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::endClip();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:130](../../library/desktop_gui/desktop_gui.chl.in#L130)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-26"></a>

### setStyle（27）

**声明**

```chali
bool setStyle(int32 control, int32 property, int32 value);
```

**行为**

设置指定控件类别的样式属性。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `control` | `int32 control` | 控件样式类别编号。 | 无 |
| `property` | `int32 property` | 样式属性编号。 | 无 |
| `value` | `int32 value` | 本项操作使用的值，含义见行为说明。 | 无 |

**返回值**

`bool`。设置指定控件类别的样式属性。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::setStyle(control, property, value);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:134](../../library/desktop_gui/desktop_gui.chl.in#L134)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-27"></a>

### getStyle（28）

**声明**

```chali
int32 getStyle(int32 control, int32 property);
```

**行为**

读取指定控件类别的样式属性值。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `control` | `int32 control` | 控件样式类别编号。 | 无 |
| `property` | `int32 property` | 样式属性编号。 | 无 |

**返回值**

`int32`。读取指定控件类别的样式属性值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::getStyle(control, property);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:136](../../library/desktop_gui/desktop_gui.chl.in#L136)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-28"></a>

### loadFont（29）

**声明**

```chali
bool loadFont(String path, int32 size, String glyphs);
```

**行为**

从文件加载指定字号和字形集合的字体。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `path` | `String path` | 输入文件路径。 | 无 |
| `size` | `int32 size` | 字体大小。 | 无 |
| `glyphs` | `String glyphs` | 需要加载的 UTF-8 字符集合。 | 无 |

**返回值**

`bool`。从文件加载指定字号和字形集合的字体。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::loadFont(path, size, glyphs);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:140](../../library/desktop_gui/desktop_gui.chl.in#L140)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-29"></a>

### openWindow（30）

**声明**

```chali
bool openWindow(int64 width, int64 height, String title, int64 options);
```

**行为**

按指定尺寸、标题与选项打开窗口；options 的 1、2、4 位分别启用缩放、隐藏和垂直同步。

支持默认整数宽高；options 的 1/2/4 分别为可缩放、隐藏和垂直同步。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `width` | `int64 width` | 逻辑像素宽度。 | 无 |
| `height` | `int64 height` | 逻辑像素高度。 | 无 |
| `title` | `String title` | 窗口标题。 | 无 |
| `options` | `int64 options` | 窗口选项位：1 可缩放，2 隐藏，4 垂直同步。 | 无 |

**返回值**

`bool`。按指定尺寸、标题与选项打开窗口；options 的 1、2、4 位分别启用缩放、隐藏和垂直同步。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::openWindow(width, height, title, options);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:167](../../library/desktop_gui/desktop_gui.chl.in#L167)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-30"></a>

### open（31）

**声明**

```chali
bool open(int64 width, int64 height, String title);
```

**行为**

按指定逻辑尺寸和标题打开窗口，成功返回 true。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `width` | `int64 width` | 逻辑像素宽度。 | 无 |
| `height` | `int64 height` | 逻辑像素高度。 | 无 |
| `title` | `String title` | 窗口标题。 | 无 |

**返回值**

`bool`。按指定逻辑尺寸和标题打开窗口，成功返回 true。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::open(width, height, title);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:171](../../library/desktop_gui/desktop_gui.chl.in#L171)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-31"></a>

### setTitle（32）

**声明**

```chali
bool setTitle(String text);
```

**行为**

设置窗口标题。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`bool`。设置窗口标题。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::setTitle(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:172](../../library/desktop_gui/desktop_gui.chl.in#L172)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-32"></a>

### setTargetFps（33）

**声明**

```chali
bool setTargetFps(int64 fps);
```

**行为**

设置目标帧率。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `fps` | `int64 fps` | 每秒目标帧数。 | 无 |

**返回值**

`bool`。设置目标帧率。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::setTargetFps(fps);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:173](../../library/desktop_gui/desktop_gui.chl.in#L173)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-33"></a>

### setExitKey（34）

**声明**

```chali
bool setExitKey(int64 key);
```

**行为**

设置触发窗口关闭的按键。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `key` | `int64 key` | HMAC 密钥或按键码，按本项签名与行为解释。 | 无 |

**返回值**

`bool`。设置触发窗口关闭的按键。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::setExitKey(key);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:174](../../library/desktop_gui/desktop_gui.chl.in#L174)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-34"></a>

### setMinSize（35）

**声明**

```chali
bool setMinSize(int64 width, int64 height);
```

**行为**

设置窗口可缩放的最小尺寸。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `width` | `int64 width` | 逻辑像素宽度。 | 无 |
| `height` | `int64 height` | 逻辑像素高度。 | 无 |

**返回值**

`bool`。设置窗口可缩放的最小尺寸。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::setMinSize(width, height);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:175](../../library/desktop_gui/desktop_gui.chl.in#L175)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-35"></a>

### setOpacity（36）

**声明**

```chali
bool setOpacity(float64 value);
```

**行为**

设置窗口透明度。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `float64 value` | 本项操作使用的值，含义见行为说明。 | 无 |

**返回值**

`bool`。设置窗口透明度。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::setOpacity(value);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:179](../../library/desktop_gui/desktop_gui.chl.in#L179)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-36"></a>

### width（37）

**声明**

```chali
int64 width();
```

**行为**

返回窗口逻辑宽度。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回窗口逻辑宽度。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::width();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:180](../../library/desktop_gui/desktop_gui.chl.in#L180)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-37"></a>

### height（38）

**声明**

```chali
int64 height();
```

**行为**

返回窗口逻辑高度。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回窗口逻辑高度。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::height();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:181](../../library/desktop_gui/desktop_gui.chl.in#L181)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-38"></a>

### frameTime（39）

**声明**

```chali
float64 frameTime();
```

**行为**

返回最近一帧耗时，单位为秒。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`float64`。返回最近一帧耗时，单位为秒。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::frameTime();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:182](../../library/desktop_gui/desktop_gui.chl.in#L182)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-39"></a>

### mousePosition（40）

**声明**

```chali
Point mousePosition();
```

**行为**

返回鼠标在窗口内的位置。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Point`。返回鼠标在窗口内的位置。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::mousePosition();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:183](../../library/desktop_gui/desktop_gui.chl.in#L183)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-40"></a>

### mouseWheel（41）

**声明**

```chali
Point mouseWheel();
```

**行为**

返回本帧鼠标滚轮增量。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Point`。返回本帧鼠标滚轮增量。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::mouseWheel();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:184](../../library/desktop_gui/desktop_gui.chl.in#L184)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-41"></a>

### dpiScale（42）

**声明**

```chali
Point dpiScale();
```

**行为**

返回窗口的二维 DPI 缩放比例。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Point`。返回窗口的二维 DPI 缩放比例。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::dpiScale();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:185](../../library/desktop_gui/desktop_gui.chl.in#L185)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-42"></a>

### focused（43）

**声明**

```chali
bool focused();
```

**行为**

查询窗口是否获得输入焦点。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。查询窗口是否获得输入焦点。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::focused();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:186](../../library/desktop_gui/desktop_gui.chl.in#L186)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-43"></a>

### time（44）

**声明**

```chali
float64 time();
```

**行为**

返回宿主启动后的计时秒数。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`float64`。返回宿主启动后的计时秒数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::time();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:187](../../library/desktop_gui/desktop_gui.chl.in#L187)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-44"></a>

### beginClip（45）

**声明**

```chali
bool beginClip(Rect bounds);
```

**行为**

按逻辑像素矩形开始裁剪，成功返回 true。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `bounds` | `Rect bounds` | 逻辑像素边界矩形。 | 无 |

**返回值**

`bool`。按逻辑像素矩形开始裁剪，成功返回 true。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::beginClip(bounds);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:188](../../library/desktop_gui/desktop_gui.chl.in#L188)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-45"></a>

### enable（46）

**声明**

```chali
bool enable(bool value);
```

**行为**

设置全局控件输入启用状态。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `bool value` | 本项操作使用的值，含义见行为说明。 | 无 |

**返回值**

`bool`。设置全局控件输入启用状态。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::enable(value);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:189](../../library/desktop_gui/desktop_gui.chl.in#L189)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-46"></a>

### lock（47）

**声明**

```chali
bool lock(bool value);
```

**行为**

设置全局控件锁定状态。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `bool value` | 本项操作使用的值，含义见行为说明。 | 无 |

**返回值**

`bool`。设置全局控件锁定状态。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::lock(value);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:190](../../library/desktop_gui/desktop_gui.chl.in#L190)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-47"></a>

### setAlpha（48）

**声明**

```chali
bool setAlpha(float64 value);
```

**行为**

设置全局控件透明度。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `float64 value` | 本项操作使用的值，含义见行为说明。 | 无 |

**返回值**

`bool`。设置全局控件透明度。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::setAlpha(value);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:191](../../library/desktop_gui/desktop_gui.chl.in#L191)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-48"></a>

### setState（49）

**声明**

```chali
bool setState(int64 value);
```

**行为**

设置全局控件绘制状态。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `int64 value` | 本项操作使用的值，含义见行为说明。 | 无 |

**返回值**

`bool`。设置全局控件绘制状态。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::setState(value);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:192](../../library/desktop_gui/desktop_gui.chl.in#L192)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-49"></a>

### enableTooltips（50）

**声明**

```chali
bool enableTooltips(bool value);
```

**行为**

启用或禁用控件提示。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `bool value` | 本项操作使用的值，含义见行为说明。 | 无 |

**返回值**

`bool`。启用或禁用控件提示。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::enableTooltips(value);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:193](../../library/desktop_gui/desktop_gui.chl.in#L193)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-50"></a>

### resetStyle（51）

**声明**

```chali
bool resetStyle();
```

**行为**

恢复默认控件样式。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。恢复默认控件样式。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::resetStyle();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:194](../../library/desktop_gui/desktop_gui.chl.in#L194)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-51"></a>

### setFontSize（52）

**声明**

```chali
bool setFontSize(int64 value);
```

**行为**

设置控件默认字体大小。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `int64 value` | 本项操作使用的值，含义见行为说明。 | 无 |

**返回值**

`bool`。设置控件默认字体大小。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::setFontSize(value);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:195](../../library/desktop_gui/desktop_gui.chl.in#L195)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-52"></a>

### setTooltip（53）

**声明**

```chali
bool setTooltip(String text);
```

**行为**

设置当前控件提示文本。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`bool`。设置当前控件提示文本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::setTooltip(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:196](../../library/desktop_gui/desktop_gui.chl.in#L196)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-53"></a>

### rgba（54）

**声明**

```chali
uint32 rgba(int64 red, int64 green, int64 blue, int64 alpha);
```

**行为**

把四个 0–255 的颜色通道编码成 RRGGBBAA。

每个颜色通道为 0..255，返回稳定的 RRGGBBAA。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `red` | `int64 red` | 红通道，0–255。 | 无 |
| `green` | `int64 green` | 绿通道，0–255。 | 无 |
| `blue` | `int64 blue` | 蓝通道，0–255。 | 无 |
| `alpha` | `int64 alpha` | 透明通道，0–255。 | 无 |

**返回值**

`uint32`。把四个 0–255 的颜色通道编码成 RRGGBBAA。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::rgba(red, green, blue, alpha);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:198](../../library/desktop_gui/desktop_gui.chl.in#L198)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-54"></a>

### pollEvent（55）

**声明**

```chali
Event pollEvent();
```

**行为**

取出下一项窗口事件；队列为空时返回无效 Event。

把宿主标量事件复制为语言事件。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Event`。取出下一项窗口事件；队列为空时返回无效 Event。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::pollEvent();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:204](../../library/desktop_gui/desktop_gui.chl.in#L204)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-55"></a>

### Widget::isValid（56）

**声明**

```chali
bool isValid();
```

**行为**

检查当前值的宿主句柄或几何边界是否有效。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。检查当前值的宿主句柄或几何边界是否有效。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isValid();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:228](../../library/desktop_gui/desktop_gui.chl.in#L228)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-56"></a>

### Widget::id（57）

**声明**

```chali
int64 id();
```

**行为**

返回宿主控件身份编号。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回宿主控件身份编号。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.id();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:229](../../library/desktop_gui/desktop_gui.chl.in#L229)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-57"></a>

### Widget::clearCallbacks（58）

**声明**

```chali
void clearCallbacks();
```

**行为**

清空已注册的回调。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.clearCallbacks();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:230](../../library/desktop_gui/desktop_gui.chl.in#L230)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-58"></a>

### Widget::close（59）

**声明**

```chali
void close();
```

**行为**

释放该句柄对应的宿主资源，后续调用须重新检查有效性。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.close();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:235](../../library/desktop_gui/desktop_gui.chl.in#L235)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-59"></a>

### Widget::onEvent（60）

**声明**

```chali
void onEvent(void fn(Event) callback);
```

**行为**

登记一般输入事件回调。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `callback` | `void fn(Event) callback` | 同步执行的事件或绘图回调。 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.onEvent(callback);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:236](../../library/desktop_gui/desktop_gui.chl.in#L236)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-60"></a>

### Widget::onClick（61）

**声明**

```chali
void onClick(void fn(Event) callback);
```

**行为**

登记点击回调。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `callback` | `void fn(Event) callback` | 同步执行的事件或绘图回调。 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.onClick(callback);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:237](../../library/desktop_gui/desktop_gui.chl.in#L237)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-61"></a>

### Widget::onChange（62）

**声明**

```chali
void onChange(void fn(Event) callback);
```

**行为**

登记值变化回调。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `callback` | `void fn(Event) callback` | 同步执行的事件或绘图回调。 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.onChange(callback);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:238](../../library/desktop_gui/desktop_gui.chl.in#L238)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-62"></a>

### Widget::onSubmit（63）

**声明**

```chali
void onSubmit(void fn(Event) callback);
```

**行为**

登记提交回调。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `callback` | `void fn(Event) callback` | 同步执行的事件或绘图回调。 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.onSubmit(callback);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:239](../../library/desktop_gui/desktop_gui.chl.in#L239)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-63"></a>

### Widget::onClose（64）

**声明**

```chali
void onClose(void fn(Event) callback);
```

**行为**

登记控件关闭回调。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `callback` | `void fn(Event) callback` | 同步执行的事件或绘图回调。 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.onClose(callback);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:240](../../library/desktop_gui/desktop_gui.chl.in#L240)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-64"></a>

### Widget::draw（65）

**声明**

```chali
Event draw(Rect bounds);
```

**行为**

在 bounds 中绘制控件并同步派发回调；同一控件每帧绘制一次。

每帧同一模型绘制一次，回调可关闭控件或窗口。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `bounds` | `Rect bounds` | 逻辑像素边界矩形。 | 无 |

**返回值**

`Event`。在 bounds 中绘制控件并同步派发回调；同一控件每帧绘制一次。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.draw(bounds);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:242](../../library/desktop_gui/desktop_gui.chl.in#L242)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-65"></a>

### Widget::setText（66）

**声明**

```chali
bool setText(String text);
```

**行为**

设置控件的主文本。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`bool`。设置控件的主文本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setText(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:260](../../library/desktop_gui/desktop_gui.chl.in#L260)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-66"></a>

### Widget::text（67）

**声明**

```chali
String text();
```

**行为**

读取控件的主文本。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`String`。读取控件的主文本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.text();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:261](../../library/desktop_gui/desktop_gui.chl.in#L261)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-67"></a>

### Widget::setLeftText（68）

**声明**

```chali
bool setLeftText(String text);
```

**行为**

设置控件的左侧文本。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`bool`。设置控件的左侧文本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setLeftText(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:262](../../library/desktop_gui/desktop_gui.chl.in#L262)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-68"></a>

### Widget::setRightText（69）

**声明**

```chali
bool setRightText(String text);
```

**行为**

设置控件的右侧文本。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`bool`。设置控件的右侧文本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setRightText(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:263](../../library/desktop_gui/desktop_gui.chl.in#L263)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-69"></a>

### Widget::setMessage（70）

**声明**

```chali
bool setMessage(String text);
```

**行为**

设置控件的消息正文。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`bool`。设置控件的消息正文。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setMessage(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:264](../../library/desktop_gui/desktop_gui.chl.in#L264)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-70"></a>

### Widget::setButtons（71）

**声明**

```chali
bool setButtons(String text);
```

**行为**

设置控件的按钮文本列表。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`bool`。设置控件的按钮文本列表。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setButtons(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:265](../../library/desktop_gui/desktop_gui.chl.in#L265)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-71"></a>

### Widget::setItems（72）

**声明**

```chali
bool setItems(List<String> items);
```

**行为**

设置独立字符串项列表；每一项允许包含分号与换行。

分号和换行用于上游的多项文字协议；每项本身不可包含分隔符。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `items` | `List<String> items` | 独立文本项列表。 | 无 |

**返回值**

`bool`。设置独立字符串项列表；每一项允许包含分号与换行。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setItems(items);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:267](../../library/desktop_gui/desktop_gui.chl.in#L267)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-72"></a>

### Widget::setTextCapacity（73）

**声明**

```chali
bool setTextCapacity(int64 capacity);
```

**行为**

设置控件的文本缓冲容量。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `capacity` | `int64 capacity` | 文本缓冲容量。 | 无 |

**返回值**

`bool`。设置控件的文本缓冲容量。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setTextCapacity(capacity);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:276](../../library/desktop_gui/desktop_gui.chl.in#L276)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-73"></a>

### Widget::setChecked（74）

**声明**

```chali
bool setChecked(bool value);
```

**行为**

设置控件的勾选状态。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `bool value` | 本项操作使用的值，含义见行为说明。 | 无 |

**返回值**

`bool`。设置控件的勾选状态。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setChecked(value);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:277](../../library/desktop_gui/desktop_gui.chl.in#L277)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-74"></a>

### Widget::checked（75）

**声明**

```chali
bool checked();
```

**行为**

读取控件的勾选状态。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。读取控件的勾选状态。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.checked();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:278](../../library/desktop_gui/desktop_gui.chl.in#L278)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-75"></a>

### Widget::setEditing（76）

**声明**

```chali
bool setEditing(bool value);
```

**行为**

设置控件的编辑状态。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `bool value` | 本项操作使用的值，含义见行为说明。 | 无 |

**返回值**

`bool`。设置控件的编辑状态。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setEditing(value);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:279](../../library/desktop_gui/desktop_gui.chl.in#L279)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-76"></a>

### Widget::editing（77）

**声明**

```chali
bool editing();
```

**行为**

读取控件的编辑状态。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。读取控件的编辑状态。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.editing();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:280](../../library/desktop_gui/desktop_gui.chl.in#L280)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-77"></a>

### Widget::setSecret（78）

**声明**

```chali
bool setSecret(bool value);
```

**行为**

设置控件的密码遮蔽状态。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `bool value` | 本项操作使用的值，含义见行为说明。 | 无 |

**返回值**

`bool`。设置控件的密码遮蔽状态。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setSecret(value);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:281](../../library/desktop_gui/desktop_gui.chl.in#L281)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-78"></a>

### Widget::secret（79）

**声明**

```chali
bool secret();
```

**行为**

读取控件的密码遮蔽状态。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。读取控件的密码遮蔽状态。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.secret();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:282](../../library/desktop_gui/desktop_gui.chl.in#L282)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-79"></a>

### Widget::setVisible（80）

**声明**

```chali
bool setVisible(bool value);
```

**行为**

设置控件的可见状态。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `bool value` | 本项操作使用的值，含义见行为说明。 | 无 |

**返回值**

`bool`。设置控件的可见状态。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setVisible(value);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:283](../../library/desktop_gui/desktop_gui.chl.in#L283)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-80"></a>

### Widget::visible（81）

**声明**

```chali
bool visible();
```

**行为**

读取控件的可见状态。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。读取控件的可见状态。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.visible();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:284](../../library/desktop_gui/desktop_gui.chl.in#L284)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-81"></a>

### Widget::setEnabled（82）

**声明**

```chali
bool setEnabled(bool value);
```

**行为**

设置控件的输入启用状态。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `bool value` | 本项操作使用的值，含义见行为说明。 | 无 |

**返回值**

`bool`。设置控件的输入启用状态。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setEnabled(value);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:285](../../library/desktop_gui/desktop_gui.chl.in#L285)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-82"></a>

### Widget::enabled（83）

**声明**

```chali
bool enabled();
```

**行为**

读取控件的输入启用状态。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。读取控件的输入启用状态。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.enabled();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:286](../../library/desktop_gui/desktop_gui.chl.in#L286)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-83"></a>

### Widget::setSelected（84）

**声明**

```chali
bool setSelected(int64 value);
```

**行为**

设置控件的所选项下标。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `int64 value` | 本项操作使用的值，含义见行为说明。 | 无 |

**返回值**

`bool`。设置控件的所选项下标。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setSelected(value);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:287](../../library/desktop_gui/desktop_gui.chl.in#L287)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-84"></a>

### Widget::selected（85）

**声明**

```chali
int64 selected();
```

**行为**

读取控件的所选项下标。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。读取控件的所选项下标。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.selected();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:288](../../library/desktop_gui/desktop_gui.chl.in#L288)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-85"></a>

### Widget::setNumber（86）

**声明**

```chali
bool setNumber(int64 value);
```

**行为**

设置控件的整数值。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `int64 value` | 本项操作使用的值，含义见行为说明。 | 无 |

**返回值**

`bool`。设置控件的整数值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setNumber(value);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:289](../../library/desktop_gui/desktop_gui.chl.in#L289)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-86"></a>

### Widget::number（87）

**声明**

```chali
int64 number();
```

**行为**

读取控件的整数值。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。读取控件的整数值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.number();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:290](../../library/desktop_gui/desktop_gui.chl.in#L290)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-87"></a>

### Widget::result（88）

**声明**

```chali
int64 result();
```

**行为**

返回控件最近一次交互结果码。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回控件最近一次交互结果码。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.result();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:291](../../library/desktop_gui/desktop_gui.chl.in#L291)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-88"></a>

### Widget::setScrollIndex（89）

**声明**

```chali
bool setScrollIndex(int64 value);
```

**行为**

设置控件的滚动项下标。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `int64 value` | 本项操作使用的值，含义见行为说明。 | 无 |

**返回值**

`bool`。设置控件的滚动项下标。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setScrollIndex(value);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:292](../../library/desktop_gui/desktop_gui.chl.in#L292)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-89"></a>

### Widget::scrollIndex（90）

**声明**

```chali
int64 scrollIndex();
```

**行为**

读取控件的滚动项下标。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。读取控件的滚动项下标。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.scrollIndex();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:293](../../library/desktop_gui/desktop_gui.chl.in#L293)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-90"></a>

### Widget::focusIndex（91）

**声明**

```chali
int64 focusIndex();
```

**行为**

返回当前获得焦点的列表项下标。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回当前获得焦点的列表项下标。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.focusIndex();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:294](../../library/desktop_gui/desktop_gui.chl.in#L294)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-91"></a>

### Widget::setRange（92）

**声明**

```chali
bool setRange(float64 minimum, float64 maximum);
```

**行为**

设置控件的最小值和最大值。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `minimum` | `float64 minimum` | 允许的最小值。 | 无 |
| `maximum` | `float64 maximum` | 最大值或一次派发的事件上限，依本项行为解释。 | 无 |

**返回值**

`bool`。设置控件的最小值和最大值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setRange(minimum, maximum);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:295](../../library/desktop_gui/desktop_gui.chl.in#L295)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-92"></a>

### Widget::setValue（93）

**声明**

```chali
bool setValue(float64 value);
```

**行为**

设置控件的浮点值。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `float64 value` | 本项操作使用的值，含义见行为说明。 | 无 |

**返回值**

`bool`。设置控件的浮点值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setValue(value);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:296](../../library/desktop_gui/desktop_gui.chl.in#L296)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-93"></a>

### Widget::value（94）

**声明**

```chali
float64 value();
```

**行为**

读取控件的浮点值。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`float64`。读取控件的浮点值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.value();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:297](../../library/desktop_gui/desktop_gui.chl.in#L297)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-94"></a>

### Widget::setColor（95）

**声明**

```chali
bool setColor(uint32 rgba);
```

**行为**

设置控件的RRGGBBAA 颜色值。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `rgba` | `uint32 rgba` | RRGGBBAA 颜色编码。 | 无 |

**返回值**

`bool`。设置控件的RRGGBBAA 颜色值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setColor(rgba);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:298](../../library/desktop_gui/desktop_gui.chl.in#L298)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-95"></a>

### Widget::color（96）

**声明**

```chali
uint32 color();
```

**行为**

读取控件的RRGGBBAA 颜色值。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`uint32`。读取控件的RRGGBBAA 颜色值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.color();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:299](../../library/desktop_gui/desktop_gui.chl.in#L299)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-96"></a>

### Widget::setHsv（97）

**声明**

```chali
bool setHsv(float64 hue, float64 saturation, float64 brightness);
```

**行为**

设置控件的HSV 颜色分量。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `hue` | `float64 hue` | HSV 色相。 | 无 |
| `saturation` | `float64 saturation` | HSV 饱和度。 | 无 |
| `brightness` | `float64 brightness` | HSV 明度。 | 无 |

**返回值**

`bool`。设置控件的HSV 颜色分量。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setHsv(hue, saturation, brightness);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:300](../../library/desktop_gui/desktop_gui.chl.in#L300)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-97"></a>

### Widget::hue（98）

**声明**

```chali
float64 hue();
```

**行为**

返回 HSV 色相。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`float64`。返回 HSV 色相。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.hue();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:304](../../library/desktop_gui/desktop_gui.chl.in#L304)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-98"></a>

### Widget::saturation（99）

**声明**

```chali
float64 saturation();
```

**行为**

返回 HSV 饱和度。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`float64`。返回 HSV 饱和度。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.saturation();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:305](../../library/desktop_gui/desktop_gui.chl.in#L305)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-99"></a>

### Widget::brightness（100）

**声明**

```chali
float64 brightness();
```

**行为**

返回 HSV 明度。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`float64`。返回 HSV 明度。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.brightness();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:306](../../library/desktop_gui/desktop_gui.chl.in#L306)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-100"></a>

### Widget::setSpacing（101）

**声明**

```chali
bool setSpacing(float64 spacing, int64 subdivisions);
```

**行为**

设置控件的网格间距及细分数。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `spacing` | `float64 spacing` | 字距或网格间距，依本项行为解释。 | 无 |
| `subdivisions` | `int64 subdivisions` | 网格细分数。 | 无 |

**返回值**

`bool`。设置控件的网格间距及细分数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setSpacing(spacing, subdivisions);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:307](../../library/desktop_gui/desktop_gui.chl.in#L307)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-101"></a>

### Widget::setContent（102）

**声明**

```chali
bool setContent(Rect bounds);
```

**行为**

设置控件的可滚动内容矩形。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `bounds` | `Rect bounds` | 逻辑像素边界矩形。 | 无 |

**返回值**

`bool`。设置控件的可滚动内容矩形。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setContent(bounds);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:310](../../library/desktop_gui/desktop_gui.chl.in#L310)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-102"></a>

### Widget::viewport（103）

**声明**

```chali
Rect viewport();
```

**行为**

返回滚动面板可视矩形。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Rect`。返回滚动面板可视矩形。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.viewport();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:314](../../library/desktop_gui/desktop_gui.chl.in#L314)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-103"></a>

### Widget::scrollOffset（104）

**声明**

```chali
Point scrollOffset();
```

**行为**

返回滚动面板的二维偏移。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Point`。返回滚动面板的二维偏移。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.scrollOffset();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:315](../../library/desktop_gui/desktop_gui.chl.in#L315)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-104"></a>

### Widget::setScrollOffset（105）

**声明**

```chali
bool setScrollOffset(float64 x, float64 y);
```

**行为**

设置控件的二维滚动偏移。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `x` | `float64 x` | 逻辑像素横坐标。 | 无 |
| `y` | `float64 y` | 逻辑像素纵坐标。 | 无 |

**返回值**

`bool`。设置控件的二维滚动偏移。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setScrollOffset(x, y);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:316](../../library/desktop_gui/desktop_gui.chl.in#L316)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-105"></a>

### Widget::beginContent（106）

**声明**

```chali
bool beginContent();
```

**行为**

开始绘制滚动面板内容，调用后需与 endClip 配对。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。开始绘制滚动面板内容，调用后需与 endClip 配对。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.beginContent();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:320](../../library/desktop_gui/desktop_gui.chl.in#L320)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-106"></a>

### createControl（107）

**声明**

```chali
Widget createControl(int64 kind, String text);
```

**行为**

按控件种类创建窗口拥有的 Widget；失败返回无效句柄。

创建指定种类；通常使用下方具名工厂，失败可用 isValid 检查。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `kind` | `int64 kind` | 控件种类编号。 | 无 |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。按控件种类创建窗口拥有的 Widget；失败返回无效句柄。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::createControl(kind, text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:324](../../library/desktop_gui/desktop_gui.chl.in#L324)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-107"></a>

### controlCount（108）

**声明**

```chali
int64 controlCount();
```

**行为**

返回生成清单支持的控件种类数量。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回生成清单支持的控件种类数量。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::controlCount();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:329](../../library/desktop_gui/desktop_gui.chl.in#L329)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-108"></a>

### windowBox（109）

**声明**

```chali
Widget windowBox(String text);
```

**行为**

创建 windowBox 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiWindowBox 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 windowBox 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::windowBox(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:331](../../library/desktop_gui/desktop_gui.chl.in#L331)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-109"></a>

### groupBox（110）

**声明**

```chali
Widget groupBox(String text);
```

**行为**

创建 groupBox 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiGroupBox 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 groupBox 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::groupBox(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:333](../../library/desktop_gui/desktop_gui.chl.in#L333)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-110"></a>

### line（111）

**声明**

```chali
Widget line(String text);
```

**行为**

创建 line 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiLine 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 line 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::line(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:335](../../library/desktop_gui/desktop_gui.chl.in#L335)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-111"></a>

### panel（112）

**声明**

```chali
Widget panel(String text);
```

**行为**

创建 panel 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiPanel 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 panel 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::panel(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:337](../../library/desktop_gui/desktop_gui.chl.in#L337)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-112"></a>

### tabBar（113）

**声明**

```chali
Widget tabBar(String text);
```

**行为**

创建 tabBar 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiTabBar 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 tabBar 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::tabBar(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:339](../../library/desktop_gui/desktop_gui.chl.in#L339)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-113"></a>

### scrollPanel（114）

**声明**

```chali
Widget scrollPanel(String text);
```

**行为**

创建 scrollPanel 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiScrollPanel 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 scrollPanel 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::scrollPanel(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:341](../../library/desktop_gui/desktop_gui.chl.in#L341)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-114"></a>

### label（115）

**声明**

```chali
Widget label(String text);
```

**行为**

创建 label 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiLabel 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 label 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::label(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:343](../../library/desktop_gui/desktop_gui.chl.in#L343)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-115"></a>

### button（116）

**声明**

```chali
Widget button(String text);
```

**行为**

创建 button 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiButton 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 button 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::button(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:345](../../library/desktop_gui/desktop_gui.chl.in#L345)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-116"></a>

### labelButton（117）

**声明**

```chali
Widget labelButton(String text);
```

**行为**

创建 labelButton 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiLabelButton 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 labelButton 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::labelButton(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:347](../../library/desktop_gui/desktop_gui.chl.in#L347)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-117"></a>

### toggle（118）

**声明**

```chali
Widget toggle(String text);
```

**行为**

创建 toggle 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiToggle 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 toggle 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::toggle(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:349](../../library/desktop_gui/desktop_gui.chl.in#L349)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-118"></a>

### toggleGroup（119）

**声明**

```chali
Widget toggleGroup(String text);
```

**行为**

创建 toggleGroup 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiToggleGroup 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 toggleGroup 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::toggleGroup(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:351](../../library/desktop_gui/desktop_gui.chl.in#L351)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-119"></a>

### toggleSlider（120）

**声明**

```chali
Widget toggleSlider(String text);
```

**行为**

创建 toggleSlider 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiToggleSlider 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 toggleSlider 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::toggleSlider(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:353](../../library/desktop_gui/desktop_gui.chl.in#L353)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-120"></a>

### checkBox（121）

**声明**

```chali
Widget checkBox(String text);
```

**行为**

创建 checkBox 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiCheckBox 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 checkBox 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::checkBox(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:355](../../library/desktop_gui/desktop_gui.chl.in#L355)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-121"></a>

### comboBox（122）

**声明**

```chali
Widget comboBox(String text);
```

**行为**

创建 comboBox 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiComboBox 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 comboBox 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::comboBox(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:357](../../library/desktop_gui/desktop_gui.chl.in#L357)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-122"></a>

### dropdownBox（123）

**声明**

```chali
Widget dropdownBox(String text);
```

**行为**

创建 dropdownBox 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiDropdownBox 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 dropdownBox 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::dropdownBox(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:359](../../library/desktop_gui/desktop_gui.chl.in#L359)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-123"></a>

### spinner（124）

**声明**

```chali
Widget spinner(String text);
```

**行为**

创建 spinner 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiSpinner 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 spinner 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::spinner(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:361](../../library/desktop_gui/desktop_gui.chl.in#L361)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-124"></a>

### valueBox（125）

**声明**

```chali
Widget valueBox(String text);
```

**行为**

创建 valueBox 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiValueBox 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 valueBox 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::valueBox(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:363](../../library/desktop_gui/desktop_gui.chl.in#L363)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-125"></a>

### textBox（126）

**声明**

```chali
Widget textBox(String text);
```

**行为**

创建 textBox 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiTextBox 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 textBox 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::textBox(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:365](../../library/desktop_gui/desktop_gui.chl.in#L365)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-126"></a>

### slider（127）

**声明**

```chali
Widget slider(String text);
```

**行为**

创建 slider 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiSlider 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 slider 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::slider(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:367](../../library/desktop_gui/desktop_gui.chl.in#L367)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-127"></a>

### sliderBar（128）

**声明**

```chali
Widget sliderBar(String text);
```

**行为**

创建 sliderBar 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiSliderBar 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 sliderBar 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::sliderBar(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:369](../../library/desktop_gui/desktop_gui.chl.in#L369)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-128"></a>

### progressBar（129）

**声明**

```chali
Widget progressBar(String text);
```

**行为**

创建 progressBar 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiProgressBar 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 progressBar 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::progressBar(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:371](../../library/desktop_gui/desktop_gui.chl.in#L371)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-129"></a>

### statusBar（130）

**声明**

```chali
Widget statusBar(String text);
```

**行为**

创建 statusBar 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiStatusBar 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 statusBar 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::statusBar(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:373](../../library/desktop_gui/desktop_gui.chl.in#L373)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-130"></a>

### dummyRec（131）

**声明**

```chali
Widget dummyRec(String text);
```

**行为**

创建 dummyRec 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiDummyRec 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 dummyRec 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::dummyRec(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:375](../../library/desktop_gui/desktop_gui.chl.in#L375)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-131"></a>

### grid（132）

**声明**

```chali
Widget grid(String text);
```

**行为**

创建 grid 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiGrid 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 grid 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::grid(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:377](../../library/desktop_gui/desktop_gui.chl.in#L377)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-132"></a>

### listView（133）

**声明**

```chali
Widget listView(String text);
```

**行为**

创建 listView 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiListView 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 listView 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::listView(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:379](../../library/desktop_gui/desktop_gui.chl.in#L379)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-133"></a>

### listViewEx（134）

**声明**

```chali
Widget listViewEx(String text);
```

**行为**

创建 listViewEx 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiListViewEx 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 listViewEx 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::listViewEx(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:381](../../library/desktop_gui/desktop_gui.chl.in#L381)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-134"></a>

### messageBox（135）

**声明**

```chali
Widget messageBox(String text);
```

**行为**

创建 messageBox 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiMessageBox 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 messageBox 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::messageBox(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:383](../../library/desktop_gui/desktop_gui.chl.in#L383)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-135"></a>

### textInputBox（136）

**声明**

```chali
Widget textInputBox(String text);
```

**行为**

创建 textInputBox 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiTextInputBox 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 textInputBox 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::textInputBox(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:385](../../library/desktop_gui/desktop_gui.chl.in#L385)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-136"></a>

### colorPicker（137）

**声明**

```chali
Widget colorPicker(String text);
```

**行为**

创建 colorPicker 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiColorPicker 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 colorPicker 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::colorPicker(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:387](../../library/desktop_gui/desktop_gui.chl.in#L387)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-137"></a>

### colorPanel（138）

**声明**

```chali
Widget colorPanel(String text);
```

**行为**

创建 colorPanel 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiColorPanel 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 colorPanel 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::colorPanel(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:389](../../library/desktop_gui/desktop_gui.chl.in#L389)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-138"></a>

### colorBarAlpha（139）

**声明**

```chali
Widget colorBarAlpha(String text);
```

**行为**

创建 colorBarAlpha 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiColorBarAlpha 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 colorBarAlpha 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::colorBarAlpha(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:391](../../library/desktop_gui/desktop_gui.chl.in#L391)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-139"></a>

### colorBarHue（140）

**声明**

```chali
Widget colorBarHue(String text);
```

**行为**

创建 colorBarHue 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiColorBarHue 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 colorBarHue 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::colorBarHue(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:393](../../library/desktop_gui/desktop_gui.chl.in#L393)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-140"></a>

### colorPickerHsv（141）

**声明**

```chali
Widget colorPickerHsv(String text);
```

**行为**

创建 colorPickerHsv 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiColorPickerHSV 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 colorPickerHsv 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::colorPickerHsv(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:395](../../library/desktop_gui/desktop_gui.chl.in#L395)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-141"></a>

### colorPanelHsv（142）

**声明**

```chali
Widget colorPanelHsv(String text);
```

**行为**

创建 colorPanelHsv 控件状态；由窗口拥有，draw 时同步派发变化。

创建 GuiColorPanelHSV 对应的控件状态，绘制时同步派发变化。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`Widget`。创建 colorPanelHsv 控件状态；由窗口拥有，draw 时同步派发变化。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::colorPanelHsv(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:397](../../library/desktop_gui/desktop_gui.chl.in#L397)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-142"></a>

### controlName（143）

**声明**

```chali
String controlName(int64 index);
```

**行为**

返回清单下标对应的控件工厂名称；越界返回空串。

查询清单中的控件名称，非法下标返回空串。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `index` | `int64 index` | 从零开始的下标。 | 无 |

**返回值**

`String`。返回清单下标对应的控件工厂名称；越界返回空串。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::controlName(index);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:399](../../library/desktop_gui/desktop_gui.chl.in#L399)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-143"></a>

### frameActive（144）

**声明**

```chali
bool frameActive();
```

**行为**

查询当前是否位于 beginFrame/endFrame 之间。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。查询当前是否位于 beginFrame/endFrame 之间。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::frameActive();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:438](../../library/desktop_gui/desktop_gui.chl.in#L438)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-144"></a>

### inputEnabled（145）

**声明**

```chali
bool inputEnabled();
```

**行为**

查询当前控件输入是否启用。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。查询当前控件输入是否启用。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::inputEnabled();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:440](../../library/desktop_gui/desktop_gui.chl.in#L440)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-145"></a>

### frameCount（146）

**声明**

```chali
uint64 frameCount();
```

**行为**

返回当前窗口已开始的帧数。

当前绘制帧；只在创建窗口的线程上读取。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`uint64`。返回当前窗口已开始的帧数。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::frameCount();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:444](../../library/desktop_gui/desktop_gui.chl.in#L444)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-146"></a>

### clipBounds（147）

**声明**

```chali
Rect clipBounds();
```

**行为**

返回当前逻辑像素裁剪矩形。

当前逻辑像素裁剪区，可用于自定义控件的命中测试。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Rect`。返回当前逻辑像素裁剪矩形。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::clipBounds();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:451](../../library/desktop_gui/desktop_gui.chl.in#L451)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-147"></a>

### drawPixel（148）

**声明**

```chali
bool drawPixel(Point point, uint32 color);
```

**行为**

绘制指定位置的像素。

绘图仅在 beginFrame/endFrame 内执行；颜色为 RRGGBBAA。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `point` | `Point point` | 像素位置。 | 无 |
| `color` | `uint32 color` | RRGGBBAA 颜色编码。 | 无 |

**返回值**

`bool`。绘制指定位置的像素。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::drawPixel(point, color);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:477](../../library/desktop_gui/desktop_gui.chl.in#L477)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-148"></a>

### drawLine（149）

**声明**

```chali
bool drawLine(Point start, Point end, float64 thickness, uint32 color);
```

**行为**

按指定线宽绘制线段。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `start` | `Point start` | 起点。 | 无 |
| `end` | `Point end` | 终点。 | 无 |
| `thickness` | `float64 thickness` | 线宽；支持填充的图形使用零表示填充。 | 无 |
| `color` | `uint32 color` | RRGGBBAA 颜色编码。 | 无 |

**返回值**

`bool`。按指定线宽绘制线段。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::drawLine(start, end, thickness, color);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:478](../../library/desktop_gui/desktop_gui.chl.in#L478)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-149"></a>

### drawRectangle（150）

**声明**

```chali
bool drawRectangle(Rect bounds, uint32 color, float64 thickness = 0, float64 roundness = 0);
```

**行为**

绘制矩形；thickness 为零时填充，否则画边框，roundness 为 0–1。

thickness 为零填充矩形；大于零绘制边框，roundness 为 0..1。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `bounds` | `Rect bounds` | 逻辑像素边界矩形。 | 无 |
| `color` | `uint32 color` | RRGGBBAA 颜色编码。 | 无 |
| `thickness` | `float64 thickness` | 线宽；支持填充的图形使用零表示填充。 | `0` |
| `roundness` | `float64 roundness` | 圆角比例，0–1。 | `0` |

**返回值**

`bool`。绘制矩形；thickness 为零时填充，否则画边框，roundness 为 0–1。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::drawRectangle(bounds, color, thickness, roundness);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:480](../../library/desktop_gui/desktop_gui.chl.in#L480)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-150"></a>

### drawCircle（151）

**声明**

```chali
bool drawCircle(Point center, float64 radius, uint32 color, float64 thickness = 0);
```

**行为**

绘制圆；thickness 为零时填充，否则画边框。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `center` | `Point center` | 图形中心。 | 无 |
| `radius` | `float64 radius` | 圆半径或椭圆二维半径。 | 无 |
| `color` | `uint32 color` | RRGGBBAA 颜色编码。 | 无 |
| `thickness` | `float64 thickness` | 线宽；支持填充的图形使用零表示填充。 | `0` |

**返回值**

`bool`。绘制圆；thickness 为零时填充，否则画边框。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::drawCircle(center, radius, color, thickness);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:481](../../library/desktop_gui/desktop_gui.chl.in#L481)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-151"></a>

### drawEllipse（152）

**声明**

```chali
bool drawEllipse(Point center, Point radius, uint32 color, bool outline = false);
```

**行为**

绘制椭圆，outline 决定只画轮廓。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `center` | `Point center` | 图形中心。 | 无 |
| `radius` | `Point radius` | 圆半径或椭圆二维半径。 | 无 |
| `color` | `uint32 color` | RRGGBBAA 颜色编码。 | 无 |
| `outline` | `bool outline` | 是否只绘制轮廓。 | `false` |

**返回值**

`bool`。绘制椭圆，outline 决定只画轮廓。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::drawEllipse(center, radius, color, outline);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:482](../../library/desktop_gui/desktop_gui.chl.in#L482)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-152"></a>

### drawTriangle（153）

**声明**

```chali
bool drawTriangle(Point a, Point b, Point c, uint32 color, bool outline = false);
```

**行为**

绘制三个顶点确定的三角形，outline 决定只画轮廓。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `a` | `Point a` | 第一个顶点。 | 无 |
| `b` | `Point b` | 第二个顶点。 | 无 |
| `c` | `Point c` | 第三个顶点。 | 无 |
| `color` | `uint32 color` | RRGGBBAA 颜色编码。 | 无 |
| `outline` | `bool outline` | 是否只绘制轮廓。 | `false` |

**返回值**

`bool`。绘制三个顶点确定的三角形，outline 决定只画轮廓。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::drawTriangle(a, b, c, color, outline);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:483](../../library/desktop_gui/desktop_gui.chl.in#L483)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-153"></a>

### drawPolygon（154）

**声明**

```chali
bool drawPolygon(Point center, int64 sides, float64 radius, float64 rotation, uint32 color, float64 thickness = 0);
```

**行为**

绘制指定边数、半径和旋转角的正多边形。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `center` | `Point center` | 图形中心。 | 无 |
| `sides` | `int64 sides` | 正多边形边数。 | 无 |
| `radius` | `float64 radius` | 圆半径或椭圆二维半径。 | 无 |
| `rotation` | `float64 rotation` | 旋转角度，单位为度。 | 无 |
| `color` | `uint32 color` | RRGGBBAA 颜色编码。 | 无 |
| `thickness` | `float64 thickness` | 线宽；支持填充的图形使用零表示填充。 | `0` |

**返回值**

`bool`。绘制指定边数、半径和旋转角的正多边形。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::drawPolygon(center, sides, radius, rotation, color, thickness);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:484](../../library/desktop_gui/desktop_gui.chl.in#L484)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-154"></a>

### drawBezier（155）

**声明**

```chali
bool drawBezier(Point start, Point first_control, Point second_control, Point end, float64 thickness, uint32 color);
```

**行为**

用起点、终点和两个控制点绘制三次贝塞尔曲线。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `start` | `Point start` | 起点。 | 无 |
| `first_control` | `Point first_control` | 第一控制点。 | 无 |
| `second_control` | `Point second_control` | 第二控制点。 | 无 |
| `end` | `Point end` | 终点。 | 无 |
| `thickness` | `float64 thickness` | 线宽；支持填充的图形使用零表示填充。 | 无 |
| `color` | `uint32 color` | RRGGBBAA 颜色编码。 | 无 |

**返回值**

`bool`。用起点、终点和两个控制点绘制三次贝塞尔曲线。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::drawBezier(start, first_control, second_control, end, thickness, color);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:485](../../library/desktop_gui/desktop_gui.chl.in#L485)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-155"></a>

### drawRing（156）

**声明**

```chali
bool drawRing(Point center, float64 inner_radius, float64 outer_radius, float64 start_angle, float64 end_angle, uint32 color);
```

**行为**

按内外半径和起止角度绘制环扇形，角度使用度，内半径为零时为扇形。

以角度制绘制环带；inner_radius 为零时绘制扇形。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `center` | `Point center` | 图形中心。 | 无 |
| `inner_radius` | `float64 inner_radius` | 内半径。 | 无 |
| `outer_radius` | `float64 outer_radius` | 外半径。 | 无 |
| `start_angle` | `float64 start_angle` | 起始角度，单位为度。 | 无 |
| `end_angle` | `float64 end_angle` | 终止角度，单位为度。 | 无 |
| `color` | `uint32 color` | RRGGBBAA 颜色编码。 | 无 |

**返回值**

`bool`。按内外半径和起止角度绘制环扇形，角度使用度，内半径为零时为扇形。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::drawRing(center, inner_radius, outer_radius, start_angle, end_angle, color);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:487](../../library/desktop_gui/desktop_gui.chl.in#L487)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-156"></a>

### drawText（157）

**声明**

```chali
bool drawText(String text, Point position, float64 size, uint32 color, float64 spacing = 1);
```

**行为**

按指定位置、字号、字距和颜色绘制文本。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |
| `position` | `Point position` | 文本绘制位置。 | 无 |
| `size` | `float64 size` | 字体大小。 | 无 |
| `color` | `uint32 color` | RRGGBBAA 颜色编码。 | 无 |
| `spacing` | `float64 spacing` | 字距或网格间距，依本项行为解释。 | `1` |

**返回值**

`bool`。按指定位置、字号、字距和颜色绘制文本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::drawText(text, position, size, color, spacing);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:488](../../library/desktop_gui/desktop_gui.chl.in#L488)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-157"></a>

### measureText（158）

**声明**

```chali
Point measureText(String text, float64 size, float64 spacing = 1);
```

**行为**

返回指定字号及字距下文本的二维尺寸。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |
| `size` | `float64 size` | 字体大小。 | 无 |
| `spacing` | `float64 spacing` | 字距或网格间距，依本项行为解释。 | `1` |

**返回值**

`Point`。返回指定字号及字距下文本的二维尺寸。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::measureText(text, size, spacing);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:489](../../library/desktop_gui/desktop_gui.chl.in#L489)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-158"></a>

### Texture::isValid（159）

**声明**

```chali
bool isValid();
```

**行为**

检查纹理句柄是否仍有效。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。检查纹理句柄是否仍有效。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isValid();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:505](../../library/desktop_gui/desktop_gui.chl.in#L505)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-159"></a>

### Texture::close（160）

**声明**

```chali
void close();
```

**行为**

卸载该纹理。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.close();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:506](../../library/desktop_gui/desktop_gui.chl.in#L506)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-160"></a>

### Texture::size（161）

**声明**

```chali
Point size();
```

**行为**

返回纹理的像素宽高。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Point`。返回纹理的像素宽高。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.size();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:507](../../library/desktop_gui/desktop_gui.chl.in#L507)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-161"></a>

### Texture::draw（162）

**声明**

```chali
bool draw(Rect destination, uint32 tint = (uint32)4294967295);
```

**行为**

将完整纹理绘制到目标矩形并乘以 tint 颜色。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `destination` | `Rect destination` | 目标矩形。 | 无 |
| `tint` | `uint32 tint` | 乘到纹理上的 RRGGBBAA 颜色，默认全白不改变颜色。 | `(uint32)4294967295` |

**返回值**

`bool`。将完整纹理绘制到目标矩形并乘以 tint 颜色。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.draw(destination, tint);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:508](../../library/desktop_gui/desktop_gui.chl.in#L508)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-162"></a>

### Texture::drawRegion（163）

**声明**

```chali
bool drawRegion(Rect source, Rect destination, Point origin, float64 rotation, uint32 tint = (uint32)4294967295);
```

**行为**

按源矩形、目标矩形、旋转原点、角度和 tint 绘制纹理区域。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `source` | `Rect source` | 纹理源矩形。 | 无 |
| `destination` | `Rect destination` | 目标矩形。 | 无 |
| `origin` | `Point origin` | 旋转原点。 | 无 |
| `rotation` | `float64 rotation` | 旋转角度，单位为度。 | 无 |
| `tint` | `uint32 tint` | 乘到纹理上的 RRGGBBAA 颜色，默认全白不改变颜色。 | `(uint32)4294967295` |

**返回值**

`bool`。按源矩形、目标矩形、旋转原点、角度和 tint 绘制纹理区域。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.drawRegion(source, destination, origin, rotation, tint);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:513](../../library/desktop_gui/desktop_gui.chl.in#L513)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-163"></a>

### loadTexture（164）

**声明**

```chali
Texture loadTexture(String path);
```

**行为**

加载窗口拥有的 GPU 纹理；失败返回无效 Texture。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `path` | `String path` | 输入文件路径。 | 无 |

**返回值**

`Texture`。加载窗口拥有的 GPU 纹理；失败返回无效 Texture。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::loadTexture(path);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:517](../../library/desktop_gui/desktop_gui.chl.in#L517)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-164"></a>

### customEvent（165）

**声明**

```chali
Event customEvent(String name, String text = "", int64 code = 0, float64 value = 0);
```

**行为**

创建独立拥有名称、文本、整数码和浮点值的自定义事件。

创建带名称与可独立拥有负载的用户事件。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `name` | `String name` | 属性或事件的精确名称。 | 无 |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | `""` |
| `code` | `int64 code` | 自定义整数事件码。 | `0` |
| `value` | `float64 value` | 本项操作使用的值，含义见行为说明。 | `0` |

**返回值**

`Event`。创建独立拥有名称、文本、整数码和浮点值的自定义事件。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::customEvent(name, text, code, value);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:520](../../library/desktop_gui/desktop_gui.chl.in#L520)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-165"></a>

### EventBus::on（166）

**声明**

```chali
int64 on(String name, void fn(Event) callback);
```

**行为**

按事件名登记监听器并返回订阅编号。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `name` | `String name` | 属性或事件的精确名称。 | 无 |
| `callback` | `void fn(Event) callback` | 同步执行的事件或绘图回调。 | 无 |

**返回值**

`int64`。按事件名登记监听器并返回订阅编号。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.on(name, callback);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:546](../../library/desktop_gui/desktop_gui.chl.in#L546)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-166"></a>

### EventBus::once（167）

**声明**

```chali
int64 once(String name, void fn(Event) callback);
```

**行为**

按事件名登记执行一次后移除的监听器。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `name` | `String name` | 属性或事件的精确名称。 | 无 |
| `callback` | `void fn(Event) callback` | 同步执行的事件或绘图回调。 | 无 |

**返回值**

`int64`。按事件名登记执行一次后移除的监听器。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.once(name, callback);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:547](../../library/desktop_gui/desktop_gui.chl.in#L547)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-167"></a>

### EventBus::off（168）

**声明**

```chali
bool off(int64 id);
```

**行为**

按订阅编号移除监听器，返回是否找到。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `id` | `int64 id` | 声音或订阅句柄，依本项行为解释。 | 无 |

**返回值**

`bool`。按订阅编号移除监听器，返回是否找到。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.off(id);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:548](../../library/desktop_gui/desktop_gui.chl.in#L548)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-168"></a>

### EventBus::clear（169）

**声明**

```chali
void clear();
```

**行为**

清除监听器和待派发队列。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.clear();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:557](../../library/desktop_gui/desktop_gui.chl.in#L557)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-169"></a>

### EventBus::emit（170）

**声明**

```chali
int64 emit(Event event);
```

**行为**

同步派发事件；EventBus 返回执行的监听器数量，自绘控件返回是否处理。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `event` | `Event event` | 独立事件值。 | 无 |

**返回值**

`int64`。同步派发事件；EventBus 返回执行的监听器数量，自绘控件返回是否处理。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.emit(event);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:562](../../library/desktop_gui/desktop_gui.chl.in#L562)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-170"></a>

### EventBus::post（171）

**声明**

```chali
bool post(Event event);
```

**行为**

将独立事件值加入延迟派发队列。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `event` | `Event event` | 独立事件值。 | 无 |

**返回值**

`bool`。将独立事件值加入延迟派发队列。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.post(event);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:575](../../library/desktop_gui/desktop_gui.chl.in#L575)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-171"></a>

### EventBus::pending（172）

**声明**

```chali
int64 pending();
```

**行为**

返回队列中尚未消费的事件数量。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回队列中尚未消费的事件数量。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.pending();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:576](../../library/desktop_gui/desktop_gui.chl.in#L576)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-172"></a>

### EventBus::pump（173）

**声明**

```chali
int64 pump(int64 maximum = 256);
```

**行为**

最多派发 maximum 项排队事件并返回实际派发数，默认上限 256。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `maximum` | `int64 maximum` | 最大值或一次派发的事件上限，依本项行为解释。 | `256` |

**返回值**

`int64`。最多派发 maximum 项排队事件并返回实际派发数，默认上限 256。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.pump(maximum);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:577](../../library/desktop_gui/desktop_gui.chl.in#L577)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-173"></a>

### CustomWidget::isValid（174）

**声明**

```chali
bool isValid();
```

**行为**

检查当前值的宿主句柄或几何边界是否有效。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。检查当前值的宿主句柄或几何边界是否有效。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.isValid();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:614](../../library/desktop_gui/desktop_gui.chl.in#L614)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-174"></a>

### CustomWidget::id（175）

**声明**

```chali
int64 id();
```

**行为**

返回宿主控件身份编号。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。返回宿主控件身份编号。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.id();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:615](../../library/desktop_gui/desktop_gui.chl.in#L615)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-175"></a>

### CustomWidget::onDraw（176）

**声明**

```chali
void onDraw(void fn(Rect) callback);
```

**行为**

登记自绘回调，参数为本次绘制边界。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `callback` | `void fn(Rect) callback` | 同步执行的事件或绘图回调。 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.onDraw(callback);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:616](../../library/desktop_gui/desktop_gui.chl.in#L616)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-176"></a>

### CustomWidget::onEvent（177）

**声明**

```chali
void onEvent(void fn(Event) callback);
```

**行为**

登记一般输入事件回调。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `callback` | `void fn(Event) callback` | 同步执行的事件或绘图回调。 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.onEvent(callback);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:617](../../library/desktop_gui/desktop_gui.chl.in#L617)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-177"></a>

### CustomWidget::on（178）

**声明**

```chali
int64 on(String name, void fn(Event) callback);
```

**行为**

按事件名登记监听器并返回订阅编号。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `name` | `String name` | 属性或事件的精确名称。 | 无 |
| `callback` | `void fn(Event) callback` | 同步执行的事件或绘图回调。 | 无 |

**返回值**

`int64`。按事件名登记监听器并返回订阅编号。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.on(name, callback);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:618](../../library/desktop_gui/desktop_gui.chl.in#L618)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-178"></a>

### CustomWidget::once（179）

**声明**

```chali
int64 once(String name, void fn(Event) callback);
```

**行为**

按事件名登记执行一次后移除的监听器。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `name` | `String name` | 属性或事件的精确名称。 | 无 |
| `callback` | `void fn(Event) callback` | 同步执行的事件或绘图回调。 | 无 |

**返回值**

`int64`。按事件名登记执行一次后移除的监听器。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.once(name, callback);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:619](../../library/desktop_gui/desktop_gui.chl.in#L619)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-179"></a>

### CustomWidget::off（180）

**声明**

```chali
bool off(int64 id);
```

**行为**

按订阅编号移除监听器，返回是否找到。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `id` | `int64 id` | 声音或订阅句柄，依本项行为解释。 | 无 |

**返回值**

`bool`。按订阅编号移除监听器，返回是否找到。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.off(id);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:620](../../library/desktop_gui/desktop_gui.chl.in#L620)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-180"></a>

### CustomWidget::clearCallbacks（181）

**声明**

```chali
void clearCallbacks();
```

**行为**

清空已注册的回调。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.clearCallbacks();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:621](../../library/desktop_gui/desktop_gui.chl.in#L621)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-181"></a>

### CustomWidget::close（182）

**声明**

```chali
void close();
```

**行为**

释放该句柄对应的宿主资源，后续调用须重新检查有效性。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.close();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:622](../../library/desktop_gui/desktop_gui.chl.in#L622)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-182"></a>

### CustomWidget::focus（183）

**声明**

```chali
bool focus();
```

**行为**

请求自绘控件获得焦点。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。请求自绘控件获得焦点。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.focus();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:639](../../library/desktop_gui/desktop_gui.chl.in#L639)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-183"></a>

### CustomWidget::blur（184）

**声明**

```chali
void blur();
```

**行为**

取消自绘控件焦点。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.blur();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:640](../../library/desktop_gui/desktop_gui.chl.in#L640)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-184"></a>

### CustomWidget::hasFocus（185）

**声明**

```chali
bool hasFocus();
```

**行为**

查询自绘控件焦点状态。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。查询自绘控件焦点状态。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.hasFocus();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:641](../../library/desktop_gui/desktop_gui.chl.in#L641)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-185"></a>

### CustomWidget::setVisible（186）

**声明**

```chali
bool setVisible(bool visible);
```

**行为**

设置控件的可见状态。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `visible` | `bool visible` | 是否可见。 | 无 |

**返回值**

`bool`。设置控件的可见状态。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setVisible(visible);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:642](../../library/desktop_gui/desktop_gui.chl.in#L642)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-186"></a>

### CustomWidget::setEnabled（187）

**声明**

```chali
bool setEnabled(bool enabled);
```

**行为**

设置控件的输入启用状态。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `enabled` | `bool enabled` | 是否启用。 | 无 |

**返回值**

`bool`。设置控件的输入启用状态。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setEnabled(enabled);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:643](../../library/desktop_gui/desktop_gui.chl.in#L643)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-187"></a>

### CustomWidget::visible（188）

**声明**

```chali
bool visible();
```

**行为**

读取控件的可见状态。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。读取控件的可见状态。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.visible();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:644](../../library/desktop_gui/desktop_gui.chl.in#L644)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-188"></a>

### CustomWidget::enabled（189）

**声明**

```chali
bool enabled();
```

**行为**

读取控件的输入启用状态。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。读取控件的输入启用状态。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.enabled();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:645](../../library/desktop_gui/desktop_gui.chl.in#L645)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-189"></a>

### CustomWidget::setText（190）

**声明**

```chali
bool setText(String text);
```

**行为**

设置控件的主文本。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `text` | `String text` | 文本内容；具体用途见本项行为。 | 无 |

**返回值**

`bool`。设置控件的主文本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setText(text);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:646](../../library/desktop_gui/desktop_gui.chl.in#L646)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-190"></a>

### CustomWidget::text（191）

**声明**

```chali
String text();
```

**行为**

读取控件的主文本。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`String`。读取控件的主文本。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.text();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:647](../../library/desktop_gui/desktop_gui.chl.in#L647)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-191"></a>

### CustomWidget::setRange（192）

**声明**

```chali
bool setRange(float64 minimum, float64 maximum);
```

**行为**

设置控件的最小值和最大值。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `minimum` | `float64 minimum` | 允许的最小值。 | 无 |
| `maximum` | `float64 maximum` | 最大值或一次派发的事件上限，依本项行为解释。 | 无 |

**返回值**

`bool`。设置控件的最小值和最大值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setRange(minimum, maximum);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:648](../../library/desktop_gui/desktop_gui.chl.in#L648)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-192"></a>

### CustomWidget::setValue（193）

**声明**

```chali
bool setValue(float64 value);
```

**行为**

设置控件的浮点值。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `float64 value` | 本项操作使用的值，含义见行为说明。 | 无 |

**返回值**

`bool`。设置控件的浮点值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setValue(value);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:649](../../library/desktop_gui/desktop_gui.chl.in#L649)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-193"></a>

### CustomWidget::value（194）

**声明**

```chali
float64 value();
```

**行为**

读取控件的浮点值。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`float64`。读取控件的浮点值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.value();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:650](../../library/desktop_gui/desktop_gui.chl.in#L650)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-194"></a>

### CustomWidget::setNumber（195）

**声明**

```chali
bool setNumber(int64 value);
```

**行为**

设置控件的整数值。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `int64 value` | 本项操作使用的值，含义见行为说明。 | 无 |

**返回值**

`bool`。设置控件的整数值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setNumber(value);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:651](../../library/desktop_gui/desktop_gui.chl.in#L651)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-195"></a>

### CustomWidget::number（196）

**声明**

```chali
int64 number();
```

**行为**

读取控件的整数值。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`int64`。读取控件的整数值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.number();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:652](../../library/desktop_gui/desktop_gui.chl.in#L652)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-196"></a>

### CustomWidget::setChecked（197）

**声明**

```chali
bool setChecked(bool value);
```

**行为**

设置控件的勾选状态。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `value` | `bool value` | 本项操作使用的值，含义见行为说明。 | 无 |

**返回值**

`bool`。设置控件的勾选状态。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setChecked(value);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:653](../../library/desktop_gui/desktop_gui.chl.in#L653)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-197"></a>

### CustomWidget::checked（198）

**声明**

```chali
bool checked();
```

**行为**

读取控件的勾选状态。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`bool`。读取控件的勾选状态。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.checked();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:654](../../library/desktop_gui/desktop_gui.chl.in#L654)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-198"></a>

### CustomWidget::bounds（199）

**声明**

```chali
Rect bounds();
```

**行为**

返回控件最近一次绘制的边界。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`Rect`。返回控件最近一次绘制的边界。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.bounds();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:655](../../library/desktop_gui/desktop_gui.chl.in#L655)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-199"></a>

### CustomWidget::setClipping（200）

**声明**

```chali
void setClipping(bool enabled);
```

**行为**

设置自绘内容是否裁剪到控件边界。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `enabled` | `bool enabled` | 是否启用。 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.setClipping(enabled);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:656](../../library/desktop_gui/desktop_gui.chl.in#L656)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-200"></a>

### CustomWidget::draw（201）

**声明**

```chali
bool draw(Rect bounds);
```

**行为**

在 bounds 中绘制控件并同步派发回调；同一控件每帧绘制一次。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `bounds` | `Rect bounds` | 逻辑像素边界矩形。 | 无 |

**返回值**

`bool`。在 bounds 中绘制控件并同步派发回调；同一控件每帧绘制一次。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.draw(bounds);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:657](../../library/desktop_gui/desktop_gui.chl.in#L657)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-201"></a>

### CustomWidget::handle（202）

**声明**

```chali
bool handle(Event event);
```

**行为**

将窗口事件转换为控件局部输入并同步派发，返回是否处理。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `event` | `Event event` | 独立事件值。 | 无 |

**返回值**

`bool`。将窗口事件转换为控件局部输入并同步派发，返回是否处理。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.handle(event);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:667](../../library/desktop_gui/desktop_gui.chl.in#L667)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-202"></a>

### CustomWidget::emit（203）

**声明**

```chali
bool emit(Event event);
```

**行为**

同步派发事件；EventBus 返回执行的监听器数量，自绘控件返回是否处理。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `event` | `Event event` | 独立事件值。 | 无 |

**返回值**

`bool`。同步派发事件；EventBus 返回执行的监听器数量，自绘控件返回是否处理。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
obj.emit(event);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:696](../../library/desktop_gui/desktop_gui.chl.in#L696)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-203"></a>

### customWidget（204）

**声明**

```chali
CustomWidget customWidget();
```

**行为**

创建可注册绘图与输入回调的自绘控件。

**参数**

无显式参数。实例方法使用当前接收者。

**返回值**

`CustomWidget`。创建可注册绘图与输入回调的自绘控件。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::customWidget();
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:698](../../library/desktop_gui/desktop_gui.chl.in#L698)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-204"></a>

### run（205）

**声明**

```chali
void run(void fn() draw, void fn(Event) onEvent);
```

**行为**

运行帧循环，在绘图后派发窗口事件；回调允许请求关闭窗口。

draw 中构建一帧；窗口输入在控件之后派发，不保存临时原生回调。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `draw` | `void fn() draw` | 每帧执行的绘制回调。 | 无 |
| `onEvent` | `void fn(Event) onEvent` | 窗口事件回调。 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::run(draw, onEvent);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:701](../../library/desktop_gui/desktop_gui.chl.in#L701)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。

<a id="api-205"></a>

### run（206）

**声明**

```chali
void run(void fn() draw);
```

**行为**

运行帧循环，在绘图后派发窗口事件；回调允许请求关闭窗口。

**参数**

| 参数 | 类型／声明 | 含义 | 默认值 |
| --- | --- | --- | --- |
| `draw` | `void fn() draw` | 每帧执行的绘制回调。 | 无 |

**返回值**

无返回值。

**错误与边界**

遵循本页“公共约定与错误边界”。

**调用形式**

```chali
std::gui::run(draw);
```

**源码**：[library/desktop_gui/desktop_gui.chl.in:720](../../library/desktop_gui/desktop_gui.chl.in#L720)。

**完整示例**：[example/desktop-gui/demo.chl](../../example/desktop-gui/demo.chl)。
