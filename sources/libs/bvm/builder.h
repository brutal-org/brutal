#pragma once

#include <bvm/mem.h>

BvmType *bvm_type_create(BvmType *super, Str name, Alloc *alloc);

void bvm_type_vfield(BvmType *self, Str name, BvmType *type, Alloc *alloc);

void bvm_type_field(BvmType *self, Str name, BvmType *type, Alloc *alloc);

BvmSig *bvm_sig_create(BvmType *ret, bool variadic, Alloc *alloc);

void bvm_sig_arg(BvmSig *self, BvmType *type);

BvmFunc *bvm_func_create(BvmSig *sig, Alloc *alloc);

int bvm_func_local(BvmFunc *self);

void bvm_func_emit(BvmFunc *self, BvmOp op);

void bvm_func_emitu(BvmFunc *self, BvmOp op, uint64_t u);

void bvm_func_emiti(BvmFunc *self, BvmOp op, int64_t i);
