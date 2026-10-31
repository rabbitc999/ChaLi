"""真实 C 库验证每种基础类型的数组别名、回写与混合签名捕获回调。"""
from compiler_path import compiler_path
from project_temp import tempfile
from pathlib import Path
import argparse
import json
import os
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def main():
    """在两种分配器及两种检查模式下验证 C 帧生成封送。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    cases = (
        ('boolean', 'bool', 'bool', 'false', 'true', 'true'),
        ('character', 'uint8_t', 'char', "'A'", "'B'", "'Z'"),
        ('i8', 'int8_t', 'int8', '(int8)126', '(int8)127', '(int8)(-17)'),
        ('u8', 'uint8_t', 'uint8', '(uint8)254', '(uint8)255', '(uint8)127'),
        ('i16', 'int16_t', 'int16', '(int16)(-3)', '(int16)(-2)', '(int16)123'),
        ('u16', 'uint16_t', 'uint16', '(uint16)65534', '(uint16)65535', '(uint16)123'),
        ('i32', 'int32_t', 'int32', '(int32)(-3)', '(int32)(-2)', '(int32)123'),
        ('u32', 'uint32_t', 'uint32', '(uint32)4294967294', '(uint32)4294967295', '(uint32)123'),
        ('i64', 'int64_t', 'int64', '(int64)(-3)', '(int64)(-2)', '(int64)123'),
        ('u64', 'uint64_t', 'uint64', '(uint64)(-1)', '(uint64)0', '(uint64)123'),
        ('f32', 'float', 'float32', '(float32)1.25', '(float32)2.25', '(float32)(-3.5)'),
        ('f64', 'double', 'float64', '(float64)1.25', '(float64)2.25', '(float64)(-3.5)'),
    )
    with tempfile.TemporaryDirectory(prefix='native-marshal-') as folder:
        work = Path(folder)
        native = work / 'native.c'
        library = work / ('native.dll' if os.name == 'nt' else 'native.so')
        body = '#include <stdint.h>\n#include <stdbool.h>\n#include <stdlib.h>\n#include <string.h>\n'
        body += '#ifdef _WIN32\n#define API __declspec(dllexport)\n#else\n#define API\n#endif\n'
        for name, ctype, *_ in cases:
            body += ('/**\n * @brief 验证同一数组的两个参数共享副本。\n'
                     ' * @param left 可写数组。\n * @param right 同一数组的别名。\n'
                     ' * @return 别名读取到的修改值。\n */\n'
                     f'API {ctype} bump_{name}({ctype} *left,{ctype} *right)'
                     f'{{left[0]=({ctype})(left[0]+1);return right[0];}}\n')
            body += ('/**\n * @brief 返回用于复制到语言的静态测试数组。\n'
                     ' * @param count 固定为二的请求数量。\n * @return 两项静态数组。\n */\n'
                     f'API {ctype} *values_{name}(int32_t count){{static {ctype} values[2];(void)count;return values;}}\n')
        body += ('/**\n * @brief 用混合位宽签名调用语言捕获闭包。\n'
                 ' * @param callback 同步回调。\n * @return 回调结果。\n */\n'
                 'API int64_t mixed(int64_t (*callback)(uint8_t,float,uint64_t))'
                 '{return callback((uint8_t)200,1.25f,UINT64_MAX);}\n')
        body += ('static int64_t releases=0;\n'
                 '/**\n * @brief 返回拥有型别名文本。\n * @return 新分配的文本。\n */\n'
                 'API const char *owned_alias(void){char *value=(char *)malloc(6);if(value)memcpy(value,"alias",6);return value;}\n'
                 '/**\n * @brief 释放拥有文本并计数。\n * @param value 已返回的原生文本。\n */\n'
                 'API void release_alias(void *value){++releases;free(value);}\n'
                 '/**\n * @brief 查询本次进程的释放次数。\n * @return 已释放文本数。\n */\n'
                 'API int64_t alias_releases(void){return releases;}\n')
        native.write_text(body, encoding='utf-8')
        result = subprocess.run(['gcc', '-std=c11', '-shared', '-fPIC', str(native), '-o', str(library)],
                                capture_output=True, text=True, encoding='utf-8', timeout=90)
        assert result.returncode == 0, result.stderr
        declarations = []
        statements = []
        for name, _, language, initial, expected, sentinel in cases:
            declarations.append('#abi("C",' + json.dumps(library.as_posix()) + ',"bump_' + name + '")\n'
                                f'{language} bump_{name}({language}[] left,{language}[] right);\n')
            declarations.append('#abi("C",' + json.dumps(library.as_posix()) + ',"values_' + name + '")\n'
                                f'{language}[] values_{name}(int32 count)[count];\n')
            statements.append(f'{language} a_{name}[]=values_{name}((int32)2);'
                              f'a_{name}[0]={initial};a_{name}[1]={sentinel};'
                              f'assert(bump_{name}(a_{name},a_{name})=={expected});'
                              f'assert(a_{name}[0]=={expected});assert(a_{name}[1]=={sentinel});\n')
        declarations.append('#abi("C",' + json.dumps(library.as_posix()) + ',"mixed")\n'
                            'int64 mixed(int64 fn(uint8,float32,uint64) callback);\n')
        declarations.append('type HostText=String;\n'
                            '#abi("C",' + json.dumps(library.as_posix()) + ',"owned_alias")\n'
                            'HostText ownedAlias()["release_alias"];\n'
                            '#abi("C",' + json.dumps(library.as_posix()) + ',"alias_releases")\n'
                            'int64 aliasReleases();\n')
        statements.append('ownedAlias();assert(aliasReleases()==1);\n')
        statements.append('int64 captured=41;'
                          'let callback=int64 fn copy(uint8 small,float32 real,uint64 large){'
                          'assert(small==(uint8)200);assert(real==(float32)1.25);'
                          'assert(large==(uint64)(-1));return captured+1;};'
                          'assert(mixed(callback)==42);'
                          'println("C frame scalar/array/callback matrix passed");\n')
        source = work / 'matrix.chl'
        source.write_text(''.join(declarations) + 'void main(){\n' + ''.join(statements) + '}\n', encoding='utf-8')
        for allocator in ('malloc', 'pool'):
            for checks in ('checked', 'unchecked'):
                result = subprocess.run([str(compiler), 'run', str(source), '--allocator', allocator, '--' + checks,
                                         '--out', str(work / (allocator + '-' + checks))], cwd=ROOT,
                                        env=dict(os.environ, CHALI_LANG='zh'), capture_output=True,
                                        text=True, encoding='utf-8', errors='replace', timeout=180)
                assert (result.returncode, result.stdout, result.stderr) == (0, 'C frame scalar/array/callback matrix passed\n', ''), result
                print(allocator + '/' + checks + ': 12 scalar arrays, shared aliases and mixed-width captured callback passed', flush=True)


if __name__ == '__main__':
    main()
