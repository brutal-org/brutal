#include <idl/ast/query.h>

bool idl_type_in_interface(IdlIface const *iface, Str name)
{
    vec_foreach_v(type, &iface->aliases)
    {
        if (str_eq(type.name, name))
        {
            return true;
        }
    }

    return false;
}
