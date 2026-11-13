"""下载固定 FreeRTOS 内核，通过 ChaLi BUILD 生成并模拟 Cortex-M4 固件。"""
import argparse
import hashlib
from pathlib import Path
import subprocess
import sys
import tarfile
import urllib.request

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
BOARD = ROOT / "tests/mcu_freertos"
RUNTIME = ROOT / "source/aot/mcu"
TAG = "V11.2.0"
SHA256 = "e148d3a442ac7652c998e0a932c4cc2a3c6abff66a3a0a96a7678c0cd092d41c"


def kernel_source(cache):
    """校验下载归档并解包到构建缓存。"""
    archive = cache / "kernel.tar.gz"
    if not archive.is_file():
        url = f"https://codeload.github.com/FreeRTOS/FreeRTOS-Kernel/tar.gz/refs/tags/{TAG}"
        with urllib.request.urlopen(url, timeout=60) as response:
            payload = response.read()
        if hashlib.sha256(payload).hexdigest() != SHA256:
            raise ValueError("FreeRTOS download checksum mismatch")
        archive.write_bytes(payload)
    if hashlib.sha256(archive.read_bytes()).hexdigest() != SHA256:
        raise ValueError("FreeRTOS cache checksum mismatch")
    with tarfile.open(archive) as source:
        source.extractall(cache, filter="data")
    return cache / f"FreeRTOS-Kernel-{TAG[1:]}"


def main():
    """构建板级代码与内核，运行调度器并核对任务输出和内存归还。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--chali", type=Path, required=True)
    parser.add_argument("--clang", type=Path, required=True)
    parser.add_argument("--qemu", nargs="+", default=["qemu-system-arm"])
    args = parser.parse_args()
    cache = ROOT / "build/mcu-freertos"
    cache.mkdir(parents=True, exist_ok=True)
    kernel = kernel_source(cache)
    port = kernel / "portable/GCC/ARM_CM3"
    with tempfile.TemporaryDirectory(prefix="freertos-build-") as directory:
        work = Path(directory)
        flags = ["--target=thumbv7em-none-eabi", "-mcpu=cortex-m4", "-mfloat-abi=soft",
                 "-O2", "-ffreestanding", "-fno-builtin", "-ffunction-sections", "-fdata-sections",
                 "-DCHALI_MCU_RTOS"]
        includes = [f"-I{path}" for path in (BOARD, RUNTIME, kernel / "include", port)]
        sources = [BOARD / name for name in ("board.c", "vectors.s", "memory.c")]
        sources += [kernel / name for name in ("tasks.c", "list.c", "queue.c", "portable/MemMang/heap_4.c")]
        sources += [port / "port.c", *sorted(RUNTIME.glob("chali_mcu_*.c"))]
        objects = []
        for index, source in enumerate(sources):
            obj = work / f"{index}.o"
            subprocess.run([str(args.clang.resolve()), *flags, *includes, "-c", str(source), "-o", str(obj)], check=True)
            objects += ["--link", str(obj)]
        prefix = cache / "chali-freertos-cortex-m4"
        subprocess.run([str(args.chali.resolve()), "build", str(BOARD / "task.chl"),
                        "--target", "mcu-cortex-m4", "--llvm", str(args.clang.resolve()),
                        "--link", str(BOARD / "board.ld"), *objects, "--out", str(prefix)], check=True)
        elf = prefix.with_suffix(".elf")
        image = elf
        if args.qemu[0] == "wsl":
            image = subprocess.check_output(["wsl", "--exec", "wslpath", "-a", str(elf)], text=True).strip()
        subprocess.run([*args.qemu, "-M", "mps2-an386", "-nographic", "-semihosting",
                        "-kernel", str(image)], timeout=20, check=True)
        print("FreeRTOS Cortex-M4: ChaLi task printed and returned all HAL allocations")


if __name__ == "__main__":
    main()
