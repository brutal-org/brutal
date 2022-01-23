#pragma once

#include <brutal/parse.h>
#include <idl/ast.h>

IdlModule idl_parse_module(Scan *scan, Alloc *alloc);
