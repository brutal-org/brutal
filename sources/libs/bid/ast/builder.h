#pragma once

#include <bid/ast.h>

/* --- Interface ---------------------------------------- */

BidIface bid_iface(
    Str name,
    Alloc *alloc);

BidIface bid_iface_barebone(
    Str name,
    Alloc *alloc);

void bid_alias(
    BidIface *iface,
    Str name,
    BidType type);

void bid_alias_mangled(
    BidIface *iface,
    Str name,
    Str mangled,
    BidType type);

void bid_alias_mangled(
    BidIface *iface,
    Str name,
    Str mangled,
    BidType type);

void bid_method(
    BidIface *iface,
    Str name,
    BidType request,
    BidType response);

void bid_method_mangled(
    BidIface *iface,
    Str name,
    Str mangled,
    BidType request,
    BidType response);

/* --- Types -------------------------------------------- */

BidType bid_nil(
    void);

BidType bid_primitive(
    Str str);

BidType bid_primitive_mangled(
    Str str,
    Str mangled);

BidType bid_enum(
    Alloc *alloc);

void bid_enum_member(
    BidType *enum_,
    Str name);

void bid_enum_constant_mangled(
    BidType *enum_,
    Str name,
    Str mangled);

BidType bid_struct(
    Alloc *alloc);

void bid_struct_member(
    BidType *struct_,
    Str name,
    BidType type);

BidType bid_vec(BidType subtype, Alloc *alloc);
