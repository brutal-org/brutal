#include <bid/json.h>
#include <brutal/debug.h>

Json bid_jgen_primitive(BidPrimitive primitive_, Alloc *alloc)
{
    Json json = json_object_with_type(str$("BidPrimitive"), alloc);

    json_put(&json, str$("name"), json_str(primitive_.name));
    json_put(&json, str$("mangled"), json_str(primitive_.mangled));

    return json;
}

Json bid_jgen_enum(BidEnum enum_, Alloc *alloc)
{
    Json json = json_object_with_type(str$("BidEnum"), alloc);
    Json members_json = json_array(alloc);

    vec_foreach_v(member, &enum_.members)
    {
        Json member_json = json_object(alloc);

        json_put(&member_json, str$("name"), json_str(member.name));
        json_put(&member_json, str$("mangled"), json_str(member.mangled));

        json_append(&members_json, member_json);
    }

    json_put(&json, str$("members"), members_json);

    return json;
}

Json bid_jgen_struct(BidStruct struct_, Alloc *alloc)
{
    Json json = json_object_with_type(str$("BidStruct"), alloc);
    Json members_json = json_array(alloc);

    vec_foreach_v(member, &struct_.members)
    {
        Json member_json = json_object(alloc);

        json_put(&member_json, str$("name"), json_str(member.name));
        json_put(&member_json, str$("type"), bid_jgen_type(member.type, alloc));

        json_append(&members_json, member_json);
    }

    json_put(&json, str$("members"), members_json);

    return json;
}

Json bid_jgen_vec(BidVec const vec_, Alloc *alloc)
{
    Json json = json_object_with_type(str$("BidVec"), alloc);

    json_put(&json, str$("subtype"), bid_jgen_type(*vec_.subtype, alloc));

    return json;
}

Json bid_jgen_type(BidType const type, Alloc *alloc)
{
    switch (type.type)
    {
    case BID_TYPE_NIL:
        return json_object_with_type(str$("BidNil"), alloc);

    case BID_TYPE_PRIMITIVE:
        return bid_jgen_primitive(type.primitive_, alloc);

    case BID_TYPE_ENUM:
        return bid_jgen_enum(type.enum_, alloc);

    case BID_TYPE_STRUCT:
        return bid_jgen_struct(type.struct_, alloc);

    case BID_TYPE_VEC:
        return bid_jgen_vec(type.vec_, alloc);

    default:
        panic$("Unknown type type {}", type.type);
    }
}

Json bid_jgen_alias(BidAlias const alias, Alloc *alloc)
{
    Json json = json_object_with_type(str$("BidAlias"), alloc);

    json_put(&json, str$("name"), json_str(alias.name));
    json_put(&json, str$("type"), bid_jgen_type(alias.type, alloc));

    return json;
}

Json bid_jgen_method(BidMethod const method, Alloc *alloc)
{
    Json json = json_object_with_type(str$("BidMethod"), alloc);

    json_put(&json, str$("name"), json_str(method.name));
    json_put(&json, str$("mangled"), json_str(method.mangled));
    json_put(&json, str$("request"), bid_jgen_type(method.request, alloc));
    json_put(&json, str$("response"), bid_jgen_type(method.response, alloc));

    return json;
}

Json bid_jgen_iface(BidIface const iface, Alloc *alloc)
{
    Json aliases_json = json_array(alloc);
    Json methods_json = json_array(alloc);

    vec_foreach_v(alias, &iface.aliases)
    {
        json_append(&aliases_json, bid_jgen_alias(alias, alloc));
    }

    vec_foreach_v(method, &iface.methods)
    {
        json_append(&methods_json, bid_jgen_method(method, alloc));
    }

    Json iface_json = json_object_with_type(str$("BidIface"), alloc);

    json_put(&iface_json, str$("id"), json_number(iface.id));
    json_put(&iface_json, str$("name"), json_str(iface.name));
    json_put(&iface_json, str$("errors"), bid_jgen_type(iface.errors, alloc));
    json_put(&iface_json, str$("aliases"), aliases_json);
    json_put(&iface_json, str$("methods"), methods_json);

    return iface_json;
}
