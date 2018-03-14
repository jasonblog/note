---
title: Nginx https
layout: post
comments: true
language: chinese
category: [linux,webserver]
keywords: nginx,https,webserver
description: 簡單介紹如何使用 Nginx 搭建 https 服務。
---

簡單介紹如何使用 Nginx 搭建 https 服務。

<!-- more -->

## 搭建 HTTPS 服務

在 CentOS 中，默認根目錄保存在 ```/usr/share/nginx/html``` 目錄下。

### 1. 確認支持 SSL

通過 ```-V``` 參數查看編譯時是否添加了 ```--with-http_ssl_module``` 參數，對於 CentOS 來說，一般是已經安裝了的。

### 2. 生成證書

可以通過以下步驟生成一個簡單的證書。

{% highlight text %}
----- 在tmp目錄下創建證書和私鑰保存的目錄
$ mkdir /tmp/nginx && cd /tmp/nginx

----- 1. 創建服務器私鑰，需要輸入一個口令
$ openssl genrsa -des3 -out server.key 1024
Generating RSA private key, 1024 bit long modulus
..++++++
.........++++++
e is 65537 (0x10001)
Enter pass phrase for server.key: Tools@123

----- 2. 創建簽名請求的證書(CSR)
$ openssl req -new -key server.key -out server.csr
Enter pass phrase for server.key:
... ...
Country Name (2 letter code) [XX]:cn                                     ← 國家
State or Province Name (full name) []:shandong                           ← 省份
Locality Name (eg, city) [Default City]:jinan                            ← 城市
Organization Name (eg, company) [Default Company Ltd]:foobar             ← 公司
Organizational Unit Name (eg, section) []:sys                            ← 部門
Common Name (eg, your name or your server's hostname) []:www.foobar.com  ← 主機名稱
Email Address []:bar@foobar.com                                          ← 郵箱
Please enter the following 'extra' attributes
to be sent with your certificate request
A challenge password []:1234           ← 證書請求密鑰，CA讀取證書的時候需要輸入密碼
An optional company name []:nicai      ← 公司名稱，CA讀取證書的時候需要輸入密碼

----- 3. 加載SSL支持的Nginx並使用上述私鑰時除去必須的口令
$ cp server.key server.key.org
$ openssl rsa -in server.key.org -out server.key

----- 4. 標記證書使用上述私鑰和CSR
$ openssl x509 -req -days 365 -in server.csr -signkey server.key -out server.crt
{% endhighlight %}

### 3. 配置 Nginx

修改 Nginx 配置文件，讓其包含新標記的證書和私鑰。

{% highlight text %}
server {
    ssl on;
    listen 443;
    server_name www.foobar.com;
    ssl_certificate /tmp/nginx/server.crt;
    ssl_certificate_key /tmp/nginx/server.key;
}
{% endhighlight %}

### 4. 重啟 Nginx

需要注意的是，如果採用 ```systemctl``` 重啟，上述的配置不能保存在 tmp 目錄下，因為 systemd 會為進程啟動不同的 tmp 目錄，從而導致無法查找到上述的配置文件。

這樣就可以訪問 [https://www.foobar.com](https://www.foobar.com)，注意需要將 ```www.foobar.com``` 添加到 ```/etc/hosts``` 中。另外，還可以加入如下代碼實現 80 端口重定向到 443。

{% highlight text %}
server {
    listen 80;
    server_name www.foobar.com;
    rewrite ^(.*) https://$server_name$1 permanent;
}
{% endhighlight %}

### 5. 其它

在 2.3 中，可不去掉密鑰口令，此時會在重啟 nginx 輸入密碼，而且此時 systemctl 將會失效。

## 參考

{% highlight text %}
{% endhighlight %}
