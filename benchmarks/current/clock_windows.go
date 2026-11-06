//go:build windows

package main

import (
	"syscall"
	"unsafe"
)

var qpc = syscall.NewLazyDLL("kernel32.dll").NewProc("QueryPerformanceCounter")
var qpf = syscall.NewLazyDLL("kernel32.dll").NewProc("QueryPerformanceFrequency")
var qpcFrequency = func() int64 {
	var value int64
	result, _, _ := qpf.Call(uintptr(unsafe.Pointer(&value)))
	if result == 0 || value <= 0 {
		panic("QPC frequency unavailable")
	}
	return value
}()

// nowNanos 使用与其他 Windows 计时壳同源的单调 QPC。
func nowNanos() int64 {
	var ticks int64
	result, _, _ := qpc.Call(uintptr(unsafe.Pointer(&ticks)))
	if result == 0 {
		panic("QPC unavailable")
	}
	return ticks/qpcFrequency*1000000000 + (ticks%qpcFrequency)*1000000000/qpcFrequency
}
