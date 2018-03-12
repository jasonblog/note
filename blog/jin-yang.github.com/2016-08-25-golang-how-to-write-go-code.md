---
title: Golang 如何编码？
layout: post
comments: true
source: https://golang.org/doc/code.html
language: chinese
category: [program,golang,linux]
keywords: golang,go
description: 这篇文章实际上是 golang 官网 [How to Write Go Code](https://golang.org/doc/code.html) 的大致翻译，主要介绍了在 golang 环境中，如何进行编写代码，其中包括了搭建一个简单 go 包，以及如何使用 go tool 等等。
---

这篇文章实际上是 golang 官网 [How to Write Go Code](https://golang.org/doc/code.html) 的大致翻译，主要介绍了在 golang 环境中，如何进行编写代码，其中包括了搭建一个简单 go 包，以及如何使用 go tool 等等。

<!-- more -->

![a logo]({{ site.url }}/images/go/code-logo.jpg "a logo"){: .pull-center width="70%" }

## 简介

这篇文章中，将介绍如何构建一个简单的 go 包，以及如何使用 go tool，其中包含了典型的使用场景，也就是获取包、编译、安装 go 包以及命令。

使用 go tool 时，对于代码在那些目录下存放都有特定的要求，本文中，将会简单介绍。

## 代码组织

### 概述

* 使用 go 语言编程时，通常将所有的 go 代码保存在一个工作目录 (workspace) 中；
* 一个工作目录中包含了多个版本管理库 (repositories)，例如通过 git、svn 管理的；
* 每个版本库中可以包含一个或者多个包 (packages)；
* 每个包在同一个目录下包含了一个或者多个源码文件；
* 指向包的目录路径为通过 import 导入时的路径。

注意，很多语言都是每个工程 (project) 都有特定的工作目录，而工作目录中又通常是和版本管理库相关的。显然，go 语言与之是有明显区别的。

### Workspace

在工作目录下，包括了如下的三个子目录：

* *src* 包括了 go 的源码文件；
* *pkg* 包括了包对象；
* *bin* 包含了可执行二进制命令。

通过 go tool 可以将 src 目录下的源码编译安装到 pkg 以及 bin 目录下。

src 子目录下通常包含了多个版本控制库，如 git 和 mercurial 。如下是一个简单的示例：

{% highlight text %}
bin/
    hello                           # 二进制可执行文件
    outyet
pkg/
    linux_amd64/                    # 当前OS类型及其架构
        github.com/golang/example/
            stringutil.a            # 包对象
src/
    github.com/golang/example/
        .git/                       # git仓库元数据
        hello/
            hello.go                # 源码
        outyet/
            main.go                 # 源码
            main_test.go            # 测试源码
        stringutil/
            reverse.go
            reverse_test.go
    golang.org/x/image/
        .git/
        bmp/
            reader.go
            writer.go
    ...                             # 其它包仓库
{% endhighlight %}

如上的示例中，在工作目录中包含了两个版本库 example 以及 image 。其中 example 版本库中包含了两个命令 (hello 和 outyet) 以及一个库 (stringutl.a)。而 image 版本库中则包含了 bmp 包。

通常，每个工作目录下包含了多个源码库，而且每个源码库通常包含了多个包以及命令。而且，大多数 go 程序员会将所有的源码及其依赖保存在同一个工作目录下。

命令以及库是从不同类型的源码包中编译的，这个后面详述。

### GOPATH 环境变量

GOPATH 环境变量指定了工作目录的位置，大概这是通过 go 编码时需要唯一设置的环境变量。

开始，需要设置一个工作目录，并相应的设置 GOPATH 环境变量。目录可以在任何目录，在此以 $HOME/work 为例。注意，工作目录不一定与 go 安装目录相同。

{% highlight text %}
$ make $HOME/work
$ export GOPATH=$HOME/work
{% endhighlight %}

为了方便，通常要将工作目录下的 *bin* 子目录添加到 PATH 环境变量中。

{% highlight text %}
$ export PATH=$PATH:$GOPATH/bin
{% endhighlight %}

更多与 GOPATH 环境变量相关的内容可以查看 go help gopath 。


### 导入目录

导入目录 (*import path*) 字符串唯一定义了一个包 (package)。而导入目录对应了工作目录中的路径或者是远端的版本库 (后面详述) 。

标准库中的包，路径都比较短比如 "fmt" 和 "net/http" 。对于自己的库，必须要选择一个现在甚至将来，都不会与标准库或者三方库冲突的路径。

如果将代码保存在某个版本库中，那么就需要将版本库作为根路径。例如代码保存在 Github 中，相应的帐号为 github.com/user ，这将作为基目录 (base path)。

注意，在你编译之前，你不需要将你的代码推送到远端，通常这么部署是一个很好的习惯，方便后面发布到远端。实际上，你可以选择任意的目录，只要对标准库以及三方库唯一即可。

假设使用 github.com/user 作为基目录，可以通过如下的目录保存你的源码。

{% highlight text %}
$ mkdir -p $GOPATH/src/github.com/user
{% endhighlight %}


### 你的第一个程序

为了编译并运行一个简单程序，首选选择一个包路径 (假设使用 github.com/user/hello) 并在工作目录中创建相应的目录。

{% highlight text %}
$ mkdir -p $GOPATH/src/github.com/user/hello
{% endhighlight %}

结下来，在上面的目录下创建一个 hello.go 文件，包含如下的内容。

{% highlight go %}
package main
import "fmt"
func main() {
    fmt.Printf("Hello World. \n")
}
{% endhighlight %}

接下来，可以通过如下方式安装。

{% highlight text %}
$ go install github.com/user/hello
{% endhighlight %}

上述的命令可以在任何目录下执行，go tool 会通过 GOPATH 环境变量自动找到工作目录下的包。

当然，如果在相应的包目录下，可以直接忽略包的路径。

{% highlight text %}
$ cd $GOPATH/src/github.com/user/hello
$ go install
{% endhighlight %}

上述的命令将编译生成 hello 命令，然后将其添加到工作目录的 bin 子目录下，在我们的示例中就是 $GOPATH/bin/hello，也就是 $HOME/work/bin/hello 。

go tool 只有在有异常的时候才会输出相应的内容，如若没有任何内容输出，也就意味着已经执行成功。

然后，可以通过如下命令执行。

{% highlight text %}
$ $GOPATH/bin/hello
Hello World
{% endhighlight %}

如果已经将 $GOPATH/bin 目录添加到了 PATH 环境变量中，那可以直接输入二进制文件名即可。

{% highlight text %}
$ hello
Hello World
{% endhighlight %}

如果在使用版本控制系统，现在就可以初始化一个版本库，添加文件，然后提交第一次的修改。当然，这一步是可选的，编写 go 代码时是非必须的。

{% highlight text %}
$ cd $GOPATH/src/github.com/user/hello
$ git init
Initialized empty Git repository in /home/user/work/src/github.com/user/hello/.git/
$ git add hello.go
$ git commit -m "initial commit"
[master (root-commit) 0b4507d] initial commit
 1 file changed, 1 insertion(+_
   create mode 100644 hello.go
{% endhighlight %}

接下来也可以将其推送到远端的仓库。

### 第一个库

接下来，我们编写一个库，然后在 hello 程序中使用它。

同上面一样，首先选择一个包的路径 (在此使用 github.com/user/stringutil) 并创建之。

{% highlight text %}
$ mkdir -p $GOPATH/src/github.com/user/stringutil
{% endhighlight %}

接下来在上述的目录下新建一个 reverse.go 文件，内容如下。

{% highlight go %}
// Package stringutil contains utility functions for working with strings.
package stringutil

// Reverse returns its argument string reversed rune-wise left to right.
func Reverse(s string) string {
    r := []rune(s)
    for i, j := 0, len(r)-1; i < len(r)/2; i, j = i+1, j-1 {
        r[i], r[j] = r[j], r[i]
    }
    return string(r)
}
{% endhighlight %}

然后通过 go build 命令进行编译。

{% highlight text %}
$ go build github.com/user/stringutil
{% endhighlight %}

如果在包的源码目录下，可以直接执行如下命令。

{% highlight text %}
$ go build
{% endhighlight %}

上述的命令并不会生成文件。为了生成文件，必须要执行 go install 才可以，此时会将包对象保存在工作目录下的 pkg 目录下。

假设，已经生成了 stringutil 包，修改上面的 hello.go 文件。


{% highlight go %}
package main
import (
     "fmt"
     "github.com/user/stringutil"
     )
func main() {
    fmt.Printf(stringutil.Reverse("!oG ,olleH"))
}
{% endhighlight %}

无论 go tool 安装一个包还是命令，它将会自动安装其所依赖的包。所以，当你安装 hello 程序时。

{% highlight text %}
$ go install github.com/user/hello
{% endhighlight %}

其中的 stringutil 包会自动安装。

执行新的命令，你将会看到一个反向的信息。

{% highlight text %}
$ hello
Hello, Go!
{% endhighlight %}

执行完上述命令之后，其工作目录类似如下。

{% highlight text %}
bin/
    hello                           # 二进制可执行文件
pkg/
    linux_amd64/                    # 当前OS类型及其架构
        github.com/user/
            stringutil.a            # 包对象
src/
    github.com/user/
        .git/                       # git仓库元数据
        hello/
            hello.go                # 源码
        stringutil/
            reverse.go
{% endhighlight %}

注意，go install 命令会将 stringutil.a 存放到 pkg/linux_amd64 与源码目录相同的镜像路径中。在使用其它的 go tool 工具时，可以直接查看相应的目录，从而避免重复编译。其中 linux_amd64 用于交叉编译，包括了当前环境的 OS 以及架构信息。

go 语言采用静态编译，当执行 go 程序的时候，不再需要包对象。

### 包名称

在 go 源码中的第一条语句必须如下：

{% highlight go %}
package name
{% endhighlight %}

其中 *name* 是用于包导入时的默认名称，一个包中的文件必须要使用相同的 *name* 。

go 语言中的约定是，import 路径的最后一个名称作为包的名称，例如，假设包的 import 路径为 "crypto/rot13"，那么包名称为 rot13 。

可执行的命令必须使用 package.main 。

当然，没有必要要求所有的包名称唯一，只要保证其导入的路径唯一即可。


## 测试

go 包含了一个轻量级的测试框架，需要使用 go test 命令以及 testing 包。

其中文件名以 *\_test.go* 方式命名，其中包含了测试函数 TestXXX，函数格式为 func (t *testing. T)。测试框架将会执行每个函数，如果测试函数调用了 t.Error 或者 t.Fail，则认为测试用例失败。

对于上面的 stringutil 包创建一个文件 $GOPATH/src/github.com/user/stringutil/reverse_test.go，其内容如下。

{% highlight go %}
package stringutil
import "testing"
func TestReverse(t *testing. T) {
    cases := []struct {
        in, want string
    }{
        {"Hello, world", "dlrow ,olleH"},
        {"Hello, 世界", "界世 ,olleH"},
        {"", ""},
    }
    for _, c := range cases {
        got := Reverse(c.in)
        if got != c.want {
            t.Errorf("Reverse(%q) == %q, want %q", c.in, got, c.want)
        }
    }
{% endhighlight %}

然后，通过 go test 执行测试。

{% highlight text %}
$ go test github.com/user/stringutil
ok       github.com/user/stringutil 0.165s
{% endhighlight %}

与往常一样，如果在包路径下，可以直接忽略包路径，直接执行如下命令即可。

{% highlight text %}
$ cd $GOPATH/src/github.com/user/stringutil
$ go test
ok       github.com/user/stringutil 0.165s
{% endhighlight %}

可以通过 go help test 命令查看相关的帮助。

## 远程包

导入路径 (import path) 可以用于通过版本管理软件 (如 git 或者Mercurial) 获取源码包。而 go tool 可以通过该特性直接获取远端仓库。例如，上述的示例保存在 Github 中的 [github.com/golang/example](https://github.com/golang/example) 仓库中。如果在导入路径中包含了上述的仓库，则 go get 将会自动 fetch、build、install 。

{% highlight text %}
$ go get github.com/golang/example/hello
$ $GOPATH/bin/hello
Hello, Go examples!
{% endhighlight %}

如果包在工作路径中不存在，那么 go get 将会将其存放在 GOPATH 指定的第一个工作空间中。当已经存在时，go get 将忽略下载，而与 go install 操作相同。

执行了上述的 go get 命令之后，工作目录的内容如下。

{% highlight text %}
bin/
    hello                           # 二进制可执行文件
pkg/
    linux_amd64/                    # 当前OS类型及其架构
        github.com/golang/example/
            stringutil.a            # 包对象
        github.com/user/
            stringutil.a            # 包对象
src/
    github.com/golang/example/
        .git/                       # git仓库元数据
        hello/
            hello.go                # 源码
        stringutil/
            reverse.go
            reverse_test.go
    github.com/user/
        hello/
            hello.go                # 源码
        stringutil/
            reverse.go
            reverse_test.go
{% endhighlight %}

其中保存在 Github 中的 hello 命令，依赖于相同目录下的 stringutil 包，所以，go get 命令会自动定位到相关的包，并下载。

{% highlight go %}
import "github.com/golang/example/stringutil"
{% endhighlight %}

这种方式可以很方便的提供给其他用户使用，相关帮助可以查看 go help importpath 。



{% highlight text %}
{% endhighlight %}
