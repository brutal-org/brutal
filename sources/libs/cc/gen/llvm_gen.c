#include <cc/gen/llvm_gen.h>
#include <stdio.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/Core.h>
#include <llvm-c/TargetMachine.h>

static int codegen_initialized = 0;

void cgen_llvm_init()
{
    if (codegen_initialized)
        return;

    LLVMInitializeAllTargetInfos();
    LLVMInitializeAllTargets();
    LLVMInitializeAllTargetMCs();
    LLVMInitializeAllAsmParsers();
    LLVMInitializeAllAsmPrinters();
    codegen_initialized = 1;
}

MaybeError cgen_llvm_compile(Module *module, IoWriter *object_writer)
{
    MaybeError result = SUCCESS;
    char *triple = LLVMGetDefaultTargetTriple();

    char *errorMsg = NULL;
    LLVMTargetRef target;
    if (LLVMGetTargetFromTriple(triple, &target, &errorMsg))
    {
        // TODO: Handle error
        fprintf(stderr, "%s\n", errorMsg);
        result = ERR(MaybeError, ERR_UNDEFINED);
        goto cleanup;
    }
    char *cpu_name = LLVMGetHostCPUName();
    char *cpu_features = LLVMGetHostCPUFeatures();
    LLVMTargetMachineRef machine = LLVMCreateTargetMachine(target, triple,
                                                           "", "",
                                                           LLVMCodeGenLevelNone, LLVMRelocDefault, LLVMCodeModelDefault);

    LLVMDisposeMessage(cpu_features);
    LLVMDisposeMessage(cpu_name);

    LLVMMemoryBufferRef buffer;
    if (LLVMTargetMachineEmitToMemoryBuffer(machine, module, LLVMObjectFile, &errorMsg, &buffer))
    {
        // TODO: Handle error
        fprintf(stderr, "%s\n", errorMsg);
        result = ERR(MaybeError, ERR_UNDEFINED);
        goto cleanup;
    }
    LLVMDisposeTargetMachine(machine);

    io_write(object_writer, (const uint8_t *)LLVMGetBufferStart(buffer), LLVMGetBufferSize(buffer));
    LLVMDisposeMemoryBuffer(buffer);
cleanup:
    // clean memory
    LLVMDisposeMessage(triple);
    LLVMDisposeMessage(errorMsg);
    return result;
}

void cgen_llvm_dump(Module *mod)
{
    LLVMDumpModule(mod);
}

CGenResult cgen_llvm_unit(CUnit unit)
{
    cgen_llvm_init();
    // create context, module and builder
    LLVMContextRef context = LLVMContextCreate();
    LLVMModuleRef module = LLVMModuleCreateWithNameInContext("file_module", context);
    LLVMBuilderRef builder = LLVMCreateBuilderInContext(context);
    CGenResult result = OK(CGenResult, module);

    vec_foreach(entry, &unit.units)
    {
        if (entry.type == CUNIT_DECLARATION)
        {
        }
    }

    // Dummy function
    LLVMTypeRef param_types[] = {LLVMInt32TypeInContext(context), LLVMInt32TypeInContext(context)};
    LLVMTypeRef ret_type = LLVMFunctionType(LLVMInt32TypeInContext(context), param_types, 2, 0);
    LLVMValueRef sum = LLVMAddFunction(module, "sum", ret_type);

    LLVMBasicBlockRef entry = LLVMAppendBasicBlockInContext(context, sum, "entry");
    LLVMPositionBuilderAtEnd(builder, entry);
    LLVMValueRef tmp = LLVMBuildAdd(builder, LLVMGetParam(sum, 0), LLVMGetParam(sum, 1), "tmp");
    LLVMBuildRet(builder, tmp);

    char *errorMsg = NULL;
    if (LLVMVerifyModule(module, LLVMReturnStatusAction, &errorMsg))
    {
        fprintf(stderr, "%s\n", errorMsg);
        result = ERR(CGenResult, ERR_UNDEFINED);
        goto cleanup;
    }

cleanup:
    // clean memory
    LLVMDisposeMessage(errorMsg);
    LLVMDisposeBuilder(builder);
    if (!result.succ)
    {
        LLVMDisposeModule(module);
        return result;
    }
    LLVMContextDispose(context);
    return result;
}
