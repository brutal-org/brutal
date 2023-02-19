#pragma once

#include <idl/ast.h>

/* --- Types ---------------------------------------------------------------- */

IdlType idl_nil(void);

IdlType idl_ctype(Str ctype);

IdlType idl_primitive(Str str);

IdlType idl_primitive_resolved(Str str, IdlAlias alias, Alloc *alloc);

IdlType idl_enum(Alloc *alloc);

void idl_enum_constant(IdlType *enum_, Str name, int value);

IdlType idl_struct(Alloc *alloc);

void idl_struct_member(IdlType *struct_, Str name, IdlType type);

IdlType idl_vec(IdlType subtype, Alloc *alloc);

/* --- Attributes ----------------------------------------------------------- */

IdlAttrs idl_attrs(Alloc *alloc);

void idl_attrs_append(IdlAttrs *attrs, IdlAttr attr);

IdlAttr idl_attr(Str name, Alloc *alloc);

void idl_attr_append(IdlAttr *attr, Str data);

/* --- Methods -------------------------------------------------------------- */

IdlMethod idl_method(Str name, IdlAttrs attrs, IdlType request, IdlType response);

/* --- Aliases -------------------------------------------------------------- */

IdlAlias idl_alias(Str name, IdlAttrs attrs, IdlType type);

/* --- Interfaces ----------------------------------------------------------- */

IdlIface idl_iface(Str name, IdlAttrs attrs, Alloc *alloc);

void idl_iface_methode(IdlIface *iface, IdlMethod method);

/* --- Modules -------------------------------------------------------------- */

IdlModule idl_module(Str name, Alloc *alloc);

void idl_module_import(IdlModule *module, Str name);

void idl_module_include(IdlModule *module, Str name);

void idl_module_alias(IdlModule *module, IdlAlias alias);

void idl_module_iface(IdlModule *module, IdlIface iface);
