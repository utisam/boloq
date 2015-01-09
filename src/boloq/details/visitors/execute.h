#pragma once
#include <queue>

namespace boloq {

/*!
 * @brief 論理関数を評価するためのvisitorです
 */
template<class T, class AssignT>
class execute_visitor {
private:
    using node_ptr = typename T::node_ptr;
    using assign_type = AssignT;

    const assign_type& _assign;
    bool _result;

public:
    /*!
     * @brief 割り当てを設定して生成します
     */
    explicit execute_visitor(const assign_type& a) : _assign(a) {}

    /*!
     * @brief ノードを評価して、子を再帰的に呼び出します
     */
    void operator()(const node_ptr& n) {
        if (n->is_terminal()) {
            _result = n->index();
            return;
        }
        const node_ptr& next = (_assign.at(n->label())) ?
            n->then_node() : n->else_node();
        next->accept(*this);
    }

    /*!
     * @brief 結果を取得します
     */
    bool result() const {return _result;}
};

/*!
 * @brief AssignT がポインタだった時用の特殊化
 */
template<class T, typename AssignT>
class execute_visitor<T, AssignT*> {
private:
    using node_ptr = typename T::node_ptr;
    using assign_type = AssignT;

    const assign_type* _assign;
    bool _result;

public:
    /*!
     * @brief 割り当てを設定して生成します
     */
    explicit execute_visitor(const assign_type* a) : _assign(a) {}

    /*!
     * @brief ノードを評価して、子を再帰的に呼び出します
     */
    void operator()(const node_ptr& n) {
        if (n->is_terminal()) {
            _result = n->index();
            return;
        }
        const node_ptr& next = (_assign[n->label()]) ?
            n->then_node() : n->else_node();
        next->accept(*this);
    }

    /*!
     * @brief 結果を取得します
     */
    bool result() const {return _result;}
};

/*!
 * @brief 組み合わせ集合を評価するためのvisitorです
 */
template<class T, class AssignT>
class contain_visitor {
private:
    using node_ptr = typename T::node_ptr;
    using assign_type = AssignT;
    using value_type = std::pair<typename T::label_type, bool>;

    std::priority_queue<value_type, std::vector<value_type>, std::greater<value_type>> _assign;
    bool _result;

public:
    /*!
     * @brief 割り当てを設定して生成します
     */
    explicit contain_visitor(const assign_type& a) : _assign(a.begin(), a.end()) {}

    /*!
     * @brief ノードを評価して、子を再帰的に呼び出します
     */
    void operator()(const node_ptr& n) {
        if (_assign.empty() && n->is_terminal()) {
            _result = n->index();
            return;
        }

        if (n->label() == _assign.top().first) {
            auto& next = (_assign.top().second) ? n->then_node() : n->else_node();
            _assign.pop();
            next->accept(*this);
        }
        else if (n->label() > _assign.top().first) {
            if (_assign.top().second) {
                _result = false;
                return;
            }
            _assign.pop();
            n->accept(*this);
        }
    }

    /*!
     * @brief 結果を取得します
     */
    bool result() const {return _result;}
};

}
