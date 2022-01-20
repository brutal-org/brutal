#pragma once

#include <idl/ast/iface.h>
#include <idl/ast/types.h>

bool idl_resolve_alias(IdlModule module, Str name, IdlAlias *alias);

bool idl_contain_alias(IdlModule module, Str name);

bool idl_attrs_has(IdlAttrs attrs, Str name);

bool idl_attrs_get(IdlAttrs attrs, Str name, IdlAttr *attr);
