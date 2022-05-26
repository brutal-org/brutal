#include "types.h"

#define AST2STR_FUNC(X) \
    [UDAST_##X] = #X,

const char *udast_type2str_array[] = {FOREACH_UD_AST(AST2STR_FUNC)};

Str udast_type_to_str(int type)
{
    return str$(udast_type2str_array[type]);
}
