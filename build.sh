#!/bin/bash

COMMONFLAGS="-Wall -Wextra -ffreestanding"
CXXFLAGS="$COMMONFLAGS -std=c++20 -pedantic"
CFLAGS="$COMMONFLAGS -O2 -nostdlib"

set -xe

i686-elf-as -o src/boot.o src/boot.S
i686-elf-g++ $CXXFLAGS -o src/init.o -c src/init.cpp
i686-elf-gcc $CFLAGS \
             -T src/linker.ld \
             -o src/kernel \
             src/boot.o src/init.o
cp src/kernel Root/boot/kernel

set +x
if [[ $1 == "iso" ]]; then
    grub-mkrescue -o os.iso Root &> /dev/null
    if [[ $2 == "run" ]]; then
        qemu-system-i386 --enable-kvm --cdrom ./os.iso
    fi
fi
