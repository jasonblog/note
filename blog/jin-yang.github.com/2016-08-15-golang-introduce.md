---
title: Golang 簡介
layout: post
comments: true
language: chinese
category: [program,golang,linux]
keywords: golang,go,簡介
description: golang 目前越來越火，包括了 Docker、InfluxDB、etcd 等等，越來越多的工程都在使用 golang 。雖然它的語法都比較 "奇葩"，一些高級語言支持的特性卻都不支持，但其高併發、高效率等等，也使其熱度不減。這篇文章主要簡單介紹下 golang 環境的搭建。
---

golang 目前越來越火，包括了 Docker、InfluxDB、etcd 等等，越來越多的工程都在使用 golang 。雖然它的語法都比較 "奇葩"，一些高級語言支持的特性卻都不支持，但其高併發、高效率等等，也使其熱度不減。

這篇文章主要簡單介紹下 golang 環境的搭建。

<!-- more -->

![golang logo]({{ site.url }}/images/go/golang-logo.png "golang logo"){: .pull-center width="60%" }

## 環境配置

golang 有多種安裝方式，主要包括了三種最常見的安裝方式：

* 第三方工具。也就是一些常見操作系統的軟件管理工具，如 Ubuntu 的 apt-get、CentOS 的 yum、Mac 的 homebrew 等，還有就是類似 GVM 的工具。

* Go 標準包安裝。官網也提供了已經編譯好的二進制安裝包，支持 Windows、Linux、Mac等系統，包括 X86、ARM 等平臺。

* Go 源碼安裝。對於經常使用 *nix 類系統的用戶，尤其對於開發者來說，這種方式比較熟悉，而且從源碼安裝可以自己進行定製。

其中，上面提到了一個 GVM 工具，如果想在同一個系統中安裝多個版本，建議可以使用該工具。

### YUM 安裝

在 CentOS-7 中可以通過 yum list all \| grep golang 查看、安裝，該二進制包在 base 中。

{% highlight text %}
# yum install golang
{% endhighlight %}

### 標準包安裝

可以從 [golang.org/dl](https://golang.org/dl) 上下載源碼包，當然也可以從 [golangtc.com](http://golangtc.com/download) 上下載。其中包的名稱類似於 go1.6.linux-amd64.tar.gz ，從中選擇相應的平臺，壓縮包中是已經編譯好的二進制包，可以直接下載解壓即可。

### 源碼安裝

直接下載源碼安裝，之前的時候需要 gcc 編譯，現在基本可以通過 go 搞定了。

#### I. 下載源碼

go 的 [官方頁面](http://www.golang.org/) 之前打不開，可以從 [Golang 中國](http://golangtc.com/) 上下載，不過最近發現好像是放開了。

#### II. 設置環境變量

通常可以在 ~/.bashrc 中設置如下的環境變量，然後通過 source ~/.bashrc 使其立即生效。

{% highlight text %}
export GOPATH=$HOME/Workspace/go             ← 可以指定多個通過:分隔
export GOROOT=${GOPATH}/goroot               ← 默認在/usr/local/go，否則需要指定
#export GOBIN=$GOROOT/bin                    ← 指定默認bin目錄
#export GOARCH=386
#export GOOS=linux
export PATH=${PATH}:${GOBIN}
{% endhighlight %}

#### III. 編譯源碼

如果下載的是隻是源碼，可以通過 ./all.bash 進行編譯，也可以下載包含可執行文件的 tarball 包。

{% highlight text %}
$ go env                                     ← 查看環境變量
$ go version                                 ← 查看版本
{% endhighlight %}

### Hello world

保存如下的內容為 hello.go 文件，然後通過 go run hello.go 執行，此時會輸出 Hello World!。

{% highlight go %}
package main
import "fmt"
func main() {
    fmt.Printf("Hello World!\n")
}
{% endhighlight %}

當然可以用更簡單的。

{% highlight go %}
package main
func main() {
    println("Hello World!")
}
{% endhighlight %}

另外，在源碼目錄下，可以直接通過 ```go build``` 進行編譯，此時將編譯該目錄下所有的源碼，並生成與 **目錄名稱** 相同的二進制文件。

當然，也可以在源碼目錄下執行 ```go install``` 進行安裝；或者將上述代碼放到 ```$GOPATH/src/examples/hello``` 目錄下，然後執行 ```go install examples/hello``` 即可，此時就會編譯並安裝到 $GOPATH/bin 目錄下。

<!--
瘦身<br>
go 採用靜態編譯，編譯後的文件會很大，可以通過 go build -ldflags "-s -w" 這種方式編譯。

首先我們看一下為什麼會比其他語言大些：

Go 編譯的可執行文件都包含了一個運行時(runtime)，和我們習慣的Java/.NET VM有些類似。

運行時負責內存分配（Stack Handing、GC Heap）、垃圾回收（Garbage Collection）、Goroutine調度（Schedule）、引用類型（slice、map、channel）管理，以及反射（Reflection）等工作。Go程序進程啟動後會自動創建兩個goroutine，分別用於執行main入口函數和GC Heap管理。

也正是因為編譯文件中嵌入了運行時，使得其可執行文件相較其他語言更大一些。但Go的二進制可執行文件都是靜態編譯的，無需其他任何鏈接庫等文件，更利於分發。

我們可以通過下面的方法將其變小點：

採用：go build -ldflags "-s -w" 這種方式編譯。

解釋一下參數的意思：

-ldflags： 表示將後面的參數傳給連接器（5/6/8l）
-s：去掉符號信息
-w：去掉DWARF調試信息

注意：
-s 去掉符號表（這樣panic時，stack trace就沒有任何文件名/行號信息了，這等價於普通C/C+=程序被strip的效果）
-w 去掉DWARF調試信息，得到的程序就不能用gdb調試了
兩個可以分開使用
實際項目中不建議做這些處理，沒啥必要。

-s去掉符號表（然後panic時候的stack trace就沒有任何文件名/行號信息了，這個等價於普通C/C++程序被strip的效果），
-w去掉DWARF調試信息，得到的程序就不能用gdb調試了。
-s和-w也可以分開使用，一般來說如果不打算用gdb調試，
-w基本沒啥損失。-s的損失就有點大了。

</li></ol>
相關的文檔，可以參考安裝包的 doc 目錄下的內容。
</p>

<br><br><h2>配置vim</h2>
通過 Vundle 安裝 Plugin '<a href="https://github.com/fatih/vim-go">fatih/vim-go</a>'，此時會安裝高亮顯示，不過還需要通過 :GoInstallBinaries 命令安裝相關的二進制文件，此時會安裝在 $GOROOT/bin/linux_386 目錄下。<br><br>

最早之前代碼都是維護在 code.google 上的，現在大多在 github 上，可以直接查看 github 上的相關文檔，對於安裝失敗的可以直接 google 之。<br><br>

直接通過 :GoInstallBinaries 安裝時，由於 golang.org 被牆，執行會失敗，可以通過 gopm.io 下載 golang.org/x/tools 然後放置到 $GOPATH/src/golang.org/x/tools 目錄下，通過 go install golang.org/x/tools/cmd/goimports 安裝。<br><br><br>

其中的使用說明可以參考相關的源碼，如 gotags 參考 https://github.com/jstemmer/gotags 。



http://studygolang.com/articles/1785   vim配置
http://golanghome.com/post/550         Go編碼規範指南
http://www.yankay.com/go-clear-concurreny/
-->


## 其它

### 環境變量

介紹下一些常見的環境變量使用方法，可以通過 go env 查看當前的環境變量。

#### GOROOT

指定 golang 的安裝路徑，如果通過 yum 安裝，可以使用系統默認的值，此時直接通過 unset GOROOT 取消該環境變量即可。

#### GOBIN

install 編譯存放路徑，不允許設置多個路徑。可以為空，此時可執行文件會保存到 $GOPATH/bin 目錄下。

#### GOPATH

唯一一個 **必須設置** 的環境變量，GOPATH 的作用是告訴 Go 命令和其他相關工具，在那裡去找到安裝在你係統上的 Go 包。這是一個路徑的列表，一個典型的 GOPATH 設置如下，類似 PATH 的設置，Windows 下用分號分割。

{% highlight text %}
export GOPATH=$HOME/go:$HOME/go/gopath
export PATH=$PATH:${GOPATH//://bin:}/bin
{% endhighlight %}

最後一條，在 Linux/Mac 中把每個 GOPATH下 的 bin 都加入到 PATH 中。

每一個列表中的路徑是一個工作區的位置，每個工作區都有源文件、相關包的對象、執行文件。GOPATH 必須設置編譯和安裝包，即使用標準的 Go 目錄樹，類似如下：

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

需要注意的是，GROOT 和 GOPATH **可以設置為不同的路徑** 。


### 常用命令

簡單列舉一下一些常用的工具。

{% highlight text %}
$ go version            # 當前的版本
$ go env                # 環境變量
$ go list               # 列出全部已安裝的package
$ go run                # 編譯並運行Go程序
$ go help               # 查看幫助
$ go install            # 安裝，需要保證源碼已經下載
{% endhighlight %}

<!--
go fix 用來修復以前老版本的代碼到新版本，例如go1之前老版本的代碼轉化到go1
-->

#### go build

用於編譯，在包的編譯過程中，若有必要，會同時編譯與之相關聯的包。

<!--
    如果是普通包，當你執行go build命令後，不會產生任何文件。

    如果是main包，當只執行go build命令後，會在當前目錄下生成一個可執行文件。如果需要在$GOPATH/bin木下生成相應的exe文件，需要執行go install 或者使用 go build -o 路徑/a.exe。

    如果某個文件夾下有多個文件，而你只想編譯其中某一個文件，可以在 go build 之後加上文件名，例如 go build a.go；go build 命令默認會編譯當前目錄下的所有go文件。

    你也可以指定編譯輸出的文件名。比如，我們可以指定go build -o myapp.exe，默認情況是你的package名(非main包)，或者是第一個源文件的文件名(main包)。

    go build 會忽略目錄下以”_”或者”.”開頭的go文件。

    如果你的源代碼針對不同的操作系統需要不同的處理，那麼你可以根據不同的操作系統後綴來命名文件。例如有一個讀取數組的程序，它對於不同的操作系統可能有如下幾個源文件：

array_linux.go
array_darwin.go
array_windows.go
array_freebsd.go

go build的時候會選擇性地編譯以系統名結尾的文件（Linux、Darwin、Windows、Freebsd）。例如Linux系統下面編譯只會選擇array_linux.go文件，其它系統命名後綴文件全部忽略。
-->

#### go clean

用來移除當前源碼包裡面編譯生成的文件。

{% highlight text %}
{% endhighlight %}

<!--
，這些文件包括：
    _obj/ 舊的object目錄，由Makefiles遺留
    _test/ 舊的test目錄，由Makefiles遺留
    _testmain.go 舊的gotest文件，由Makefiles遺留
    test.out 舊的test記錄，由Makefiles遺留
    build.out 舊的test記錄，由Makefiles遺留
    *.[568ao] object文件，由Makefiles遺留
    DIR(.exe) 由 go build 產生
    DIR.test(.exe) 由 go test -c 產生
    MAINFILE(.exe) 由 go build MAINFILE.go產生
-->

#### go fmt

主要是用來幫你格式化所寫好的代碼文件，只需要簡單執行 fmt go test.go 命令，就可以讓 go 幫我們格式化我們的代碼文件。

其中通過 -w 可以將格式化結果保存到文件中，當然可以使用目錄作為參數，格式化整個工程。

#### go get

用來動態獲取遠程代碼包的，目前支持的有 BitBucket、GitHub、Google Code 和 Launchpad。

這個命令在內部實際上分成了兩步操作：1) 下載源碼包；2) 執行 go install。下載源碼包的 go 工具會自動根據不同的域名調用不同的源碼工具，當然必須確保安裝了合適的源碼管理工具。

另外，其實 go get 支持自定義域名的功能，具體參見 go help remote 。

#### go install

該命令在內部實際上分成了兩步操作：1) 生成目標文件，可執行文件或者 .a 包；2) 把編譯好的結果移到 $GOPATH/pkg 或者 $GOPATH/bin 目錄下。

#### go test

該命令會自動讀取源碼目錄下面名為 *_test.go 的文件，生成並運行測試用的可執行文件，輸出的信息類似：

{% highlight text %}
ok   archive/tar   0.011s
FAIL archive/zip   0.022s
ok   compress/gzip 0.033s
{% endhighlight %}

默認的情況下，不需要任何的參數，它會自動把你源碼包下面所有 test 文件測試完畢，當然你也可以帶上參數，詳情請參考 go help testflag 。

#### go doc

go doc 命令其實就是一個很強大的文檔工具。

{% highlight text %}
$ go doc builtin                  # 查看builtin文檔
$ go doc net/http                 # http包的報文幫助
$ go doc fmt Printf               # 查看包中的某個函數
$ go doc -http=:8080              # 瀏覽器打開127.1:8080
{% endhighlight %}

通過 -http 參數，將會看到一個 golang.org 的本地 copy 版本，通過它你可以查看 pkg 文檔等其它內容。設置了GOPATH 後，不但會列出標準包的文檔，還會列出你本地 GOPATH 中所有項目的相關文檔，這對於經常被限制訪問的用戶來說是一個不錯的選擇。

<!--
$ go doc -src fmt Printf           # 查看相應的代碼
-->


### 三方包

簡單以安裝 [GoLang Colorized Output](https://github.com/bclicn/color) 包為例。

{% highlight text %}
$ go get github.com/bclicn/color
{% endhighlight %}

當然，也可以手動下載源碼，放到 $GOPATH/src/github.com/bclicn/color 目錄下，然後直接執行 go install 命令。

如果沒有下載源碼，可能會導致如下的報錯。

{% highlight text %}
$ go install color
can't load package: package color: cannot find package "color" in any of:
        $GOROOT/goroot/src/color (from $GOROOT)
        $GOPATH/src/color (from $GOPATH)
{% endhighlight %}

另外，[Github golang/tools](https://github.com/golang/tools) 中是一個 golang.org/x/tools 的鏡像，可以直接將上述的代碼倉庫下載下來，然後放到 $GOPATH/golang.org/x/tools 目錄下即可。


## 三方工具

簡單介紹下一些常見的工具。

![golang logo]({{ site.url }}/images/go/golang-logo-2.png "golang logo"){: .pull-center width="50%" }

### GVM (Go Version Manager)

工程項目保存在 [Github moovweb/gvm](https://github.com/moovweb/gvm) 中，這是一套使用 Bash 編寫的腳本工具，支持多個 go 編譯器共存，方便進行切換。

首先，需要安裝一些常見的工具，這個是在 scripts/gvm-check 腳本中檢查。

{% highlight text %}
$ yum install git binutils bison gcc make curl
{% endhighlight %}

如果感興趣，可以直接查看其源碼，其實很簡單，只是設置好環境變量即可，接下來看看如何安裝，並使用。

{% highlight text %}
----- 下載安裝腳本並直接執行
$ bash < <(curl -s -S -L https://raw.githubusercontent.com/moovweb/gvm/master/binscripts/gvm-installer)
{% endhighlight %}

默認會直接將上述的代碼倉庫 Clone 到 ~/.gvm 目錄下，並在 ~/.bashrc 中添加一行腳本執行命令，用來設置環境變量。

#### 安裝

其中，每個子命令執行的腳本，實際是對應到 script 目錄下相應名稱的腳本；另外，建議可以直接下載該工具，然後下載相應的 go 版本。

{% highlight text %}
$ gvm install go1.4             ← 安裝相應的版本
$ gvm use go1.4 --default       ← 設置使用相應的版本，可同時設置為默認
{% endhighlight %}

注意，go-1.5 之後的版本，將 C 編譯器使用 go 編譯器直接替換掉了，這也就意味著，如果你要安裝 1.5 之後的版本，需要先安裝 1.4 版的才可以。

這也就是說，需要如下兩步：1) 用 gcc 編譯生成 go-1.4；2) 用 go-1.4 編譯大於 go-1.5 的版本。

當然，上述的方式是通過源碼安裝的，假設，你已經通過 yum 安裝好了 go ，那麼可以通過如下方式進行編譯。實際上就是將 GOROOT_BOOTSTRAP 環境變量設置為安裝目錄，如下所示。

{% highlight text %}
$ export GOROOT_BOOTSTRAP=/usr/lib/golang
{% endhighlight %}

實際上，安裝完之後，相關的數據會保存在如下的目錄中 (假設版本為go1.5) 。

{% highlight text %}
~/.gvm/gos/go1.5
~/.gvm/pkgsets/go1.5
~/.gvm/environments/go1.5
{% endhighlight %}

簡單來說，**需要設置環境變量，會保存在 ~/.gvm/environments/ 中**，可以用來設置相應的變量；例如，你可以將 GOPATH 設置為自己想放置的目錄。

#### 常用命令

如下統計常用的命令。

{% highlight text %}
$ gvm list                      ← 查看當前所有版本
$ gvm install go1.4             ← 安裝
$ gvm use go1.4 --default       ← 設置當前使用版本
{% endhighlight %}

### GDM (Go Dependency Manager)

源碼可以從 [Github sparrc/gdm](https://github.com/sparrc/gdm) 上下載，其工作原理很簡單，可以通過 save 命令會生成 Godeps 文件，保存了相關的包及其版本。

在新的環境中，通過 restore 命令可以下載代碼包。

## 參考

官方網站 golang.org 被牆，常見的工具可以從 [Gopm Registry](https://gopm.io/) 上下載，而文檔資料等可以從 [Golang 中國](http://golangtc.com/) 上查看。

另外，[Github - Golang](https://github.com/golang) 提供了很多 golang.org/x/ 的鏡像包，只需要下載並保存到 $GOPATH/src 目錄下。

### 文檔

* [Network programming with Go](https://jan.newmarch.name/go/)，關於網絡編程相關。
* [Go Resources](http://www.golang-book.com/)，有兩本不錯的書 An Introduction to Programming in Go、Introducing Go。
* [Go Bootcamp](http://www.golangbootcamp.com/)、[The Little Go Book](http://openmymind.net/The-Little-Go-Book/)、[Learning Go](https://www.miek.nl/go/)、[Building Web Apps With Go](https://www.gitbook.com/book/codegangsta/building-web-apps-with-go/details) 。

{% highlight text %}
{% endhighlight %}
