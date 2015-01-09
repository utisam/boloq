#pragma once
// #include <ostream>

namespace boloq {

/*!
 * @brief 出力を行うためのvisitorです
 */
template <class T>
class io_visitor {
    using node_ptr = typename T::node_ptr;

    std::ostream& ost;
    unsigned int indent_level;

public:

    /*!
     * @brief コンストラクタ
     *
     * @param o 出力先のストリーム
     */
    io_visitor(std::ostream& o)
            : ost(o), indent_level(0)
    {}

    /*!
     * @brief ノードを表示して、子を再帰的に呼び出します
     */
    void operator()(const node_ptr& n) {
        for (unsigned int i = 0; i < indent_level; i++) {
            ost << '\t';
        }
        ost << n->label() << ' ' << n->index() << std::endl;

        if (!n->is_terminal()) {
            indent_level++;
            n->then_node()->accept(*this);
            n->else_node()->accept(*this);
            indent_level--;
        }
    }

};

}

