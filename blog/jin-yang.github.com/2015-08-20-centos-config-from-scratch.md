---
title: CentOS 安装与配置
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,centos,安装,配置
description: CentOS (Community Enterprise Operating System) 也即社区版的企业操作系统，是 Linux 的发行版本之一，来自于 Red Hat Enterprise Linux 所开放的源码编译而成。因此，对稳定性较高的服务器通常以 CentOS 替代商业版的 Red Hat 。两者的不同在于 CentOS 并不包含封闭源代码软件。下面会介绍桌面版的 CentOS 在安装时需要作的一些常用配置。
---

CentOS (Community Enterprise Operating System) 也即社区版的企业操作系统，是 Linux 的发行版本之一，来自于 Red Hat Enterprise Linux 所开放的源码编译而成。因此，对稳定性较高的服务器通常以 CentOS 替代商业版的 Red Hat 。

两者的不同在于 CentOS 并不包含封闭源代码软件。下面会介绍桌面版的 CentOS 在安装时需要作的一些常用配置。

<!-- more -->

![centos logo]({{ site.url }}/images/linux/centos-logo.png "centos logo"){: .pull-center }

## 安装 CentOS

对于 CentOS 桌面版的安装也比较简单，完全是基于图形界面的，安装时可以从 [www.centos.org](https://www.centos.org/download/) 上下载相应版本，一般用 LiveGNOME 或者 LiveKDE 。

当然，也可以从国内的一些镜像网站上下载，例如 [阿里云镜像](http://mirrors.aliyun.com/)、[搜狐镜像](http://mirrors.sohu.com/)、[网易镜像](http://mirrors.163.com/) 上下载。

下载了 ISO 镜像之后可以通过如下方式安装到 U 盘中，如果有 Linux 发行版，安装过程将很简单；如下是通过 dd 命令直接复制即可。

{% highlight text %}
----- 查看U盘的设备号
# fdisk -l
... ...
Disk /dev/sdc: 4002 MB, 4002910208 bytes, 7818184 sectors
... ...

----- 将ISO镜像原样复制到U盘中
# dd if=xxx.iso of=/dev/sdb

----- 从另外一个终端执行，查看复制到U盘的进度
# while killall -USR1 dd; do sleep 5; done
{% endhighlight %}

接下来就看看如何配置一些常用的环境。


## 软件安装

![centos power logo]({{ site.url }}/images/linux/centos-power-logo.jpg "centos power logo"){: .pull-right }

在 CentOS 中，会通过 YUM 或者 RPM 进行软件包的管理。其中 RPM (Redhat Package Manager) 是由 RedHat 公司提供的软件包管理器，可以实现软件包的安装、查询、卸载、升级以及校验等，相关的数据会保存在 /var/lib/rpm 目录下，但 rpm 不能很好的解决软件之间的依赖关系。

<!--
There are five basic modes for RPM command
    Install : It is used to install any RPM package.
    Remove : It is used to erase, remove or un-install any RPM package.
    Upgrade : It is used to update the existing RPM package.
    Verify : It is used to query about different RPM packages.
    Query : It is used for the verification of any RPM package.-->

在安装时，通常系统文件存放在 /bin、/sbin、/lib 目录下，而第三方软件会存放在 /usr/local/bin、/usr/local/sbin、 /usr/local/lib、/usr/share/man 和 /usr/local/share/man (后两者为说明文件) 。

RPM 包通常有一个通用的命名方式：name-version-release.arch.rpm 。

* name: 表示包的名称，包括主包名和分包名；
* version: 表示包的版本信息；
* release: 用于标识 rpm 包本身的发行号，可还包含适应的操作系统；
* arch: 表示主机平台，noarch 表示此包能安装到所有平台上面。

如 gd-devel-2.0.35-11.el6.x86_64.rpm ，gd 是这个包的主包名；devel 是这个包的分包名； 2.0.35 是表示版本信息，2 为主版本号，0 表示次版本号，35 为源码包的发行号也叫修订号； 11.el6 中的 11 是 rpm 的发行号， el6 表示 RHEL6； x86_64 是表示适合的平台。

为了解决各个包之间的依赖关系，可以采用 YUM (Yellowdog Updater Modified) 进行管理，该软件是由 Seth Vidal 开发，用于管理 RPM 包。

### 常用命令

如下列举一些常用的命令。

{% highlight text %}
----- 查看软件信息
# rpm -q kernel                                           // 查看安装包的全名
# rpm -ql kernel                                          // 查看已安装软件包含有的文件
# rpm -qi kernel                                          // 查看已安装软件包的摘要信息
# rpm -qa | grep software                                 // 查看是否安装了software
# rpm -qf /usr/sbin/ifcfg                                 // 查看某文件属于那个包
# rpm -qc iproute                                         // 查看生成了那些配置文件
# rpm -qd iproute                                         // 查看生成了那些帮助文件
# rpm -qpi iproute.rpm                                    // 查看rpm包的信息
# rpm -qpl iproute.rpm                                    // 查看rpm包中含有的软件列表
# yum info mysql                                          // 查看包信息

----- 搜索软件
# yum search package                                      // 搜索package

----- 安装软件
# rpm --checksig foobar.rpm                               // 校验PGP Signature，查看完整性和来源
# rpm -qpR foobar.rpm                                     // 查看依赖
# rpm -ivh --nodeps --force foobar.rpm                    // 强制安装，如果其它软件包未安装，则不能工作
# yum -y install foobar                                   // 默认为 yes ，通常用于脚本文件，不需要交互

----- 只下载软件，如下命令包含在yum-utils包中
# yumdownloader PACK                                      // 使用yum自带软件
# yum -y install yum-downloadonly                         // 通过yum-downloadonly插件
# yum -y install --downloadonly --downloaddir=/tmp/ PACK  // 直接使用

----- 安装软件包群，很多被打包成Group的软件，可以通过如下命令查看、安装
# yum grouplist
# yum groupinstall 'MySQL Database'
# yum groupupdate 'MySQL Database'
# yum groupremove 'MySQL Database'

----- 卸载软件
# rpm -evv --nodeps foobar                                // 不需要指定到foobar.rpm
# yum -y remove foobar

----- 升级软件包
# rpm -Uvh foobar.rpm                                     // 如果新版本不工作，仍可以使用之前的版本
# yum check-update                                        // 查看可升级的软件包
# yum update foobar                                       // 升级特定的软件
# yum update                                              // 升级所有软件

----- 清除/更新Cache
# yum clean all                                           // 默认会保存在/var/cache/yum
# yum makecache                                           // 通常在修改/etc/yum/repos.d目录下配置文件之后

----- 获得源码，需要安装yum-utils工具，提供了yumdownloader
# vi /etc/yum.repos.d/CentOS-Source.repo                  // 将enabled设置为1
# yum clean all                                           // 清空缓存
# yum makecache                                           // 使修改生效
# rpm -qf `which mysql`                                   // 查看对应安装包
# yumdownloader --source mariadb                          // 下载源码包，不加--source则只下载
# rpm2cpio coreutils-8.4-19.el6_4.2.src.rpm | cpio -ivd   // 解压rpm包

----- 列出软件
# yum list | less                                         // 列出所有可安装的软件包
# yum list updates                                        // 列出所有可更新的软件包
# yum list installed                                      // 列出所有已安装的软件包
# yum list extras                                         // 列出所有已安装但不在 Yum Repository 內的软件包
# yum list [package]                                      // 列出所指定的软件包

----- 其它
# yum shell                                               // 交互环境，可以执行多条命令
# yum history                                             // 查看历史
# rpm -Vp xxx.rpm                                         // 与数据库中的版本校验 (Verify)
# rpm -Va                                                 // 校验所有的
# rpm --import /etc/pki/rpm-gpg/RPM-GPG-KEY-CentOS-6      // 导入 GPG key
----- 查询未安装软件包的依赖关系
$ rpm -qRp vim-common-6.3.046-2.el4.1.x86_64.rpm
----- 查询已安装软件包的依赖关系
$ rpm -qR vim-common-6.3.046-2.el4.1

----- 数据库出错，通过下述命令重建
# rm /var/lib/rpm/__db*
# rpm --rebuilddb
# rpmdb_verify Packages
{% endhighlight %}

yum makecache 会把服务器的包信息下载到本地电脑缓存起来，配合 yum -C search xxx 使用，不用上网检索就能查找软件的相关信息。

### 问题排查

如果在通过 yum 安装软件时出现，Another app is currently holding the yum lock 错误，可以通过 cat /var/run/yum.pid 查看占用的进程的 PID ，通过 rm -f /var/run/yum.pid 删除，重新安装。



## 源配置

CentOS 中官方的源只包含了有限的安装包，为此需要安装一些常用的三方源。当然，也可以自己手动创建 Yum 仓库，主要采用了 createrepo，用于生成 repodata 软件信息仓库。

可以通常如下命令查看源列表的配置。

{% highlight text %}
# yum repolist                                            // 查看现在可用的源列表
# yum repolist all                                        // 查看所有的源列表
# yum --enablerepo=epel install mysql                     // 从指定源列表下载
{% endhighlight %}

接下来看看如何配置第三方的源。

### 使用第三方源

配置完成后可以通过 yum repolist all 命令查看三方源是否生效，通过 yum list software 查看相应的软件。对于一些 RPM 包，也可以从 [pkgs.org/search/](http://pkgs.org/search/) 或者 [http://www.rpmfind.net](http://www.rpmfind.net) 查找相应的软件包，以及与该软件包相关的依赖。

注意：如果在安装过程中出现 error: Failed dependencies 可以使用 --nodeps --force 选项强制安装，不过此时有可能导致部分功能失效。

另外，关于 CentOS 的第三方源，可以参考 [CentOS Wiki](http://wiki.centos.org/zh/AdditionalResources/Repositories) 中给出的参考意见。

#### 准备工作

在使用时，最好先安装 yum-priorities 插件，该插件用来设置 yum 在调用软件源时的顺序，因为官方提供的软件源，都是比较稳定和被推荐使用的，因此，官方源的顺序要高于第三方源的顺序。

{% highlight text %}
# yum install yum-priorities
{% endhighlight %}

安装完后需要设置 /etc/yum.repos.d/ 目录下的 *.repo 相关文件，例如 CentOS-Base.repo、epel.repo、nux-dextop.repo 等，在这些文件中插入顺序指令 priority=N (N为1到99的正整数，数值越小优先级越高)，一般第三的软件源设置的优先级大于 10 。


#### 国内官方源

也就是一些 CentOS 的镜像，常见的有 [centos.ustc.edu.cn](http://centos.ustc.edu.cn/)、[mirrors.163.com](http://mirrors.163.com/centos/)、[mirrors.sohu.com](http://mirrors.sohu.com/centos/)，只需要修改基本数据源中的 URL 配置选项。


#### EPEL, Extra Packages for Enterprise Linux

EPEL是由 Fedora 社区打造，为 RHEL 及衍生发行版如 CentOS、Scientific Linux 等提供高质量软件包的项目，详细内容可以参考 [EPEL-Wiki](https://fedoraproject.org/wiki/EPEL/zh-cn) 。

安装源，其中对应的版本需要根据当前的版本自行选择。

{% highlight text %}
# rpm -Uvh http://mirrors.ustc.edu.cn/epel/beta/7/x86_64/epel-release-7-0.2.noarch.rpm
# rpm -Uvh http://dl.fedoraproject.org/pub/epel/beta/7/x86_64/epel-release-7-0.2.noarch.rpm
{% endhighlight %}

接下来时导入证书，当然这步也可以在通过 yum 安装时根据提示自动导入。

{% highlight text %}
# rpm -import /etc/pki/rpm-gpg/RPM-GPG-KEY-EPEL-7
{% endhighlight %}


#### rpmforge

可以从 [pkgs.repoforge.org](http://pkgs.repoforge.org/rpmforge-release/) 或者 [apt.sw.be](http://apt.sw.be/redhat/el7/en/x86_64/rpmforge/RPMS/) 下载。

{% highlight text %}
# rpm -Uvh http://apt.sw.be/redhat/el7/en/x86_64/rpmforge/RPMS/rpmforge-release-xxx.rpm
{% endhighlight %}

#### nux-dextop

直接从 [nux-dextop-release*rpm](http://li.nux.ro/download/nux/dextop/el7/x86_64/) 上查找安装最新的配置，通常 mplayer 会包含在该三方源中。



### 使用本地源

可以通过如下方法使用本地源，也就是下载的包含安装包的 ISO 镜像。

对于 VMware 需要通过如下方法挂载，[Setting] -> [Hardware] -> [CD/DVD] -> 右边 Device 里勾选 Connected，在 Use ISO image file 里选择 ISO 文件后确定即可。

CentOS 中使用的 yum 源配置文件保存在 /etc/yum.repos.d 目录下，主要包括了两个配置文件 CentOS-Base.repo 和 CentOS-Media.repo；其中，前者是配置网络 yum 源的，而后者是用来配置本地 yum 源。

{% highlight text %}
----- 对于VM来说，也可以使用Share Folder
# mount /dev/cdrom /media/cdrom
# mount -o loop /mnt/hgfs/Share/CentOS-xxx-xxx-bin-DVD.iso /media/cdrom/
# vi /etc/yum.repos.d/iso.repo
[c6-media]
name=CentOS-$releasever - Media      # 自定义名称
baseurl=file:///media/cdrom/         # 可以指定多个路径
        file:///media/cdrom2/
gpgcheck=1
enabled=0
gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-CentOS-6

----- 清除缓存
# yum clean all

----- 证书也可以使用如下方式安装
# rpm --import /etc/pki/rpm-gpg/RPM-GPG-KEY-CentOS-5
# yum --enablerepo=c6-media install mysql-server                   # 本地和网络
# yum --disablerepo=\* --enablerepo=c6-media install mysql-server  # 只使用本地光盘
{% endhighlight %}

#### 原生本地源

如上的 ISO 镜像文件中，已经包含了索引文件 (位于 repodata 文件夹)，如果自己创建本地镜像，例如平时收藏的 RPM 软件包或者集成测试等，此时需要通过 createrepo 创建索引文件。

当然，如过没有 createrepo 命令，则需要安装 createrepo.xxx.rpm 包。

{% highlight text %}
----- 1. 创建本地yum仓库目录
$ mkdir -p /share/CentOS/7/local/x86_64/RPMS

----- 2. 创建索引&更新缓存
$ createrepo /share/CentOS/7/local/x86_64
$ yum makecache

----- 3. 创建本地repo文件
$ cat<<-"EOF">/etc/yum.repos.d/CentOS-Local.repo
[local]
name=CentOS-$releasever - local packages for $basearch
baseurl=file:///share/CentOS/$releasever/local/$basearch
enabled=1
gpgcheck=0
protect=1
EOF
{% endhighlight %}

另外，可以参考 [How to create public mirrors for CentOS](https://wiki.centos.org/HowTos/CreatePublicMirrors)、[Create Local Repos](https://wiki.centos.org/HowTos/CreateLocalRepos) 。


## 常用软件配置

### 安装编译环境

如下是安装 C/C++ 编译工具。

{% highlight text %}
# yum install gcc
# yum install gcc-c++             # 安装g++
{% endhighlight %}

### 支持 NTFS 文件系统

可以在安装完第三方软件源 EPEL 之后通过如下命令安装。

{% highlight text %}
# wget -O /etc/yum.repos.d/epel.repo http://mirrors.aliyun.com/repo/epel-7.repo
# yum update

# yum --enablerepo=epel install ntfs-3g
{% endhighlight %}

但是如果是最新版本的 CentOS，可能在三方库中没有相应的安装包，那么此时就需要从源码编译安装。直接从 [www.tuxera.com](http://www.tuxera.com/community/ntfs-3g-download/) 下载源码，然后通过如下方式编译。

{% highlight text %}
$ tar -zxvf ntfs-3g_ntfsprogs-2014.2.15.tgz
$ cd ntfs-3g_ntfsprogs-2014.2.15
$ ./configure
$ make
# make install                # 完成安装，需要root权限
{% endhighlight %}

接下来配置为自动挂载，假设对应的磁盘为 /dev/sda5 。

{% highlight text %}
# blkid /dev/sda5             # 查看UUID
# vi /etc/fstab               # 添加如下的内容
UUID=xxxxxxxxxxx /media/disk  ntfs-3g defaults 0 0
{% endhighlight %}


### 安装 Flash 插件

直接从如下网站 [www.adobe.com](http://get.adobe.com/cn/flashplayer/) 获得，主要有两种方法。

A) YUM 安装。下载时选择 "YUM，适用于Linux(YUM)"，实际会下载一个 RPM 安装包，用来安装 adobe 的 yum 源配置文件。

{% highlight text %}
# rpm -ivh adobe-release-i386-1.0-1.noarch.rpm        安装Adobe的源
# yum install flash-plugin                            安装falsh插件
{% endhighlight %}

B）RPM 安装。在下载页面选择 ".rpm，适用于其它Linux"，此时将会直接下载 RPM 安装包，可以直接通过 RPM 进行安装。

{% highlight text %}
# rpm -ivh flash-plugin-11.2.202.297-release.i386.rpm
{% endhighlight %}

### Gnome-Terminal配置

A) 取消声音。Edit->Preferences->Profiles->选择对应配置文件[Edit]->General->取消Terminal bell。

B) 设置启动快捷键。在 CentOS 的系统菜单中选择 Applications -> System Tools -> Keyboard -> Shortcuts -> Custom Shortcuts 设置命令为 gnome-terminal --hide-menubar --maximize，详细参数可以参考 gnome-terminal --help-window-options。

C) 颜色设置。个人比较喜欢的颜色配置，文本颜色 #dbfef8，背景颜色 #2f4f4f 。

D) 设置为半透明。首先尝试在 Edit->Preferences 菜单中设置，如果不生效，则在 ~/.bashrc 文件中添加如下内容，其中 80 对应不同的透明度。

{% highlight bash %}
if [ -n "$WINDOWID" ]; then
    TRANSPARENCY_HEX=$(printf 0x%x $((0xffffffff * 80 / 100)))
    xprop -id "$WINDOWID" -f _NET_WM_WINDOW_OPACITY 32c -set _NET_WM_WINDOW_OPACITY "$TRANSPARENCY_HEX"
fi
{% endhighlight %}

另外，可以将 bash 替换为 zsh 。
{% highlight text %}
# chsh -s /bin/zsh     # 修改默认shell
{% endhighlight %}

guake，一个不错的弹出式终端工具，很酷，不过感觉不太实用。

### 绘图软件

inkscape 用于绘制矢量图，另一个比较简单的是 [xfig](http://www.xfig.org/userman/) ，一款 old style 的画图工具。

{% highlight text %}
# yum install inkscape
{% endhighlight %}

另外一个就是 GIMP，同样可以通过 YUM 安装，方式同上。


### 虚拟机

常用的是 VirtualBox，可以直接从 [www.virtualbox.org](https://www.virtualbox.org/) 上下载相应的安装包，也就是 CentOS 的版本，然后通过如下方式安装。

{% highlight text %}
# yum install kernel-devel                      # 编译内核模块时需要该包
# rpm -ivh VirtualBox-x.x.x.rpm                 # 安装
# /sbin/vboxconfig                              # 重新编译内核模块
{% endhighlight %}

安装时可以直接参考网上的文章。


### 浏览器设置

主要包括了比较常用的 FireFox 以及 Chrome，其中前者是默认安装的。

#### Firefox

可以安装常用的插件，如 Regular Expressions Tester (一个正则表达式的测试工具)、Vimperator (将对火狐的部分操作改为VIM模式)、Mozilla Archive Format (用来保存查看mhtml格式的文件)。

#### Chrome

可以参考 [Google Chrome 51 Released – Install on RHEL/CentOS 7/6 and Fedora 23-15](http://www.tecmint.com/install-google-chrome-on-redhat-centos-fedora-linux/)


### CHM 阅读器

Linux 中常见的 chm 阅读器有 xchm、kchmiewer 等，在 CentOS 可以直接安装 xchm 。

{% highlight text %}
# yum --enablerepo=nux-dextop,epel install xchm
{% endhighlight %}



### 音频/视频软件

在 CentOS 中，默认使用的是 Rythmbox/Totem，不过感觉使用有点麻烦，还是用 Mplayer/Audacious 比较方便，不过需要依赖 nux-dextop 源，当然也可以从 [pkgs.org](http://pkgs.org/search/) 上下载相关的二进制文件。

{% highlight text %}
# yum --enablerepo=nux-dextop install mplayer audacious plugins-freeworld-mp3
{% endhighlight %}

对于 Mplayer，如果使用时无法缩放，可以在 ~/.mplayer/config 中添加 zoom=yes 配置项。

其中 plugins-freeworld-mp3 是 Audacious 中的 MP3 解码器。不过默认的外观不太好看，不过还好支持其它主题，可以从 [gnome-look.org](http://gnome-look.org) 中的 XMMS Themes 中选择主题，保存在 /usr/share/audacious/Skins 目录下，然后可以从 Audacious 的 Settings 窗口中看到。

对于中文，在主窗口中右击，选择 Settings->Playlist->Compalibility[Fallback...]，设置为 cp936 (比其它的要更通用)，重新加载播放列表即可。

另外，除上述的 GUI 播放器之外，还有些终端播放器，如 [Console Music](https://github.com/cmus/cmus)、[Music On Console](https://moc.daper.net/) ([Github](https://github.com/sagitter/moc)) 。




### 笔记类

比较悲剧，Linux 下面没有发现很好用的笔记软件，其中 WizNote 算是比较好用的，不过还是有 BUGs 。

#### tagspace

直接从官网 [www.tagspaces.org](http://www.tagspaces.org/) 下载，然后解压直接运行即可。

#### 为知笔记

一个跨平台的笔记 [www.wiz.cn](http://www.wiz.cn/)，安装方法可以查看 [GitHub](https://github.com/wizteam/wizqtclient)，可以查看相关的开源依赖 [为知笔记中使用的开源组件和协议](http://blog.wiz.cn/wiznote-opensource.html) 。

#### MarkDown

一些网页的工具， [StackEdit](https://stackedit.io/editor)、[Markable](https://markable.in/)、[Cmd Markdown](https://www.zybuluo.com/mdeditor)、[MaHua](http://mahua.jser.me/)、[马克飞象](https://maxiang.io/) 。

#### Haroopad

一个 MarkDown 软件，使用 Chromium 作为 UI，可以参考 [官方文档](http://pad.haroopress.com/) 。

<!--
$ tar -zxvf haroopad-v0.12.2_amd64.tar.gz
$ tar -zxvf data.tar.gz
# cp -r --link usr /usr                         # 创建硬链接
$ tar -zxvf control.tar.gz
# ./postinst
# vi /usr/share/applications/Haroopad.desktop   # 修改ICON
... ...
Icon=/usr/share/icons/hicolor/128x128/apps/haroopad.png
... ...
-->

#### 其它

另外，两个在 Mac 上很经典的软件 Mou 以及 [MacDown](http://macdown.uranusjr.com/features/)，对于 MacDown 源码可以参考 [Github](https://github.com/MacDownApp/macdown) 。


## 参考

官方的镜像列表，可以参考 [List of CentOS Mirrors](https://www.centos.org/download/mirrors/) 。

<!--
gpg签名
/etc/pki/rpm-gpg/RPM*
rpm 安装时可能会报 NOKEY 的错误信息 --nogpgcheck nosignature
-->

{% highlight text %}
{% endhighlight %}
