# cheeseDOS - My x86 DOS
# Copyright (C) 2025  Connor Thomson
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

set -e

CC=gcc
AS=as
LD=ld
SU=sudo

INCLUDES="-Isrc/kernel \
-Isrc/kernel/shell \
-Isrc/kernel/ramdisk \
-Isrc/drivers \
-Isrc/drivers/vga \
-Isrc/drivers/keyboard \
-Isrc/libraries/string \
-Isrc/calc \
-Isrc/rtc"

CFLAGS="-m32 -ffreestanding -O2 -Wall -Wextra \
-fno-stack-protector -fno-builtin-strcpy -fno-builtin-strncpy \
-march=i386 $INCLUDES"

LDFLAGS="-m elf_i386 -T src/build/link.ld"

BUILD_DIR=build
ISO_DIR="$BUILD_DIR/iso"
BOOT_DIR="$ISO_DIR/boot"
GRUB_DIR="$BOOT_DIR/grub"

KERNEL="$BUILD_DIR/kernel.elf"
ISO="cdos.iso"
GRUB_CFG=src/boot/grub.cfg

OBJS=(
  "$BUILD_DIR/boot.o"
  "$BUILD_DIR/kernel.o"
  "$BUILD_DIR/shell.o"
  "$BUILD_DIR/vga.o"
  "$BUILD_DIR/keyboard.o"
  "$BUILD_DIR/ramdisk.o"
  "$BUILD_DIR/calc.o"
  "$BUILD_DIR/string.o"
  "$BUILD_DIR/rtc.o"
)

build_object() {
  $CC $CFLAGS -c "$1" -o "$2"
}

function all {
  podman build -t cheesedos-build src/build
  podman run --rm -v "$(pwd)":/src:z -w /src cheesedos-build bash "$0" build
}

function build {
  clean
  mkdir -p "$BUILD_DIR"

  $AS --32 -o "$BUILD_DIR/boot.o" src/boot/boot.S
  build_object src/kernel/kernel.c "$BUILD_DIR/kernel.o"
  build_object src/kernel/shell/shell.c "$BUILD_DIR/shell.o"
  build_object src/drivers/vga/vga.c "$BUILD_DIR/vga.o"
  build_object src/drivers/keyboard/keyboard.c "$BUILD_DIR/keyboard.o"
  build_object src/kernel/ramdisk/ramdisk.c "$BUILD_DIR/ramdisk.o"
  build_object src/calc/calc.c "$BUILD_DIR/calc.o"
  build_object src/libraries/string/string.c "$BUILD_DIR/string.o"
  build_object src/rtc/rtc.c "$BUILD_DIR/rtc.o"

  $LD $LDFLAGS -o "$KERNEL" "${OBJS[@]}"

  mkdir -p "$GRUB_DIR"
  cp "$GRUB_CFG" "$GRUB_DIR/"
  cp "$KERNEL" "$BOOT_DIR/"

  grub-mkrescue \
    --directory=/usr/lib/grub/i386-pc \
    --install-modules="multiboot" \
    --fonts="" --themes="" --locales="" \
    -o "$ISO" \
    "$ISO_DIR"

  rm -rf "$BUILD_DIR"
}

function run {
  qemu-system-i386 -drive file="$ISO",format=raw -m 3M -cpu 486
}

function write {
  lsblk
  read -p "Enter target device (e.g. sdb): " dev
  echo "Writing to /dev/$dev ..."
  $SU dd if="$ISO" of="/dev/$dev" bs=4M status=progress && sync
}

function clean {
  rm -rf "$BUILD_DIR" "$ISO"
}

case "$1" in
  "") all ;;
  all) all ;;
  build) build ;;
  run) run ;;
  write) write ;;
  clean) clean ;;
  *) echo "Usage: $0 {all|build|run|write|clean}" ;;
esac
