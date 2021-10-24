#include <ud/ast/sexpr.h>

UdExpr ud_nil(void);

UdExpr ud_bool(bool value);

UdExpr ud_atom(Str atom);

UdExpr ud_num(int64_t num);

UdExpr ud_rune(Rune rune);

UdExpr ud_str(Str str);

UdExpr ud_pair(Alloc *alloc, UdExpr lhs, UdExpr rhs);

UdExpr ud_builtin(UdBuiltin builtin);

UdExpr ud_syntax(UdSyntax syntax);

UdExpr ud_lambda(Alloc *alloc, UdExpr env, UdExpr parms, UdExpr body);
