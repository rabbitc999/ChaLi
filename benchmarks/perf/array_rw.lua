-- 固定数组读写：与 ChaLi/C 使用同样的 4,000,000 轮和数据相关下标。
local N, REPS = 4000000, 5
local function work(seed)
    local values = {}
    for k = 0, 15 do
        values[k + 1] = k * 3
    end
    local acc = seed
    for i = 0, N - 1 do
        local index = (acc & 15) + 1
        values[index] = (values[index] + i) & 2147483647
        acc = (acc + values[index]) & 2147483647
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
