# Roadmap

The following roadmap is always subject to change.

## Current Milestone

### Milestone 3 - ACPI, PCIe, Bootloader And BID

- [x] Fibers
- [x] Event loop
- [x] IRQ routing to userspace
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
    - [x] Serialization
- [x] Mac support
- [x] Bootloader
  - [x] Load the kernel
  - [x] Load modules
  - [x] Pass handover information
  - [x] Boot the kernel
  - [x] Load modules

## Upcoming Milestones

### Milestone 4 - Filesystem and User Interfaces

- [ ] VFS
- [ ] ext2
- [ ] Compositor
- [ ] Shell

### Milestone 5 - DOOM! / porting software

- [ ] DOOM :rage2:
- [ ] full libc implementation 
  - [x] ctypes
  - [x] math
  - [x] assert
  - [x] errno
  - [ ] stdlib
  - [ ] stdio
  - [ ] string
  - [ ] fenv 
  - [ ] locale

## Past Milestones

### Milestone 1 - To Userspace And Beyond

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

### Milestone 2 - Userspace, IPC, And Syscalls

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

# Backlog

- [ ] Multiboot support
- [ ] FAT32

## Architectures

- [ ] riscv
- [ ] arm64

## Sub-Projects

### Drivers

- Virtio
  - [ ] Device Configuration
  - [ ] Virt Queues
  - [ ] Network device
  - [ ] Gpu device
  - [ ] Input device
  - [ ] Entropy device
  - [ ] Console device
  - [ ] Block device
- [ ] Rtl8139
- [ ] E1000

### Network Stack

- Protocols
  - [ ] Ethernet
  - [ ] Arp
  - [ ] IPv4
  - [ ] UDP
  - [ ] TCP
  - [ ] HTTP
  - [ ] ICMP
  - [ ] IRC
  - [ ] TELNET
- Tools
  - [ ] ping
  - [ ] irc
  - [ ] telnet
  - [ ] fetch
  - [ ] httpd

### C Compiler

- [x] Lexer
- [x] Parser
- [ ] Sema
- [x] AST
- [ ] Code gen (LLVM?)
- [x] Code gen C
