# Roadmap

## Milestone 1 - Baby Steps

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

## Milestone 2 - Userspace

- [ ] Syscalls
  - [x] log
  - [x] map
  - [x] alloc
  - [x] unmap
  - [x] create
  - [x] start
  - [x] exit
  - [ ] ipc
  - [ ] irq
  - [x] drop
  - [x] close
- [x] Userspace ELF loader
- [ ] IPC
- [ ] IDL

## Milestone 3 - Filesystem

- [ ] IRQ routing to userspace
- [ ] Disk driver (virtio-block? AHCI? NVME? IDE (lol)?)
- [ ] VFS
- [ ] ext2

## Milestone 4 - User Interfaces

- [ ] Keyboard driver
- [ ] Mouse driver
- [ ] Framebuffer driver (virtio-gpu ?)
- [ ] Compositor
- [ ] Shell

## Milestone 5 - DOOM!

- [ ] DOOM :rage2:

## Milestone 6 - Networking

- [ ] Implement virtio-net or E1000 driver
- [ ] Implement TCP/IP
- [ ] Send a message on IRC

## Good to have

- [ ] Multiboot support
- [ ] FAT32
