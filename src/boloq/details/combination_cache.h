#pragma once

namespace boloq {

/*!
 * @brief 基本的な演算キャッシュのテーブルです
 */
template<class N>
class basic_combination_cache {
public:
    /*! @brief このクラスが扱うノードの型 */
    using node_type = N;
    /*! @brief このクラスが扱うノードのポインタ型 */
    using node_ptr = typename node_type::node_ptr;

private:
    using self_type = basic_combination_cache<N>;

    using index_type = typename node_type::index_type;
    using label_type = typename node_type::label_type;

    using unique_key_type = const std::tuple<label_type, index_type, index_type>;
    using change_key_type = const std::tuple<index_type, label_type>;
    using bin_op_key_type = const std::tuple<index_type, index_type>;

    using cache_ptr = std::weak_ptr<typename node_ptr::element_type>;

    index_generator<unique_key_type, index_type> igen;

    std::unordered_map<unique_key_type, cache_ptr> unique_table;
    std::unordered_map<change_key_type, cache_ptr> offset_table;
    std::unordered_map<change_key_type, cache_ptr> onset_table;
    std::unordered_map<change_key_type, cache_ptr> change_table;
    std::unordered_map<bin_op_key_type, cache_ptr> union_table;
    std::unordered_map<bin_op_key_type, cache_ptr> intersection_table;
    std::unordered_map<bin_op_key_type, cache_ptr> join_table;
    std::unordered_map<bin_op_key_type, cache_ptr> meet_table;

    const node_type __terminal_false, __terminal_true;
    const node_ptr terminal_false, terminal_true;

    /*!
     * unique tableのための検索キーを生成します
     */
    static unique_key_type make_unique_key(const label_type& l, const node_ptr& t, const node_ptr& e) {
        return std::make_tuple(l, t->index(), e->index());
    }

    /*!
     * change tableのための検索キーを生成します
     */
    static change_key_type make_change_key(const node_ptr& n, const label_type& l) {
        return std::make_tuple(n->index(), l);
    }

    /*!
     * 二項演算のための検索キーを生成します
     */
    static bin_op_key_type make_bin_op_key(const node_ptr& p, const node_ptr& q) {
        return std::make_tuple(p->index(), q->index());
    }

public:

    /*!
     * @brief コンストラクタ
     */
    basic_combination_cache() :
            __terminal_false(0), __terminal_true(1),
            terminal_false(&__terminal_false, null_deleter()),
            terminal_true(&__terminal_true, null_deleter())
    {}

    /*! @brief コピーは禁止されています */
    basic_combination_cache(const basic_combination_cache&) = delete;
    /*! @brief 代入は禁止されています */
    basic_combination_cache& operator=(const basic_combination_cache&) = delete;

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
        if (t == zero()) return e;
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

    const node_ptr apply_offset(const node_ptr& _root, const label_type& v) {
        if (_root->label() == v) return _root->else_node();
        if (_root->label() > v) return _root;
        const auto key = make_change_key(_root, v);
        const auto it = offset_table.find(key);
        if (it != offset_table.end() && !it->second.expired()) {
            return it->second.lock();
        }
        const node_ptr& r = new_var(_root->label(),
                                   apply_offset(_root->then_node(), v),
                                   apply_offset(_root->else_node(), v));
        offset_table[key] = r;
        return r;
    }

    const node_ptr apply_onset(const node_ptr& _root, const label_type& v) {
        if (_root->label() == v) return _root->then_node();
        if (_root->label() > v) return zero();
        const auto key = make_change_key(_root, v);
        const auto it = onset_table.find(key);
        if (it != onset_table.end() && !it->second.expired()) {
            return it->second.lock();
        }
        const node_ptr& r = new_var(_root->label(),
                                   apply_onset(_root->then_node(), v),
                                   apply_onset(_root->else_node(), v));
        onset_table[key] = r;
        return r;
    }

    /*!
     * @brief 特定のアイテムの存在を反転させた結果を返します
     */
    const node_ptr apply_change(const node_ptr& _root, const label_type& v) {
        if (_root->label() == v) return new_var(v, _root->else_node(), _root->then_node());
        if (_root->label() > v) return new_var(v, _root, zero());
        const auto key = make_change_key(_root, v);
        const auto it = change_table.find(key);
        if (it != change_table.end() && !it->second.expired()) {
            return it->second.lock();
        }
        const node_ptr& r = new_var(_root->label(),
                                   apply_change(_root->then_node(), v),
                                   apply_change(_root->else_node(), v));
        change_table[key] = r;
        return r;
    }

    /*!
     * @brief 和集合を返します
     */
    const node_ptr apply_union(const node_ptr& p, const node_ptr& q) {
        if (p == zero()) return q;
        if (q == zero() || p == q) return p;
        const auto key = make_bin_op_key(p, q);
        const auto it = union_table.find(key);
        if (it != union_table.end() && !it->second.expired()) {
            return it->second.lock();
        }

        node_ptr r;
        if (p->label() < q->label()) {
            r = new_var(p->label(),
                        p->then_node(),
                        apply_union(p->else_node(), q));
        }
        else if (p->label() > q->label()) {
            r = new_var(q->label(),
                        q->then_node(),
                        apply_union(p, q->else_node()));
        }
        else {
            r = new_var(p->label(),
                        apply_union(p->then_node(), q->then_node()) ,
                        apply_union(p->else_node(), q->else_node()));
        }

        union_table[key] = r;
        return r;
    }

    /*!
     * @brief 積集合を返します
     */
    const node_ptr apply_intersection(const node_ptr& p, const node_ptr& q) {
        if (p == zero() || q == zero()) return zero();
        if (p == q) return p;
        const auto key = make_bin_op_key(p, q);
        const auto it = intersection_table.find(key);
        if (it != intersection_table.end() && !it->second.expired()) {
            return it->second.lock();
        }

        node_ptr r;
        if (p->label() < q->label()) {
            r = apply_intersection(p->else_node(), q);
        }
        else if (p->label() > q->label()) {
            r = apply_intersection(q->else_node(), p);
        }
        else {
            r = new_var(p->label(),
                        apply_intersection(p->then_node(), q->then_node()),
                        apply_intersection(p->else_node(), q->else_node()));
        }

        intersection_table[key] = r;
        return r;
    }

    const node_ptr apply_join(const node_ptr& p, const node_ptr& q) {
        if (p == zero() || q == zero()) return zero();
        if (p == one()) return q;
        if (q == one()) return p;

        auto m = std::minmax(p, q, [](const node_ptr& a, const node_ptr& b) {
            return a->label() < b->label();
        });
        const node_ptr& f = std::get<0>(m), g = std::get<1>(m);

        const auto key = make_bin_op_key(f, g);
        const auto it = join_table.find(key);
        if (it != join_table.end() && !it->second.expired()) {
            return it->second.lock();
        }

        const auto f1 = apply_onset(f, f->label());
        const auto f0 = apply_offset(f, f->label());
        node_ptr r;
        if (f->label() == g->label()) {
            const auto g1 = apply_onset(g, f->label());
            const auto g0 = apply_offset(g, f->label());
            r = apply_union(apply_change(
                    // (f1 * g1) + (f1 * g0) + (f0 * g1)
                    apply_union(apply_union(apply_join(f1, g1), apply_join(f1, g0)), apply_join(f0, g1)),
                f->label()), apply_join(f0, g0));
        }
        else {
            r = apply_union(apply_change(apply_join(f1, g), f->label()), apply_join(f0, g));
        }

        join_table[key] = r;
        return r;
    }

    const node_ptr apply_meet(const node_ptr& p, const node_ptr& q) {
        if (p == zero() || q == zero()) return zero();
        if (p == one() || q == one()) return one();

        auto m = std::minmax(p, q, [](const node_ptr& a, const node_ptr& b) {
            return (a->label() != b->label()) ?
                a->label() < b->label() :
                a->index() < b->index();
        });
        const node_ptr& f = std::get<0>(m), g = std::get<1>(m);

        const auto key = make_bin_op_key(f, g);
        const auto it = meet_table.find(key);
        if (it != meet_table.end() && !it->second.expired()) {
            return it->second.lock();
        }

        const auto f1 = apply_onset(f, f->label());
        const auto f0 = apply_offset(f, f->label());
        node_ptr r;
        if (f->label() == g->label()) {
            const auto g1 = apply_onset(g, f->label());
            const auto g0 = apply_offset(g, f->label());
            r = apply_union(apply_union(apply_union(
                    apply_change(apply_meet(f1, g1), f->label()),
                    apply_meet(f0, g0)),
                    apply_meet(f1, g0)),
                    apply_meet(f0, g1));
        }
        else {
            r = apply_union(apply_meet(f0, g), apply_meet(f1, g));
        }

        meet_table[key] = r;
        return r;
    }

};

}
