"""验证书稿日志工具和 HTTP 文件服务器的实际原生产物。"""
import argparse
from concurrent.futures import ThreadPoolExecutor
import os
from pathlib import Path
import queue
import re
import socket
import subprocess
import threading
import time
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]


def command(arguments, cwd, expected=0):
    """运行有界子进程并保留失败时所需的输出。"""
    result = subprocess.run(list(map(str, arguments)), cwd=cwd, env=dict(os.environ, CHALI_LANG='zh'), capture_output=True, encoding='utf-8', timeout=300)
    assert (result.returncode == 0) == (expected == 0), (arguments, result.returncode, result.stdout, result.stderr)
    return result


def build(compiler, project, work):
    """每个工程导出一次原生程序，业务案例复用同一被测字节。"""
    prefix = work / project
    command([compiler, 'aot', ROOT / 'example' / project / 'src/main.chl', '--out', prefix], ROOT)
    executable = prefix.with_suffix('.exe' if os.name == 'nt' else '')
    assert executable.exists(), executable
    return executable


def log_checks(executable, work):
    """覆盖选项、原文、行号、边界和读取失败。"""
    path = work / 'input with spaces.log'
    path.write_bytes('INFO start\r\nERROR failed\r\n\r\nerror 重试\nlast ERROR'.encode())
    cases = [
        (['ERROR', path], '2:ERROR failed\n5:last ERROR\n'),
        (['--ignore-case', 'ERROR', path], '2:ERROR failed\n4:error 重试\n5:last ERROR\n'),
        (['--ignore-case', '--count', 'ERROR', path], '3\n'),
        (['--count', 'absent', path], '0\n'),
        (['absent', path], ''),
        (['重试', path], '4:error 重试\n'),
        (['--', '--literal', path], ''),
    ]
    for arguments, expected in cases:
        result = command([executable, *arguments], work)
        assert result.stdout == expected and result.stderr == '', (arguments, result)
    help_result = command([executable, '--help'], work)
    assert 'usage: log-search' in help_result.stdout and help_result.stderr == '', help_result
    for arguments, fragment in [([], 'usage:'), (['', path], 'query must not be empty'), (['--bad', 'x', path], 'unknown option'), (['x', work/'missing.log'], 'missing.log')]:
        result = command([executable, *arguments], work, 1)
        assert result.stdout == '' and fragment in result.stderr, result
    empty = work/'empty.log';empty.write_bytes(b'')
    assert command([executable, '--count', 'x', empty], work).stdout == '0\n'
    zero = work/'zero.log';zero.write_bytes(b'a\x00b')
    result = command([executable, 'a', zero], work, 1)
    assert 'zero byte' in result.stderr, result
    large = work/'large.log';large.write_bytes(b'x' * (4194304+1))
    result = command([executable, 'x', large], work, 1)
    assert 'byte limit' in result.stderr, result
    print('日志工具：15 组输入、读取与输出检查通过', flush=True)


def receive(peer):
    """读取连接关闭前的完整响应。"""
    data = b''
    while True:
        try:
            part = peer.recv(65536)
        except ConnectionResetError:
            # 拒绝超长请求时，尚未消费的入站字节可能使 Windows 在关闭后复位。
            # 返回已收到的数据；调用方仍必须核对完整响应头和准确正文长度。
            return data
        if not part:
            return data
        data += part


def http_checks(executable, work):
    """覆盖实际 TCP 请求、表示长度及多个连接同时等待。"""
    public = ROOT/'example/http-files/public'
    child = subprocess.Popen([str(executable), '0', str(public)], cwd=work, env=dict(os.environ, CHALI_LANG='zh'), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    lines = queue.Queue()
    reader = threading.Thread(target=lambda: lines.put(child.stderr.readline()), daemon=True)
    reader.start()
    try:
        ready = lines.get(timeout=15).decode('utf-8')
        assert re.fullmatch(r'listening 127\.0\.0\.1:\d+\r?\n', ready), ready
        port = int(ready.rsplit(':', 1)[1])
        def request(raw):
            with socket.create_connection(('127.0.0.1', port), timeout=20) as peer:
                peer.settimeout(20)
                peer.sendall(raw)
                return receive(peer)
        def verify(raw, status, body=None, head=False):
            data = request(raw)
            header, content = data.split(b'\r\n\r\n', 1)
            assert header.startswith(f'HTTP/1.1 {status} '.encode()), data
            length = int(re.search(br'(?im)^Content-Length: (\d+)\r?$', header)[1])
            assert b'Connection: close' in header and (head or length == len(content)), data
            if head:assert content == b'', data
            if body is not None:assert content == body, data
            return header
        get = lambda target: f'GET {target} HTTP/1.1\r\nHost: localhost\r\n\r\n'.encode()
        verify(get('/'), 200, (public/'index.html').read_bytes())
        verify(get('/notes.txt'), 200, (public/'notes.txt').read_bytes())
        verify(get('/style.css'), 200, (public/'style.css').read_bytes())
        header = verify(b'HEAD /notes.txt HTTP/1.1\r\nHost: localhost\r\n\r\n', 200, head=True)
        assert f'Content-Length: {(public/"notes.txt").stat().st_size}'.encode() in header
        verify(get('/notes.txt?download=1'), 200, (public/'notes.txt').read_bytes())
        for route in ['/missing', '/../secret.txt', '/%2e%2e/secret.txt', '/notes.txt/extra']:
            verify(get(route), 404)
        verify(b'GET / HTTP/1.0\r\n\r\n', 200, (public/'index.html').read_bytes())
        verify(b'GET / HTTP/1.1\r\n\r\n', 400)
        verify(b'GET / HTTP/1.1\r\nHost: a\r\nHost: b\r\n\r\n', 400)
        header=verify(b'POST / HTTP/1.1\r\nHost: a\r\n\r\n', 405)
        assert b'Allow: GET, HEAD' in header
        verify(b'GET / HTTP/9.9\r\nHost: a\r\n\r\n', 505)
        verify(b'GET / HTTP/1.1\r\nHost: a\r\nTransfer-Encoding: chunked\r\n\r\n', 400)
        verify(b'GET / HTTP/1.1\r\nHost: a\r\nContent-Length: 2\r\n\r\nxx', 400)
        verify(b'GET / HTTP/1.1\r\nHost: a\r\nBad Header: x\r\n\r\n', 400)
        verify(b'GET / HTTP/1.1\r\nHost: a\x00b\r\n\r\n', 400)
        verify(b'GET / HTTP/1.1\r\nHost: a\r\nX-Pad: '+b'x'*9000+b'\r\n\r\n', 431)
        with socket.create_connection(('127.0.0.1', port), timeout=20) as slow:
            slow.settimeout(20)
            slow.sendall(b'GET /notes.txt HTTP/1.1\r\nHost:')
            verify(get('/style.css'), 200, (public/'style.css').read_bytes())
            slow.sendall(b' localhost\r\n\r\n')
            assert receive(slow).endswith((public/'notes.txt').read_bytes())
        with ThreadPoolExecutor(max_workers=4) as pool:
            list(pool.map(lambda _: verify(get('/notes.txt'), 200, (public/'notes.txt').read_bytes()), range(8)))
        with socket.create_connection(('127.0.0.1', port), timeout=20) as idle:
            idle.settimeout(20)
            assert receive(idle).startswith(b'HTTP/1.1 408 ')
        print('HTTP 服务：GET／HEAD、中文字节长度、错误、路径、分段、并发等待与超时通过', flush=True)
    finally:
        if child.poll() is None:
            child.terminate()
            try:child.wait(timeout=10)
            except subprocess.TimeoutExpired:child.kill();child.wait()
        reader.join(timeout=2)
        child.stdout.close();child.stderr.close()
    # 有界运行必须自行退出，不能只依赖测试进程杀掉服务。
    child = subprocess.Popen([str(executable), '0', str(public), '1'], cwd=work, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    try:
        ready=child.stderr.readline().decode('utf-8');port=int(ready.rsplit(':',1)[1])
        with socket.create_connection(('127.0.0.1',port),timeout=20) as peer:
            peer.sendall(b'GET / HTTP/1.1\r\nHost: a\r\n\r\n');receive(peer)
        child.wait(timeout=10)
        assert child.returncode == 0 and child.stdout.read() == b'', child.stderr.read()
    finally:
        if child.poll() is None:child.kill();child.wait()
        child.stdout.close();child.stderr.close()
    for args in [['-1',public],['65536',public],['0',public,'-1'],['0',work/'missing-public']]:
        command([executable,*args],work,1)
    print('HTTP 服务：连接限额退出、资源收束及启动错误通过', flush=True)


def main():
    """在仓库临时目录构建并验证两个教学工程。"""
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali',type=Path,default=compiler_path())
    parser.add_argument('--project',choices=['all','log','http'],default='all')
    args=parser.parse_args()
    with tempfile.TemporaryDirectory(prefix='book-projects-') as folder:
        work=Path(folder)
        if args.project in ('all','log'):log_checks(build(args.chali.resolve(),'log-search',work),work)
        if args.project in ('all','http'):http_checks(build(args.chali.resolve(),'http-files',work),work)
    print('两个书稿项目的原生功能验收通过',flush=True)


if __name__=='__main__':main()
