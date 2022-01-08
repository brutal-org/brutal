#pragma once

#include <bid/ast.h>
#include <cc/ast.h>

/* --- Types ---------------------------------------------------------------- */

CType bid_cgen_decl_primitive(BidType type);

CType bid_cgen_decl_enum(BidType type, Alloc *alloc);

CType bid_cgen_decl_struct(BidType type, Alloc *alloc);

CType bid_cgen_decl_vec(BidType type, Alloc *alloc);

CType bid_cgen_decl_type(BidType type, Alloc *alloc);

/* --- Pack ----------------------------------------------------------------- */

Str bid_cgen_pack_name(Str name, Alloc *alloc);

CExpr bid_cgen_pack_ref(Str name, Alloc *alloc);

CType bid_cgen_pack_type(BidAlias alias, Alloc *alloc);

void bid_cgen_pack_body(CStmt *block, BidType type, CExpr path, Alloc *alloc);

CDecl bid_cgen_pack_func(BidAlias alias, Alloc *alloc);

/* --- Unpack --------------------------------------------------------------- */

Str bid_cgen_unpack_name(Str name, Alloc *alloc);

CExpr bid_cgen_unpack_ref(Str name, Alloc *alloc);

CType bid_cgen_unpack_type(BidAlias alias, Alloc *alloc);

void bid_cgen_unpack_body(CStmt *block, BidType type, CExpr path, Alloc *alloc);

CDecl bid_cgen_unpack_func(BidAlias alias, Alloc *alloc);

/* --- Bindings ------------------------------------------------------------- */

CExpr bid_cgen_binding(BidMethod method, BidIface const iface, Alloc *alloc);

/* --- Methodes ------------------------------------------------------------- */

CType bid_cgen_method_type(BidMethod method, BidIface const iface, Alloc *alloc);

CStmt bid_cgen_method_body(BidMethod method, BidIface const iface, Alloc *alloc);

/* --- Provider ------------------------------------------------------------- */

CType bid_cgen_provider_type(BidIface const iface, Alloc *alloc);

CDecl bid_cgen_provider_func(BidIface const iface, Alloc *alloc);

/* --- Dispatch ------------------------------------------------------------- */

CType bid_cgen_dispatch_type(Alloc *alloc);

CExpr bid_cgen_dispatch_handler(BidMethod method, Alloc *alloc);

void bid_cgen_dispatch_case(CStmt *block, BidMethod method, BidIface const iface, Alloc *alloc);

CStmt bid_cgen_dispatch_body(BidIface const iface, Alloc *alloc);

CDecl bid_cgen_dispatch_func(BidIface const iface, Alloc *alloc);

/* --- Source & Header ------------------------------------------------------ */

CUnit bid_cgen_header(MAYBE_UNUSED BidIface const iface, Alloc *alloc);

CUnit bid_cgen_source(MAYBE_UNUSED BidIface const iface, Alloc *alloc);
