#pragma once

#include <idl/ast.h>
#include <json/objects.h>

Json idl_jgen_primitive(IdlPrimitive primitive_, Alloc *alloc);

Json idl_jgen_enum(IdlEnum enum_, Alloc *alloc);

Json idl_jgen_struct(IdlStruct struct_, Alloc *alloc);

Json idl_jgen_type(IdlType const type, Alloc *alloc);

Json idl_jgen_alias(IdlAlias const alias, Alloc *alloc);

Json idl_jgen_method(IdlMethod const method, Alloc *alloc);

Json idl_jgen_iface(IdlIface const iface, Alloc *alloc);

Json idl_jgen_module(IdlModule const module, Alloc *alloc);
