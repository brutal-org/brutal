#include <cc/gen/llvm_gen.h>
#include <stdio.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/Core.h>
#include <llvm-c/TargetMachine.h>

void codegen_emit_object_file(LLVMModuleRef module, IoWriter * /*object_writer*/)
{
    LLVMInitializeNativeTarget();
    char *triple = LLVMGetDefaultTargetTriple();

    char *errorMsg = NULL;
    LLVMTargetRef target;
    if (LLVMGetTargetFromTriple(triple, &target, &errorMsg))
    {
        // TODO: Handle error
        fprintf(stderr, "%s\n", errorMsg);
        goto cleanup;
    }
    char *cpu_name = LLVMGetHostCPUName();
    char *cpu_features = LLVMGetHostCPUFeatures();
    LLVMTargetMachineRef machine = LLVMCreateTargetMachine(target, triple,
                                                           cpu_name, cpu_features,
                                                           LLVMCodeGenLevelNone, LLVMRelocDefault, LLVMCodeModelDefault);

    LLVMMemoryBufferRef buffer;
    LLVMTargetMachineEmitToMemoryBuffer(machine, module, LLVMObjectFile, &errorMsg, &buffer);
    LLVMDisposeMessage(cpu_features);
    LLVMDisposeMessage(cpu_name);

cleanup:
    // clean memory
    LLVMDisposeMessage(triple);
    LLVMDisposeMessage(errorMsg);
}

void codegen_from_unit(CUnit unit, IoWriter *object_writer)
{
    // create context, module and builder
    LLVMContextRef context = LLVMContextCreate();
    LLVMModuleRef module = LLVMModuleCreateWithNameInContext("file_module", context);
    LLVMBuilderRef builder = LLVMCreateBuilderInContext(context);

    vec_foreach(entry, &unit.units)
    {
        if (entry.type == CUNIT_DECLARATION)
        {
        }
    }
    char *errorMsg = NULL;
    if (LLVMVerifyModule(module, LLVMReturnStatusAction, &errorMsg))
    {
        fprintf(stderr, "%s\n", errorMsg);
        goto cleanup;
    }

    codegen_emit_object_file(module, object_writer);

cleanup:
    // clean memory
    LLVMDisposeMessage(errorMsg);
    LLVMDisposeBuilder(builder);
    LLVMDisposeModule(module);
    LLVMContextDispose(context);
}
