# 用戶管理工具

目錄

用戶管理工具
用戶
添加用戶
刪除用戶
用戶的組
將用戶加入到組
查看系統所有組
用戶權限
更改讀寫權限
更改文件或目錄的擁有者
環境變量
總結
8.1. 用戶
添加用戶
$useradd -m username
該命令爲用戶創建相應的帳號和用戶目錄/home/username；

用戶添加之後，設置密碼：

密碼以交互方式創建:

```
$passwd username
```

刪除用戶
```
$userdel -r username
```

不帶選項使用 userdel，只會刪除用戶。用戶的家目錄將仍會在/home目錄下。要完全的刪除用戶信息，使用-r選項；

帳號切換 登錄帳號爲userA用戶狀態下，切換到userB用戶帳號工作:

```
$su userB
```

進入交互模型，輸入密碼授權進入；

8.2. 用戶的組
將用戶加入到組
默認情況下，添加用戶操作也會相應的增加一個同名的組，用戶屬於同名組； 查看當前用戶所屬的組:

```
$groups
```

一個用戶可以屬於多個組，將用戶加入到組:

```
$usermod -G groupNmame username
```

變更用戶所屬的根組(將用加入到新的組，並從原有的組中除去）:

```
$usermod -g groupName username
```

查看系統所有組
系統的所有用戶及所有組信息分別記錄在兩個文件中：/etc/passwd , /etc/group 默認情況下這兩個文件對所有用戶可讀：

查看所有用戶及權限:

```
$more /etc/passwd
```

查看所有的用戶組及權限:

```
$more /etc/group
```

8.3. 用戶權限
使用ls -l可查看文件的屬性字段，文件屬性字段總共有10個字母組成，第一個字母表示文件類型，如果這個字母是一個減號”-”,則說明該文件是一個普通文件。字母”d”表示該文件是一個目錄，字母”d”,是dirtectory(目錄)的縮寫。 後面的9個字母爲該文件的權限標識，3個爲一組，分別表示文件所屬用戶、用戶所在組、其它用戶的讀寫和執行權限； 例如:

```
[/home/weber#]ls -l /etc/group
-rwxrw-r-- colin king 725 2013-11-12 15:37 /home/colin/a
```

表示這個文件對文件擁有者colin這個用戶可讀寫、可執行；對colin所在的組（king）可讀可寫；對其它用戶只可讀；

更改讀寫權限
使用chmod命令更改文件的讀寫權限，更改讀寫權限有兩種方法，一種是字母方式，一種是數字方式

字母方式:

```
$chown userMark(+|-)PermissionsMark
```

userMark取值：

u：用戶
g：組
o：其它用戶
a：所有用戶
PermissionsMark取值：

r:讀
w：寫
x：執行
例如:

```
$chmod a+x main         對所有用戶給文件main增加可執行權限
$chmod g+w blogs        對組用戶給文件blogs增加可寫權限
```

數字方式：

數字方式直接設置所有權限，相比字母方式，更加簡潔方便；

使用三位八進制數字的形式來表示權限，第一位指定屬主的權限，第二位指定組權限，第三位指定其他用戶的權限，每位通過4(讀)、2(寫)、1(執行)三種數值的和來確定權限。如6(4+2)代表有讀寫權，7(4+2+1)有讀、寫和執行的權限。

例如:

```
$chmod 740 main     將main的用戶權限設置爲rwxr-----
```

更改文件或目錄的擁有者
```
$chown username dirOrFile
```

使用-R選項遞歸更改該目下所有文件的擁有者:

```
$chown -R weber server/
```

8.4. 環境變量
bashrc與profile都用於保存用戶的環境信息，bashrc用於交互式non-loginshell，而profile用於交互式login shell。

/etc/profile，/etc/bashrc 是系統全局環境變量設定
~/.profile，~/.bashrc用戶目錄下的私有環境變量設定

當登入系統獲得一個shell進程時，其讀取環境設置腳本分爲三步:

首先讀入的是全局環境變量設置文件/etc/profile，然後根據其內容讀取額外的文檔，如/etc/profile.d和/etc/inputrc
讀取當前登錄用戶Home目錄下的文件~/.bash_profile，其次讀取~/.bash_login，最後讀取~/.profile，這三個文檔設定基本上是一樣的，讀取有優先關係
讀取~/.bashrc
~/.profile與~/.bashrc的區別:

這兩者都具有個性化定製功能
~/.profile可以設定本用戶專有的路徑，環境變量，等，它只能登入的時候執行一次
~/.bashrc也是某用戶專有設定文檔，可以設定路徑，命令別名，每次shell script的執行都會使用它一次
例如，我們可以在這些環境變量中設置自己經常進入的文件路徑，以及命令的快捷方式：

```
.bashrc
alias m='more'
alias cp='cp -i'
alias mv='mv -i'
alias ll='ls -l'
alias lsl='ls -lrt'
alias lm='ls -al|more'

log=/opt/applog/common_dir
unit=/opt/app/unittest/common

.bash_profile
. /opt/app/tuxapp/openav/config/setenv.prod.sh.linux
export PS1='$PWD#'
```

通過上述設置，我們進入log目錄就只需要輸入cd $log即可；

8.5. 總結
useradd passwd userdel usermod chmod chown .bashrc .bash_profile
