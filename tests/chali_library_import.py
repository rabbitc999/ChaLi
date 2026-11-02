"""普通 import 消费无实现源码的原生库，并验证内容锁及错误依赖。"""
from pathlib import Path
import argparse
import copy
import hashlib
import json
import os
from chali_library_package import invoke, encode
from compiler_path import compiler_path
from project_temp import tempfile
from native_lsp_transport import Transport


def main():
    """构造实际机器归档和隔离消费者，不依赖提供方实现源码。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=compiler_path())
    compiler=parser.parse_args().chali.resolve()
    with tempfile.TemporaryDirectory(prefix='library-import-') as directory:
        work=Path(directory)
        target='windows-x64' if os.name=='nt' else 'linux-arm64' if 'aarch64' in os.uname().machine.lower() else 'linux-x64'
        source=work/'add.s'
        source.write_text('.text\n.globl packaged_add\npackaged_add:\n'+('add x0,x0,x1\nret\n' if target=='linux-arm64' else 'leaq (%rcx,%rdx),%rax\nretq\n' if os.name=='nt' else 'leaq (%rdi,%rsi),%rax\nretq\n'))
        obj=work/'add.o';archive=work/'add.a'
        invoke([compiler,'asm',source,'--out',obj],work)
        invoke([compiler,'ar',obj,'--out',archive],work)
        metadata=work/'manifest.json'
        invoke([compiler,'lib','template','--module','example::math','--package-version','1.0.0','--target',target,'--out',metadata],work)
        manifest=json.loads(metadata.read_text())
        entries=[dict(name='interface/main.chl',kind=1,data=b'module example::math;\nstruct Pair {int64 x;int64 y;};\n#abi("C","@link","packaged_add") int64 add(int64 a,int64 b);\n'),
                 dict(name=manifest['targets'][0]['implementation'],kind=4,data=archive.read_bytes())]
        modules=work/'modules'/'example';modules.mkdir(parents=True)
        package=modules/'math.chlib';package.write_bytes(encode(manifest,entries))
        source.unlink();obj.unlink();archive.unlink();metadata.unlink()
        consumer=work/'consumer.chl'
        consumer.write_text('import example::math;\nvoid main(){example::math::Pair pair;pair.x=20;pair.y=22;assert(example::math::add(pair.x,pair.y)==42);println("package import OK");}\n')
        invoke([compiler,'--check',consumer],work)
        output=work/'consumer'
        invoke([compiler,'aot',consumer,'--checked','--out',output],work)
        executable=output.with_suffix('.exe') if os.name=='nt' else output
        assert invoke([executable],work).stdout=='package import OK\n'
        assert invoke([compiler,'run',consumer,'--out',work/'checked-run'],work).stdout=='package import OK\n'
        (modules/'math.chl').write_text('module example::math;\nint64 add(int64 a,int64 b){return 0;}\n')
        original=package.read_bytes()
        lock=work/'chali.libraries.lock'
        invoke([compiler,'lib','lock',package,'--out',lock],work)
        locked=lock.read_bytes()
        assert json.loads(locked)['modules']==[dict(module='example::math',version='1.0.0',sha256=hashlib.sha256(original).hexdigest())]
        invoke([compiler,'lib','lock',package,'--out',lock],work)
        assert lock.read_bytes()==locked
        invoke([compiler,'check',consumer,'--library-lock',lock],work)
        dependencies=invoke([compiler,'--deps',consumer,'--library-lock',lock],work).stdout.replace('\\','/')
        assert str(lock.resolve()).replace('\\','/') in dependencies,dependencies

        # 工程清单选择真实内容锁；不传命令行锁也须解析包并追踪锁依赖。
        project=work/'chali.toml'
        project.write_text('[aot]\nlibrary_lock = "chali.libraries.lock"\n',encoding='utf-8')
        invoke([compiler,'check',consumer],work)
        configured=invoke([compiler,'--deps',consumer],work).stdout.replace('\\','/')
        assert str(lock.resolve()).replace('\\','/') in configured,configured
        assert str(package.resolve()).replace('\\','/') in configured,configured
        lock.write_text('{}',encoding='utf-8')
        invoke([compiler,'check',consumer],work,False)
        lock.write_bytes(locked)
        invoke([compiler,'check',consumer],work)
        server=Transport(compiler,work)
        consumer_uri=consumer.as_uri()
        try:
            server.query('initialize',dict(rootUri=work.as_uri()))
            server.send(dict(method='initialized',params={}))
            server.send(dict(method='textDocument/didOpen',params=dict(textDocument=dict(uri=consumer_uri,languageId='chali',version=1,text=consumer.read_text()))))
            server.diagnostic(consumer_uri,1,lambda values:not values)
            for dependency,original_bytes in ((lock,locked),(package,original)):
                cursor=len(server.notifications)
                dependency.write_bytes(b'{}')
                server.send(dict(method='workspace/didChangeWatchedFiles',params=dict(changes=[dict(uri=dependency.as_uri(),type=2)])))
                server.diagnostic(consumer_uri,1,lambda values:bool(values),after=cursor)
                cursor=len(server.notifications)
                dependency.write_bytes(original_bytes)
                server.send(dict(method='workspace/didChangeWatchedFiles',params=dict(changes=[dict(uri=dependency.as_uri(),type=2)])))
                server.diagnostic(consumer_uri,1,lambda values:not values,after=cursor)
        finally:
            server.close()
        project.unlink()

        object_prefix=work/'separate'
        invoke([compiler,'aot',consumer,'--emit-obj','--library-lock',lock,'--out',object_prefix],work)
        linked=work/('standalone.exe' if os.name=='nt' else 'standalone')
        invoke([compiler,'link',object_prefix.with_suffix('.o'),'--package',package,'--library-lock',lock,'--out',linked],work)
        assert invoke([linked],work).stdout=='package import OK\n'
        preserved=linked.read_bytes()
        stale=copy.deepcopy(manifest);stale['version']='1.0.1'
        package.write_bytes(encode(stale,entries))
        invoke([compiler,'check',consumer,'--library-lock',lock],work,False)
        invoke([compiler,'link',object_prefix.with_suffix('.o'),'--package',package,'--library-lock',lock,'--out',linked],work,False)
        assert linked.read_bytes()==preserved
        package.unlink()
        invoke([compiler,'check',consumer,'--library-lock',lock],work,False)
        package.write_bytes(original)
        for destination in [package,lock]:
            before=destination.read_bytes()
            invoke([compiler,'link',object_prefix.with_suffix('.o'),'--package',package,'--library-lock',lock,'--out',destination],work,False)
            assert destination.read_bytes()==before
        invoke([compiler,'lib','lock',package,'--out',package],work,False)
        assert package.read_bytes()==original
        for invalid in [b'{}',locked.replace(b'"format":1',b'"format":2'),locked.rstrip()[:-1]+b',"unknown":0}',b'{"format":1,"compilerVersion":"1.0.12","modules":[]}']:
            lock.write_bytes(invalid)
            invoke([compiler,'check',consumer,'--library-lock',lock],work,False)
        duplicate=json.loads(locked);duplicate['modules']*=2
        for invalid in [json.dumps(duplicate,separators=(',',':')).encode(),b'{"format":1,'+locked.lstrip()[1:]]:
            lock.write_bytes(invalid)
            invoke([compiler,'check',consumer,'--library-lock',lock],work,False)
        with lock.open('wb') as handle:handle.truncate(1024*1024+1)
        invoke([compiler,'check',consumer,'--library-lock',lock],work,False)
        lock.write_bytes(locked)

        for implementation in [b'int64 secret(){return 1;}\n', b'int64 state=7;\n']:
            invalid=copy.deepcopy(entries)
            invalid[0]['data']+=implementation
            package.write_bytes(encode(manifest,invalid))
            result=invoke([compiler,'--check',consumer],work,False)
            assert 'c-v1' in result.stderr+result.stdout
        package.write_bytes(original)

        bad=copy.deepcopy(manifest);bad['module']='wrong::identity'
        package.write_bytes(encode(bad,entries))
        invoke([compiler,'--check',consumer],work,False)
        package.write_bytes(original[:-1])
        invoke([compiler,'--check',consumer],work,False)
        package.write_bytes(original)
        # 实现依赖即使未出现在公开 import 中，也必须加载并精确锁定。
        dependency=copy.deepcopy(manifest);dependency['module']='example::dependency'
        dependency_entries=copy.deepcopy(entries)
        dependency_entries[0]['data']=b'module example::dependency;\n'
        dependency_data=encode(dependency,dependency_entries)
        dependency_path=modules/'dependency.chlib';dependency_path.write_bytes(dependency_data)
        unlisted=copy.deepcopy(entries)
        unlisted[0]['data']=unlisted[0]['data'].replace(b'module example::math;\n',b'module example::math;\nimport example::dependency;\n')
        package.write_bytes(encode(manifest,unlisted))
        result=invoke([compiler,'check',consumer],work,False)
        assert '清单' in result.stdout+result.stderr
        pinned=copy.deepcopy(manifest)
        pinned['dependencies']=[dict(module='example::dependency',version='1.0.0',sha256=hashlib.sha256(dependency_data).hexdigest())]
        package.write_bytes(encode(pinned,entries))
        # 已找到的包提供其完整模块根，传递依赖无需重复指定搜索目录。
        invoke([compiler,'--check',consumer],work)
        invoke([compiler,'aot',consumer,'-I',work/'modules','--out',work/'with-dependency'],work)
        before=lock.read_bytes()
        invoke([compiler,'lib','lock',package,'--out',lock],work,False)
        assert lock.read_bytes()==before
        invoke([compiler,'lib','lock',package,dependency_path,'--out',lock],work)
        closure=lock.read_bytes()
        invoke([compiler,'lib','lock',dependency_path,package,'--out',lock],work)
        assert lock.read_bytes()==closure
        invoke([compiler,'check',consumer,'--library-lock',lock],work)
        invoke([compiler,'aot',consumer,'--emit-obj','--library-lock',lock,'--out',object_prefix],work)
        invoke([compiler,'link',object_prefix.with_suffix('.o'),'--package',package,'--library-lock',lock,'--out',linked],work)
        assert invoke([linked],work).stdout=='package import OK\n'

        dependency_before=dependency_path.read_bytes()
        invoke([compiler,'link',object_prefix.with_suffix('.o'),'--package',package,'--library-lock',lock,'--out',dependency_path],work,False)
        assert dependency_path.read_bytes()==dependency_before
        pinned['dependencies'][0]['sha256']='0'*64
        package.write_bytes(encode(pinned,entries))
        invoke([compiler,'--check',consumer,'-I',work/'modules'],work,False)
        dependency_path.unlink()
        invoke([compiler,'--check',consumer,'-I',work/'modules'],work,False)
        assert executable.exists()
    print('library imports OK')


if __name__=='__main__':
    main()
