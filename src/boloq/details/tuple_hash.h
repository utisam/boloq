#pragma once
namespace std{

namespace {

/*! \internal
 * Recursive template code derived from Matthieu M.
 */
template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
struct HashValueImpl {
    static void apply(size_t& seed, Tuple const& tuple) {
        HashValueImpl<Tuple, Index-1>::apply(seed, tuple);
        boost::hash_combine(seed, std::get<Index>(tuple));
    }
};

/*! \internal */
template <class Tuple>
struct HashValueImpl<Tuple, 0> {
    static void apply(size_t& seed, Tuple const& tuple) {
        boost::hash_combine(seed, std::get<0>(tuple));
    }
};

}

/*!
 * @brief tupleのためのハッシュ関数です
 */
template <typename ... TT>
struct hash<const std::tuple<TT...>> {
    /*!
     * 関数を実行します
     */
    size_t operator()(std::tuple<TT...> const& tt) const {
        size_t seed = 0;
        HashValueImpl<std::tuple<TT...> >::apply(seed, tt);
        return seed;
    }
};

}
