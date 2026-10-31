"""核对同步／异步 TCP 的回环行为，不访问外网。"""
from pathlib import Path
import argparse
import os
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile
ROOT=Path(__file__).resolve().parents[1]
def main():
    """用两分配器及两安全配置执行现有确定性网络样例。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=compiler_path())
    args=parser.parse_args();compiler=args.chali.resolve()
    with tempfile.TemporaryDirectory(prefix='net-c-api-') as directory:
        work=Path(directory)
        for allocator in ('malloc','pool'):
            for checks in ('checked','unchecked'):
                for name in ('std_net','std_net_async'):
                    prefix=work/f'{name}-{allocator}-{checks}'
                    def run(command):
                        result=subprocess.run(list(map(str,command)),cwd=ROOT,env=dict(os.environ,CHALI_LANG='zh'),capture_output=True,text=True,encoding='utf-8',errors='replace',timeout=300)
                        assert result.returncode==0,(command,result.returncode,result.stdout,result.stderr)
                        return result
                    run([compiler,'aot',ROOT/'example/samples'/f'{name}.chl','--allocator',allocator,'--'+checks,'--out',prefix])
                    result=run([prefix.with_suffix('.exe' if os.name=='nt' else '')])
                    expected=(ROOT/'example/expected'/f'{name}.txt').read_text(encoding='utf-8')
                    assert result.stdout==expected and not result.stderr,result
                    print(f'{allocator}/{checks}/{name}: loopback transfers, status, close and errors passed',flush=True)
    print('Synchronous and asynchronous TCP loopback configurations passed')
if __name__=='__main__':main()
