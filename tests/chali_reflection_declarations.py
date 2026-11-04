"""真实 CHALI 声明到元数据：布局、注解、稳定身份与容器。"""
from __future__ import annotations
from compiler_path import compiler_path
import argparse
import os
from pathlib import Path
import subprocess
from project_temp import tempfile
ROOT = Path(__file__).resolve().parents[1]


def main():
    """验证前端限制，以及原生查询返回的真实声明身份。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    args = parser.parse_args()
    args.chali = args.chali.resolve()
    environment = dict(os.environ, CHALI_LANG="zh")
    checks = 0

    def run(command, success=True, cwd=ROOT):
        nonlocal checks
        result = subprocess.run(list(map(str, command)), cwd=cwd, env=environment, capture_output=True, text=True,
                                encoding='utf-8', errors='replace', timeout=240)
        assert (result.returncode == 0) == success, (command, result.stdout, result.stderr)
        checks += 1
        return result

    with tempfile.TemporaryDirectory(prefix='reflection-declarations-') as temporary:
        work = Path(temporary)
        sample = ROOT / 'example/reflection/declarations.chl'
        # 直接构建 LLVM 原生产物，通过实际查询检查元数据。
        run([args.chali, 'aot', '--reflection', sample, '--out', work / 'program'])
        assert run([str(work / 'program') + ('.exe' if os.name == 'nt' else '')]).stdout == '42\n'
        source = work / 'case.chl'

        def check(text, error=None, enabled=True):
            source.write_text(text, encoding='utf-8')
            result = run([args.chali, *(['--reflection'] if enabled else ['--no-reflection']), '--check', source],
                         success=error is None)
            if error:
                assert error in result.stderr, result.stderr

        check('@Reflect(1) int64 f(){return 1;} void main(){}', '不接受参数')
        check('@Reflect struct S{@Reflect int64 x;}; void main(){}', '成员随类型暴露')
        check('@Reflect void main(){}', '功能未开启', enabled=False)
        check('@std::reflect::Reflect void main(){}')
        check('@annotation Reflect(x:int32)\n@Reflect(1) void main(){}')
        check('struct Box<T>{T value;};\n@Reflect int64 read(Box<int64> x){return x.value;}\nvoid main(){return;}')
        (work / 'chali.toml').write_text('[package]\nname="reflection"\nversion="0.1.0"\n[chali]\nimport_paths=["."]\n',
                                         encoding='utf-8')
        for module in ('one', 'two'):
            (work / (module + '.chl')).write_text('module ' + module + ';\n@Reflect struct Same{int64 value;};\n',
                                                  encoding='utf-8')
        source.write_text('import one;\nimport two;\nimport std::reflect;\nvoid main(){\n'
                          'let one=std::reflect::getStructInfo<one::Same>().unwrap();\n'
                          'let two=std::reflect::getStructInfo<two::Same>().unwrap();\n'
                          'println("{}|{}",one.name,one.typeId);println("{}|{}",two.name,two.typeId);}', encoding='utf-8')
        run([args.chali, 'aot', '--reflection', source, '--out', work / 'names'], cwd=work)

        def identities(prefix):
            """读取原生产物实际报告的稳定身份，不依赖已经删除的导出格式。"""
            assert not prefix.with_suffix('.quad').exists()
            output = run([str(prefix) + ('.exe' if os.name == 'nt' else '')]).stdout
            result = {}
            for line in output.splitlines():
                name, value = line.rsplit('|', 1)
                result[name] = int(value)
            return result

        names = identities(work / 'names')
        assert names['one::Same'] != names['two::Same']
        snapshots = []
        for index, alias in enumerate(('String', 'std::core::String')):
            extra = 'struct Unrelated{int32 x;};\n' if index else ''
            source.write_text('import std::reflect;\n' + extra + 'struct Box<T>{T value;};\n@Reflect String read(Box<' + alias +
                              '> value){return value.value;}\nvoid main(){\n'
                              'let info=std::reflect::getFuncInfo("program::read").unwrap();\n'
                              'assert(info.params[0].typeId==std::reflect::getTypeId<Box<' + alias + '>>());\n'
                              'assert(info.returnTypeId==std::reflect::getTypeId<std::core::String>());\n'
                              'println("function|{}",info.typeId);println("box|{}",info.params[0].typeId);\n'
                              'println("string|{}",info.returnTypeId);}', encoding='utf-8')
            prefix = work / ('identity' + str(index))
            run([args.chali, 'aot', '--reflection', source, '--out', prefix])
            snapshots.append(identities(prefix))
        assert snapshots[0] == snapshots[1], snapshots
        assert all(value != 0 for value in snapshots[0].values()), snapshots
        print(f'CHALI 声明元数据：{checks} 项集成检查通过')


if __name__ == '__main__':
    main()
