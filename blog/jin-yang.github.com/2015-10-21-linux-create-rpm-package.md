---
title: RPM 包制作
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,rpm,制作
description: RPM Package Manager 简称 RPM，这一文件格式在历史名称上虽然打着 RedHat 标志，但其原始设计理念是开放式的，现在包括 OpenLinux、SuSE、Turbo Linux 等多个 Linux 分发版本都有采用，可以算是公认的行业标准了。在此介绍下如何制作 RPM 包，尤其是如何写 .spec 配置文件，以及常见的技巧。
---

RPM Package Manager 简称 RPM，这一文件格式在历史名称上虽然打着 RedHat 标志，但其原始设计理念是开放式的，现在包括 OpenLinux、SuSE、Turbo Linux 等多个 Linux 分发版本都有采用，可以算是公认的行业标准了。

在此介绍下如何制作 RPM 包，尤其是如何写 .spec 配置文件，以及常见的技巧。

<!-- more -->

## 简介

若要构建一个标准的 RPM 包，需要创建 .spec 文件，其中包含软件打包的全部信息。然后，使用 rpmbuild 命令，按照 spec 文件的配置，系统会按照步骤生成最终的 RPM 包。

另外，需要注意的是，在使用时，需要使用普通用户，一定不要用 root 用户。

结下来，看看如何编译制作 RPM 包。

## 环境配置

接下来，看看如何配置一步步制作 RPM 包。

### 安装工具

首先，在 CentOS 中配置环境。

{% highlight text %}
----- 安装环境，需要rpm-build工具来打包，该包只依赖于gdb以及rpm版本
# yum install rpm-build -y

----- 新建所需目录
$ mkdir -pv ~/rpm-maker/{BUILD,BUILDROOT,RPMS,SOURCES,SPECS,SRPMS}
{% endhighlight %}

### 配置环境

打包相关的配置保存在宏文件 (macrofiles) 中，默认使用 ```$HOME/rpmbuild``` 目录，用户配置文件保存在 ```$HOME/.rpmmacros```；而制作包时的操作实际都在 topdir 指定的目录下，可以通过如下方式查看。

{% highlight text %}
--- 查看所有的配置文件顺序
$ rpmbuild --showrc | grep macros

--- 确认上述的根工作目录，两种方式相同
$ rpmbuild --showrc | grep ': _topdir'
$ rpm --eval '%_topdir'

--- 设置topdir变量，然后再次验证下，确认已经修改为目标目录
$ echo "%_topdir %{getenv:HOME}/rpm-maker" > ~/.rpmmacros
$ rpm --eval '%_topdir'
--- 也可以在制作rpm包时通过--define指定
$ rpmbuild --define '_topdir rpm-maker' -ba package.spec
{% endhighlight %}

下载 MySQL 的源码，并保存在 SPECS 目录下，整体目录结构如下。

{% highlight text %}
$ tree ~/rpm-maker
.
|-- BUILD                      ← 解压后的tar.gz包
|-- BUILDROOT                  ← 临时安装目录，会打包该目录下文件
|-- RPMS                       ← 编译后的RPM包
|-- SOURCES                    ← 所需的文件
|   `-- mysql-xxx.tar.gz       ← 包括了源码包
|-- SPECS                      ← *.spec编译脚本
|   |-- mysql-xxx.spec
|   `-- mysql-yyy.spec
`-- SRPMS                      ← src格式的rpm包存放的位置
{% endhighlight %}

### 执行命令

接下来，看看如何生成 rpm 包，也就是使用 rpmbuild 命令，过程可以分为多个阶段。

{% highlight text %}
$ rpmbuild --bb mysql.spec   

使用参数：
   -bb 制作成二进制RPM包
   -bs 制作源码的RPM包
   -ba 源码和二进制两种形式的RPM包
   -bl 检测BUILDROOT没有包含到rpm包中的文件

   -bp 执行到pre
   -bc 执行到build段
   -bi 执行install段

   --quiet 默认会输出每次执行的shell命令，此时忽略

----- 通过源码包重新编译
$ rpmbuild --rebuild package-0.0-0.src.rpm

----- 编译时指定参数
$ rpmbuild -bb SPECS/librdkafka.spec --define "__version 0.9.4" --define "__release 1"
{% endhighlight %}

一般来说，执行的顺序为 `rpmbuild -bp`，再 `-bc` 再 `-bi`，如果没问题，`rpmbuild -ba` 生成 src 包与二进制包，使用时通过 `rpm -ivh xxx.rpm;` 以及 `rpm -e xxx.rpm` 进行安装卸载。

### 执行步骤

{% highlight text %}
1. 在 %prep 段中，通过 %setup -q 宏解压、打补丁，一般是从 SOURCES 目录下解压到 BUILD 目录下，一般目录是 "NAME-VERSION" 。
2. 通过 %build 段定义了如何进行编译，编译目录就是上述的 BUILD/NAME-VERSION 。
   2.1 首先通过 %configure %cmake 进行配置。
   2.2 然后利用 %{__make} %{?_smp_mflags} 进行并行编译。
3. 接着就是安装，也就是 %install 字段，一般会在 BUILDROOT/NAME-VERSION-RELEASE-ARCH 目录下。
   3.1 通常为了清理环境会先使用 rm -rf %{buildroot} 清理。
   3.2 接着通过 %{__make} install DESTDIR=%{buildroot} 命令进行安装，其中 DESTDIR 相当于是根目录了。
   3.3 通过上步安装的文件，需要都打包到 RPM 包中，如果不需要那么就先清理掉。
   3.4 编译没有生成的也可以通过 %{__install} %{__mv} %{__rm} 命令直接复制。
4. 执行检查规范，对应了 %test 段，一般执行一些单元测试。
5. 开始打包
6. 在 %clean 段处理清理操作，通常会通过 rm -rf %{buildroot} 删除编译的中间内容。
{% endhighlight %}

另外，还有 `%check` 字段，用于做些检查。

<!--
%check
ctest -V %{?_smp_mflags}
%{!?el5:-N}
-->

## SPEC 文件

接下来准备 spec 文件，也是核心的内容，该文件包括三部分：介绍部分，编译部分，files 部分。接下来，是一个简单的示例，可以看看到底是如何制作 RPM 包的。

{% highlight text %}
### 0.define section                ← ###自定义宏段(可选)，方便后续引用
%define mysql_user mysql                            ← 定义宏，方便后续引用
%define _topdir /home/jinyang/databases/rpm-maker

### 1.The introduction section      ← ###介绍区域段
Name:           mysql                               ← 包名称，通常会在指定源码压缩包时引用
Version:        5.7.17                              ← 版本号，同上
Release:        1%{?dist}                           ← 释出号，每次制作rpm包时递增该数字
Summary:        MySQL from FooBar.                  ← 软件包简介，最好不要超过50字符
License:        GPLv2+ and BSD                      ← 许可，GPL、BSD、MIT等，也可以使用or

Group:          Applications/Databases              ← 程序组名，从/usr/share/doc/rpm-VER/GROUPS选择
URL:            http://kidding.com                  ← 一般为官网
Source0:        %{name}-boost-%{version}.tar.gz     ← 源码包名称(可以使用URL)，可以用SourceN指定多个，如配置文件
#Patch0:         some-bugs.patch                    ← 如果需要打补丁，则依次填写
BuildRequires:  gcc,make                            ← 制作过程中用到的软件包
Requires:       pcre,pcre-devel,openssl,chkconfig   ← 安装时所需软件包，可使用bash >= 1.1.1
Requires(pre):  test                                ← 指定不同阶段的依赖

BuildRoot:      %_topdir/BUILDROOT                  ← 会打包该目录下文件，可查看安装后文件路径
Packager:       FooBar <foobar@kidding.com>
Vendor:         kidding.com

%description                        ← ###软件包的详细描述，可以撒丫子写了
It is a MySQL from FooBar.

#--- 2.The Prep section             ← ###准备阶段，主要是解压源码，并切换到源码目录下
%prep
%setup -q                                           ← 宏的作用是解压并切换到目录
#%patch0 -p1                                        ← 如果需要打补丁，则依次写

#--- 3.The Build Section            ← ###编译制作阶段，主要目的就是编译
%build

make %{?_smp_mflags}                                ← 多核则并行编译

#--- 4.Install section              ← ###安装阶段
%install
if [-d %{buildroot}]; then
   rm -rf %{buildroot}                              ← 清空下安装目录，实际会自动清除
fi
make install DESTDIR=%{buildroot}                   ← 安装到buildroot目录下
%{__install} -Dp -m0755 contrib/init.d %{buildroot}%{_initrddir}/foobar
%{__install} -d %{buildroot}%{_sysconfdir}/foobar.d/

#--- 4.1 scripts section            ← ###没必要可以不填
%pre                                                ← 安装前执行的脚本
%post                                               ← 安装后执行的脚本
%preun                                              ← 卸载前执行的脚本
%postun                                             ← 卸载后执行的脚本
%pretrans                                           ← 在事务开始时执行脚本
%posttrans                                          ← 在事务结束时执行脚本

#--- 5. Clean section               ← ###清理段，可以通过--clean删除BUILD
%clean
rm -rf %{buildroot}

#--- 6. File section                ← ###打包时要包含的文件，注意同时需要在%install中安装
%files
%defattr (-,root,root,0755)                         ← 设定默认权限
%config(noreplace) /etc/my.cnf                      ← 表明是配置文件，noplace表示替换文件
%doc %{src_dir}/Docs/ChangeLog                      ← 表明这个是文档
%attr(644, root, root) %{_mandir}/man8/mysqld.8*    ← 分别是权限，属主，属组
%attr(755, root, root) %{_sbindir}/mysqld

#--- 7. Chagelog section            ← ###记录SPEC的修改日志段
%changelog
* Fri Dec 29 2012 foobar <foobar@kidding.com> - 1.0.0-1
- Initial version
{% endhighlight %}

对于 ```%config(noreplace)``` 表示为配置文件，且不能覆盖，新安装时如果配置文件存在则会将原文件保存为 `*.rpmsave` ，升级时则不会覆盖原文件，直接将包中的文件命名为 `*.rpmnew`，更详细内容可以参考 [RPM, %config, and (noreplace)](http://people.ds.cam.ac.uk/jw35/docs/rpm_config.html) 。

<!-- /reference/linux/rpm-config-noreplace.maff -->

<!--
###  4.1 scripts section #没必要可以不写 %pre        #rpm安装前制行的脚本 if [ $1 == 1 ];then    #$1==1 代表的是第一次安装，2代表是升级，0代表是卸载         /usr/sbin/useradd -r nginx 2> /dev/null  ##其实这个脚本写的不完整fi %post       #安装后执行的脚本  %preun      #卸载前执行的脚本 if [ $1 == 0 ];then         /usr/sbin/userdel -r nginx 2> /dev/null fi %postun     #卸载后执行的脚本  ###  5.clean section 清理段,删除buildroot  %clean rm -rf %{buildroot}      ###  6.file section 要包含的文件 %files  %defattr (-,root,root,0755)   #设定默认权限，如果下面没有指定权限，则继承默认 /etc/           #下面的内容要根据你在%{rootbuild}下生成的来写     /usr/ /var/      ###  7.chagelog section  改变日志段 %changelog *  Fri Dec 29 2012 laoguang <ibuler@qq.com> - 1.0.14-1 - Initial version

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


关于脚本部分，通常所有安装脚本和触发器脚本都是使用 ```/bin/sh``` 来执行的，如果想使用其他脚本，如 Lua，可使用 ```-p /usr/bin/lua``` 来告诉 rpm 使用 lua 解释器。如

{% highlight text %}
%post -p /usr/bin/lua
# lua script here
%postun -p /usr/bin/perl
$ perl sciprt here
{% endhighlight %}

另外，如果只执行一条命令，也使用 ```-p``` 选项可直接执行，如 ```%post -p /sbin/ldconfig``` 。

一般最后一条命令的 exit 状态就是脚本的 exit 状态，除一些特殊情况，一般脚本都是以 ```exit 0``` 状态退出，所以大部分小脚本片段都会使用 ```"||:"``` 退出。

### %prep

{% highlight text %}
%setup -q -T -a 0 -a 7 -a 10 -c -n %{src_dir}
参数列表：
    -T 禁止自动解压源码文件
    -D 解压前不删除目录
    -a 切换目录前，解压指定Source文件，例如-a 0表示解压Source0
    -b 切换目录后，解压指定Source文件，例如-a 0表示解压Source0
    -n 解压后目录名称与RPM名称不同，则通过该参数指定切换目录
    -c 解压缩之前先生成目录
{% endhighlight %}

<!--
%patch 最简单的补丁方式，自动指定patch level。
%patch 0 使用第0个补丁文件，相当于%patch ?p 0。
%patch -s 不显示打补丁时的信息。
%patch -T 将所有打补丁时产生的输出文件删除。
-->

### systemd

spec 脚本中提供了与 systemd 相关的脚本，关于脚本的详细内容可以查看 [macros.systemd.in](https://cgit.freedesktop.org/systemd/systemd/tree/src/core/macros.systemd.in)，实用方式很简单，如下：

{% highlight text %}
----- 安装结束后，实际执行systemctl preset，执行服务预设的内容
%systemd_post foobar.service

----- 执行卸载前，systemctl disable
%systemd_preun foobar.service

----- 卸载后执行重启操作，daemon-reload+try-restart
%systemd_postun_with_restart foobar.service

----- 如果不需要重启，例如有状态链接的(D-Bus)，执行systemctl daemon-reload
%systemd_postun foobar.service
{% endhighlight %}

关于安装升级的各个操作步骤详见 [RPM SPEC pre/post/preun/postun argument values](http://meinit.nl/rpm-spec-prepostpreunpostun-argument-values) 以及 [Fedora Packaging Guidelines for RPM Scriptlets](https://fedoraproject.org/wiki/Packaging:Scriptlets) 。

### 校验

在安装完 RPM 包之后，可以通过 ```--verify``` 或者 ```-V``` 进行校验，正常不会显示任何信息，可以通过 ```--verbose``` 或者 ```-v``` 显示每个文件的信息；文件丢失显示 ```missing```，属性方面的修改内容如下：

{% highlight text %}
SM5DLUGT c filename
属性：
  S: 文件大小;
  M: 权限;
  5: MD5检查和;
  D: 主从设备号;
  L: 符号连接;
  U: 属主;
  G: 属组;
  T: 最后修改时间。
类型：
  c: 配置文件；
  d: 文档文件。
{% endhighlight %}

## 增加签名

当制作 RPM 包之后，为了防止被篡改，可以使用私钥进行签名，然后将公钥开放出去，然后用户下载完软件包可以通过公钥进行验证签名，从而确保文件的原始性。

可以通过如下步骤进行操作。

### 1. 生成密钥对

首先，需要使用 gpp 来生成公私钥对，一般可以使用 RSA 非对称加密。

{% highlight text %}
$ gpg --gen-key
{% endhighlight %}

此时会在 `~/.gnupg` 目录下创建相关文件，创建完后可以通过如下命令查看当前的密钥对。

{% highlight text %}
$ gpg --list-keys
/home/foobar/.gnupg/pubring.gpg
------------------------------
pub   2048R/860FB269 2017-01-13
uid                  foobar (just for test) <foobar@example.com>
{% endhighlight %}

其中 UID 分别对应了用户名、注释、邮箱。

<!-- Repository Signer (OSSXP) -->

### 2. 软件包签名

修改 RPM 宏，使用上述生成的密钥对。

{% highlight text %}
$ echo %_signature gpg >> ~/.rpmmacros
$ echo "%_gpg_name foobar (just for test)" >> ~/.rpmmacros
{% endhighlight %}

对已有 rpm 软件包进行签名。

{% highlight text %}
$ rpm --addsign package_name.rpm
Enter pass phrase:
Pass phrase is good.
package_name.rpm
{% endhighlight %}

如果失败，则会出现如下的错误。

{% highlight text %}
$ rpm --addsign package_name.rpm
Enter pass phrase:
Pass phrase check failed.
{% endhighlight %}

此时需要仔细检查密钥名称和写入 rpm 宏里面的是否一致，最好是将 `--list-keys` 显示的内容整体复制出来。

当然也可以在通过 `rpmbuild --sign` 打包时自动包含签名。

### 3. 增加签名

生成签名使用的是私钥，验证签名需要使用公钥。简单来说需要执行：1)将 gpg 产生的公钥导出到一个文件；2) 将这个公钥文件导入到 RPM 数据库里；3) 使用 rpm 命令进行检验。

{% highlight text %}
----- 导出公钥到一个文本文档
$ gpg --export -a "foobar (just for test)" > RPM-GPG-KEY-FOOBAR

----- 查看rpm数据库中已有的公钥
$ rpm -q gpg-pubkey-*
gpg-pubkey-f4a80eb5-53a7ff4b
gpg-pubkey-352c64e5-52ae6884
gpg-pubkey-442df0f8-4783f24a

----- 将公钥导入到RPM数据库，注意需要root用户执行
# rpm --import RPM-GPG-KEY-FOOBAR
{% endhighlight %}

通过上述命令重新查看公钥时，会发现新增了一个 `gpg-pubkey-860fb269-5a65a0ad` 公钥，其中 `860FB269` 与上述 gpg 生成的密钥信息相同。

如果不需要，可以通过 `rpm -e gpg-pubkey-860fb269-5a65a0ad` 删除即可。

### 4. 签名验证

通过 `rpm -K package_name.rpm` 命令对签名验证即可，如下是验证成功以及失败时输出的内容。

{% highlight text %}
package_name.rpm: rsa sha1 (md5) pgp md5 OK
package_name.rpm: RSA sha1 (MD5) (PGP) (MD5) (PGP) md5 NOT OK (MISSING KEYS: PGP#c0eb63c7 PGP#c0eb63c7)
{% endhighlight %}

### 其它

如果导入公钥失败，那么在安装时会生成一个类似 `Header V3 RSA/SHA1 signature: NOKEY, key ID c0eb63c7` 的告警信息。

如果要在 YUM 中验证签名，可以将公钥复制到系统 RPM 公钥目录，一般如下操作即可。

{% highlight text %}
# cp RPM-GPG-KEY-FOOBAR /etc/pki/rpm-gpg/
{% endhighlight %}

在源配置文件中通过如下两行来指定 gpg key 检验：

{% highlight text %}
gpgcheck=1
gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-FOOBAR
{% endhighlight %}

这样在通过 YUM 安装软件包时，当下载完毕以后会首先使用公钥进行签名验证。

## 常用设置

接下来，看看一些常用的实用技巧。

在一个 SPEC 文件中可以同时打包多个 RPM 包，当然也可以通过 `%package -n foobar` 指定子模块 (subpackage) 的名称 。

<!--
http://ftp.rpm.org/max-rpm/s1-rpm-subpack-building-subpackages.html
http://ftp.rpm.org/max-rpm/s1-rpm-subpack-spec-file-changes.html
http://ftp.rpm.org/max-rpm/ch-rpm-b-command.html
-->

### 宏定义

在定义文件的安装路径时，通常会使用类似 ```%_sharedstatedir``` 的宏，这些宏一般会在 ```/usr/lib/rpm/macros``` 中定义，当然部分会同时在不同平台上覆盖配置，可以直接 ```grep``` 查看。

RPM 内建宏定义在 `/usr/lib/rpm/redhat/macros` 文件中，这些宏基本上定义了目录路径或体系结构等等；同时也包含了一组用于调试 spec 文件的宏，关于 Macro 详细可以查看 [Macro syntax](http://rpm.org/user_doc/macros.html)，其中常用如下：

{% highlight text %}
%dump                 打印宏的值，包括一些内建的宏定义，也可以通过rpm --showrc查看
%{echo:message}       打印信息到标准输出
%{warn:message}       打印信息到标准错误
%{error:message}      打印信息到标准错误，然后返回BADSPEC
%{expand:expression}  类似Bash中的eval内置命令
{% endhighlight %}

另外常用的是根据宏来设置变量。

{% highlight text %}
%{?foobar:expr} 如果宏 foobar 存在则使用 expand expr，否则为空；也可以取反 %{!?foobar:expr}
%{?macro}       只测试该宏是否存在，存在就用该宏的值，反之则不用，如 %configure %{?_with_foobar}
{% endhighlight %}

另外，在判断宏的 Bool 值时，可以通过如下方式测试，如果 `variable` 定义，则为 `01` 也就是 `true` 否则为 `0` 。

{% highlight text %}
%if 0%{?variable:1}
... ...
%endif
{% endhighlight %}


### 配置脚本

很多制作 RPM 包的操作都是通过宏定义设置的，如下简单列举一下常见的宏定义操作。

{% highlight text %}
__os_install_post
    安装完之后的操作，例如去除二进制文件中的注释等；
__spec_install_pre
    在安装前的操作，设置一些环境变量，然后删除BUILDROOT中的文件(如果文件多时耗时增加)；
{% endhighlight %}

如果想取消某些操作，可以将这些操作设置为 ```%{nil}``` 即可。

{% highlight text %}
----- 查看对应的命令
$ rpm --showrc | grep -A 4 ': __os_install_post'
-14: __os_install_post
    /usr/lib/rpm/redhat/brp-compress
    %{!?__debug_package:
    /usr/lib/rpm/redhat/brp-strip %{__strip}
    /usr/lib/rpm/redhat/brp-strip-comment-note %{__strip} %{__objdump}

----- 在SPEC文件头部添加如下内容
%global __os_install_post %{nil}

----- 在用户的~/.rpmmacros文件中添加如下配置
%__os_install_post %{nil}
{% endhighlight %}

<!--
或者在全局配置文件中添加如下配置
/etc/rpm/macros
%__os_install_post %{nil}       
-->

### define vs. global

两者都可以用来进行变量定义，不过在细节上有些许差别，简单列举如下：

* define 用来定义宏，global 用来定义变量；
* 如果定义带参数的宏 (类似于函数)，必须要使用 define；
* 在 ```%{}``` 内部，必须要使用 global 而非 define；
* define 在使用时计算其值，而 global 则在定义时就计算其值；

可以简单参考如下的示例。

{% highlight spec %}
#--- %prep之前的参数是必须要有的
Name:           mysql
Version:        5.7.17
Release:        1%{?dist}
Summary:        MySQL from FooBar.
License:        GPLv2+ and BSD

%description
It is a MySQL from FooBar.

%prep
#--- 带参数时，必须使用%define定义
%define myecho() echo %1 %2
%{!?bar: %define bar defined}

echo 1: %{bar}
%{myecho 2: %{bar}}
echo 3: %{bar}

# 如下是输出内容
#1: defined
#2: defined
#3: %{bar}
{% endhighlight %}

显然 3 的输出是不符合预期的，可以将 ```%define``` 修改为 ```global``` 即可。

### 变量使用

define 定义的变量类似于局部变量，只在 ```%{!?foo: ... }``` 区间有效，不过 SPEC 并不会自动清除该变量，只有再次遇到 ```%{}``` 时才会清除，WTF!!!

在命令行中通过 ```--define 'with_ssl bundled'``` 进行定义，在 SPEC 脚本中，使用 ```%{?with_ssl: }``` 处理上述参数，或者通过 ```%{!?with_ssl: }``` 处理未定义参数时的情况。

{% highlight text %}
#--- 根据传入的变量，设置好相应的ssl_option变量
%{?with_ssl: %global ssl_option -DWITH_SSL=%{with_ssl}}

#--- 接下来，通过如下方式使用上述定义的变量
%{?ssl_option}
{% endhighlight %}

在 RPM 中，问号 ```?``` 用于条件检测，默认如果对应的变量值不存在，那么 RPM 会原样保留字符串，通过问号表示，如果不存在则移除。

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

除了上述的表示方法外，还可以使用如下内容：

{% highlight text %}
%define _without_foobar            1
# enabled by default
%define with_foobar 0%{!?_without_foobar:1}
{% endhighlight %}

如果 ```_without_foobar``` 变量不存在，那么默认 ```with_foobar``` 为 01 也就是 true，否则就是 0 也就是 false；可以通过如下命令行进行测试。

{% highlight text %}
$ rpm --define='with_foobar 0%{!?_without_foobar:1}' --eval='foobar:%{with_foobar}'
foobar:01
$ rpm --define='_without_foobar 1' --define='with_foobar 0%{!?_without_foobar:1}' --eval='foobar:%{with_foobar}'
foobar:0
{% endhighlight %}


### 其它

如下是 ```%if``` 判断的使用。

{% highlight text %}
%if 0%{?rhel} == 6
%global compatver             5.1.17
BuildRequires:  systemd
%else
%global compatver             5.1.17
BuildRequires:  sysv
%endif
{% endhighlight %}

可以使用 Requires(pre)、Requires(post) 等都是针对不同阶段的依赖设置；可以通过 ```%package PACKAGE-NAME``` 设置生成不同的 RPM 分支包。

另外，可以生成 GPG 签名，在此不再赘述。

#### 添加用户

可以在 ```%pre``` 段中添加如下内容。

{% highlight text %}
/usr/sbin/groupadd -g 66 -o -r monitor >/dev/null 2>&1 || :

参数：
  --gid/-g
    指定group id；
  --non-unique/-o
    group id可以不唯一，此时相当于指定了一个 alias；
  --system/-r
    创建系统分组；

/usr/sbin/useradd -M %{!?el5:-N} -g monitor -o -r -d %{_libdir}/%{name} -s /bin/false \
    -c "Uagent Server" -u 66 monitor >/dev/null 2>&1 || :
参数：
  --no-create-home/-M
    不创建HOME目录；
  --no-user-group/-N
    不创建相同用户名的分组；
  --non-unique/-o
    user id可以不唯一，此时相当于指定了一个 alias；
  --system/-r
    创建系统用户；
  --home-dir/-d HOME_DIR
    指定HOME目录，如果不存在则会创建；
  --shell/-s SHELL
    指定默认的shell；
{% endhighlight %}


#### 初始宏定义

很多的宏，是在 ```/etc/rpm``` 目录下定义的，如上面的 ```dist``` 在 ```/etc/rpm/macros.dist``` 文件中定义。


#### RPM包查看

对于生成的 RPM 包，只能查看头部信息和脚本内容，指令分别如下。

{% highlight text %}
$ rpm --info -qp XXX.rpm
$ rpm --scripts -qp XXX.rpm
{% endhighlight %}

#### 变量定义

在定义 `Version` 时，如果使用 `%{?package_version:1.0.0}` 可以工作但是，使用 `%{!?package_version:1.0.0}` 却无效。

而且这里的参数不能通过类似 `rpmbuld --define='package_version 1.9.1' foobar.spec` 的方式传入。

### 测试脚本

如下是一个测试用的脚本，可以用来生成简单的测试 SPEC 脚本，并执行。

该脚本会将 /tmp/foobar 目录作为工作目录，然后生成一个简单的 Hello world 程序。

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



## 参考

很多关于 RPM 的介绍可以参考文档 [Maximum RPM](http://rpm.org/max-rpm-snapshot/index.html)；另外，还可以参考下 MySQL 源码包中的相关示例 [mysql.spec](/reference/databases/mysql/mysql.spec) 。

<!--
http://fedoraproject.org/wiki/How_to_create_an_RPM_package

包含生成GPG签名
http://laoguang.blog.51cto.com/6013350/1103628

一个collectd创建的示例
http://giovannitorres.me/how-to-build-rpms-collectd-example.html

rpm mock
https://leo108.com/pid-2207.asp

-->

{% highlight text %}
{% endhighlight %}
