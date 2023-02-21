#include <cc/ast/stmt.h>

char const *cstmt_type_to_str_raw[] = {
    [CSTMT_EMPTY] = "empty",
    [CSTMT_DECL] = "declaration",
    [CSTMT_EXPR] = "expression",
    [CSTMT_BLOCK] = "block",
    [CSTMT_IF] = "if",
    [CSTMT_FOR] = "for",
    [CSTMT_WHILE] = "while",
    [CSTMT_DO] = "do",
    [CSTMT_SWITCH] = "switch",
    [CSTMT_RETURN] = "return",
    [CSTMT_GOTO] = "goto",
    [CSTMT_BREAK] = "break",
    [CSTMT_CONTINUE] = "continue",
    [CSTMT_LABEL] = "label",
    [CSTMT_CASE] = "case",
    [CSTMT_DEFAULT] = "default",
};

Str cstmt_type_to_str(CStmtType type)
{
    return str$(cstmt_type_to_str_raw[type]);
}
