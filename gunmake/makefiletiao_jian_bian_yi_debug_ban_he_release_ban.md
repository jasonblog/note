# Makefile條件編譯debug版和release版


一般，在開發測試階段用debug版本，而上線發佈用release版本。

使用Makefile定製編譯不同版本，避免修改程序和Makefile文件，將會十分方便。

讀了一些資料，找到一個解決方法，Makefile預定義宏與條件判斷，結合make預定義變量，進行條件編譯。

 

比如，有一個test.cpp，包含這段代碼

```c
#ifdef debug
//your code
#endif
```

你希望在debug版本要執行它，在release版本不執行。

我們可以寫這樣的一個Makefile：


```sh
ver = debug

ifeq ($(ver), debug)
ALL: test_d
CXXFLAGS = -c -g -Ddebug
else
ALL: test_r
CXXFLAGS = -c -O3 
endif

test_d: test.do
    g++ -o $@ $^

test_r: test.ro
    g++ -o $@ $^

%.do: %.cpp
    g++ $(CXXFLAGS) $< -o $@

%.ro: %.cpp
    g++ $(CXXFLAGS) $< -o $@
```

簡單說一下，Makefile根據ver的不同定義了不同的編譯選項CXXFLAGS與輸出程序ALL，

debug版本輸出程序是test_d，release版本輸出程序是test_r

debug版本編譯選項是"-c -g -Ddebug"，release版本編譯選項是"-c -O3"

debug版本object文件後綴是".do"，release版本object文件後綴是".ro"

debug版本編譯選項使用"-D"定義宏debug，使得your code能夠執行。

不同版本的編譯選項、object文件、輸出程序均不同，所以可以同時編譯兩個版本的程序，互不影響。

 

Makefile執行時，首先判斷ver變量，如果ver的值是debug，編譯debug版，否則編譯release版。當然，默認情況下是編譯debug版的。

如果想編譯release版，要怎麼做？

只要在執行make時，對ver變量賦值，使得ver的值不為debug，比如

```sh
# make ver=release
 ```
 
 