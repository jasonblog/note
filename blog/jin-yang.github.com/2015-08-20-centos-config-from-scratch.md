---
title: CentOS 安裝與配置
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,centos,安裝,配置
description: CentOS (Community Enterprise Operating System) 也即社區版的企業操作系統，是 Linux 的發行版本之一，來自於 Red Hat Enterprise Linux 所開放的源碼編譯而成。因此，對穩定性較高的服務器通常以 CentOS 替代商業版的 Red Hat 。兩者的不同在於 CentOS 並不包含封閉源代碼軟件。下面會介紹桌面版的 CentOS 在安裝時需要作的一些常用配置。
---

CentOS (Community Enterprise Operating System) 也即社區版的企業操作系統，是 Linux 的發行版本之一，來自於 Red Hat Enterprise Linux 所開放的源碼編譯而成。因此，對穩定性較高的服務器通常以 CentOS 替代商業版的 Red Hat 。

兩者的不同在於 CentOS 並不包含封閉源代碼軟件。下面會介紹桌面版的 CentOS 在安裝時需要作的一些常用配置。

<!-- more -->

![centos logo]({{ site.url }}/images/linux/centos-logo.png "centos logo"){: .pull-center }

## 安裝 CentOS

對於 CentOS 桌面版的安裝也比較簡單，完全是基於圖形界面的，安裝時可以從 [www.centos.org](https://www.centos.org/download/) 上下載相應版本，一般用 LiveGNOME 或者 LiveKDE 。

當然，也可以從國內的一些鏡像網站上下載，例如 [阿里雲鏡像](http://mirrors.aliyun.com/)、[搜狐鏡像](http://mirrors.sohu.com/)、[網易鏡像](http://mirrors.163.com/) 上下載。

下載了 ISO 鏡像之後可以通過如下方式安裝到 U 盤中，如果有 Linux 發行版，安裝過程將很簡單；如下是通過 dd 命令直接複製即可。

{% highlight text %}
----- 查看U盤的設備號
# fdisk -l
... ...
Disk /dev/sdc: 4002 MB, 4002910208 bytes, 7818184 sectors
... ...

----- 將ISO鏡像原樣複製到U盤中
# dd if=xxx.iso of=/dev/sdb

----- 從另外一個終端執行，查看複製到U盤的進度
# while killall -USR1 dd; do sleep 5; done
{% endhighlight %}

接下來就看看如何配置一些常用的環境。


## 軟件安裝

![centos power logo]({{ site.url }}/images/linux/centos-power-logo.jpg "centos power logo"){: .pull-right }

在 CentOS 中，會通過 YUM 或者 RPM 進行軟件包的管理。其中 RPM (Redhat Package Manager) 是由 RedHat 公司提供的軟件包管理器，可以實現軟件包的安裝、查詢、卸載、升級以及校驗等，相關的數據會保存在 /var/lib/rpm 目錄下，但 rpm 不能很好的解決軟件之間的依賴關係。

<!--
There are five basic modes for RPM command
    Install : It is used to install any RPM package.
    Remove : It is used to erase, remove or un-install any RPM package.
    Upgrade : It is used to update the existing RPM package.
    Verify : It is used to query about different RPM packages.
    Query : It is used for the verification of any RPM package.-->

在安裝時，通常系統文件存放在 /bin、/sbin、/lib 目錄下，而第三方軟件會存放在 /usr/local/bin、/usr/local/sbin、 /usr/local/lib、/usr/share/man 和 /usr/local/share/man (後兩者為說明文件) 。

RPM 包通常有一個通用的命名方式：name-version-release.arch.rpm 。

* name: 表示包的名稱，包括主包名和分包名；
* version: 表示包的版本信息；
* release: 用於標識 rpm 包本身的發行號，可還包含適應的操作系統；
* arch: 表示主機平臺，noarch 表示此包能安裝到所有平臺上面。

如 gd-devel-2.0.35-11.el6.x86_64.rpm ，gd 是這個包的主包名；devel 是這個包的分包名； 2.0.35 是表示版本信息，2 為主版本號，0 表示次版本號，35 為源碼包的發行號也叫修訂號； 11.el6 中的 11 是 rpm 的發行號， el6 表示 RHEL6； x86_64 是表示適合的平臺。

為了解決各個包之間的依賴關係，可以採用 YUM (Yellowdog Updater Modified) 進行管理，該軟件是由 Seth Vidal 開發，用於管理 RPM 包。

### 常用命令

如下列舉一些常用的命令。

{% highlight text %}
----- 查看軟件信息
# rpm -q kernel                                           // 查看安裝包的全名
# rpm -ql kernel                                          // 查看已安裝軟件包含有的文件
# rpm -qi kernel                                          // 查看已安裝軟件包的摘要信息
# rpm -qa | grep software                                 // 查看是否安裝了software
# rpm -qf /usr/sbin/ifcfg                                 // 查看某文件屬於那個包
# rpm -qc iproute                                         // 查看生成了那些配置文件
# rpm -qd iproute                                         // 查看生成了那些幫助文件
# rpm -qpi iproute.rpm                                    // 查看rpm包的信息
# rpm -qpl iproute.rpm                                    // 查看rpm包中含有的軟件列表
# yum info mysql                                          // 查看包信息

----- 搜索軟件
# yum search package                                      // 搜索package

----- 安裝軟件
# rpm --checksig foobar.rpm                               // 校驗PGP Signature，查看完整性和來源
# rpm -qpR foobar.rpm                                     // 查看依賴
# rpm -ivh --nodeps --force foobar.rpm                    // 強制安裝，如果其它軟件包未安裝，則不能工作
# yum -y install foobar                                   // 默認為 yes ，通常用於腳本文件，不需要交互

----- 只下載軟件，如下命令包含在yum-utils包中
# yumdownloader PACK                                      // 使用yum自帶軟件
# yum -y install yum-downloadonly                         // 通過yum-downloadonly插件
# yum -y install --downloadonly --downloaddir=/tmp/ PACK  // 直接使用

----- 安裝軟件包群，很多被打包成Group的軟件，可以通過如下命令查看、安裝
# yum grouplist
# yum groupinstall 'MySQL Database'
# yum groupupdate 'MySQL Database'
# yum groupremove 'MySQL Database'

----- 卸載軟件
# rpm -evv --nodeps foobar                                // 不需要指定到foobar.rpm
# yum -y remove foobar

----- 升級軟件包
# rpm -Uvh foobar.rpm                                     // 如果新版本不工作，仍可以使用之前的版本
# yum check-update                                        // 查看可升級的軟件包
# yum update foobar                                       // 升級特定的軟件
# yum update                                              // 升級所有軟件

----- 清除/更新Cache
# yum clean all                                           // 默認會保存在/var/cache/yum
# yum makecache                                           // 通常在修改/etc/yum/repos.d目錄下配置文件之後

----- 獲得源碼，需要安裝yum-utils工具，提供了yumdownloader
# vi /etc/yum.repos.d/CentOS-Source.repo                  // 將enabled設置為1
# yum clean all                                           // 清空緩存
# yum makecache                                           // 使修改生效
# rpm -qf `which mysql`                                   // 查看對應安裝包
# yumdownloader --source mariadb                          // 下載源碼包，不加--source則只下載
# rpm2cpio coreutils-8.4-19.el6_4.2.src.rpm | cpio -ivd   // 解壓rpm包

----- 列出軟件
# yum list | less                                         // 列出所有可安裝的軟件包
# yum list updates                                        // 列出所有可更新的軟件包
# yum list installed                                      // 列出所有已安裝的軟件包
# yum list extras                                         // 列出所有已安裝但不在 Yum Repository 內的軟件包
# yum list [package]                                      // 列出所指定的軟件包

----- 其它
# yum shell                                               // 交互環境，可以執行多條命令
# yum history                                             // 查看歷史
# rpm -Vp xxx.rpm                                         // 與數據庫中的版本校驗 (Verify)
# rpm -Va                                                 // 校驗所有的
# rpm --import /etc/pki/rpm-gpg/RPM-GPG-KEY-CentOS-6      // 導入 GPG key
----- 查詢未安裝軟件包的依賴關係
$ rpm -qRp vim-common-6.3.046-2.el4.1.x86_64.rpm
----- 查詢已安裝軟件包的依賴關係
$ rpm -qR vim-common-6.3.046-2.el4.1

----- 數據庫出錯，通過下述命令重建
# rm /var/lib/rpm/__db*
# rpm --rebuilddb
# rpmdb_verify Packages
{% endhighlight %}

yum makecache 會把服務器的包信息下載到本地電腦緩存起來，配合 yum -C search xxx 使用，不用上網檢索就能查找軟件的相關信息。

### 問題排查

如果在通過 yum 安裝軟件時出現，Another app is currently holding the yum lock 錯誤，可以通過 cat /var/run/yum.pid 查看佔用的進程的 PID ，通過 rm -f /var/run/yum.pid 刪除，重新安裝。



## 源配置

CentOS 中官方的源只包含了有限的安裝包，為此需要安裝一些常用的三方源。當然，也可以自己手動創建 Yum 倉庫，主要採用了 createrepo，用於生成 repodata 軟件信息倉庫。

可以通常如下命令查看源列表的配置。

{% highlight text %}
# yum repolist                                            // 查看現在可用的源列表
# yum repolist all                                        // 查看所有的源列表
# yum --enablerepo=epel install mysql                     // 從指定源列表下載
{% endhighlight %}

接下來看看如何配置第三方的源。

### 使用第三方源

配置完成後可以通過 yum repolist all 命令查看三方源是否生效，通過 yum list software 查看相應的軟件。對於一些 RPM 包，也可以從 [pkgs.org/search/](http://pkgs.org/search/) 或者 [http://www.rpmfind.net](http://www.rpmfind.net) 查找相應的軟件包，以及與該軟件包相關的依賴。

注意：如果在安裝過程中出現 error: Failed dependencies 可以使用 --nodeps --force 選項強制安裝，不過此時有可能導致部分功能失效。

另外，關於 CentOS 的第三方源，可以參考 [CentOS Wiki](http://wiki.centos.org/zh/AdditionalResources/Repositories) 中給出的參考意見。

#### 準備工作

在使用時，最好先安裝 yum-priorities 插件，該插件用來設置 yum 在調用軟件源時的順序，因為官方提供的軟件源，都是比較穩定和被推薦使用的，因此，官方源的順序要高於第三方源的順序。

{% highlight text %}
# yum install yum-priorities
{% endhighlight %}

安裝完後需要設置 /etc/yum.repos.d/ 目錄下的 *.repo 相關文件，例如 CentOS-Base.repo、epel.repo、nux-dextop.repo 等，在這些文件中插入順序指令 priority=N (N為1到99的正整數，數值越小優先級越高)，一般第三的軟件源設置的優先級大於 10 。


#### 國內官方源

也就是一些 CentOS 的鏡像，常見的有 [centos.ustc.edu.cn](http://centos.ustc.edu.cn/)、[mirrors.163.com](http://mirrors.163.com/centos/)、[mirrors.sohu.com](http://mirrors.sohu.com/centos/)，只需要修改基本數據源中的 URL 配置選項。


#### EPEL, Extra Packages for Enterprise Linux

EPEL是由 Fedora 社區打造，為 RHEL 及衍生髮行版如 CentOS、Scientific Linux 等提供高質量軟件包的項目，詳細內容可以參考 [EPEL-Wiki](https://fedoraproject.org/wiki/EPEL/zh-cn) 。

安裝源，其中對應的版本需要根據當前的版本自行選擇。

{% highlight text %}
# rpm -Uvh http://mirrors.ustc.edu.cn/epel/beta/7/x86_64/epel-release-7-0.2.noarch.rpm
# rpm -Uvh http://dl.fedoraproject.org/pub/epel/beta/7/x86_64/epel-release-7-0.2.noarch.rpm
{% endhighlight %}

接下來時導入證書，當然這步也可以在通過 yum 安裝時根據提示自動導入。

{% highlight text %}
# rpm -import /etc/pki/rpm-gpg/RPM-GPG-KEY-EPEL-7
{% endhighlight %}


#### rpmforge

可以從 [pkgs.repoforge.org](http://pkgs.repoforge.org/rpmforge-release/) 或者 [apt.sw.be](http://apt.sw.be/redhat/el7/en/x86_64/rpmforge/RPMS/) 下載。

{% highlight text %}
# rpm -Uvh http://apt.sw.be/redhat/el7/en/x86_64/rpmforge/RPMS/rpmforge-release-xxx.rpm
{% endhighlight %}

#### nux-dextop

直接從 [nux-dextop-release*rpm](http://li.nux.ro/download/nux/dextop/el7/x86_64/) 上查找安裝最新的配置，通常 mplayer 會包含在該三方源中。



### 使用本地源

可以通過如下方法使用本地源，也就是下載的包含安裝包的 ISO 鏡像。

對於 VMware 需要通過如下方法掛載，[Setting] -> [Hardware] -> [CD/DVD] -> 右邊 Device 裡勾選 Connected，在 Use ISO image file 裡選擇 ISO 文件後確定即可。

CentOS 中使用的 yum 源配置文件保存在 /etc/yum.repos.d 目錄下，主要包括了兩個配置文件 CentOS-Base.repo 和 CentOS-Media.repo；其中，前者是配置網絡 yum 源的，而後者是用來配置本地 yum 源。

{% highlight text %}
----- 對於VM來說，也可以使用Share Folder
# mount /dev/cdrom /media/cdrom
# mount -o loop /mnt/hgfs/Share/CentOS-xxx-xxx-bin-DVD.iso /media/cdrom/
# vi /etc/yum.repos.d/iso.repo
[c6-media]
name=CentOS-$releasever - Media      # 自定義名稱
baseurl=file:///media/cdrom/         # 可以指定多個路徑
        file:///media/cdrom2/
gpgcheck=1
enabled=0
gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-CentOS-6

----- 清除緩存
# yum clean all

----- 證書也可以使用如下方式安裝
# rpm --import /etc/pki/rpm-gpg/RPM-GPG-KEY-CentOS-5
# yum --enablerepo=c6-media install mysql-server                   # 本地和網絡
# yum --disablerepo=\* --enablerepo=c6-media install mysql-server  # 只使用本地光盤
{% endhighlight %}

#### 原生本地源

如上的 ISO 鏡像文件中，已經包含了索引文件 (位於 repodata 文件夾)，如果自己創建本地鏡像，例如平時收藏的 RPM 軟件包或者集成測試等，此時需要通過 createrepo 創建索引文件。

當然，如過沒有 createrepo 命令，則需要安裝 createrepo.xxx.rpm 包。

{% highlight text %}
----- 1. 創建本地yum倉庫目錄
$ mkdir -p /share/CentOS/7/local/x86_64/RPMS

----- 2. 創建索引&更新緩存
$ createrepo /share/CentOS/7/local/x86_64
$ yum makecache

----- 3. 創建本地repo文件
$ cat<<-"EOF">/etc/yum.repos.d/CentOS-Local.repo
[local]
name=CentOS-$releasever - local packages for $basearch
baseurl=file:///share/CentOS/$releasever/local/$basearch
enabled=1
gpgcheck=0
protect=1
EOF
{% endhighlight %}

另外，可以參考 [How to create public mirrors for CentOS](https://wiki.centos.org/HowTos/CreatePublicMirrors)、[Create Local Repos](https://wiki.centos.org/HowTos/CreateLocalRepos) 。


## 常用軟件配置

### 安裝編譯環境

如下是安裝 C/C++ 編譯工具。

{% highlight text %}
# yum install gcc
# yum install gcc-c++             # 安裝g++
{% endhighlight %}

### 支持 NTFS 文件系統

可以在安裝完第三方軟件源 EPEL 之後通過如下命令安裝。

{% highlight text %}
# wget -O /etc/yum.repos.d/epel.repo http://mirrors.aliyun.com/repo/epel-7.repo
# yum update

# yum --enablerepo=epel install ntfs-3g
{% endhighlight %}

但是如果是最新版本的 CentOS，可能在三方庫中沒有相應的安裝包，那麼此時就需要從源碼編譯安裝。直接從 [www.tuxera.com](http://www.tuxera.com/community/ntfs-3g-download/) 下載源碼，然後通過如下方式編譯。

{% highlight text %}
$ tar -zxvf ntfs-3g_ntfsprogs-2014.2.15.tgz
$ cd ntfs-3g_ntfsprogs-2014.2.15
$ ./configure
$ make
# make install                # 完成安裝，需要root權限
{% endhighlight %}

接下來配置為自動掛載，假設對應的磁盤為 /dev/sda5 。

{% highlight text %}
# blkid /dev/sda5             # 查看UUID
# vi /etc/fstab               # 添加如下的內容
UUID=xxxxxxxxxxx /media/disk  ntfs-3g defaults 0 0
{% endhighlight %}


### 安裝 Flash 插件

直接從如下網站 [www.adobe.com](http://get.adobe.com/cn/flashplayer/) 獲得，主要有兩種方法。

A) YUM 安裝。下載時選擇 "YUM，適用於Linux(YUM)"，實際會下載一個 RPM 安裝包，用來安裝 adobe 的 yum 源配置文件。

{% highlight text %}
# rpm -ivh adobe-release-i386-1.0-1.noarch.rpm        安裝Adobe的源
# yum install flash-plugin                            安裝falsh插件
{% endhighlight %}

B）RPM 安裝。在下載頁面選擇 ".rpm，適用於其它Linux"，此時將會直接下載 RPM 安裝包，可以直接通過 RPM 進行安裝。

{% highlight text %}
# rpm -ivh flash-plugin-11.2.202.297-release.i386.rpm
{% endhighlight %}

### Gnome-Terminal配置

A) 取消聲音。Edit->Preferences->Profiles->選擇對應配置文件[Edit]->General->取消Terminal bell。

B) 設置啟動快捷鍵。在 CentOS 的系統菜單中選擇 Applications -> System Tools -> Keyboard -> Shortcuts -> Custom Shortcuts 設置命令為 gnome-terminal --hide-menubar --maximize，詳細參數可以參考 gnome-terminal --help-window-options。

C) 顏色設置。個人比較喜歡的顏色配置，文本顏色 #dbfef8，背景顏色 #2f4f4f 。

D) 設置為半透明。首先嚐試在 Edit->Preferences 菜單中設置，如果不生效，則在 ~/.bashrc 文件中添加如下內容，其中 80 對應不同的透明度。

{% highlight bash %}
if [ -n "$WINDOWID" ]; then
    TRANSPARENCY_HEX=$(printf 0x%x $((0xffffffff * 80 / 100)))
    xprop -id "$WINDOWID" -f _NET_WM_WINDOW_OPACITY 32c -set _NET_WM_WINDOW_OPACITY "$TRANSPARENCY_HEX"
fi
{% endhighlight %}

另外，可以將 bash 替換為 zsh 。
{% highlight text %}
# chsh -s /bin/zsh     # 修改默認shell
{% endhighlight %}

guake，一個不錯的彈出式終端工具，很酷，不過感覺不太實用。

### 繪圖軟件

inkscape 用於繪製矢量圖，另一個比較簡單的是 [xfig](http://www.xfig.org/userman/) ，一款 old style 的畫圖工具。

{% highlight text %}
# yum install inkscape
{% endhighlight %}

另外一個就是 GIMP，同樣可以通過 YUM 安裝，方式同上。


### 虛擬機

常用的是 VirtualBox，可以直接從 [www.virtualbox.org](https://www.virtualbox.org/) 上下載相應的安裝包，也就是 CentOS 的版本，然後通過如下方式安裝。

{% highlight text %}
# yum install kernel-devel                      # 編譯內核模塊時需要該包
# rpm -ivh VirtualBox-x.x.x.rpm                 # 安裝
# /sbin/vboxconfig                              # 重新編譯內核模塊
{% endhighlight %}

安裝時可以直接參考網上的文章。


### 瀏覽器設置

主要包括了比較常用的 FireFox 以及 Chrome，其中前者是默認安裝的。

#### Firefox

可以安裝常用的插件，如 Regular Expressions Tester (一個正則表達式的測試工具)、Vimperator (將對火狐的部分操作改為VIM模式)、Mozilla Archive Format (用來保存查看mhtml格式的文件)。

#### Chrome

可以參考 [Google Chrome 51 Released – Install on RHEL/CentOS 7/6 and Fedora 23-15](http://www.tecmint.com/install-google-chrome-on-redhat-centos-fedora-linux/)


### CHM 閱讀器

Linux 中常見的 chm 閱讀器有 xchm、kchmiewer 等，在 CentOS 可以直接安裝 xchm 。

{% highlight text %}
# yum --enablerepo=nux-dextop,epel install xchm
{% endhighlight %}



### 音頻/視頻軟件

在 CentOS 中，默認使用的是 Rythmbox/Totem，不過感覺使用有點麻煩，還是用 Mplayer/Audacious 比較方便，不過需要依賴 nux-dextop 源，當然也可以從 [pkgs.org](http://pkgs.org/search/) 上下載相關的二進制文件。

{% highlight text %}
# yum --enablerepo=nux-dextop install mplayer audacious plugins-freeworld-mp3
{% endhighlight %}

對於 Mplayer，如果使用時無法縮放，可以在 ~/.mplayer/config 中添加 zoom=yes 配置項。

其中 plugins-freeworld-mp3 是 Audacious 中的 MP3 解碼器。不過默認的外觀不太好看，不過還好支持其它主題，可以從 [gnome-look.org](http://gnome-look.org) 中的 XMMS Themes 中選擇主題，保存在 /usr/share/audacious/Skins 目錄下，然後可以從 Audacious 的 Settings 窗口中看到。

對於中文，在主窗口中右擊，選擇 Settings->Playlist->Compalibility[Fallback...]，設置為 cp936 (比其它的要更通用)，重新加載播放列表即可。

另外，除上述的 GUI 播放器之外，還有些終端播放器，如 [Console Music](https://github.com/cmus/cmus)、[Music On Console](https://moc.daper.net/) ([Github](https://github.com/sagitter/moc)) 。




### 筆記類

比較悲劇，Linux 下面沒有發現很好用的筆記軟件，其中 WizNote 算是比較好用的，不過還是有 BUGs 。

#### tagspace

直接從官網 [www.tagspaces.org](http://www.tagspaces.org/) 下載，然後解壓直接運行即可。

#### 為知筆記

一個跨平臺的筆記 [www.wiz.cn](http://www.wiz.cn/)，安裝方法可以查看 [GitHub](https://github.com/wizteam/wizqtclient)，可以查看相關的開源依賴 [為知筆記中使用的開源組件和協議](http://blog.wiz.cn/wiznote-opensource.html) 。

#### MarkDown

一些網頁的工具， [StackEdit](https://stackedit.io/editor)、[Markable](https://markable.in/)、[Cmd Markdown](https://www.zybuluo.com/mdeditor)、[MaHua](http://mahua.jser.me/)、[馬克飛象](https://maxiang.io/) 。

#### Haroopad

一個 MarkDown 軟件，使用 Chromium 作為 UI，可以參考 [官方文檔](http://pad.haroopress.com/) 。

<!--
$ tar -zxvf haroopad-v0.12.2_amd64.tar.gz
$ tar -zxvf data.tar.gz
# cp -r --link usr /usr                         # 創建硬鏈接
$ tar -zxvf control.tar.gz
# ./postinst
# vi /usr/share/applications/Haroopad.desktop   # 修改ICON
... ...
Icon=/usr/share/icons/hicolor/128x128/apps/haroopad.png
... ...
-->

#### 其它

另外，兩個在 Mac 上很經典的軟件 Mou 以及 [MacDown](http://macdown.uranusjr.com/features/)，對於 MacDown 源碼可以參考 [Github](https://github.com/MacDownApp/macdown) 。


## 參考

官方的鏡像列表，可以參考 [List of CentOS Mirrors](https://www.centos.org/download/mirrors/) 。

<!--
gpg簽名
/etc/pki/rpm-gpg/RPM*
rpm 安裝時可能會報 NOKEY 的錯誤信息 --nogpgcheck nosignature
-->

{% highlight text %}
{% endhighlight %}
