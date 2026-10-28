"""按新 20% 门槛重新判定已保存数据；不运行编译器或任何被测程序。"""
from pathlib import Path
import hashlib
import json

ROOT = Path(__file__).resolve().parents[4]
HERE = Path(__file__).resolve().parent
REPORTS = {}


def read(name):
    """读取原始报告并保留其字节摘要与实际测量提交。"""
    relative = 'benchmarks/results/' + name + '/results.json'
    path = ROOT / relative
    value = json.loads(path.read_text(encoding='utf-8'))
    REPORTS[relative] = dict(sha256=hashlib.sha256(path.read_bytes()).hexdigest(),
                             compiler_source_commit=value.get('compiler_source_commit') or value.get('chali_compiler', {}).get('source_commit'),
                             compiler_sha256=value.get('compiler_sha256') or value.get('chali_compiler', {}).get('sha256'),
                             original_git_commit=value.get('git_commit'))
    return value, relative


def main():
    """保存混合历史提交的派生快照，明确它不是当前发行编译器的最终验收。"""
    basic_cpp, basic_cpp_path = read('1.0.11-cpp-o2-reference')
    lua, lua_path = read('1.0.11-thread-state')
    strings, strings_path = read('1.0.11-string-baseline')
    groups = [
        ('1.0.11-reference-storage-children', ['int_chain', 'float_chain', 'call', 'array']),
        ('1.0.11-list-bounded-growth', ['list_growth']),
        ('1.0.11-map-set-bounded-growth', ['map_growth', 'set_growth']),
        ('1.0.11-feature-data-snapshot', ['enum_payload', 'struct_nested', 'array_2d', 'array_3d']),
        ('1.0.11-interface-object-scalars', ['interface_mono', 'interface_poly']),
        ('1.0.11-string-memory-address', ['string_read', 'string_concat', 'string_slice', 'string_find']),
    ]
    rows = []
    for report_name, cases in groups:
        report, path = read(report_name)
        for case in cases:
            measured = report['configs']['chali-aot']['median_ms'][case] if 'configs' in report else report['configurations'][case]['chali-aot']['median_ms']
            if case.startswith('string_'):
                reference = strings['configurations'][case]
                cpp_ms = reference['cpp-o2']['median_ms']
                lua_ms = min(cell['median_ms'] for name, cell in reference.items() if name.startswith('lua'))
                cpp_path = lua_source = strings_path
            else:
                cpp_ms = basic_cpp['cases'][case]['median_ms']
                cpp_path = basic_cpp_path
                lua_ms = lua['configs']['lua']['median_ms'].get(case)
                lua_source = lua_path if lua_ms is not None else None
            rows.append(dict(case=case, chali_ms=measured, cpp_o2_ms=cpp_ms,
                             cpp_ratio=measured / cpp_ms, cpp_limit=1.20,
                             cpp_pass=measured <= cpp_ms * 1.20,
                             lua_ms=lua_ms, lua_pass=measured < lua_ms if lua_ms is not None else None,
                             measurement=path, cpp_reference=cpp_path, lua_reference=lua_source))
    result = dict(kind='saved-data-reassessment', date='2026-10-29', cpp_limit=1.20,
                  chali_optimization='LLVM O3', cpp_reference_optimization='O2',
                  mixed_measurement_commits=True, final_release_validation=False,
                  current_uncommitted_string_buffer_measured=False,
                  note='Only saved medians are re-evaluated; original reports and their 1.05 judgments remain unchanged. Missing Lua cells are not counted as Lua passes.',
                  cpp_pass_count=sum(row['cpp_pass'] for row in rows), rows=rows, reports=REPORTS)
    (HERE / 'results.json').write_text(json.dumps(result, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    lines = ['# 已保存数据按 20% 门槛重新判定', '',
             '仅重新计算已保存数据，未运行编译器、参考程序或重新计时；此表来自不同历史提交，不是当前发行版最终验收。正在实现的 String 缓冲区尚未纳入计时。', '',
             '| 负载 | ChaLi / ms | C++ O2 / ms | 耗时比 | 20% C++ 门槛 | Lua 参考 |',
             '| --- | ---: | ---: | ---: | --- | --- |']
    for row in rows:
        lua_status = '无该项参考' if row['lua_pass'] is None else '快于 Lua' if row['lua_pass'] else '仍慢于 Lua'
        cpp_status = '通过' if row['cpp_pass'] else '未通过'
        lines.append(f"| {row['case']} | {row['chali_ms']:.4f} | {row['cpp_o2_ms']:.4f} | {row['cpp_ratio']:.4f} | {cpp_status} | {lua_status} |")
    lines += ['', f"17 项中 {result['cpp_pass_count']} 项达到当前 C++ O2 的 1.20 倍条件。三维数组在新标准下达到 C++ 条件；其旧 5% 判定保留不改。Lua 无参考的单元没有宣称快于 Lua。仍需继续优化其余负载，并在最终源码上完成正式计时和全量门禁。", '']
    (HERE / 'README.md').write_text('\n'.join(lines), encoding='utf-8')
    print(f"saved-data C++ O2 gate: {result['cpp_pass_count']}/{len(rows)}; no benchmark executed")


if __name__ == '__main__':
    main()
