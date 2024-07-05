#!/usr/bin/env bash
set -eu

root_dir=root
img_file=disk.img

# Create a test directory to convert to ext2.
mkdir -p "$root_dir"
echo asdf > "${root_dir}/qwer"

# Create a 32M ext2 without sudo.
# If 32M is not enough for the contents of the directory,
# it will fail.
rm -f "$img_file"
mke2fs \
  -b 1024 \
  -L "CatK" \
  -d "$root_dir" \
  -m 5 \
  -r 1 \
  -t ext2 \
  "$img_file" \
  1G \
;

# Test the ext2 by mounting it with sudo.
# sudo is only used for testing.
mountpoint=mnt
mkdir -p "$mountpoint"
sudo mount "$img_file" "$mountpoint"
sudo ls -l "$mountpoint"
sudo cmp "${mountpoint}/qwer" "${root_dir}/qwer"
sudo umount "$mountpoint"
