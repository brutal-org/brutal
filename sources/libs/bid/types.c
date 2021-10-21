#include <bid/types.h>

bool bid_contain_attrib(BidType *type, Str attrib)
{
    for (int i = 0; i < type->attribs.length; i++)
    {
        if (str_eq(type->attribs.data[i].name, attrib))
        {
            return true;
        }
    }

    return false;
}
