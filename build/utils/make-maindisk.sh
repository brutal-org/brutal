#!/bin/bash

DISK=./bin/loader/disk.hdd

dd if=/dev/zero of=$DISK bs=1024 count=184000

fdisk $DISK << EOF
g
n

2048
128000
t
uefi
n

129024
367966
w
EOF

START_UEFI="`sfdisk -J $DISK | jq -r '.partitiontable.partitions[0].start'`"
LEN_UEFI="`sfdisk -J $DISK | jq -r '.partitiontable.partitions[0].size'`"
START_SDISK="`sfdisk -J $DISK | jq -r '.partitiontable.partitions[1].start'`"
LEN_SDISK="`sfdisk -J $DISK | jq -r '.partitiontable.partitions[1].size'`"

echo "disk[0] $START_UEFI - $LEN_UEFI disk[1] $START_SDISK - $LEN_SDISK"

build/utils/make-system-disk.sh $LEN_SDISK
build/utils/make-uefi-disk.sh $LEN_UEFI

dd seek=$START_UEFI if=./bin/loader/disk_uefi.hdd of=$DISK bs=512
dd seek=$START_SDISK if=./bin/loader/disk_system.hdd of=$DISK bs=512

rm ./bin/loader/disk_uefi.hdd
rm ./bin/loader/disk_system.hdd
