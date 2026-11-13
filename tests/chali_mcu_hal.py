"""在宿主验证板级转发，并核对两种 MCU 指令集引用的 ABI 符号。"""
import argparse
from pathlib import Path
import subprocess
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
HAL = ROOT / 'source/aot/mcu'
PROBE = r'''
#include "chali_mcu_hal.h"
#include <assert.h>
#include <stdint.h>

_Alignas(16) static unsigned char storage[16];
static int calls;
#define PORT(name) CAT(PREFIX, name)
#define CAT(a, b) JOIN(a, b)
#define JOIN(a, b) a ## b

size_t PORT(Print)(const void *data, size_t size) {
    assert(size == 3 && ((const char *)data)[1] == 0);
    ++calls;
    return 2;
}
void *PORT(Allocate)(size_t size) {
    ++calls;
    return size == sizeof(storage) ? storage : NULL;
}
void PORT(Free)(void *pointer) {
    assert(pointer == storage);
    ++calls;
}
int main(void) {
    const char message[] = {'a', 0, 'b'};
    assert(chali_mcu_print(NULL, 0) == 0);
    assert(chali_mcu_allocate(0) == NULL);
    chali_mcu_free(NULL);
    assert(calls == 0);
    assert(chali_mcu_print(message, sizeof(message)) == 2);
    assert(chali_mcu_allocate(sizeof(storage) + 1) == NULL);
    void *pointer = chali_mcu_allocate(sizeof(storage));
    assert(pointer == storage && (uintptr_t)pointer % 16 == 0);
    chali_mcu_free(pointer);
    assert(calls == 4);
    return 0;
}
'''


def main():
    """分别验证裸机和 RTOS 的转发及未定义符号。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--clang', default='clang')
    parser.add_argument('--nm', default='llvm-nm')
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix='mcu-hal-') as directory:
        work = Path(directory)
        probe = work / 'probe.c'
        probe.write_text(PROBE, encoding='ascii')
        for rtos in (False, True):
            prefix = 'RTOS_HAL_Chali_' if rtos else 'HAL_Chali_'
            flags = ['-DCHALI_MCU_RTOS'] if rtos else []
            common = [args.clang, '-std=c11', '-Wall', '-Wextra', '-Werror',
                      '-I', str(HAL), *flags, str(HAL / 'chali_mcu_hal.c')]
            exe = work / 'probe.exe'
            subprocess.run([*common, '-DPREFIX=' + prefix, str(probe), '-o', str(exe)], check=True)
            subprocess.run([str(exe)], check=True)
            for target, cpu in [('thumbv7em-none-eabi', ['-mcpu=cortex-m4', '-mfloat-abi=soft']),
                                ('riscv32-unknown-elf', ['-march=rv32imac', '-mabi=ilp32'])]:
                obj = work / 'hal.o'
                subprocess.run([*common, '--target=' + target, *cpu, '-ffreestanding',
                                '-c', '-o', str(obj)], check=True)
                undefined = subprocess.check_output([args.nm, '--undefined-only', str(obj)], text=True)
                symbols = [prefix + name for name in ('Print', 'Allocate', 'Free')]
                assert {line.split()[-1] for line in undefined.splitlines() if line.strip()} == set(symbols), undefined
                thumb = target.startswith('thumb')
                asm = work / 'port.s'
                asm.write_text('.text\n' + ''.join(
                    f'.globl {symbol}\n' + ('.thumb_func\n' if thumb else '') +
                    f'{symbol}:\n  ' + ('bx lr' if thumb else 'ret') + '\n'
                    for symbol in symbols), encoding='ascii')
                firmware = work / 'hal.elf'
                subprocess.run([args.clang, '--target=' + target, *cpu, '-fuse-ld=lld', '-nostdlib',
                                '-Wl,-e,chali_mcu_print', str(obj), str(asm), '-o', str(firmware)], check=True)
                assert not subprocess.check_output([args.nm, '--undefined-only', str(firmware)], text=True).strip()


if __name__ == '__main__':
    main()
