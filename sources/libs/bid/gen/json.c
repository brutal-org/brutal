#include <bid/gen/json.h>
#include <brutal/debug.h>

Json bidgen_json_primitive(BidPrimitive primitive_, Alloc *alloc)
{
    Json json = json_object_with_type(str$("BidPrimitive"), alloc);
    Json args_json = json_array(alloc);

    json_put(&json, str$("name"), json_string(primitive_.name, alloc));

    vec_foreach(arg, &primitive_.args)
    {
        json_append(&args_json, json_string(arg, alloc));
    }

    json_put(&json, str$("args"), args_json);

    return json;
}

Json bidgen_json_enum(BidEnum enum_, Alloc *alloc)
{
    Json json = json_object_with_type(str$("BidEnum"), alloc);
    Json members_json = json_array(alloc);

    vec_foreach(member, &enum_.members)
    {
        json_append(&members_json, json_string(member, alloc));
    }

    json_put(&json, str$("members"), members_json);

    return json;
}

Json bidgen_json_struct(BidStruct struct_, Alloc *alloc)
{
    Json json = json_object_with_type(str$("BidStruct"), alloc);
    Json members_json = json_array(alloc);

    vec_foreach(member, &struct_.members)
    {
        Json member_json = json_object(alloc);

        json_put(&member_json, str$("name"), json_string(member.name, alloc));
        json_put(&member_json, str$("type"), bidgen_json_type(member.type, alloc));

        json_append(&members_json, member_json);
    }

    json_put(&json, str$("members"), members_json);

    return json;
}

Json bidgen_json_type(BidType const type, Alloc *alloc)
{
    switch (type.type)
    {
    case BID_TYPE_PRIMITIVE:
        return bidgen_json_primitive(type.primitive_, alloc);

    case BID_TYPE_ENUM:
        return bidgen_json_enum(type.enum_, alloc);

    case BID_TYPE_STRUCT:
        return bidgen_json_struct(type.struct_, alloc);

    default:
        panic$("Unknown type type {}", type.type);
    }
}

Json bidgen_json_alias(BidAlias const alias, Alloc *alloc)
{
    Json json = json_object_with_type(str$("BidAlias"), alloc);

    json_put(&json, str$("name"), json_string(alias.name, alloc));
    json_put(&json, str$("type"), bidgen_json_type(alias.type, alloc));

    return json;
}

Json bidgen_json_event(BidEvent const event, Alloc *alloc)
{
    Json json = json_object_with_type(str$("BidEvent"), alloc);

    json_put(&json, str$("name"), json_string(event.name, alloc));
    json_put(&json, str$("data"), bidgen_json_type(event.data, alloc));

    return json;
}

Json bidgen_json_methode(BidMethod const methode, Alloc *alloc)
{
    Json json = json_object_with_type(str$("BidMethod"), alloc);

    json_put(&json, str$("name"), json_string(methode.name, alloc));
    json_put(&json, str$("request"), bidgen_json_type(methode.request, alloc));
    json_put(&json, str$("response"), bidgen_json_type(methode.response, alloc));

    return json;
}

Json bidgen_json_iface(BidIface const iface, Alloc *alloc)
{
    Json aliases_json = json_array(alloc);
    Json events_json = json_array(alloc);
    Json methodes_json = json_array(alloc);

    vec_foreach(alias, &iface.aliases)
    {
        json_append(&aliases_json, bidgen_json_alias(alias, alloc));
    }

    vec_foreach(event, &iface.events)
    {
        json_append(&events_json, bidgen_json_event(event, alloc));
    }

    vec_foreach(methode, &iface.methods)
    {
        json_append(&methodes_json, bidgen_json_methode(methode, alloc));
    }

    Json iface_json = json_object_with_type(str$("BidIface"), alloc);

    json_put(&iface_json, str$("name"), json_string(iface.name, alloc));
    json_put(&iface_json, str$("id"), json_number(iface.id));
    json_put(&iface_json, str$("errors"), bidgen_json_enum(iface.errors, alloc));
    json_put(&iface_json, str$("aliases"), aliases_json);
    json_put(&iface_json, str$("events"), events_json);
    json_put(&iface_json, str$("methods"), methodes_json);

    return iface_json;
}
