#include <bid/ast/query.h>

bool bid_type_in_interface(BidIface const *iface, Str name)
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
