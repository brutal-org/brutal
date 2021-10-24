#include <udfore/udfore.h>

void ud_dump(UdExpr const *expr, IoWriter *writer)
{
    print(writer, "{}[", ud_type_to_string(expr->type));

    switch (expr->type)
    {
    case UD_NIL:
        break;

    case UD_BOOL:
        print(writer, expr->bool_ ? "true" : "false");
        break;

    case UD_ATOM:
        print(writer, "{}", expr->atom_);
        break;

    case UD_NUM:
        print(writer, "{}", expr->num_);
        break;

    case UD_RUNE:
        if (isprint(expr->rune_))
        {
            print(writer, "{c}", expr->rune_);
        }
        else
        {
            print(writer, "{#02x}", expr->rune_);
        }
        break;

    case UD_PAIR:
        print(writer, "lhs=");
        ud_dump(expr->pair_.lhs, writer);
        print(writer, ", rhs=");
        ud_dump(expr->pair_.rhs, writer);
        break;

    case UD_LAMBDA:
        print(writer, "params=");
        ud_dump(expr->lambda_.parms, writer);
        print(writer, ", env=");
        ud_dump(expr->lambda_.env, writer);
        print(writer, ", body=");
        ud_dump(expr->lambda_.body, writer);

    default:
        break;
    }

    print(writer, "]", ud_type_to_string(expr->type));
}
