#pragma once

namespace boloq {

class __function_type_visitor {
protected:
    bool _result;

public:
    bool result() const {return _result;}
};


/*!
 * @brief f(x) = x かどうかを判定するためのvisitorです
 */
template<class T>
class is_wire_visitor : public __function_type_visitor {
    using node_ptr = typename T::node_ptr;
public:
    void operator()(const node_ptr& n) {
        _result = n->then_node()->index() == 1 && n->else_node()->index() == 0;
    }
};

/*!
 * @brief 論理否定を表すかどうかを判定するvisitorです
 */
template<class T>
class is_negation_visitor : public __function_type_visitor {
    using node_ptr = typename T::node_ptr;
public:
    void operator()(const node_ptr& n) {
        _result = n->then_node()->index() == 0 && n->else_node()->index() == 1;
    }
};

/*!
 * @brief 論理積を表すかどうかを判定するvisitorです
 */
template<class T>
class is_conjunction_visitor : public __function_type_visitor {
    using node_ptr = typename T::node_ptr;
public:
    void operator()(const node_ptr& n) {
        if (n->else_node()->index() != 0 || n->then_node()->index() == 0) {
            _result = false; return;
        }
        if (n->then_node()->index() == 1) {
            _result = true; return;
        }
        operator()(n->then_node());
    }
};

/*!
 * @brief 論理和を表すかどうかを判定するvisitorです
 */
template<class T>
class is_disjunction_visitor : public __function_type_visitor {
    using node_ptr = typename T::node_ptr;
public:
    void operator()(const node_ptr& n) {
        if (n->then_node()->index() != 1 || n->else_node()->index() == 1) {
            _result = false; return;
        }
        if (n->else_node()->index() == 0) {
            _result = true; return;
        }
        operator()(n->else_node());
    }
};

/*!
 * @brief 排他的論理和を表すかどうかを判定するvisitorです
 */
template<class T>
class is_exclusive_disjunction_visitor : public __function_type_visitor {
    using node_ptr = typename T::node_ptr;
public:
    void operator()(const node_ptr& n) {
        if (n->is_terminal()) {
            _result = false; return;
        }

        const auto& next_then = n->then_node();
        if (T(next_then) != ~T(n->else_node())) {
            _result = false; return;
        }
        else if (next_then->is_terminal()) {
            _result = true; return;
        }
        operator()(next_then);
    }
};

}
