"""验证学生系统的系统密码派生与 HTTP 长度、认证头边界。"""
from pathlib import Path
import hashlib
import os
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT=Path(__file__).resolve().parents[1]


def main():
    """以独立实现的测试向量验证 C ABI，而非复制生产实现得到期望值。"""
    vectors=[('password','salt',1),('password','salt',2),('口令abc','独立salt',2)]
    checks=[]
    for password,salt,iterations in vectors:
        expected=hashlib.pbkdf2_hmac('sha256',password.encode(),salt.encode(),iterations).hex()
        checks.append(f'assert(crypto::derive("{password}","{salt}",(uint64){iterations}).unwrap()=="{expected}");')
    code='''import crypto;
import httpd;
import school_auth;
import std::clock;
void main()
{
'''+ '\n'.join(checks)+'''
    let one=crypto::token();let two=crypto::token();
    assert(one.isOk()&&two.isOk());
    assert(len(one.unwrap())==64&&one.unwrap()!=two.unwrap());
    assert(crypto::equalDigest(one.unwrap(),one.unwrap()));
    assert(!crypto::equalDigest(one.unwrap(),two.unwrap()));
    assert(httpd::contentLength("GET / HTTP/1.1\\r\\n")==0);
    assert(httpd::contentLength("Content-Length: 1048576\\r\\n")==1048576);
    assert(httpd::contentLength("Content-Length: 1048577\\r\\n")==-1);
    assert(httpd::contentLength("Content-Length: -1\\r\\n")==-1);
    assert(httpd::contentLength("Content-Length: 1x\\r\\n")==-1);
    assert(httpd::contentLength("Content-Length: 1\\r\\nContent-Length: 1\\r\\n")==-1);
    assert(httpd::contentLength("Transfer-Encoding: chunked\\r\\n")==-1);
    httpd::Request request;
    request.headers="GET / HTTP/1.1\\r\\naUtHoRiZaTiOn: Bearer abc\\r\\n";
    assert(httpd::header(&request,"Authorization")=="Bearer abc");
    request.headers+="Authorization: Bearer different\\r\\n";
    assert(httpd::header(&request,"authorization")=="");
    school_auth::State state;
    school_auth::Session expired;
    expired.token=one.unwrap();expired.userId=1;expired.version=1;
    expired.expires=std::clock::nowMs()-1;
    state.sessions.push(expired);
    school_auth::Session live=expired;
    live.token=two.unwrap();live.expires=std::clock::nowMs()+60000;
    state.sessions.push(live);
    school_auth::prune(&state);
    assert(len(state.sessions)==1&&state.sessions[0].token==two.unwrap());
    println("系统密码派生与 HTTP 协议边界通过");
}
'''
    with tempfile.TemporaryDirectory(prefix='students-protocol-') as directory:
        work=Path(directory);source=work/'protocol.chl';source.write_text(code,encoding='utf-8')
        result=subprocess.run([str(compiler_path()),'run',str(source),'--import-dir',str(ROOT/'example/students-server/src'),'--out',str(work/'protocol')],cwd=ROOT,capture_output=True,text=True,encoding='utf-8',timeout=240)
        assert result.returncode==0,(result.stdout,result.stderr)
        assert result.stdout.strip()=='系统密码派生与 HTTP 协议边界通过',result.stdout
        print(result.stdout,end='')


if __name__=='__main__':main()
