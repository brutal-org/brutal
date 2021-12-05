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

bool bid_type_is_handle(BidType *type)
{
    if (type->type != BID_TYPE_PRIMITIVE)
    {
        return false;
    }

    return str_eq(type->primitive_.name, str$("BrHandle")) ||
           str_eq(type->primitive_.name, str$("BrSpace")) ||
           str_eq(type->primitive_.name, str$("BrMemObj")) ||
           str_eq(type->primitive_.name, str$("BrTask")) ||
           str_eq(type->primitive_.name, str$("BrIrq"));
}
