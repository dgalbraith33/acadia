#!/us

set -e

if [[ $# -ne 1 ]]; then
  echo "Must specify disk image name."
fi

dd if=/dev/zero of=$1 bs=1M count=200

dev=$(losetup --find --partscan --show $1)
if [ -z "$dev" ]; then
  echo "Couldn't mount loopback device"
  exit 1;
fi
echo "Loopback device: ${dev}"

cleanup() {
  umount efi
  rm -rf efi
  losetup -d $dev
}
trap cleanup EXIT

parted -s $dev mklabel gpt mkpart EFI fat32 1MiB 10MiB mkpart ext2 10MiB 100% set 1 esp on
mkfs.fat -F 12 "${dev}p1"
mke2fs "${dev}p2"

limine-deploy "${dev}"

mkdir -p efi/
mount "${dev}p1" efi/

mkdir -p efi/EFI/BOOT
cp /usr/share/limine/BOOTX64.EFI efi/EFI/BOOT
cp /usr/share/limine/limine.sys efi/
cp ../zion/boot/limine.cfg efi/
cp zion/zion efi/
mkdir -p efi/sys
cp sys/test efi/sys/test
cp sys/test2 efi/sys/test2

chown drew:drew $1
