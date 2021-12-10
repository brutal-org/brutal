#pragma once

#include <bvm/local.h>
#include <bvm/mem.h>

BvmVal bvm_call(BvmLocal *local, BvmMem *mem, BvmFunc *func, int argc);

BvmRes bvm_eval(BvmLocal *local, BvmMem *mem);
