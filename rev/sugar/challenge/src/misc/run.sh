#!/bin/sh

./qemu-system-x86_64 -L . -bios OVMF.fd -drive format=raw,file=disk.img -net none -nographic
