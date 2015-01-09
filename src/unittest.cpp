#include <boloq.h>
#include <boloq/io.h>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <iostream>
#include <boost/test/unit_test.hpp>

using namespace std;
using namespace boloq;

vector<unordered_map<size_t, bool>> assign_generator(const vector<size_t>& labels) {
    vector<unordered_map<size_t, bool>> result;
    for (size_t i = 0; i < (1U << labels.size()); i++) {
        unordered_map<size_t, bool> assign;
        for (size_t j = 0; j < labels.size(); j++) {
            assign[labels[j]] = (i & (1U << j));
        }
        result.push_back(assign);
    }
    return result;
}

BOOST_AUTO_TEST_SUITE(boloq_boolean_function_test)

BOOST_AUTO_TEST_CASE(test_de_morgans_low_A) {
    boolean_function x('x'), y('y');

    auto a = ~x | ~y;
    auto b = ~(x & y);
    BOOST_REQUIRE((~x | ~y) == ~(x & y));
    BOOST_REQUIRE((~x | ~y) == a);
    BOOST_REQUIRE(~(x & y) == b);
    auto assigns = assign_generator({{'x', 'y'}});
    for (auto& assign : assigns) {
        BOOST_REQUIRE_EQUAL(a.execute(assign), !(assign['x'] && assign['y']));
    }
}

BOOST_AUTO_TEST_CASE(test_de_morgans_low_B) {
    boolean_function x('x'), y('y');

    auto a = ~x & ~y;
    auto b = ~(x | y);
    BOOST_REQUIRE(a == b);
    auto assigns = assign_generator({{'x', 'y'}});
    for (auto& assign : assigns) {
        BOOST_REQUIRE_EQUAL(a.execute(assign), !(assign['x'] || assign['y']));
    }
}

BOOST_AUTO_TEST_CASE(test_xor2) {
    boolean_function x('x'), y('y');

    auto a = x ^ y;
    auto assigns = assign_generator({{'x', 'y'}});
    for (auto& assign : assigns) {
        BOOST_REQUIRE_EQUAL(a.execute(assign), assign['x'] ^ assign['y']);
    }
}

BOOST_AUTO_TEST_CASE(test_assign_by_array) {
    boolean_function x(0);
    const array<size_t, 1> a = {{true}};
    BOOST_REQUIRE(x.execute(a));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(boloq_combination_test)

BOOST_AUTO_TEST_CASE(test_or) {
    combination x('x'), y('y');
    auto f = x + y + x.changed('y');

    auto assigns = assign_generator({{'x', 'y'}});
    for (auto& a : assigns) {
        BOOST_REQUIRE_EQUAL(f.contain(a), (a['x'] || a['y']));
    }
}

BOOST_AUTO_TEST_CASE(test_and) {
    combination f('x');
    f.change('y');

    auto assigns = assign_generator({{'x', 'y'}});
    for (auto& a : assigns) {
        BOOST_REQUIRE_EQUAL(f.contain(a), (a['x'] && a['y']));
    }
}

BOOST_AUTO_TEST_CASE(test_construction) {
    combination _0001('d');
    combination _000x = _0001 + combination::one();
    combination _001x = _000x.changed('c');
    combination _0100('b');
    combination _00xx = _001x + _000x;
    auto f = _00xx + _0100;

    auto assigns = assign_generator({{'a', 'b', 'c', 'd'}});
    for (auto& a : assigns) {
        BOOST_REQUIRE_EQUAL(f.contain(a), (!a['a'] && !a['b']) || (!a['a'] && !a['c'] && !a['d']));
    }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(boloq_function_types_test)

BOOST_AUTO_TEST_CASE(test_is_wire) {
    boolean_function x('x');
    BOOST_REQUIRE(x.is_wire());

    BOOST_REQUIRE(!boolean_function::one().is_wire());
    BOOST_REQUIRE(!boolean_function::zero().is_wire());
}

BOOST_AUTO_TEST_CASE(test_is_negation) {
    boolean_function x('x');
    BOOST_REQUIRE((~x).is_negation());

    BOOST_REQUIRE(!boolean_function::one().is_negation());
    BOOST_REQUIRE(!boolean_function::zero().is_negation());
}

BOOST_AUTO_TEST_CASE(test_is_conjunction) {
    boolean_function x('x'), y('y'), z('z');
    BOOST_REQUIRE(x.is_conjunction());
    BOOST_REQUIRE((x & y).is_conjunction());
    BOOST_REQUIRE((x & y & z).is_conjunction());

    BOOST_REQUIRE(!boolean_function::one().is_conjunction());
    BOOST_REQUIRE(!boolean_function::zero().is_conjunction());

    BOOST_REQUIRE(!(x | y).is_conjunction());
    BOOST_REQUIRE(!(x ^ y).is_conjunction());
}

BOOST_AUTO_TEST_CASE(test_is_disjunction) {
    boolean_function x('x'), y('y'), z('z');
    BOOST_REQUIRE(x.is_disjunction());
    BOOST_REQUIRE((x | y).is_disjunction());
    BOOST_REQUIRE((x | y | x).is_disjunction());

    BOOST_REQUIRE(!boolean_function::one().is_disjunction());
    BOOST_REQUIRE(!boolean_function::zero().is_disjunction());

    BOOST_REQUIRE(!(x & y).is_disjunction());
    BOOST_REQUIRE(!(x ^ y).is_disjunction());
}

BOOST_AUTO_TEST_CASE(test_is_exclusive_disjunction) {
    boolean_function x('x'), y('y'), z('z');

    BOOST_REQUIRE(x.is_exclusive_disjunction());
    BOOST_REQUIRE((x ^ y).is_exclusive_disjunction());
    BOOST_REQUIRE((x ^ y ^ z).is_exclusive_disjunction());

    BOOST_REQUIRE(!boolean_function::one().is_exclusive_disjunction());
    BOOST_REQUIRE(!boolean_function::zero().is_exclusive_disjunction());

    BOOST_REQUIRE(!(x & y).is_exclusive_disjunction());
    BOOST_REQUIRE(!(x | y).is_exclusive_disjunction());
}

BOOST_AUTO_TEST_SUITE_END()
