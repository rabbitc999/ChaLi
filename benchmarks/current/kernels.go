// UTF-8 字节任务和具名结构、接口的 Go 对照。
package main

import (
	"bytes"
	"fmt"
	"os"
)

const mask int64 = 2147483647

type Vec struct{ x, y, z int64 }
type Transform struct {
	position, velocity Vec
	weights            [4]int64
}
type Record struct {
	transform Transform
	payload   [8]int64
	flags     int64
}
type Event struct {
	tag  int
	x, y int64
}
type Operation interface{ apply(int64) int64 }
type Multiply struct{}
type Subtract struct{}
type ExclusiveOr struct{}

func (Multiply) apply(v int64) int64    { return (v*3 + 7) & mask }
func (Subtract) apply(v int64) int64    { return (v*5 - 11) & mask }
func (ExclusiveOr) apply(v int64) int64 { return ((v * 7) ^ 13) & mask }

var ops = [4]Operation{Multiply{}, Subtract{}, ExclusiveOr{}, Multiply{}}
var events [256]Event
var records [256]Record
var matrix [64][64]int64
var cube [16][16][16]int64
var unit = []byte("ChaLi-字符串-🙂-0123456789|")
var text, first, second []byte

func setup() {
	first = append(append([]byte{}, unit...), 'a')
	second = append(append([]byte{}, unit...), 'B')
	for i := int64(0); i < 256; i++ {
		text = append(text, unit...)
		text = append(text, byte(65+i%26))
		e := Event{tag: int(i % 4)}
		if e.tag == 1 {
			e.x = i*3 + 7
		} else if e.tag == 2 {
			e.x = i * 5
			e.y = i + 11
		} else if e.tag == 3 {
			e.x = i * 7
			e.y = i + 13
		}
		events[i] = e
		t := &records[i].transform
		t.position = Vec{i*3 + 1, i*5 + 2, i*7 + 3}
		t.velocity = Vec{i + 4, i + 5, i + 6}
		for k := 0; k < 4; k++ {
			t.weights[k] = i*13 + int64(k)
		}
		for k := 0; k < 8; k++ {
			records[i].payload[k] = i*17 + int64(k)*3
		}
		records[i].flags = i*11 + 9
	}
	for r := 0; r < 64; r++ {
		for c := 0; c < 64; c++ {
			matrix[r][c] = int64(r*131 + c*17 + 3)
		}
	}
	for x := 0; x < 16; x++ {
		for y := 0; y < 16; y++ {
			for z := 0; z < 16; z++ {
				cube[x][y][z] = int64(x*257 + y*31 + z*7 + 3)
			}
		}
	}
}
func key(i, s int64) int64 { return (i*65537+s*17)%1000000007 + 1000000007 }
func mix(v int64) int64    { return v*17 + 3 }
func work(name string, s int64) int64 {
	a := s
	switch name {
	case "int_chain":
		for i := int64(0); i < 4000000; i++ {
			a = (a*48271 + i) & mask
		}
	case "call":
		for i := int64(0); i < 1000000; i++ {
			a = (mix(a) + i) & mask
		}
	case "array", "array_rw":
		var v [16]int64
		for k := 0; k < 16; k++ {
			v[k] = int64(k * 3)
		}
		if name == "array" {
			for i := 0; i < 4000000; i++ {
				a = (a + v[a&15]) & mask
			}
		} else {
			for i := int64(0); i < 4000000; i++ {
				k := a & 15
				v[k] = (v[k] + i) & mask
				a = (a + v[k]) & mask
			}
		}
	case "list", "list_growth":
		v := []int64{}
		for i := int64(0); i < 400000; i++ {
			v = append(v, i*3+s)
		}
		for _, x := range v {
			a = (a*31 + x) & mask
		}
	case "map_growth":
		v := map[int64]int64{}
		for i := int64(0); i < 20000; i++ {
			v[key(i, s)] = i*3 + s
		}
		for i := int64(0); i < 20000; i++ {
			k := key(i, s)
			a = (a*31 + v[k]) & mask
			v[k] = i*5 + s
		}
		for i := int64(0); i < 20000; i++ {
			k := key(i, s)
			miss := int64(0)
			if _, ok := v[-k]; ok {
				miss = 1
			}
			a = (a*31 + v[k] + miss) & mask
		}
		for i := int64(0); i < 20000; i += 3 {
			delete(v, key(i, s))
			a++
		}
		for i := int64(0); i < 20000; i += 3 {
			v[key(i, s)] = i*7 + s
		}
		for i := int64(0); i < 20000; i++ {
			a = (a*31 + v[key(i, s)]) & mask
		}
	case "set_growth":
		v := map[int64]struct{}{}
		for j := 0; j < 2; j++ {
			for i := int64(0); i < 20000; i++ {
				v[key(i, s)] = struct{}{}
			}
		}
		for i := int64(0); i < 20000; i++ {
			k := key(i, s)
			hit, miss := int64(0), int64(0)
			if _, ok := v[k]; ok {
				hit = 1
			}
			if _, ok := v[-k]; ok {
				miss = 2
			}
			a = (a*31 + hit + miss) & mask
		}
		for i := int64(0); i < 20000; i += 3 {
			delete(v, key(i, s))
			a++
		}
		for i := int64(0); i < 20000; i += 3 {
			v[key(i, s)] = struct{}{}
		}
		for i := int64(0); i < 20000; i++ {
			hit := int64(0)
			if _, ok := v[key(i, s)]; ok {
				hit = 1
			}
			a = (a*31 + hit) & mask
		}
	case "enum_payload":
		for i := int64(0); i < 262144; i++ {
			e := &events[(i*13+((a>>7)&255))&255]
			v := e.x + e.y
			if e.tag == 0 {
				v = 17
			}
			a = (a*33 + v) & mask
		}
	case "interface_mono":
		for i := 0; i < 262144; i++ {
			a = ops[0].apply(a)
		}
	case "interface_poly":
		for i := int64(0); i < 262144; i++ {
			a = ops[(i+s)&3].apply(a)
		}
	case "struct_nested":
		for i := int64(0); i < 262144; i++ {
			r := &records[(i*13+(a&255))&255]
			t := &r.transform
			v := t.position.x + t.position.z + t.velocity.y + t.weights[i&3] + r.payload[(i+s)&7] + r.flags
			a = (a*33 + v) & mask
		}
	case "array_2d":
		for i := int64(0); i < 262144; i++ {
			a = (a*33 + matrix[(a>>6)&63][(i+a)&63]) & mask
		}
	case "array_3d":
		for i := int64(0); i < 262144; i++ {
			a = (a*33 + cube[(a>>4)&15][(a>>8)&15][(i+s)&15]) & mask
		}
	case "string_read":
		for i := int64(0); i < 131072; i++ {
			a = (a*33 + int64(text[(i*13+((a>>7)&65535))%int64(len(text))])) & mask
		}
	case "string_concat":
		v := []byte{}
		for i := int64(0); i < 2048; i++ {
			chunk := first
			if (i+s)&1 != 0 {
				chunk = second
			}
			next := make([]byte, len(v)+len(chunk))
			copy(next, v)
			copy(next[len(v):], chunk)
			v = next
		}
		for _, b := range v {
			a = (a*33 + int64(b)) & mask
		}
	case "string_slice":
		for i := int64(0); i < 1024; i++ {
			block := (i*13 + (a & 127)) & 127
			start, width := block*int64(len(first)), (((i+s)&3)+1)*int64(len(first))
			other := start
			if i&1 != 0 {
				other = ((block + 13) & 127) * int64(len(first))
			}
			p := bytes.Clone(text[start : start+width])
			q := bytes.Clone(text[other : other+width])
			equal := int64(0)
			if bytes.Equal(p, q) {
				equal = 1
			}
			a = (a*33 + int64(p[0]) + int64(p[len(p)-1]) + equal) & mask
		}
	case "string_find":
		patterns := [][]byte{[]byte("字符串"), []byte("🙂"), []byte("不存在!"), []byte("012345")}
		for i := int64(0); i < 256; i++ {
			start := (((a >> 7) + i) & 255) * int64(len(first))
			v := bytes.Index(text[start:], patterns[(i+s)&3])
			found := int64(v)
			if v >= 0 {
				found += start
			}
			a = (a*33 + found + 1) & mask
		}
	default:
		panic(name)
	}
	return a
}
func floating(s int64) float64 {
	x := float64(s)
	for i := 0; i < 1000000; i++ {
		x = x*1.0000001 + 0.0000001
	}
	return x
}
func main() {
	setup()
	name := os.Args[1]
	verify := len(os.Args) > 2 && os.Args[2] == "--verify"
	if name == "float_chain" {
		if verify {
			for s := int64(0); s < 5; s++ {
				fmt.Printf("value %d %.17g\n", s, floating(s))
			}
			return
		}
		sink := floating(0)
		for s := int64(0); s < 5; s++ {
			start := nowNanos()
			v := floating(s)
			elapsed := nowNanos() - start
			sink = sink*0.5 + v
			fmt.Println("rep", elapsed)
		}
		fmt.Printf("sink %.17g\n", sink)
		return
	}
	if verify {
		for s := int64(0); s < 5; s++ {
			fmt.Println("value", s, work(name, s))
		}
		return
	}
	sink := work(name, 0)
	for s := int64(0); s < 5; s++ {
		start := nowNanos()
		v := work(name, s)
		elapsed := nowNanos() - start
		sink += v
		fmt.Println("rep", elapsed)
	}
	fmt.Println("sink", sink)
}
