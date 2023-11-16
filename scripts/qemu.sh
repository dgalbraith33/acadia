#! /bin/bash

set -e

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo $DIR

BUILD_DIR="${DIR}/../builddbg"

pushd $BUILD_DIR 
ninja && ninja install

sudo sh ${DIR}/build_image.sh disk.img

# Use machine q35 to access PCI devices.
qemu-system-x86_64 -machine q35 -d guest_errors -m 1G -serial stdio -hda disk.img
popd

# Extra options to add to this script in the future.
# Debug opts: -S -s
# Interrupt opts: -d int

