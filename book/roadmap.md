# Roadmap

The following roadmap is always subject to change.

## Current Milestone

### Milestone 3 - ACPI, PCIe, BID, CC, Loader

- [X] Fibers
- [x] Event loop
- [X] IRQ routing to userspace
- [x] ACPI Server
  - [x] MCFG
- [x] PCIe Server
  - [x] Device enumeration
- [ ] BID (Interface definition language)
  - [x] Parser
  - [ ] Sema
  - [ ] C code gen
    - [ ] Client Code
    - [x] Server Code
    - [ ] Serialization
- [ ] CC (C Compiler)
  - [ ] Lexer
  - [ ] Parser
  - [ ] Sema
  - [x] AST
  - [ ] Code gen (LLVM?)
  - [x] Code gen C
- [ ] Bootloader
  - [x] Load the kernel
  - [x] Load modules
  - [ ] Pass handover information

## Upcoming Milestones

### Milestone 4 - Filesystem and User Interfaces

- [ ] VFS
- [ ] ext2
- [ ] Compositor
- [ ] Shell

### Milestone 5 - DOOM!

- [ ] DOOM :rage2:

### Milestone 6 - Networking

- [ ] Hardware
  - [ ] rtl8139
  - [ ] e1000?
  - [ ] virtio-net?
- [ ] Protocols
  - [ ] Ethernet
  - [ ] Arp
  - [ ] IPv4
  - [ ] UDP
  - [ ] TCP
  - [ ] HTTP
  - [ ] ICMP
  - [ ] IRC
  - [ ] TELNET
- [ ] Tools
  - [ ] ping
  - [ ] irc
  - [ ] telnet
  - [ ] fetch
- [ ] webserver ?

## Past Milestones

### Milestone 1 - To userspace and beyond

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

### Milestone 2 - Userspace, IPC, and Syscalls

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

## Backlog

- [ ] Multiboot support
- [ ] FAT32
- [ ] RISCV support
- [ ] In house bootloader
- [ ] Virtio
  - [ ] Device Configuration
  - [ ] Virt Queues
  - [ ] network device
  - [ ] gpu device
  - [ ] input device
  - [ ] entropy device
  - [ ] console device
  - [ ] block device
