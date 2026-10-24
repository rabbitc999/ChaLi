-- 整数依赖链：与其他语言相同的 4,000,000 轮乘加及掩码。
local N, REPS = 4000000, 5
local function work(seed)
    local acc = seed
    for i = 0, N - 1 do
        acc = (acc * 48271 + i) & 2147483647
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
