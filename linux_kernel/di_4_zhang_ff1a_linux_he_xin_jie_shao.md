#Chapter 4:淺談Linux 核心
# 背景知識
1991年 當Linus Torvalds 還是赫爾辛基大學的學生時，發表了最初的Linux,並在同年8月在 comp.os.unix上發了一篇公告
```
From: torvalds@klaava.Helsinki.FI (Linus Benedict Torvalds)
Newsgroups: comp.os.minix
Subject: What would you like to see most in minix?
Summary: small poll for my new operating system
Message-ID: <1991Aug25.205708.9541@klaava.Helsinki.FI>
Date: 25 Aug 91 20:57:08 GMT
Organization: University of Helsinki

Hello everybody out there using minix –

I’m doing a (free) operating system (just a hobby, won’t be big and
professional like gnu) for 386(486) AT clones. This has been brewing
since april, and is starting to get ready. I’d like any feedback on
things people like/dislike in minix, as my OS resembles it somewhat
(same physical layout of the file-system (due to practical reasons)
among other things).

I’ve currently ported bash(1.08) and gcc(1.40), and things seem to work.
This implies that I’ll get something practical within a few months, and
I’d like to know what features most people would want. Any suggestions
are welcome, but I won’t promise I’ll implement them 🙂

Linus (torvalds@kruuna.helsinki.fi)

PS. Yes – it’s free of any minix code, and it has a multi-threaded fs.
It is NOT protable (uses 386 task switching etc), and it probably never
will support anything other than AT-harddisks, as that’s all I have :-(.
```
出處:http://www.thelinuxdaily.com/2010/04/the-first-linux-announcement-from-linus-torvalds/


# Linux source code
Linux 的source code 的官方網站是https://www.kernel.org/ 可以用git或wget下載原始碼
現在就讓我們開始吧~
實驗環境:
```
ubuntuDistributor ID:	Ubuntu
Description:	Ubuntu 16.04.1 LTS
Release:	16.04
Codename:	xenial
kernel version:4.4.0-45-generic
```
## 下載source code 
首先先安裝必要的package
```
sudo apt-get install libncurses5-dev gcc make git exuberant-ctags bc libssl-dev
```
下載source code
The latest stable
```
git clone git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
```

下載好了之後我們先來看看 directory 裡有什麼東西吧

```
cd linux
ls
```
|文件|說明|
|------|----------|
|arch|包含各種處理器架構的程式碼|
|block|holds code for block-device drivers|
|certs||
|crypto|Linux kernel的加密API|
|Documentation|Linux kernel的說明文件|
|drivers|驅動程式|
|firmware||
|fs|filesystem code|
|include|標頭檔|
|init|核心初始化|
|ipc|Inter-Process Communication|
|kernel|Kernel level code 和system call code|
|lib|函式庫|
|mm|High level memory management code|
|net|The high-level networking code|
|scripts|建立kerenel 會用到的scripts|
|security|Code for different Linux security models |
|sound|音效卡|
|tools|kernel tools|
|usr||

# 編譯核心
安裝cross compile tool
```
$ sudo apt-get install binutils-arm-linux-gnueabi
```
## 設定.config
可以自己設定或選擇預設的設定
```
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- <defconfig>
```
>defconfig 的內容可以利用 make ARCH=arm help 查看

自己設定.config

```
make menuconfig
make xconfig # QTK圖形界面
make gconfig # GTK圖形界面
make defconfig # 使用預設的config
```
config 設定好了後就來compile linux kernel
## 編譯linux
```
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- V=1 zImage
```
>把V=1 打開是為了可以觀察詳細的指令輸出情形

然後稍待片刻:tea:
當所有中間檔、函式庫、都編譯建構好後，他們會被集合起來放進一隻ELF檔，叫做vmlinux。
vmlinux 是一個真正的kernel proper是可獨立運作且一體成形的完整ELF映像檔。

>待補:
>a. uImage: 給 uboot 用的 binary format
>b. dtbs: 編譯 device tree
>c. 針對make target解釋

參考資料:
[Overview of the Kernel Source](http://courses.linuxchix.org/kernel-hacking-2002/08-overview-kernel-source.html)


