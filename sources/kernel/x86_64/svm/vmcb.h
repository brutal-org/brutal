#pragma once
#include <brutal/base.h>
typedef enum
{
    SVM_INTERCEPT_INTR, // physical maskable interrupt
    SVM_INTERCEPT_NMI,
    SVM_INTERCEPT_INIT,
    SVM_INTERCEPT_VINTR,
    SVM_INTERCEPT_CR0_WRITE_SPECIFIC, // other bit change than TS or MP
    SVM_INTERCEPT_IDTR_READS,
    SVM_INTERCEPT_GDTR_READ,
    SVM_INTERCEPT_LDTR_READ,
    SVM_INTERCEPT_TR_READ,
    SVM_INTERCEPT_IDTR_WRITES,
    SVM_INTERCEPT_GDTR_WRITE,
    SVM_INTERCEPT_LDTR_WRITE,
    SVM_INTERCEPT_TR_WRITE,
    SVM_INTERCEPT_RDTSC,
    SVM_INTERCEPT_RDPMC,
    SVM_INTERCEPT_PUSHF,
    SVM_INTERCEPT_POPF,
    SVM_INTERCEPT_CPUID,
    SVM_INTERCEPT_RSM,
    SVM_INTERCEPT_IRET,
    SVM_INTERCEPT_INTN,
    SVM_INTERCEPT_INVD,
    SVM_INTERCEPT_PAUSE,
    SVM_INTERCEPT_HLT,
    SVM_INTERCEPT_INVLPG,
    SVM_INTERCEPT_INVLPGA,
    SVM_INTERCEPT_IOIO_PROT,
    SVM_INTERCEPT_MSR_PROT,
    SVM_INTERCEPT_TASK_SWITCH,
    SVM_INTERCEPT_FREEZE,
    SVM_INTERCEPT_SHUTDOWN
} SvmIntercept3Bits;

typedef enum
{
    SVM_INTERCEPT_VMRUN,
    SVM_INTERCEPT_VMMCALL,
    SVM_INTERCEPT_VMLOAD,
    SVM_INTERCEPT_VMSAVE,
    SVM_INTERCEPT_STGI,
    SVM_INTERCEPT_CLGI,
    SVM_INTERCEPT_SKINIT,
    SVM_INTERCEPT_RDTSCP,
    SVM_INTERCEPT_ICEBP,
    SVM_INTERCEPT_WBINVD,
    SVM_INTERCEPT_MONITOR,
    SVM_INTERCEPT_MWAIT,
    SVM_INTERCEPT_XSETBV,
    SVM_INTERCEPT_RDPRU,
    SVM_INTERCEPT_EFER_AFTER,
    SVM_INTERCEPT_CR0_WRITE_AFTER, // doing SVM_INTERCEPT_CR0_WRITE_AFTER + x correspond to the CRX ...
} SvmIntercept4Bits;

// offset 0

typedef struct PACKED
{
    // 0
    uint16_t intercept_cr_reads; // bit 0: cr0 ...
    uint16_t intercept_cr_writes;
    // 4
    uint16_t intercept_dr_reads; // bit 0: dr0 ...
    uint16_t intercept_dr_writes;
    // 8
    uint32_t intercept_exception; // bit 0 -> int 0 ...
    // for specific intercept, see amd documentation (volume 2 appendix B)
    // c
    uint32_t specific_intercept3;
    // 10
    uint32_t specific_intercept4;
    // 14
    uint32_t specific_intercept5; // see amd doc
    // 18
    uint32_t _reserved[9];
    // 3c
    uint16_t pause_filter_threshold;
    // 3e
    uint16_t pause_filter_count;
    // 40
    uint64_t iopm_phys_addr;
    // 48
    uint64_t msrpm_phys_addr;
    // 50
    uint64_t tsc_offset;
    // 58
    uint32_t guest_asid;
    uint8_t tlb_control;
    uint64_t reserved : 24;
    // 60
    uint8_t v_tpr;
    bool v_irq : 1;
    bool v_gif : 1;
    uint8_t _reserved2 : 6;
    uint8_t v_intpr_prio : 4;
    bool v_ign_tpr : 1;
    uint8_t _reserved3 : 3;
    bool v_intrmask : 1;
    bool amd_virtual_gif : 1;
    uint8_t _reserved4 : 5;
    bool avic_enable : 1;
    uint8_t v_intr_vector;
    uint32_t _reserved5 : 24;
    // 68
    bool is_interrupt_shadow : 1;
    bool interrupt_mask : 1; // value of IF
    uint64_t _reserved6 : 62;
    // 70
    uint64_t exit_code;
    // 78
    uint64_t exit_info;
    // 80
    uint64_t exit_info_2;
    // 88
    uint64_t exit_int_info;
    // 90
    bool np_enable : 1;
    bool enable_secure_encrypted_virtualization : 1;
    bool enable_encrypted_state : 1;
    bool guest_mode_exe_trap : 1;
    bool sss_check_en : 1; // enable supervisor shadow stack
    bool virtual_transparent_encryption : 1;
    bool _reserved7 : 1;
    bool enable_invlpgb : 1;
    uint64_t _reserved9 : 56;
    // 98
    uint64_t avic_apic_bar : 52;
    uint16_t _reserved10 : 12;
    // A0
    uint64_t ghcb_phys_addr;
    // A8
    uint64_t event_inj; // event injection
    // B0
    uint64_t nested_cr3;
    // B8
    bool lbr_virt_enable : 1;
    bool virtualize_vmsave_load : 1;
    uint64_t _reserved11 : 62;
    // C0
    uint32_t vmcb_clean;
    uint32_t _reserved22;
    // C8
    uint64_t next_rip;
    // D0
    uint8_t fetch_byte;
    uint8_t instruction_bytes[15];
    // E0
    uint64_t avic_apic_backing_page : 52;
    uint16_t _reserved12 : 12;
    // E8
    uint8_t _reserved13[8];
    // F0

    uint16_t _reserved15 : 12;
    uint64_t avic_logical_table_ptr : 40;
    uint16_t _reserved14 : 12;
    // F8
    uint8_t avic_physical_max_index;
    uint16_t _reserved17 : 3;
    uint64_t avic_physical_table_ptr : 40;
    uint16_t _reserved16 : 12;
    // 100
    uint8_t _reserved18[8];
    // 108
    uint16_t _reserved20 : 12;
    uint64_t vmsa_ptr : 40;
    uint16_t _reserved19 : 12;
    // 109
    uint8_t _reserved21[0x3e0 - 0x109];
    // 3DF
    uint8_t available[32];
} SvmVMCBControl;

typedef struct PACKED {
    uint16_t selector;
    uint16_t attrib;
    uint32_t limit;
    uint64_t base;
} VMCB_Segment;


// offset 0x400

typedef struct
{
    // 0
    VMCB_Segment es;
    VMCB_Segment cs;
    VMCB_Segment ss;
    VMCB_Segment ds;
    VMCB_Segment fs;
    VMCB_Segment gs;
    VMCB_Segment gdtr;
    VMCB_Segment ldtr;
    VMCB_Segment idtr;
    VMCB_Segment tr;
    // A0
    uint8_t _reserved1[2603];
    // cb
    uint8_t cpl;
    // cc
    uint32_t _reserved2;
    // d0
    uint64_t efer;
    // d8
    uint8_t _reserved3[112];
    // 148
    uint64_t cr4;
    uint64_t cr3;
    uint64_t cr0;
    uint64_t dr7;
    uint64_t dr6;
    uint64_t rflags;
    uint64_t rip;
    // 180
    uint8_t _reserved4[88];
    // 1D8
    uint64_t rsp;
    uint64_t s_cet;
    uint64_t ssp;
    uint64_t isst_addr;
    uint64_t rax;
    uint64_t star;
    uint64_t lstar;
    uint64_t cstar;
    uint64_t sfmask;
    uint64_t gs_base;
    uint64_t sysenter_cs;
    uint64_t sysenter_esp;
    uint64_t sysenter_eip;
    uint64_t cr2;
    // 248
    uint8_t _reserved5[32];
    uint64_t g_pat;
    uint64_t debug_ctl;
    uint64_t br_from;
    uint64_t br_to;
    uint64_t last_excp_from;

} SvmVMCBSaveState;
