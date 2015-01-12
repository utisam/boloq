#pragma once

namespace boloq {

template<class T>
class __function_type_visitor {
public:
    using result_type = bool;
};


/*!
 * @brief f(x) = x かどうかを判定するためのvisitorです
 */
template<class T>
class is_wire_visitor : public __function_type_visitor<T> {
    using node_ptr = typename T::node_ptr;
public:
    bool operator()(const node_ptr& n) const {
        return n->then_node()->index() == 1 && n->else_node()->index() == 0;
    }
};

/*!
 * @brief 論理否定を表すかどうかを判定するvisitorです
 */
template<class T>
class is_negation_visitor : public __function_type_visitor<T> {
    using node_ptr = typename T::node_ptr;
public:
    bool operator()(const node_ptr& n) const {
        return n->then_node()->index() == 0 && n->else_node()->index() == 1;
    }
};

/*!
 * @brief 論理積を表すかどうかを判定するvisitorです
 */
template<class T>
class is_conjunction_visitor : public __function_type_visitor<T> {
    using node_ptr = typename T::node_ptr;
public:
    bool operator()(const node_ptr& n) const {
        if (n->else_node()->index() != 0 || n->then_node()->index() == 0) {
            return false;
        }
        if (n->then_node()->index() == 1) {
            return true;
        }
        return operator()(n->then_node());
    }
};

/*!
 * @brief 論理和を表すかどうかを判定するvisitorです
 */
template<class T>
class is_disjunction_visitor : public __function_type_visitor<T> {
    using node_ptr = typename T::node_ptr;
public:
    bool operator()(const node_ptr& n) const {
        if (n->then_node()->index() != 1 || n->else_node()->index() == 1) {
            return false;
        }
        if (n->else_node()->index() == 0) {
            return true;
        }
        return operator()(n->else_node());
    }
};

/*!
 * @brief 排他的論理和を表すかどうかを判定するvisitorです
 */
template<class T>
class is_exclusive_disjunction_visitor : public __function_type_visitor<T> {
    using node_ptr = typename T::node_ptr;
public:
    bool operator()(const node_ptr& n) const {
        if (n->is_terminal()) {
            return false;
        }

        const auto& next_then = n->then_node();
        if (T(next_then) != ~T(n->else_node())) {
            return false;
        }
        else if (next_then->is_terminal()) {
            return true;
        }
        return operator()(next_then);
    }
};

}
