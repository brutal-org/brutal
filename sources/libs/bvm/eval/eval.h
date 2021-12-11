#pragma once

#include <bvm/ctx/local.h>
#include <bvm/ctx/mem.h>

BvmVal bvm_call(BvmLocal *local, BvmMem *mem, BvmFunc *func, int argc);

BvmRes bvm_eval(BvmLocal *local, BvmMem *mem);
