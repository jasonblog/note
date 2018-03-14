---
title: Google 測試框架
layout: post
comments: true
language: chinese
category: [linux,program]
keywords: hello world,示例,sample,markdown
description: 簡單記錄一下一些與 Markdown 相關的內容，包括了一些使用模版。
---

GTest 是很多開源工程的測試框架。




<!-- more -->

## 簡介


### 安裝

可以通過如下方式安裝，包括 `yum` 以及源碼安裝。

#### yum

可以直接通過 `yum --enablerepo=epel install gtest-devel gmock-devel` 安裝。

#### 源碼安裝

{% highlight text %}
$ tar -xf googletest-release-X.X.X.tar.gz
$ cd googletest-release-X.X.X && mkdir build
$ cmake -Dgtest_build_samples=ON \
      -Dgmock_build_samples=ON \
      -Dgtest_build_tests=ON \
      -Dgmock_build_tests=ON \
      ..
$ make
$ CTEST_OUTPUT_ON_FAILURE=1 make test
{% endhighlight %}

也可以直接參考其中的 `travis.sh` 文件。

## GTest

源碼目錄下包含了多個工程目錄，主要是為了提供多平臺的支持，如 `msvc` 文件夾是用在微軟 Visual Studio 中，`xcode` 文件夾是用於 Mac Xcode，`codegrear` 文件夾是用於 Borland C++ Builder，在 Linux 環境中，需要使用 `make` 文件夾了。

在 `make` 目錄下，直接執行 `make` 命令編譯即可，同時會生成一個測試程序。為編譯生成其他用例的可執行文件，可以參照 `make` 目錄下的 `Makefile` 文件，或者執行以下步驟：

{% highlight text %}
----- 將gtest-main.a文件拷貝到samples目錄下，執行以下命令
$ g++ -I ../include/ -c sample2_unittest.cc
$ g++ -I ../include/ sample2.o sample2_unittest.o gtest_main.a -lpthread -o test2
{% endhighlight %}



## 參考

代碼可以從 [github gtest](https://github.com/google/googletest) 上下載，包括了 GTest 以及 GMock ，關於其文檔可以參考 [Testing Framework](https://github.com/google/googletest/blob/master/googletest/docs/Primer.md) 以及 [Advance Guide](https://github.com/google/googletest/blob/master/googletest/docs/AdvancedGuide.md) 。

<!--
http://blog.csdn.net/breaksoftware/article/details/50917733
-->

{% highlight text %}
{% endhighlight %}
