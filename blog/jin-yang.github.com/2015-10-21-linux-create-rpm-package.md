---
title: RPM 包製作
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,rpm,製作
description: RPM Package Manager 簡稱 RPM，這一文件格式在歷史名稱上雖然打著 RedHat 標誌，但其原始設計理念是開放式的，現在包括 OpenLinux、SuSE、Turbo Linux 等多個 Linux 分發版本都有采用，可以算是公認的行業標準了。在此介紹下如何製作 RPM 包，尤其是如何寫 .spec 配置文件，以及常見的技巧。
---

RPM Package Manager 簡稱 RPM，這一文件格式在歷史名稱上雖然打著 RedHat 標誌，但其原始設計理念是開放式的，現在包括 OpenLinux、SuSE、Turbo Linux 等多個 Linux 分發版本都有采用，可以算是公認的行業標準了。

在此介紹下如何製作 RPM 包，尤其是如何寫 .spec 配置文件，以及常見的技巧。

<!-- more -->

## 簡介

若要構建一個標準的 RPM 包，需要創建 .spec 文件，其中包含軟件打包的全部信息。然後，使用 rpmbuild 命令，按照 spec 文件的配置，系統會按照步驟生成最終的 RPM 包。

另外，需要注意的是，在使用時，需要使用普通用戶，一定不要用 root 用戶。

結下來，看看如何編譯製作 RPM 包。

## 環境配置

接下來，看看如何配置一步步製作 RPM 包。

### 安裝工具

首先，在 CentOS 中配置環境。

{% highlight text %}
----- 安裝環境，需要rpm-build工具來打包，該包只依賴於gdb以及rpm版本
# yum install rpm-build -y

----- 新建所需目錄
$ mkdir -pv ~/rpm-maker/{BUILD,BUILDROOT,RPMS,SOURCES,SPECS,SRPMS}
{% endhighlight %}

### 配置環境

打包相關的配置保存在宏文件 (macrofiles) 中，默認使用 ```$HOME/rpmbuild``` 目錄，用戶配置文件保存在 ```$HOME/.rpmmacros```；而製作包時的操作實際都在 topdir 指定的目錄下，可以通過如下方式查看。

{% highlight text %}
--- 查看所有的配置文件順序
$ rpmbuild --showrc | grep macros

--- 確認上述的根工作目錄，兩種方式相同
$ rpmbuild --showrc | grep ': _topdir'
$ rpm --eval '%_topdir'

--- 設置topdir變量，然後再次驗證下，確認已經修改為目標目錄
$ echo "%_topdir %{getenv:HOME}/rpm-maker" > ~/.rpmmacros
$ rpm --eval '%_topdir'
--- 也可以在製作rpm包時通過--define指定
$ rpmbuild --define '_topdir rpm-maker' -ba package.spec
{% endhighlight %}

下載 MySQL 的源碼，並保存在 SPECS 目錄下，整體目錄結構如下。

{% highlight text %}
$ tree ~/rpm-maker
.
|-- BUILD                      ← 解壓後的tar.gz包
|-- BUILDROOT                  ← 臨時安裝目錄，會打包該目錄下文件
|-- RPMS                       ← 編譯後的RPM包
|-- SOURCES                    ← 所需的文件
|   `-- mysql-xxx.tar.gz       ← 包括了源碼包
|-- SPECS                      ← *.spec編譯腳本
|   |-- mysql-xxx.spec
|   `-- mysql-yyy.spec
`-- SRPMS                      ← src格式的rpm包存放的位置
{% endhighlight %}

### 執行命令

接下來，看看如何生成 rpm 包，也就是使用 rpmbuild 命令，過程可以分為多個階段。

{% highlight text %}
$ rpmbuild --bb mysql.spec   

使用參數：
   -bb 製作成二進制RPM包
   -bs 製作源碼的RPM包
   -ba 源碼和二進制兩種形式的RPM包
   -bl 檢測BUILDROOT沒有包含到rpm包中的文件

   -bp 執行到pre
   -bc 執行到build段
   -bi 執行install段

   --quiet 默認會輸出每次執行的shell命令，此時忽略

----- 通過源碼包重新編譯
$ rpmbuild --rebuild package-0.0-0.src.rpm

----- 編譯時指定參數
$ rpmbuild -bb SPECS/librdkafka.spec --define "__version 0.9.4" --define "__release 1"
{% endhighlight %}

一般來說，執行的順序為 `rpmbuild -bp`，再 `-bc` 再 `-bi`，如果沒問題，`rpmbuild -ba` 生成 src 包與二進制包，使用時通過 `rpm -ivh xxx.rpm;` 以及 `rpm -e xxx.rpm` 進行安裝卸載。

### 執行步驟

{% highlight text %}
1. 在 %prep 段中，通過 %setup -q 宏解壓、打補丁，一般是從 SOURCES 目錄下解壓到 BUILD 目錄下，一般目錄是 "NAME-VERSION" 。
2. 通過 %build 段定義瞭如何進行編譯，編譯目錄就是上述的 BUILD/NAME-VERSION 。
   2.1 首先通過 %configure %cmake 進行配置。
   2.2 然後利用 %{__make} %{?_smp_mflags} 進行並行編譯。
3. 接著就是安裝，也就是 %install 字段，一般會在 BUILDROOT/NAME-VERSION-RELEASE-ARCH 目錄下。
   3.1 通常為了清理環境會先使用 rm -rf %{buildroot} 清理。
   3.2 接著通過 %{__make} install DESTDIR=%{buildroot} 命令進行安裝，其中 DESTDIR 相當於是根目錄了。
   3.3 通過上步安裝的文件，需要都打包到 RPM 包中，如果不需要那麼就先清理掉。
   3.4 編譯沒有生成的也可以通過 %{__install} %{__mv} %{__rm} 命令直接複製。
4. 執行檢查規範，對應了 %test 段，一般執行一些單元測試。
5. 開始打包
6. 在 %clean 段處理清理操作，通常會通過 rm -rf %{buildroot} 刪除編譯的中間內容。
{% endhighlight %}

另外，還有 `%check` 字段，用於做些檢查。

<!--
%check
ctest -V %{?_smp_mflags}
%{!?el5:-N}
-->

## SPEC 文件

接下來準備 spec 文件，也是核心的內容，該文件包括三部分：介紹部分，編譯部分，files 部分。接下來，是一個簡單的示例，可以看看到底是如何製作 RPM 包的。

{% highlight text %}
### 0.define section                ← ###自定義宏段(可選)，方便後續引用
%define mysql_user mysql                            ← 定義宏，方便後續引用
%define _topdir /home/jinyang/databases/rpm-maker

### 1.The introduction section      ← ###介紹區域段
Name:           mysql                               ← 包名稱，通常會在指定源碼壓縮包時引用
Version:        5.7.17                              ← 版本號，同上
Release:        1%{?dist}                           ← 釋出號，每次製作rpm包時遞增該數字
Summary:        MySQL from FooBar.                  ← 軟件包簡介，最好不要超過50字符
License:        GPLv2+ and BSD                      ← 許可，GPL、BSD、MIT等，也可以使用or

Group:          Applications/Databases              ← 程序組名，從/usr/share/doc/rpm-VER/GROUPS選擇
URL:            http://kidding.com                  ← 一般為官網
Source0:        %{name}-boost-%{version}.tar.gz     ← 源碼包名稱(可以使用URL)，可以用SourceN指定多個，如配置文件
#Patch0:         some-bugs.patch                    ← 如果需要打補丁，則依次填寫
BuildRequires:  gcc,make                            ← 製作過程中用到的軟件包
Requires:       pcre,pcre-devel,openssl,chkconfig   ← 安裝時所需軟件包，可使用bash >= 1.1.1
Requires(pre):  test                                ← 指定不同階段的依賴

BuildRoot:      %_topdir/BUILDROOT                  ← 會打包該目錄下文件，可查看安裝後文件路徑
Packager:       FooBar <foobar@kidding.com>
Vendor:         kidding.com

%description                        ← ###軟件包的詳細描述，可以撒丫子寫了
It is a MySQL from FooBar.

#--- 2.The Prep section             ← ###準備階段，主要是解壓源碼，並切換到源碼目錄下
%prep
%setup -q                                           ← 宏的作用是解壓並切換到目錄
#%patch0 -p1                                        ← 如果需要打補丁，則依次寫

#--- 3.The Build Section            ← ###編譯製作階段，主要目的就是編譯
%build

make %{?_smp_mflags}                                ← 多核則並行編譯

#--- 4.Install section              ← ###安裝階段
%install
if [-d %{buildroot}]; then
   rm -rf %{buildroot}                              ← 清空下安裝目錄，實際會自動清除
fi
make install DESTDIR=%{buildroot}                   ← 安裝到buildroot目錄下
%{__install} -Dp -m0755 contrib/init.d %{buildroot}%{_initrddir}/foobar
%{__install} -d %{buildroot}%{_sysconfdir}/foobar.d/

#--- 4.1 scripts section            ← ###沒必要可以不填
%pre                                                ← 安裝前執行的腳本
%post                                               ← 安裝後執行的腳本
%preun                                              ← 卸載前執行的腳本
%postun                                             ← 卸載後執行的腳本
%pretrans                                           ← 在事務開始時執行腳本
%posttrans                                          ← 在事務結束時執行腳本

#--- 5. Clean section               ← ###清理段，可以通過--clean刪除BUILD
%clean
rm -rf %{buildroot}

#--- 6. File section                ← ###打包時要包含的文件，注意同時需要在%install中安裝
%files
%defattr (-,root,root,0755)                         ← 設定默認權限
%config(noreplace) /etc/my.cnf                      ← 表明是配置文件，noplace表示替換文件
%doc %{src_dir}/Docs/ChangeLog                      ← 表明這個是文檔
%attr(644, root, root) %{_mandir}/man8/mysqld.8*    ← 分別是權限，屬主，屬組
%attr(755, root, root) %{_sbindir}/mysqld

#--- 7. Chagelog section            ← ###記錄SPEC的修改日誌段
%changelog
* Fri Dec 29 2012 foobar <foobar@kidding.com> - 1.0.0-1
- Initial version
{% endhighlight %}

對於 ```%config(noreplace)``` 表示為配置文件，且不能覆蓋，新安裝時如果配置文件存在則會將原文件保存為 `*.rpmsave` ，升級時則不會覆蓋原文件，直接將包中的文件命名為 `*.rpmnew`，更詳細內容可以參考 [RPM, %config, and (noreplace)](http://people.ds.cam.ac.uk/jw35/docs/rpm_config.html) 。

<!-- /reference/linux/rpm-config-noreplace.maff -->

<!--
###  4.1 scripts section #沒必要可以不寫 %pre        #rpm安裝前制行的腳本 if [ $1 == 1 ];then    #$1==1 代表的是第一次安裝，2代表是升級，0代表是卸載         /usr/sbin/useradd -r nginx 2> /dev/null  ##其實這個腳本寫的不完整fi %post       #安裝後執行的腳本  %preun      #卸載前執行的腳本 if [ $1 == 0 ];then         /usr/sbin/userdel -r nginx 2> /dev/null fi %postun     #卸載後執行的腳本  ###  5.clean section 清理段,刪除buildroot  %clean rm -rf %{buildroot}      ###  6.file section 要包含的文件 %files  %defattr (-,root,root,0755)   #設定默認權限，如果下面沒有指定權限，則繼承默認 /etc/           #下面的內容要根據你在%{rootbuild}下生成的來寫     /usr/ /var/      ###  7.chagelog section  改變日誌段 %changelog *  Fri Dec 29 2012 laoguang <ibuler@qq.com> - 1.0.14-1 - Initial version

{% highlight text %}
%global mysqldatadir /var/lib/mysql
%define _topdir /home/jinyang/databases/rpm-maker
Name:           mysql
Version:        5.7.17
Release:        1%{?dist}
Summary:        MySQL from FooBar.

Group:          Applications/Databases
License:        GPLv2+ and BSD
URL:            http://kidding.com
Source0:        %{name}-boost-%{version}.tar.gz
#BuildRequires:
#Requires:

%description
It is a MySQL from FooBar.

%prep
#%setup -q -T -D

%build
#(
#    if ! [ -d release ]; then
#        mkdir release
#    fi
#    pushd release
#    cmake ../%{name}-%{version}                  \
#        -DCMAKE_INSTALL_PREFIX=/opt/mysql-5.7      \
#        -DCMAKE_BUILD_TYPE=Debug                   \
#        -DWITH_EMBEDDED_SERVER=OFF                 \
#        -DWITH_EXAMPLE_STORAGE_ENGINE=ON           \
#        -DWITH_SAFEMALLOC=OFF                      \
#        -DWITH_BOOST=../%{name}-%{version}/boost
#    make %{?_smp_mflags}
#    popd
#)

%install
### Cleanup
##if [-d %{buildroot}]; then
##   rm -rf %{buildroot}
##fi
{% endhighlight %}
-->


關於腳本部分，通常所有安裝腳本和觸發器腳本都是使用 ```/bin/sh``` 來執行的，如果想使用其他腳本，如 Lua，可使用 ```-p /usr/bin/lua``` 來告訴 rpm 使用 lua 解釋器。如

{% highlight text %}
%post -p /usr/bin/lua
# lua script here
%postun -p /usr/bin/perl
$ perl sciprt here
{% endhighlight %}

另外，如果只執行一條命令，也使用 ```-p``` 選項可直接執行，如 ```%post -p /sbin/ldconfig``` 。

一般最後一條命令的 exit 狀態就是腳本的 exit 狀態，除一些特殊情況，一般腳本都是以 ```exit 0``` 狀態退出，所以大部分小腳本片段都會使用 ```"||:"``` 退出。

### %prep

{% highlight text %}
%setup -q -T -a 0 -a 7 -a 10 -c -n %{src_dir}
參數列表：
    -T 禁止自動解壓源碼文件
    -D 解壓前不刪除目錄
    -a 切換目錄前，解壓指定Source文件，例如-a 0表示解壓Source0
    -b 切換目錄後，解壓指定Source文件，例如-a 0表示解壓Source0
    -n 解壓後目錄名稱與RPM名稱不同，則通過該參數指定切換目錄
    -c 解壓縮之前先生成目錄
{% endhighlight %}

<!--
%patch 最簡單的補丁方式，自動指定patch level。
%patch 0 使用第0個補丁文件，相當於%patch ?p 0。
%patch -s 不顯示打補丁時的信息。
%patch -T 將所有打補丁時產生的輸出文件刪除。
-->

### systemd

spec 腳本中提供了與 systemd 相關的腳本，關於腳本的詳細內容可以查看 [macros.systemd.in](https://cgit.freedesktop.org/systemd/systemd/tree/src/core/macros.systemd.in)，實用方式很簡單，如下：

{% highlight text %}
----- 安裝結束後，實際執行systemctl preset，執行服務預設的內容
%systemd_post foobar.service

----- 執行卸載前，systemctl disable
%systemd_preun foobar.service

----- 卸載後執行重啟操作，daemon-reload+try-restart
%systemd_postun_with_restart foobar.service

----- 如果不需要重啟，例如有狀態鏈接的(D-Bus)，執行systemctl daemon-reload
%systemd_postun foobar.service
{% endhighlight %}

關於安裝升級的各個操作步驟詳見 [RPM SPEC pre/post/preun/postun argument values](http://meinit.nl/rpm-spec-prepostpreunpostun-argument-values) 以及 [Fedora Packaging Guidelines for RPM Scriptlets](https://fedoraproject.org/wiki/Packaging:Scriptlets) 。

### 校驗

在安裝完 RPM 包之後，可以通過 ```--verify``` 或者 ```-V``` 進行校驗，正常不會顯示任何信息，可以通過 ```--verbose``` 或者 ```-v``` 顯示每個文件的信息；文件丟失顯示 ```missing```，屬性方面的修改內容如下：

{% highlight text %}
SM5DLUGT c filename
屬性：
  S: 文件大小;
  M: 權限;
  5: MD5檢查和;
  D: 主從設備號;
  L: 符號連接;
  U: 屬主;
  G: 屬組;
  T: 最後修改時間。
類型：
  c: 配置文件；
  d: 文檔文件。
{% endhighlight %}

## 增加簽名

當製作 RPM 包之後，為了防止被篡改，可以使用私鑰進行簽名，然後將公鑰開放出去，然後用戶下載完軟件包可以通過公鑰進行驗證簽名，從而確保文件的原始性。

可以通過如下步驟進行操作。

### 1. 生成密鑰對

首先，需要使用 gpp 來生成公私鑰對，一般可以使用 RSA 非對稱加密。

{% highlight text %}
$ gpg --gen-key
{% endhighlight %}

此時會在 `~/.gnupg` 目錄下創建相關文件，創建完後可以通過如下命令查看當前的密鑰對。

{% highlight text %}
$ gpg --list-keys
/home/foobar/.gnupg/pubring.gpg
------------------------------
pub   2048R/860FB269 2017-01-13
uid                  foobar (just for test) <foobar@example.com>
{% endhighlight %}

其中 UID 分別對應了用戶名、註釋、郵箱。

<!-- Repository Signer (OSSXP) -->

### 2. 軟件包簽名

修改 RPM 宏，使用上述生成的密鑰對。

{% highlight text %}
$ echo %_signature gpg >> ~/.rpmmacros
$ echo "%_gpg_name foobar (just for test)" >> ~/.rpmmacros
{% endhighlight %}

對已有 rpm 軟件包進行簽名。

{% highlight text %}
$ rpm --addsign package_name.rpm
Enter pass phrase:
Pass phrase is good.
package_name.rpm
{% endhighlight %}

如果失敗，則會出現如下的錯誤。

{% highlight text %}
$ rpm --addsign package_name.rpm
Enter pass phrase:
Pass phrase check failed.
{% endhighlight %}

此時需要仔細檢查密鑰名稱和寫入 rpm 宏裡面的是否一致，最好是將 `--list-keys` 顯示的內容整體複製出來。

當然也可以在通過 `rpmbuild --sign` 打包時自動包含簽名。

### 3. 增加簽名

生成簽名使用的是私鑰，驗證簽名需要使用公鑰。簡單來說需要執行：1)將 gpg 產生的公鑰導出到一個文件；2) 將這個公鑰文件導入到 RPM 數據庫裡；3) 使用 rpm 命令進行檢驗。

{% highlight text %}
----- 導出公鑰到一個文本文檔
$ gpg --export -a "foobar (just for test)" > RPM-GPG-KEY-FOOBAR

----- 查看rpm數據庫中已有的公鑰
$ rpm -q gpg-pubkey-*
gpg-pubkey-f4a80eb5-53a7ff4b
gpg-pubkey-352c64e5-52ae6884
gpg-pubkey-442df0f8-4783f24a

----- 將公鑰導入到RPM數據庫，注意需要root用戶執行
# rpm --import RPM-GPG-KEY-FOOBAR
{% endhighlight %}

通過上述命令重新查看公鑰時，會發現新增了一個 `gpg-pubkey-860fb269-5a65a0ad` 公鑰，其中 `860FB269` 與上述 gpg 生成的密鑰信息相同。

如果不需要，可以通過 `rpm -e gpg-pubkey-860fb269-5a65a0ad` 刪除即可。

### 4. 簽名驗證

通過 `rpm -K package_name.rpm` 命令對簽名驗證即可，如下是驗證成功以及失敗時輸出的內容。

{% highlight text %}
package_name.rpm: rsa sha1 (md5) pgp md5 OK
package_name.rpm: RSA sha1 (MD5) (PGP) (MD5) (PGP) md5 NOT OK (MISSING KEYS: PGP#c0eb63c7 PGP#c0eb63c7)
{% endhighlight %}

### 其它

如果導入公鑰失敗，那麼在安裝時會生成一個類似 `Header V3 RSA/SHA1 signature: NOKEY, key ID c0eb63c7` 的告警信息。

如果要在 YUM 中驗證簽名，可以將公鑰複製到系統 RPM 公鑰目錄，一般如下操作即可。

{% highlight text %}
# cp RPM-GPG-KEY-FOOBAR /etc/pki/rpm-gpg/
{% endhighlight %}

在源配置文件中通過如下兩行來指定 gpg key 檢驗：

{% highlight text %}
gpgcheck=1
gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-FOOBAR
{% endhighlight %}

這樣在通過 YUM 安裝軟件包時，當下載完畢以後會首先使用公鑰進行簽名驗證。

## 常用設置

接下來，看看一些常用的實用技巧。

在一個 SPEC 文件中可以同時打包多個 RPM 包，當然也可以通過 `%package -n foobar` 指定子模塊 (subpackage) 的名稱 。

<!--
http://ftp.rpm.org/max-rpm/s1-rpm-subpack-building-subpackages.html
http://ftp.rpm.org/max-rpm/s1-rpm-subpack-spec-file-changes.html
http://ftp.rpm.org/max-rpm/ch-rpm-b-command.html
-->

### 宏定義

在定義文件的安裝路徑時，通常會使用類似 ```%_sharedstatedir``` 的宏，這些宏一般會在 ```/usr/lib/rpm/macros``` 中定義，當然部分會同時在不同平臺上覆蓋配置，可以直接 ```grep``` 查看。

RPM 內建宏定義在 `/usr/lib/rpm/redhat/macros` 文件中，這些宏基本上定義了目錄路徑或體系結構等等；同時也包含了一組用於調試 spec 文件的宏，關於 Macro 詳細可以查看 [Macro syntax](http://rpm.org/user_doc/macros.html)，其中常用如下：

{% highlight text %}
%dump                 打印宏的值，包括一些內建的宏定義，也可以通過rpm --showrc查看
%{echo:message}       打印信息到標準輸出
%{warn:message}       打印信息到標準錯誤
%{error:message}      打印信息到標準錯誤，然後返回BADSPEC
%{expand:expression}  類似Bash中的eval內置命令
{% endhighlight %}

另外常用的是根據宏來設置變量。

{% highlight text %}
%{?foobar:expr} 如果宏 foobar 存在則使用 expand expr，否則為空；也可以取反 %{!?foobar:expr}
%{?macro}       只測試該宏是否存在，存在就用該宏的值，反之則不用，如 %configure %{?_with_foobar}
{% endhighlight %}

另外，在判斷宏的 Bool 值時，可以通過如下方式測試，如果 `variable` 定義，則為 `01` 也就是 `true` 否則為 `0` 。

{% highlight text %}
%if 0%{?variable:1}
... ...
%endif
{% endhighlight %}


### 配置腳本

很多製作 RPM 包的操作都是通過宏定義設置的，如下簡單列舉一下常見的宏定義操作。

{% highlight text %}
__os_install_post
    安裝完之後的操作，例如去除二進制文件中的註釋等；
__spec_install_pre
    在安裝前的操作，設置一些環境變量，然後刪除BUILDROOT中的文件(如果文件多時耗時增加)；
{% endhighlight %}

如果想取消某些操作，可以將這些操作設置為 ```%{nil}``` 即可。

{% highlight text %}
----- 查看對應的命令
$ rpm --showrc | grep -A 4 ': __os_install_post'
-14: __os_install_post
    /usr/lib/rpm/redhat/brp-compress
    %{!?__debug_package:
    /usr/lib/rpm/redhat/brp-strip %{__strip}
    /usr/lib/rpm/redhat/brp-strip-comment-note %{__strip} %{__objdump}

----- 在SPEC文件頭部添加如下內容
%global __os_install_post %{nil}

----- 在用戶的~/.rpmmacros文件中添加如下配置
%__os_install_post %{nil}
{% endhighlight %}

<!--
或者在全局配置文件中添加如下配置
/etc/rpm/macros
%__os_install_post %{nil}       
-->

### define vs. global

兩者都可以用來進行變量定義，不過在細節上有些許差別，簡單列舉如下：

* define 用來定義宏，global 用來定義變量；
* 如果定義帶參數的宏 (類似於函數)，必須要使用 define；
* 在 ```%{}``` 內部，必須要使用 global 而非 define；
* define 在使用時計算其值，而 global 則在定義時就計算其值；

可以簡單參考如下的示例。

{% highlight spec %}
#--- %prep之前的參數是必須要有的
Name:           mysql
Version:        5.7.17
Release:        1%{?dist}
Summary:        MySQL from FooBar.
License:        GPLv2+ and BSD

%description
It is a MySQL from FooBar.

%prep
#--- 帶參數時，必須使用%define定義
%define myecho() echo %1 %2
%{!?bar: %define bar defined}

echo 1: %{bar}
%{myecho 2: %{bar}}
echo 3: %{bar}

# 如下是輸出內容
#1: defined
#2: defined
#3: %{bar}
{% endhighlight %}

顯然 3 的輸出是不符合預期的，可以將 ```%define``` 修改為 ```global``` 即可。

### 變量使用

define 定義的變量類似於局部變量，只在 ```%{!?foo: ... }``` 區間有效，不過 SPEC 並不會自動清除該變量，只有再次遇到 ```%{}``` 時才會清除，WTF!!!

在命令行中通過 ```--define 'with_ssl bundled'``` 進行定義，在 SPEC 腳本中，使用 ```%{?with_ssl: }``` 處理上述參數，或者通過 ```%{!?with_ssl: }``` 處理未定義參數時的情況。

{% highlight text %}
#--- 根據傳入的變量，設置好相應的ssl_option變量
%{?with_ssl: %global ssl_option -DWITH_SSL=%{with_ssl}}

#--- 接下來，通過如下方式使用上述定義的變量
%{?ssl_option}
{% endhighlight %}

在 RPM 中，問號 ```?``` 用於條件檢測，默認如果對應的變量值不存在，那麼 RPM 會原樣保留字符串，通過問號表示，如果不存在則移除。

{% highlight text %}
$ rpm --eval='foo:%{foo}'$'\n''bar:%{?bar}'
foo:%{foo}
bar:

$ rpm --define='foo foov' --eval='foo:%{foo}'$'\n''bar:%{?bar}'
foo:foov
bar:

$ rpm --define='foo foov' --define='bar barv' --eval='foo:%{foo}'$'\n''bar:%{?bar}'
foo:foov
bar:barv
{% endhighlight %}

除了上述的表示方法外，還可以使用如下內容：

{% highlight text %}
%define _without_foobar            1
# enabled by default
%define with_foobar 0%{!?_without_foobar:1}
{% endhighlight %}

如果 ```_without_foobar``` 變量不存在，那麼默認 ```with_foobar``` 為 01 也就是 true，否則就是 0 也就是 false；可以通過如下命令行進行測試。

{% highlight text %}
$ rpm --define='with_foobar 0%{!?_without_foobar:1}' --eval='foobar:%{with_foobar}'
foobar:01
$ rpm --define='_without_foobar 1' --define='with_foobar 0%{!?_without_foobar:1}' --eval='foobar:%{with_foobar}'
foobar:0
{% endhighlight %}


### 其它

如下是 ```%if``` 判斷的使用。

{% highlight text %}
%if 0%{?rhel} == 6
%global compatver             5.1.17
BuildRequires:  systemd
%else
%global compatver             5.1.17
BuildRequires:  sysv
%endif
{% endhighlight %}

可以使用 Requires(pre)、Requires(post) 等都是針對不同階段的依賴設置；可以通過 ```%package PACKAGE-NAME``` 設置生成不同的 RPM 分支包。

另外，可以生成 GPG 簽名，在此不再贅述。

#### 添加用戶

可以在 ```%pre``` 段中添加如下內容。

{% highlight text %}
/usr/sbin/groupadd -g 66 -o -r monitor >/dev/null 2>&1 || :

參數：
  --gid/-g
    指定group id；
  --non-unique/-o
    group id可以不唯一，此時相當於指定了一個 alias；
  --system/-r
    創建系統分組；

/usr/sbin/useradd -M %{!?el5:-N} -g monitor -o -r -d %{_libdir}/%{name} -s /bin/false \
    -c "Uagent Server" -u 66 monitor >/dev/null 2>&1 || :
參數：
  --no-create-home/-M
    不創建HOME目錄；
  --no-user-group/-N
    不創建相同用戶名的分組；
  --non-unique/-o
    user id可以不唯一，此時相當於指定了一個 alias；
  --system/-r
    創建系統用戶；
  --home-dir/-d HOME_DIR
    指定HOME目錄，如果不存在則會創建；
  --shell/-s SHELL
    指定默認的shell；
{% endhighlight %}


#### 初始宏定義

很多的宏，是在 ```/etc/rpm``` 目錄下定義的，如上面的 ```dist``` 在 ```/etc/rpm/macros.dist``` 文件中定義。


#### RPM包查看

對於生成的 RPM 包，只能查看頭部信息和腳本內容，指令分別如下。

{% highlight text %}
$ rpm --info -qp XXX.rpm
$ rpm --scripts -qp XXX.rpm
{% endhighlight %}

#### 變量定義

在定義 `Version` 時，如果使用 `%{?package_version:1.0.0}` 可以工作但是，使用 `%{!?package_version:1.0.0}` 卻無效。

而且這裡的參數不能通過類似 `rpmbuld --define='package_version 1.9.1' foobar.spec` 的方式傳入。

### 測試腳本

如下是一個測試用的腳本，可以用來生成簡單的測試 SPEC 腳本，並執行。

該腳本會將 /tmp/foobar 目錄作為工作目錄，然後生成一個簡單的 Hello world 程序。

{% highlight bash %}
#!/bin/bash

WORKSPACE=/tmp/foobar

echo "0. Prepare dirs"
mkdir -pv ${WORKSPACE}/{BUILD,BUILDROOT,RPMS,SOURCES,SPECS,SRPMS,Workspace/foobar-1.0.0}

echo "1. Generate tar"
cd ${WORKSPACE}/Workspace/foobar-1.0.0
cat << EOF > main.c
#include
int main(int argc, char *argv)
{
    printf("Hello World!!!\n");
        return 0;
}
EOF
cat << "EOF" > Makefile
all: main.c
        gcc -o foobar $< -Wall
install:
        install -m 755 foobar ${DESTDIR}/usr/bin/foobar
EOF
cd ${WORKSPACE}/Workspace
tar -jcf foobar-1.0.0.tar.bz2 foobar-1.0.0
#[ "${WORKSPACE}/BUILD" != "/" ] && rm -rf "${WORKSPACE}/BUILD/"
rm -rf "${WORKSPACE}/SOURCES/foobar-1.0.0.tar.bz2"
mv foobar-1.0.0.tar.bz2 ${WORKSPACE}/SOURCES/

echo "2. Prepare spec-file"
cd ${WORKSPACE}
cat << EOF > SPECS/foobar-1.0.0.spec
### 0. The define section
%global _topdir ${WORKSPACE}

### 1.The introduction section
Name:           foobar
Version:        1.0.0
Release:        1%{?dist}
Summary:        Just a RPM example
License:        GPLv2+ and BSD
BuildRequires:  gcc,make
Source0:        %{name}-%{version}.tar.bz2
BuildRoot:      %_topdir/BUILDROOT

%description
It is a RPM example.

#--- 2.The Prep section
%prep
%setup -q
#%patch0 -p1

#--- 3.The Build Section
%build
make %{?_smp_mflags}
#echo %{_sysconfdir}
~
%install
#/usr/bin/
install -d -m 0751 %{buildroot}/%{_bindir}
make install DESTDIR=%{buildroot}
#--- 4.1 scripts section
%pre
echo "pre" >> /tmp/foobar
%post
echo "post" >> /tmp/foobar
%preun
echo "preun" >> /tmp/foobar
%postun
echo "postun" >> /tmp/foobar

#--- 5.clean section
%clean
rm -rf %{buildroot}

#--- 6.file section
%files
%defattr(-,root,root,-)
%attr(755, root, root) %{_bindir}/foobar

#--- 7.chagelog section
%changelog
* Fri Dec 29 2012 foobar foobar@kidding.com - 1.0.0-1
- Initial version
EOF

echo "3. Perform rpmbuild"
cd ${WORKSPACE}
rpmbuild --clean --define '_topdir /tmp/foobar' -ba SPECS/foobar-1.0.0.spec~
{% endhighlight %}



## 參考

很多關於 RPM 的介紹可以參考文檔 [Maximum RPM](http://rpm.org/max-rpm-snapshot/index.html)；另外，還可以參考下 MySQL 源碼包中的相關示例 [mysql.spec](/reference/databases/mysql/mysql.spec) 。

<!--
http://fedoraproject.org/wiki/How_to_create_an_RPM_package

包含生成GPG簽名
http://laoguang.blog.51cto.com/6013350/1103628

一個collectd創建的示例
http://giovannitorres.me/how-to-build-rpms-collectd-example.html

rpm mock
https://leo108.com/pid-2207.asp

-->

{% highlight text %}
{% endhighlight %}
