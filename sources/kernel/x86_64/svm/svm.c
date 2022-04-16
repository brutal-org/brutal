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
#include <kernel/x86_64/svm/svm.h>
#include <kernel/x86_64/svm/vmcb.h>

extern void svm_load_asm(volatile void *);
void svm_start();
bool svm_is_available(void)
{
    if (!cpuid_svm())
    {
        return false;
    }
    if ((asm_read_msr(MSR_VM_CR) & ((uint64_t)VM_CR_SVME_DISABLE)) == 0)
    {
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

typedef struct
{
    void *vmcb;
    void *hsave;
    SvmVMCBSaveState *vmcb_save;
    SvmVMCBControl *vmcb_ctrl;
    IdtDesc idt;
    GdtDesc gdt_desc;
} SvmCtx;

void owo(void)
{
    while (true)
    {
    }
}

void svm_enter(void)
{
    asm volatile("cli");
    svm_enable();
    SvmCtx *ctx = (SvmCtx *)UNWRAP(heap_alloc(sizeof(SvmCtx))).base;

    asm volatile("sgdt %0" ::"m"(ctx->gdt_desc));
    asm volatile("sidt %0" ::"m"(ctx->idt));

    ctx->vmcb = (void *)(UNWRAP(heap_alloc(4096))).base;
    ctx->hsave = (void *)(UNWRAP(heap_alloc(4096)).base);

    vmm_update_flags(vmm_kernel_space(),
                     (VmmRange){.base = (uintptr_t)ctx->hsave, .size = 0x1000}, BR_MEM_NOWCACHE | BR_MEM_WRITABLE | BR_MEM_READABLE);
    vmm_update_flags(vmm_kernel_space(),
                     (VmmRange){.base = (uintptr_t)ctx->vmcb, .size = 0x1000}, BR_MEM_NOWCACHE | BR_MEM_WRITABLE | BR_MEM_READABLE);
    vmm_space_switch(vmm_kernel_space());

    mem_set(ctx->vmcb, 0, 0x1000);

    asm volatile("vmsave %0" ::"a"(mmap_io_to_phys((uintptr_t)ctx->vmcb))
                 : "memory");
    ctx->vmcb_save = ctx->vmcb + 0x400;
    ctx->vmcb_ctrl = ctx->vmcb;

    svm_load_asm(ctx->vmcb_save);
    ctx->vmcb_save->gdtr.base = (uintptr_t)&ctx->gdt_desc.offset;
    ctx->vmcb_save->gdtr.limit = sizeof(ctx->gdt_desc.size);
    ctx->vmcb_save->idtr.base = (uintptr_t)&ctx->idt.offset;
    ctx->vmcb_save->idtr.limit = sizeof(ctx->idt.size);
    ctx->vmcb_ctrl->virtual_transparent_encryption = 1;
    ctx->vmcb_save->rflags = asm_read_rflag() & ~(uint64_t)(1 << 9);
    ctx->vmcb_save->rip = (uintptr_t)owo;
    ctx->vmcb_ctrl->guest_asid = 1;
    ctx->vmcb_ctrl->specific_intercept4 |= (1 << SVM_INTERCEPT_VMRUN);
    ctx->vmcb_ctrl->interrupt_mask = 0;
    ctx->vmcb_save->cpl = 0;
    ctx->vmcb_save->rflags |= 1 << 1;
    ctx->vmcb_save->rsp = UNWRAP(heap_alloc(4096)).base + 4096;
    ctx->vmcb_save->g_pat = asm_read_msr(MSR_PAT);
    ctx->vmcb_save->cs.attrib |= GDT_EXECUTABLE;
    ctx->vmcb_ctrl->v_tpr |= 1;
    asm_write_msr(MSR_SVM_HOST_SAVE_ADDR, mmap_io_to_phys((uint64_t)ctx->hsave));
    while (true)
    {
        asm volatile(" \n"
                     "vmload %0 \n"
                     "vmrun %0 \n"
                     "vmsave %0 \n"
                     " " ::"a"(mmap_io_to_phys((uintptr_t)ctx->vmcb))
                     : "memory");

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

        log$(" - type: {#x}", ctx->vmcb_ctrl->type);
        log$(" - vector: {#x}", ctx->vmcb_ctrl->vector);
    }
    while (true)
    {
    }
}
