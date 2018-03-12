---
title: Linux 密码管理
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: linux,密码,管理
description: 简单介绍下 Linux 中的密码管理。
---

简单介绍下 Linux 中的密码管理。

<!-- more -->

## 简介

常见的加密工具有，seahorse([gnome-keyring](https://wiki.gnome.org/Projects/GnomeKeyring))、[Password Safe](https://www.schneier.com/passsafe.html)、LastPass (跨平台的密码保存工具)、keepass (跨平台的密码保存机制)，在 CentOS 中可以通过 ```yum install keepassx``` 安装。

建议使用 keepass，这样只需要记住密码和对应的库文件即可，然后可以在其它平台上打开。

在 gnome-keyring 官网中，包括了详细的介绍。

### 加密方法

密码算法都是公开的，保密应该依赖于密钥的保密，而不是算法的保密。

对称加密，密钥可同时用于加解密，一般密钥会直接出现在加密代码中，破解的可能性相当大，而且系统管理员很可能知道密钥，进而算出密码原文。由于密钥需要保密，因此需要事先秘密约定，或者用额外的安全信道来传输。

非对称加密，密码的安全性等同于私钥的安全性。密码明文经过公钥加密，要还原密码明文，必须要相应的私钥才行。因此只要保证私钥的安全，密码明文就安全。

<!--
通常利用哈希算法的单向性来保证明文以不可还原的有损方式进行存储。

MD5 和 SHA-1 已破解，虽不能还原明文，但很容易找到能生成相同哈希值的替代明文。而且这两个算法速度较快，暴力破解相对省时，建议不要使用它们。

使用更安全的 SHA-256 等成熟算法，更加复杂的算法增加了暴力破解的难度。但如果遇到简单密码，用彩虹字典的暴力破解法，很快就能得到密码原文。

加入随机 salt 的哈希算法
    密码原文（或经过 hash 后的值）和随机生成的 salt 字符串混淆，然后再进行 hash，最后把 hash 值和 salt 值一起存储。验证密码的时候只要用 salt 再与密码原文做一次相同步骤的运算，比较结果与存储的 hash 值就可以了。这样一来哪怕是简单的密码，在进过 salt 混淆后产生的也是很不常见的字符串，根本不会出现在彩虹字典中。salt 越长暴力破解的难度越大

    具体的 hash 过程也可以进行若干次叠代，虽然 hash 叠代会增加碰撞率，但也增加暴力破解的资源消耗。就算真被破解了，黑客掌握的也只是这个随机 salt 混淆过的密码，用户原始密码依然安全，不用担心其它使用相同密码的应用。

上面这几种方法都不可能得到密码的明文，就算是系统管理员也没办法。对于那些真的忘了密码的用户，网站只能提供重置密码的功能了。

下面的 python 程序演示了如何使用 salt 加 hash 来单向转换密码明文

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

这里先随机生成 64 bits 的 salt，再选择 SHA-256 算法使用 HMAC 对密码和 salt 进行 10 次叠代混淆，最后将 salt 和 hash 结果一起返回。

使用的方法很简单：

hashed = encrypt_password('secret password')

下面是验证函数，它直接使用 encrypt_password 来对密码进行相同的单向转换并比较

def validate_password(hashed, input_password):
    return hashed == encrypt_password(input_password, salt=hashed[:8])

assert validate_password(hashed, 'secret password')

虽然只有简短几行，但借助 python 标准库帮助，这已经是一个可用于生产环境的高安全密码加密验证算法了。

总结一下用户密码的存储：
    上善不战而屈人之兵。如果可能不要存任何密码信息 让别人（OpenID）来帮你做事，避开这个问题
    如果非要自己认证，也只能存 不可逆的有损密码信息 。通过单向 hash 和 salt 来保证只有用户知道密码明文
    绝对不能存可还原密码原文的信息 。如果因为种种原因一定要可还原密码原文，请使用非对称加密，并保管好私钥
-->



## Linux Shadow

Linux 通过 libcrypt 中的 ```man 3 crypt``` 函数对密码进行加密，该函数在 glibc 库中。在 ```/etc/shadow``` 中保存的格式为 ```$id$salt$encrypted```，可以通过上述的 man 查看其含义。

接下来看看 ```/etc/shadow``` 文件的内容：

{% highlight text %}
root:$1$Bg1H/4mz$X89TqH7tpi9dX1B9j5YsF.:14838:0:99999:7:::
{% endhighlight %}

如果密码字符串为 ```*```，则表示是系统用户不能被登入；如果字符串为 ```!!```，则表示用户被禁用，不能登陆；如果字符串为空，则表示没有密码。

可以通过 ```passwd -d UserName``` 清空一个用户的口令密码。

### 密码解析

实际上，上述的加密算法就是用明文密码和 salt (动态生成的随机字符串)，然后通过 ```crypt()``` 函数完成加密，可以通过如下程序验证。

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

然后通过如下命令编译并测试。

{% highlight text %}
$ gcc passwd.c -Wall -lcrypt -o passwd
$ ./passwd username yourpassword
{% endhighlight %}

<!--
对于示例的密码域$1$Bg1H/4mz$X89TqH7tpi9dX1B9j5YsF.，我们参考了Linux标准源文件passwd.c，在其中的pw_encrypt函数中找到了加密方法。

    id为1时，采用md5进行加密；
    id为5时，采用SHA256进行加密；
    id为6时，采用SHA512进行加密。


可以尝试使用 John the Ripper 来破解密码。


包括了两个主要的数据结构，struct passwd(对应/etc/passwd) + struct spwd(对应/etc/shadow) 。对于加密函数 char *crypt(const char *key, const char *salt)，其中 key 是一个明文密码；salt 随机生成用来辅助加密的字符串，同时决定了加密函数所采用的加密算法。<br><br>

其中函数中的 salt 采用与 /etc/shadow 中保存的相同格式，其中 $ID$ 分别对应 MD5($1$)、SHA256($5$)、SHA512($6$)，部分会支持 Blowfish($2a$)；$salt$ 通常为12bytes 。<br><br>

密码验证方法可以从示例的 verify.c 中获取。

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
        for (i = start; i < end; i++) // 多线程可分段
        {
                mypwd[0] = data[i];
                memset(mypwd + 1, data[0], pwdlen-1); //填充长度
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
                generate(i,0,36); //留作多线程
        }

        puts("password not found");
        return 0;
}
-->

## PAM

通常在用户登陆时，需要有一套的验证授权机制，如 Linux 用户登陆时。最开始的时候，这一整套的验证机制是硬编码到程序中的，这样当程序有 bug 或者需要修改验证策略时，只能修改源程序。

为了改善这些问题，人们开始思考其他的方法，至此，```Pluggable Authentication Modules, PAM``` 应运而生了。也就是说，最早 Linux 是通过 login 完成用户的登陆，后来为了方便配置使用 PAM 。

关于 PAM 可以参考 [官方网站](http://www.linux-pam.org/) ，包含了详细的帮助文档。

### 相关文件

在 PAM 中，包含了如下文件 ```/usr/lib64/libpam.so.*``` (PAM的核心库)、```/etc/pam.d/``` (配置文件) 以及 ```/usr/lib64/security/pam_xxx.so``` (可以动态加载的模块)。

配置文件有两种方式，一种是单独的配置文件，如 ```/etc/pam.conf```，其内容如下，第一个表示服务的名称。

{% highlight text %}
ftpd auth required pam_unix.so nullok
{% endhighlight %}

在 CentOS 7 中，相关服务的配置保存在 ```/etc/pam.d/``` 目录下，如 login、sshd 等。在配置文件中包含了四列：1) 模块类型；2) 控制标记；3) 模块路径；4) 模块参数，主要介绍前两个。

#### 1. 模块类型

PAM 有四种模块类型，代表不同的任务，一个类型可能有多行，按顺序依次由 PAM 模块调用：

* 认证管理，auth<br>用来对用户的身份进行识别，如提示用户输入密码、判断用户是否为 root 等。

* 账号管理，account<br>对帐号的各项属性进行检查，如是否允许登录、是否达到最大用户数、root 用户是否允许在这个终端登录等。

* 会话管理，session<br>定义用户登录前的及用户退出后所要进行的操作，如登录连接信息、用户数据的打开与关闭、挂载文件系统等。

* 密码管理，password<br>使用用户信息来更新，如修改用户密码。

#### 2. 控制标记

通过控制标记来处理和判断各个模块的返回值。

* required<br>即使某个模块对用户的验证失败，也要等所有的模块都执行完毕后才返回错误信息。
* requisite<br>如果某个模块返回失败，则立刻返回失败，不再进行同类型后面的操作。
* sufficient<br>如果验证通过，则立即返回验证成功消息，无论前面模块是否有失败，而且也不再执行后面模块。如果验证失败，sufficient 的作用和 optional 相同 。
* optional<br>即使指定的模块验证失败，也允许用户接受应用程序提供的服务，一般返回 PAM_IGNORE 。

### 常用模块

{% highlight text %}
pam_unix.so
  auth       提示用户输入密码，并与/etc/shadow文件相比对，匹配返回0。
  account    检查用户的账号信息(如是否过期)，帐号可用时返回0。
  password   修改用户的密码，将用户输入的密码，作为用户的新密码更新shadow文件。

pam_shells.so
  account    如果用户想登录系统，那么它的shell必须是在/etc/shells中。

pam_cracklib.so
  password   提示用户输入密码，并与系统中的字典进行比对，检查密码的强度。

pam_securetty.so
  auth       用户要以root登录时，则登录的tty必须在/etc/securetty中。
{% endhighlight %}

### 示例程序

如下程序从命令行接收一个用户名作为参数，然后对这个用户名进行 auth 和 account 验证。

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
        retval = pam_authenticate(pamh, 0); // 进行auth类型认证
    } else { // 认证出错，则输出错误信息
        printf("pam_authenticate(): %d\n", retval);
        printf("%s\n", pam_strerror(pamh, retval));
    }

    if (retval == PAM_SUCCESS) {
        retval = pam_acct_mgmt(pamh, 0);    // 进行account类型认证
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

添加如下的配置文件，并同时通过如下命令编译执行。

{% highlight text %}
# cat << EOF > /etc/pam.d/pamtest
# 提示用户输入密码
auth     required   pam_unix.so
# 验证用户账号是否可用
account  required   pam_unix.so
# 向系统日志输出一条信息
account  required   pam_warn.so
EOF

$ gcc -o pamtest pamtest.c -lpam -lpam_misc -Wall
{% endhighlight %}

## Python Keyring

在 Python 中，密码保存以及获取可以使用 keyring 模块，可以从 [pypi keyring](https://pypi.python.org/pypi/keyring) 上下载，可以简单通过 ```easy_install keyring``` 或者 ```pip install keyring``` 安装。

{% highlight text %}
>>> import keyring
>>> keyring.set_password("system", "username", "password")
>>> keyring.get_password("system", "username")
'password'

>>> keyring.get_keyring()                                              // 查看当前使用的keyring
>>> keyring.set_keyring(keyring.backends.file.PlaintextKeyring())      // 设置keyring
{% endhighlight %}

<!--
以 CentOS 为例，设置完成之后可以通过 Passwords and Keys 查看(可以直接在终端输入seahorse)，其加密后的文件保存在 ~/.local/share/keyrings 目录下。
-->

对于 PlaintextKeyring，实际上只是通过 base64 保存，该值可以很容易解密；加密后的文件保存在 ```~/.local/share/python_keyring/keyring_pass.cfg``` 文件中。

{% highlight text %}
[system]
username = cGFzc3dvcmQ=
{% endhighlight %}

可以直接通过 ```base64.decodestring('cGFzc3dvcmQ=')``` 解密。

如果要使用强密码加密措施，可以安装 pycrypto 模块，从官网下载，然后安装。

{% highlight text %}
# python setup.py build && python setup.py install

>>> import keyring
>>> keyring.set_keyring(keyring.backends.file.EncryptedKeyring())      // 设置keyring
{% endhighlight %}

加密后的文件会保存在 ```~/.local/share/python_keyring/crypted_pass.cfg``` 文件中。

<!--
当导入 keyring 时，此时会执行 __init__.py ，然后是 core.py 的 init_backend() 函数，在此会列出有效的 keyring 。
-->

## 其它

简单介绍其它的密码保存方式。

### MySQL

MySQL 的认证密码有两种方式，MySQL-4.1 版本之前是 MySQL323 加密，之后的版本采用 MySQLSHA1 加密。

{% highlight text %}
mysql> select old_password("test");                                      // 老版本加密方式
mysql> select password("test");                                          // 新版本加密方式

mysql> select user, password from mysql.user;                            // 查看mysql中的应用名和密码
mysql> update user set password=password('password') where user='root';  // 更新密码
mysql> flush privileges;                                                 // 刷新权限
{% endhighlight %}

其中密码保存在 mysql/user.MYD 文件中。

### Chrome FirFox

两者都会保存密码，并在需要的时候自动填充。

Chrome 密码管理器的进入方式：右侧扳手图标 → 设置 → 显示高级设置 → 密码和表单 → 管理已保存的密码；或者直接在地址栏中打开 [chrome://chrome/settings/passwords](chrome://chrome/settings/passwords)；Chrome 后端会用一些系统提供的密码保存方式，不同平台有所区别，如 Linux 中是 gnome-keyring 。

FireFox 通过自己的方式保存密码，可以设置主密码，通常密码保存在 key3.db 中，如在 CentOS 中，保存在 ```~/.mozilla/firefox/cud38a2f.default/key3.db```，可以参考 [密码管理器--在 Firefox 中记住、删除和更改已保存的密码](https://support.mozilla.org/zh-CN/kb/password-manager-remember-delete-change-passwords) 。

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


获取WIFI和Chrome用户名密码
http://www.ftium4.com/chrome-cookies-encrypted-value-python.html
https://github.com/lijiejie/chromePass
http://www.lijiejie.com/python-get-chrome-all-saved-passwords/
https://github.com/hassaanaliw/chromepass/blob/master/chromepass.py
-->

### John the Ripper

一个开源的密码破解工具，可以参考 [官方网站](http://www.openwall.com/john/) 。

{% highlight text %}
$ tar -xvf john-1.8.0.tar.xz && cd john-1.8.0/run
$ ./john --test

# unshadow /etc/passwd /etc/shadow &lt; mypasswd
$ ./john mypasswd
{% endhighlight %}

通过 ```make``` 可以查看所支持的系统，当然可以 ```make clean generic``` 产生通用的程序产生通用的程序产生通用的程序，产生的程序保存在 ```../run``` 目录下，可以通过 ```john --test``` 测试。

<!--
http://blog.sina.com.cn/s/blog_64d959260100un5x.html
-->

### 使用OpenSSL加密

详细可以参考 [OpenSSL使用指南](/reference/linux/OpenSSL.pdf) 。

{% highlight text %}
----- 使用Base64编码方法(Base64 Encoding)进行加密/解密
$ echo "Welcome to Linux" | openssl enc -base64
$ echo "V2VsY29tZSB0byBMaW51eAo=" | openssl enc -base64 -d

----- 使用对称加密
$ echo "Welcome to Linux" | openssl enc -aes-256-cbc -a         加密，需要秘密
enter aes-256-cbc encryption password: 123456
Verifying - enter aes-256-cbc encryption password:
U2FsdGVkX1+yYQz9vEKnm56GoXo7F3I/kHP2C5KW4lqBcneAeDIXa6WU29pYhPAL
$ echo "U2FsdGVkX1+yYQz9vEKnm56GoXo7F3I/kHP2C5KW4lqBcneAeDIXa6WU29pYhPAL" | openssl enc -aes-256-cbc -d -a
enter aes-256-cbc decryption password: 123456
Welcome to Linux

----- 输出到文件，然后解密
$ echo "OpenSSL" | openssl enc -aes-256-cbc > openssl.dat
$ openssl enc -aes-256-cbc -d -in openssl.dat
----- 加密文件
$ openssl enc -aes-256-cbc -in /etc/services -out services.dat
$ openssl enc -aes-256-cbc -d -in services.dat > services.txt
----- 加密文件夹
$ tar cz /etc | openssl enc -aes-256-cbc -out etc.tar.gz.dat
$ openssl enc -aes-256-cbc -d -in etc.tar.gz.dat | tar xz

----- 使用公私钥非对称加密
------- 1. 生成私钥，1024指定长度(bit)，默认512
$ openssl genrsa -out test.key 1024
Generating RSA private key, 1024 bit long modulus
.......++++++
....................................++++++
e is 65537 (0x10001)
------- 2. 生成公钥
$ openssl rsa -in test.key -pubout -out test_pub.key
writing RSA key
------- 3. 使用公钥加密，-in(要加密文件) -inkey(公钥) -pubin(用纯公钥文件加密) -out(加密后文件)
$ openssl rsautl -encrypt -in hello.txt -inkey test_pub.key -pubin -out hello.en
------- 4. 使用私钥解密，-in(被加密文件) -inkey(私钥) -out(解密后文件)
$ openssl rsautl -decrypt -in hello.en -inkey test.key -out hello.de
{% endhighlight %}


## 参考

一个 Solaris 的开发指南，可以供参考 [编写 PAM 应用程序和服务](http://docs.oracle.com/cd/E19253-01/819-7056/6n91eac3n/index.html) 或者 [本地文档](/reference/linux/pam_compile_new_module.maff) 。

安全审计工具可以参考 [cisofy.com/lynis](https://cisofy.com/lynis/) ； 关于破解工具可以参考 [THC-Hydra](https://www.thc.org/thc-hydra/) 支持多种协议。

关于存储密码的方法可以参考 [Storing User Passwords Securely: hashing, salting, and Bcrypt](http://dustwell.com/how-to-handle-passwords-bcrypt.html) 或者 [本地文档](/reference/linux/Storing_User_Passwords_Securely.maff) 。一篇很不错的介绍数字签名的文章 [What is a Digital Signature?](http://www.youdzone.com/signature.html) 或者 [本地文档](/reference/linux/What_is_a_Digital_Signature.maff) 。

<!--
常见的破解工具可以参考 <a href="http://sectools.org/tag/pass-audit/">Top 125 Network Security Tools</a> 。
-->

{% highlight text %}
{% endhighlight %}
