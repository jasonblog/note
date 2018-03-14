---
title: Golang 如何編碼？
layout: post
comments: true
source: https://golang.org/doc/code.html
language: chinese
category: [program,golang,linux]
keywords: golang,go
description: 這篇文章實際上是 golang 官網 [How to Write Go Code](https://golang.org/doc/code.html) 的大致翻譯，主要介紹了在 golang 環境中，如何進行編寫代碼，其中包括了搭建一個簡單 go 包，以及如何使用 go tool 等等。
---

這篇文章實際上是 golang 官網 [How to Write Go Code](https://golang.org/doc/code.html) 的大致翻譯，主要介紹了在 golang 環境中，如何進行編寫代碼，其中包括了搭建一個簡單 go 包，以及如何使用 go tool 等等。

<!-- more -->

![a logo]({{ site.url }}/images/go/code-logo.jpg "a logo"){: .pull-center width="70%" }

## 簡介

這篇文章中，將介紹如何構建一個簡單的 go 包，以及如何使用 go tool，其中包含了典型的使用場景，也就是獲取包、編譯、安裝 go 包以及命令。

使用 go tool 時，對於代碼在那些目錄下存放都有特定的要求，本文中，將會簡單介紹。

## 代碼組織

### 概述

* 使用 go 語言編程時，通常將所有的 go 代碼保存在一個工作目錄 (workspace) 中；
* 一個工作目錄中包含了多個版本管理庫 (repositories)，例如通過 git、svn 管理的；
* 每個版本庫中可以包含一個或者多個包 (packages)；
* 每個包在同一個目錄下包含了一個或者多個源碼文件；
* 指向包的目錄路徑為通過 import 導入時的路徑。

注意，很多語言都是每個工程 (project) 都有特定的工作目錄，而工作目錄中又通常是和版本管理庫相關的。顯然，go 語言與之是有明顯區別的。

### Workspace

在工作目錄下，包括瞭如下的三個子目錄：

* *src* 包括了 go 的源碼文件；
* *pkg* 包括了包對象；
* *bin* 包含了可執行二進制命令。

通過 go tool 可以將 src 目錄下的源碼編譯安裝到 pkg 以及 bin 目錄下。

src 子目錄下通常包含了多個版本控制庫，如 git 和 mercurial 。如下是一個簡單的示例：

{% highlight text %}
bin/
    hello                           # 二進制可執行文件
    outyet
pkg/
    linux_amd64/                    # 當前OS類型及其架構
        github.com/golang/example/
            stringutil.a            # 包對象
src/
    github.com/golang/example/
        .git/                       # git倉庫元數據
        hello/
            hello.go                # 源碼
        outyet/
            main.go                 # 源碼
            main_test.go            # 測試源碼
        stringutil/
            reverse.go
            reverse_test.go
    golang.org/x/image/
        .git/
        bmp/
            reader.go
            writer.go
    ...                             # 其它包倉庫
{% endhighlight %}

如上的示例中，在工作目錄中包含了兩個版本庫 example 以及 image 。其中 example 版本庫中包含了兩個命令 (hello 和 outyet) 以及一個庫 (stringutl.a)。而 image 版本庫中則包含了 bmp 包。

通常，每個工作目錄下包含了多個源碼庫，而且每個源碼庫通常包含了多個包以及命令。而且，大多數 go 程序員會將所有的源碼及其依賴保存在同一個工作目錄下。

命令以及庫是從不同類型的源碼包中編譯的，這個後面詳述。

### GOPATH 環境變量

GOPATH 環境變量指定了工作目錄的位置，大概這是通過 go 編碼時需要唯一設置的環境變量。

開始，需要設置一個工作目錄，並相應的設置 GOPATH 環境變量。目錄可以在任何目錄，在此以 $HOME/work 為例。注意，工作目錄不一定與 go 安裝目錄相同。

{% highlight text %}
$ make $HOME/work
$ export GOPATH=$HOME/work
{% endhighlight %}

為了方便，通常要將工作目錄下的 *bin* 子目錄添加到 PATH 環境變量中。

{% highlight text %}
$ export PATH=$PATH:$GOPATH/bin
{% endhighlight %}

更多與 GOPATH 環境變量相關的內容可以查看 go help gopath 。


### 導入目錄

導入目錄 (*import path*) 字符串唯一定義了一個包 (package)。而導入目錄對應了工作目錄中的路徑或者是遠端的版本庫 (後面詳述) 。

標準庫中的包，路徑都比較短比如 "fmt" 和 "net/http" 。對於自己的庫，必須要選擇一個現在甚至將來，都不會與標準庫或者三方庫衝突的路徑。

如果將代碼保存在某個版本庫中，那麼就需要將版本庫作為根路徑。例如代碼保存在 Github 中，相應的帳號為 github.com/user ，這將作為基目錄 (base path)。

注意，在你編譯之前，你不需要將你的代碼推送到遠端，通常這麼部署是一個很好的習慣，方便後面發佈到遠端。實際上，你可以選擇任意的目錄，只要對標準庫以及三方庫唯一即可。

假設使用 github.com/user 作為基目錄，可以通過如下的目錄保存你的源碼。

{% highlight text %}
$ mkdir -p $GOPATH/src/github.com/user
{% endhighlight %}


### 你的第一個程序

為了編譯並運行一個簡單程序，首選選擇一個包路徑 (假設使用 github.com/user/hello) 並在工作目錄中創建相應的目錄。

{% highlight text %}
$ mkdir -p $GOPATH/src/github.com/user/hello
{% endhighlight %}

結下來，在上面的目錄下創建一個 hello.go 文件，包含如下的內容。

{% highlight go %}
package main
import "fmt"
func main() {
    fmt.Printf("Hello World. \n")
}
{% endhighlight %}

接下來，可以通過如下方式安裝。

{% highlight text %}
$ go install github.com/user/hello
{% endhighlight %}

上述的命令可以在任何目錄下執行，go tool 會通過 GOPATH 環境變量自動找到工作目錄下的包。

當然，如果在相應的包目錄下，可以直接忽略包的路徑。

{% highlight text %}
$ cd $GOPATH/src/github.com/user/hello
$ go install
{% endhighlight %}

上述的命令將編譯生成 hello 命令，然後將其添加到工作目錄的 bin 子目錄下，在我們的示例中就是 $GOPATH/bin/hello，也就是 $HOME/work/bin/hello 。

go tool 只有在有異常的時候才會輸出相應的內容，如若沒有任何內容輸出，也就意味著已經執行成功。

然後，可以通過如下命令執行。

{% highlight text %}
$ $GOPATH/bin/hello
Hello World
{% endhighlight %}

如果已經將 $GOPATH/bin 目錄添加到了 PATH 環境變量中，那可以直接輸入二進制文件名即可。

{% highlight text %}
$ hello
Hello World
{% endhighlight %}

如果在使用版本控制系統，現在就可以初始化一個版本庫，添加文件，然後提交第一次的修改。當然，這一步是可選的，編寫 go 代碼時是非必須的。

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

接下來也可以將其推送到遠端的倉庫。

### 第一個庫

接下來，我們編寫一個庫，然後在 hello 程序中使用它。

同上面一樣，首先選擇一個包的路徑 (在此使用 github.com/user/stringutil) 並創建之。

{% highlight text %}
$ mkdir -p $GOPATH/src/github.com/user/stringutil
{% endhighlight %}

接下來在上述的目錄下新建一個 reverse.go 文件，內容如下。

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

然後通過 go build 命令進行編譯。

{% highlight text %}
$ go build github.com/user/stringutil
{% endhighlight %}

如果在包的源碼目錄下，可以直接執行如下命令。

{% highlight text %}
$ go build
{% endhighlight %}

上述的命令並不會生成文件。為了生成文件，必須要執行 go install 才可以，此時會將包對象保存在工作目錄下的 pkg 目錄下。

假設，已經生成了 stringutil 包，修改上面的 hello.go 文件。


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

無論 go tool 安裝一個包還是命令，它將會自動安裝其所依賴的包。所以，當你安裝 hello 程序時。

{% highlight text %}
$ go install github.com/user/hello
{% endhighlight %}

其中的 stringutil 包會自動安裝。

執行新的命令，你將會看到一個反向的信息。

{% highlight text %}
$ hello
Hello, Go!
{% endhighlight %}

執行完上述命令之後，其工作目錄類似如下。

{% highlight text %}
bin/
    hello                           # 二進制可執行文件
pkg/
    linux_amd64/                    # 當前OS類型及其架構
        github.com/user/
            stringutil.a            # 包對象
src/
    github.com/user/
        .git/                       # git倉庫元數據
        hello/
            hello.go                # 源碼
        stringutil/
            reverse.go
{% endhighlight %}

注意，go install 命令會將 stringutil.a 存放到 pkg/linux_amd64 與源碼目錄相同的鏡像路徑中。在使用其它的 go tool 工具時，可以直接查看相應的目錄，從而避免重複編譯。其中 linux_amd64 用於交叉編譯，包括了當前環境的 OS 以及架構信息。

go 語言採用靜態編譯，當執行 go 程序的時候，不再需要包對象。

### 包名稱

在 go 源碼中的第一條語句必須如下：

{% highlight go %}
package name
{% endhighlight %}

其中 *name* 是用於包導入時的默認名稱，一個包中的文件必須要使用相同的 *name* 。

go 語言中的約定是，import 路徑的最後一個名稱作為包的名稱，例如，假設包的 import 路徑為 "crypto/rot13"，那麼包名稱為 rot13 。

可執行的命令必須使用 package.main 。

當然，沒有必要要求所有的包名稱唯一，只要保證其導入的路徑唯一即可。


## 測試

go 包含了一個輕量級的測試框架，需要使用 go test 命令以及 testing 包。

其中文件名以 *\_test.go* 方式命名，其中包含了測試函數 TestXXX，函數格式為 func (t *testing. T)。測試框架將會執行每個函數，如果測試函數調用了 t.Error 或者 t.Fail，則認為測試用例失敗。

對於上面的 stringutil 包創建一個文件 $GOPATH/src/github.com/user/stringutil/reverse_test.go，其內容如下。

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

然後，通過 go test 執行測試。

{% highlight text %}
$ go test github.com/user/stringutil
ok       github.com/user/stringutil 0.165s
{% endhighlight %}

與往常一樣，如果在包路徑下，可以直接忽略包路徑，直接執行如下命令即可。

{% highlight text %}
$ cd $GOPATH/src/github.com/user/stringutil
$ go test
ok       github.com/user/stringutil 0.165s
{% endhighlight %}

可以通過 go help test 命令查看相關的幫助。

## 遠程包

導入路徑 (import path) 可以用於通過版本管理軟件 (如 git 或者Mercurial) 獲取源碼包。而 go tool 可以通過該特性直接獲取遠端倉庫。例如，上述的示例保存在 Github 中的 [github.com/golang/example](https://github.com/golang/example) 倉庫中。如果在導入路徑中包含了上述的倉庫，則 go get 將會自動 fetch、build、install 。

{% highlight text %}
$ go get github.com/golang/example/hello
$ $GOPATH/bin/hello
Hello, Go examples!
{% endhighlight %}

如果包在工作路徑中不存在，那麼 go get 將會將其存放在 GOPATH 指定的第一個工作空間中。當已經存在時，go get 將忽略下載，而與 go install 操作相同。

執行了上述的 go get 命令之後，工作目錄的內容如下。

{% highlight text %}
bin/
    hello                           # 二進制可執行文件
pkg/
    linux_amd64/                    # 當前OS類型及其架構
        github.com/golang/example/
            stringutil.a            # 包對象
        github.com/user/
            stringutil.a            # 包對象
src/
    github.com/golang/example/
        .git/                       # git倉庫元數據
        hello/
            hello.go                # 源碼
        stringutil/
            reverse.go
            reverse_test.go
    github.com/user/
        hello/
            hello.go                # 源碼
        stringutil/
            reverse.go
            reverse_test.go
{% endhighlight %}

其中保存在 Github 中的 hello 命令，依賴於相同目錄下的 stringutil 包，所以，go get 命令會自動定位到相關的包，並下載。

{% highlight go %}
import "github.com/golang/example/stringutil"
{% endhighlight %}

這種方式可以很方便的提供給其他用戶使用，相關幫助可以查看 go help importpath 。



{% highlight text %}
{% endhighlight %}
