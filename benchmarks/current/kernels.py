"""当前跨语言对照的 Python 实现：整数按相同掩码归并，字符串按 UTF-8 字节处理。"""
from dataclasses import dataclass
import sys
import time

MASK = 2147483647
UNIT = 'ChaLi-字符串-🙂-0123456789|'.encode()
TEXT = b''.join(UNIT + bytes([65 + i % 26]) for i in range(256))
FIRST, SECOND = UNIT + b'a', UNIT + b'B'
WIDTH = len(FIRST)


@dataclass
class Vector3:
    x: int
    y: int
    z: int


@dataclass
class Transform:
    position: Vector3
    velocity: Vector3
    weights: list


@dataclass
class Record:
    transform: Transform
    payload: list
    flags: int


RECORDS = [Record(Transform(Vector3(i*3+1, i*5+2, i*7+3), Vector3(i+4, i+5, i+6),
                            [i*13+k for k in range(4)]), [i*17+k*3 for k in range(8)], i*11+9)
           for i in range(256)]
EVENTS = [(i % 4, () if i % 4 == 0 else (i*3+7,) if i % 4 == 1
           else (i*5, i+11) if i % 4 == 2 else (i*7, i+13)) for i in range(256)]
MATRIX = [[r*131+c*17+3 for c in range(64)] for r in range(64)]
CUBE = [[[r*257+c*31+d*7+3 for d in range(16)] for c in range(16)] for r in range(16)]


class Multiply:
    """单态和多态接收者中的乘加实现。"""
    def apply(self, value): return (value*3+7) & MASK


class Subtract:
    """多态接收者中的乘减实现。"""
    def apply(self, value): return (value*5-11) & MASK


class ExclusiveOr:
    """多态接收者中的乘法与异或实现。"""
    def apply(self, value): return ((value*7)^13) & MASK


OPS = [Multiply(), Subtract(), ExclusiveOr(), Multiply()]


def key(i, seed): return (i*65537+seed*17) % 1000000007+1000000007
def mix(value): return value*17+3


def work(case, seed):
    """按与其他语言一致的输入规模、操作序列与初值计算。"""
    acc = seed
    if case == 'int_chain':
        for i in range(4000000): acc = (acc*48271+i) & MASK
    elif case == 'float_chain':
        acc = float(seed)
        for _ in range(1000000): acc = acc*1.0000001+0.0000001
    elif case == 'call':
        for i in range(1000000): acc = (mix(acc)+i) & MASK
    elif case in ('array', 'array_rw'):
        a = [k*3 for k in range(16)]
        if case == 'array':
            for _ in range(4000000): acc = (acc+a[acc & 15]) & MASK
        else:
            for i in range(4000000):
                at = acc & 15
                a[at] = (a[at]+i) & MASK
                acc = (acc+a[at]) & MASK
    elif case in ('list', 'list_growth'):
        a = []
        for i in range(400000): a.append(i*3+seed)
        for value in a: acc = (acc*31+value) & MASK
    elif case == 'map_growth':
        a = {}
        for i in range(20000): a[key(i,seed)] = i*3+seed
        for i in range(20000):
            k = key(i,seed); acc = (acc*31+a[k]) & MASK; a[k] = i*5+seed
        for i in range(20000):
            k = key(i,seed); acc = (acc*31+a[k]+(-k in a)) & MASK
        for i in range(0,20000,3): del a[key(i,seed)]; acc += 1
        for i in range(0,20000,3): a[key(i,seed)] = i*7+seed
        for i in range(20000): acc = (acc*31+a[key(i,seed)]) & MASK
    elif case == 'set_growth':
        a = set()
        for i in range(20000): a.add(key(i,seed))
        for i in range(20000): a.add(key(i,seed))
        for i in range(20000):
            k = key(i,seed); acc = (acc*31+(k in a)+2*(-k in a)) & MASK
        for i in range(0,20000,3): a.remove(key(i,seed)); acc += 1
        for i in range(0,20000,3): a.add(key(i,seed))
        for i in range(20000): acc = (acc*31+(key(i,seed) in a)) & MASK
    elif case == 'enum_payload':
        for i in range(262144):
            tag, payload = EVENTS[(i*13+((acc >> 7) & 255)) & 255]
            value = 17 if tag == 0 else payload[0] if tag == 1 else payload[0]+payload[1]
            acc = (acc*33+value) & MASK
    elif case in ('interface_mono','interface_poly'):
        for i in range(262144):
            operation = OPS[0] if case == 'interface_mono' else OPS[(i+seed) & 3]
            acc = operation.apply(acc)
    elif case == 'struct_nested':
        for i in range(262144):
            row = RECORDS[(i*13+(acc & 255)) & 255]
            t = row.transform
            value = t.position.x+t.position.z+t.velocity.y+t.weights[i & 3]+row.payload[(i+seed) & 7]+row.flags
            acc = (acc*33+value) & MASK
    elif case == 'array_2d':
        for i in range(262144): acc = (acc*33+MATRIX[(acc >> 6) & 63][(i+acc) & 63]) & MASK
    elif case == 'array_3d':
        for i in range(262144): acc = (acc*33+CUBE[(acc >> 4) & 15][(acc >> 8) & 15][(i+seed) & 15]) & MASK
    elif case == 'string_read':
        for i in range(131072): acc = (acc*33+TEXT[(i*13+((acc >> 7) & 65535)) % len(TEXT)]) & MASK
    elif case == 'string_concat':
        result = b''
        for i in range(2048): result = result+(FIRST if ((i+seed) & 1) == 0 else SECOND)
        for value in result: acc = (acc*33+value) & MASK
    elif case == 'string_slice':
        for i in range(1024):
            block = (i*13+(acc & 127)) & 127
            start, width = block*WIDTH, (((i+seed) & 3)+1)*WIDTH
            other = start if (i & 1) == 0 else ((block+13) & 127)*WIDTH
            part, compared = TEXT[start:start+width], TEXT[other:other+width]
            acc = (acc*33+part[0]+part[-1]+(part == compared)) & MASK
    elif case == 'string_find':
        patterns = ['字符串'.encode(), '🙂'.encode(), '不存在!'.encode(), b'012345']
        for i in range(256):
            start = (((acc >> 7)+i) & 255)*WIDTH
            found = TEXT.find(patterns[(i+seed) & 3], start)
            acc = (acc*33+found+1) & MASK
    else: raise ValueError(case)
    return acc


def main():
    """构造不计时；校验模式输出五个种子，正式模式预热后记录五个独立计时区间。"""
    case = sys.argv[1]
    if '--verify' in sys.argv:
        for seed in range(5): print('value', seed, format(work(case,seed), '.17g'))
        return
    sink = work(case,0)
    for seed in range(5):
        start = time.perf_counter_ns(); value = work(case,seed); elapsed = time.perf_counter_ns()-start
        sink = sink*0.5+value if case == 'float_chain' else sink+value
        print('rep', elapsed)
    print('sink', format(sink,'.17g'))


if __name__ == '__main__': main()
