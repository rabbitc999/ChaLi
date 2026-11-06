-- Lua 的结构、载荷和方法调用对照；全局数据在计时前构造。
local MASK=2147483647
local records,events,matrix,cube={},{},{},{}
local multiply={apply=function(self,v)return(v*3+7)&MASK end}
local subtract={apply=function(self,v)return(v*5-11)&MASK end}
local exclusive={apply=function(self,v)return((v*7)~13)&MASK end}
local operations={multiply,subtract,exclusive,multiply}
for i=0,255 do
 local tag=i%4
 events[i+1]={tag=tag,x=tag==1 and i*3+7 or tag==2 and i*5 or tag==3 and i*7 or 0,y=tag==2 and i+11 or tag==3 and i+13 or 0}
 local weights,payload={},{}
 for k=0,3 do weights[k+1]=i*13+k end
 for k=0,7 do payload[k+1]=i*17+k*3 end
 records[i+1]={transform={position={x=i*3+1,y=i*5+2,z=i*7+3},velocity={x=i+4,y=i+5,z=i+6},weights=weights},payload=payload,flags=i*11+9}
end
for r=0,63 do matrix[r+1]={};for c=0,63 do matrix[r+1][c+1]=r*131+c*17+3 end end
for x=0,15 do cube[x+1]={};for y=0,15 do cube[x+1][y+1]={};for z=0,15 do cube[x+1][y+1][z+1]=x*257+y*31+z*7+3 end end end
local kernels={}
kernels.enum_payload=function(seed)
 local a=seed;for i=0,262143 do local e=events[((i*13+((a>>7)&255))&255)+1];local v=e.tag==0 and 17 or e.x+e.y;a=(a*33+v)&MASK end;return a
end
kernels.interface_mono=function(seed)
 local a=seed;for i=0,262143 do a=operations[1]:apply(a) end;return a
end
kernels.interface_poly=function(seed)
 local a=seed;for i=0,262143 do a=operations[((i+seed)&3)+1]:apply(a) end;return a
end
kernels.struct_nested=function(seed)
 local a=seed;for i=0,262143 do local r=records[((i*13+(a&255))&255)+1];local t=r.transform;local v=t.position.x+t.position.z+t.velocity.y+t.weights[(i&3)+1]+r.payload[((i+seed)&7)+1]+r.flags;a=(a*33+v)&MASK end;return a
end
kernels.array_2d=function(seed)
 local a=seed;for i=0,262143 do a=(a*33+matrix[((a>>6)&63)+1][((i+a)&63)+1])&MASK end;return a
end
kernels.array_3d=function(seed)
 local a=seed;for i=0,262143 do a=(a*33+cube[((a>>4)&15)+1][((a>>8)&15)+1][((i+seed)&15)+1])&MASK end;return a
end
work=assert(kernels[BENCH_CASE])
