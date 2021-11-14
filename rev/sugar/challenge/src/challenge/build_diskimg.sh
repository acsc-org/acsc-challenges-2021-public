#!/bin/bash
set -e

DISKGUID="644b505a-2ad7-4b3d-a40a-a0fa8e32d35d"

CHALFILE=${1}
DESTFILE=${2}
DISKFILE=$(mktemp)
PARTFILE=$(mktemp)

dd if=/dev/zero of=${DISKFILE} bs=1M count=64
gdisk ${DISKFILE} << EOF
x
g
${DISKGUID}
m
n
1
2048
131038
ef00
w
Y
EOF

dd if=${DISKFILE} of=${PARTFILE} bs=512 skip=2048 count=$((131038 - 2048))

mkfs.vfat -F32 -s1 ${PARTFILE}
mmd -i ${PARTFILE} ::/EFI ::/EFI/BOOT
mcopy -i ${PARTFILE} ${CHALFILE} ::/EFI/BOOT/BOOTX64.EFI

dd if=${PARTFILE} of=${DISKFILE} bs=512 seek=2048 count=$((131038 - 2048)) conv=notrunc

cp ${DISKFILE} ${DESTFILE}
