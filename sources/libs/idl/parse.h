#pragma once

#include <bruwutal/parse.h>
#include <idl/ast.h>

IdlModule idl_parse_module(Scan *scan, Alloc *alloc);

IoResult idl_import_module(Str name, IdlModule *module, Alloc *alloc);
