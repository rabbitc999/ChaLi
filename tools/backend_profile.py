"""汇总显式启用的 AOT 所有权操作计数，按类别和四元式位置展示。"""
from pathlib import Path
import argparse
from collections import Counter
import json


def category(site):
    """区分深复制与原始槽搬运，不把不同清理层级相加冒充析构次数。"""
    event = site.split('\t')[0]
    if event.startswith('call:__chali_agg_cpy_'):
        return 'aggregate_deep_copy'
    if event.startswith('call:__chali_agg_rel_') or event.startswith('call:__chali_arr_rel_'):
        return 'recursive_cleanup'
    if event == 'call:__chali_copy_struct':
        return 'slot_transfer'
    if event == 'call:std::lang::__stringCopy':
        return 'string_copy'
    if event.startswith('call:'):
        return 'other_call'
    return event


def main():
    """读取计数文件；耗时测量必须使用未插桩产物另行执行。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('profile', type=Path)
    parser.add_argument('--top', type=int, default=20)
    parser.add_argument('--json', action='store_true')
    args = parser.parse_args()
    if args.top < 1:
        parser.error('--top 必须为正数')
    data = json.loads(args.profile.read_text(encoding='utf-8'))
    if data.get('schema') != 1:
        parser.error('不支持的计数文件版本')
    counts, requested = Counter(), Counter()
    for event in data['events']:
        kind = category(event['site'])
        counts[kind] += event['count']
        requested[kind] += event['requested_bytes']
    top = sorted(data['events'], key=lambda event: event['count'], reverse=True)[:args.top]
    report = dict(scope=data['scope'], counts=dict(counts), requested_bytes=dict(requested), top=top)
    if args.json:
        print(json.dumps(report, ensure_ascii=False, indent=2))
    else:
        print('计数口径：'+data['scope'])
        for kind, count in counts.most_common():
            print(f'{kind}: {count}，请求字节 {requested[kind]}')
        print('\n执行次数最高的位置：')
        for event in top:
            print(str(event['count'])+'\t'+event['site'])


if __name__ == '__main__':
    main()
