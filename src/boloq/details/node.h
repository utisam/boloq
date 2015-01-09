#pragma once

namespace boloq {

/*!
 * @brief 基本的なノードのクラス
 */
template<class LT, class IT>
class basic_node : public std::enable_shared_from_this<const basic_node<LT, IT>> {
private:
    using self_type = basic_node<LT, IT>;

public:
    /*! @brief インデックスを表す型 */
    using index_type = IT;
    /*! @brief ラベルを表す型 */
    using label_type = LT;
    /*! このクラスのポインタを表す型 */
    using node_ptr = std::shared_ptr<const self_type>;

private:
    const index_type _index;
    const label_type _label;

    const node_ptr _then_node;
    const node_ptr _else_node;

    const node_ptr self() const {
        return this->shared_from_this();
    }

public:

    /*!
     * @brief 定節点を生成するコンストラクタ
     *
     * 定節点は必ず 0-節点 もしくは 1-節点 のどちらかです。
     * index には 0 もしくは 1 を指定してください。
     */
    explicit constexpr basic_node(const index_type& i) :
            _index(i), _label(std::numeric_limits<label_type>::max()),
            _then_node(nullptr), _else_node(nullptr)
    {}

    /*!
     * @brief コンストラクタ
     *
     * index には2以上を指定してください
     */
    constexpr basic_node(const index_type& i, const label_type& l, const node_ptr& _then, const node_ptr& _else) :
        _index(i), _label(l), _then_node(_then), _else_node(_else)
    {}

    /*!
     * @brief ノードの内容一意に定まる値を返します
     */
    constexpr const index_type& index() const {return _index;}

    /*!
     * @brief ノードのラベルを返します
     *
     * 一般的には、論理関数の変数名や組み合わせ集合のアイテム名を表します
     */
    constexpr const label_type& label() const {return _label;}

    /*!
     * @brief このノードが終端かどうかを表します
     */
    constexpr bool is_terminal() const {return index() < 2;};

    /*! 1枝側のノードを返します */
    const node_ptr then_node() const {
        if (is_terminal()) return self();
        return _then_node;
    }
    /*! 0枝側のノードを返します */
    const node_ptr else_node() const {
        if (is_terminal()) return self();
        return _else_node;
    }

    /*!
     * @brief visitorを受理します
     */
    template<class V>
    void accept(V& visitor) const {
        visitor(self());
    }
};

/*!
 * @brief 標準的なノードのクラス
 */
using node = basic_node<size_t, size_t>;

/*!
 * @brief 定節点用のdeleter
 */
struct null_deleter {
    /*! @brief 何もしません */
    void operator()(void const *) const {}
};

}
