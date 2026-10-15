#!/usr/bin/env python3
"""检查标准库公开函数/方法的小驼峰与类型的大驼峰命名。"""
import re
from library_reference import inventory


def main():
    """扫描源码及编译器内建签名，拒绝不符合命名约定的公开符号。"""
    errors = []
    functions = types = 0
    for module, rows in inventory().items():
        for row in rows:
            name, kind = row['name'], row['kind']
            # 双下划线是内部编译器/宿主协议；同名构造器沿用类型名。
            if name.startswith('__') or (kind == 'function' and name == row['owner']):
                continue
            if kind == 'function':
                functions += 1
                if not re.fullmatch(r'[a-z][A-Za-z0-9]*', name):
                    errors.append(f'{module}::{row["owner"]}::{name}：函数/方法必须使用小驼峰')
            elif kind in ('struct', 'enum', 'interface'):
                types += 1
                if not re.fullmatch(r'[A-Z][a-z0-9]*(?:[A-Z][a-z0-9]+)*', name):
                    errors.append(f'{module}::{name}：类型必须使用大驼峰')
    if errors:
        raise SystemExit('\n'.join(errors))
    print(f'标准库命名通过：{functions} 个函数/方法，{types} 个类型。')


if __name__ == '__main__':
    main()
