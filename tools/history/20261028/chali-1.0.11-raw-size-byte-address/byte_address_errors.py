"""以独立宿主进程触发空、未知和已释放逻辑地址的原始诊断。"""
import ctypes
from pathlib import Path
import sys
x=ctypes.CDLL(str(Path(sys.argv[1]).resolve()))
x.chali_init.restype=None;x.chali_init()
x.f_byteRead.argtypes=[ctypes.c_int64,ctypes.c_int64];x.f_byteRead.restype=ctypes.c_int64
x.f_staleAddress.restype=ctypes.c_int64
if sys.argv[2]=='zero-offset':
    print(x.f_byteRead(0,1));x.chali_finish();sys.exit(0)
address={'null':lambda:0,'unknown':lambda:9223372036854775807,'released':lambda:x.f_staleAddress()}[sys.argv[2]]()
x.f_byteRead(address,0)
raise AssertionError('invalid byte read unexpectedly returned')
