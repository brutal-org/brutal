#include <bid/ast.h>
#include <bid/ast/builder.h>
#include <brutal/debug.h>
#include <brutal/io.h>

static BidType prefix_type(BidType type, BidIface iface, Alloc *alloc)
{
    BidType prefixed;

    switch (type.type)
    {

    case BID_TYPE_NIL:
        return bid_nil();

    case BID_TYPE_PRIMITIVE:
    {
        Str name = type.primitive_.name;
        Str mangled = name;

        if (bid_type_in_interface(&iface, type.primitive_.name))
        {
            mangled = str_fmt(alloc, "{case:pascal}{case:pascal}", iface.name, type.primitive_.name);
        }

        return bid_primitive_mangled(name, mangled);
    }

    case BID_TYPE_ENUM:
    {
        prefixed = bid_enum(alloc);

        vec_foreach(membr, &type.enum_.members)
        {
            Str mangled = str_fmt(alloc, "{case:constant}_{case:constant}", iface.name, membr.name);
            bid_enum_constant_mangled(&prefixed, membr.name, mangled);
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

    case BID_TYPE_VEC:
    {
        return bid_vec(prefix_type(*type.vec_.subtype, iface, alloc), alloc);
    }

    default:
        panic$("Unknow type type {}", type.type);
    }
}

BidIface bid_pass_prefix(BidIface iface, Alloc *alloc)
{
    BidIface prefixed = bid_iface(iface.name, alloc);

    prefixed.id = iface.id;

    Str error_name = str_fmt(alloc, "{case:pascal}Error", iface.name);
    bid_alias_mangled(&prefixed, str$("Error"), error_name, prefix_type(iface.errors, iface, alloc));
    prefixed.errors = bid_primitive_mangled(str$("Error"), error_name);

    vec_foreach(alias, &iface.aliases)
    {
        Str name = str_fmt(alloc, "{case:pascal}{case:pascal}", iface.name, alias.name);
        bid_alias_mangled(&prefixed, alias.name, name, prefix_type(alias.type, iface, alloc));
    }

    vec_foreach(methods, &iface.methods)
    {
        Str name = str_fmt(alloc, "{case:snake}_{case:snake}", iface.name, methods.name);

        BidType prefixed_request = prefix_type(methods.request, iface, alloc);
        BidType prefixed_response = prefix_type(methods.response, iface, alloc);

        if (prefixed_request.type != BID_TYPE_PRIMITIVE && prefixed_request.type != BID_TYPE_NIL)
        {
            Str request_name = str_fmt(alloc, "{case:pascal}Request", iface.name, methods.name);
            Str mangled = str_fmt(alloc, "{case:pascal}{case:pascal}Request", iface.name, methods.name);
            bid_alias_mangled(&prefixed, request_name, mangled, prefixed_request);
            prefixed_request = bid_primitive_mangled(name, mangled);
        }

        if (prefixed_response.type != BID_TYPE_PRIMITIVE && prefixed_response.type != BID_TYPE_NIL)
        {
            Str response_name = str_fmt(alloc, "{case:pascal}Response", iface.name, methods.name);
            Str mangled = str_fmt(alloc, "{case:pascal}{case:pascal}Response", iface.name, methods.name);
            bid_alias_mangled(&prefixed, response_name, mangled, prefixed_response);
            prefixed_response = bid_primitive_mangled(name, mangled);
        }

        bid_method_mangled(&prefixed, methods.name, name, prefixed_request, prefixed_response);
    }

    return prefixed;
}
