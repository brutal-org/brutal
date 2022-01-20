#include <idl/ast/query.h>

bool idl_resolve_alias(IdlModule module, Str name, IdlAlias *out_alias)
{
    vec_foreach_v(alias, &module.aliases)
    {
        if (str_eq(alias.name, name))
        {
            *out_alias = alias;
            return true;
        }
    }

    vec_foreach_v(import, &module.imports)
    {
        if (idl_resolve_alias(import.module, name, out_alias))
        {
            return true;
        }
    }

    return false;
}

bool idl_contain_alias(IdlModule module, Str name)
{
    vec_foreach_v(alias, &module.aliases)
    {
        if (str_eq(alias.name, name))
        {
            return true;
        }
    }

    return false;
}

bool idl_attrs_has(IdlAttrs attrs, Str name)
{
    vec_foreach_v(attr, &attrs)
    {
        if (str_eq(attr.name, name))
        {
            return true;
        }
    }

    return false;
}

bool idl_attrs_get(IdlAttrs attrs, Str name, IdlAttr *out_attr)
{
    vec_foreach_v(attr, &attrs)
    {
        if (str_eq(attr.name, name))
        {
            *out_attr = attr;
            return true;
        }
    }

    return false;
}
