---
title: Google 测试框架
layout: post
comments: true
language: chinese
category: [linux,program]
keywords: hello world,示例,sample,markdown
description: 简单记录一下一些与 Markdown 相关的内容，包括了一些使用模版。
---

GTest 是很多开源工程的测试框架。




<!-- more -->

## 简介


### 安装

可以通过如下方式安装，包括 `yum` 以及源码安装。

#### yum

可以直接通过 `yum --enablerepo=epel install gtest-devel gmock-devel` 安装。

#### 源码安装

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

也可以直接参考其中的 `travis.sh` 文件。

## GTest

源码目录下包含了多个工程目录，主要是为了提供多平台的支持，如 `msvc` 文件夹是用在微软 Visual Studio 中，`xcode` 文件夹是用于 Mac Xcode，`codegrear` 文件夹是用于 Borland C++ Builder，在 Linux 环境中，需要使用 `make` 文件夹了。

在 `make` 目录下，直接执行 `make` 命令编译即可，同时会生成一个测试程序。为编译生成其他用例的可执行文件，可以参照 `make` 目录下的 `Makefile` 文件，或者执行以下步骤：

{% highlight text %}
----- 将gtest-main.a文件拷贝到samples目录下，执行以下命令
$ g++ -I ../include/ -c sample2_unittest.cc
$ g++ -I ../include/ sample2.o sample2_unittest.o gtest_main.a -lpthread -o test2
{% endhighlight %}



## 参考

代码可以从 [github gtest](https://github.com/google/googletest) 上下载，包括了 GTest 以及 GMock ，关于其文档可以参考 [Testing Framework](https://github.com/google/googletest/blob/master/googletest/docs/Primer.md) 以及 [Advance Guide](https://github.com/google/googletest/blob/master/googletest/docs/AdvancedGuide.md) 。

<!--
http://blog.csdn.net/breaksoftware/article/details/50917733
-->

{% highlight text %}
{% endhighlight %}
