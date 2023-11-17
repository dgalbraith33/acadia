#! /bin/bash

set -e

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo $DIR

BUILD_DIR="${DIR}/../builddbg"

pushd $BUILD_DIR 
ninja 
ninja install

sudo sh ${DIR}/build_image.sh disk.img

QEMU_ARGS=
if [[ $1 == "debug" ]]; then
  QEMU_ARGS+="-S -s"
fi

# Use machine q35 to access PCI devices.
qemu-system-x86_64 -machine q35 -d guest_errors -m 1G -serial stdio -hda disk.img ${QEMU_ARGS}
popd

# Extra options to add to this script in the future.
# Debug opts: -S -s
# Interrupt opts: -d int

