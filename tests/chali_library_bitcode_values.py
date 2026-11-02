"""对照原生与 bitcode 的记录、枚举、借用返回、修改及共享数据映像。"""
import argparse
from pathlib import Path
from compiler_path import compiler_path
from project_temp import tempfile
from chali_library_package import invoke, decode


def main():
    """在两种分配器和各 LTO 模式下核对已有聚合 ABI，不扩展语言类型边界。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    parser.add_argument('--external-clang', type=Path)
    args = parser.parse_args()
    compiler = args.chali.resolve()
    tools = ['--llvm', args.external_clang.resolve()] if args.external_clang else []
    with tempfile.TemporaryDirectory(prefix='library-bitcode-values-') as directory:
        work = Path(directory)
        modules = work / 'modules/demo'
        modules.mkdir(parents=True)
        source = work / 'provider.chl'
        source.write_text('''module demo::values;
struct Point {int64 x;float64 y;};
enum Event {Empty,Count(int64 number)};
Point make(int64 number){Point result;result.x=number;result.y=2.5;return result;}
#borrow(#ret,[value])
Point const& view(Point const& value){return value;}
void bump(Point mut& value){value.x=value.x+1;}
Event event(int64 number){return Event::Count(number);}
#borrow(#ret,[value])
Event const& viewEvent(Event const& value){return value;}
''', encoding='utf-8')
        invoke([compiler, 'lib', 'build', source, '--module', 'demo::values', '--package-version', '1.0.0',
                '--checked', '--with-bitcode', *tools, '--out', modules / 'values.chlib'], work)
        manifest, _ = decode((modules / 'values.chlib').read_bytes())
        assert 'unit-data-v1' in manifest['requiredFeatures'] and manifest['targets'][0]['unitData']
        source.unlink()
        consumer = work / 'main.chl'
        consumer.write_text('''import demo::values;
struct Unrelated {int64 marker;};
void main(){
 Unrelated other;other.marker=9;
 String padding="unrelated consumer image";assert(padding=="unrelated consumer image");
 int64 index=0;
 while(index<10){
  demo::values::Point point=demo::values::make(40+index);
  {demo::values::Point const& borrowed=demo::values::view(&point);assert(borrowed.x==40+index&&borrowed.y==2.5);}
  demo::values::bump(&point);assert(point.x==41+index);
  demo::values::Event event=demo::values::event(73+index);
  match(demo::values::viewEvent(&event)){demo::values::Event::Count(number)=>{assert(number==73+index);}_=>{assert(false);}}
  assert(other.marker==9&&padding=="unrelated consumer image");
  index=index+1;
 }
 println("bitcode values OK");
}
''', encoding='utf-8')
        for implementation, mode in [('native', 'off'), ('native', 'full'), ('bitcode', 'off'), ('bitcode', 'full'), ('bitcode', 'thin')]:
            for allocator in ['malloc', 'pool']:
                result = invoke([compiler, 'run', consumer, '--library-implementation', implementation,
                                 '--lto-mode', mode, '--allocator', allocator, *tools], work)
                assert result.stdout == 'bitcode values OK\n', (implementation, mode, allocator, result.stdout)
        print('bitcode aggregate ABI: native/off/full/thin, malloc/pool, records/enums/borrows/mutation/unit data OK')


if __name__ == '__main__':
    main()
