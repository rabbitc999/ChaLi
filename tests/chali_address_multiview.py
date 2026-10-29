"""核对多项视图、单项回退和禁用缓存时的访问及持续宿主线程生命周期。"""
from compiler_path import compiler_path
from project_temp import tempfile
from pathlib import Path
import argparse
import ctypes
import hashlib
import json
import os
import queue
import re
import subprocess
import sys
import threading

ROOT = Path(__file__).resolve().parents[1]


def lifecycle(path):
    """同一宿主线程跨 64 次初始化／销毁执行交错分配访问。"""
    library = ctypes.CDLL(str(path))
    library.chali_init.restype = None
    library.chali_finish.restype = None
    library.address_probe.argtypes = [ctypes.c_int64]
    library.address_probe.restype = ctypes.c_int64
    requests, responses = queue.Queue(), queue.Queue()

    def worker():
        """仅执行显式参数的导出，不依赖初始化线程的语言 TLS 全局。"""
        while True:
            seed = requests.get()
            if seed is None:
                return
            try:
                responses.put(library.address_probe(seed))
            except BaseException as error:
                responses.put(repr(error))

    thread = threading.Thread(target=worker, daemon=True)
    thread.start()
    try:
        for cycle in range(64):
            library.chali_init()
            try:
                requests.put(cycle)
                assert responses.get(timeout=30) == 232 * cycle + 1428, (cycle, path)
                requests.put(-cycle)
                assert responses.get(timeout=30) == -232 * cycle + 1428, (cycle, path)
            finally:
                library.chali_finish()
    finally:
        requests.put(None)
        thread.join(timeout=30)
        assert not thread.is_alive(), path


def main():
    """编译八种独立配置并保存命令、诊断与实际 LLVM 原语摘录。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--worker', type=Path)
    args = parser.parse_args()
    if args.worker:
        lifecycle(args.worker.resolve())
        print('persistent thread: 64 init/finish cycles OK')
        return
    compiler = args.chali.resolve()
    work = Path(tempfile.mkdtemp(prefix='address-multiview-'))
    results = []
    for allocator in ('malloc', 'pool'):
        for mode in ('multi', 'round-robin', 'single', 'off'):
            label = allocator + '-' + mode
            env = dict(os.environ)
            env.pop('CHALI_NO_AOT_ADDRESS_CACHE', None)
            env.pop('CHALI_NO_AOT_MULTI_ADDRESS_CACHE', None)
            env.pop('CHALI_NO_AOT_ADDRESS_PROMOTION', None)
            if mode == 'round-robin':
                env['CHALI_NO_AOT_ADDRESS_PROMOTION'] = '1'
            if mode == 'single':
                env['CHALI_NO_AOT_MULTI_ADDRESS_CACHE'] = '1'
            if mode == 'off':
                env['CHALI_NO_AOT_ADDRESS_CACHE'] = '1'

            def execute(command, suffix):
                """保存每一步返回值和输出，执行失败立即保留现场。"""
                process = subprocess.run(list(map(str, command)), cwd=ROOT, env=env,
                                         capture_output=True, text=True, encoding='utf-8', timeout=600)
                item = dict(label=label + '-' + suffix, command=list(map(str, command)),
                            code=process.returncode, stdout=process.stdout, stderr=process.stderr)
                results.append(item)
                (work / (item['label'] + '.json')).write_text(json.dumps(item, ensure_ascii=False, indent=2), encoding='utf-8')
                assert process.returncode == 0, (item, work)
                return process

            fixture = ROOT / 'tests/fixtures/aot_address_multiview.chl'
            prefix = work / label
            checked = execute([compiler, 'run', fixture, '--allocator', allocator, '--out', str(prefix) + '-run'], 'run')
            assert checked.stdout == 'multiple address views OK\n' and not checked.stderr, (checked, work)
            execute([compiler, 'aot', fixture, '--shared', '--allocator', allocator, '--out', prefix], 'shared')
            llvm = prefix.with_suffix('.ll').read_text(encoding='utf-8')
            primitive = re.search(r'^define internal ptr @chali_primitive_address\(.*?^\}', llvm, re.M | re.S)
            assert primitive is not None, work
            assert primitive[0].count('call ptr @chali_cached_view_address') == (1 if mode == 'single' else 4), (label, work)
            assert ('swap1:' in primitive[0]) == (mode in ('multi', 'off')), (label, work)
            reset = re.search(r'^define void @chali_reset_address_view\(.*?^\}', llvm, re.M | re.S)
            assert reset is not None, (label, work)
            cleared = re.findall(r'store ptr null, ptr getelementptr .*?ptr @(chali_address_view[123]?),', reset[0])
            assert sorted(cleared) == ['chali_address_view', 'chali_address_view1', 'chali_address_view2', 'chali_address_view3'], (label, work)
            (work / (label + '-primitive.ll')).write_text(primitive[0] + '\n' + reset[0] + '\n', encoding='utf-8')
            library = prefix.with_suffix('.dll' if os.name == 'nt' else '.so')
            worker = execute([sys.executable, __file__, '--worker', library], 'lifecycle')
            assert worker.stdout == 'persistent thread: 64 init/finish cycles OK\n' and not worker.stderr, (worker, work)
    (work / 'results.json').write_text(json.dumps(dict(
        compiler_sha256=hashlib.sha256(compiler.read_bytes()).hexdigest(), results=results),
        ensure_ascii=False, indent=2), encoding='utf-8')
    print('address views: 8 configurations, checked access and 512 persistent-thread lifecycle rounds OK;', work)


if __name__ == '__main__':
    main()
