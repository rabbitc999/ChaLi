"""持续宿主线程跨模块轮次使用数据／堆视图，零次循环不提前访问运行时。"""
import ctypes
from pathlib import Path
import queue
import sys
import threading

library=ctypes.CDLL(str(Path(sys.argv[1]).resolve()))
for name in ('dataSum','recordSum','heapSum'):
    fn=getattr(library,'f_'+name);fn.argtypes=[ctypes.c_int64];fn.restype=ctypes.c_int64
library.f_pointerSum.argtypes=[ctypes.c_int64,ctypes.c_int64];library.f_pointerSum.restype=ctypes.c_int64
for name in ('matrixAddress','heapAddress'):
    getattr(library,'f_'+name).restype=ctypes.c_int64
library.f_setup.restype=ctypes.c_int64
library.chali_init.restype=None;library.chali_finish.restype=None
commands=queue.Queue();results=queue.Queue()
def worker():
    while True:
        command=commands.get()
        if command=='stop':return
        try:
            if command=='zero':
                assert library.f_dataSum(0)==0
                assert library.f_recordSum(0)==0
            else:
                # 普通宿主线程不共享初始化线程的语言全局 TLS；显式传入逻辑地址。
                assert library.f_pointerSum(command[0],3)==6
                assert library.f_pointerSum(command[1],6)==30
            results.put(None)
        except BaseException as error:results.put(repr(error))
thread=threading.Thread(target=worker);thread.start()
def check(command):
    commands.put(command);error=results.get(timeout=30);assert error is None,error
try:
    check('zero')
    for cycle in range(4):
        library.chali_init();library.f_setup()
        assert library.f_dataSum(6)==42
        assert library.f_recordSum(4)==112
        assert library.f_heapSum(6)==30
        check((library.f_matrixAddress(),library.f_heapAddress()))
        library.chali_finish()
        check('zero')
finally:
    commands.put('stop');thread.join(timeout=30);assert not thread.is_alive()
print('persistent host thread: zero loops and four init/finish cycles PASS')
