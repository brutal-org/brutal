#pragma once

#include <bvm/ctx/global.h>
#include <bvm/ctx/local.h>

BvmVal bvm_call(BvmLocal *local, BvmGlobal *global, BvmFunc *func, int argc, Alloc *alloc);

BvmRes bvm_eval(BvmLocal *local, BvmGlobal *mem, Alloc *alloc);
