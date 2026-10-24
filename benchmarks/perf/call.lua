-- Lua 函数调用：1,000,000 轮，与其他配置使用相同函数和递推关系。
local N, REPS = 1000000, 5
local function mix(value)
    return value * 17 + 3
end

local function work(seed)
    local acc = seed
    for i = 0, N - 1 do
        acc = (mix(acc) + i) & 2147483647
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
