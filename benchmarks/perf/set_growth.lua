-- set_growth：离散整数键 table；Set 用 true 表示存在；保持默认 GC。
local N,REPS=20000,5
local function key(index,seed) return (index*65537+seed*17)%1000000007+1000000007 end
local function work(seed)
    local values={}
    local total=seed
    for i=0,N-1 do values[key(i,seed)]=true end
    for i=0,N-1 do values[key(i,seed)]=true end
    for i=0,N-1 do
        local k=key(i,seed)
        total=(total*31+(values[k]~=nil and 1 or 0)+2*(values[-k]~=nil and 1 or 0))&2147483647
    end
    for i=0,N-1,3 do
        local k=key(i,seed)
        total=total+(values[k]~=nil and 1 or 0)
        values[k]=nil
    end
    for i=0,N-1,3 do values[key(i,seed)]=true end
    for i=0,N-1 do total=(total*31+(values[key(i,seed)]~=nil and 1 or 0))&2147483647 end
    return total
end

local sink = work(0)
for r = 0, REPS - 1 do
    local start = now_ns()
    sink = sink + work(r)
    io.write(string.format("rep %d\n", now_ns() - start))
end
io.write(string.format("sink %d\n", sink))
