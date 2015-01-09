#pragma once

namespace boloq {

/*!
 * @brief ノードのインデックスを一意になるように生成します
 */
template<class KT, class IT>
class index_generator {
    using key_type = KT;
    using index_type = IT;

    std::unordered_map<key_type, index_type> index_table;

public:

    explicit index_generator() = default;
    explicit index_generator(const size_t n) {
        index_table.reserve(n);
    }

    /*!
     * @brief インデックスを生成します
     */
    index_type get_index(const key_type& key) {
        const auto it = index_table.find(key);
        if (it != index_table.end()) {
            return it->second;
        }
        const index_type new_index = index_table.size();
        return index_table[key] = new_index;
    }
};

}
