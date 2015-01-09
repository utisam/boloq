#pragma once

namespace boloq {

/*!
 * @brief 二分決定図を操作する基本的なクラスです
 */
template<class T>
class basic_boolean_function {
private:
    using table_type = T;
    using self_type = basic_boolean_function<table_type>;

public:
    /*!
     * @brief 実際に操作されるノードの型
     */
    using node_ptr = typename table_type::node_ptr;

    /*!
     * @brief ラベルの型を表します
     *
     * この型は内部で用いられるノードの型に依存します。
     */
    using label_type = typename table_type::node_type::label_type;

private:
    node_ptr _root;

    static table_type& table() {
        static table_type instance;
        return instance;
    }

    template<class V, typename R, typename ...Args>
    R visit_result(const Args& ...args) const {
        V v(args...);
        this->accept(v);
        return v.result();
    }

public:

    basic_boolean_function() : _root(nullptr) {}

    /*!
     * @brief コンストラクタ
     */
    explicit basic_boolean_function(const label_type& _label) :
            _root(table().new_var(_label))
    {}

    /*!
     * @brief visitor 内で生成するためのコンストラクタ
     */
    explicit basic_boolean_function(const node_ptr& r) :
            _root(r)
    {}

    /*!
     * @brief 0 定節点
     */
    static const self_type one() {
        return self_type(table().one());
    }

    /*!
     * @brief 1 定節点
     */
    static const self_type zero() {
        return self_type(table().zero());
    }

    /*!
     * @brief ITE関数を実行します
     */
    self_type ite(const self_type& then_node, const self_type& else_node) const {
        return self_type(table().ite(_root, then_node._root, else_node._root));
    }

    /*!
     * @brief 同じ論理関数を表現しているか比較します
     *
     * この判定はO(1)で行う事ができます。
     */
    bool operator==(const self_type& o) const {
        return _root->index() == o._root->index();
    }

    /*!
     * @brief 違う論理関数を表現しているか比較します
     *
     * この判定はO(1)で行う事ができます。
     */
    bool operator!=(const self_type& o) const {
        return _root->index() != o._root->index();
    }

    /*!
     * @brief not演算を行った結果を返します
     */
    self_type operator~() const {
        return self_type(table().apply_not(_root));
    }

    /*!
     * @brief and演算を行った結果を返します
     */
    self_type operator&(const self_type& o) const {
        return self_type(table().apply_and(_root, o._root));
    }

    /*!
     * @brief and演算を適用します
     */
    self_type& operator&=(const self_type& o) {
        _root = table().apply_and(_root, o._root);
        return *this;
    }

    /*!
     * or演算を行った結果を返します
     */
    self_type operator|(const self_type& o) const {
        return self_type(table().apply_or(_root, o._root));
    }

    /*!
     * @brief or演算を適用します
     */
    self_type& operator|=(const self_type& o) {
        _root = table().apply_or(_root, o._root);
        return *this;
    }


    /*!
     * xor演算を行った結果を返します
     */
    self_type operator^(const self_type& o) const {
        return self_type(table().apply_xor(_root, o._root));
    }

    /*!
     * @brief or演算を適用します
     */
    self_type& operator^=(const self_type& o) {
        _root = table().apply_xor(_root, o._root);
        return *this;
    }

    /*!
     * @brief visitorを受理します
     *
     * visitorオブジェクトは、node_ptrを引数にした関数インタフェースをもたなくてはいけません
     */
    template<class V>
    void accept(V& visitor) const {
        _root->accept(visitor);
    }

    /*!
     * @brief 論理関数を評価します
     */
    template<class AssignT>
    bool execute(const AssignT& assign) const {
        return visit_result<execute_visitor<self_type, AssignT>, bool>(assign);
    }

    /*!
     * @brief f(x) = x かどうかを判定します
     */
    bool is_wire() const {
        return visit_result<is_wire_visitor<self_type>, bool>();
    }

    /*!
     * @brief 論理否定を表すかどうかを判定します
     */
    bool is_negation() const {
        return visit_result<is_negation_visitor<self_type>, bool>();
    }

    /*!
     * @brief 論理積を表すかどうかを判定します
     */
    bool is_conjunction() const {
        return visit_result<is_conjunction_visitor<self_type>, bool>();
    }

    /*!
     * @brief 論理和を表すかどうかを判定します
     */
    bool is_disjunction() const {
        return visit_result<is_disjunction_visitor<self_type>, bool>();
    }

    /*!
     * @brief 排他的論理和を表すかどうかを判定します
     */
    bool is_exclusive_disjunction() const {
        return visit_result<is_exclusive_disjunction_visitor<self_type>, bool>();
    }

};

/*!
 * @brief 標準的なノードを用いる論理関数です
 */
using boolean_function = basic_boolean_function<boolean_function_cache>;

}

