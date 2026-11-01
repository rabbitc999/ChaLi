"""库包的跨实现字节对照、损坏拒绝、预算、目标和事务发布验收。"""
from pathlib import Path
import argparse
import copy
import hashlib
import json
import os
import struct
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT=Path(__file__).resolve().parents[1]
MAGIC=b'CHLIB1\r\n'


def invoke(arguments,work,success=True):
    """真实运行格式工具，失败时保留原始诊断。"""
    environment=dict(os.environ)
    if os.name=='nt': environment['CHALI_LLVM_CLANG']=str(ROOT/'build/dependencies/llvm/toolchain/bin/clang.exe')
    result=subprocess.run(list(map(str,arguments)),cwd=work,env=environment,capture_output=True,encoding='utf-8',timeout=180)
    assert (result.returncode==0)==success,(arguments,result.returncode,result.stdout,result.stderr)
    return result


def encode(manifest,entries,*,sort=True):
    """独立按公开小端格式构造损坏／扩展用例，摘要交给 hashlib。"""
    metadata=json.dumps(manifest,ensure_ascii=False,separators=(',',':')).encode()
    result=MAGIC+struct.pack('<IIQ',1,len(entries),len(metadata))+hashlib.sha256(metadata).digest()+metadata
    for entry in sorted(entries,key=lambda item:item['name']) if sort else entries:
        name=entry['name'].encode();data=entry['data']
        header=struct.pack('<HHIQ',len(name),entry['kind'],entry.get('flags',0),len(data))
        result+=header+hashlib.sha256(header+name+data).digest()+name+data
    return result


def decode(data):
    """独立解码并核对每个实际字节的规范编码。"""
    assert data[:8]==MAGIC
    version,count,size=struct.unpack_from('<IIQ',data,8);assert version==1
    metadata=data[56:56+size];assert hashlib.sha256(metadata).digest()==data[24:56]
    manifest=json.loads(metadata);position=56+size;entries=[]
    for _ in range(count):
        header=data[position:position+16];name_size,kind,flags,payload_size=struct.unpack('<HHIQ',header)
        digest=data[position+16:position+48];position+=48
        name=data[position:position+name_size];position+=name_size
        payload=data[position:position+payload_size];position+=payload_size
        assert hashlib.sha256(header+name+payload).digest()==digest
        entries.append(dict(name=name.decode(),kind=kind,flags=flags,data=payload))
    assert position==len(data) and encode(manifest,entries)==data
    return manifest,entries


def main():
    """验证格式层；原生执行使用现有链接器，不冒充尚未接入的语言库导入。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=compiler_path())
    parser.add_argument('--other-chali',type=Path,help='使用完全相同的输入核对另一 LLVM 变体')
    args=parser.parse_args();compiler=args.chali.resolve()
    with tempfile.TemporaryDirectory(prefix='library-package-') as folder:
        work=Path(folder);assets=work/'assets = 中文 &';assets.mkdir()
        interface=assets/'public.chl';interface.write_text('module example::math;\n#abi("C","@link","add") int64 add(int64 a,int64 b);\n',encoding='utf-8')
        license_file=assets/'LICENSE.txt';license_file.write_text('Example library license\n',encoding='utf-8')
        targets={};archives={}
        for target in ['windows-x64','linux-x64','linux-arm64']:
            manifest_path=work/(target+'.json')
            invoke([compiler,'lib','template','--module','example::math','--package-version','1.2.3','--target',target,'--license','Apache-2.0','--license-asset','legal/LICENSE.txt','--out',manifest_path],work)
            manifest=json.loads(manifest_path.read_text(encoding='utf-8'))
            current=manifest['targets'][0];targets[target]=current
            runtime={'windows-x64':'runtime_windows.ll','linux-x64':'runtime_linux.ll','linux-arm64':'runtime_linux_arm64.ll'}[target]
            assert 'target datalayout = "'+current['dataLayout']+'"' in (ROOT/'source/aot'/runtime).read_text(encoding='utf-8')
            assembly=assets/(target+'.s')
            body='add x0,x0,x1\nret\n' if target=='linux-arm64' else 'leaq (%rcx,%rdx),%rax\nretq\n' if target=='windows-x64' else 'leaq (%rdi,%rsi),%rax\nretq\n'
            assembly.write_text('.text\n.globl add\nadd:\n'+body,encoding='utf-8')
            obj=assets/(target+'.o');archive=assets/(target+'.a')
            invoke([compiler,'asm',assembly,'--target',target,'--out',obj],work)
            invoke([compiler,'ar',obj,'--target',target,'--out',archive],work);archives[target]=archive
        host='windows-x64' if os.name=='nt' else 'linux-arm64' if 'aarch64' in os.uname().machine.lower() else 'linux-x64'
        manifest=json.loads((work/(host+'.json')).read_text(encoding='utf-8'))
        manifest_path=work/'manifest.json';manifest_path.write_text(json.dumps(manifest,separators=(',',':')),encoding='utf-8')
        package=work/'math.chlib'
        entry_args=['--entry','interface:interface/main.chl='+str(interface),'--entry','license:legal/LICENSE.txt='+str(license_file),
                    '--entry','archive:'+manifest['targets'][0]['implementation']+'='+str(archives[host])]
        invoke([compiler,'lib','pack','--manifest',manifest_path,*entry_args,'--out',package],work)
        original=package.read_bytes();base,entries=decode(original)
        info=json.loads(invoke([compiler,'lib','inspect',package,'--module','example::math','--package-version','1.2.3','--target',host],work).stdout)
        assert info['sha256']==hashlib.sha256(original).hexdigest() and info['manifest']==manifest
        for item in info['entries']:
            entry=next(entry for entry in entries if entry['name']==item['name'])
            assert item['sha256']==hashlib.sha256(entry['data']).hexdigest() and item['bytes']==len(entry['data'])
        reverse=[]
        for offset in reversed(range(0,len(entry_args),2)): reverse+=entry_args[offset:offset+2]
        invoke([compiler,'lib','pack','--manifest',manifest_path,*reverse,'--out',package],work)
        assert package.read_bytes()==original
        if args.other_chali:
            other=work/'other.chlib'
            invoke([args.other_chali.resolve(),'lib','pack','--manifest',manifest_path,*entry_args,'--out',other],work)
            assert other.read_bytes()==original
            invoke([args.other_chali.resolve(),'lib','inspect',package,'--target',host],work)
        # 同一清单携带全部三个对象目标；仅宿主执行，交叉目标检查格式与选择。
        multi=copy.deepcopy(base);multi['targets']=sorted(targets.values(),key=lambda value:value['triple'])
        multi_entries=[item for item in entries if item['kind']!=4]
        multi_entries += [dict(name=targets[name]['implementation'],kind=4,data=archives[name].read_bytes()) for name in targets]
        multi_path=work/'multi.chlib';multi_path.write_bytes(encode(multi,multi_entries))
        for target in targets: invoke([compiler,'lib','inspect',multi_path,'--target',target],work)
        # 未知可忽略载荷不作为可执行实现，未知必需种类和标志必须拒绝。
        optional=work/'optional.chlib';optional.write_bytes(encode(base,entries+[dict(name='future/data',kind=65535,flags=1,data=b'\0\1\2')]))
        assert json.loads(invoke([compiler,'lib','inspect',optional],work).stdout)['entries'][0]['optional']
        failures=[]
        for key,value in [('compilerVersion','1.0.11'),('interfaceFormat',2),('languageAbi',2),('runtimeAbi',2),('requiredFeatures',['unknown-feature']),('interface','absent.chl')]:
            bad=copy.deepcopy(base);bad[key]=value;failures.append(encode(bad,entries))
        for key,value in [('pointerBits',32),('endian','big'),('dataLayout','invalid'),('nativeAbi','unknown'),('triple','linux-x64')]:
            bad=copy.deepcopy(base);bad['targets'][0][key]=value;failures.append(encode(bad,entries))
        for dependency in [dict(module='bad/path',version='1.0.0',sha256='0'*64),dict(module='example::math',version='1.0.0',sha256='0'*64),dict(module='other',version='1.0.0',sha256='wrong')]:
            bad=copy.deepcopy(base);bad['dependencies']=[dependency];failures.append(encode(bad,entries))
        other_target=next(name for name in targets if name!=host)
        bad=copy.deepcopy(base);bad['targets'][0]=copy.deepcopy(targets[other_target]);bad['targets'][0]['implementation']=base['targets'][0]['implementation'];failures.append(encode(bad,entries))
        failures += [encode(base,entries+[dict(name='unknown/data',kind=65535,data=b'')]),
                     encode(base,entries+[dict(name='../escape',kind=8,data=b'text')]),
                     encode(base,entries+[dict(name='flag/data',kind=8,flags=4,data=b'text')]),
                     encode(base,entries+entries[:1]),encode(base,list(reversed(entries)),sort=False),original+b'extra']
        changed=copy.deepcopy(entries);changed[0]['data']+=b'\0';failures.append(encode(base,changed))
        failures.append(encode(base,[entry for entry in entries if entry['kind']!=4]))
        bad=copy.deepcopy(base);bad['unexpected']=1;failures.append(encode(bad,entries))
        for count in [0,4097]:
            bad=bytearray(original);struct.pack_into('<I',bad,12,count);failures.append(bytes(bad))
        bad=bytearray(original);struct.pack_into('<Q',bad,16,1024*1024+1);failures.append(bytes(bad))
        entry_offset=56+struct.unpack_from('<Q',original,16)[0]
        for name_size in [0,241]:
            bad=bytearray(original);struct.pack_into('<H',bad,entry_offset,name_size);failures.append(bytes(bad))
        bad=bytearray(original);struct.pack_into('<Q',bad,entry_offset+8,2**64-1);failures.append(bytes(bad))
        for position in [0,8,12,16,24,56,len(original)-1]:
            bad=bytearray(original);bad[position]^=0x80;failures.append(bytes(bad))
        for length in [0,7,15,55,len(original)-1]:failures.append(original[:length])
        for index,data in enumerate(failures):
            path=work/f'bad-{index}.chlib';path.write_bytes(data)
            invoke([compiler,'lib','inspect',path],work,False)
        for flags in [['--module','wrong'],['--package-version','9.9.9'],['--target',other_target]]:
            invoke([compiler,'lib','inspect',package,*flags],work,False)
        sparse=work/'oversize.chlib'
        with sparse.open('wb') as handle:handle.truncate(256*1024*1024+1)
        invoke([compiler,'lib','inspect',sparse],work,False)
        # 失败不得破坏原包、输入清单或资产。
        bad_object=assets/'wrong.a';bad_object.write_bytes(b'bad object')
        wrong_args=entry_args[:-2]+['--entry','archive:'+base['targets'][0]['implementation']+'='+str(bad_object)]
        invoke([compiler,'lib','pack','--manifest',manifest_path,*wrong_args,'--out',package],work,False)
        assert package.read_bytes()==original
        for path in [manifest_path,interface,archives[host]]:
            snapshot=path.read_bytes()
            invoke([compiler,'lib','pack','--manifest',manifest_path,*entry_args,'--out',path],work,False)
            assert path.read_bytes()==snapshot
        invoke([compiler,'lib','pack','--manifest',manifest_path,*entry_args,'--out',work/'absent/out.chlib'],work,False)
        assert not list(work.glob('*.chali-*.tmp'))
        # 格式往返后的真实原生负载仍可供独立链接器执行。
        provider=work/'provider.a';provider.write_bytes(next(entry['data'] for entry in entries if entry['kind']==4))
        source=work/'main.c';source.write_text('/**\n * @brief 验证从库包恢复的原生归档。\n * @param a 左值。\n * @param b 右值。\n * @return 两数之和。\n */\nextern long long add(long long a,long long b);\n/**\n * @brief 运行真实 C ABI 调用。\n * @return 结果正确时为零。\n */\nint main(void){return add(20,22)==42?0:1;}\n',encoding='utf-8')
        obj=work/'main.o';invoke(['gcc','-c',source,'-o',obj],work)
        executable=work/('main.exe' if os.name=='nt' else 'main')
        invoke([compiler,'link',obj,provider,'--out',executable],work);invoke([executable],work)
    print('library container: deterministic wire format, SHA-256, budgets, native targets, optional entries, rejection and transaction checks passed')


if __name__=='__main__':
    main()
