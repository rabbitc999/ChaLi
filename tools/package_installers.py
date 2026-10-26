"""核对二进制架构后生成 MSI、DEB、RPM 或 tar.gz；不运行编译器门禁。"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import struct
import subprocess
import tarfile
import uuid
import xml.etree.ElementTree as ET
from project_temp import tempfile

ROOT = Path(__file__).resolve().parents[1]
ARCH = {'amd64': ('amd64', 'x86_64', 'x64', 62, 0x8664),
        'x86': ('i386', 'i686', 'x86', 3, 0x14c),
        'arm64': ('arm64', 'aarch64', 'arm64', 183, 0xaa64)}


def run(command):
    """执行打包工具并在失败时保留原始诊断。"""
    print('+ ' + ' '.join(map(str, command)), flush=True)
    subprocess.run(list(map(str, command)), check=True)


def archive_permissions(info):
    """为安装归档固定 root 所有权及安全模式，不依赖 WSL 挂载盘的权限表示。"""
    info.uid = info.gid = 0
    info.uname = info.gname = 'root'
    info.mode = 0o755 if info.isdir() or info.name.endswith('/bin/chali') else 0o644
    return info


def validate(binary, system, architecture):
    """读取 ELF 或 PE 头，拒绝与包架构不符的输入文件。"""
    with binary.open('rb') as stream:
        header = stream.read(64)
        if system == 'linux':
            if header[:4] != b'\x7fELF' or header[5] != 1:
                raise ValueError('输入不是受支持的小端 ELF 文件')
            machine = struct.unpack_from('<H', header, 18)[0]
            expected = ARCH[architecture][3]
            bits = 32 if architecture == 'x86' else 64
            if header[4] != (1 if bits == 32 else 2):
                raise ValueError('ELF 位宽与目标不符')
        else:
            if header[:2] != b'MZ':
                raise ValueError('输入不是 Windows PE 文件')
            stream.seek(struct.unpack_from('<I', header, 60)[0])
            pe = stream.read(6)
            if pe[:4] != b'PE\0\0':
                raise ValueError('PE 头无效')
            machine = struct.unpack_from('<H', pe, 4)[0]
            expected = ARCH[architecture][4]
        if machine != expected:
            raise ValueError(f'二进制机器类型 {machine:#x} 与 {architecture} 不符')


def msi(binary, version, arch, variant, output, work):
    """创建支持升级、卸载及系统 PATH 的 WiX MSI 包。"""
    namespace = 'http://wixtoolset.org/schemas/v4/wxs'
    ET.register_namespace('', namespace)
    root = ET.Element('Wix', xmlns=namespace)
    family = str(uuid.uuid5(uuid.NAMESPACE_URL, 'https://gitee.com/rabbitc999/chali/installer/' + arch)).upper()
    package = ET.SubElement(root, 'Package', Name='ChaLi', Manufacturer='小兔糖糖_OvO', Version=version,
                            UpgradeCode=family, Scope='perMachine', Language='2052', Codepage='936')
    ET.SubElement(package, 'MajorUpgrade', AllowSameVersionUpgrades='yes', DowngradeErrorMessage='A newer ChaLi version is already installed.')
    ET.SubElement(package, 'MediaTemplate', EmbedCab='yes')
    directory = ET.SubElement(package, 'StandardDirectory', Id='ProgramFilesFolder' if arch == 'x86' else 'ProgramFiles64Folder')
    install = ET.SubElement(directory, 'Directory', Id='INSTALLFOLDER', Name='ChaLi')
    component = ET.SubElement(install, 'Component', Id='Compiler', Guid='*', Bitness='always32' if arch == 'x86' else 'always64')
    ET.SubElement(component, 'File', Id='CompilerExe', Source=str(binary), Name='chali.exe', KeyPath='yes')
    ET.SubElement(component, 'Environment', Id='CompilerPath', Name='PATH', Value='[INSTALLFOLDER]',
                  Permanent='no', Part='last', Action='set', System='yes')
    feature = ET.SubElement(package, 'Feature', Id='Main', Title='ChaLi compiler', Level='1')
    ET.SubElement(feature, 'ComponentRef', Id='Compiler')
    license_files = [(ROOT / 'LICENSE', 'LICENSE'), (ROOT / 'NOTICE', 'NOTICE')]
    license_files += [(ROOT / ('licenses' if name == 'miniz' else 'library') / name / 'LICENSE', name + '-LICENSE.txt')
                      for name in ('miniz', 'ipage', 'caspp')]
    if variant == 'static':
        license_files += [(ROOT / 'library/llvm/LICENSE.TXT', 'LLVM-LICENSE.txt')]
        license_files += [(path, 'LLVM-' + path.name) for path in sorted((ROOT / 'library/llvm/licenses').glob('*')) if path.is_file()]
    for index, (path, name) in enumerate(license_files):
        if not path.is_file():
            raise ValueError('缺少发布许可文件：' + str(path))
        identifier = 'License' + str(index)
        item = ET.SubElement(install, 'Component', Id=identifier, Guid='*', Bitness='always32' if arch == 'x86' else 'always64')
        ET.SubElement(item, 'File', Id=identifier + 'File', Source=str(path), Name=name, KeyPath='yes')
        ET.SubElement(feature, 'ComponentRef', Id=identifier)
    source = work / 'chali.wxs'
    ET.ElementTree(root).write(source, encoding='utf-8', xml_declaration=True)
    run(['wix', 'build', '-arch', ARCH[arch][2], '-intermediateFolder', work / 'wix', '-o', output, source])


def main():
    """生成带真实架构与内容 SHA256 的快速安装产物。"""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--binary', type=Path, required=True)
    parser.add_argument('--platform', choices=['windows', 'linux'], required=True)
    parser.add_argument('--arch', choices=ARCH, required=True)
    parser.add_argument('--variant', choices=['external', 'static'], default='external')
    parser.add_argument('--distribution', help='Linux 构建发行版标识；默认读取 /etc/os-release')
    parser.add_argument('--build-metadata', type=Path, help='交叉构建的版本、架构与二进制 SHA256 收据')
    parser.add_argument('--formats', nargs='+', choices=['msi', 'deb', 'rpm', 'tar'], required=True)
    parser.add_argument('--out', type=Path, default=ROOT / 'dist/installers')
    args = parser.parse_args()
    if args.arch == 'x86':
        parser.error('当前不提供 x86 安装包：32 位运行时未通过启动验证')
    binary = args.binary.resolve()
    validate(binary, args.platform, args.arch)
    if (args.platform == 'windows' and set(args.formats) != {'msi'}) or (args.platform == 'linux' and 'msi' in args.formats):
        parser.error('MSI 只接收 Windows PE；DEB/RPM/tar 只接收 Linux ELF')
    version = (ROOT / 'VERSION').read_text().strip()
    binary_digest = hashlib.sha256(binary.read_bytes()).hexdigest()
    if args.build_metadata:
        receipt = json.loads(args.build_metadata.read_text(encoding='utf-8'))
        if receipt.get('version') != version or receipt.get('architecture') != args.arch or receipt.get('binarySha256') != binary_digest:
            raise ValueError('交叉构建收据的版本、架构或二进制 SHA256 不符')
    else:
        from release import compiler_version
        if compiler_version(binary) != version:
            raise ValueError('输入编译器的实际版本与 VERSION 不符，不能仅改文件名后打包')
    output = args.out.resolve()
    output.mkdir(parents=True, exist_ok=True)
    filename_arch = 'x64' if args.arch == 'amd64' else args.arch
    stem = f'chali-{version}-llvm-{args.variant}'
    distribution = args.distribution
    if args.platform == 'linux':
        if distribution is None:
            release = dict(line.split('=', 1) for line in Path('/etc/os-release').read_text().splitlines() if '=' in line)
            distribution = release['ID'].strip('"') + '-' + release.get('VERSION_ID', 'rolling').strip('"')
        if not re.fullmatch(r'[a-zA-Z0-9][a-zA-Z0-9._-]*', distribution):
            parser.error('发行版标识只能包含字母、数字、点、下划线和连字符')
    stem += '-' + (distribution if args.platform == 'linux' else 'windows') + '-' + filename_arch
    results = []
    with tempfile.TemporaryDirectory(prefix='installers-') as directory:
        work = Path(directory)
        payload = work / 'payload'
        executable = payload / 'usr/bin/chali'
        executable.parent.mkdir(parents=True)
        shutil.copy2(binary, executable)
        executable.chmod(0o755)
        docs = payload / 'usr/share/doc/chali'
        docs.mkdir(parents=True)
        for name in ('LICENSE', 'NOTICE', 'README.md', 'README.cn.md', 'README.en.md'):
            if (ROOT / name).is_file():
                shutil.copy2(ROOT / name, docs / name)
        notices = docs / 'licenses'
        notices.mkdir()
        for name in ('miniz', 'ipage', 'caspp'):
            shutil.copy2(ROOT / ('licenses' if name == 'miniz' else 'library') / name / 'LICENSE', notices / (name + '-LICENSE.txt'))
        for kind in dict.fromkeys(args.formats):
            destination = output / (stem + ('.tar.gz' if kind == 'tar' else '.' + kind))
            if kind == 'msi':
                msi(binary, version, args.arch, args.variant, destination, work)
            elif kind == 'tar':
                with tarfile.open(destination, 'w:gz') as archive:
                    archive.add(payload / 'usr', arcname='usr', filter=archive_permissions)
            elif kind == 'deb':
                control = payload / 'DEBIAN'
                control.mkdir()
                symbols = subprocess.check_output(['readelf', '--version-info', binary], text=True)
                glibc = max((tuple(map(int, value.split('.'))) for value in re.findall(r'GLIBC_(\d+\.\d+(?:\.\d+)?)', symbols)), default=(2, 17))
                dependencies = 'libc6 (>= ' + '.'.join(map(str, glibc)) + ')'
                needed = subprocess.check_output(['readelf', '-d', binary], text=True)
                if 'libstdc++.so' in needed:
                    dependencies += ', libstdc++6'
                if 'libgcc_s.so' in needed:
                    dependencies += ', libgcc-s1'
                (control / 'control').write_text(f'Package: chali\nVersion: {version}\nArchitecture: {ARCH[args.arch][0]}\nMaintainer: 小兔糖糖_OvO <rabbitc999@outlook.com>\nSection: devel\nPriority: optional\nDepends: {dependencies}\nDescription: ChaLi compiler and shared VM with embedded standard library\n', encoding='utf-8')
                # WSL 的 Windows 挂载盘常把暂存目录显示为 0777，直接构建会拒绝控制目录。
                # 按 deb(5) 组装标准三成员归档，显式固定 tar 内权限，不降低 dpkg 的检查。
                marker = work / 'debian-binary'
                marker.write_bytes(b'2.0\n')
                control_archive = work / 'control.tar.gz'
                with tarfile.open(control_archive, 'w:gz', format=tarfile.USTAR_FORMAT) as archive:
                    archive.add(control / 'control', arcname='./control', filter=archive_permissions)
                data_archive = work / 'data.tar.xz'
                with tarfile.open(data_archive, 'w:xz', format=tarfile.USTAR_FORMAT) as archive:
                    archive.add(payload / 'usr', arcname='./usr', filter=archive_permissions)
                package = work / 'package.deb'
                run(['ar', 'qcD', package, marker, control_archive, data_archive])
                run(['dpkg-deb', '--info', package])
                shutil.copy2(package, destination)
                shutil.rmtree(control)
            else:
                top = work / 'rpm'
                for name in ('BUILD', 'BUILDROOT', 'RPMS', 'SOURCES', 'SPECS', 'SRPMS', 'tmp'):
                    (top / name).mkdir(parents=True)
                spec = top / 'SPECS/chali.spec'
                spec.write_text(f'''Name: chali
Version: {version}
Release: 1
Summary: ChaLi compiler and shared VM
License: Apache-2.0
Packager: 小兔糖糖_OvO <rabbitc999@outlook.com>
%description
ChaLi compiler with embedded standard library.
%prep
%build
%install
mkdir -p "%{{buildroot}}/usr"
cp -a "{payload}/usr/." "%{{buildroot}}/usr/"
%files
%defattr(0644,root,root,0755)
%attr(0755,root,root) /usr/bin/chali
%dir /usr/share/doc/chali
/usr/share/doc/chali/*
''', encoding='utf-8')
                run(['rpmbuild', '-bb', '--target', ARCH[args.arch][1], '--define', '_topdir ' + str(top),
                     '--define', '_tmppath ' + str(top / 'tmp'), '--define', '__os_install_post %{nil}', spec])
                packages = list((top / 'RPMS').rglob('*.rpm'))
                if len(packages) != 1:
                    raise ValueError('RPM 产物数量异常')
                shutil.copy2(packages[0], destination)
            results.append({'file': destination.name, 'sha256': hashlib.sha256(destination.read_bytes()).hexdigest(), 'bytes': destination.stat().st_size})
    (output / (stem + '.json')).write_text(json.dumps({'version': version, 'platform': args.platform,
        'author': '小兔糖糖_OvO', 'email': 'rabbitc999@outlook.com', 'license': 'Apache-2.0',
        'architecture': args.arch, 'distribution': distribution, 'llvm': args.variant, 'binarySha256': binary_digest,
        'packages': results, 'defaultInstaller': args.platform == 'windows' and args.variant == 'static',
        'gatesRun': False}, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    print('安装包生成完成：' + str(output))


if __name__ == '__main__':
    main()
