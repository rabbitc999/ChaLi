"""验证 LLVM 原生产物中的反射元数据、注解与关闭开关，不生成四元式文件。"""
from compiler_path import compiler_path
from pathlib import Path
import argparse
import os
import subprocess
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
EXE = '.exe' if os.name == 'nt' else ''


def main():
    """使用公开查询验证实际原生产物里的字段、身份及独立拥有值。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    chali = parser.parse_args().chali.resolve()
    env = dict(os.environ, CHALI_LANG='zh', CHALI_NO_STDIN_INPUT='1')
    checks = 0

    def run(command, success=True):
        nonlocal checks
        result = subprocess.run(list(map(str, command)), cwd=ROOT, env=env, capture_output=True,
                                text=True, encoding='utf-8', errors='replace', timeout=600)
        assert (result.returncode == 0) == success, (command, result.stdout, result.stderr)
        checks += 1
        return result

    with tempfile.TemporaryDirectory(prefix='reflection-metadata-') as directory:
        work = Path(directory)
        plain = work / 'plain.chl'
        plain.write_text('void main(){}\n', encoding='utf-8')
        for enabled in (False, True):
            prefix = work / ('plain-on' if enabled else 'plain-off')
            run([chali, 'build', plain, '--emit-llvm', '--reflection' if enabled else '--no-reflection', '--out', prefix])
            assert prefix.with_suffix('.ll').is_file()
            assert not prefix.with_suffix('.quad').exists()
            run([chali, 'build', plain, '--reflection' if enabled else '--no-reflection', '--out', prefix])
            assert run([str(prefix) + EXE]).stdout == ''
        annotated = work / 'annotated.chl'
        annotated.write_text('''import std::reflect;
@annotation Label(text:String="ok")
@Reflect @Label
struct Data{int64 value;String text;}
@Reflect @Label
int64 plus(int64 value){return value+1;}
void main(){
    let info=std::reflect::getStructInfo<Data>().unwrap();
    assert(info.name=="program::Data"&&info.typeId==std::reflect::getTypeId<Data>());
    assert(info.fields.len()==2&&info.fields[0].name=="value"&&info.fields[1].name=="text");
    assert(info.fields[0].offset==(uint32)0&&info.fields[1].offset==(uint32)8&&info.fields[1].isOwned);
    let label=std::reflect::getAnnotationInfo("program::Label").unwrap();
    bool found=false;
    for(std::reflect::Annotation annotation in info.annotations){
        if(annotation.annotationId==label.typeId){
            assert(annotation.args.len()==1);
            match(annotation.args[0]){std::reflect::ReflectValue::Text(text)=>{assert(text=="ok");found=true;}_=>{assert(false);}}
        }
    }
    assert(found);
    info.name="changed";
    assert(std::reflect::getStructInfo<Data>().unwrap().name=="program::Data");
    let function=std::reflect::getFuncInfo("program::plus").unwrap();
    assert(function.params.len()==1&&function.returnTypeId==std::reflect::getTypeId<int64>());
    println("reflection metadata OK");
}
''', encoding='utf-8')
        prefix = work / 'annotated'
        run([chali, 'build', annotated, '--reflection', '--out', prefix])
        assert run([str(prefix) + EXE]).stdout == 'reflection metadata OK\n'
        bad = work / 'bad.chl'
        bad.write_text('@annotation Label(text:int32)\n@Label("wrong")\nvoid main(){}\n', encoding='utf-8')
        run([chali, 'build', bad, '--emit-llvm', '--reflection', '--out', work / 'bad'], success=False)
    print(f'反射原生元数据：{checks} 项集成检查通过')


if __name__ == '__main__':
    main()
