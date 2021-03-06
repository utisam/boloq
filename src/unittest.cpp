#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boloq.h>
#include <boloq/io.h>

#include <boost/test/unit_test.hpp>
#include <array>
#include <unordered_set>
#include <iostream>

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

BOOST_AUTO_TEST_CASE(test_hash_test) {
    boolean_function x('x'), y('y');

    array<boolean_function, 8> fns = {{
        ~x | ~y, ~(x & y),
        ~x & ~y, ~(x | y),
        ~x | y, ~(x & ~y),
        ~x & y, ~(x | ~y),
    }};

    hash<boolean_function> hash_fn;
    for (size_t i = 0; i < fns.size(); i += 2) {
        BOOST_REQUIRE_EQUAL(fns[i], fns[i + 1]);
        BOOST_REQUIRE_EQUAL(hash_fn(fns[i]), hash_fn(fns[i + 1]));
    }

    unordered_set<boolean_function> fn_set(fns.begin(), fns.end());
    BOOST_REQUIRE_EQUAL(fn_set.size(), 4);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(boloq_combination_test)

BOOST_AUTO_TEST_CASE(test_offset) {
    combination x('x'), y('y'); // { x }, { y }
    auto f = (x + y); // { x, y }
    BOOST_REQUIRE_EQUAL(f.offset('x'), y);
    BOOST_REQUIRE_EQUAL(f.offset('y'), x);
    BOOST_REQUIRE_EQUAL(f.offset('z'), f);
    auto xy = x.changed('y'); // { xy }
    BOOST_REQUIRE_EQUAL(xy.offset('x'), combination::zero());
    BOOST_REQUIRE_EQUAL(xy.offset('y'), combination::zero());
    BOOST_REQUIRE_EQUAL(xy.offset('z'), xy);
}

BOOST_AUTO_TEST_CASE(test_onset) {
    combination x('x'), y('y');
    auto f = (x + y);
    BOOST_REQUIRE_EQUAL(f.onset('x'), combination::one());
    BOOST_REQUIRE_EQUAL(f.onset('y'), combination::one());
    BOOST_REQUIRE_EQUAL(f.onset('z'), combination::zero());
    auto xy = x.changed('y');
    BOOST_REQUIRE_EQUAL(xy.onset('x'), y);
    BOOST_REQUIRE_EQUAL(xy.onset('y'), x);
    BOOST_REQUIRE_EQUAL(xy.onset('z'), combination::zero());
}

BOOST_AUTO_TEST_CASE(test_change) {
    auto f = combination::one();
    f.change('x');
    BOOST_REQUIRE_EQUAL(f, combination('x'));
    auto assigns = assign_generator({{'x'}});
    for (auto& a : assigns) BOOST_REQUIRE_EQUAL(f.contain(a), a['x']);
}

BOOST_AUTO_TEST_CASE(test_union) {
    combination x('x'), y('y');
    auto f = x + y + x.changed('y');

    auto assigns = assign_generator({{'x', 'y'}});
    for (auto& a : assigns) {
        BOOST_REQUIRE_EQUAL(f.contain(a), (a['x'] || a['y']));
    }
}

BOOST_AUTO_TEST_CASE(test_intersection) {
    combination f('x');
    f.change('y');

    auto assigns = assign_generator({{'x', 'y'}});
    for (auto& a : assigns) {
        BOOST_REQUIRE_EQUAL(f.contain(a), (a['x'] && a['y']));
    }
}

BOOST_AUTO_TEST_CASE(test_join) {
    combination x('x'), y('y'), z('z'), w('w');
    auto xy = x * y;
    BOOST_REQUIRE_EQUAL(xy, x.changed('y'));
    BOOST_REQUIRE_EQUAL(xy * x * y, x.changed('y'));
    BOOST_REQUIRE_EQUAL((xy + x + y) * x * y, x.changed('y'));

    count_visitor<combination, size_t> cv;
    // {xz, xw, yz, yw}
    BOOST_REQUIRE_EQUAL(((x + y) * (z + w)).accept(cv), 4);
}

BOOST_AUTO_TEST_CASE(test_meet) {
    combination x('x'), y('y'), z('z');
    auto f = x * y + x * z;
    BOOST_REQUIRE_EQUAL(f.meet(y * z), y + z);
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

    count_visitor<combination, size_t> cv;
    BOOST_REQUIRE_EQUAL(f.accept(cv), 5);
}

BOOST_AUTO_TEST_CASE(test_hash_test) {
    combination x('x'), y('y');
    combination o = combination::one();

    array<combination, 2> fns = {{
        x.changed('y'), y.changed('x'),
    }};

    hash<combination> hash_fn;
    for (size_t i = 0; i < fns.size(); i += 2) {
        BOOST_REQUIRE_EQUAL(fns[i], fns[i + 1]);
        BOOST_REQUIRE_EQUAL(hash_fn(fns[i]), hash_fn(fns[i + 1]));
    }

    unordered_set<combination> fn_set(fns.begin(), fns.end());
    BOOST_REQUIRE_EQUAL(fn_set.size(), 1);
}

BOOST_AUTO_TEST_CASE(test_count) {
    count_visitor<combination, size_t> cv;

    combination x('x'), y('y'); // { x }, { y }
    auto f = (x + y); // { x, y }

    BOOST_REQUIRE_EQUAL(x.accept(cv), 1);
    BOOST_REQUIRE_EQUAL(y.accept(cv), 1);
    BOOST_REQUIRE_EQUAL(f.accept(cv), 2);
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
