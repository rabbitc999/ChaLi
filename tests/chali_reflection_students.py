"""反射版学生信息管理系统的端到端回归。

先 `chali aot` 构建一次原生产物，再逐条跑命令：每条命令都是一个独立进程，
所以这套用例顺带验证了跨进程持久化和停机零资源残留。

用法：python tests/chali_reflection_students.py [--compiler 路径]
"""
from compiler_path import compiler_path
import argparse
import os
import subprocess
import sys
from project_temp import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / 'example' / 'reflection-students' / 'main.chl'

ONE = '{"id":1,"name":"张伟","age":18,"className":"高一(3)班","score":92}'
ONE_UPDATED = '{"id":1,"name":"张伟","age":19,"className":"高一(3)班","score":95}'
TWO = '{"id":2,"name":"李娜","age":17,"className":"高一(3)班","score":88}'
INJECTION = '{"id":6,"name":"\'; DROP TABLE students; --","age":10,"className":"c","score":10}'

# （传给程序的参数，期望的标准输出）	—— 按顺序在同一份数据库上执行。
# 期望值都是实测得到的真实输出，改程序行为时必须同步改这里。
CASES = [
    # ---- 先建表、新增、列出 ----
    (('schema',), 'Student: id, name, age, className, score'),
    (('add', ONE), ONE),
    (('add', TWO), TWO),
    (('list',), '[' + ONE + ',' + TWO + ']'),
    # ---- 按学号查、改、再查 ----
    (('get', '1'), '[' + ONE + ']'),
    (('update', ONE_UPDATED), ONE_UPDATED),
    (('get', '1'), '[' + ONE_UPDATED + ']'),
    # ---- 搜索与排序 ----
    (('search', '李娜'), '[' + TWO + ']'),
    (('list', 'score', 'desc'), '[' + ONE_UPDATED + ',' + TWO + ']'),
    (('list', 'name', 'asc'), '[' + ONE_UPDATED + ',' + TWO + ']'),
    # ---- 删除后仍在（跨进程持久化） ----
    (('delete', '2'), '已删除'),
    (('list',), '[' + ONE_UPDATED + ']'),
    # ---- 业务校验 ----
    (('get', '999'), '错误：学生不存在'),
    (('delete', '999'), '错误：学生不存在'),
    (('add', '{"id":0,"name":"x","age":10,"className":"c","score":10}'), '错误：学号必须是正整数'),
    (('add', '{"id":3,"name":"王强","age":200,"className":"高二(1)班","score":50}'), '错误：年龄必须在 1 到 150 之间'),
    (('add', '{"id":4,"name":"","age":10,"className":"c","score":10}'), '错误：姓名必须为 1 到 120 字节的非空文本'),
    (('add', '{"id":5,"name":"ok","age":10,"className":"c","score":101}'), '错误：成绩必须在 0 到 100 之间'),
    # ---- 模型与命令边界 ----
    (('add', 'not-json'), '错误：1:1: 这里只认 true / false / null [unexpected-token]'),
    (('list', 'badfield'), '错误：模型 program::Student 没有字段 badfield'),
    (('list', 'id', 'sideways'), '错误：排序方向必须是 asc 或 desc'),
    (('bogus',), '错误：命令或参数错误：add/update JSON；get/delete 学号；list [字段 [asc|desc]]；search 姓名；schema'),
    # ---- 重复学号：主键冲突要变成诊断 ----
    (('add', ONE), '错误：UNIQUE constraint failed: students.id'),
    # ---- SQL 注入：文本原样入库，表没有被删掉 ----
    (('add', INJECTION), INJECTION),
    (('list',), '[' + ONE_UPDATED + ',' + INJECTION + ']'),
    (('schema',), 'Student: id, name, age, className, score'),
]


def main():
    """构建原生产物后逐条执行用例，最后报告通过数量。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--compiler', default=str(compiler_path()))
    options = parser.parse_args()
    options.compiler = str(Path(options.compiler).resolve())

    with tempfile.TemporaryDirectory() as temporary:
        work = Path(temporary)
        prefix = work / 'students'
        command = [options.compiler, 'aot', str(SOURCE), '--out', str(prefix)]
        environment = dict(os.environ, CHALI_LLVM_CLANG=(str(ROOT / 'build/dependencies/llvm/toolchain/bin/clang.exe')
                                                          if os.name == 'nt' else os.environ.get('CHALI_LLVM_CLANG', 'clang')))
        compiled = subprocess.run(command, cwd=work, env=environment, capture_output=True, text=True,
                                  encoding='utf-8', timeout=600)
        if compiled.returncode != 0:
            raise AssertionError((command, compiled.returncode, compiled.stdout, compiled.stderr))
        launcher = [str(prefix) + ('.exe' if os.name == 'nt' else '')]

        database = work / 'students.db'
        for arguments, expected in CASES:
            result = subprocess.run([*launcher, str(database), *arguments],
                                    cwd=work, capture_output=True, text=True, encoding='utf-8', timeout=300)
            label = ' '.join(arguments)
            # 业务失败也走 println，进程退出码恒为 0；只有运行期错误或停机仍有资源才返回 1。
            if result.returncode != 0:
                raise AssertionError((label, '退出码', result.returncode, result.stdout, result.stderr))
            if result.stderr.strip():
                raise AssertionError((label, '标准错误非空', result.stderr))
            if result.stdout != expected + '\n':
                raise AssertionError((label, '输出不符', repr(result.stdout), '期望', repr(expected + '\n')))
        print('AOT 反射版学生管理系统回归通过：%d 条命令（含持久化、校验、注入）' % len(CASES))
        return 0


if __name__ == '__main__':
    sys.exit(main())
