#pragma once
#include <ud/parse/parse.h>

UdDecl ud_parse_func_decl(Lex *lex, Alloc *alloc);
UdDecl ud_parse_var_decl(Lex *lex, Alloc *alloc);
UdDecl ud_parse_type_decl(Lex *lex, Alloc *alloc);
