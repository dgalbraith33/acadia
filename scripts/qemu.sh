#! /bin/bash

set -e

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo $DIR

BUILD_DIR="${DIR}/../builddbg"

pushd $BUILD_DIR 
ninja 
ninja install

CARGO_USR_ROOT="${DIR}/../sysroot/usr/"
CARGO_SYS_ROOT="${DIR}/../sysroot/"

# Need to pushd so rustup gets the toolchain from rust/rust_toolchain.toml
pushd "${DIR}/../rust"

for BIN in ${DIR}/../rust/usr/*/; do
  cargo install --force --path "${BIN}" --root $CARGO_USR_ROOT
done

for BIN in ${DIR}/../rust/sys/*/; do
  cargo install --force --path "${BIN}" --root $CARGO_SYS_ROOT
done
popd

sudo sh ${DIR}/build_image.sh disk.img

QEMU_ARGS=
if [[ $1 == "debug" ]]; then
  QEMU_ARGS+="-S -s"
fi

# Use machine q35 to access PCI devices.
qemu-system-x86_64 -machine q35 -d guest_errors -m 1G -serial stdio -hda disk.img ${QEMU_ARGS} -device nec-usb-xhci,id=xhci -device usb-kbd,bus=xhci.0
popd

# Extra options to add to this script in the future.
# Debug opts: -S -s
# Interrupt opts: -d int

