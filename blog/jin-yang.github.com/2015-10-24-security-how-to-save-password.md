---
title: Linux 密碼管理
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,密碼,管理
description: 簡單介紹下 Linux 中的密碼管理。
---

簡單介紹下 Linux 中的密碼管理。

<!-- more -->

## 簡介

常見的加密工具有，seahorse([gnome-keyring](https://wiki.gnome.org/Projects/GnomeKeyring))、[Password Safe](https://www.schneier.com/passsafe.html)、LastPass (跨平臺的密碼保存工具)、keepass (跨平臺的密碼保存機制)，在 CentOS 中可以通過 ```yum install keepassx``` 安裝。

建議使用 keepass，這樣只需要記住密碼和對應的庫文件即可，然後可以在其它平臺上打開。

在 gnome-keyring 官網中，包括了詳細的介紹。

### 加密方法

密碼算法都是公開的，保密應該依賴於密鑰的保密，而不是算法的保密。

對稱加密，密鑰可同時用於加解密，一般密鑰會直接出現在加密代碼中，破解的可能性相當大，而且系統管理員很可能知道密鑰，進而算出密碼原文。由於密鑰需要保密，因此需要事先祕密約定，或者用額外的安全信道來傳輸。

非對稱加密，密碼的安全性等同於私鑰的安全性。密碼明文經過公鑰加密，要還原密碼明文，必須要相應的私鑰才行。因此只要保證私鑰的安全，密碼明文就安全。

<!--
通常利用哈希算法的單向性來保證明文以不可還原的有損方式進行存儲。

MD5 和 SHA-1 已破解，雖不能還原明文，但很容易找到能生成相同哈希值的替代明文。而且這兩個算法速度較快，暴力破解相對省時，建議不要使用它們。

使用更安全的 SHA-256 等成熟算法，更加複雜的算法增加了暴力破解的難度。但如果遇到簡單密碼，用彩虹字典的暴力破解法，很快就能得到密碼原文。

加入隨機 salt 的哈希算法
    密碼原文（或經過 hash 後的值）和隨機生成的 salt 字符串混淆，然後再進行 hash，最後把 hash 值和 salt 值一起存儲。驗證密碼的時候只要用 salt 再與密碼原文做一次相同步驟的運算，比較結果與存儲的 hash 值就可以了。這樣一來哪怕是簡單的密碼，在進過 salt 混淆後產生的也是很不常見的字符串，根本不會出現在彩虹字典中。salt 越長暴力破解的難度越大

    具體的 hash 過程也可以進行若干次疊代，雖然 hash 疊代會增加碰撞率，但也增加暴力破解的資源消耗。就算真被破解了，黑客掌握的也只是這個隨機 salt 混淆過的密碼，用戶原始密碼依然安全，不用擔心其它使用相同密碼的應用。

上面這幾種方法都不可能得到密碼的明文，就算是系統管理員也沒辦法。對於那些真的忘了密碼的用戶，網站只能提供重置密碼的功能了。

下面的 python 程序演示瞭如何使用 salt 加 hash 來單向轉換密碼明文

import os
from hashlib import sha256
from hmac import HMAC

def encrypt_password(password, salt=None):
    """Hash password on the fly."""
    if salt is None:
        salt = os.urandom(8) # 64 bits.

    assert 8 == len(salt)
    assert isinstance(salt, str)

    if isinstance(password, unicode):
        password = password.encode('UTF-8')

    assert isinstance(password, str)

    result = password
    for i in xrange(10):
        result = HMAC(result, salt, sha256).digest()

    return salt + result

這裡先隨機生成 64 bits 的 salt，再選擇 SHA-256 算法使用 HMAC 對密碼和 salt 進行 10 次疊代混淆，最後將 salt 和 hash 結果一起返回。

使用的方法很簡單：

hashed = encrypt_password('secret password')

下面是驗證函數，它直接使用 encrypt_password 來對密碼進行相同的單向轉換並比較

def validate_password(hashed, input_password):
    return hashed == encrypt_password(input_password, salt=hashed[:8])

assert validate_password(hashed, 'secret password')

雖然只有簡短几行，但藉助 python 標準庫幫助，這已經是一個可用於生產環境的高安全密碼加密驗證算法了。

總結一下用戶密碼的存儲：
    上善不戰而屈人之兵。如果可能不要存任何密碼信息 讓別人（OpenID）來幫你做事，避開這個問題
    如果非要自己認證，也只能存 不可逆的有損密碼信息 。通過單向 hash 和 salt 來保證只有用戶知道密碼明文
    絕對不能存可還原密碼原文的信息 。如果因為種種原因一定要可還原密碼原文，請使用非對稱加密，並保管好私鑰
-->



## Linux Shadow

Linux 通過 libcrypt 中的 ```man 3 crypt``` 函數對密碼進行加密，該函數在 glibc 庫中。在 ```/etc/shadow``` 中保存的格式為 ```$id$salt$encrypted```，可以通過上述的 man 查看其含義。

接下來看看 ```/etc/shadow``` 文件的內容：

{% highlight text %}
root:$1$Bg1H/4mz$X89TqH7tpi9dX1B9j5YsF.:14838:0:99999:7:::
{% endhighlight %}

如果密碼字符串為 ```*```，則表示是系統用戶不能被登入；如果字符串為 ```!!```，則表示用戶被禁用，不能登陸；如果字符串為空，則表示沒有密碼。

可以通過 ```passwd -d UserName``` 清空一個用戶的口令密碼。

### 密碼解析

實際上，上述的加密算法就是用明文密碼和 salt (動態生成的隨機字符串)，然後通過 ```crypt()``` 函數完成加密，可以通過如下程序驗證。

{% highlight c %}
#define _XOPEN_SOURCE
#include <stdio.h>
#include <string.h>
#include <shadow.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if(argc < 3) {
        fprintf(stderr, "Usage: %s UserName Password\n", *argv);
        exit(EXIT_FAILURE);
    }
    char *user = argv[1];

    if (geteuid() != 0) {
        fprintf(stderr, "Must be root\n");
        exit(EXIT_FAILURE);
    }

    struct spwd *shd= getspnam(user);
    if (shd == NULL) {
        fprintf(stderr, "User \"%s\" doesn't exist\n", user);
        exit(EXIT_FAILURE);
    }

    char encrypted[128], *ptr, *salt;
    strncpy(encrypted, shd->sp_pwdp, sizeof(encrypted));

    salt = encrypted;
    ptr = strrchr(encrypted, '$');
    if (ptr == NULL)
        exit(EXIT_FAILURE);
    ptr++;
    *ptr = 0;

    printf("salt: %s\n         crypt: %s\n", salt, crypt(argv[2], salt));
    printf("shadowd passwd: %s\n", shd->sp_pwdp);
    return 0;
}
{% endhighlight %}

然後通過如下命令編譯並測試。

{% highlight text %}
$ gcc passwd.c -Wall -lcrypt -o passwd
$ ./passwd username yourpassword
{% endhighlight %}

<!--
對於示例的密碼域$1$Bg1H/4mz$X89TqH7tpi9dX1B9j5YsF.，我們參考了Linux標準源文件passwd.c，在其中的pw_encrypt函數中找到了加密方法。

    id為1時，採用md5進行加密；
    id為5時，採用SHA256進行加密；
    id為6時，採用SHA512進行加密。


可以嘗試使用 John the Ripper 來破解密碼。


包括了兩個主要的數據結構，struct passwd(對應/etc/passwd) + struct spwd(對應/etc/shadow) 。對於加密函數 char *crypt(const char *key, const char *salt)，其中 key 是一個明文密碼；salt 隨機生成用來輔助加密的字符串，同時決定了加密函數所採用的加密算法。<br><br>

其中函數中的 salt 採用與 /etc/shadow 中保存的相同格式，其中 $ID$ 分別對應 MD5($1$)、SHA256($5$)、SHA512($6$)，部分會支持 Blowfish($2a$)；$salt$ 通常為12bytes 。<br><br>

密碼驗證方法可以從示例的 verify.c 中獲取。

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

char data[37] = "abcdefghijklmnopqrstuvwxyz0123456789";
char *goalPass = "$1$AAODv...$gXPqGkIO3Cu6dnclE";
char mypwd[10];
int minlen = 1; //min password length
int maxlen = 3; //max password length




void subgenerate(int index, int pwdlen)
{
        if (index == pwdlen)
                return;
        int i;
        for (i = 0; i < 36; i++)
        {
                mypwd[index] = data[i];
                memset(mypwd + index + 1, data[0], pwdlen- index -1);
                if (i != 0)
                {
//                     printf("%s      ",mypwd);
                        if (!strcmp(goalPass, crypt(mypwd, "$1$AAODv...$")))
                        {
                                printf("find password:%s", mypwd);
                                exit(0);
                        }
                }
                subgenerate(index + 1, pwdlen);
        }
}

void generate(int pwdlen, int start, int end)
{
        int i;
        for (i = start; i < end; i++) // 多線程可分段
        {
                mypwd[0] = data[i];
                memset(mypwd + 1, data[0], pwdlen-1); //填充長度
//              printf("%s      ",mypwd);
                if (!strcmp(goalPass, crypt(mypwd, "$1$AAODv...$")))
                {
                        printf("find password:%s", mypwd);
                        exit(0);
                }
                subgenerate(1, pwdlen);
        }
}


int main()
{
        char mypwd[10];
        if (maxlen > 9) puts("max password length must little then 9");
        int i，threadnum = 10;
        for (i = minlen; i <= maxlen; i++)
        {
                printf("/npassword length:%d/n", i);
                //password length
                memset(mypwd, 0, 10);
                generate(i,0,36); //留作多線程
        }

        puts("password not found");
        return 0;
}
-->

## PAM

通常在用戶登陸時，需要有一套的驗證授權機制，如 Linux 用戶登陸時。最開始的時候，這一整套的驗證機制是硬編碼到程序中的，這樣當程序有 bug 或者需要修改驗證策略時，只能修改源程序。

為了改善這些問題，人們開始思考其他的方法，至此，```Pluggable Authentication Modules, PAM``` 應運而生了。也就是說，最早 Linux 是通過 login 完成用戶的登陸，後來為了方便配置使用 PAM 。

關於 PAM 可以參考 [官方網站](http://www.linux-pam.org/) ，包含了詳細的幫助文檔。

### 相關文件

在 PAM 中，包含了如下文件 ```/usr/lib64/libpam.so.*``` (PAM的核心庫)、```/etc/pam.d/``` (配置文件) 以及 ```/usr/lib64/security/pam_xxx.so``` (可以動態加載的模塊)。

配置文件有兩種方式，一種是單獨的配置文件，如 ```/etc/pam.conf```，其內容如下，第一個表示服務的名稱。

{% highlight text %}
ftpd auth required pam_unix.so nullok
{% endhighlight %}

在 CentOS 7 中，相關服務的配置保存在 ```/etc/pam.d/``` 目錄下，如 login、sshd 等。在配置文件中包含了四列：1) 模塊類型；2) 控制標記；3) 模塊路徑；4) 模塊參數，主要介紹前兩個。

#### 1. 模塊類型

PAM 有四種模塊類型，代表不同的任務，一個類型可能有多行，按順序依次由 PAM 模塊調用：

* 認證管理，auth<br>用來對用戶的身份進行識別，如提示用戶輸入密碼、判斷用戶是否為 root 等。

* 賬號管理，account<br>對帳號的各項屬性進行檢查，如是否允許登錄、是否達到最大用戶數、root 用戶是否允許在這個終端登錄等。

* 會話管理，session<br>定義用戶登錄前的及用戶退出後所要進行的操作，如登錄連接信息、用戶數據的打開與關閉、掛載文件系統等。

* 密碼管理，password<br>使用用戶信息來更新，如修改用戶密碼。

#### 2. 控制標記

通過控制標記來處理和判斷各個模塊的返回值。

* required<br>即使某個模塊對用戶的驗證失敗，也要等所有的模塊都執行完畢後才返回錯誤信息。
* requisite<br>如果某個模塊返回失敗，則立刻返回失敗，不再進行同類型後面的操作。
* sufficient<br>如果驗證通過，則立即返回驗證成功消息，無論前面模塊是否有失敗，而且也不再執行後面模塊。如果驗證失敗，sufficient 的作用和 optional 相同 。
* optional<br>即使指定的模塊驗證失敗，也允許用戶接受應用程序提供的服務，一般返回 PAM_IGNORE 。

### 常用模塊

{% highlight text %}
pam_unix.so
  auth       提示用戶輸入密碼，並與/etc/shadow文件相比對，匹配返回0。
  account    檢查用戶的賬號信息(如是否過期)，帳號可用時返回0。
  password   修改用戶的密碼，將用戶輸入的密碼，作為用戶的新密碼更新shadow文件。

pam_shells.so
  account    如果用戶想登錄系統，那麼它的shell必須是在/etc/shells中。

pam_cracklib.so
  password   提示用戶輸入密碼，並與系統中的字典進行比對，檢查密碼的強度。

pam_securetty.so
  auth       用戶要以root登錄時，則登錄的tty必須在/etc/securetty中。
{% endhighlight %}

### 示例程序

如下程序從命令行接收一個用戶名作為參數，然後對這個用戶名進行 auth 和 account 驗證。

{% highlight c %}
#include <stdio.h>
#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <security/pam_modules.h>

int main(int argc, char *argv[])
{
    pam_handle_t *pamh=NULL;
    int retval;
    const char *user="nobody";
    struct pam_conv conv = { misc_conv, NULL };

    if(argc == 2)
        user = argv[1];
    if(argc > 2) {
        fprintf(stderr, "Usage: %s [username]\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    printf("user: %s\n",user);
    retval = 0;

    retval = pam_start("pamtest", user, &conv, &pamh);
    if (retval == PAM_SUCCESS) {
        retval = pam_authenticate(pamh, 0); // 進行auth類型認證
    } else { // 認證出錯，則輸出錯誤信息
        printf("pam_authenticate(): %d\n", retval);
        printf("%s\n", pam_strerror(pamh, retval));
    }

    if (retval == PAM_SUCCESS) {
        retval = pam_acct_mgmt(pamh, 0);    // 進行account類型認證
    } else {
        printf("pam_acct_mgmt() : %d\n", retval);
        printf("%s\n", pam_strerror( pamh, retval));
    }

    if (retval == PAM_SUCCESS) {
        fprintf(stdout, "Authenticated\n");
    } else {
        fprintf(stdout, "Not Authenticated\n");
    }

    if (pam_end(pamh,retval) != PAM_SUCCESS) {     /* close Linux-PAM */
        pamh = NULL;
        fprintf(stderr, "pamtest0: failed to release authenticator\n");
        exit(EXIT_FAILURE);
    }

    return ( retval == PAM_SUCCESS ? 0:1 );       /* indicate success */
}
{% endhighlight %}

添加如下的配置文件，並同時通過如下命令編譯執行。

{% highlight text %}
# cat << EOF > /etc/pam.d/pamtest
# 提示用戶輸入密碼
auth     required   pam_unix.so
# 驗證用戶賬號是否可用
account  required   pam_unix.so
# 向系統日誌輸出一條信息
account  required   pam_warn.so
EOF

$ gcc -o pamtest pamtest.c -lpam -lpam_misc -Wall
{% endhighlight %}

## Python Keyring

在 Python 中，密碼保存以及獲取可以使用 keyring 模塊，可以從 [pypi keyring](https://pypi.python.org/pypi/keyring) 上下載，可以簡單通過 ```easy_install keyring``` 或者 ```pip install keyring``` 安裝。

{% highlight text %}
>>> import keyring
>>> keyring.set_password("system", "username", "password")
>>> keyring.get_password("system", "username")
'password'

>>> keyring.get_keyring()                                              // 查看當前使用的keyring
>>> keyring.set_keyring(keyring.backends.file.PlaintextKeyring())      // 設置keyring
{% endhighlight %}

<!--
以 CentOS 為例，設置完成之後可以通過 Passwords and Keys 查看(可以直接在終端輸入seahorse)，其加密後的文件保存在 ~/.local/share/keyrings 目錄下。
-->

對於 PlaintextKeyring，實際上只是通過 base64 保存，該值可以很容易解密；加密後的文件保存在 ```~/.local/share/python_keyring/keyring_pass.cfg``` 文件中。

{% highlight text %}
[system]
username = cGFzc3dvcmQ=
{% endhighlight %}

可以直接通過 ```base64.decodestring('cGFzc3dvcmQ=')``` 解密。

如果要使用強密碼加密措施，可以安裝 pycrypto 模塊，從官網下載，然後安裝。

{% highlight text %}
# python setup.py build && python setup.py install

>>> import keyring
>>> keyring.set_keyring(keyring.backends.file.EncryptedKeyring())      // 設置keyring
{% endhighlight %}

加密後的文件會保存在 ```~/.local/share/python_keyring/crypted_pass.cfg``` 文件中。

<!--
當導入 keyring 時，此時會執行 __init__.py ，然後是 core.py 的 init_backend() 函數，在此會列出有效的 keyring 。
-->

## 其它

簡單介紹其它的密碼保存方式。

### MySQL

MySQL 的認證密碼有兩種方式，MySQL-4.1 版本之前是 MySQL323 加密，之後的版本採用 MySQLSHA1 加密。

{% highlight text %}
mysql> select old_password("test");                                      // 老版本加密方式
mysql> select password("test");                                          // 新版本加密方式

mysql> select user, password from mysql.user;                            // 查看mysql中的應用名和密碼
mysql> update user set password=password('password') where user='root';  // 更新密碼
mysql> flush privileges;                                                 // 刷新權限
{% endhighlight %}

其中密碼保存在 mysql/user.MYD 文件中。

### Chrome FirFox

兩者都會保存密碼，並在需要的時候自動填充。

Chrome 密碼管理器的進入方式：右側扳手圖標 → 設置 → 顯示高級設置 → 密碼和表單 → 管理已保存的密碼；或者直接在地址欄中打開 [chrome://chrome/settings/passwords](chrome://chrome/settings/passwords)；Chrome 後端會用一些系統提供的密碼保存方式，不同平臺有所區別，如 Linux 中是 gnome-keyring 。

FireFox 通過自己的方式保存密碼，可以設置主密碼，通常密碼保存在 key3.db 中，如在 CentOS 中，保存在 ```~/.mozilla/firefox/cud38a2f.default/key3.db```，可以參考 [密碼管理器--在 Firefox 中記住、刪除和更改已保存的密碼](https://support.mozilla.org/zh-CN/kb/password-manager-remember-delete-change-passwords) 。

<!--
genrsa
### Google Chrome:
~/.config/google-chrome/Default/Login Data
### Chromium:
~/.config/chromium/Default/Login Data

### Google Chrome
    google-chrome --password-store=basic

### Chromium
    chromium --password-store=basic

### Google Chrome:
sqlite3 -header -csv -separator "," ~/.config/google-chrome/Default/Login\ Data "SELECT * FROM logins" > ~/Passwords.csv

### Chromium
sqlite3 -header -csv -separator "," ~/.config/chromium/Default/Login\ Data "SELECT * FROM logins" > ~/Passwords.csv


獲取WIFI和Chrome用戶名密碼
http://www.ftium4.com/chrome-cookies-encrypted-value-python.html
https://github.com/lijiejie/chromePass
http://www.lijiejie.com/python-get-chrome-all-saved-passwords/
https://github.com/hassaanaliw/chromepass/blob/master/chromepass.py
-->

### John the Ripper

一個開源的密碼破解工具，可以參考 [官方網站](http://www.openwall.com/john/) 。

{% highlight text %}
$ tar -xvf john-1.8.0.tar.xz && cd john-1.8.0/run
$ ./john --test

# unshadow /etc/passwd /etc/shadow &lt; mypasswd
$ ./john mypasswd
{% endhighlight %}

通過 ```make``` 可以查看所支持的系統，當然可以 ```make clean generic``` 產生通用的程序產生通用的程序產生通用的程序，產生的程序保存在 ```../run``` 目錄下，可以通過 ```john --test``` 測試。

<!--
http://blog.sina.com.cn/s/blog_64d959260100un5x.html
-->

### 使用OpenSSL加密

詳細可以參考 [OpenSSL使用指南](/reference/linux/OpenSSL.pdf) 。

{% highlight text %}
----- 使用Base64編碼方法(Base64 Encoding)進行加密/解密
$ echo "Welcome to Linux" | openssl enc -base64
$ echo "V2VsY29tZSB0byBMaW51eAo=" | openssl enc -base64 -d

----- 使用對稱加密
$ echo "Welcome to Linux" | openssl enc -aes-256-cbc -a         加密，需要祕密
enter aes-256-cbc encryption password: 123456
Verifying - enter aes-256-cbc encryption password:
U2FsdGVkX1+yYQz9vEKnm56GoXo7F3I/kHP2C5KW4lqBcneAeDIXa6WU29pYhPAL
$ echo "U2FsdGVkX1+yYQz9vEKnm56GoXo7F3I/kHP2C5KW4lqBcneAeDIXa6WU29pYhPAL" | openssl enc -aes-256-cbc -d -a
enter aes-256-cbc decryption password: 123456
Welcome to Linux

----- 輸出到文件，然後解密
$ echo "OpenSSL" | openssl enc -aes-256-cbc > openssl.dat
$ openssl enc -aes-256-cbc -d -in openssl.dat
----- 加密文件
$ openssl enc -aes-256-cbc -in /etc/services -out services.dat
$ openssl enc -aes-256-cbc -d -in services.dat > services.txt
----- 加密文件夾
$ tar cz /etc | openssl enc -aes-256-cbc -out etc.tar.gz.dat
$ openssl enc -aes-256-cbc -d -in etc.tar.gz.dat | tar xz

----- 使用公私鑰非對稱加密
------- 1. 生成私鑰，1024指定長度(bit)，默認512
$ openssl genrsa -out test.key 1024
Generating RSA private key, 1024 bit long modulus
.......++++++
....................................++++++
e is 65537 (0x10001)
------- 2. 生成公鑰
$ openssl rsa -in test.key -pubout -out test_pub.key
writing RSA key
------- 3. 使用公鑰加密，-in(要加密文件) -inkey(公鑰) -pubin(用純公鑰文件加密) -out(加密後文件)
$ openssl rsautl -encrypt -in hello.txt -inkey test_pub.key -pubin -out hello.en
------- 4. 使用私鑰解密，-in(被加密文件) -inkey(私鑰) -out(解密後文件)
$ openssl rsautl -decrypt -in hello.en -inkey test.key -out hello.de
{% endhighlight %}


## 參考

一個 Solaris 的開發指南，可以供參考 [編寫 PAM 應用程序和服務](http://docs.oracle.com/cd/E19253-01/819-7056/6n91eac3n/index.html) 或者 [本地文檔](/reference/linux/pam_compile_new_module.maff) 。

安全審計工具可以參考 [cisofy.com/lynis](https://cisofy.com/lynis/) ； 關於破解工具可以參考 [THC-Hydra](https://www.thc.org/thc-hydra/) 支持多種協議。

關於存儲密碼的方法可以參考 [Storing User Passwords Securely: hashing, salting, and Bcrypt](http://dustwell.com/how-to-handle-passwords-bcrypt.html) 或者 [本地文檔](/reference/linux/Storing_User_Passwords_Securely.maff) 。一篇很不錯的介紹數字簽名的文章 [What is a Digital Signature?](http://www.youdzone.com/signature.html) 或者 [本地文檔](/reference/linux/What_is_a_Digital_Signature.maff) 。

<!--
常見的破解工具可以參考 <a href="http://sectools.org/tag/pass-audit/">Top 125 Network Security Tools</a> 。
-->

{% highlight text %}
{% endhighlight %}
