# Roadmap

The following roadmap is always subject to change.

## Current Milestone

# Milestone 5 - Storage and FileSystems

- [ ] DOOM :rage2:
- [ ] Terminal
  - [ ] TTY
  - [ ] Shell
  - [ ] Optimise IPCs
- [ ] Files System
  - [ ] GPT
  - [ ] MBR
  - [x] Ahci
    - [x] Read
    - [x] Write
  - [ ] Fat
    - [ ] Read
    - [ ] Write
    - [ ] Create files
    - [ ] Format a partition
  - [ ] Ext2Fs
    - [x] Read
    - [x] Write
    - [ ] Create files
    - [ ] Format a partition
- [ ] Benchmarking (https://github.com/benchmark-action/github-action-benchmark/tree/master)

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

### Milestone 3 - ACPI, PCIe, Bootloader And IDL

- [x] Fibers
- [x] Event loop
- [x] IRQ routing to userspace
- [x] ACPI Server
  - [x] MCFG
- [x] PCIe Server
  - [x] Device enumeration
- [x] IDL (Interface definition language)
  - [x] Parser
  - [x] Sema
  - [x] C code gen
    - [x] Client Code
    - [x] Server Code
    - [x] Serialization
- [x] Mac support
- [x] Bootloader
  - [x] Load the kernel
  - [x] Load modules
  - [x] Pass handover information
  - [x] Boot the kernel
  - [x] Load modules

## Milestone 4 - Capabilities and Graphics

- [x] IPC/IDL
  - [x] Capabilities
  - [x] Import
  - [x] Attributes
  - [x] Modules
- [x] Graphic Library
  - [x] SDL Backend
    - [x] Render
    - [x] Events
  - [x] BRUTAL Backend
    - [x] Compositor
      - [x] Event Dispatch
      - [x] Compositing
    - [x] Client
      - [x] Render
      - [x] Events
  - [x] Paint
    - [x] Dots
    - [x] Lines
    - [x] Rect
    - [x] Poly
    - [x] Text
  - [x] Event
    - [x] Mouse Events
    - [x] Keyboard Events
  - [x] Layout
    - [x] LTR/RTL Layout
    - [x] Dock Layout
    - [x] Flex Layout
  - [x] Views
    - [x] Panel
    - [x] Icon
    - [x] Button
    - [x] Text
    - [x] Frame
    - [x] Image
    - [x] Splitter
    - [x] Switch
    - [x] Slider

# Backlog

- [ ] Multiboot support

## Architectures

- [ ] riscv
  - [x] fdt
  - [x] interrupt
  - [x] pmm
  - [x] vmm
  - [ ] irq
  - [ ] timer
  - [ ] smp
  - [ ] userspace
  - [ ] own bootloader
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

### C Compiler

- [x] Lexer
- [x] Parser
- [ ] Sema
- [x] AST
- [ ] Code gen (LLVM?)
- [x] Code gen C

## Iso C Library

- [ ] assert.h
- [ ] complex.h
- [ ] ctype.h
- [ ] errno.h
- [ ] fenv.h
- [ ] float.h
- [ ] inttypes.h
- [ ] iso646.h
- [ ] limits.h
- [ ] locale.h
- [ ] math.h
- [ ] setjmp.h
- [ ] signal.h
- [ ] stdalign.h
- [ ] stdarg.h
- [ ] stdatomic.h
- [ ] stdbool.h
- [ ] stddef.h
- [ ] stdint.h
- [ ] stdio.h
- [ ] stdlib.h
- [ ] stdnoreturn.h
- [ ] string.h
- [ ] tgmath.h
- [ ] threads.h
- [ ] time.h
- [ ] uchar.h
- [ ] wchar.h
- [ ] wctype.h

## Networking

- [ ] Networking
  - [ ] NICs
    - [ ] e1000
    - [ ] Rtl8139
  - [ ] Protocols
    - [ ] Ethernet
    - [ ] Arp
    - [ ] IPv4 (v6?)
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
    - [ ] httpd
