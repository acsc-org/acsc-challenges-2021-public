#!/bin/sh
set -e
cd $(dirname "$0")

rm -rf build
mkdir build

IMGID=$(docker build --target build_uefi_challenge -q .)
docker run -v "${PWD}/build":/build --rm -it "${IMGID}"

#docker image rm "${IMGID}"

chown -R $SUDO_UID:$SUDO_GID build/
