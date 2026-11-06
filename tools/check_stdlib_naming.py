"""核对标准库和原生扩展公开 API 的类型、方法与模块命名。"""
import re

from library_reference import inventory, declarations, ROOT


def public_declarations():
    """读取实际标准库和原生模板，ABI 指令不作为语言函数声明扫描。"""
    modules = inventory()
    for path in sorted((ROOT / 'library').glob('**/*.chl.in')):
        source = path.read_text(encoding='utf-8')
        match = re.search(r'\bmodule\s+([\w:]+)\s*;', source)
        if match and match[1] not in modules:
            source = re.sub(r'^#abi\([^\n]*\)', '', source, flags=re.M)
            modules[match[1]] = declarations(path, source)
    return modules


def main():
    """检查完整名称和同一作用域中的重复签名，构造器保留类型名称。"""
    problems = []
    count = 0
    modules = public_declarations()
    for module, rows in modules.items():
        for segment in module.split('::'):
            if not segment.startswith('__') and not re.fullmatch(r'[a-z][a-z0-9]*(?:_[a-z0-9]+)*', segment):
                problems.append('模块应使用下划线命名：' + module)
        signatures = set()
        for row in rows:
            name, owner, kind = row['name'], row['owner'], row['kind']
            if name.startswith('_') or any(part.startswith('_') for part in owner.split('::')):
                continue
            if kind in ('struct', 'enum', 'interface', 'opcode'):
                if not re.fullmatch(r'[A-Z][A-Za-z0-9]*', name):
                    problems.append('复合类型应使用大驼峰：' + module + '::' + name)
            elif kind == 'function':
                constructor = owner and name == owner.split('::')[-1]
                if not constructor and not re.fullmatch(r'[a-z][A-Za-z0-9]*', name):
                    problems.append('方法和模块函数应使用小驼峰：' + module + '::' + owner + '::' + name)
                signature = (owner, row['signature'])
                if signature in signatures:
                    problems.append('同一作用域存在重复签名：' + module + '::' + owner + '::' + row['signature'])
                signatures.add(signature)
                count += 1
    if problems:
        raise SystemExit('\n'.join(problems))
    print(f'标准库命名检查通过：{len(modules)} 个模块、{count} 个公开函数重载，包含 GUI、音频、XML 与密码学。')


if __name__ == '__main__':
    main()
