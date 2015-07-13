# Ubuntu installer

- 選擇套件來源

```
sudo /usr/bin/software-properties-gtk
```

```
sudo apt-get install autoconf automake linux-headers-`uname -r` vim-gnome libclang-dev p7zip guake p7zip-full liblzma-dev \
indicator-multiload filezilla pidgin pcmanx-gtk2 gparted meld speedcrunch vim ssh python-pip id-utils cflow autogen \
cutecom hexedit ccache clang pbzip2 smplayer plink putty-tools ghex doxygen doxygen-doc libstdc++6 lib32stdc++6 build-essential \
doxygen-gui graphviz git-core cconv alsa-oss wmctrl terminator curl gnome-tweak-tool qt4-qtconfig cgdb dos2unix libreadline-dev \
hexedit ccache ruby subversion htop astyle ubuntu-restricted-extras libncurses5-dev quicksynergy xdot python-pycallgraph \
exuberant-ctags cscope  libsdl1.2-dev gitk libncurses5-dev binutils-dev python-virtualenv  gtkterm libtool \
gcc-4.4 gcc-4.4-multilib g++-4.4-multilib g++-4.4 mpi-default-dev libbz2-dev libicu-dev python-dev scons csh \
enca ttf-anonymous-pro libperl4-corelibs-perl cgvg catfish ipython gawk i2c-tools sshfs wavesurfer audacity fcitx fcitx-chewing libswitch-perl bin86 inotify-tools u-boot-tools subversion crash tree mscgen krename umbrello intel2gas kernelshark trace-cmd pppoe
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
打開終端機，输入：alsamixer
```


- PCMan 沒有游標問題

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
sudo apt-get install fcitx-sunpinyin （双拼）
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

- 下載最近版本的npm

```
http://nodejs.org/dist/
```

- 使用 Global 安裝 gitbook

```
sudo npm install gitbook -g
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
