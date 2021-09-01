# Roadmap

## Milestone 1 - To userspace and beyond

- [x] Baby steps (x86_64)
- [x] GDT/IDT
- [x] PMM
- [x] VMM
- [x] Parse MADT
- [x] apic (ioapic/lapic)
- [x] HPET
- [x] APIC timer
- [x] SMP
- [x] Multi tasking
- [x] Scheduling
- [x] Userspace
- [x] Syscalls
- [x] SIMD (avx, sse, fpu, xsave)

## Milestone 2 - Userspace, IPC, and Syscalls

- [x] Syscalls
  - [x] log
  - [x] map
  - [x] alloc
  - [x] unmap
  - [x] create
  - [x] start
  - [x] exit
  - [x] ipc
  - [x] irq (stub)
  - [x] drop
  - [x] close
- [x] Userspace ELF loader
- [x] IPC
- [x] IDL

## Milestone 3 - ACPI, PCIe, VirtIO

- [X] Fibers
- [x] Event loop
- [X] IRQ routing to userspace
- [ ] ACPI Server
  - [ ] MCFG
- [ ] PCIe Server
  - [ ] Device enumeration
- [ ] Virtio
  - [ ] Device Configuration
  - [ ] Virt Queues
  - [ ] network device
  - [ ] gpu device
  - [ ] input device
  - [ ] entropy device
  - [ ] console device
  - [ ] block device

## Milestone 4 - Filesystem and User Interfaces

- [ ] VFS
- [ ] ext2
- [ ] Compositor
- [ ] Shell

## Milestone 5 - DOOM!

- [ ] DOOM :rage2:

## Milestone 6 - Networking

- [ ] Implement TCP/IP
- [ ] Send a message on IRC

## Good to have

- [ ] Multiboot support
- [ ] FAT32
- [ ] In house bootloader
