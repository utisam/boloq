#pragma once
#include <boloq/common.h>
#include <boloq/details/boolean_function_cache.h>
#include <boloq/details/boolean_function.h>

namespace boloq {

/*!
 * @brief 標準的なノードを用いるハッシュテーブル
 */
using boolean_function_cache = basic_boolean_function_cache<node>;

/*!
 * @brief 標準的なノードを用いる論理関数
 */
using boolean_function = basic_boolean_function<boolean_function_cache>;

}
