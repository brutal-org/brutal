#pragma once

#include <brutal/parse.h>
#include <idl/ast.h>

IdlModule idl_parse(Scan *scan, Alloc *alloc);
