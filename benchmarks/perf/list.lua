-- 动态 table 追加 400,000 个元素后遍历；保持官方 VM 默认垃圾回收。
local N, REPS = 400000, 5
local function work(seed)
    local values = {}
    for i = 0, N - 1 do
        values[#values + 1] = i * 3 + seed
    end
    local total = seed
    for i = 1, #values do
        total = (total * 31 + values[i]) & 2147483647
    end
    return total
end

local sink = work(0)
for r = 0, REPS - 1 do
    local start = now_ns()
    sink = sink + work(r)
    local elapsed = now_ns() - start
    io.write(string.format("rep %d\n", elapsed))
end
io.write(string.format("sink %d\n", sink))
