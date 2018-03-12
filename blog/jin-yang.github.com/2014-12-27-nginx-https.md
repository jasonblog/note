---
title: Nginx https
layout: post
comments: true
language: chinese
category: [linux,webserver]
keywords: nginx,https,webserver
description: 简单介绍如何使用 Nginx 搭建 https 服务。
---

简单介绍如何使用 Nginx 搭建 https 服务。

<!-- more -->

## 搭建 HTTPS 服务

在 CentOS 中，默认根目录保存在 ```/usr/share/nginx/html``` 目录下。

### 1. 确认支持 SSL

通过 ```-V``` 参数查看编译时是否添加了 ```--with-http_ssl_module``` 参数，对于 CentOS 来说，一般是已经安装了的。

### 2. 生成证书

可以通过以下步骤生成一个简单的证书。

{% highlight text %}
----- 在tmp目录下创建证书和私钥保存的目录
$ mkdir /tmp/nginx && cd /tmp/nginx

----- 1. 创建服务器私钥，需要输入一个口令
$ openssl genrsa -des3 -out server.key 1024
Generating RSA private key, 1024 bit long modulus
..++++++
.........++++++
e is 65537 (0x10001)
Enter pass phrase for server.key: Tools@123

----- 2. 创建签名请求的证书(CSR)
$ openssl req -new -key server.key -out server.csr
Enter pass phrase for server.key:
... ...
Country Name (2 letter code) [XX]:cn                                     ← 国家
State or Province Name (full name) []:shandong                           ← 省份
Locality Name (eg, city) [Default City]:jinan                            ← 城市
Organization Name (eg, company) [Default Company Ltd]:foobar             ← 公司
Organizational Unit Name (eg, section) []:sys                            ← 部门
Common Name (eg, your name or your server's hostname) []:www.foobar.com  ← 主机名称
Email Address []:bar@foobar.com                                          ← 邮箱
Please enter the following 'extra' attributes
to be sent with your certificate request
A challenge password []:1234           ← 证书请求密钥，CA读取证书的时候需要输入密码
An optional company name []:nicai      ← 公司名称，CA读取证书的时候需要输入密码

----- 3. 加载SSL支持的Nginx并使用上述私钥时除去必须的口令
$ cp server.key server.key.org
$ openssl rsa -in server.key.org -out server.key

----- 4. 标记证书使用上述私钥和CSR
$ openssl x509 -req -days 365 -in server.csr -signkey server.key -out server.crt
{% endhighlight %}

### 3. 配置 Nginx

修改 Nginx 配置文件，让其包含新标记的证书和私钥。

{% highlight text %}
server {
    ssl on;
    listen 443;
    server_name www.foobar.com;
    ssl_certificate /tmp/nginx/server.crt;
    ssl_certificate_key /tmp/nginx/server.key;
}
{% endhighlight %}

### 4. 重启 Nginx

需要注意的是，如果采用 ```systemctl``` 重启，上述的配置不能保存在 tmp 目录下，因为 systemd 会为进程启动不同的 tmp 目录，从而导致无法查找到上述的配置文件。

这样就可以访问 [https://www.foobar.com](https://www.foobar.com)，注意需要将 ```www.foobar.com``` 添加到 ```/etc/hosts``` 中。另外，还可以加入如下代码实现 80 端口重定向到 443。

{% highlight text %}
server {
    listen 80;
    server_name www.foobar.com;
    rewrite ^(.*) https://$server_name$1 permanent;
}
{% endhighlight %}

### 5. 其它

在 2.3 中，可不去掉密钥口令，此时会在重启 nginx 输入密码，而且此时 systemctl 将会失效。

## 参考

{% highlight text %}
{% endhighlight %}
