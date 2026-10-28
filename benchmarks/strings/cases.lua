local DATA = {}
local MASK = 2147483647
function setup()
    local unit = "ChaLi-字符串-🙂-0123456789|"
    DATA.first, DATA.second = unit .. "a", unit .. "B"
    DATA.width = #DATA.first
    local pieces = {}
    for index = 0, 255 do pieces[#pieces + 1] = unit .. string.char(65 + index % 26) end
    DATA.text = table.concat(pieces)
end

local function read_work(seed)
    local source, length = DATA.text, #DATA.text
    for index = 0, 131071 do
        local at = (index * 13 + ((seed >> 7) & 65535)) % length
        seed = (seed * 33 + source:byte(at + 1)) & MASK
    end
    return seed
end

local function concat_work(seed)
    local result = ""
    for index = 0, 2047 do
        local chunk = ((index + seed) & 1) == 0 and DATA.first or DATA.second
        result = result .. chunk
    end
    local acc = seed
    for index = 1, #result do acc = (acc * 33 + result:byte(index)) & MASK end
    return acc
end

local function concat_builder_work(seed)
    local pieces = {}
    for index = 0, 2047 do
        pieces[#pieces + 1] = ((index + seed) & 1) == 0 and DATA.first or DATA.second
    end
    local result = table.concat(pieces)
    local acc = seed
    for index = 1, #result do acc = (acc * 33 + result:byte(index)) & MASK end
    return acc
end

local function slice_work(seed)
    local source = DATA.text
    local initial = seed
    for index = 0, 1023 do
        local block = (index * 13 + (seed & 127)) & 127
        local start = block * DATA.width
        local width = (((index + initial) & 3) + 1) * DATA.width
        local other = (index & 1) == 0 and start or (((block + 13) & 127) * DATA.width)
        local part, compared = source:sub(start + 1, start + width), source:sub(other + 1, other + width)
        seed = (seed * 33 + part:byte(1) + part:byte(#part) + (part == compared and 1 or 0)) & MASK
    end
    return seed
end

local function find_work(seed)
    local patterns = {"字符串", "🙂", "不存在!", "012345"}
    local initial = seed
    for index = 0, 255 do
        local start = (((seed >> 7) + index) & 255) * DATA.width
        local found = DATA.text:find(patterns[((index + initial) & 3) + 1], start + 1, true)
        local position = found and found - 1 or -1
        seed = (seed * 33 + position + 1) & MASK
    end
    return seed
end

work = assert(({string_read=read_work, string_concat=concat_work, string_concat_builder=concat_builder_work,
               string_slice=slice_work, string_find=find_work})[BENCH_CASE])
