"""用 Python 任意精度整数核对学校、Karatsuba、Toom-Cook、NTT 及自动分派。"""
import argparse
import ctypes as C
import os
import random
from bignum_c_api import BigInt, Buffer, ROOT, bind, build_shared, integer_value


def cases(seed):
    """覆盖进位、符号、平方、二/三路分割和变换补齐的边界。"""
    rng = random.Random(seed)
    widths = [1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17, 23, 24, 25, 31, 32, 33,
              47, 48, 49, 63, 64, 65, 79, 80, 81, 95, 96, 97, 127, 128, 129,
              159, 160, 161, 191, 192, 193, 215, 216, 217, 239, 240, 241,
              255, 256, 257, 383, 384, 385, 511, 512, 513, 767, 768, 769,
              959, 960, 961, 1023, 1024, 1025, 1535, 1536, 1537, 1919, 1920, 1921,
              2047, 2048, 2049, 3071, 3072, 3073, 3839, 3840, 3841, 4095, 4096,
              6144, 7679, 7680, 7681, 8192]
    for a in (0, 1, -1, (1 << 32) - 1, 1 << 32, (1 << 64) - 1):
        for b in (0, 1, -1, (1 << 32) - 1, 1 << 32):
            yield a, b
    for width in widths:
        for pattern in range(9):
            a = ((1 << (width * 32)) - 1) if pattern == 0 else (
                rng.getrandbits(width * 32 - 1) | (1 << (width * 32 - 1)))
            short = width if pattern < 4 else max(1, width // (2, 3, 4, 8, 128)[pattern - 4])
            b = rng.getrandbits(short * 32 - 1) | (1 << (short * 32 - 1))
            if pattern == 2:
                b = a
            if pattern == 3:
                b = -a
            if pattern == 5:
                a = -a
            if pattern == 6:
                b = -b
            yield a, b
    for width in (12288, 15360, 15361, 16384, 24576, 32768):
        a = rng.getrandbits(width * 32 - 1) | (1 << (width * 32 - 1))
        yield a, a
        yield a, rng.getrandbits(width * 32 - 1) | (1 << (width * 32 - 1))


def main():
    """编译纯 C 内核，逐算法核对完整结果和输入/输出别名。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--portable', action='store_true')
    parser.add_argument('--seed', type=int, default=20261105)
    args = parser.parse_args()
    suffix = '.dll' if os.name == 'nt' else '.so'
    variant = 'portable' if args.portable else 'native'
    library = build_shared(ROOT / ('tmp/bigint-multiplication-' + variant + suffix), portable=args.portable, testing=True)
    pointer = C.POINTER(BigInt)
    initialize = bind(library, 'chali_bigint_init', [pointer, C.c_void_p])
    clear = bind(library, 'chali_bigint_clear', [pointer], None)
    resize = bind(library, 'chali_c_buffer_resize', [C.POINTER(Buffer), C.c_size_t])
    multiply = bind(library, 'chali_bigint_test_multiply', [pointer, pointer, pointer, C.c_int])

    def make(number):
        """直接装载独立生成的小端分块，避免把大输入构造时间计入乘法验收。"""
        result = BigInt()
        assert initialize(C.byref(result), None) == 0
        count = (abs(number).bit_length() + 31) // 32
        assert resize(C.byref(result.limbs), count) == 0
        if count:
            raw = abs(number).to_bytes(count * 4, 'little')
            C.memmove(result.limbs.data, raw, len(raw))
        result.sign = (number > 0) - (number < 0)
        return result

    checks = aliases = 0
    for index, (a, b) in enumerate(cases(args.seed)):
        if index % 100 == 0:
            print(f'{variant}: case {index}, {checks} products checked', flush=True)
        expected = a * b
        for route in range(5):
            if route == 1 and max(abs(a).bit_length(), abs(b).bit_length()) > 8192 * 32:
                continue
            left, right, result = make(a), make(b), make(99)
            try:
                status = multiply(C.byref(result), C.byref(left), C.byref(right), route)
                assert status == 0 and integer_value(result) == expected, (index, route, status)
                assert integer_value(left) == a and integer_value(right) == b, (index, route, 'input changed')
                checks += 1
                if index % 7 == 0:
                    status = multiply(C.byref(left), C.byref(left), C.byref(right), route)
                    assert status == 0 and integer_value(left) == expected, (index, route, 'left alias')
                    aliases += 1
                if index % 11 == 0:
                    clear(C.byref(left))
                    left = make(a)
                    status = multiply(C.byref(right), C.byref(left), C.byref(right), route)
                    assert status == 0 and integer_value(right) == expected, (index, route, 'right alias')
                    aliases += 1
                if a == b:
                    clear(C.byref(left))
                    left = make(a)
                    status = multiply(C.byref(left), C.byref(left), C.byref(left), route)
                    assert status == 0 and integer_value(left) == a * a, (index, route, 'self square')
                    aliases += 1
            finally:
                for value in (left, right, result):
                    clear(C.byref(value))
    print(f'C bigint/{variant}: {checks} Python product checks, {aliases} alias and square checks passed')


if __name__ == '__main__':
    main()
