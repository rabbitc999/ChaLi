"""核对库参考页、声明覆盖、链接和完整示例；反射需可选的专用工具链。"""
from compiler_path import compiler_path
from pathlib import Path
import argparse
import json
import os
import re
import subprocess
import sys
from project_temp import tempfile
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
import library_reference as reference
from library_reference_examples import EXAMPLES


def run(command, allowed_stderr="", stdin=None):
    """执行命令，失败保留标准输出、错误与实际退出码。"""
    result=subprocess.run(list(map(str,command)),cwd=ROOT,capture_output=True,
                          text=True,encoding='utf-8',errors='replace',timeout=180,input=stdin)
    assert result.returncode==0 and result.stderr==allowed_stderr,(command,result.returncode,result.stdout,result.stderr)
    return result.stdout


def check_structure():
    """检查扫描中易遗漏的元组、嵌套泛型、枚举载荷和多行声明边界。"""
    fixture='''module fixture;
enum Error { Bad, Empty }
enum Value<T> { Some(T item), None;
    Result<T,Error> unwrapOr(T value) { return Result<T,Error>::Ok(value); }
};
struct Box<T> {
    static Box<T> create(T value) { Box<T> box; return box; }
    (int64, Option<T>) next(T fn(T) mapper) { return 1,Option<T>::None; }
};
Result<std::core::List<T>, Error> convert<T>(
    std::core::List<T> values, String url="http://localhost/");
int64 __hidden();
'''
    # 短名示例仍须保留实际的歧义边界与源码断言。
    rows=json.loads((ROOT/'doc/library/api-index.json').read_text(encoding='utf-8'))
    assert all(row['usage'].startswith('intIter(') for row in rows
               if row['module']=='std::iter' and row['name']=='intIter')
    assert not any(row['module']=='std::iter' and row['name']=='range' for row in rows)
    folder=next(row for row in rows if row['module']=='std::algorithms' and row['name']=='foldValue')
    assert folder['usage']=='foldValue(items, initial, folder);'
    span=next(row for row in rows if row['module']=='std::memory' and row['name']=='Span::index')
    assert 'key >= 0 && key < self.length' in span['errors']['preconditions']
    entries=reference.declarations(ROOT/'source/std/core.chl',fixture)
    functions=[r for r in entries if r['kind']=='function']
    assert [(r['owner'],r['name']) for r in functions]==[('Value','unwrapOr'),('Box','create'),('Box','next'),('','convert')]
    extra=reference.declarations(ROOT/'source/std/core.chl','module fixture; int64 fn(int64) make(String s="a,  b"); int64 constant=other();')
    assert [r['name'] for r in extra if r['kind']=='function']==['make']
    assert reference.parameters(extra[0]['signature'])==[('s','String s','"a,  b"')]
    macro_fixture='''module fixture;
/// 生成带指定名称的方法。
$macro GET<T>(name:ident) -> member
T name() { return self.value; }
$end_macro
enum Kind { Integer, Symbol };
'''
    macro_entries=reference.declarations(ROOT/'source/std/core.chl',macro_fixture)
    assert [(r['kind'],r['name']) for r in macro_entries if not r['owner']]==[('macro','GET'),('enum','Kind')]
    assert macro_entries[0]['signature']=='$macro GET<T>(name:ident) -> member'
    assert reference.comment_text(macro_entries[0]['comment'])=='生成带指定名称的方法。'
    assert functions[2]['result']=='(int64, Option<T>)'
    assert reference.parameters(functions[3]['signature'])[-1]==('url','String url','"http://localhost/"')
    assert any(r['signature']=='Bad, Empty' for r in entries)
    index=json.loads((ROOT/'doc/library/api-index.json').read_text(encoding='utf-8'))
    assert len(index)==sum(r['kind']=='function' and not r['owner'].startswith('__') for entries in reference.inventory().values() for r in entries)
    assert len({(r['module'],r['name'],r['signature']) for r in index})==len(index)
    symbols=json.loads((ROOT/'doc/library/symbol-index.json').read_text(encoding='utf-8'))
    assert symbols and all(r['documentation'] for r in symbols)
    macros=[r for r in symbols if r['kind']=='macro' and r['module']=='std::macro']
    assert {r['name'] for r in macros}>={'MIN','MAX','CLAMP','Equal','Hash','Clone'}
    assert all(r['comment'] and r['signature'].startswith('$macro ') for r in macros)
    assert any(r['name']=='DateTime::year' and '公历年份' in r['documentation'] for r in symbols)
    assert any(r['name']=='LuFactorization' and r['kind']=='struct' for r in symbols)
    assert any(r['module']=='std::big_number' and r['name']=='BigInteger::pow' for r in index)
    for module, names in {'std::gui': {'openWindow','Widget::draw','textBox','EventBus::pump'},
                          'std::audio': {'open','isPlaying'}, 'std::xml': {'XmlNode::childCount','escapeAttribute'},
                          'std::crypto': {'randomHex','sha256','hmacSha256','verifyEd25519'}}.items():
        assert names <= {r['name'] for r in index if r['module']==module}, module
    assert not any(r['name'].startswith('__') or '#abi' in r['signature'] for r in index)
    assert not any(r['module']=='std' and r['name']=='pause' for r in index)
    for critical in ('mysql','maria','bytesToFloat32','parseInt64','getStructInfo','zipValue','tryFold'):
        assert any(r['name'].split('::')[-1]==critical for r in index),critical
    for page in (ROOT/'doc/library').glob('*.md'):
        text=page.read_text(encoding='utf-8')
        text=re.sub(r'```.*?```','',text,flags=re.S)
        for target in re.findall(r'\]\(([^)]+)\)',text):
            file,_,fragment=target.partition('#')
            path=page.parent/file if file else page
            assert path.exists(),(page,target)
            if fragment.startswith('api-'):
                assert 'id="'+fragment+'"' in path.read_text(encoding='utf-8'),(page,target)


def main():
    """对文档代码执行真实解释运行，复用正式样例的期望输出。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=Path(os.environ.get('CHALI_COMPILER',compiler_path() if os.name=='nt' else compiler_path())))
    parser.add_argument('--reflection-compiler',type=Path,help='编入前端和 VM 反射支持的可选工具链')
    args=parser.parse_args()
    run([sys.executable,ROOT/'tools/library_reference.py','--check'])
    check_structure()
    passed=0
    with tempfile.TemporaryDirectory(prefix='library-reference-',dir=ROOT/'tmp') as temporary:
        work=Path(temporary)
        for example in sorted(set(EXAMPLES.values())):
            source=ROOT/example
            name=source.stem
            compiler=args.chali
            flags=[]
            if example in (EXAMPLES['std::gui'], EXAMPLES['std::audio']):
                run([compiler.resolve(),'check',source])
                print('[通过语义检查] '+example+'；设备与显示测试由原生模块专项执行',flush=True)
                continue
            if example==EXAMPLES['std::reflect']:
                if not args.reflection_compiler:
                    print('[跳过] reflect：需 --reflection-compiler 指定启用反射的工具链',flush=True)
                    continue
                compiler=args.reflection_compiler
                flags=['--reflection']
            expected=ROOT/'example/expected'/name
            stdinText=expected.with_suffix('.in').read_text(encoding='utf-8') if expected.with_suffix('.in').exists() else None
            if example==EXAMPLES['std::io']: flags+=['--',work/'io.txt']
            output=run([compiler.resolve(),'run',source,'--out',work/name,*flags],
                       allowed_stderr='stackvm：按回车继续…' if name=='std_pause' else '',stdin=stdinText)
            expected_text=expected.with_suffix('.txt')
            if expected_text.exists(): assert output==expected_text.read_text(encoding='utf-8'),(name,output)
            elif name in ('io','sqlite','reflect'): assert output==name+' API ok\n',(name,output)
            passed+=1
            print('[通过] '+example,flush=True)
    print(f'库参考文档、重载清单、链接与 {passed} 个完整程序验证通过。')


if __name__=='__main__':
    main()
