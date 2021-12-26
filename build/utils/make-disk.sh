#!/bin/bash

DISK="./bin/loader/disk.hdd"
MNT_PATH="./bin/mnt"
dd if=/dev/zero of=$DISK bs=1024 count=262144

parted $DISK << EOF
mktable gpt
Yes
mkpart 1 fat32 0 64
I
mkpart 2 ext2 64 200
set 1 boot on
quit
EOF

LOOP_DISK="`losetup -f`"
echo "using loop disk $LOOP_DISK"
losetup -P $LOOP_DISK $DISK

LOOP_DISK_PARTIITION="`losetup -f`"
sudo losetup -P $LOOP_DISK_PARTIITION $DISK
mkfs.ext2 $LOOP_DISK_PARTIITION\p2
mkfs.fat -F 32 $LOOP_DISK_PARTIITION\p1
mkdir -p $MNT_PATH
mount $LOOP_DISK_PARTIITION\p1 $MNT_PATH

cp -r ./bin/loader/image/* $MNT_PATH/

sync
umount $MNT_PATH
losetup -d $LOOP_DISK_PARTIITION
losetup -d $LOOP_DISK
rm -rf $MNT_PATH
