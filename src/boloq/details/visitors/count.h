#pragma once

namespace boloq {

template<class T, class UIntT>
class count_visitor {
public:
    using result_type = UIntT;

private:
    /*! @brief このクラスが扱うノードの型 */
    using node_ptr = typename T::node_ptr;

    std::unordered_map<node_ptr, result_type> count_cache;
public:

    result_type operator()(const node_ptr& n) {
        if (n->is_terminal()) {
            if (n->index()) return result_type(1);
            else return result_type(0);
        }

        const auto it = count_cache.find(n);
        if (it != count_cache.end()) return it->second;

        result_type r = operator()(n->then_node()) + operator()(n->else_node());
        count_cache[n] = r;
        return r;
    }

};

}
