-- 固定 16 个元素，Lua table 使用一基索引；4,000,000 轮数据相关读取。
local N, REPS = 4000000, 5
local function work(seed)
    local values = {}
    for k = 0, 15 do
        values[k + 1] = k * 3
    end
    local acc = seed
    for _ = 1, N do
        acc = (acc + values[(acc & 15) + 1]) & 2147483647
    end
    return acc
end

local sink = work(0)
for r = 0, REPS - 1 do
    local start = now_ns()
    sink = sink + work(r)
    local elapsed = now_ns() - start
    io.write(string.format("rep %d\n", elapsed))
end
io.write(string.format("sink %d\n", sink))
