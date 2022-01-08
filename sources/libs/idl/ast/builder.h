#pragma once

#include <idl/ast.h>

/* --- Interface ---------------------------------------- */

IdlIface idl_iface(
    Str name,
    Alloc *alloc);

IdlIface idl_iface_barebone(
    Str name,
    Alloc *alloc);

void idl_alias(
    IdlIface *iface,
    Str name,
    IdlType type);

void idl_alias_mangled(
    IdlIface *iface,
    Str name,
    Str mangled,
    IdlType type);

void idl_alias_mangled(
    IdlIface *iface,
    Str name,
    Str mangled,
    IdlType type);

void idl_method(
    IdlIface *iface,
    Str name,
    IdlType request,
    IdlType response);

void idl_method_mangled(
    IdlIface *iface,
    Str name,
    Str mangled,
    IdlType request,
    IdlType response);

/* --- Types -------------------------------------------- */

IdlType idl_nil(
    void);

IdlType idl_primitive(
    Str str);

IdlType idl_primitive_mangled(
    Str str,
    Str mangled);

IdlType idl_enum(
    Alloc *alloc);

void idl_enum_constant(
    IdlType *enum_,
    Str name);

void idl_enum_constant_mangled(
    IdlType *enum_,
    Str name,
    Str mangled);

IdlType idl_struct(
    Alloc *alloc);

void idl_struct_member(
    IdlType *struct_,
    Str name,
    IdlType type);

IdlType idl_vec(IdlType subtype, Alloc *alloc);
