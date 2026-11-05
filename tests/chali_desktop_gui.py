"""在真实隐藏窗口中核对全部控件、C 状态、事件和窗口代际所有权。"""
import argparse
import ctypes as c
from pathlib import Path
import re
import threading

ROOT = Path(__file__).resolve().parents[1]


class Event(c.Structure):
    """与公开 C 事件值保持一致的基础标量布局。"""
    _fields_ = [('kind', c.c_int64), ('source', c.c_int64), ('code', c.c_int64),
                ('x', c.c_double), ('y', c.c_double), ('delta_x', c.c_double), ('delta_y', c.c_double)]


def main():
    """执行不移动系统指针、不显示交互窗口的真实 C ABI 验收。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--library', type=Path, required=True)
    parser.add_argument('--raygui-header', type=Path, required=True)
    args = parser.parse_args()
    controls = re.findall(r'^CHALI_GUI_CONTROL\((\w+),\s*(Gui\w+)\)',
                          (ROOT / 'library/desktop_gui/controls.def').read_text(encoding='utf-8'), re.M)
    header = args.raygui_header.read_text(encoding='utf-8')
    declarations = header[header.index('RAYGUIAPI int GuiWindowBox'):header.index('RAYGUIAPI int GuiColorPanelHSV')]
    upstream = re.findall(r'RAYGUIAPI int (Gui\w+)\(', declarations) + ['GuiColorPanelHSV']
    assert [name for _, name in controls] == upstream and len(controls) == 34
    library = c.CDLL(str(args.library.resolve()))
    template = (ROOT / 'library/desktop_gui/desktop_gui.chl.in').read_text(encoding='utf-8')
    exports = re.findall(r'#abi\("C",\s*"[^"]+",\s*"(chali_gui_\w+)"', template)
    for name in exports:
        assert getattr(library, name, None) is not None, 'Missing language ABI export: ' + name

    def function(name, arguments, result):
        """绑定精确 C 原型，避免平台默认整数宽度截断。"""
        entry = getattr(library, 'chali_gui_' + name)
        entry.argtypes = arguments
        entry.restype = result
        return entry

    open_window = function('open_ex', [c.c_int32, c.c_int32, c.c_char_p, c.c_int32], c.c_bool)
    close = function('close', [], None)
    close_id = function('close_id', [c.c_int64], None)
    window_id = function('window_id', [], c.c_int64)
    begin = function('begin_frame', [], c.c_bool)
    end = function('end_frame', [], None)
    poll = function('poll_event', [], Event)
    request_close = function('request_close', [], None)
    should_close = function('should_close', [], c.c_bool)
    set_window = function('set_window_value', [c.c_int32, c.c_double], c.c_bool)
    configure = function('configure', [c.c_int32, c.c_double], c.c_bool)
    create = function('widget_create', [c.c_int32, c.c_char_p, c.c_int64], c.c_int64)
    valid = function('widget_valid', [c.c_int64], c.c_bool)
    destroy = function('widget_close', [c.c_int64], None)
    integer = function('widget_integer', [c.c_int64, c.c_int32], c.c_int64)
    set_integer = function('widget_set_integer', [c.c_int64, c.c_int32, c.c_int64], c.c_bool)
    real = function('widget_real', [c.c_int64, c.c_int32], c.c_double)
    set_real = function('widget_set_real', [c.c_int64, c.c_int32, c.c_double], c.c_bool)
    set_range = function('widget_range', [c.c_int64, c.c_double, c.c_double], c.c_bool)
    set_text = function('widget_set_text', [c.c_int64, c.c_int32, c.c_char_p, c.c_int64], c.c_bool)
    get_text = function('widget_text', [c.c_int64, c.c_int32], c.c_void_p)
    capacity = function('widget_capacity', [c.c_int64, c.c_int32], c.c_bool)
    draw = function('widget_draw', [c.c_int64, c.c_float, c.c_float, c.c_float, c.c_float], c.c_int64)
    free = function('free', [c.c_void_p], None)
    begin_clip = function('begin_clip', [c.c_float] * 4, c.c_bool)
    end_clip = function('end_clip', [], None)

    assert not open_window(0, 480, b'invalid', 2)
    assert open_window(640, 480, b'ChaLi hidden C GUI validation', 2)
    try:
        first_window = window_id()
        assert first_window > 0 and not open_window(640, 480, b'duplicate', 2)
        assert set_window(0, 0)
        assert not set_window(0, float('nan'))
        assert poll().kind == 1
        models = {}
        for index, (name, _) in enumerate(controls):
            text = b'First;Second;Third' if name in ('tab_bar', 'toggle_group', 'toggle_slider', 'combo_box', 'dropdown_box', 'list_view', 'list_view_ex') else name.encode()
            identity = create(index, text, len(text))
            assert identity > 0 and valid(identity)
            assert integer(identity, 11) == index
            models[name] = identity
        assert len(set(models.values())) == 34
        assert set_integer(models['check_box'], 4, 1) and integer(models['check_box'], 4) == 1
        assert set_range(models['slider'], -10, 10) and set_real(models['slider'], 0, 1.25)
        assert real(models['slider'], 0) == 1.25 and not set_real(models['slider'], 0, float('nan'))
        assert not set_range(models['slider'], 10, 10)
        assert real(models['slider'], 0) == 1.25
        assert set_range(models['spinner'], -200, 200) and set_integer(models['spinner'], 1, -123)
        assert integer(models['spinner'], 1) == -123
        assert set_integer(models['color_picker'], 12, 0x12345678)
        assert integer(models['color_picker'], 12) == 0x12345678
        text = '中文 text'.encode()
        textbox = models['text_box']
        assert set_text(textbox, 0, text, len(text))
        pointer = get_text(textbox, 0)
        try:
            assert c.string_at(pointer) == text
        finally:
            free(pointer)
        assert not set_text(textbox, 0, b'a\0b', 3)
        assert not capacity(textbox, 2) and capacity(textbox, 128)
        assert not set_integer(models['grid'], 10, 0)
        errors = []

        def foreign_thread():
            """外线程不能绘制、销毁或接管已有窗口。"""
            try:
                assert window_id() == 0 and should_close() and not begin()
                assert not valid(textbox)
                assert not open_window(200, 100, b'foreign', 2)
                close()
            except BaseException as error:
                errors.append(error)

        thread = threading.Thread(target=foreign_thread)
        thread.start()
        thread.join(timeout=5)
        assert not thread.is_alive() and not errors and window_id() == first_window and valid(textbox)
        assert configure(1, 1)
        assert begin() and not begin()
        assert begin_clip(0, 0, 300, 300) and begin_clip(20, 20, 400, 400)
        end_clip()
        end_clip()
        end_clip()
        assert not begin_clip(float('inf'), 0, 20, 20)
        assert not begin_clip(1e30, 0, 20, 20)
        for name, identity in models.items():
            draw(identity, 24, 24, 300, 200)
            assert draw(identity, 24, 24, 300, 200) == 0, name
        end()
        assert not begin_clip(0, 0, 20, 20)
        request_close()
        kinds = []
        while True:
            event = poll()
            if not event.kind:
                break
            kinds.append(event.kind)
        assert kinds.count(2) == 1 and should_close()
        old = models['button']
        destroy(old)
        destroy(old)
        assert not valid(old)
        replacement = create(7, b'new', 3)
        assert replacement > old
        close()
        close()
        assert not valid(replacement)
        assert open_window(320, 200, b'new generation', 2)
        assert window_id() > first_window
        close_id(first_window)
        assert window_id() > first_window
        assert not valid(replacement)
        assert begin()
        draw(create(7, b'reopened', 8), 20, 20, 100, 30)
        end()
    finally:
        close()
    print('GUI C ABI: all 34 upstream controls, hidden rendering, text ownership, events and window generations passed')


if __name__ == '__main__':
    main()
