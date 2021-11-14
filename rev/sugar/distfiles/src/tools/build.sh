#!/bin/sh
set -e
cd $(dirname "$0")

rm -rf build
mkdir build

IMGID_QEMU=$(docker build --target build_qemu -q .)
docker run -v "${PWD}/build":/build --rm -it "${IMGID_QEMU}"

IMGID_OVMF=$(docker build --target build_uefi_ovmf -q .)
docker run -v "${PWD}/build":/build --rm -it "${IMGID_OVMF}"

#docker image rm "${IMGID_QEMU}"
#docker image rm "${IMGID_OVMF}"

chown -R $SUDO_UID:$SUDO_GID build/
