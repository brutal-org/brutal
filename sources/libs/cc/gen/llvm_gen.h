#pragma once
#include <cc/ast/unit.h>

typedef void Module;
typedef void Context;

typedef struct
{
    Module *mod;
    Context *ctx;
} CGenContext;

void cgen_llvm_init();

CGenContext cgen_llvm_init_context(const char* name);
void cgen_llvm_deinit_context(CGenContext *ctx);

MaybeError cgen_llvm_unit(CGenContext *ctx, CUnit cunit);
void cgen_llvm_dump(CGenContext *ctx);
MaybeError cgen_llvm_compile(CGenContext *ctx, IoWriter *object_writer);
