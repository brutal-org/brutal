#include <bid/c_convert/bid_convert_header.h>

void bid_write_header(const struct bid *bid, IoWriter *writer)
{
    (void)bid;
    print(writer, "#pragma once\n\n");
    print(writer, "#include <brutal/base.h>\n\n");
    print(writer, "// -- generated idl file --\n\n");
}

Str bid_write_interface(IoWriter *writer, struct bid_ast_node *interface)
{
    print(writer, "// interface: {}\n\n", interface->interface.name);
    return interface->interface.name;
}
