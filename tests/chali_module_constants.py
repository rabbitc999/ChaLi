"""验证模块常量可见性、流式 I/O 与普通函数的默认借用返回约束。"""
from compiler_path import compiler_path
from project_temp import tempfile
from pathlib import Path
import argparse
import os
import subprocess


def main():
    """用同一编译器运行正例并逐一检查作用域负例。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    with tempfile.TemporaryDirectory(prefix='module-constants-') as directory:
        work = Path(directory)
        (work/'provider.chl').write_text(
            'module provider;\n/** Public value. */\nconst int64 PUBLIC = 41;\n'
            'int64 hidden = 1;\nint64 read() { return provider::hidden; }\n', encoding='utf-8')
        env = dict(os.environ, CHALI_LANG='zh', CHALI_QUIET='1')

        def invoke(name, text, mode):
            source = work/(name+'.chl')
            source.write_text(text,encoding='utf-8')
            args = [str(compiler),mode,str(source)]
            if mode == 'run':
                args += ['--out',str(work/name)]
            return subprocess.run(args,cwd=work,env=env,capture_output=True,encoding='utf-8',timeout=180)

        visible = invoke('visible',
            'import provider; void main() { assert(provider::PUBLIC + provider::read() == 42);'
            ' assert(PUBLIC == 41); println("visibility OK"); }', 'run')
        assert visible.returncode==0 and visible.stdout=='visibility OK\n',visible
        for name,value in [('qualified','provider::hidden'),('bare','hidden')]:
            result=invoke(name,'import provider; void main() { int64 observed = '+value+'; }','check')
            assert result.returncode!=0 and 'hidden' in result.stderr,result
        mutation=invoke('constant-write','import provider; void main() { provider::PUBLIC = 0; }','check')
        assert mutation.returncode!=0,mutation
        default=invoke('default-borrow',
            'int64 mut& first(int64 mut& a,int64 mut& b){return a;} '
            'int64 mut& escape(int64 mut& stable){int64 temporary=2; '
            'int64 mut& alias=first(&stable,&temporary); return alias;} void main(){}','check')
        assert default.returncode!=0 and 'CHL1204' in default.stderr,default

        (work/'nested').mkdir()
        (work/'nested'/'input.bin').write_bytes(b'a\x00b' * 30000)
        (work/'destination.bin').write_bytes(b'old')
        io=invoke('streams', '''
import std::io;
import std::path;
void main() {
    let input = std::io::FileStream::open("nested/input.bin", "rb").take();
    let temporary = std::io::create_temp_file(".").unwrap();
    let output = std::io::FileStream::open(temporary, "wb").take();
    assert(std::io::copy_to(&input,&output).unwrap() == 90000);
    assert(input.close() == 0 && output.close() == 0);
    assert(std::io::replace_file(temporary, "destination.bin").isOk());
    assert(std::io::walk_directory("nested").unwrap().len() == 1);
    println("streams OK");
}
''','run')
        assert io.returncode==0 and io.stdout=='streams OK\n',io
        assert (work/'destination.bin').read_bytes()==b'a\x00b' * 30000
        assert not list(work.glob('.chali-*.tmp'))
    print('module constants, private globals, default borrow union and binary I/O passed')


if __name__ == '__main__':
    main()
