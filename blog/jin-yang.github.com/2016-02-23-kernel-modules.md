---
title: Linux 內核模塊
layout: post
comments: true
language: chinese
category: [linux]
keywords: linux,內核模塊,kernel,module
description: 簡單介紹下 Linux 中的內核模塊編寫，包括了內核簽名機制的配置。
---

簡單介紹下 Linux 中的內核模塊編寫，包括了內核簽名機制的配置。

<!-- more -->

## 簡單示例

一個很簡單的 helloworld 程序，可以參考 [github LKM helloworld]({{ site.example_repository }}/linux/LKM) 。

如下是 Makefile 文件。

{% highlight makefile %}
ifneq	($(KERNELRELEASE),)
obj-m := hello.o
else
KERNEL_DIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
all:
	make -C $(KERNEL_DIR) SUBDIRS=$(PWD) modules
#    rm -r -f .tmp_versions *.mod.c .*.cmd *.o *.symvers
endif

clean:
	rm -rf *.o *.ko *.mod.c *.order *.symvers .*.cmd .tmp_versions
.PHONY:clean
{% endhighlight %}

下面是驅動的測試程序。

{% highlight c %}
/* FILE: hello.c */
#include <linux/init.h>
#include <linux/module.h>

static int __init hello_init(void)
{
    printk(KERN_ALERT "hello module!\n");
    return 0;
}

static void __exit hello_exit(void)
{
    printk(KERN_ALERT "bye module!\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Andy <justkidding@gmail.com>");
MODULE_DESCRIPTION("A simple Hello World Module");
MODULE_ALIAS("the simplest module");
{% endhighlight %}

然後可以通過如的方式進行測試。

{% highlight text %}
# insmod hello.ko
# lsmod
# dmesg | tail -5
Module                  Size  Used by
hello                  12496  0
# rmmod hello
{% endhighlight %}



## 內核簽名機制

在插入到內核模塊時，可能會報如下錯誤。

{% highlight text %}
# insmod hello.ko
insmod: ERROR: could not insert module hello.ko: Required key not available
{% endhighlight %}

原因是內核啟用了 [Module signature verification](https://lwn.net/Articles/222162/) ，可以通過如下命令檢測內核配置項。

{% highlight text %}
----- 查看內核的配置項
$ grep "CONFIG_MODULE_SIG" /boot/config-`uname -r`

----- 查看當前系統key
# keyctl list %:.system_keyring
{% endhighlight %}

內核啟動時，會有類似如下的輸出。

{% highlight text %}
Loaded X.509 cert 'CentOS Linux kpatch signing key: xxxx'
Loaded X.509 cert 'CentOS Linux Driver update signing key: xxxx'
Loaded X.509 cert 'CentOS Linux kernel signing key: xxxx'
EFI: Loaded cert 'Lenovo Ltd.: ThinkPad Product CA 2012: xxxx' linked to '.system_keyring'
EFI: Loaded cert 'Lenovo UEFI CA 2014: xxxx' linked to '.system_keyring'
{% endhighlight %}

主要是由於目前 BIOS 支持 EFI，如果支持 UEFI Secure Boot 啟動，那麼內核所有模塊都必須使用 UEFI Secure key 簽名；當然，如果 BIOS 支持關閉 UEFI Secure Boot，那麼可以在 BIOS 的 boot 項中關閉 UEFI Secure Boot 。

否則只能為自己製作一個。

接下來看看如何使用，主要包括瞭如下工具：

* openssl，生成X509公私祕鑰對。
* sign-file，對內核模塊使用X509公私祕鑰對簽名。
* mokutil，手動註冊公鑰到系統。
* keyctl，手動取消註冊公鑰到系統。

下面看看如何設置。

### 配置示例

#### 1. 生成配置文件

{% highlight text %}
# cat << EOF > configuration_file.config
[ req ]
default_bits = 4096
distinguished_name = req_distinguished_name
prompt = no
string_mask = utf8only
x509_extensions = myexts

[ req_distinguished_name ]
O = Organization
CN = Organization signing key
emailAddress = E-mail address

[ myexts ]
basicConstraints=critical,CA:FALSE
keyUsage=digitalSignature
subjectKeyIdentifier=hash
authorityKeyIdentifier=keyid
EOF
{% endhighlight %}

#### 2. 生成祕鑰

一般會把公私鑰放在 ``` /usr/src/kernels/`uname -r` ``` 文件夾中，不過還是建議方法 HOME 目錄下。

{% highlight text %}
$ openssl req -x509 -new -nodes -utf8 -sha256 -days 36500 \
  -batch -config configuration_file.config -outform DER \
  -out public_key.der -keyout private_key.priv
{% endhighlight %}

#### 3. 導入到 mok 列表

上步生成了一對公私鑰，接下來將其添加到 mok 列表中，此時會需要輸入密碼，該密碼會在確認 MOK 請求的時候輸入。

{% highlight text %}
# mokutil --import public_key.der
{% endhighlight %}

#### 4. 重啟系統

{% highlight text %}
# shutdown -r now
{% endhighlight %}

在啟動時，shim.efi 會發現新添加的 KEY，並會啟動 MokManager.efi 模塊，此時需要選擇 Eroll Key 選項，然後輸入上面的密碼。

#### 5. 查看 key ring

接著 key ring 會添加到內核中，其中描述是配置文件中指定的 req_distinguished_name.O 中。

{% highlight text %}
# keyctl list %:.system_keyring | grep "Organization"
# cat /proc/keys
{% endhighlight %}

同樣，也可以查看系統的啟動日誌。

{% highlight text %}
# dmesg | grep 'EFI: Loaded cert'
{% endhighlight %}

#### 6. 添加到模塊中

{% highlight text %}
/usr/src/kernels/$(uname -r)/scripts/sign-file sha256 private_key.priv public_key.der hello.ko
{% endhighlight %}

#### 7. 添加 hello.ko

仍然同上，直接插入模塊即可。

{% highlight text %}
# insmod hello.ko
{% endhighlight %}

<!--
  7. I ended up signing the kernel modules of VirtualBox with the following for loop:

  for i in /usr/lib/modules/$(uname -r)/extra/VirtualBox/*ko; do
        sudo /usr/src/kernels/$(uname -r)/scripts/sign-file sha256 private_key.priv  public_key.der "$i";
done

        8. VirtualBox will happily run virtual machines. ==> Done!

        I expect VirtualBox to break everytime an update for VirtualBox is released. In that case, you will have to repeat step 7. I am not sure what the effect of a shim update will be.
-->

## PROC 文件系統

最初 proc 文件系統是為提供一種內核及其模塊向進程 (process) 發送信息的機制，這就是 proc 名字的由來。通過 proc 可以和內核內部數據結構進行交互，獲取對於進程的有用信息，並可在運行時改變設置內核參數。

後來，這個系統的使用有些混亂和失控，於是在新內核中建議使用 sysfs 實現內核信息向用戶空間的導出，例如，模塊參數。


{% highlight text %}
$ ls /sys/module/hid/parameters               # 查看HID(Human Interface Devices)的參數
{% endhighlight %}

Linux 內核和用戶空間的通信可通過 /proc 目錄下的文件實現，編譯時需要 ```CONFIG_PROC_FS``` 配置，通常來說，可寫的配置項一般保存在 /proc/sys 目錄下。其它介紹如下：

#### 1. 進程相關的目錄

proc 目錄下以 PID 作為目錄，存儲了進程相關信息，可以通過 cat 查看。

{% highlight text %}
$ cat /proc/1/cmdline              # 查看init命令啟動參數
{% endhighlight %}

#### 2. 通用系統信息

包括了內存、文件系統、設備驅動、系統總線、電源等管理，如 devices、diskstats、meminfo、modules 等。

#### 3. 系統控制信息

用來檢測修改系統的運行參數，存在於 ```/proc/sys``` 目錄下，可以使用 cat、echo 來查看或修改系統的運行參數。不過此處修改只是臨時，如果要持久化需要修改 ```/etc/sysctl.conf``` 中的配置。

接下來看看內核中的實現。

### 內核實現

相關的 API 實現在 ```fs/proc/generic.c``` 文件中，常見的 ```/proc/meminfo```、```/proc/stat``` 等統計項的源碼在 fs/proc 目錄下，具體的實現可以查看這裡的源碼。

很多簡單的示例可以參考源碼中的 loadavg.c 實現。



## 參考

關於 Signed Kernel Modules 可以參考 Gentoo 中的文檔 [Signed kernel module support](https://wiki.gentoo.org/wiki/Signed_kernel_module_support)，或者 [本地保存的文檔](/reference/linux/Signed kernel module support.mht) 。

關於 proc 的文檔，可參考源碼 Documentation/filesystems 目錄下的 [seq_file.txt]({{ site.kernel_docs_url }}/Documentation/filesystems/seq_file.txt) 以及 [proc.txt]({{ site.kernel_docs_url }}/Documentation/filesystems/proc.txt) 。


{% highlight text %}
{% endhighlight %}
