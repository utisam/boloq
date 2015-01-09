/*! \mainpage libboloq
 * テンプレートメタプログラミングを用いたBDD/ZDDライブラリです。
 *
 * # 使い方
 *
 * ## sudo ./waf install_release
 *
 * README.md を参考にinstallしてください。
 * ヘッダのみで構成されています。
 *
 * ## サンプル
 *
 * ヘッダファイルはデフォルトで /usr/local/include にインストールされます。
 *
 * ~~~~~~~~~~~~~~~{.cpp}
 * #include <iostream>
 * #include <boloq.h>
 * #include <boloq/io.h>
 * using namespace std;
 * int main() {
 *     using namespace boloq;
 *     auto x = boolean_function('x');
 *     auto y = boolean_function('y');
 *     cout << ~(x & y) << endl;
 *     return 0;
 * }
 * ~~~~~~~~~~~~~~~
 *
 * 表示に用いる boloq/io.h を別途 include しなければならないことに注意してください。
 *
 * ## コンパイル
 *
 * ヘッダファイルへのパスが通っていればOKです。
 * リンクの必要はありません。
 *
 * # boolean_function/combination クラス
 *
 * * boolean_function: BDDを用いて表します。
 * * combination: ZDDを用いて表します。
 *
 * # このライブラリのメリット
 *
 * * 標準ライブラリを用いており、比較的シンプルです
 *   * 参照カウンタに std::shared_ptr を利用しています
 *   * ハッシュテーブルに std::unordered_map を利用しています
 * * カスタマイズ可能
 *   * テンプレートメタプログラミングを用いています
 *   * 場合によっては最小限のコードでカスタマイズできます
 */
#pragma once
#include <boloq/boolean_function.h>
#include <boloq/combination.h>

/*! \namespace boloq
 *
 * @brief boloq の名前空間
 */
