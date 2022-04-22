#include <bal/abi.h>
#include <brutal-debug>
#include <kernel/heap.h>
#include <kernel/mmap.h>
#include <kernel/pmm.h>
#include <kernel/vmm.h>

#include "kernel/x86_64/gdt.h"
#include "kernel/x86_64/idt.h"
#include <kernel/x86_64/asm.h>
#include <kernel/x86_64/cpuid.h>
#include <kernel/x86_64/gdt.h>
#include <kernel/x86_64/svm/svm.h>
#include <kernel/x86_64/svm/vmcb.h>

extern void svm_load_asm(volatile void *);
extern void svm_start(void);
extern void svm_run(void *);
bool svm_is_available(void)
{
    if (!cpuid_svm())
    {
        return false;
    }
    if ((asm_read_msr(MSR_VM_CR) & ((uint64_t)VM_CR_SVME_DISABLE)) == 0)
    {
        return true;
    }

    // maybe we will add more checks later
    return false;
}

void svm_enable(void)
{
    asm_write_msr(MSR_VM_CR, asm_read_msr(MSR_VM_CR) & ~((uint64_t)VM_CR_SVME_DISABLE)); // disable = 0
                                                                                         //    asm_write_msr(MSR_VM_CR, asm_read_msr(MSR_VM_CR) | VM_CR_SVM_LOCK); // lock = 1
                                                                                         //    asm_write_msr(MSR_SVM_LOCK_KEY, 0);
    asm_write_msr(MSR_EFER, asm_read_msr(MSR_EFER) | EFER_ENABLE_SVME);
}

void svm_disable(void)
{

    asm_write_msr(MSR_EFER, asm_read_msr(MSR_EFER) & ~((uint64_t)EFER_ENABLE_SVME));
    //   asm_write_msr(MSR_SVM_LOCK_KEY, 0);

    asm_write_msr(MSR_VM_CR, asm_read_msr(MSR_VM_CR) | ((uint64_t)VM_CR_SVME_DISABLE)); // disable = 0
    //    asm_write_msr(MSR_VM_CR, asm_read_msr(MSR_VM_CR) | VM_CR_SVM_LOCK); // lock = 1
}

typedef struct PACKED
{
    void *phys_vmcb;
    void *svm_save_ctx;
    void *svm_host_ctx;

    volatile void *vmcb;
    volatile void *hsave;
    volatile SvmVMCBSaveState *vmcb_save;
    volatile SvmVMCBControl *vmcb_ctrl;
} SvmCtx;

void owo(void)
{
    while (true)
    {
    }
}

void svm_enter(void *bios, size_t bios_size)
{
    bios_size = 128 * 1024;
    asm volatile("sti");
    svm_enable();
    volatile SvmCtx *ctx = (SvmCtx *)UNWRAP(heap_alloc(sizeof(SvmCtx))).base;

    ctx->vmcb = (void *)(UNWRAP(heap_alloc(4096))).base;
    ctx->hsave = (void *)(UNWRAP(heap_alloc(4096)).base);
    ctx->phys_vmcb = (void *)mmap_io_to_phys((uintptr_t)ctx->vmcb);
    ctx->svm_save_ctx = (void *)(UNWRAP(heap_alloc(4096)).base + 2048);
    vmm_update_flags(vmm_kernel_space(),
                     (VmmRange){.base = (uintptr_t)ctx->hsave, .size = 0x1000}, BR_MEM_NOWCACHE | BR_MEM_WRITABLE | BR_MEM_READABLE);
    vmm_update_flags(vmm_kernel_space(),
                     (VmmRange){.base = (uintptr_t)ctx->vmcb, .size = 0x1000}, BR_MEM_NOWCACHE | BR_MEM_WRITABLE | BR_MEM_READABLE);
    vmm_space_switch(vmm_kernel_space());

    mem_set((void *)ctx->vmcb, 0, 0x1000);

    asm volatile("vmsave %0" ::"a"(mmap_io_to_phys((uintptr_t)ctx->vmcb))
                 : "memory");
    ctx->vmcb_save = ctx->vmcb + 0x400;
    ctx->vmcb_ctrl = ctx->vmcb;

    ctx->vmcb_ctrl->intercept_exception = (uint32_t)-1;
    ctx->vmcb_ctrl->specific_intercept4 |= (1 << SVM_INTERCEPT_VMRUN);
    /*
    ctx->vmcb_save->rflags = asm_read_rflag();
    ctx->vmcb_save->rip = (uintptr_t)0xfff0;
    ctx->vmcb_ctrl->guest_asid = 1;
    ctx->vmcb_ctrl->np_enable = 0;
    ctx->vmcb_ctrl->interrupt_mask = 0;
    ctx->vmcb_ctrl->tlb_control = 1;
    ctx->vmcb_save->cpl = 3;
    ctx->vmcb_save->efer = asm_read_msr(MSR_EFER);
    ctx->vmcb_save->rsp = UNWRAP(heap_alloc(4096)).base + 4096;
    ctx->vmcb_save->g_pat = asm_read_msr(MSR_PAT);
    ctx->vmcb_save->cs.attrib |= GDT_EXECUTABLE;
    ctx->vmcb_save->cs.selector |= 3;
    */
    VMCB_Segment default_segment = {
        .attrib = GDT_READWRITE | GDT_PRESENT | GDT_SEGMENT | 1,
        .base = 0,
        .selector = 0,
        .limit = 0xffff,
    };

    ctx->vmcb_save->ds = default_segment;
    ctx->vmcb_save->es = default_segment;
    ctx->vmcb_save->ss = default_segment;
    ctx->vmcb_save->fs = default_segment;
    ctx->vmcb_save->gs = default_segment;
    ctx->vmcb_save->ldtr = default_segment;
    ctx->vmcb_save->tr = default_segment;

    ctx->vmcb_save->ldtr = (VMCB_Segment){
        .attrib = GDT_READWRITE | GDT_PRESENT,
        .base = 0,
        .selector = 0,
        .limit = 0xffff,
    };

    ctx->vmcb_save->tr = (VMCB_Segment){
        .attrib = GDT_READWRITE | GDT_PRESENT | 1,
        .base = 0,
        .selector = 0,
        .limit = 0xffff,
    };

    ctx->vmcb_save->cs = (VMCB_Segment){
        .attrib = GDT_READWRITE | GDT_PRESENT | GDT_SEGMENT | 1,
        .base = 0xffff0000,
        .selector = 0xf000,
        .limit = 0xffff,
    };

    ctx->vmcb_save->idtr = (VMCB_Segment){
        .base = 0,
        .limit = 0xffff,
    };

    ctx->vmcb_save->gdtr = (VMCB_Segment){
        .base = 0,
        .limit = 0xffff,
    };

    ctx->vmcb_save->dr6 = 0xffff0ff0;
    ctx->vmcb_save->dr7 = 0x400;

    ctx->vmcb_save->rip = 0xfff0;

    ctx->vmcb_save->rflags = 0b10;
    ctx->vmcb_save->cr0 = (1 << 29) | 1 << 30 | 1 << 4;
    ctx->vmcb_save->efer = (1 << 12);
    ctx->vmcb_ctrl->guest_asid = 1;

    ctx->vmcb_ctrl->v_tpr |= 1;
    asm_write_msr(MSR_SVM_HOST_SAVE_ADDR, mmap_io_to_phys((uint64_t)ctx->hsave));

    // fixme: stop doing debug thing
    vmm_map(vmm_kernel_space(), (VmmRange){.base = 0, .size = align_up$(bios_size, 0x1000)}, (PmmRange){.base = 0, .size = align_up$(bios_size, 0x1000)}, BR_MEM_READABLE | BR_MEM_WRITABLE | BR_MEM_EXECUTABLE | BR_MEM_USER);
    vmm_space_switch(vmm_kernel_space());
    mem_cpy((void *)0, bios, bios_size);
    while (true)
    {
        log$("============= SVM START ==============");

        svm_run((void *)ctx);

        log$("exit IP: {#x}", ctx->vmcb_save->rip);

        log$("exit code: {#x}", ctx->vmcb_ctrl->exit_code);
        log$("exit info: {#x}", ctx->vmcb_ctrl->exit_info);

        log$("exit info2: {#x}", ctx->vmcb_ctrl->exit_info_2);
        log$("exit interrupt info:");
        log$(" - error code: {#x}", ctx->vmcb_ctrl->error_code);
        log$(" - cr0: {#x}", ctx->vmcb_save->cr0);
        log$(" - cr2: {#x}", ctx->vmcb_save->cr2);
        log$(" - cr3: {#x}", ctx->vmcb_save->cr3);
        log$(" - cr4: {#x}", ctx->vmcb_save->cr4);
        log$(" - cpl: {#x}", ctx->vmcb_save->cpl);
        log$(" - efer: {#x}", ctx->vmcb_save->efer);

        log$(" - rsp: {#x}", ctx->vmcb_save->rsp);

        log$(" - type: {#x}", ctx->vmcb_ctrl->type);
        log$(" - vector: {#x}", ctx->vmcb_ctrl->vector);
        while (true)
        {
        }
    }
    while (true)
    {
    }
}
