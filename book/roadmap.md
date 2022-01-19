# Roadmap

The following roadmap is always subject to change.

## Current Milestone

## Milestone 4 - FileSystem, Capabilities and Graphics

- [ ] BBus
    - [ ] Capabilities
- [ ] Graphic Library
    - [x] SDL Backend
        - [x] Render
        - [x] Events
    - [ ] Brutal Backend
        - [ ] Compositor
            - [ ] Event Dispatch
            - [ ] Compositing
            - [ ] Tilling Mode
            - [ ] Floating Mode
        - [ ] Client
            - [x] Render
            - [ ] Events
    - [x] Paint
        - [x] Dots
        - [x] Lines
        - [x] Rect
        - [x] Poly
        - [x] Text
    - [x] Event
        - [x] Mouse Events
        - [x] Keyboard Events
    - [ ] Layout
        - [x] LTR/RTL Layout
        - [x] Dock Layout
        - [ ] Flex Layout
        - [ ] Grid Layout
    - [ ] Views
        - [x] Panel
        - [ ] Icon
        - [x] Button
        - [x] Text
        - [x] Frame
        - [ ] Image
        - [ ] Splitter
        - [ ] Switch
        - [ ] Slider
- [ ] Files System
    - [x] Ahci
        - [x] Read
        - [x] Write
    - [ ] Ext2Fs
        - [x] Read
        - [ ] Write

### Milestone 5 - Networking and Shell

- [ ] TTy
- [ ] Shell
- [ ] Optimise IPCs
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

### Milestone 3 - ACPI, PCIe, Bootloader And BID

- [x] Fibers
- [x] Event loop
- [x] IRQ routing to userspace
- [x] ACPI Server
  - [x] MCFG
- [x] PCIe Server
  - [x] Device enumeration
- [x] BID (Interface definition language)
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

## Ports

- [ ] DOOM :rage2:
