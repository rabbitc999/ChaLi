"""原生扩展的模块清单与 API 语义，签名始终从实际模板扫描。"""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
TEMPLATES = {
    'std::gui': 'library/desktop_gui/desktop_gui.chl.in',
    'std::audio': 'library/desktop_gui/desktop_audio.chl.in',
    'std::xml': 'library/xml/xml.chl.in',
    'std::crypto': 'library/crypto/crypto.chl.in',
}
MODULES = {
    'std::gui': ('桌面图形界面', '窗口、控件、绘图和事件操作在创建窗口的线程执行。用 beginFrame/endFrame 配对组织一帧；Widget.draw 每帧对同一控件调用一次并同步派发回调。窗口拥有控件和纹理，关闭窗口后旧句柄失效；用 isValid 检查创建结果。无图形会话时不能运行交互示例。原生服务随支持它的发行版内嵌，无需另行部署共享库。'),
    'std::audio': ('音频播放', '先 open 初始化音频设备，再 load 加载声音并用返回的正整数句柄播放。无效句柄和设备不可用通过 false 或零报告。unload 释放单项声音，close 释放所有声音及设备；不要继续使用已释放的句柄。需要可用的音频设备和有效输入文件。'),
    'std::xml': ('XML 文档', 'parse 接收 UTF-8 文本，成功返回拥有节点值，失败返回 Error，解析诊断包含从 1 开始的行列号。childCount 统计元素与文本组成的混合内容；child 和 text 使用相同的从 0 开始的下标，类型不匹配或越界返回 None。节点通过值复制独立保存。escapeText 与 escapeAttribute 只转义文本，不解析文档。'),
    'std::crypto': ('密码学', '使用 libsodium 提供安全随机数、SHA-256、HMAC-SHA-256 和 Ed25519 验签。String 按 UTF-8 字节及其显式长度处理；摘要和随机输出为小写十六进制。randomHex、sha256 和 hmacSha256 通过 Result 报告失败；verifyEd25519 对格式错误、非法密钥或签名不匹配返回 false。随机字节数为 1–65536，Ed25519 公钥为 64 个十六进制字符，分离签名为 128 个十六进制字符。模块及其原生静态归档随支持原生服务的发行版内嵌。'),
}


def native_inventory():
    """移除 ABI 指令但保留源码行号，从渲染后的控件模板取得完整工厂集。"""
    from chali_api import declarations
    from setup_desktop_gui import render_gui_module
    data = {}
    for module, relative in TEMPLATES.items():
        path = ROOT / relative
        source = path.read_text(encoding='utf-8')
        if module == 'std::gui':
            source = render_gui_module(source, '@chali/desktop')
        source = re.sub(r'^#abi\([^\n]*\)', '', source, flags=re.M)
        data[module] = declarations(path, source, 0, ROOT)
    return data


GUI = {
    'shouldClose': '查询窗口是否已收到关闭请求。', 'beginFrame': '开始一帧，失败时返回 false。',
    'endFrame': '提交当前帧并结束绘制。', 'close': '释放当前窗口及它拥有的控件、字体与纹理资源。',
    'open': '按指定逻辑尺寸和标题打开窗口，成功返回 true。',
    'openWindow': '按指定尺寸、标题与选项打开窗口；options 的 1、2、4 位分别启用缩放、隐藏和垂直同步。',
    'requestClose': '向当前窗口提交关闭请求。', 'setBackground': '设置帧背景颜色，编码为 RRGGBBAA。',
    'endClip': '结束当前裁剪区域，与 beginClip 配对。', 'setStyle': '设置指定控件类别的样式属性。',
    'getStyle': '读取指定控件类别的样式属性值。', 'loadFont': '从文件加载指定字号和字形集合的字体。',
    'setTitle': '设置窗口标题。', 'setTargetFps': '设置目标帧率。', 'setExitKey': '设置触发窗口关闭的按键。',
    'setMinSize': '设置窗口可缩放的最小尺寸。', 'setOpacity': '设置窗口透明度。',
    'width': '返回窗口逻辑宽度。', 'height': '返回窗口逻辑高度。', 'frameTime': '返回最近一帧耗时，单位为秒。',
    'mousePosition': '返回鼠标在窗口内的位置。', 'mouseWheel': '返回本帧鼠标滚轮增量。',
    'dpiScale': '返回窗口的二维 DPI 缩放比例。', 'focused': '查询窗口是否获得输入焦点。',
    'time': '返回宿主启动后的计时秒数。', 'beginClip': '按逻辑像素矩形开始裁剪，成功返回 true。',
    'enable': '设置全局控件输入启用状态。', 'lock': '设置全局控件锁定状态。',
    'setAlpha': '设置全局控件透明度。', 'setState': '设置全局控件绘制状态。',
    'enableTooltips': '启用或禁用控件提示。', 'resetStyle': '恢复默认控件样式。',
    'setFontSize': '设置控件默认字体大小。', 'setTooltip': '设置当前控件提示文本。',
    'rgba': '把四个 0–255 的颜色通道编码成 RRGGBBAA。',
    'pollEvent': '取出下一项窗口事件；队列为空时返回无效 Event。',
    'createControl': '按控件种类创建窗口拥有的 Widget；失败返回无效句柄。',
    'controlCount': '返回生成清单支持的控件种类数量。', 'controlName': '返回清单下标对应的控件工厂名称；越界返回空串。',
    'frameActive': '查询当前是否位于 beginFrame/endFrame 之间。', 'inputEnabled': '查询当前控件输入是否启用。',
    'frameCount': '返回当前窗口已开始的帧数。', 'clipBounds': '返回当前逻辑像素裁剪矩形。',
    'drawPixel': '绘制指定位置的像素。', 'drawLine': '按指定线宽绘制线段。',
    'drawRectangle': '绘制矩形；thickness 为零时填充，否则画边框，roundness 为 0–1。',
    'drawCircle': '绘制圆；thickness 为零时填充，否则画边框。',
    'drawEllipse': '绘制椭圆，outline 决定只画轮廓。', 'drawTriangle': '绘制三个顶点确定的三角形，outline 决定只画轮廓。',
    'drawPolygon': '绘制指定边数、半径和旋转角的正多边形。',
    'drawBezier': '用起点、终点和两个控制点绘制三次贝塞尔曲线。',
    'drawRing': '按内外半径和起止角度绘制环扇形，角度使用度，内半径为零时为扇形。',
    'drawText': '按指定位置、字号、字距和颜色绘制文本。', 'measureText': '返回指定字号及字距下文本的二维尺寸。',
    'loadTexture': '加载窗口拥有的 GPU 纹理；失败返回无效 Texture。',
    'customEvent': '创建独立拥有名称、文本、整数码和浮点值的自定义事件。',
    'customWidget': '创建可注册绘图与输入回调的自绘控件。',
    'run': '运行帧循环，在绘图后派发窗口事件；回调允许请求关闭窗口。',
}
PROPERTIES = {
    'Text': '主文本', 'LeftText': '左侧文本', 'RightText': '右侧文本', 'Message': '消息正文',
    'Buttons': '按钮文本列表', 'TextCapacity': '文本缓冲容量', 'Checked': '勾选状态',
    'Editing': '编辑状态', 'Secret': '密码遮蔽状态', 'Visible': '可见状态', 'Enabled': '输入启用状态',
    'Selected': '所选项下标', 'Number': '整数值', 'ScrollIndex': '滚动项下标', 'Value': '浮点值',
    'Color': 'RRGGBBAA 颜色值', 'Hsv': 'HSV 颜色分量', 'Spacing': '网格间距及细分数',
    'Content': '可滚动内容矩形', 'ScrollOffset': '二维滚动偏移', 'Range': '最小值与最大值',
}
PARAMETERS = {
    'x':'逻辑像素横坐标。', 'y':'逻辑像素纵坐标。', 'width':'逻辑像素宽度。', 'height':'逻辑像素高度。',
    'title':'窗口标题。', 'text':'文本内容；具体用途见本项行为。', 'control':'控件样式类别编号。',
    'property':'样式属性编号。', 'value':'本项操作使用的值，含义见行为说明。', 'rgba':'RRGGBBAA 颜色编码。',
    'path':'输入文件路径。', 'size':'字体大小。', 'glyphs':'需要加载的 UTF-8 字符集合。',
    'options':'窗口选项位：1 可缩放，2 隐藏，4 垂直同步。', 'fps':'每秒目标帧数。',
    'key':'HMAC 密钥或按键码，按本项签名与行为解释。', 'bounds':'逻辑像素边界矩形。',
    'red':'红通道，0–255。', 'green':'绿通道，0–255。', 'blue':'蓝通道，0–255。', 'alpha':'透明通道，0–255。',
    'callback':'同步执行的事件或绘图回调。', 'items':'独立文本项列表。', 'capacity':'文本缓冲容量。',
    'minimum':'允许的最小值。', 'maximum':'最大值或一次派发的事件上限，依本项行为解释。',
    'hue':'HSV 色相。', 'saturation':'HSV 饱和度。', 'brightness':'HSV 明度。',
    'spacing':'字距或网格间距，依本项行为解释。', 'subdivisions':'网格细分数。', 'kind':'控件种类编号。',
    'index':'从零开始的下标。', 'point':'像素位置。', 'start':'起点。', 'end':'终点。',
    'thickness':'线宽；支持填充的图形使用零表示填充。', 'color':'RRGGBBAA 颜色编码。',
    'roundness':'圆角比例，0–1。', 'center':'图形中心。', 'radius':'圆半径或椭圆二维半径。',
    'outline':'是否只绘制轮廓。', 'a':'第一个顶点。', 'b':'第二个顶点。', 'c':'第三个顶点。',
    'sides':'正多边形边数。', 'rotation':'旋转角度，单位为度。', 'first_control':'第一控制点。',
    'second_control':'第二控制点。', 'inner_radius':'内半径。', 'outer_radius':'外半径。',
    'start_angle':'起始角度，单位为度。', 'end_angle':'终止角度，单位为度。', 'position':'文本绘制位置。',
    'destination':'目标矩形。', 'tint':'乘到纹理上的 RRGGBBAA 颜色，默认全白不改变颜色。',
    'source':'纹理源矩形。', 'origin':'旋转原点。', 'name':'属性或事件的精确名称。',
    'code':'自定义整数事件码。', 'id':'声音或订阅句柄，依本项行为解释。', 'event':'独立事件值。',
    'visible':'是否可见。', 'enabled':'是否启用。', 'draw':'每帧执行的绘制回调。',
    'onEvent':'窗口事件回调。', 'other':'参与相交运算的另一个矩形。',
    'input':'参与摘要或验签的原始 UTF-8 文本。', 'count':'安全随机字节数量。',
    'public_key':'64 个十六进制字符表示的 32 字节 Ed25519 公钥。',
    'signature':'128 个十六进制字符表示的 64 字节 Ed25519 分离签名。',
}
METHODS = {
    'isValid': '检查当前值的宿主句柄或几何边界是否有效。', 'id': '返回宿主控件身份编号。',
    'clearCallbacks': '清空已注册的回调。', 'close': '释放该句柄对应的宿主资源，后续调用须重新检查有效性。',
    'onEvent': '登记一般输入事件回调。', 'onClick': '登记点击回调。', 'onChange': '登记值变化回调。',
    'onSubmit': '登记提交回调。', 'onClose': '登记控件关闭回调。', 'onDraw': '登记自绘回调，参数为本次绘制边界。',
    'setItems': '设置独立字符串项列表；每一项允许包含分号与换行。',
    'setClipping': '设置自绘内容是否裁剪到控件边界。', 'setRange': '设置控件的最小值和最大值。',
    'beginContent': '开始绘制滚动面板内容，调用后需与 endClip 配对。',
    'on': '按事件名登记监听器并返回订阅编号。', 'once': '按事件名登记执行一次后移除的监听器。',
    'off': '按订阅编号移除监听器，返回是否找到。', 'clear': '清除监听器和待派发队列。',
    'post': '将独立事件值加入延迟派发队列。', 'pending': '返回队列中尚未消费的事件数量。',
    'pump': '最多派发 maximum 项排队事件并返回实际派发数，默认上限 256。',
    'emit': '同步派发事件；EventBus 返回执行的监听器数量，自绘控件返回是否处理。',
    'focus': '请求自绘控件获得焦点。', 'blur': '取消自绘控件焦点。', 'hasFocus': '查询自绘控件焦点状态。',
    'handle': '将窗口事件转换为控件局部输入并同步派发，返回是否处理。',
    'bounds': '返回控件最近一次绘制的边界。', 'viewport': '返回滚动面板可视矩形。',
    'scrollOffset': '返回滚动面板的二维偏移。', 'focusIndex': '返回当前获得焦点的列表项下标。',
    'result': '返回控件最近一次交互结果码。', 'hue': '返回 HSV 色相。',
    'saturation': '返回 HSV 饱和度。', 'brightness': '返回 HSV 明度。',
}


def native_behavior(entry):
    """依据模块与所属类型解释实际入口，未知名称必须补充说明。"""
    module, owner, name = entry['module'], entry['owner'], entry['name']
    if module == 'std::audio':
        return {'open': '初始化音频设备，成功返回 true。', 'load': '加载声音文件，成功返回正整数句柄，失败返回零。',
                'play': '播放指定声音，成功返回 true。', 'stop': '停止指定声音播放，成功返回 true。',
                'isPlaying': '查询指定声音是否正在播放。', 'unload': '停止并卸载指定声音，成功返回 true。',
                'close': '卸载全部声音并关闭音频设备。'}[name]
    if module == 'std::xml':
        return {'name': '返回元素的源限定名。', 'attribute': '按精确名称读取属性；不存在返回 None。',
                'childCount': '返回按源顺序排列的元素及文本片段总数。',
                'child': '按混合内容下标读取元素子节点；越界或该位置为文本时返回 None。',
                'text': '按混合内容下标读取文本；越界或该位置为元素时返回 None。',
                'parse': '解析一份 UTF-8 XML 文档，成功返回根节点，失败返回带行列的 Error。',
                'escapeText': '转义文本中的 &、< 和 >。',
                'escapeAttribute': '转义文本、单双引号以及回车、换行和制表符，适用于引号包围的属性值。'}[name]
    if module == 'std::crypto':
        return {'randomHex': '生成 count 个安全随机字节并返回小写十六进制文本。',
                'sha256': '计算 input 的 SHA-256 并返回小写十六进制摘要。',
                'hmacSha256': '以 key 为密钥计算 input 的 HMAC-SHA-256，返回小写十六进制摘要。',
                'verifyEd25519': '使用十六进制 Ed25519 公钥验证分离签名；格式不合法或验证失败返回 false。'}[name]
    if not owner:
        if name in GUI: return GUI[name]
        if entry['result'] == 'Widget': return '创建 '+name+' 控件状态；由窗口拥有，draw 时同步派发变化。'
        if name in ('label', 'button'): return '在指定逻辑像素矩形内绘制即时'+('标签。' if name == 'label' else '按钮，返回本帧是否点击。')
    if owner == 'Event':
        if name == 'kind': return '将宿主事件码转换为 EventKind。'
        if name == 'isValid': return '检查事件是否为非空事件。'
        return '检查控件事件是否包含 '+{'changed':'值变化','clicked':'点击','submitted':'提交','editingChanged':'编辑变化','closed':'关闭','selectionChanged':'选择变化','scrolled':'滚动','hovered':'悬停','focusChanged':'焦点变化'}[name]+' 标志。'
    if owner in ('Rect', 'Point'):
        return {'at':'按指定分量构造几何值。', 'contains':'检查给定点是否在矩形内。',
                'intersection':'返回两个矩形的相交区域。', 'isValid':'检查矩形宽度和高度是否为正。'}[name]
    if owner == 'Texture':
        return {'isValid':'检查纹理句柄是否仍有效。', 'close':'卸载该纹理。', 'size':'返回纹理的像素宽高。',
                'draw':'将完整纹理绘制到目标矩形并乘以 tint 颜色。',
                'drawRegion':'按源矩形、目标矩形、旋转原点、角度和 tint 绘制纹理区域。'}[name]
    if name == 'draw': return '在 bounds 中绘制控件并同步派发回调；同一控件每帧绘制一次。'
    if name in METHODS: return METHODS[name]
    if name.startswith('set') and name[3:] in PROPERTIES: return '设置控件的'+PROPERTIES[name[3:]]+'。'
    prop = name[:1].upper()+name[1:]
    if prop in PROPERTIES: return '读取控件的'+PROPERTIES[prop]+'。'
    raise ValueError('缺少原生 API 说明：'+module+'::'+owner+'::'+name)
