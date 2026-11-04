"""直接链接 C ABI 的完整封送、标量原语、回调与失败清理验收。"""
from pathlib import Path
import argparse
import os
import re
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def main():
    """同一语言例程以原生对象提供实现，消费者不加载动态库。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    compiler = parser.parse_args().chali.resolve()
    with tempfile.TemporaryDirectory(prefix='linked-abi-') as directory:
        work = Path(directory)
        environment = {**os.environ, 'CHALI_LLVM_CLANG': str(ROOT/'build/dependencies/llvm/toolchain/bin/clang.exe')} if os.name == 'nt' else dict(os.environ)

        def invoke(arguments, success=True, extra=None):
            """检查真实原生执行及原始失败文本。"""
            result = subprocess.run(list(map(str, arguments)), cwd=ROOT, env={**environment, **(extra or {})},
                                    capture_output=True, encoding='utf-8', timeout=180)
            assert (result.returncode == 0) == success, (arguments, result.stdout, result.stderr)
            return result

        scalars = [('int8','int8_t','-61'), ('uint8','uint8_t','205'), ('int16','int16_t','-30000'),
                   ('uint16','uint16_t','60000'), ('int32','int32_t','-123456'), ('uint32','uint32_t','4000000000'),
                   ('int64','int64_t','-1234567890123'), ('uint64','uint64_t','-1'),
                   ('float32','float','1.25'), ('float64','double','-2.5'), ('bool','_Bool','true')]
        native_source = '#include <stdint.h>\n'
        declarations, assertions = '', ''
        for index, (language, ctype, value) in enumerate(scalars):
            native_source += ('/**\n * @brief 原样传回一个固定宽度标量。\n'
                              ' * @param value 原生输入值。\n * @return 相同位模式的原生值。\n */\n'
                              f'{ctype} identity_{index}({ctype} value){{return value;}}\n')
            declarations += f'#abi("C","@link","identity_{index}") {language} identity_{index}({language} value);\n'
            assertions += f'assert(identity_{index}(({language}){value})==({language}){value});'
        provider = work/'scalars.c'; provider.write_text(native_source, encoding='utf-8')
        native = work/'scalars.o'; invoke(['gcc','-O2','-c',provider,'-o',native])
        source = work/'scalars.chl'; source.write_text(declarations+'void main(){'+assertions+'println("scalars OK");}', encoding='utf-8')
        output = work/'scalars'
        invoke([compiler,'aot',source,'--link',native,'--out',output], extra={'CHALI_ABI_CXX':str(work/'missing-cxx')})
        assert not output.with_suffix('.abi.cpp').exists()
        assert invoke([output.with_suffix('.exe') if os.name=='nt' else output]).stdout == 'scalars OK\n'
        for target, small_extension, bool_extension in [
            ('linux-x64', 'signext ', 'zeroext '),
            ('linux-arm64', '', ''),
            ('windows-x64', '', 'zeroext ')]:
            prefix = work/target
            invoke([compiler,'aot',source,'--emit-llvm','--target',target,'--out',prefix])
            ir = prefix.with_suffix('.ll').read_text(encoding='utf-8')
            assert 'declare '+small_extension+'i8 @"identity_0"(i8'+(' '+small_extension.strip() if small_extension else '')+')' in ir
            assert 'declare '+bool_extension+'i1 @"identity_10"(i1'+(' '+bool_extension.strip() if bool_extension else '')+')' in ir
            assert 'call '+small_extension+'i8 @"identity_0"(i8 '+small_extension in ir
            assert 'call '+bool_extension+'i1 @"identity_10"(i1 '+bool_extension in ir
            for index, bits, signed in [(1,8,False),(2,16,True),(3,16,False)]:
                extension = ('signext ' if signed else 'zeroext ') if target=='linux-x64' else ''
                llvm_type = 'i'+str(bits)
                assert 'declare '+extension+llvm_type+' @"identity_'+str(index)+'"('+llvm_type+(' '+extension.strip() if extension else '')+')' in ir
        invalid = work/'invalid.chl'
        invalid.write_text('#abi("C","", "identity") int32 identity(int32 x);void main(){}', encoding='utf-8')
        result = invoke([compiler,'--check',invalid], False)
        assert 'abi' in result.stdout.lower()+result.stderr.lower()
        for source_text in (
            'struct Packet{int64 value;};#abi("C","@link","bad") Packet bad(Packet value);',
            '#abi("C","@link","bad") int64 bad(int64 mut& value);',
            '#abi("C","@link","bad") String bad() ["free_owned"];',
            '#abi("C","@link","bad") int64 bad(int64 fn() cb);',
        ):
            invalid.write_text(source_text + 'void main(){}', encoding='utf-8')
            result = invoke([compiler, '--check', invalid], False)
            assert '@link' in result.stdout + result.stderr, result
        exported = work/'exported.chl'
        exported.write_text('struct Packet{int32 count;float64 value;};\n'
                            '#export("invert") bool invert(bool value){return !value;}\n'
                            '#export("small") int8 small(int8 value){return value;}\n'
                            '#export("transform") Packet transform(Packet value){value.count+=1;value.value+=0.5;return value;}\n'
                            'void main(){}', encoding='utf-8')
        prefix = work/'exported'
        invoke([compiler,'aot',exported,'--shared','--out',prefix])
        header = prefix.with_suffix('.h')
        struct_name = re.search(r'typedef struct (\w+)',header.read_text(encoding='utf-8')).group(1)
        consumer = work/'consumer.c'
        consumer.write_text('#include "'+header.as_posix()+'"\n'
                            '/**\n * @brief 从独立 C11 宿主验证语言导出的平台 ABI。\n'
                            ' * @return 全部值、布局和初始化正确时为零。\n */\n'
                            'int main(void){chali_init();'
                            'if(invert(true)||!invert(false)||small(-61)!=-61)return 1;'
                            +struct_name+' input={4,2.5};'+struct_name+' output=transform(input);'
                            'int result=output.f0!=5||output.f1!=3.0;chali_finish();return result;}\n', encoding='utf-8')
        library = prefix.with_suffix('.dll' if os.name=='nt' else '.so')
        executable = work/('consumer.exe' if os.name=='nt' else 'consumer')
        invoke(['gcc','-std=c11',consumer,library,*(['-Wl,-rpath,$ORIGIN'] if os.name!='nt' else []),'-o',executable])
        invoke([executable])
        reference = work/'layout-reference.c'
        reference.write_text('#include "'+header.as_posix()+'"\n#include <stddef.h>\n'
                             '_Static_assert(sizeof('+struct_name+')==16,"C structure size");\n'
                             '_Static_assert(_Alignof('+struct_name+')==8,"C structure alignment");\n'
                             '_Static_assert(offsetof('+struct_name+',f1)==8,"C field offset");\n'
                             '/**\n * @brief 以目标 C 编译器核对结构体传参和返回约定。\n'
                             ' * @param value 原生布局的值。\n * @return 转发后的原生结构体。\n */\n'
                             +struct_name+' forward('+struct_name+' value){return transform(value);}\n', encoding='utf-8')
        clang = environment.get('CHALI_LLVM_CLANG','clang')
        for target in ['x86_64-w64-windows-gnu','x86_64-unknown-linux-gnu','aarch64-unknown-linux-gnu']:
            reference_ir = work/(target+'-layout.ll')
            invoke([clang,'--target='+target,'-std=c11','-ffreestanding','-S','-emit-llvm','-O1',reference,'-o',reference_ir])
            generated = reference_ir.read_text(encoding='utf-8')
            assert '@transform(' in generated and target in generated
            if 'windows' in target: assert 'sret(' in generated
    print('linked C ABI: scalar widths, target extensions, complex direct-link rejection and C exports passed')


if __name__ == '__main__':
    main()
