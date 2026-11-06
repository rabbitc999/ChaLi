//go:build !windows

package main

import "time"

var clockOrigin = time.Now()

func nowNanos() int64 { return time.Since(clockOrigin).Nanoseconds() }
