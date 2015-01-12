#pragma once

namespace boloq {

/*!
 * @brief 二分決定図を操作する基本的なクラスです
 */
template<class T>
class basic_combination {
private:
    using table_type = T;
    using self_type = basic_combination<table_type>;
    friend std::hash<self_type>;

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

    explicit basic_combination(const node_ptr& r) :
            _root(r)
    {}

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

    basic_combination() : _root(nullptr) {}

    /*!
     * @brief コンストラクタ
     */
    explicit basic_combination(const label_type& _label) :
            _root(table().new_var(_label))
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
     * @brief v を含まない組合せを集めた部分集合を返す
     */
    self_type offset(const label_type& v) {
        return self_type(table().apply_offset(_root, v));
    }

    /*!
     * @brief v を含む組合せから v を取り除いた集合を返す
     */
    self_type onset(const label_type& v) {
        return self_type(table().apply_onset(_root, v));
    }

    /*!
     * @brief 特定のアイテムの存在を反転させます
     */
    self_type& change(const label_type& v) {
        _root = table().apply_change(_root, v);
        return *this;
    }

    /*!
     * @brief 特定のアイテムの存在を反転させた結果を返します
     */
    self_type changed(const label_type& v) {
        return self_type(table().apply_change(_root, v));
    }

    /*!
     * @brief 同じ組み合わせ集合を表現しているか比較します
     *
     * この判定はO(1)で行う事ができます。
     */
    bool operator==(const self_type& o) const {
        return _root->index() == o._root->index();
    }

    /*!
     * @brief 違う組み合わせ集合を表現しているか比較します
     *
     * この判定はO(1)で行う事ができます。
     */
    bool operator!=(const self_type& o) const {
        return _root->index() != o._root->index();
    }

    /*!
     * @brief union を行った結果を返します
     */
    self_type operator+(const self_type& o) const {
        return self_type(table().apply_union(_root, o._root));
    }

    /*!
     * @brief union を適用します
     */
    self_type& operator+=(const self_type& o) {
        _root = table().apply_union(_root, o.root);
        return *this;
    }

    /*!
     * @brief subtract を行った結果を返します
     */
    self_type operator-(const self_type& o) const {
        return self_type(table().apply_subtract(_root, o._root));
    }

    /*!
     * @brief subtract を適用します
     */
    self_type& operator-=(const self_type& o) {
        _root = table().apply_subtract(_root, o.root);
        return *this;
    }

    /*!
     * @brief intersection の結果を返します
     */
    self_type operator&(const self_type& o) const {
        return self_type(table().apply_intersection(_root, o._root));
    }

    /*!
     * @brief intersection を適用します
     */
    self_type& operator&=(const self_type& o) {
        _root = table().apply_intersection(_root, o._root);
        return *this;
    }

    /*!
     * @brief join を行った結果を返します
     */
    self_type operator*(const self_type& o) const {
        return self_type(table().apply_join(_root, o._root));
    }

    /*!
     * @brief join を適用します
     */
    self_type& operator*=(const self_type& o) {
        _root = table().apply_join(_root, o.root);
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
     * @brief 組み合わせ集合を評価します
     */
    template<class AssignT>
    bool contain(const AssignT& assign) const {
        return visit_result<contain_visitor<self_type, AssignT>, bool>(assign);
    }

};

}

namespace std {

template<class T>
struct hash<boloq::basic_combination<T>> {
    std::hash<typename boloq::basic_combination<T>::node_ptr> hash_fn;
    size_t operator()(const boloq::basic_combination<T>& bf) const {
        return hash_fn(bf._root);
    }
};

}
