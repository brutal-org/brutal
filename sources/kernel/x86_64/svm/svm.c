#include <kernel/x86_64/svm/svm.h>
#include <kernel/x86_64/cpuid.h>
#include <kernel/x86_64/asm.h>
#include <kernel/pmm.h>
#include <brutal/debug.h>
#include <kernel/mmap.h>
#include <kernel/heap.h>
#include <kernel/x86_64/svm/vmcb.h>
#include "kernel/x86_64/gdt.h"

void svm_load_asm(void*);
void svm_run_asm(void* vmcb);
bool svm_is_available(void)
{
    if(!cpuid_svm())
    {
        return false;
    }
    if((asm_read_msr(MSR_VM_CR) & ((uint64_t)VM_CR_SVME_DISABLE)) == 0)
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


typedef struct
{
    void* vmcb;
    void* hsave;
    SvmVMCBSaveState* vmcb_save;
    SvmVMCBControl * vmcb_ctrl;
    Gdt gdt;
    GdtDesc desc;
} SvmCtx;

void owo(void)
{
    while(true)
    {
    }
}

void svm_enter(void)
{
    asm volatile("cli");
    svm_enable();
    SvmCtx * ctx = (SvmCtx*)UNWRAP(heap_alloc(sizeof(SvmCtx))).base;

    ctx->vmcb = (void*)(UNWRAP(heap_alloc(4096))).base;
    ctx->hsave = (void*)(UNWRAP(heap_alloc(4096)).base);

    mem_set(ctx->vmcb, 0, 0x1000);
    ctx->vmcb_save = ctx->vmcb + 0x400;
    ctx->vmcb_ctrl = ctx->vmcb;

    gdt_structure_init(&ctx->gdt);

    ctx->desc.offset = (uintptr_t)&ctx->gdt;
    ctx->desc.size = sizeof(ctx->gdt) - 1;

    svm_load_asm(ctx->vmcb_save);
    ctx->vmcb_save->rflags = asm_read_rflag();
    ctx->vmcb_save->rip = (uintptr_t)owo;
    ctx->vmcb_ctrl->guest_asid = 1;
    ctx->vmcb_ctrl->specific_intercept4 |= SVM_INTERCEPT_VMRUN;
    ctx->vmcb_save->rsp = UNWRAP(heap_alloc(4096)).base + 4096;
    asm_write_msr(MSR_SVM_HOST_SAVE_ADDR, mmap_io_to_phys((uint64_t)ctx->hsave));

    asm volatile(   "vmload %0 \n"
                    "vmrun %0 \n"
                    "vmsave %0 \n" :: "a" (mmap_io_to_phys((uintptr_t)ctx->vmcb)) : "memory");

    log$("exit info: {#x}", ctx->vmcb_ctrl->exit_code);
    while(true)
    {

    }
}
