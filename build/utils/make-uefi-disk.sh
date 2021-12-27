#!/bin/bash

echo "making uefi disk with size: $1 sectors"

EFI_DISK=./bin/loader/disk_uefi.hdd
dd if=/dev/zero of=$EFI_DISK bs=512 count=$1
mkfs.fat -F 32 $EFI_DISK

mcopy -snv -i ./bin/loader/disk_uefi.hdd ./bin/loader/image/* ::
