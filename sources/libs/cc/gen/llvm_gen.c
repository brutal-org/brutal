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

MaybeError cgen_llvm_compile(CGenContext *ctx, IoWriter *object_writer)
{
    MaybeError result = SUCCESS;
    char *triple = LLVMGetDefaultTargetTriple();

    char *errorMsg = NULL;
    LLVMTargetRef target;
    if (LLVMGetTargetFromTriple(triple, &target, &errorMsg))
    {
        // TODO: Handle error
        fprintf(stderr, "%s\n", errorMsg);
        result = ERROR(ERR_UNDEFINED);
    }
    else
    {
        char *cpu_name = LLVMGetHostCPUName();
        char *cpu_features = LLVMGetHostCPUFeatures();
        LLVMTargetMachineRef machine = LLVMCreateTargetMachine(target, triple,
                                                               cpu_name, cpu_features,
                                                               LLVMCodeGenLevelNone, LLVMRelocDefault, LLVMCodeModelDefault);

        LLVMDisposeMessage(cpu_features);
        LLVMDisposeMessage(cpu_name);

        LLVMMemoryBufferRef buffer;
        if (LLVMTargetMachineEmitToMemoryBuffer(machine, ctx->mod, LLVMObjectFile, &errorMsg, &buffer))
        {
            // TODO: Handle error
            fprintf(stderr, "%s\n", errorMsg);
            result = ERR(MaybeError, ERR_UNDEFINED);
        }
        else
        {
            io_write(object_writer, (const uint8_t *)LLVMGetBufferStart(buffer), LLVMGetBufferSize(buffer));
        }
        LLVMDisposeTargetMachine(machine);
        LLVMDisposeMemoryBuffer(buffer);
    }
    // clean memory
    LLVMDisposeMessage(triple);
    LLVMDisposeMessage(errorMsg);
    return result;
}

void cgen_llvm_dump(CGenContext *ctx)
{
    LLVMDumpModule(ctx->mod);
}

CGenContext cgen_llvm_init_context(const char *name)
{
    CGenContext result;
    result.ctx = LLVMContextCreate();
    result.mod = LLVMModuleCreateWithNameInContext(name, result.ctx);
    return result;
}

void cgen_llvm_deinit_context(CGenContext *ctx)
{
    LLVMDisposeModule(ctx->mod);
    LLVMContextDispose(ctx->ctx);
}

MaybeError cgen_llvm_unit(CGenContext *ctx, CUnit unit)
{
    cgen_llvm_init();

    MaybeError result = SUCCESS;

    LLVMBuilderRef builder = LLVMCreateBuilderInContext(ctx->ctx);

    vec_foreach(entry, &unit.units)
    {
        if (entry.type == CUNIT_DECLARATION)
        {
        }
    }

    // Dummy function
    LLVMTypeRef param_types[] = {LLVMInt32TypeInContext(ctx->ctx), LLVMInt32TypeInContext(ctx->ctx)};
    LLVMTypeRef ret_type = LLVMFunctionType(LLVMInt32TypeInContext(ctx->ctx), param_types, 2, 0);
    LLVMValueRef sum = LLVMAddFunction(ctx->mod, "sum", ret_type);

    LLVMBasicBlockRef entry = LLVMAppendBasicBlockInContext(ctx->ctx, sum, "entry");
    LLVMPositionBuilderAtEnd(builder, entry);
    LLVMValueRef tmp = LLVMBuildAdd(builder, LLVMGetParam(sum, 0), LLVMGetParam(sum, 1), "tmp");
    LLVMBuildRet(builder, tmp);

    char *errorMsg = NULL;
    if (LLVMVerifyModule(ctx->mod, LLVMReturnStatusAction, &errorMsg))
    {
        fprintf(stderr, "%s\n", errorMsg);
        result = ERROR(ERR_UNDEFINED);
    }

    // clean memory
    LLVMDisposeMessage(errorMsg);
    LLVMDisposeBuilder(builder);

    return result;
}
