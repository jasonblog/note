---
title: Linux Chroot
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,chroot
description: chroot 也就是 change root directory，也就是改變程序執行時所參考的根目錄位置。在 Linux 系統中，系統默認的目錄結構都是以 "/"，即是以根 (root) 開始的，而在使用 chroot 之後，系統的目錄結構將以指定的位置作為 "/" 位置。應用場景比較多，如安全中的蜜罐、系統啟動時用的 initrd、嵌入式系統掛載的 NFS、Linux From Scratch 的編譯過程等等。
---

chroot 也就是 change root directory，也就是改變程序執行時所參考的根目錄位置。在 Linux 系統中，系統默認的目錄結構都是以 "/"，即是以根 (root) 開始的，而在使用 chroot 之後，系統的目錄結構將以指定的位置作為 "/" 位置。

應用場景比較多，如安全中的蜜罐、系統啟動時用的 initrd、嵌入式系統掛載的 NFS、Linux From Scratch 的編譯過程等等。

在此，簡單介紹下 chroot 。

<!-- more -->

![chroot logo]({{ site.url }}/images/linux/chroot-logo.png "chroot logo"){: .pull-center }

## 簡介

在經過 chroot 之後，系統讀取到的目錄和文件將不在是舊系統根下的而是新根下 (即被指定的新的位置) 的目錄結構和文件，因此它帶來的好處大致有以下的幾個:

* 在經過 chroot 之後，在新根下將訪問不到舊系統的根目錄結構和文件，限制了用戶的權力，這樣就增強了系統的安全性。

* chroot 後，是一個與原系統根下文件不相關的目錄結構。那麼，在這個新的環境中，可以用來測試軟件的靜態編譯以及一些與系統不相關的獨立開發，例如 LFS 。

* 切換系統的根位置最明顯的就是在系統啟動時，從初始 RAM 磁盤 (initrd) 切換系統的根位置並執行真正的系統目錄的 init；另外的就是急救系統。

在 Linux 中，實際上是有標準的目錄結構的，其中包括了目錄的命名、屬性、權限等，可以參考 [Filesystem Hierarchy Standard, FHS](http://www.pathname.com/fhs/)。FHS 希望可以讓使用者瞭解到已安裝軟件放置於那個目錄下，所以希望軟件開發商、操作系統製作者等都能夠遵循 FHS 的標準。

上面扯的有點多，實際上構建一個 chroot 環境只需要一些簡單的目錄以及配置文件等，接下來看看如何構建一個 chroot 環境。



## 搭建環境

注意，我們構建的 chroot 環境是基於當前架構的，比如現在使用的是 X86 機器，那麼搭建的運行環境就是基於 X86 機器的。如果想在 X86 上構建一個基於 NFS 的 ARM 測試環境，那麼就要用到了交叉編譯工具，重新編譯所有的二進制文件，目前用的較多的是 busybox；在此就不過多討論了。

當然，我們僅僅是搭建一個基本的運行環境，然後按需添加寫可執行文件。因為架構是一樣的，那麼我們就可以直接在本機上覆制所需要的文件，基本步驟大致如下：

1. 創建所必須的目錄，如 /lib、/lib64、/bin、/sbin、/etc 等。
2. 複製基本的可執行文件，如 bash、env、ln、cat、ls、mv、cp、rm、mkdir 等。
3. 複製上述執行文件依賴的庫文件，這一步可以自動執行。
4. 創建所必須的文件，以及環境變量，如 /dev/console、/etc/passwd、/etc/shadow 等。

上述的操作比較繁瑣，我們直接通過一個簡單的腳本 [chroot_create.sh](/reference/linux/chroot_create.sh) 完成上述的操作，需要注意的是，這個腳本只在 CentOS 上測試過，對於像 Debian、Gentoo 等其它的發佈版本，可能需要修改。

上述的腳本運行時需要用 root 權限，相關的操作可以直接查看 --help 。

{% highlight text %}
-h, --help
    直接查看幫助
-c, --create
    先搭建環境，然後執行chroot命令；默認操作是直接執行chroot
--root
    指定根目錄，默認是在當前目錄下創建一個 root 目錄
--verbose
    顯示操作的詳細信息，包括複製了那些文件等。
{% endhighlight %}

常見的命令如下：

{% highlight text %}
# chroot_create.sh                  # 在當前目錄下創建root，並切換到該目錄
# chroot_chreate.sh -h              # 查看幫助
# chroot_chreate.sh -v dir          # 指定root目錄，顯示詳細信息，下面相同
# chroot_chreate.sh --root dir
# chroot_chreate.sh --root=dir
# chroot_chreate.sh -d dir          # 直接調轉到相應的目錄
{% endhighlight %}




在腳本的開頭包括了一些用戶可以修改的配置，如通過 cmdlist 添加執行的命令、user_process() 函數會在創建 root 環境時調用、before_enter_jail() 會在執行 chroot 命令前執行。

chroot 時可以執行命令，在此如果不使用 /usr/bin/env -i 則會將當前的環境便令傳入 chroot 目錄中，為了提供統一的環境，建議使用上述的參數。


### 調試

通常在打算包含一些命令時，會發現報錯，那麼可以將 strace 命令添加進去，對命令進行調試。例如，通過如下命令查看 sudo 的運行情況，根據其訪問的文件來進行設置，可能需要複製或者創建某些文件。

{% highlight text %}
$ strace sudo
{% endhighlight %}

#### I have no name!

雖然添加了 /etc/passwd(644) /etc/shadow(644) 文件 (此時不需要 /etc/group) ，但是仍有上述的問題。通過 strace \`which whoami\` 可以發現，訪問了 /etc/nsswitch.conf 文件，因此需要添加該配置文件。

如果只需要解決上述的問題，根據配置文件中的設置，也就是隻設置了 file ，因此只需要添加 /lib/i386-linux-gnu/libnss_files.so.2 即可。

#### 添加 iptables

此時需要添加 /lib/xtables 目錄下的內容，也可以根據需要添加，如對於雙網卡的設置，只需要添加如下的動態鏈接庫即可 libipt_MASQUERADE.so 。

#### 動態鏈接庫

稍微再囉嗦下"動態鏈接庫"，通過 strace 可以發現，很多命令都會查找動態鏈接庫，通常會按照一定的目錄順序查找。為了加快查找速度可設置 LD_LIBRARY_PATH=/lib 或者使用 ldconfig 命令。

如果使用 ldconfig ，實際調用的是 ldconfig.real，在執行完 ldconfig 命令後，將會生成 /etc/ld.so.cache 文件。



## 源碼解析

chroot 在 coreutils 中實現，實際是通過 chdir+chroot+execvp 實現，代碼可以參考如下。

{% highlight c %}
int main (int argc, char **argv)
{
    ... ...

    if (chroot (argv[optind]) != 0)
        error (EXIT_CANCELED, errno, _("cannot change root directory to %s"), argv[optind]);

    if (chdir ("/"))
        error (EXIT_CANCELED, errno, _("cannot chdir to root directory"));
    ... ...

    execvp (argv[0], argv); /* Execute the given command.  */
    ... ...
}
{% endhighlight %}

chroot 在內核中的實現是 sys_chroot()，該函數實際上是設置當前進程的 current->fs->root 變量。之所以使用 chdir 是因為，很多系統在實現 chroot 時，並不會改變當前的路徑，因此通常需要先通過 chdir 更改路徑。

chroot 相關的內容也可以參考 [理解 chroot][understand-chroot]，在此就不過多介紹了。

### 風險

在使用 chroot 時，其中的一個用途是製作一個 Jail 做隔離，限制某些應用的權限，但是實際上如果使用不當，仍有可能突破該 Jail 。

突破的方法也很簡單，就是循環利用 chdir("..") 即可，估計內核中的代碼是沒有做根路經的檢查的。當然，解決辦法也很簡單，就是在 jail 中不要使用 root 用戶，那麼這樣也不會造成太大的危害。

本來是有篇文章介紹的，現在已經成為了死鏈。


## 其它

在 chroot 環境中，如果要執行 ps 命令將會失敗，需要掛載 proc 文件系統，不過會顯示所有的進程，包括原用戶的進程。

也就是說，採用 chroot 的方法只是對目錄做了一些簡單的隔離，而對於 CPU、磁盤、網絡等資源，實際上是沒有隔離的。目前比較流行的是採用類似 LXC 的解決方案，如 systemd ，可以參考 [systemd-nspawn 快速指南](https://linux.cn/article-4678-1.html) ，也就是 systemd 用以替換 chroot 的方案。

最後介紹一個比較奇葩的工具 supermin ，可以用來創建一個很小的內核鏡像，不過是使用 OCaml 語言寫的。



<!--
[chroot最佳實踐](/reference/linux/container/chroot_best_practice.doc) 關於 chroot 的安全設置介紹，包括如何 break jail，可以參考 [How to break out of a chroot() jail](http://www.bpfh.net/simes/computing/chroot-break.html) 或者 [本地文檔](/reference/linux/container/Breaking out of a chroot padded cell.mht) ，可將文件 [break_jail.c](/reference/linux/container/break_jail.c) 編譯運行。
-->


[understand-chroot]:            https://www.ibm.com/developerworks/cn/linux/l-cn-chroot/            "理解 chroot"
[break-the-jail]:               http://www.bpfh.net/simes/computing/chroot-break.html
