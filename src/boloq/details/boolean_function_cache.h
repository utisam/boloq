#pragma once

namespace boloq {

/*!
 * @brief 基本的な演算キャッシュのテーブルです
 */
template<class N>
class basic_boolean_function_cache {
public:
    /*! @brief このクラスが扱うノードの型 */
    using node_type = N;
    /*! @brief このクラスが扱うノードのポインタ型 */
    using node_ptr = typename node_type::node_ptr;

private:
    using self_type = basic_boolean_function_cache<N>;

    using index_type = typename node_type::index_type;
    using label_type = typename node_type::label_type;

    using unique_key_type = const std::tuple<label_type, index_type, index_type>;
    using compute_key_type = const std::tuple<index_type, index_type, index_type>;

    using cache_ptr = std::weak_ptr<const typename node_ptr::element_type>;

    using unique_table_type = std::unordered_map<unique_key_type, cache_ptr>;
    using compute_table_type = std::unordered_map<compute_key_type, cache_ptr>;

    const node_type __terminal_false, __terminal_true;
    const node_ptr terminal_false, terminal_true;

    index_generator<unique_key_type, index_type> igen;

    unique_table_type unique_table;
    compute_table_type compute_table;

    const node_ptr next_then_node(const node_ptr& n, const label_type& label) const {
        if (n->label() != label) return n;
        return n->then_node();
    }

    const node_ptr next_else_node(const node_ptr& n, const label_type& label) const {
        if (n->label() != label) return n;
        return n->else_node();
    }

    /*!
     * unique tableのための検索キーを生成します
     */
    static unique_key_type make_unique_key(const label_type& l, const node_ptr& t, const node_ptr& e) {
        return unique_key_type(l, t->index(), e->index());
    }

    /*!
     * compute tableのための検索キーを生成します
     */
    static compute_key_type make_compute_key(const node_ptr& i, const node_ptr& t, const node_ptr& e) {
        return compute_key_type(i->index(), t->index(), e->index());
    }

public:

    /*!
     * @brief コンストラクタ
     */
    constexpr basic_boolean_function_cache() :
            __terminal_false(0), __terminal_true(1),
            terminal_false(&__terminal_false, null_deleter()),
            terminal_true(&__terminal_true, null_deleter())
    {}

    /*! @brief コピーは禁止されています */
    basic_boolean_function_cache(const basic_boolean_function_cache&) = delete;
    /*! @brief 代入は禁止されています */
    basic_boolean_function_cache& operator=(const basic_boolean_function_cache&) = delete;

    /*!
     * @brief 0定節点を返します
     */
    const node_ptr& zero() const {return terminal_false;}
    /*!
     * @brief 1定節点を返します
     */
    const node_ptr& one() const {return terminal_true;}

    /*!
     * @brief 新しいノードを生成します
     */
    const node_ptr new_var(const label_type& _label, const node_ptr& t, const node_ptr& e) {
        // もうすでに存在するなら既存のノードを返す
        const unique_key_type key = make_unique_key(_label, t, e);
        const auto it = unique_table.find(key);
        if (it != unique_table.end() && !it->second.expired()) {
            return it->second.lock();
        }
        // 存在しなければ新しく生成
        const node_ptr pf(new node_type(igen.get_index(key) + 2, _label, t, e));
        unique_table[key] = pf; // 登録
        return pf;
    }

    /*!
     * @brief 新しいノードを生成します
     */
    const node_ptr new_var(const label_type& _label) {
        return new_var(_label, one(), zero());
    }

    /*!
     * if-then-else に基づいてBDDをマージする
     */
    const node_ptr ite(const node_ptr& if_node, const node_ptr& then_node, const node_ptr& else_node) {
        // if_node が終端なら then_node もしくは else_node を定義どおりに返す
        if (if_node->is_terminal()) {
            return (if_node->index()) ? then_node : else_node;
        }

        // 計算済みなら計算結果を返す
        const compute_key_type compute_key = make_compute_key(if_node, then_node, else_node);
        const auto it = compute_table.find(compute_key);
        if (it != compute_table.end() && !it->second.expired()) {
            return it->second.lock();
        }

        const label_type& v = std::min(if_node->label(),
                                      std::min(then_node->label(),
                                               else_node->label()));

        // 子を計算
        const node_ptr then_child = ite(next_then_node(if_node, v),
                                        next_then_node(then_node, v),
                                        next_then_node(else_node, v));
        const node_ptr else_child = ite(next_else_node(if_node, v),
                                        next_else_node(then_node, v),
                                        next_else_node(else_node, v));

        // ルールに従ってノードをスキップする
        if (then_child == else_child) return then_child;

        // ノード (v, T, E) が存在しないなら新しいノードを生成
        const node_ptr unique = new_var(v, then_child, else_child);
        // 計算結果を登録
        compute_table[compute_key] = unique;
        return unique;
    }

    /*!
     * @brief not を適用した結果を返します
     */
    const node_ptr apply_not(const node_ptr& a) {
        return ite(a, zero(), one());
    }

    /*!
     * @brief and を適用した結果を返します
     */
    const node_ptr apply_and(const node_ptr& a, const node_ptr& b) {
        return ite(a, b, zero());
    }

    /*!
     * @brief or を適用した結果を返します
     */
    const node_ptr apply_or(const node_ptr& a, const node_ptr& b) {
        return ite(a, one(), b);
    }

    /*!
     * @brief or を適用した結果を返します
     */
    const node_ptr apply_xor(const node_ptr& a, const node_ptr& b) {
        return ite(a, apply_not(b), b);
    }
};

}
