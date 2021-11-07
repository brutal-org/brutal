#include <bid/ast.h>
#include <bid/ast/builder.h>
#include <brutal/io.h>

BidIface bid_pass_prefix(BidIface iface, Alloc *alloc)
{
    BidIface prefixed = bid_iface(alloc);

    prefixed.name = str_dup(iface.name, alloc);
    prefixed.id = iface.id;

    vec_foreach(alias, &iface.aliases)
    {
        Str name = str_fmt(alloc, "{case:pascal}{case:pascal}", iface.name, alias.name);
        bid_alias(&prefixed, name, alias.type);
    }

    vec_foreach(event, &iface.events)
    {
        Str name = str_fmt(alloc, "{case:snake}_{case:snake}", iface.name, event.name);
        BidType prefixed_data = event.data;

        if (prefixed_data.type != BID_TYPE_PRIMITIVE)
        {
            Str name = str_fmt(alloc, "{case:pascal}{case:pascal}Data", iface.name, event.name);
            bid_alias(&prefixed, name, prefixed_data);
            prefixed_data = bid_primitive(name, alloc);
        }

        bid_event(&prefixed, name, prefixed_data);
    }

    vec_foreach(methods, &iface.methods)
    {
        Str name = str_fmt(alloc, "{case:snake}_{case:snake}", iface.name, methods.name);

        BidType prefixed_request = methods.request;
        BidType prefixed_response = methods.response;

        if (prefixed_request.type != BID_TYPE_PRIMITIVE)
        {
            Str name = str_fmt(alloc, "{case:pascal}{case:pascal}Request", iface.name, methods.name);
            bid_alias(&prefixed, name, prefixed_request);
            prefixed_request = bid_primitive(name, alloc);
        }

        if (prefixed_response.type != BID_TYPE_PRIMITIVE)
        {
            Str name = str_fmt(alloc, "{case:pascal}{case:pascal}Response", iface.name, methods.name);
            bid_alias(&prefixed, name, prefixed_response);
            prefixed_response = bid_primitive(name, alloc);
        }

        bid_method(&prefixed, name, prefixed_request, prefixed_response);
    }

    return prefixed;
}
