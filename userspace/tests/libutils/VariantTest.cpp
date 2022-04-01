#include <libutils/Var.h>

#include "tests/Driver.h"

using Number = Var<int, double>;

TEST(variant_default_constructor)
{
    Number nbr;

    Assert::truth(nbr.is<int>());
    Assert::falsity(nbr.is<double>());

    Assert::equal(nbr.get<int>(), 0);
}

TEST(variant_is_method)
{
    Number nbr;

    nbr = 5;
    Assert::truth(nbr.is<int>());
    Assert::falsity(nbr.is<double>());

    nbr = 6.;
    Assert::truth(nbr.is<double>());
    Assert::falsity(nbr.is<int>());
}

TEST(variant_get_method)
{
    Number nbr;

    nbr = 5;
    Assert::equal(nbr.get<int>(), 5);

    nbr = 6.;
    Assert::equal(nbr.get<double>(), 6);
}

TEST(variant_with_method)
{
    Number nbr;

    nbr = 5;
    nbr.with<double>([](double &)
        { Assert::unreachable(); });

    nbr.with<int>([](int &v)
        { Assert::equal(v, 5); });

    nbr = 6.;
    nbr.with<int>([](int &)
        { Assert::unreachable(); });

    nbr.with<double>([](double &v)
        { Assert::equal(v, 6.); });

    Assert::equal(nbr.get<double>(), 6);
}

TEST(variant_visit)
{
    Number nbr5{5};
    Number nbr6{6.0};

    bool has_visited_int = false;
    bool has_visited_double = false;

    Visitor visitor{
        [&](int &value)
        {
            Assert::equal(value, 5);
            has_visited_int = true;
        },
        [&](double &value)
        {
            Assert::equal(value, 6.);
            has_visited_double = true;
        },
    };

    nbr5.visit(visitor);
    nbr6.visit(visitor);

    Assert::truth(has_visited_int);
    Assert::truth(has_visited_double);
}

TEST(variant_copy_ctor)
{
    Number nbr{5};
    Number other = nbr;

    Assert::truth(other.is<int>());
    Assert::equal(other.get<int>(), 5);
}

TEST(variant_copy_assign)
{
    Number nbr{5};
    Number other{nbr};

    Assert::truth(other.is<int>());
    Assert::equal(other.get<int>(), 5);
}

TEST(variant_move_ctor)
{
    Number nbr{5};
    Number other{std::move(nbr)};

    Assert::truth(other.is<int>());
    Assert::equal(other.get<int>(), 5);
}

TEST(variant_move_assign)
{
    Number nbr{5};
    Number other = std::move(nbr);

    Assert::truth(other.is<int>());
    Assert::equal(other.get<int>(), 5);
}
