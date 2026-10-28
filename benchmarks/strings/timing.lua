setup()
if VERIFY then
    for seed = 0, 4 do print("value " .. seed .. " " .. work(seed)) end
    return
end
local sink = work(0)
for seed = 0, 4 do
    local start = now_ns()
    local value = work(seed)
    local elapsed = now_ns() - start
    sink = sink + value
    print("rep " .. elapsed)
end
print("sink " .. sink)
