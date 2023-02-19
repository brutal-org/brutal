#pragma once

#include <brutal-base>

enum csr_regs
{
    CSR_SSTATUS = 0x100, // supervisor status
    CSR_SISA = 0x101,    // supervisor extension
    CSR_SIE = 0x104,     // supervisor interrupt enable
    CSR_STVEC = 0x105,   // supervisor trap vector
    CSR_SEPC = 0x141,    // supervisor trap program counter
    CSR_SCAUSE = 0x142,  // supervisor trap cause
    CSR_STVAL = 0x143,   // supervisor trap value
    CSR_SIP = 0x144,     // supervisor interrupt pending
    CSR_SATP = 0x180,    // supervisor address translation and protection
    CSR_MSTATUS = 0x300, // machine status
    CSR_MISA = 0x301,    // machine extension
    CSR_MIE = 0x304,     // machine interrupt enable
    CSR_MTVEC = 0x305,   // machine trap vector
    CSR_MCAUSE = 0x342,  // machine trap cause
    CSR_MIP = 0x344,     // machine interrupt pending
    // see volume III chapter 2.1 CSR Address mapping conventions for full csr regs
};

#define SCAUSE_INTERRUPT_MASK (1 << 31)
#define SCAUSE_CODE_MASK (0x7fffffff)

#define SSTATUS_INTERRUPT_ENABLE (1 << 3)

static inline void write_csr(uint32_t csr, uint64_t value)
{
    asm volatile("csrw %0, %1"
                 :
                 : "i"(csr), "r"(value));
}

static inline uint64_t read_csr(uint32_t csr)
{
    uint64_t v;
    asm volatile("csrr %0, %1"
                 : "=r"(v)
                 : "i"(csr));
    return v;
}
