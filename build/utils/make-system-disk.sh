#!/bin/bash

EXT_DISK=./bin/loader/disk_system.hdd

# maybe change sysroot to something else later
echo "making ext disk with size: $1 sectors"
dd if=/dev/zero of=$EXT_DISK bs=512 count=$1
mke2fs $EXT_DISK -d ./sysroot
