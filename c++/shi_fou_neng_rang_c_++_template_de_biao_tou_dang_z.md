# 是否能讓 C++ template 的標頭檔只含宣告不含實作?


過去一直覺得自訂 template 的時候, 將宣告和定義 (實作) 同時放在標頭檔裡最保險, 但不確定是否能將兩者拆開放到不同檔案。拆開的明顯好處是不會因為修改 template 的實作, 而需要重新編譯 include 此標頭檔的檔案。在經歷過改一行標頭檔要重新編譯十分鐘後, 我愈來愈在意這件事了。

在說明如何折開 template 的宣告和實作之前, 得先明白編譯使用到 template 程式碼過程發生了什麼事。實際上有兩個步驟需要留意:

讀入 template 宣告, 檢查 caller 是否有正確使用目標函式、類別。
instantiate (實例化) 特定參數的 template。
比方說定義 std::map<std::string, int> scores 的時候, 除了需要 map 的宣告瞭解 scores 是否有正確使用 map 的介面外, 還需要 map 的定義 (實作)才知道如何 instantiate std::map<std::string, int>。實例化時會檢查參數 std::string 和 int 是否支援 map 預期的介面。附帶一提, 《Effective C++》

`稱 template 為「隱式介面 + 編譯期多型` 而 `virtual 是「顯示介面 + 執行期多型」`, 很精闢的描述。

回到原本的議題, template 標頭檔是否能只含宣告? 可以, 只要之後有辦法 instantiate 用到的 template 即可。假設 <map> 裡只有 std::map 的宣告, 就需要在某個 cpp 檔裡面 include map 的定義, 然後明確地告訴 compiler 你要 instantiate std::map<std::string, int>。

以下以自訂函式說明:

- t.h

```cpp
#ifndef T_H
#define T_H

template <typename T>
void foo(T& t);

#endif //  T_H
```

- t.cpp


```cpp
#include "t.h"
#include <vector>

template <typename T>
void foo(T&t) { t[0] = 10; }

template void foo<std::vector<int> >(std::vector<int>& t); // *注意*
main.cpp

#include <iostream>
#include <vector>
#include "t.h"

int main(void) {
  std::vector<int> ns;
  ns.push_back(0);
  std::cout << ns[0] << std::endl;
  foo(ns);
  std::cout << ns[0] << std::endl;
  return 0;
}
```

編譯和執行:

```sh
$ g++ -c t.cpp
$ g++ -c main.cpp
$ g++ t.o main.o -o main
$ ./main
0
10
```
注意 t.cpp 的最後一行, 這行就是使用 explicit instantiation 的語法, 要求編譯器 instantiate 指定參數的 template。

反之, 若少了那一行:

```cpp
$ g++ -c t.cpp
$ g++ -c main.cpp
$ g++ t.o main.o -o main
main.o: In function `main':
main.cpp:(.text+0x63): undefined reference to `void foo<std::vector<int, std::allocator<int> > >(std::vector<int, std::allocator<int> >&)'
collect2: ld returned 1 exit status
```

編譯各別 cpp 檔時沒有問題, 但是 link object 檔的時候會回報找不到實例化的函式 foo(), 因為編譯 t.cpp 時, 編譯器不知道需要實例化帶有什麼參數的 foo, t.o 裡也就沒有 main.cpp 用到的 foo 了。

雖然看似 explicit instantiation 可以幫忙拆離 template 的宣告和定義, 《C++ Templates - The Complete Guide》 卻不建議這麼做, 原因是專案變大後很可能會漏掉需要 instantiate 的型別。

說了這麼多, 最後又說建議不使用。那 explicit instantiation 到底有什麼用? 參考 Minimizing Code Bloat: Redundant Template Instantiation 得知, 若有某個型別的 template 很常被使用 (如 std::string), 可以用它來減少編譯和連結的時間。作法是用 explicit template instantiation declarations 避免程式碼 instantiate template, 然後在某個地方自己明確地使用一次 explicit instantiation。

以下以 <string> 為例, 尋找用到此技巧的相關資訊。

/usr/include/c++/4.6/string.h 會 include <bits/stringfwd.h> 和 <bits/basic_string.tcc>。前者宣告 string 如下:

typedef basic_string<char>    string;   /// A string of @c char
後者讓 include 此標頭檔的檔案不會 instantiate string:

extern template class basic_string<char>;
最後, 在 /usr/lib/debug/usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.16 裡可以找到 string 的實體 (應該是啦, 我沒有很仔細地比對):



```sh
$ nm /usr/lib/debug/usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.16 | g _ZStrsIcSt11char_traitsIcESaIcEERSt13basic_istreamIT_T0_ES7_RSbIS4_S5_T1_E
000000000006b810 T _ZStrsIcSt11char_traitsIcESaIcEERSt13basic_istreamIT_T0_ES7_RSbIS4_S5_T1_E
$ c++filt _ZStrsIcSt11char_traitsIcESaIcEERSt13basic_istreamIT_T0_ES7_RSbIS4_S5_T1_E
std::basic_istream<char, std::char_traits<char> >& std::operator>><char, std::char_traits<char>, std::allocator<char> >(std::basic_istream<char, std::char_traits<char> >&, std::basic_string<char, std::char_traits<char>, std::allocator<char> >&)
```

或 gcc-4.6.3/libstdc++-v3/src/string-inst.cc 中找到這段:


```cpp
#include <string>

// Instantiation configuration.
#ifndef C
# define C char
#endif

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  typedef basic_string<C> S;

  template class basic_string<C>;
```
