# 在真实 CentOS Stream 9 用户空间构建，避免把 Ubuntu 二进制改名成 CentOS 包。
FROM quay.io/centos/centos:stream9@sha256:6f8045fc4f552544d8a401d61848bafc8d06ca7c11897c5b54a2c37fc419a20a
RUN dnf --enablerepo=crb install -y epel-release && dnf clean all
RUN dnf --enablerepo=crb install -y gcc-c++ make git python3.11 python-unversioned-command rpm-build libstdc++-static \
    clang lld cmake ninja-build pkgconf-pkg-config libsodium-devel libsodium-static libX11-devel libXrandr-devel libXinerama-devel libXcursor-devel libXi-devel mesa-libGL-devel mesa-dri-drivers xorg-x11-server-Xvfb alsa-lib-devel \
    && dnf clean all
WORKDIR /workspace
ENV PYTHONUTF8=1 GIT_CONFIG_COUNT=1 GIT_CONFIG_KEY_0=safe.directory GIT_CONFIG_VALUE_0=/workspace
CMD ["python3.11", "tools/build_linux_packages.py", "--available", "--formats", "rpm", "tar", "--jobs", "4"]
