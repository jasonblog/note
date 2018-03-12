---
title: Golang 简介
layout: post
comments: true
language: chinese
category: [program,golang,linux]
keywords: golang,go,简介
description: golang 目前越来越火，包括了 Docker、InfluxDB、etcd 等等，越来越多的工程都在使用 golang 。虽然它的语法都比较 "奇葩"，一些高级语言支持的特性却都不支持，但其高并发、高效率等等，也使其热度不减。这篇文章主要简单介绍下 golang 环境的搭建。
---

golang 目前越来越火，包括了 Docker、InfluxDB、etcd 等等，越来越多的工程都在使用 golang 。虽然它的语法都比较 "奇葩"，一些高级语言支持的特性却都不支持，但其高并发、高效率等等，也使其热度不减。

这篇文章主要简单介绍下 golang 环境的搭建。

<!-- more -->

![golang logo]({{ site.url }}/images/go/golang-logo.png "golang logo"){: .pull-center width="60%" }

## 环境配置

golang 有多种安装方式，主要包括了三种最常见的安装方式：

* 第三方工具。也就是一些常见操作系统的软件管理工具，如 Ubuntu 的 apt-get、CentOS 的 yum、Mac 的 homebrew 等，还有就是类似 GVM 的工具。

* Go 标准包安装。官网也提供了已经编译好的二进制安装包，支持 Windows、Linux、Mac等系统，包括 X86、ARM 等平台。

* Go 源码安装。对于经常使用 *nix 类系统的用户，尤其对于开发者来说，这种方式比较熟悉，而且从源码安装可以自己进行定制。

其中，上面提到了一个 GVM 工具，如果想在同一个系统中安装多个版本，建议可以使用该工具。

### YUM 安装

在 CentOS-7 中可以通过 yum list all \| grep golang 查看、安装，该二进制包在 base 中。

{% highlight text %}
# yum install golang
{% endhighlight %}

### 标准包安装

可以从 [golang.org/dl](https://golang.org/dl) 上下载源码包，当然也可以从 [golangtc.com](http://golangtc.com/download) 上下载。其中包的名称类似于 go1.6.linux-amd64.tar.gz ，从中选择相应的平台，压缩包中是已经编译好的二进制包，可以直接下载解压即可。

### 源码安装

直接下载源码安装，之前的时候需要 gcc 编译，现在基本可以通过 go 搞定了。

#### I. 下载源码

go 的 [官方页面](http://www.golang.org/) 之前打不开，可以从 [Golang 中国](http://golangtc.com/) 上下载，不过最近发现好像是放开了。

#### II. 设置环境变量

通常可以在 ~/.bashrc 中设置如下的环境变量，然后通过 source ~/.bashrc 使其立即生效。

{% highlight text %}
export GOPATH=$HOME/Workspace/go             ← 可以指定多个通过:分隔
export GOROOT=${GOPATH}/goroot               ← 默认在/usr/local/go，否则需要指定
#export GOBIN=$GOROOT/bin                    ← 指定默认bin目录
#export GOARCH=386
#export GOOS=linux
export PATH=${PATH}:${GOBIN}
{% endhighlight %}

#### III. 编译源码

如果下载的是只是源码，可以通过 ./all.bash 进行编译，也可以下载包含可执行文件的 tarball 包。

{% highlight text %}
$ go env                                     ← 查看环境变量
$ go version                                 ← 查看版本
{% endhighlight %}

### Hello world

保存如下的内容为 hello.go 文件，然后通过 go run hello.go 执行，此时会输出 Hello World!。

{% highlight go %}
package main
import "fmt"
func main() {
    fmt.Printf("Hello World!\n")
}
{% endhighlight %}

当然可以用更简单的。

{% highlight go %}
package main
func main() {
    println("Hello World!")
}
{% endhighlight %}

另外，在源码目录下，可以直接通过 ```go build``` 进行编译，此时将编译该目录下所有的源码，并生成与 **目录名称** 相同的二进制文件。

当然，也可以在源码目录下执行 ```go install``` 进行安装；或者将上述代码放到 ```$GOPATH/src/examples/hello``` 目录下，然后执行 ```go install examples/hello``` 即可，此时就会编译并安装到 $GOPATH/bin 目录下。

<!--
瘦身<br>
go 采用静态编译，编译后的文件会很大，可以通过 go build -ldflags "-s -w" 这种方式编译。

首先我们看一下为什么会比其他语言大些：

Go 编译的可执行文件都包含了一个运行时(runtime)，和我们习惯的Java/.NET VM有些类似。

运行时负责内存分配（Stack Handing、GC Heap）、垃圾回收（Garbage Collection）、Goroutine调度（Schedule）、引用类型（slice、map、channel）管理，以及反射（Reflection）等工作。Go程序进程启动后会自动创建两个goroutine，分别用于执行main入口函数和GC Heap管理。

也正是因为编译文件中嵌入了运行时，使得其可执行文件相较其他语言更大一些。但Go的二进制可执行文件都是静态编译的，无需其他任何链接库等文件，更利于分发。

我们可以通过下面的方法将其变小点：

采用：go build -ldflags "-s -w" 这种方式编译。

解释一下参数的意思：

-ldflags： 表示将后面的参数传给连接器（5/6/8l）
-s：去掉符号信息
-w：去掉DWARF调试信息

注意：
-s 去掉符号表（这样panic时，stack trace就没有任何文件名/行号信息了，这等价于普通C/C+=程序被strip的效果）
-w 去掉DWARF调试信息，得到的程序就不能用gdb调试了
两个可以分开使用
实际项目中不建议做这些处理，没啥必要。

-s去掉符号表（然后panic时候的stack trace就没有任何文件名/行号信息了，这个等价于普通C/C++程序被strip的效果），
-w去掉DWARF调试信息，得到的程序就不能用gdb调试了。
-s和-w也可以分开使用，一般来说如果不打算用gdb调试，
-w基本没啥损失。-s的损失就有点大了。

</li></ol>
相关的文档，可以参考安装包的 doc 目录下的内容。
</p>

<br><br><h2>配置vim</h2>
通过 Vundle 安装 Plugin '<a href="https://github.com/fatih/vim-go">fatih/vim-go</a>'，此时会安装高亮显示，不过还需要通过 :GoInstallBinaries 命令安装相关的二进制文件，此时会安装在 $GOROOT/bin/linux_386 目录下。<br><br>

最早之前代码都是维护在 code.google 上的，现在大多在 github 上，可以直接查看 github 上的相关文档，对于安装失败的可以直接 google 之。<br><br>

直接通过 :GoInstallBinaries 安装时，由于 golang.org 被墙，执行会失败，可以通过 gopm.io 下载 golang.org/x/tools 然后放置到 $GOPATH/src/golang.org/x/tools 目录下，通过 go install golang.org/x/tools/cmd/goimports 安装。<br><br><br>

其中的使用说明可以参考相关的源码，如 gotags 参考 https://github.com/jstemmer/gotags 。



http://studygolang.com/articles/1785   vim配置
http://golanghome.com/post/550         Go编码规范指南
http://www.yankay.com/go-clear-concurreny/
-->


## 其它

### 环境变量

介绍下一些常见的环境变量使用方法，可以通过 go env 查看当前的环境变量。

#### GOROOT

指定 golang 的安装路径，如果通过 yum 安装，可以使用系统默认的值，此时直接通过 unset GOROOT 取消该环境变量即可。

#### GOBIN

install 编译存放路径，不允许设置多个路径。可以为空，此时可执行文件会保存到 $GOPATH/bin 目录下。

#### GOPATH

唯一一个 **必须设置** 的环境变量，GOPATH 的作用是告诉 Go 命令和其他相关工具，在那里去找到安装在你系统上的 Go 包。这是一个路径的列表，一个典型的 GOPATH 设置如下，类似 PATH 的设置，Windows 下用分号分割。

{% highlight text %}
export GOPATH=$HOME/go:$HOME/go/gopath
export PATH=$PATH:${GOPATH//://bin:}/bin
{% endhighlight %}

最后一条，在 Linux/Mac 中把每个 GOPATH下 的 bin 都加入到 PATH 中。

每一个列表中的路径是一个工作区的位置，每个工作区都有源文件、相关包的对象、执行文件。GOPATH 必须设置编译和安装包，即使用标准的 Go 目录树，类似如下：

{% highlight text %}
|-- bin/
|   `-- foobar               (installed command)
|-- pkg/
|   `-- linux_amd64/
|       `-- foo/
|           `-- bar.a        (installed package object)
`-- src/
    `-- foo/
        |-- bar/             (go code in package bar)
        |   `-- bar.go
        `-- foobar/          (go code in package main)
            `-- foobar.go
{% endhighlight %}

#### 其它

需要注意的是，GROOT 和 GOPATH **可以设置为不同的路径** 。


### 常用命令

简单列举一下一些常用的工具。

{% highlight text %}
$ go version            # 当前的版本
$ go env                # 环境变量
$ go list               # 列出全部已安装的package
$ go run                # 编译并运行Go程序
$ go help               # 查看帮助
$ go install            # 安装，需要保证源码已经下载
{% endhighlight %}

<!--
go fix 用来修复以前老版本的代码到新版本，例如go1之前老版本的代码转化到go1
-->

#### go build

用于编译，在包的编译过程中，若有必要，会同时编译与之相关联的包。

<!--
    如果是普通包，当你执行go build命令后，不会产生任何文件。

    如果是main包，当只执行go build命令后，会在当前目录下生成一个可执行文件。如果需要在$GOPATH/bin木下生成相应的exe文件，需要执行go install 或者使用 go build -o 路径/a.exe。

    如果某个文件夹下有多个文件，而你只想编译其中某一个文件，可以在 go build 之后加上文件名，例如 go build a.go；go build 命令默认会编译当前目录下的所有go文件。

    你也可以指定编译输出的文件名。比如，我们可以指定go build -o myapp.exe，默认情况是你的package名(非main包)，或者是第一个源文件的文件名(main包)。

    go build 会忽略目录下以”_”或者”.”开头的go文件。

    如果你的源代码针对不同的操作系统需要不同的处理，那么你可以根据不同的操作系统后缀来命名文件。例如有一个读取数组的程序，它对于不同的操作系统可能有如下几个源文件：

array_linux.go
array_darwin.go
array_windows.go
array_freebsd.go

go build的时候会选择性地编译以系统名结尾的文件（Linux、Darwin、Windows、Freebsd）。例如Linux系统下面编译只会选择array_linux.go文件，其它系统命名后缀文件全部忽略。
-->

#### go clean

用来移除当前源码包里面编译生成的文件。

{% highlight text %}
{% endhighlight %}

<!--
，这些文件包括：
    _obj/ 旧的object目录，由Makefiles遗留
    _test/ 旧的test目录，由Makefiles遗留
    _testmain.go 旧的gotest文件，由Makefiles遗留
    test.out 旧的test记录，由Makefiles遗留
    build.out 旧的test记录，由Makefiles遗留
    *.[568ao] object文件，由Makefiles遗留
    DIR(.exe) 由 go build 产生
    DIR.test(.exe) 由 go test -c 产生
    MAINFILE(.exe) 由 go build MAINFILE.go产生
-->

#### go fmt

主要是用来帮你格式化所写好的代码文件，只需要简单执行 fmt go test.go 命令，就可以让 go 帮我们格式化我们的代码文件。

其中通过 -w 可以将格式化结果保存到文件中，当然可以使用目录作为参数，格式化整个工程。

#### go get

用来动态获取远程代码包的，目前支持的有 BitBucket、GitHub、Google Code 和 Launchpad。

这个命令在内部实际上分成了两步操作：1) 下载源码包；2) 执行 go install。下载源码包的 go 工具会自动根据不同的域名调用不同的源码工具，当然必须确保安装了合适的源码管理工具。

另外，其实 go get 支持自定义域名的功能，具体参见 go help remote 。

#### go install

该命令在内部实际上分成了两步操作：1) 生成目标文件，可执行文件或者 .a 包；2) 把编译好的结果移到 $GOPATH/pkg 或者 $GOPATH/bin 目录下。

#### go test

该命令会自动读取源码目录下面名为 *_test.go 的文件，生成并运行测试用的可执行文件，输出的信息类似：

{% highlight text %}
ok   archive/tar   0.011s
FAIL archive/zip   0.022s
ok   compress/gzip 0.033s
{% endhighlight %}

默认的情况下，不需要任何的参数，它会自动把你源码包下面所有 test 文件测试完毕，当然你也可以带上参数，详情请参考 go help testflag 。

#### go doc

go doc 命令其实就是一个很强大的文档工具。

{% highlight text %}
$ go doc builtin                  # 查看builtin文档
$ go doc net/http                 # http包的报文帮助
$ go doc fmt Printf               # 查看包中的某个函数
$ go doc -http=:8080              # 浏览器打开127.1:8080
{% endhighlight %}

通过 -http 参数，将会看到一个 golang.org 的本地 copy 版本，通过它你可以查看 pkg 文档等其它内容。设置了GOPATH 后，不但会列出标准包的文档，还会列出你本地 GOPATH 中所有项目的相关文档，这对于经常被限制访问的用户来说是一个不错的选择。

<!--
$ go doc -src fmt Printf           # 查看相应的代码
-->


### 三方包

简单以安装 [GoLang Colorized Output](https://github.com/bclicn/color) 包为例。

{% highlight text %}
$ go get github.com/bclicn/color
{% endhighlight %}

当然，也可以手动下载源码，放到 $GOPATH/src/github.com/bclicn/color 目录下，然后直接执行 go install 命令。

如果没有下载源码，可能会导致如下的报错。

{% highlight text %}
$ go install color
can't load package: package color: cannot find package "color" in any of:
        $GOROOT/goroot/src/color (from $GOROOT)
        $GOPATH/src/color (from $GOPATH)
{% endhighlight %}

另外，[Github golang/tools](https://github.com/golang/tools) 中是一个 golang.org/x/tools 的镜像，可以直接将上述的代码仓库下载下来，然后放到 $GOPATH/golang.org/x/tools 目录下即可。


## 三方工具

简单介绍下一些常见的工具。

![golang logo]({{ site.url }}/images/go/golang-logo-2.png "golang logo"){: .pull-center width="50%" }

### GVM (Go Version Manager)

工程项目保存在 [Github moovweb/gvm](https://github.com/moovweb/gvm) 中，这是一套使用 Bash 编写的脚本工具，支持多个 go 编译器共存，方便进行切换。

首先，需要安装一些常见的工具，这个是在 scripts/gvm-check 脚本中检查。

{% highlight text %}
$ yum install git binutils bison gcc make curl
{% endhighlight %}

如果感兴趣，可以直接查看其源码，其实很简单，只是设置好环境变量即可，接下来看看如何安装，并使用。

{% highlight text %}
----- 下载安装脚本并直接执行
$ bash < <(curl -s -S -L https://raw.githubusercontent.com/moovweb/gvm/master/binscripts/gvm-installer)
{% endhighlight %}

默认会直接将上述的代码仓库 Clone 到 ~/.gvm 目录下，并在 ~/.bashrc 中添加一行脚本执行命令，用来设置环境变量。

#### 安装

其中，每个子命令执行的脚本，实际是对应到 script 目录下相应名称的脚本；另外，建议可以直接下载该工具，然后下载相应的 go 版本。

{% highlight text %}
$ gvm install go1.4             ← 安装相应的版本
$ gvm use go1.4 --default       ← 设置使用相应的版本，可同时设置为默认
{% endhighlight %}

注意，go-1.5 之后的版本，将 C 编译器使用 go 编译器直接替换掉了，这也就意味着，如果你要安装 1.5 之后的版本，需要先安装 1.4 版的才可以。

这也就是说，需要如下两步：1) 用 gcc 编译生成 go-1.4；2) 用 go-1.4 编译大于 go-1.5 的版本。

当然，上述的方式是通过源码安装的，假设，你已经通过 yum 安装好了 go ，那么可以通过如下方式进行编译。实际上就是将 GOROOT_BOOTSTRAP 环境变量设置为安装目录，如下所示。

{% highlight text %}
$ export GOROOT_BOOTSTRAP=/usr/lib/golang
{% endhighlight %}

实际上，安装完之后，相关的数据会保存在如下的目录中 (假设版本为go1.5) 。

{% highlight text %}
~/.gvm/gos/go1.5
~/.gvm/pkgsets/go1.5
~/.gvm/environments/go1.5
{% endhighlight %}

简单来说，**需要设置环境变量，会保存在 ~/.gvm/environments/ 中**，可以用来设置相应的变量；例如，你可以将 GOPATH 设置为自己想放置的目录。

#### 常用命令

如下统计常用的命令。

{% highlight text %}
$ gvm list                      ← 查看当前所有版本
$ gvm install go1.4             ← 安装
$ gvm use go1.4 --default       ← 设置当前使用版本
{% endhighlight %}

### GDM (Go Dependency Manager)

源码可以从 [Github sparrc/gdm](https://github.com/sparrc/gdm) 上下载，其工作原理很简单，可以通过 save 命令会生成 Godeps 文件，保存了相关的包及其版本。

在新的环境中，通过 restore 命令可以下载代码包。

## 参考

官方网站 golang.org 被墙，常见的工具可以从 [Gopm Registry](https://gopm.io/) 上下载，而文档资料等可以从 [Golang 中国](http://golangtc.com/) 上查看。

另外，[Github - Golang](https://github.com/golang) 提供了很多 golang.org/x/ 的镜像包，只需要下载并保存到 $GOPATH/src 目录下。

### 文档

* [Network programming with Go](https://jan.newmarch.name/go/)，关于网络编程相关。
* [Go Resources](http://www.golang-book.com/)，有两本不错的书 An Introduction to Programming in Go、Introducing Go。
* [Go Bootcamp](http://www.golangbootcamp.com/)、[The Little Go Book](http://openmymind.net/The-Little-Go-Book/)、[Learning Go](https://www.miek.nl/go/)、[Building Web Apps With Go](https://www.gitbook.com/book/codegangsta/building-web-apps-with-go/details) 。

{% highlight text %}
{% endhighlight %}
