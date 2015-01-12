#pragma once
#include <algorithm>
#include <boloq/common.h>
#include <boloq/details/combination_cache.h>
#include <boloq/details/combination.h>
#include <boloq/details/visitors/count.h>

namespace boloq {

/*!
 * @brief 標準的なノードを用いるハッシュテーブルです
 */
using combination_cache = basic_combination_cache<node>;

/*!
 * @brief 標準的なノードを用いる組み合わせ集合です
 */
using combination = basic_combination<combination_cache>;

}
