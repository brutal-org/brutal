#include <bid/ast.h>
#include <bid/ast/builder.h>
#include <brutal/io.h>

static BidType prefix_type(BidType type, BidIface iface, Alloc *alloc)
{
    BidType prefixed;

    switch (type.type)
    {

    case BID_TYPE_PRIMITIVE:
    {
        Str name = type.primitive_.name;

        if (bid_type_in_interface(&iface, type.primitive_.name))
        {
            name = str_fmt(alloc, "{case:pascal}{case:pascal}", iface.name, type.primitive_.name);
        }

        prefixed = bid_primitive(name, alloc);

        vec_foreach(arg, &type.primitive_.args)
        {
            if (bid_type_in_interface(&iface, arg))
            {
                arg = str_fmt(alloc, "{case:pascal}{case:pascal}", iface.name, arg);
            }

            bid_primitive_arg(&prefixed, arg);
        }

        return prefixed;
    }

    case BID_TYPE_ENUM:
    {
        prefixed = bid_enum(alloc);

        vec_foreach(membr, &type.enum_.members)
        {
            membr = str_fmt(alloc, "{case:constant}_{case:constant}", iface.name, membr);
            bid_enum_constant(&prefixed, membr);
        }

        return prefixed;
    }

    case BID_TYPE_STRUCT:
    {
        prefixed = bid_struct(alloc);

        vec_foreach(membr, &type.struct_.members)
        {
            bid_struct_member(&prefixed, membr.name, prefix_type(membr.type, iface, alloc));
        }

        return prefixed;
    }

    default:
        return type;
    }
}

BidIface bid_pass_prefix(BidIface iface, Alloc *alloc)
{
    BidIface prefixed = bid_iface(alloc);

    prefixed.name = str_dup(iface.name, alloc);
    prefixed.id = iface.id;

    Str error_name = str_fmt(alloc, "{case:pascal}Error", iface.name);
    bid_alias(&prefixed, error_name, prefix_type(iface.errors, iface, alloc));
    prefixed.errors = bid_primitive(error_name, alloc);

    vec_foreach(alias, &iface.aliases)
    {
        Str name = str_fmt(alloc, "{case:pascal}{case:pascal}", iface.name, alias.name);
        bid_alias(&prefixed, name, prefix_type(alias.type, iface, alloc));
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

        bid_event(&prefixed, name, prefix_type(prefixed_data, iface, alloc));
    }

    vec_foreach(methods, &iface.methods)
    {
        Str name = str_fmt(alloc, "{case:snake}_{case:snake}", iface.name, methods.name);

        BidType prefixed_request = methods.request;
        BidType prefixed_response = methods.response;

        if (prefixed_request.type != BID_TYPE_PRIMITIVE)
        {
            Str name = str_fmt(alloc, "{case:pascal}{case:pascal}Request", iface.name, methods.name);
            bid_alias(&prefixed, name, prefix_type(prefixed_request, iface, alloc));
            prefixed_request = bid_primitive(name, alloc);
        }

        if (prefixed_response.type != BID_TYPE_PRIMITIVE)
        {
            Str name = str_fmt(alloc, "{case:pascal}{case:pascal}Response", iface.name, methods.name);
            bid_alias(&prefixed, name, prefix_type(prefixed_response, iface, alloc));
            prefixed_response = bid_primitive(name, alloc);
        }

        bid_method(&prefixed, name, prefixed_request, prefixed_response);
    }

    return prefixed;
}
