-- 双精度依赖链：1,000,000 轮，保留原始运算次序。
local N, REPS = 1000000, 5
local function work(seed)
    local value = seed + 0.0
    for _ = 1, N do
        value = value * 1.0000001 + 0.0000001
    end
    return value
end

local sink = work(0)
for r = 0, REPS - 1 do
    local start = now_ns()
    sink = sink * 0.5 + work(r)
    local elapsed = now_ns() - start
    io.write(string.format("rep %d\n", elapsed))
end
io.write(string.format("sink %.9f\n", sink))
