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

FROM builder AS build_qemu
RUN curl https://download.qemu.org/qemu-6.0.0.tar.xz | tar Jxvf -
CMD cd qemu-6.0.0 \
    && ./configure --target-list=x86_64-softmmu --enable-debug --disable-user --disable-capstone --disable-vnc --disable-sdl --disable-gtk --static \
    && make -j$(nproc) \
    && cp build/qemu-system-x86_64 /build/ \
    && cp build/pc-bios/kvmvapic.bin /build/ \
    && cp build/pc-bios/vgabios-stdvga.bin /build/

FROM builder AS build_uefi
RUN git clone https://github.com/tianocore/edk2.git \
    && git -C edk2 checkout edk2-stable202105 \
    && git -C edk2 submodule update --init \
    && make -C edk2/BaseTools

FROM build_uefi AS build_uefi_ovmf
CMD cd edk2 \
    && OvmfPkg/build.sh -a X64 -n $(nproc) \
    && cp Build/OvmfX64/DEBUG_GCC5/FV/OVMF.fd /build/
