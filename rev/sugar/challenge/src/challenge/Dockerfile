FROM ubuntu:20.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /root
RUN apt-get update \
	&& apt-get install -y \
		build-essential \
		git \
		libglib2.0-dev \
		libfdt-dev \
		libpixman-1-dev \
		zlib1g-dev \
		python3 \
        python-is-python3 \
        curl \
        ninja-build \
        uuid-dev \
        iasl \
        nasm \
        gdisk \
        dosfstools \
        mtools \
	&& rm -rf /var/lib/apt/lists/*

FROM builder AS build_uefi
RUN git clone https://github.com/tianocore/edk2.git \
    && git -C edk2 checkout edk2-stable202105 \
    && git -C edk2 submodule update --init \
    && make -C edk2/BaseTools

FROM build_uefi AS build_uefi_challenge
COPY ChallengePkg edk2/ChallengePkg
COPY build_diskimg.sh .
SHELL ["/bin/bash", "-c"]
CMD export WORKSPACE=${PWD}/edk2 \
    && . ${WORKSPACE}/edksetup.sh \
    && build -p ChallengePkg/ChallengePkg.dsc -a X64 -b DEBUG -t GCC5 \
    && ./build_diskimg.sh ${WORKSPACE}/Build/ChallengePkgX64/DEBUG_GCC5/X64/Challenge.efi /build/disk.img
