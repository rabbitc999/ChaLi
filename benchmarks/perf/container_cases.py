"""默认容器的同算法负载；按语言生成可独立运行的计时程序。"""

N = 50000
WIDTH = 1024
REPS = 5


def loop(language, variable, limit, body):
    """生成当前语言的有界循环，ChaLi 使用权威文法支持的 while。"""
    if language == 'chali':
        return '{ int64 ' + variable + ' = 0; while (' + variable + ' < ' + str(limit) + ') { ' + body + ' ' + variable + ' = ' + variable + ' + 1; } }'
    if language == 'cpp':
        return 'for (std::int64_t ' + variable + ' = 0; ' + variable + ' < ' + str(limit) + '; ++' + variable + ') { ' + body + ' }'
    if language == 'python':
        return 'for ' + variable + ' in range(' + str(limit) + '):\n    ' + body.replace('\n', '\n    ')
    return 'for ' + variable + ' = 0, ' + str(limit) + ' - 1 do ' + body + ' end'


def kernels(language):
    """给出四种操作模式，保留各语言默认容器及默认扩容行为。"""
    if language == 'chali':
        declarations = ['List<int64> values;', 'Map<int64, int64> values;', 'Set<int64> values;']
        append, list_read, list_write, pop = 'values.push(i + seed);', 'values[i % 1024]', 'values[i % 1024] = i + seed;', 'values.pop()'
        put, map_read, update = 'values.put(i, i + seed);', 'values[i % 1024]', 'values.put(i % 1024, i + seed);'
        erase, absent = 'values.remove(i);', 'if (values.has(i) == 0) { total = total + 1; }'
        add, has, length = 'values.add(i);', 'if (values.has(i % 2048) != 0) { total = total + 1; }', 'values.len()'
        duplicate, total, accumulate = 'values.add(i % 1024);', 'int64 total = 0;', lambda value: 'total = total + ' + value + ';'
        returned = lambda value: 'return ' + value + ';'
    elif language == 'cpp':
        declarations = ['std::vector<std::int64_t> values;', 'std::unordered_map<std::int64_t, std::int64_t> values;', 'std::unordered_set<std::int64_t> values;']
        append, list_read, list_write, pop = 'values.push_back(i + seed);', 'values.at(i % 1024)', 'values.at(i % 1024) = i + seed;', 'values.back()'
        put, map_read, update = 'values[i] = i + seed;', 'values.at(i % 1024)', 'values[i % 1024] = i + seed;'
        erase, absent = 'values.erase(i);', 'if (values.count(i) == 0) { ++total; }'
        add, has, length = 'values.insert(i);', 'if (values.count(i % 2048) != 0) { ++total; }', 'static_cast<std::int64_t>(values.size())'
        duplicate, total, accumulate = 'values.insert(i % 1024);', 'std::int64_t total = 0;', lambda value: 'total += ' + value + ';'
        returned = lambda value: 'return ' + value + ';'
    elif language == 'python':
        declarations = ['values = []', 'values = {}', 'values = set()']
        append, list_read, list_write, pop = 'values.append(i + seed)', 'values[i % 1024]', 'values[i % 1024] = i + seed', 'values.pop()'
        put, map_read, update = 'values[i] = i + seed', 'values[i % 1024]', 'values[i % 1024] = i + seed'
        erase, absent = 'values.pop(i)', 'if i not in values:\n    total += 1'
        add, has, length = 'values.add(i)', 'if i % 2048 in values:\n    total += 1', 'len(values)'
        duplicate, total, accumulate = 'values.add(i % 1024)', 'total = 0', lambda value: 'total += ' + value
        returned = lambda value: 'return ' + value
    else:
        declarations = ['local values = {}'] * 3
        append, list_read, list_write, pop = 'values[#values + 1] = i + seed', 'values[i % 1024 + 1]', 'values[i % 1024 + 1] = i + seed', 'table.remove(values)'
        put, map_read, update = 'values[i] = i + seed', 'values[i % 1024]', 'values[i % 1024] = i + seed'
        erase, absent = 'values[i] = nil', 'if values[i] == nil then total = total + 1 end'
        add, has, length = 'values[i] = true', 'if values[i % 2048] then total = total + 1 end', 'count'
        duplicate, total, accumulate = 'if not values[i % 1024] then values[i % 1024] = true; count = count + 1 end', 'local total = 0', lambda value: 'total = total + ' + value
        returned = lambda value: 'return ' + value
    join = '\n'
    values = {}
    for container, declaration in zip(['list', 'map', 'set'], declarations):
        fill = append if container == 'list' else put if container == 'map' else add
        setup = declaration + join + total
        small = setup + join + loop(language, 'i', WIDTH, fill)
        full = setup + join + loop(language, 'i', N, fill)
        read = list_read if container == 'list' else map_read
        read_all = ('values[i + 1]' if language == 'lua' else 'values[i]') if container == 'list' else ('values.at(i)' if language == 'cpp' else 'values[i]')
        build_sum = accumulate(read_all) if container != 'set' else has.replace('i % 2048', 'i')
        values[container + '_build'] = full + join + loop(language, 'i', N, build_sum) + join + returned('total' if container != 'set' else 'total + seed')
        values[container + '_read'] = small + join + loop(language, 'i', N, has if container == 'set' else accumulate(read)) + join + returned('total' if container != 'set' else 'total + seed')
        if container == 'set':
            prefix = ('local count = 0' if language == 'lua' else '')
            values['set_write'] = declaration + join + prefix + join + loop(language, 'i', N, duplicate) + join + returned(length + ' + seed')
        else:
            write = list_write if container == 'list' else update
            values[container + '_write'] = small + join + loop(language, 'i', N, write + join + accumulate(read)) + join + returned('total')
        if container == 'list':
            remove = accumulate(pop) + (' values.pop_back();' if language == 'cpp' else '')
        elif language == 'python' and container == 'set':
            remove = 'values.remove(i)\n' + absent
        else:
            remove = erase + join + absent
        values[container + '_remove'] = full + join + loop(language, 'i', N, remove) + join + returned('total' if container == 'list' else 'total + seed')
    return values


def expected(case):
    """用闭式公式计算六次调用的校验和，独立于各语言容器实现。"""
    container, operation = case.split('_')
    if container in ('list', 'map') and operation in ('build', 'write', 'remove'):
        if container == 'map' and operation == 'remove':
            return 6 * N + 10
        return 6 * N * (N - 1) // 2 + 10 * N
    if operation == 'read' and container != 'set':
        cycles, remainder = divmod(N, WIDTH)
        return 6 * (cycles * WIDTH * (WIDTH - 1) // 2 + remainder * (remainder - 1) // 2) + 10 * N
    if container == 'set' and operation == 'read':
        cycles, remainder = divmod(N, 2 * WIDTH)
        return 6 * (cycles * WIDTH + min(remainder, WIDTH)) + 10
    if container == 'set' and operation == 'write':
        return 6 * min(N, WIDTH) + 10
    return 6 * N + 10


def source(language, body):
    """生成各自语言的单调时钟计时壳，预热一次再记录五次执行。"""
    if language == 'chali':
        return 'import std::clock;\nint64 work(int64 seed) {\n' + body + '''
}
void main() {
    int64 sink = work(0);
    int64 times[5];
    int64 r = 0;
    while (r < 5) {
        int64 start = std::clock::nowNanos();
        sink = sink + work(r);
        times[r] = std::clock::nowNanos() - start;
        r = r + 1;
    }
    r = 0;
    while (r < 5) { println("rep {}", times[r]); r = r + 1; }
    println("sink {}", sink);
}
'''
    if language == 'cpp':
        return '''#include <chrono>
#include <cstdint>
#include <cstdio>
#include <vector>
#include <unordered_map>
#include <unordered_set>
/**
 * @brief 使用默认标准容器执行指定负载。
 * @param seed 本次调用的数据初值。
 * @return 供跨语言核对的校验值。
 */
std::int64_t work(std::int64_t seed) {
''' + body + '''
}
/**
 * @brief 读取单调时钟的纳秒数。
 * @return 当前单调时间。
 */
std::int64_t now_ns() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}
/**
 * @brief 预热后通过 volatile 函数入口测量，避免编译器将被测调用外提。
 * @return 全部测量完成时返回零。
 */
int main() {
    std::int64_t (*volatile entry)(std::int64_t) = work;
    std::int64_t sink = entry(0), times[5];
    for (int r = 0; r < 5; ++r) {
        const auto start = now_ns();
        sink += entry(r);
        times[r] = now_ns() - start;
    }
    for (auto elapsed : times) std::printf("rep %lld\\n", static_cast<long long>(elapsed));
    std::printf("sink %lld\\n", static_cast<long long>(sink));
}
'''
    if language == 'python':
        return 'import time\ndef work(seed):\n    ' + body.replace('\n', '\n    ') + '''
sink = work(0)
times = []
for r in range(5):
    start = time.perf_counter_ns()
    sink += work(r)
    times.append(time.perf_counter_ns() - start)
for elapsed in times:
    print("rep", elapsed)
print("sink", sink)
'''
    return 'local function work(seed)\n' + body + '''
end
local sink, times = work(0), {}
for r = 0, 4 do
    local start = now_ns()
    sink = sink + work(r)
    times[#times + 1] = now_ns() - start
end
for _, elapsed in ipairs(times) do io.write(string.format("rep %d\\n", elapsed)) end
io.write(string.format("sink %d\\n", sink))
'''
