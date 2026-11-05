"""直接通过 C ABI，以 Decimal 和冻结 mpmath 参考值验证大浮点内核。"""
import argparse
import ctypes as C
import decimal as D
import json
import os
import random
from bignum_c_api import BigFloat, BigString, MathError, ROOT, bind, build_shared


def rounded(value, digits):
    """以独立 Decimal 运算实现有效位数四舍五入，平局远离零。"""
    if not value:
        return D.Decimal(0)
    return value.quantize(D.Decimal(1).scaleb(value.adjusted() - digits + 1), rounding=D.ROUND_HALF_UP)


class Kernel:
    """固定 C 原型并配对数值、文本的创建与释放。"""
    def __init__(self, library):
        """绑定基础算术与所有高精度数学入口。"""
        self.library = library
        p = C.POINTER(BigFloat)
        self.initialize = bind(library, 'chali_bigfloat_init', [p, C.c_void_p])
        self.clear = bind(library, 'chali_bigfloat_clear', [p], None)
        self.parse = bind(library, 'chali_bigfloat_parse', [p, C.c_char_p, C.c_size_t, C.POINTER(C.c_int)])
        self.format = bind(library, 'chali_bigfloat_to_string', [C.POINTER(BigString), p, C.c_int, C.c_int])
        self.clear_text = bind(library, 'chali_big_string_clear', [C.POINTER(BigString)], None)
        self.add = bind(library, 'chali_bigfloat_add', [p, p, p, C.c_int])
        self.multiply = bind(library, 'chali_bigfloat_multiply', [p, p, p])
        self.divide = bind(library, 'chali_bigfloat_divide', [p, p, p, C.c_int])
        self.round = bind(library, 'chali_bigfloat_round', [p, p, C.c_int])
        self.sqrt = bind(library, 'chali_bigfloat_sqrt', [p, p, C.c_int])
        self.root = bind(library, 'chali_bigfloat_root', [p, p, C.c_uint64, C.c_int])
        self.compare = bind(library, 'chali_bigfloat_compare', [p, p, C.POINTER(C.c_int), C.c_void_p])
        self.math = {}
        for name in ['pi', 'exp', 'ln', 'log10', 'sin', 'cos', 'tan', 'atan', 'asin', 'acos', 'pow', 'fmod']:
            parameters = [p] if name == 'pi' else ([p, p, p] if name in ['pow', 'fmod'] else [p, p])
            if name != 'fmod':
                parameters += [C.c_int]
            parameters += [C.POINTER(MathError)]
            self.math[name] = bind(library, 'chali_bigfloat_' + name, parameters)

    def value(self, text):
        """构造一个有独立所有者的十进制 C 值。"""
        value = BigFloat()
        assert self.initialize(C.byref(value), None) == 0
        raw = str(text).encode('ascii')
        valid = C.c_int()
        status = self.parse(C.byref(value), raw, len(raw), C.byref(valid))
        if status or not valid.value:
            self.clear(C.byref(value))
            raise AssertionError((text, status, valid.value))
        return value

    def number(self, value):
        """解析 C 服务的完整精确输出，始终释放返回文本。"""
        text = BigString()
        try:
            assert self.format(C.byref(text), C.byref(value), 0, 0) == 0
            return D.Decimal(C.string_at(text.data, text.length).decode('ascii'))
        finally:
            self.clear_text(C.byref(text))

    def release(self, *values):
        """配对释放一组已初始化的 C 数值。"""
        for value in values:
            self.clear(C.byref(value))


def arithmetic(kernel, seed):
    """核对四则运算、舍入、比较、平方根以及原地运算。"""
    rng = random.Random(seed)
    for index in range(800):
        a = str(rng.randrange(-10 ** rng.randrange(1, 90), 10 ** rng.randrange(1, 90))) + 'e' + str(rng.randrange(-150, 151))
        b = str(rng.randrange(-10 ** 40, 10 ** 40) or 1) + 'e' + str(rng.randrange(-150, 151))
        av, bv, output = kernel.value(a), kernel.value(b), kernel.value('999')
        x, y, digits = D.Decimal(a), D.Decimal(b), rng.randrange(1, 81)
        try:
            assert kernel.number(av) == x
            for subtract in [0, 1]:
                assert kernel.add(C.byref(output), C.byref(av), C.byref(bv), subtract) == 0
                assert kernel.number(output) == (x - y if subtract else x + y), (index, 'add', subtract)
            assert kernel.multiply(C.byref(output), C.byref(av), C.byref(bv)) == 0
            assert kernel.number(output) == x * y, (index, 'multiply')
            assert kernel.round(C.byref(output), C.byref(av), digits) == 0
            assert kernel.number(output) == rounded(x, digits), (index, 'round')
            assert kernel.divide(C.byref(output), C.byref(av), C.byref(bv), digits) == 0
            assert kernel.number(output) == rounded(x / y, digits), (index, 'divide')
            order = C.c_int()
            assert kernel.compare(C.byref(av), C.byref(bv), C.byref(order), None) == 0
            assert order.value == ((x > y) - (x < y))
            if x >= 0:
                assert kernel.sqrt(C.byref(output), C.byref(av), digits) == 0
                assert kernel.number(output) == rounded(x.sqrt(), digits), (index, 'sqrt')
            assert kernel.multiply(C.byref(av), C.byref(av), C.byref(bv)) == 0
            assert kernel.number(av) == x * y and kernel.number(bv) == y, (index, 'left alias')
            assert kernel.divide(C.byref(bv), C.byref(av), C.byref(bv), digits) == 0
            assert kernel.number(bv) == rounded(x, digits), (index, 'right alias')
        finally:
            kernel.release(av, bv, output)
    for x, degree, digits in [('2', 3, 60), ('1e-70', 7, 60), ('1234567.89', 4, 80), ('0', 5, 20), ('4', 2, 60)]:
        av, output = kernel.value(x), kernel.value('999')
        try:
            assert kernel.root(C.byref(output), C.byref(av), degree, digits) == 0
            expected = rounded(D.Decimal(x) ** (D.Decimal(1) / degree), digits) if D.Decimal(x) else D.Decimal(0)
            assert kernel.number(output) == expected, (x, degree, digits)
        finally:
            kernel.release(av, output)
    print('C bigfloat: 800 Decimal cases, 1600 aliases and 5 roots passed', flush=True)


def mathematics(kernel):
    """读取独立生成的参考值，并核对数学错误保留原结果。"""
    rows = json.loads((ROOT / 'tests/fixtures/bigfloat_math.json').read_text(encoding='utf-8'))['values']
    for row in rows:
        name, digits, first, second = row['case']
        a, b, output = kernel.value(first), kernel.value(second), kernel.value('999')
        error = MathError()
        try:
            arguments = [C.byref(output)]
            if name != 'pi':
                arguments.append(C.byref(a))
            if name in ['pow', 'fmod']:
                arguments.append(C.byref(b))
            if name != 'fmod':
                arguments.append(digits)
            arguments.append(C.byref(error))
            status = kernel.math[name](*arguments)
            assert status == 0, (row['case'], status, error.kind, error.message)
            with D.localcontext() as context:
                context.prec = digits + 180
                assert kernel.number(output) == rounded(D.Decimal(row['expected']), digits), row['case']
        finally:
            kernel.release(a, b, output)
    for name, first in [('ln', '-1'), ('log10', '0'), ('asin', '2'), ('acos', '-2')]:
        a, output = kernel.value(first), kernel.value('999')
        error = MathError()
        try:
            status = kernel.math[name](C.byref(output), C.byref(a), 20, C.byref(error))
            assert status != 0 and error.kind and error.message, (name, status, error.kind)
            assert kernel.number(output) == D.Decimal(999), (name, 'changed on error')
        finally:
            kernel.release(a, output)
    print(f'C bigfloat: {len(rows)} frozen mpmath values and 4 transactional domain errors passed', flush=True)


def large_products(kernel, seed):
    """让十进制 C 接口跨过 Toom/NTT 区间，以独立 Decimal 核对大尾数与指数。"""
    rng = random.Random(seed)
    widths = [1500, 1540, 1550, 1840, 2000, 2460, 3000, 8000, 16000, 72000, 74000, 120000]
    for index, width in enumerate(widths):
        first = '7' + ''.join(str(rng.randrange(10)) for _ in range(width - 1))
        second_width = width if index % 3 != 1 else max(1, width // 8)
        second = '9' + ''.join(str(rng.randrange(10)) for _ in range(second_width - 1))
        a = ('-' if index % 2 else '') + first + 'e-123'
        b = a if index % 3 == 0 else second + 'e57'
        av, bv, output = kernel.value(a), kernel.value(b), kernel.value('999')
        try:
            with D.localcontext() as context:
                context.prec = width * 2 + 200
                expected = D.Decimal(a) * D.Decimal(b)
                assert kernel.multiply(C.byref(output), C.byref(av), C.byref(bv)) == 0
                assert kernel.number(output) == expected, (width, 'large product')
                assert kernel.number(av) == D.Decimal(a) and kernel.number(bv) == D.Decimal(b)
                assert kernel.multiply(C.byref(av), C.byref(av), C.byref(bv)) == 0
                assert kernel.number(av) == expected, (width, 'large left alias')
                kernel.clear(C.byref(av))
                av = kernel.value(a)
                assert kernel.multiply(C.byref(bv), C.byref(av), C.byref(bv)) == 0
                assert kernel.number(bv) == expected, (width, 'large right alias')
        finally:
            kernel.release(av, bv, output)
    print(f'C bigfloat: {len(widths)} large Decimal products and {len(widths) * 2} aliases, '
          f'up to {max(widths)} digits, passed', flush=True)


def main():
    """构建纯 C 动态库，完全绕过 C++ 适配层执行独立数值参考。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--portable', action='store_true')
    parser.add_argument('--seed', type=int, default=519)
    args = parser.parse_args()
    suffix = '.dll' if os.name == 'nt' else '.so'
    variant = 'portable' if args.portable else 'native'
    library = build_shared(ROOT / ('tmp/bigfloat-c-' + variant + suffix), floating=True, portable=args.portable)
    with D.localcontext() as context:
        context.prec = 1800
        kernel = Kernel(library)
        arithmetic(kernel, args.seed)
        large_products(kernel, args.seed)
        mathematics(kernel)


if __name__ == '__main__':
    main()
