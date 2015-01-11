#pragma once
#include <memory>
#include <tuple>
#include <unordered_map>
#include <boost/functional/hash.hpp>
#include <boloq/details/index_generator.h>
#include <boloq/details/node.h>
#include <boloq/details/tuple_hash.h>
#include <boloq/details/visitors/execute.h>
#include <boloq/details/visitors/function_types.h>

namespace boloq {

/*!
 * @brief 標準的なノードのクラス
 */
using node = basic_node<size_t, size_t>;

}
