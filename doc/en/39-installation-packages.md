# Installation packages and architecture builds

<!-- LANGUAGE NAVIGATION -->
**English** · [简体中文](../39-installation-packages.md)
<!-- END LANGUAGE NAVIGATION -->

[`tools/build.py`](../../tools/build.py) refreshes generated frontend assets and embedded manuals before building the compiler and installation packages. It does not run test gates. Finish documentation and regenerate embedded content before compiling final distributions.

## Quick installation

Windows MSI installs ChaLi under Program Files and registers the system PATH. Open a new terminal after installation:

```powershell
msiexec /i .\chali-1.0.8-llvm-static-windows-x64.msi
```

Windows x64 defaults to statically embedded LLVM, so its compiler EXE does not need a separate LLVM toolchain installation. Both MSI variants are available:

| Installer | Purpose |
| --- | --- |
| `chali-1.0.8-llvm-static-windows-x64.msi` | Default installer with statically embedded LLVM |
| `chali-1.0.8-llvm-external-windows-x64.msi` | External LLVM toolchain variant |

Use Windows application settings or `msiexec /x package.msi` to uninstall. Static and external LLVM variants of the same architecture share an upgrade family; choose one variant.

On Debian/Ubuntu, install the package built for the appropriate distribution and architecture:

```sh
sudo apt install ./chali-1.0.8-llvm-external-ubuntu-22.04-x64.deb
```

On CentOS Stream 9:

```sh
sudo dnf install ./chali-1.0.8-llvm-external-centos-9-x64.rpm
```

A matching tar.gz is also available from the packager; extract it and place `usr/bin/chali` on your PATH. Compiler installation does not provision application databases, static web pages or application-specific C ABI libraries.

## Names and architectures

Names follow `chali-VERSION-llvm-VARIANT-SYSTEM_OR_DISTRIBUTION-ARCHITECTURE`. For example, `chali-1.0.8-llvm-external-windows-x64.exe` or `chali-1.0.8-llvm-external-ubuntu-22.04-arm64.deb`. Static LLVM uses `llvm-static`.

| Filename architecture | DEB architecture | RPM architecture |
| --- | --- | --- |
| x64 | amd64 | x86_64 |
| arm64 | arm64 | aarch64 |

Distribution identifiers come from the actual build environment's `/etc/os-release`. An Ubuntu binary is not relabeled as CentOS. PE/ELF machine headers must match the requested package architecture. Adjacent JSON records compiler and package SHA256 values.

These are build targets and naming rules, not a claim that every target has passed runtime acceptance. Cross-compiling a package does not run it on the target machine. No x86 installers are provided: the 32-bit compiler can be built but its runtime fails startup validation. The embedded AOT runtime currently provides x64 host layouts; building an ARM64 VM compiler is separate from supporting native AOT export on that CPU.

## Automatic builds

Windows needs a supported GNU C++17 toolchain, Python 3.11 or newer, Git Bash, and WiX's `wix` command for MSI generation:

```powershell
python tools/build.py --jobs 4
python tools/build.py --variant external --jobs 4
python tools/build.py --wsl never --jobs 4
```

The entry point detects WSL distributions capable of running Linux commands. Without WSL it keeps native build outputs. Windows x64 defaults to statically embedded LLVM; `--variant external` selects the external toolchain build. Release builds skip x86, and the installer generator refuses x86 packaging.

WSL/Linux uses native and installed cross toolchains. Ubuntu ARM64 targets can use `g++-aarch64-linux-gnu`. DEB requires `dpkg-deb`; RPM requires `rpmbuild`. Automatic mode skips missing cross toolchains after probing the available targets.

```sh
python3.11 tools/build_linux_packages.py --available --jobs 4
python3.11 tools/build_linux_packages.py --arch amd64 arm64 --jobs 4
```

Target source snapshots, objects and temporary files stay under the repository's `tmp/`. Outputs go to `dist/installers/`. A Linux snapshot reuses the current host-generated parser, so first run `sh gen.sh` on a host with ParseEngine. Embedded standard-library and native-service assets are rebuilt for each target.

## CentOS build environment

The [CentOS Stream 9 container](../../tools/packaging/centos-stream9.Dockerfile) pins its base image digest and enables CRB for static development dependencies:

```sh
docker build -f tools/packaging/centos-stream9.Dockerfile -t chali-centos-stream9-builder tools/packaging
docker run --rm -v "$PWD:/workspace" chali-centos-stream9-builder
```

Its default command builds available architectures and emits RPM and tar.gz outputs. RPM derives system dependencies from the binary; DEB's minimum glibc requirement comes from ELF symbol versions. Ubuntu 22.04 builds do not imply compatibility with all older distributions, and CentOS Stream 9 does not imply CentOS 7 compatibility.

See [current capabilities](17-capabilities.md) and [LLVM AOT export](38-aot.md) for supported platform behavior and compiler-side LLVM variants.
