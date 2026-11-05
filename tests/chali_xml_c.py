"""通过真实 C ABI 核对 XML 的混合内容、UTF-8、所有权和错误边界。"""
import argparse
import ctypes
import json
from pathlib import Path
import random
import xml.etree.ElementTree as ET


def merge_children(children):
    """合并相邻文本，以独立 XML 模型核对 Expat 的文本分片。"""
    result = []
    for child in children:
        if isinstance(child, str) and result and isinstance(result[-1], str):
            result[-1] += child
        else:
            result.append(child)
    return result


def expected(element):
    """将 Python 标准库元素转换为桥接暴露的保序模型。"""
    children = [element.text] if element.text else []
    for child in element:
        children.append(expected(child))
        if child.tail:
            children.append(child.tail)
    return dict(name=element.tag, attributes=element.attrib, children=merge_children(children))


def normalize(value):
    """递归保留元素和属性，仅合并相邻文本片段。"""
    if isinstance(value, str):
        return value
    return dict(name=value['name'], attributes=value['attributes'],
                children=merge_children([normalize(child) for child in value['children']]))


def main():
    """加载实际桥接库并验证输入边界和结果释放。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--library', type=Path, required=True)
    library = ctypes.CDLL(str(parser.parse_args().library.resolve()))
    library.chali_xml_parse.argtypes = [ctypes.c_char_p, ctypes.c_int64]
    library.chali_xml_parse.restype = ctypes.c_void_p
    library.chali_xml_free.argtypes = [ctypes.c_void_p]
    library.chali_xml_free.restype = None

    def parse(data, size=None):
        """立即复制 C 结果并始终调用配对的 C 释放入口。"""
        pointer = library.chali_xml_parse(data, len(data) if size is None else size)
        assert pointer
        try:
            return json.loads(ctypes.string_at(pointer))
        finally:
            library.chali_xml_free(pointer)

    rng = random.Random(8432)

    def element(depth=0):
        """生成具有独立模型的属性、实体和混合内容。"""
        node = ET.Element('node' + str(rng.randrange(9)))
        for index in range(rng.randrange(4)):
            node.set('a' + str(index), rng.choice(['x&y', '中文', 'a"b', '<>', '\tline\nnext']))
        node.text = rng.choice(['hello', '中文 &<>', 'text\nnext', None])
        if depth < 5:
            for _ in range(rng.randrange(4)):
                child = element(depth + 1)
                child.tail = rng.choice(['tail', ' x&y ', None])
                node.append(child)
        return node

    for _ in range(600):
        node = element()
        assert normalize(parse(ET.tostring(node, encoding='utf-8'))) == expected(node)
    for data in (b'', b'<r><a></r>', b'<r>\xff</r>', b'<r/>trailing'):
        error = parse(data)
        assert isinstance(error['error'], str) and error['line'] >= 1 and error['column'] >= 1
    assert parse(b'<!DOCTYPE r><r/>')['error'] == 'DOCTYPE is disabled'
    nested = b'<a>' * 129 + b'</a>' * 129
    assert parse(nested)['error'] == 'XML nesting or output limit exceeded'
    for size in (-1, 16 * 1024 * 1024 + 1):
        assert parse(b'', size) == dict(error='XML input limit exceeded', line=1, column=1)
    library.chali_xml_free(None)
    print('XML C ABI: 600 independent trees, UTF-8, mixed content, limits and owned results passed')


if __name__ == '__main__':
    main()
