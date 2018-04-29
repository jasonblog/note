# Ubuntu installer

- 選擇套件來源

```
sudo /usr/bin/software-properties-gtk
```

```
sudo apt-get install autoconf automake linux-headers-`uname -r` vim-gnome \
                     libclang-dev p7zip guake p7zip-full liblzma-dev \
                     indicator-multiload filezilla pidgin pcmanx-gtk2 gparted meld \
                     speedcrunch vim ssh python-pip id-utils cflow autogen \
                     cutecom hexedit ccache clang pbzip2 smplayer plink putty-tools \
                     ghex doxygen doxygen-doc libstdc++6 lib32stdc++6 build-essential \
                     doxygen-gui graphviz git-core cconv alsa-oss wmctrl terminator \
                     curl gnome-tweak-tool qt4-qtconfig cgdb dos2unix libreadline-dev \
                     hexedit ccache ruby subversion htop astyle ubuntu-restricted-extras \
                     libncurses5-dev quicksynergy xdot python-pycallgraph exuberant-ctags cscope \
                     libsdl1.2-dev gitk libncurses5-dev binutils-dev python-virtualenv  gtkterm \
                     libtool mpi-default-dev libbz2-dev libicu-dev python-dev scons csh \
                     enca ttf-anonymous-pro libperl4-corelibs-perl cgvg catfish ipython gawk \
                     i2c-tools sshfs wavesurfer audacity fcitx fcitx-chewing libswitch-perl bin86 \
                     inotify-tools u-boot-tools subversion crash tree mscgen krename umbrello \
                     intel2gas kernelshark trace-cmd pppoe shutter dcfldd flex bison help2man \
                     texinfo texi2html ghp-import autossh samba sdcv xournal cloc geogebra libluajit-5.1-dev
```

-  PCMAN 字體

```
英文 ACSII DejaVu Sans Mono
中文字型 Serif
```

- gnome-open

```
sudo ln -s /usr/bin/gnome-open ~/bin/o
```

- 安裝 ack

```
https://github.com/petdance/ack

~/.ackrc
#--smart-case
#--follow

--smart-case
--no-heading
--no-group

--color-filename=magenta
--color-lineno=green
--color-match=red


--type-set=mk=.mk
--type-set=cpp=.cpp
--type-set=h=.h
```


 - slickedit vs

```
#!/bin/bash

VSEXEC=
unset XMODIFIERS
$VSEXEC $@


$ chmod +x vs
$ sudo mv vs /usr/bin
```



- LiHei Pro 儷黑Pro 字型

```
http://mengko616.googlepages.com/LiHeiProPC.ttf.tar.gz
/usr/share/fonts/truetype
sudo mkdir ttf-macosx
sudo fc-cache -v -f
```


- Install Oracle JDK 6

```
下載並安裝JDK 1.6（我用64位的）
$ wget http://ghaffarian.net/downloads/Java/JDK/jdk-6u45-linux-x64.bin
$ chmod u+x jdk-6u45-linux-x64.bin
$ ./jdk-6u45-linux-x64.bin
$ sudo mkdir -p /usr/lib/jvm
$ sudo mv jdk1.6.0_45 /usr/lib/jvm/


sudo update-alternatives --install /usr/bin/javac javac /usr/lib/jvm/jdk1.6.0_45/bin/javac 1
sudo update-alternatives --install /usr/bin/java java /usr/lib/jvm/jdk1.6.0_45/bin/java 1
sudo update-alternatives --install /usr/bin/javaws javaws /usr/lib/jvm/jdk1.6.0_45/bin/javaws 1
sudo update-alternatives --install /usr/bin/jar jar /usr/lib/jvm/jdk1.6.0_45/bin/jar 1
sudo update-alternatives --install /usr/bin/javap javap /usr/lib/jvm/jdk1.6.0_45/bin/javap 1
sudo update-alternatives --install "/usr/bin/javadoc" "javadoc" "/usr/lib/jvm/jdk1.6.0_45/bin/javadoc" 1


update-alternatives --config java
update-alternatives --config javac
update-alternatives --config javaws
update-alternatives --config jar
update-alternatives --config javap


$ java -version
```

- ubuntu 聲音很小音量小聲

```
解決之道:
打開終端機，輸入：alsamixer
```


- PCMan 沒有遊標問題

```
deb http://ppa.launchpad.net/fourdollars/pcmanx/ubuntu YOUR_UBUNTU_VERSION_HERE main
deb-src http://ppa.launchpad.net/fourdollars/pcmanx/ubuntu YOUR_UBUNTU_VERSION_HERE main
```
```
- 將 /tmp 設到 RamDisk (tmpfs) 的方法

基本上只要打以下指令，就能將 /tmp 綁定到 /dev/shm
mkdir /dev/shm/tmp
chmod 1777 /dev/shm/tmp
sudo mount --bind /dev/shm/tmp /tmp

※ 註：為何是用 mount --bind 綁定，而不是 ln -s 軟連結，原因是 /tmp 目錄，系統不給刪除。

不過每次開機都要打指令才能用，這樣是行不通的，必須讓它開機時自動執行，才會方便。
```

- 用文書編輯器，建立 /etc/init.d/ramtmp.sh 內容如下：

```
#!/bin/sh
# RamDisk tmp
PATH=/sbin:/bin:/usr/bin:/usr/sbin

mkdir -p /dev/shm/tmp
mkdir -p /dev/shm/cache
mount --bind /dev/shm/tmp /tmp
mount --bind /dev/shm/cache /home/shihyu/.cache
chmod 1777 /dev/shm/tmp
chmod 1777 /dev/shm/cache
```

- 將此檔改權限為 755，使其可執行

```
sudo chmod 755 /etc/init.d/ramtmp.sh
```

- 在 /etc/rcS.d 中，建立相關軟連結(捷徑)，使其一開機就執行 以下指令僅能終端機操作

```
cd /etc/rcS.d
sudo ln -s ../init.d/ramtmp.sh S50ramtmp.sh
```


- fcitx安裝 以前曾安裝過fcitx的話，最好把過去的都清理掉，包括 PPA

```
sudo apt-get install fcitx
sudo apt-get install im-config
sudo add-apt-repository ppa:fcitx-team/nightly
sudo apt-get update
```

- 檢查更新 ,更新完, 再繼續安裝碼表

```
sudo apt-get install fcitx-table-boshiamy (嘸蝦米）
sudo apt-get install fcitx-table-cangjie-big （倉頡大字集）
sudo apt-get install fcitx-table-zhengma-large （鄭碼大字集）
sudo apt-get install fcitx-table-wubi-large （五筆大字集）
sudo apt-get install fcitx-chewing （新酷音）
sudo apt-get install fcitx-sunpinyin （雙拼）
sudo apt-get install fcitx-table-easy-big （輕鬆大詞庫）
sudo apt-get install fcitx-m17n
sudo apt-get remove ibus
```
- im-config

```
選fcitx為預設
重開機或登入
在有可輸入中文的框中，按Ctrl+Space，然後用Ctrl+Shift選輸入法輸入，預設的簡繁轉換為Ctrl+Shift+F
```

- ubuntu 13.10 記得清除 Text Entry 裡面快速鍵設定, fcitx 才會有作用 IntelliJ IDEA 13 on Ubuntu Linux 13.10 字型美化看習慣 Mac OS X 的字型，再回到 Ubuntu Linux 桌面總會不習慣。主要的差異是 Font Rendering 的效果，讓 Ubuntu 預設的字型看起來過於銳利。裝上 fontconfig-infinality 即可改善。

```
sudo add-apt-repository ppa:no1wantdthisname/ppa
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install fontconfig-infinality
sudo bash /etc/fonts/infinality/infctl.sh setstyle
```

- 最後一道指令要選擇風格，建議用 2 或 4 這兩種其中一個。

```
Select a style:
1) debug       3) linux       5) osx2      7) win98
2) infinality  4) osx       6) win7      8) winxp
```

-  Install NVIDIA GeForce driver in Ubuntu 14.04 / 13.10 / 13.04 / 12.04 using PPA

```
To add PPA for Ubuntu 14.04 / 13.10 / 13.04 / 12.10

Ubuntu 14.04 / 13.10 / 13.04 / 12.10 users, Run the following command in the terminal and type the password for the user when prompted.  Others use the Ubuntu-X PPA.

sudo apt-add-repository ppa:xorg-edgers/ppa
To add PPA for Ubuntu 12.04 / 11.10 / 11.04 / 10.04

sudo apt-add-repository ppa:ubuntu-x-swat/x-updates

To Update

sudo apt-get update
Now install the NVIDIA GeForce graphics driver 304.108 in your system, as you install other packages by using apt-get.

To Install

sudo apt-get install nvidia-current nvidia-settings
Above command will install the driver.
```

- Install TeamViewer 9 in Ubuntu 14.04 64bit

```
sudo dpkg --add-architecture i386
sudo apt-get update
wget http://www.teamviewer.com/download/teamviewer_linux.deb
sudo dpkg -i teamviewer_linux.deb
sudo apt-get install -f
```

- Linux 開機就掛載指定的磁區

```
Linux 版本：Linux mint debian edition 64bit

 sudo ls -l /dev/disk/by-uuid
查詢要掛載磁區的UUID

編輯　 /etc/fstab  這個檔案，檔案內會有已掛載的資訊．可以照本宣科的作．當然要存檔

 mount -a 　重新掛載磁區

確認掛載結果　 df

重開機後，應該就會自動掛載指定的磁區了．

UUID=1f833a4c-b564-4616-bc72-0fc92d62a687 /media/HD         ext4    defaults        0       2
```

# Gitbook 安裝

- 安裝 Node.js

```
sudo apt-get update
sudo apt-get install nodejs
sudo apt-get install npm
```


- 使用 Global 安裝 gitbook

```
sudo npm install gitbook -g
```


如果遇到：

```sh
Error loading version latest: Error: Cannot find module 'internal/util/types'
```

`將node版本降低:`

```sh
// 1.安裝node管理 n
sudo npm install -g n 
// 2. 降低版本，更新npm
sudo n 6

sudo npm install npm -g
// 3.此時運行gitbook2.6.7的命令即可。
gitbook build --gitbook=2.6.7
// 4. 待轉換完成後，將npm版本（例如9.10.0）切回來即可,以免影響其他模塊
sudo n 9.10.0
```


- 下載最近版本的npm , 目前可以解決下面說的情況

```
http://nodejs.org/dist/
```

- 安裝 nodejs-legacy 避免後續執行 gitbook 的時後發生錯誤 "/usr/bin/env: node: No such file or directory"

```
sudo apt-get install nodejs-legacy
```

- 安裝 Calibre Tools 以便進行輸出電子書檔案  如果未安裝即執行 gitbook pdf 等輸出電子書檔案時會出現錯誤 "Need to install ebook-convert from Calibre"，因此需要先行安裝 Calibre 以便進行後續的輸出動作

```
sudo apt-get install Calibre
```

- 當執行 gitbook pdf -v 出現 "RuntimeError: X server required. If you are running on a headless machine, use xvfb"，所以也需要先安裝 xvfb 避免錯誤

```
sudo apt-get install xvfb
updatedb
```

# 安裝gitbook-editor

- 下載GITBOOK EDTOR

```
https://github.com/GitbookIO/editor-legacy/releases
wget https://github.com/GitbookIO/editor-legacy/releases/download/1.1.0/gitbook-linux64.tar.gz
```
- 解壓及安裝

```
tar -xvzf gitbook-linux64.tar.gz
cd linux64
sudo ln -s /lib/x86_64-linux-gnu/libudev.so.1 /lib/x86_64-linux-gnu/libudev.so.0
bash ./install.sh
```

- Makefile

```
files=zh-tw/*.md
cmd=opencc -c zht2zhs.ini -i

hans:
	@mkdir -p zh-cn
	@for name in ${files} ; do \
		${cmd} $$name -o $${name//tw/cn} ; \
	done

init:
	@cp README.md zh-tw/
	@$(MAKE) hans

serve:
	@gitbook serve

build:
	@gitbook build

clean:
	rm -fr _book

publish:
	@ghp-import _book -p -n
```
---
#  多個 SSH Key 對應多個 Github 帳號

- 產生個別的 ssh keys

```
$ cd ~/.ssh
$ ssh-keygen -t rsa -C "account1@email.com" -f id_rsa_account1
$ ssh-keygen -t rsa -C "account2@email.com" -f id_rsa_account2
```

- github setting Deploy keys

```
勾選
Allow write access
Can this key be used to push to this repository? Deploy keys always have pull access.
```
- 建立 config 檔

```
$ cd ~/.ssh
$ touch config
$ gedit config
```

- 編輯內容如下

```
#account1
Host github.com-account1
    HostName github.com
    User git
    IdentityFile ~/.ssh/id_rsa_account1

#account2
Host github.com-account2
    HostName github.com
    User git
    IdentityFile ~/.ssh/id_rsa_account2
```

- 修改相對應 repo 的 remote url。例如：
    - ssh://git@github.com-account1/**github account**/repo1.git
    - github account 是github帳號 **ex : jasonblog , ccccjason , shihyu**

```
$ cd /path/to/repo1
$ git remote set-url origin ssh://git@github.com-account1/account1/repo1.git
```

- ~/.ssh/config

```
#account1
Host github.com-ccccjason
HostName github.com
User git
IdentityFile ~/.ssh/id_rsa_ccccjason
```


```
git remote set-url origin ssh://git@github.com-ccccjason/ccccjason/book.git _book

git clone  ssh://git@github.com-ccccjason/ccccjason/book.git _book
```

## Graphics issues after/while installing Ubuntu 16.04/16.10 with NVIDIA graphics

```sh
sudo apt-get purge nvidia-*
sudo add-apt-repository ppa:graphics-drivers/ppa
sudo apt-get update
sudo apt-get install nvidia-375
Reboot.
```

##  update-alternatives gcc

```sh
update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-3.3 33
update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-3.4 34
update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-2.95 295
update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.1 41

使用 update-alternatives --config gcc 來設定用那一版本的 gcc
```

## grub add for windows 7

- grub/grub.cfg

```sh
### BEGIN /etc/grub.d/30_os-prober ###
menuentry 'Windows Boot Manager (於 /dev/sdc1)' --class windows --class os $menuentry_id_option 'osprober-efi-BCED-7265' {
	insmod part_gpt
	insmod fat
	set root='hd2,gpt1'
	if [ x$feature_platform_search_hint = xy ]; then
	  search --no-floppy --fs-uuid --set=root --hint-bios=hd2,gpt1 --hint-efi=hd2,gpt1 --hint-baremetal=ahci2,gpt1  BCED-7265
	else
	  search --no-floppy --fs-uuid --set=root BCED-7265
	fi
	chainloader /EFI/Microsoft/Boot/bootmgfw.efi
}
set timeout_style=menu
if [ "${timeout}" = 0 ]; then
  set timeout=10
fi
### END /etc/grub.d/30_os-prober ###
```
- /etc/grub.d/30_os-prober

```sh
#! /bin/sh
set -e

# grub-mkconfig helper script.
# Copyright (C) 2006,2007,2008,2009  Free Software Foundation, Inc.
#
# GRUB is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# GRUB is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GRUB.  If not, see <http://www.gnu.org/licenses/>.

prefix="/usr"
exec_prefix="/usr"
datarootdir="/usr/share"
quick_boot="1"

export TEXTDOMAIN=grub
export TEXTDOMAINDIR="${datarootdir}/locale"

. "${datarootdir}/grub/grub-mkconfig_lib"

found_other_os=

adjust_timeout () {
  if [ "$quick_boot" = 1 ] && [ "x${found_other_os}" != "x" ]; then
    cat << EOF
set timeout_style=menu
if [ "\${timeout}" = 0 ]; then
  set timeout=10
fi
EOF
  fi
}

if [ "x${GRUB_DISABLE_OS_PROBER}" = "xtrue" ]; then
  exit 0
fi

if [ -z "`which os-prober 2> /dev/null`" -o -z "`which linux-boot-prober 2> /dev/null`" ] ; then
  # missing os-prober and/or linux-boot-prober
  exit 0
fi

OSPROBED="`os-prober | tr ' ' '^' | paste -s -d ' '`"
if [ -z "${OSPROBED}" ] ; then
  # empty os-prober output, nothing doing
  exit 0
fi

osx_entry() {
    found_other_os=1
    if [ x$2 = x32 ]; then
        # TRANSLATORS: it refers to kernel architecture (32-bit)
	bitstr="$(gettext "(32-bit)")"
    else
        # TRANSLATORS: it refers to kernel architecture (64-bit)
	bitstr="$(gettext "(64-bit)")"
    fi
    # TRANSLATORS: it refers on the OS residing on device %s
    onstr="$(gettext_printf "(on %s)" "${DEVICE}")"
        cat << EOF
menuentry '$(echo "${LONGNAME} $bitstr $onstr" | grub_quote)' --class osx --class darwin --class os \$menuentry_id_option 'osprober-xnu-$2-$(grub_get_device_id "${DEVICE}")'  {
EOF
	save_default_entry | grub_add_tab
	prepare_grub_to_access_device ${DEVICE} | grub_add_tab
	cat << EOF
        load_video
        set do_resume=0
        if [ /var/vm/sleepimage -nt10 / ]; then
           if xnu_resume /var/vm/sleepimage; then
             set do_resume=1
           fi
        fi
        if [ \$do_resume = 0 ]; then
           xnu_uuid ${OSXUUID} uuid
           if [ -f /Extra/DSDT.aml ]; then
              acpi -e /Extra/DSDT.aml
           fi
           if [ /kernelcache -nt /System/Library/Extensions ]; then
              $1 /kernelcache boot-uuid=\${uuid} rd=*uuid
           else
              $1 /mach_kernel boot-uuid=\${uuid} rd=*uuid
              if [ /System/Library/Extensions.mkext -nt /System/Library/Extensions ]; then
                xnu_mkext /System/Library/Extensions.mkext
              else
                xnu_kextdir /System/Library/Extensions
              fi
           fi
           if [ -f /Extra/Extensions.mkext ]; then
              xnu_mkext /Extra/Extensions.mkext
           fi
           if [ -d /Extra/Extensions ]; then
              xnu_kextdir /Extra/Extensions
           fi
           if [ -f /Extra/devprop.bin ]; then
              xnu_devprop_load /Extra/devprop.bin
           fi
           if [ -f /Extra/splash.jpg ]; then
              insmod jpeg
              xnu_splash /Extra/splash.jpg
           fi
           if [ -f /Extra/splash.png ]; then
              insmod png
              xnu_splash /Extra/splash.png
           fi
           if [ -f /Extra/splash.tga ]; then
              insmod tga
              xnu_splash /Extra/splash.tga
           fi
        fi
}
EOF
}

used_osprober_linux_ids=

wubi=

for OS in ${OSPROBED} ; do
  DEVICE="`echo ${OS} | cut -d ':' -f 1`"
  LONGNAME="`echo ${OS} | cut -d ':' -f 2 | tr '^' ' '`"
  LABEL="`echo ${OS} | cut -d ':' -f 3 | tr '^' ' '`"
  BOOT="`echo ${OS} | cut -d ':' -f 4`"
  if UUID="`${grub_probe} --target=fs_uuid --device ${DEVICE%@*}`"; then
    EXPUUID="$UUID"

    if [ x"${DEVICE#*@}" != x ] ; then
      EXPUUID="${EXPUUID}@${DEVICE#*@}"
    fi

    if [ "x${GRUB_OS_PROBER_SKIP_LIST}" != "x" -a "x`echo ${GRUB_OS_PROBER_SKIP_LIST} | grep -i -e '\b'${EXPUUID}'\b'`" != "x" ] ; then
      echo "Skipped ${LONGNAME} on ${DEVICE} by user request." >&2
      continue
    fi
  fi

  BTRFS="`echo ${OS} | cut -d ':' -f 5`"
  if [ "x$BTRFS" = "xbtrfs" ]; then
	BTRFSuuid="`echo ${OS} | cut -d ':' -f 6`"
	BTRFSsubvol="`echo ${OS} | cut -d ':' -f 7`"
  fi

  if [ -z "${LONGNAME}" ] ; then
    LONGNAME="${LABEL}"
  fi

  gettext_printf "Found %s on %s\n" "${LONGNAME}" "${DEVICE}" >&2

  case ${BOOT} in
    chain)

      case ${LONGNAME} in
	Windows*)
	  if [ -z "$wubi" ]; then
	    if [ -x /usr/share/lupin-support/grub-mkimage ] && \
	       /usr/share/lupin-support/grub-mkimage --test; then
	      wubi=yes
	    else
	      wubi=no
	    fi
	  fi
	  if [ "$wubi" = yes ]; then
	    echo "Skipping ${LONGNAME} on Wubi system" >&2
	    continue
	  fi
	  ;;
      esac

      found_other_os=1
	  onstr="$(gettext_printf "(on %s)" "${DEVICE}")"
      cat << EOF
menuentry '$(echo "${LONGNAME} $onstr" | grub_quote)' --class windows --class os \$menuentry_id_option 'osprober-chain-$(grub_get_device_id "${DEVICE}")' {
EOF
      save_default_entry | grub_add_tab
      prepare_grub_to_access_device ${DEVICE} | grub_add_tab

      if [ x"`${grub_probe} --device ${DEVICE} --target=partmap`" = xmsdos ]; then
	  cat << EOF
	parttool \${root} hidden-
EOF
      fi

      case ${LONGNAME} in
	Windows\ Vista*|Windows\ 7*|Windows\ Server\ 2008*)
	;;
	*)
	  cat << EOF
	drivemap -s (hd0) \${root}
EOF
	;;
      esac

      cat <<EOF
	chainloader +1
}
EOF
    ;;
    efi)

	found_other_os=1
	EFIPATH=${DEVICE#*@}
	DEVICE=${DEVICE%@*}
	onstr="$(gettext_printf "(on %s)" "${DEVICE}")"
      cat << EOF
menuentry '$(echo "${LONGNAME} $onstr" | grub_quote)' --class windows --class os \$menuentry_id_option 'osprober-efi-$(grub_get_device_id "${DEVICE}")' {
EOF
      save_default_entry | sed -e "s/^/\t/"
      prepare_grub_to_access_device ${DEVICE} | sed -e "s/^/\t/"

      cat <<EOF
	chainloader ${EFIPATH}
}
EOF
    ;;
    linux)
      if [ "x$BTRFS" = "xbtrfs" ]; then
         LINUXPROBED="`linux-boot-prober btrfs ${BTRFSuuid} ${BTRFSsubvol}  2> /dev/null | tr ' ' '^' | paste -s -d ' '`"
      else
         LINUXPROBED="`linux-boot-prober ${DEVICE} 2> /dev/null | tr ' ' '^' | paste -s -d ' '`"
      fi
      prepare_boot_cache=
      boot_device_id=
      is_top_level=true
      title_correction_code=
      OS="${LONGNAME}"

      for LINUX in ${LINUXPROBED} ; do
        LROOT="`echo ${LINUX} | cut -d ':' -f 1`"
        LBOOT="`echo ${LINUX} | cut -d ':' -f 2`"
        LLABEL="`echo ${LINUX} | cut -d ':' -f 3 | tr '^' ' '`"
        LKERNEL="`echo ${LINUX} | cut -d ':' -f 4`"
        LINITRD="`echo ${LINUX} | cut -d ':' -f 5`"
        LPARAMS="`echo ${LINUX} | cut -d ':' -f 6- | tr '^' ' '`"

        if [ -z "${LLABEL}" ] ; then
          LLABEL="${LONGNAME}"
        fi

	if [ "${LROOT}" != "${LBOOT}" ]; then
	  LKERNEL="${LKERNEL#/boot}"
	  LINITRD="${LINITRD#/boot}"
	fi

	if [ -z "${prepare_boot_cache}" ]; then
	  prepare_boot_cache="$(prepare_grub_to_access_device ${LBOOT} | grub_add_tab)"
	  [ "${prepare_boot_cache}" ] || continue
	fi

	found_other_os=1
	onstr="$(gettext_printf "(on %s)" "${DEVICE}")"
	recovery_params="$(echo "${LPARAMS}" | grep 'single\|recovery')" || true
	counter=1
	while echo "$used_osprober_linux_ids" | grep 'osprober-gnulinux-$LKERNEL-${recovery_params}-$counter-$boot_device_id' > /dev/null; do
	    counter=$((counter+1));
	done
	if [ -z "$boot_device_id" ]; then
	    boot_device_id="$(grub_get_device_id "${DEVICE}")"
	fi
	used_osprober_linux_ids="$used_osprober_linux_ids 'osprober-gnulinux-$LKERNEL-${recovery_params}-$counter-$boot_device_id'"

	if [ "x$is_top_level" = xtrue ] && [ "x${GRUB_DISABLE_SUBMENU}" != xy ]; then
            cat << EOF
menuentry '$(echo "$OS $onstr" | grub_quote)' --class gnu-linux --class gnu --class os \$menuentry_id_option 'osprober-gnulinux-simple-$boot_device_id' {
EOF
	    save_default_entry | grub_add_tab
	    printf '%s\n' "${prepare_boot_cache}"
	    cat <<  EOF
	linux ${LKERNEL} ${LPARAMS}
EOF
            if [ -n "${LINITRD}" ] ; then
          cat << EOF
	initrd ${LINITRD}
EOF
            fi
        cat << EOF
}
EOF
	    echo "submenu '$(gettext_printf "Advanced options for %s" "${OS} $onstr" | grub_quote)' \$menuentry_id_option 'osprober-gnulinux-advanced-$boot_device_id' {"
	    is_top_level=false
	fi
	title="${LLABEL} $onstr"
        cat << EOF
	menuentry '$(echo "$title" | grub_quote)' --class gnu-linux --class gnu --class os \$menuentry_id_option 'osprober-gnulinux-$LKERNEL-${recovery_params}-$boot_device_id' {
EOF
	save_default_entry | sed -e "s/^/$grub_tab$grub_tab/"
	printf '%s\n' "${prepare_boot_cache}" | grub_add_tab
	cat <<  EOF
		linux ${LKERNEL} ${LPARAMS}
EOF
        if [ -n "${LINITRD}" ] ; then
            cat << EOF
		initrd ${LINITRD}
EOF
        fi
        cat << EOF
	}
EOF
	if [ x"$title" = x"$GRUB_ACTUAL_DEFAULT" ] || [ x"Previous Linux versions>$title" = x"$GRUB_ACTUAL_DEFAULT" ]; then
	    replacement_title="$(echo "Advanced options for ${OS} $onstr" | sed 's,>,>>,g')>$(echo "$title" | sed 's,>,>>,g')"
	    quoted="$(echo "$GRUB_ACTUAL_DEFAULT" | grub_quote)"
	    title_correction_code="${title_correction_code}if [ \"x\$default\" = '$quoted' ]; then default='$(echo "$replacement_title" | grub_quote)'; fi;"
	    grub_warn "$(gettext_printf "Please don't use old title \`%s' for GRUB_DEFAULT, use \`%s' (for versions before 2.00) or \`%s' (for 2.00 or later)" "$GRUB_ACTUAL_DEFAULT" "$replacement_title" "gnulinux-advanced-$boot_device_id>gnulinux-$version-$type-$boot_device_id")"
	fi
      done
      if [ x"$is_top_level" != xtrue ]; then
	  echo '}'
      fi
      echo "$title_correction_code"
    ;;
    macosx)
      if [ "${UUID}" ]; then
	OSXUUID="${UUID}"
	osx_entry xnu_kernel 32
	osx_entry xnu_kernel64 64
      fi
    ;;
    hurd)
      found_other_os=1
      onstr="$(gettext_printf "(on %s)" "${DEVICE}")"
      cat << EOF
menuentry '$(echo "${LONGNAME} $onstr" | grub_quote)' --class hurd --class gnu --class os \$menuentry_id_option 'osprober-gnuhurd-/boot/gnumach.gz-false-$(grub_get_device_id "${DEVICE}")' {
EOF
      save_default_entry | grub_add_tab
      prepare_grub_to_access_device ${DEVICE} | grub_add_tab
      grub_device="`${grub_probe} --device ${DEVICE} --target=drive`"
      mach_device="`echo "${grub_device}" | sed -e 's/(\(hd.*\),msdos\(.*\))/\1s\2/'`"
      grub_fs="`${grub_probe} --device ${DEVICE} --target=fs`"
      case "${grub_fs}" in
	*fs)	hurd_fs="${grub_fs}" ;;
	*)	hurd_fs="${grub_fs}fs" ;;
      esac
      cat << EOF
	multiboot /boot/gnumach.gz root=device:${mach_device}
	module /hurd/${hurd_fs}.static ${hurd_fs} --readonly \\
			--multiboot-command-line='\${kernel-command-line}' \\
			--host-priv-port='\${host-port}' \\
			--device-master-port='\${device-port}' \\
			--exec-server-task='\${exec-task}' -T typed '\${root}' \\
			'\$(task-create)' '\$(task-resume)'
	module /lib/ld.so.1 exec /hurd/exec '\$(exec-task=task-create)'
}
EOF
    ;;
    minix)
	  cat << EOF
menuentry "${LONGNAME} (on ${DEVICE}, Multiboot)" {
EOF
         save_default_entry | sed -e "s/^/\t/"
         prepare_grub_to_access_device ${DEVICE} | sed -e "s/^/\t/"
	 cat << EOF
	multiboot /boot/image_latest
}
EOF
    ;;
    *)
      echo -n "  "
      # TRANSLATORS: %s is replaced by OS name.
      gettext_printf "%s is not yet supported by grub-mkconfig.\n" "${LONGNAME}" >&2
    ;;
  esac
done

adjust_timeout
```


---

##解決Ubuntu 16.04下提示boot分區空間不足的辦法


最近看瞭看/boot的大小，發現幾次升級後，大小不足，所以想擴容，一開始還想用磁盤操作，但上網查詢後發現，磁盤操作實在風險太大，特別是雙系統的Linux，操作又是很麻煩，後來發現可以刪除多餘的舊內核來清理/boot，釋放空間。下面來看看詳細的解決方法吧。

###前言

因為linux內核一直在更新，更新後，舊的內核就不在使用，但舊的內核文件還在boot裡面，佔據著空間，更新幾次過後boot分區就會被佔滿，顯示boot磁盤空間不足。

###解決辦法

將不用的內核文件刪除，釋放空間。

###步驟如下

一、查看已安裝的內核 dpkg --get-selections |grep linux-image
```sh
dpkg --get-selections |grep linux-image
linux-image-4.4.0-21-generic deinstall
linux-image-4.4.0-57-generic install
linux-image-4.4.0-59-generic install
linux-image-4.4.0-62-generic install
linux-image-4.4.0-64-generic install
linux-image-extra-4.4.0-21-generic deinstall
linux-image-extra-4.4.0-57-generic install
linux-image-extra-4.4.0-59-generic install
linux-image-extra-4.4.0-62-generic install
linux-image-extra-4.4.0-64-generic install
linux-image-extra-virtual install
linux-image-generic install
```

後面帶deinstall的為已刪除的內核，可以忽略

二、使用uname -a查看自己當前啟動的是哪個內核

```sh
uname -a
Linux eason 4.4.0-62-generic #83-Ubuntu SMP Wed Jan 18 14:10:15 UTC 2017 x86_64 x86_64 x86_64 GNU/Linux
```

從輸出可知我們當前啟動的內核是4.4.0-62-generic

三、運行apt-get remove命令卸載其他內核，為瞭保險起見，保留最近的一兩個版本。

```sh
sudo apt-get remove linux-image-4.4.0-57-generic
sudo apt-get remove linux-image-4.4.0-59-generic
sudo apt-get remove linux-image-extra-4.4.0-57-generic
sudo apt-get remove linux-image-extra-4.4.0-59-generic
```

如提示有未卸載幹凈的可以執行sudo apt-get remove來卸載。


`使用 sudp dpkg -P  可以乾淨移除`

```
sudp dpkg -P linux-image-4.4.0-57-generic
```

###總結

以上就是這篇文章的全部內容瞭，希望本文的內容對大傢的學習或者工作能帶來一定的幫助，如果有疑問大傢可以留言交流。
