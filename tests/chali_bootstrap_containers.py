"""交叉验证实验性 ChaLi List/Map/Set 的复制、冲突、删除和扩容行为。"""
from pathlib import Path
import argparse
import os
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
CHECK = r'''
void main() {
    BootstrapMap<int64> empty;
    BootstrapMap<int64> emptyCopy = empty;
    assert(emptyCopy.len() == 0 && emptyCopy.has(0) == 0);
    BootstrapMap<int64> actual;
    Map<int64,int64> expected;
    BootstrapSet actualSet;
    Set<int64> expectedSet;
    int64 step = 0;
    while (step < 1500) {
        int64 key = ((step * 37) % 127) - 63;
        if (step % 3 == 0) {
            actual.remove(key); expected.remove(key);
            actualSet.remove(key); expectedSet.remove(key);
        } else {
            actual.put(key, step); expected.put(key, step);
            actualSet.add(key); expectedSet.add(key);
            actualSet.add(key); expectedSet.add(key);
        }
        assert(actual.len() == expected.len() && actualSet.len() == expectedSet.len());
        int64 scan = -63;
        while (scan <= 63) {
            assert(actual.has(scan) == expected.has(scan));
            assert(actualSet.has(scan) == expectedSet.has(scan));
            if (actual.has(scan) != 0) { assert(actual[scan] == expected[scan]); }
            scan = scan + 1;
        }
        step = step + 1;
    }
    actual.put(0,7); actual.put(-9223372036854775807 - 1,11); actual.put(9223372036854775807,13);
    assert(actual[0]==7 && actual[-9223372036854775807 - 1]==11 && actual[9223372036854775807]==13);
    BootstrapMap<int64> cloned = actual.copy();
    cloned.put(0,19);
    assert(actual[0]==7 && cloned[0]==19);
    BootstrapSet clonedSet = actualSet.copy();
    clonedSet.add(10000);
    assert(actualSet.has(10000) == 0 && clonedSet.has(10000) != 0);
    BootstrapMap<String> texts;
    texts.put(1,"one"); texts.put(9,"nine"); texts.put(17,"seventeen");
    texts.remove(9); texts.put(25,"twenty-five");
    assert(texts[1]=="one" && texts[17]=="seventeen" && texts.has(9)==0);
    BootstrapMap<String> textCopy = texts.copy();
    textCopy.put(1,"changed");
    assert(texts[1]=="one" && textCopy[1]=="changed");
    BootstrapList<String> list;
    list.push("a"); list.push("b");
    BootstrapList<String> listCopy = list.copy();
    listCopy.set(0,"changed");
    assert(list[0]=="a" && listCopy[0]=="changed" && list.pop()=="b" && list.len()==1);
    BootstrapMap<String> wrapped;
    BootstrapSet wrappedSet;
    wrapped.put(7,"seven"); wrapped.put(15,"fifteen"); wrapped.put(23,"twenty-three"); wrapped.put(-1,"minus-one");
    wrappedSet.add(7); wrappedSet.add(15); wrappedSet.add(23); wrappedSet.add(-1);
    wrapped.remove(7); wrappedSet.remove(7);
    assert(wrapped.has(7)==0 && wrapped[15]=="fifteen" && wrapped[23]=="twenty-three" && wrapped[-1]=="minus-one");
    assert(wrappedSet.has(7)==0 && wrappedSet.has(15)==1 && wrappedSet.has(23)==1 && wrappedSet.has(-1)==1);
    wrapped.remove(23); wrappedSet.remove(23);
    wrapped.remove(23); wrappedSet.remove(23);
    assert(wrapped.len()==2 && wrappedSet.len()==2 && wrapped[15]=="fifteen" && wrapped[-1]=="minus-one");
    wrapped.put(0,""); wrappedSet.add(0);
    assert(wrapped.has(0)==1 && wrapped[0]=="" && wrappedSet.has(0)==1);
    wrapped.remove(-1); wrapped.remove(15); wrapped.remove(0);
    wrappedSet.remove(-1); wrappedSet.remove(15); wrappedSet.remove(0);
    assert(wrapped.len()==0 && wrappedSet.len()==0);
    wrapped.put(7,"again"); wrappedSet.add(7);
    assert(wrapped[7]=="again" && wrappedSet.has(7)==1);
    BootstrapList<String> segmentedBoundary;
    int64 boundary = 0;
    while (boundary < 300) { segmentedBoundary.push(format("item {}",boundary)); boundary = boundary + 1; }
    BootstrapList<String> segmentedCopy = segmentedBoundary.copy();
    segmentedCopy.set(64,"changed");
    assert(segmentedBoundary[63]=="item 63" && segmentedBoundary[64]=="item 64" && segmentedCopy[64]=="changed");
    boundary = 299;
    while (boundary >= 0) { assert(segmentedBoundary.pop()==format("item {}",boundary)); boundary = boundary - 1; }
    assert(segmentedBoundary.len()==0);
    segmentedBoundary.push("reused");
    assert(segmentedBoundary.pop()=="reused");
    println("bootstrap List/Map/Set correctness OK");
}
'''


def main():
    """分别在 VM 和独立 AOT 程序中执行同一交叉检查。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    backend_group = parser.add_mutually_exclusive_group()
    backend_group.add_argument('--vm-only', action='store_true')
    backend_group.add_argument('--aot-only', action='store_true')
    parser.add_argument('--map-triples', action='store_true')
    parser.add_argument('--segmented-list', action='store_true')
    args = parser.parse_args()
    compiler = args.chali.resolve()
    with tempfile.TemporaryDirectory(prefix='bootstrap-correctness-') as directory:
        work = Path(directory)
        path = work / 'check.chl'
        prototype = (ROOT / 'benchmarks/perf/bootstrap_containers.chl').read_text(encoding='utf-8')
        if args.map_triples:
            start = prototype.index('/** int64 键的开放寻址哈希表')
            prototype = prototype[:start] + (ROOT / 'benchmarks/perf/bootstrap_map_triples.chl').read_text(encoding='utf-8')
        if args.segmented_list:
            start = prototype.index('struct BootstrapList<T>')
            end = prototype.index('/** 三元组槽位' if args.map_triples else '/** int64 键的开放寻址哈希表')
            prototype = prototype[:start] + (ROOT / 'benchmarks/perf/bootstrap_list_segmented.chl').read_text(encoding='utf-8') + '\n' + prototype[end:]
        path.write_text(prototype + CHECK, encoding='utf-8')
        for backend in (('aot',) if args.aot_only else ('run',) if args.vm_only else ('run', 'aot')):
            target = work / (backend + ('.exe' if os.name == 'nt' else ''))
            result = subprocess.run([str(compiler), backend, str(path), '--out', str(target)],
                                    cwd=ROOT, capture_output=True, text=True, encoding='utf-8', timeout=600)
            assert result.returncode == 0, (backend, result.stdout, result.stderr)
            if backend == 'aot':
                result = subprocess.run([str(target)], cwd=ROOT, capture_output=True, text=True,
                                        encoding='utf-8', timeout=300)
            assert result.returncode == 0 and result.stdout == 'bootstrap List/Map/Set correctness OK\n' and not result.stderr, (
                backend, result.stdout, result.stderr)
            print(backend + ': ' + result.stdout.strip(), flush=True)


if __name__ == '__main__':
    main()
