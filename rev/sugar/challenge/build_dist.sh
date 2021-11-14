#!/bin/bash
set -e
cd $(dirname "$0")

if [ -z "${SUDO_UID}" ]
then
    echo "ERROR: Run this script with sudo."
    exit 1
fi

rm -f dist.tar.gz
rm -rf dist/
mkdir dist

pushd src/tools
    ./build.sh
popd
for f in qemu-system-x86_64 kvmvapic.bin vgabios-stdvga.bin OVMF.fd
do
    cp src/tools/build/${f} dist
done

pushd src/challenge
    ./build.sh
popd
cp src/challenge/build/disk.img dist

for f in run.sh
do
    cp src/misc/${f} dist
done

for f in build.sh Dockerfile
do
    cp --parents src/tools/${f} dist
done

chown -R $SUDO_UID:$SUDO_GID dist

tar zcvf ./dist.tar.gz dist/
chown $SUDO_UID:$SUDO_GID ./dist.tar.gz
