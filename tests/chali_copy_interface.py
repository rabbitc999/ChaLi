"""检查 Copy<T> 的签名、泛型约束和不可复制资源边界。"""
from pathlib import Path
import argparse
import os
import subprocess
from compiler_path import compiler_path
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
REJECTED = {
    'missing_method': 'struct Missing:Copy<Missing>{int64 value;}; void main(){Missing value;}',
    'wrong_result': 'struct Wrong:Copy<Wrong>{int64 copy(){return 0;}}; void main(){Wrong value;}',
    'noncopyable': 'struct Resource:Closeable,Copy<Resource>{void close(){} Resource copy(){return self;}}; void main(){Resource value;Resource duplicate=value.copy();}',
    'missing_constraint': 'struct Plain{int64 value;}; T duplicate<T:Copy<T>>(T const& value){return value.copy();} void main(){Plain value;Plain other=duplicate(&value);}',
    'closeable_fresh_copy': 'struct Resource:Closeable,Copy<Resource>{void close(){} Resource copy(){Resource fresh;return fresh;}}; void main(){}',
    'closeable_inherited_copy': 'interface Duplicable:Copy<Resource>{}; struct Resource:Closeable,Duplicable{void close(){} Resource copy(){Resource fresh;return fresh;}}; void main(){}',
    'opcode_copy': '#NonCopyable opcode Unique{new=__std_bint_from_int;drop=__std_bint_drop;}; impl Unique : Copy<Unique> = externalCopy; void main(){}',
    'opcode_inherited_copy': '#NonCopyable opcode Unique{new=__std_bint_from_int;drop=__std_bint_drop;}; interface Duplicable:Copy<Unique>{}; impl Unique:Duplicable=externalCopy; void main(){}',
}


def main():
    """复制接口不会为资源绕过现有语义检查，也不按方法名字隐式满足约束。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--chali', type=Path, default=compiler_path())
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix='copy-interface-') as directory:
        for name, source in REJECTED.items():
            path = Path(directory) / (name + '.chl')
            path.write_text(source + '\n', encoding='utf-8')
            result = subprocess.run([str(args.chali.resolve()), 'check', str(path)], cwd=ROOT,
                                    env=dict(os.environ, CHALI_LANG='zh'), capture_output=True,
                                    text=True, encoding='utf-8', timeout=120)
            assert result.returncode != 0, (name, '非法复制实现被接受')
            assert 'Syntax error' not in result.stderr and '语法错误' not in result.stderr, (
                name, result.stdout, result.stderr)
            if name in ('noncopyable', 'closeable_fresh_copy', 'closeable_inherited_copy', 'opcode_copy', 'opcode_inherited_copy'):
                assert 'Copy<T> 与 #NonCopyable 不能同时使用' in result.stdout + result.stderr, (
                    name, result.stdout, result.stderr)
            print(name + ': 语义拒绝通过', flush=True)


if __name__ == '__main__':
    main()
