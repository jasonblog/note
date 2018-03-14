---
title: Linux C 網絡編程
layout: post
comments: true
language: chinese
category: [program,linux]
keywords: linux,c,network
description: 簡單介紹下，在 Linux C 中進行網絡編程時常用到的一些技巧。
---

簡單介紹下，在 Linux C 中進行網絡編程時常用到的一些技巧。

<!-- more -->

## 結構體

在網絡編程時，可以看到多種結構體，例如 `struct sockaddr` 、`struct `，這些結構體的大小一致，可以直接用來相互轉換。

一般來說，`struct sockaddr` 是通用的 socket 地址，其定義如下：

{% highlight c %}
struct sockaddr {
	unsigned short sa_family;
	char sa_data[14];
};

struct sockaddr_in {
	short int            sin_family;
	unsigned short int   sin_port;
	struct in_addr       sin_addr;
	unsigned char        sin_zero[8];
};
{% endhighlight %}

其中 `struct in_addr` 就是 32 位 IP 地址。

{% highlight text %}
struct in_addr {
	unsigned long s_addr;
};
{% endhighlight %}

在使用的時候，一般使用 `struct sockaddr_in` 作為函數 (如 `bind()`) 的參數傳入，使用時再轉換為 `struct sockaddr` 即可，畢竟都是 16 個字符長。

使用示例如下：

{% highlight c %}
int sockfd;
struct sockaddr_in addr;

addr.sin_family = AF_INET;
addr.sin_port = htons(MYPORT);
addr.sin_addr.s_addr = inet_addr("192.168.0.1");
bzero(&(addr.sin_zero), 8);

sockfd = socket(AF_INET, SOCK_STREAM, 0);
bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr));
{% endhighlight %}


## getaddrinfo

`getaddrinfo()` 函數的前身是做 DNS 解析的 `gethostbyname()` 函數，現在通過 `getaddrinfo()` 可以做 DNS 解析以及 Service Name 查詢，如下是其聲明：

{% highlight c %}
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

int getaddrinfo(const char *node, const char *service,
	const struct addrinfo *hints,
	struct addrinfo **res);
void freeaddrinfo(struct addrinfo *res);
const char *gai_strerror(int errcode);
{% endhighlight %}

其中 node 可以是域名、IP，如 `"www.example.com"`；而 service 可以是 `"http"` 或者端口號，其定義在 `/etc/services` 文件中。


### 使用場景

通常有兩種使用方式：A) 建立 Server 監聽本機所有的 IP 地址；B) 作為客戶端鏈接到服務器，需要解析服務器的地址信息。

{% highlight c %}
int status;
struct addrinfo hints; /* 指定入參配置 */
struct addrinfo *res;  /* 返回結果 */

memset(&hints, 0, sizeof(hints));
hints.ai_family = AF_UNSPEC;     /* 返回IPv4以及IPV6，也可以指定 AF_INET 或 AF_INET6 */
hints.ai_socktype = SOCK_STREAM; /* 使用TCP協議 */
hints.ai_flags = AI_PASSIVE;     /* 返回結果中會填寫本地的IP地址 */

if ((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) != 0) {
	fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
	exit(1);
}
{% endhighlight %}

其中 `flags` 參數設置為 `AI_PASSIVE` 表示獲取本地 IP 地址，這樣在調用函數時，第一個參數可以指定為 `NULL` 。


關於客戶端的使用可以直接參考如下的示例。

{% highlight c %}
#define _POSIX_SOURCE

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char **argv)
{
	int status;
	struct addrinfo hints, *res, *this;
	char ipaddr[INET6_ADDRSTRLEN];

	if (argc != 2) {
		fprintf(stderr, "usage: showip hostname\n");
		return 1;
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;     /* AF_INET(IPv4) AF_INET6(IPv6) */
	hints.ai_socktype = SOCK_STREAM; /* TCP stream sockets */

	if ((status = getaddrinfo(argv[1], NULL, &hints, &res))) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		return 2;
	}

	printf("IP addresses for %s:\n\n", argv[1]);

	for(this = res; this != NULL; this = this->ai_next) {
		void *addr;
		char *ipver;

		if (this->ai_family == AF_INET) { /* IPv4 */
			struct sockaddr_in *ipv4;

			ipv4 = (struct sockaddr_in *)this->ai_addr;
			addr = &(ipv4->sin_addr);
			ipver = "IPv4";
		} else { /* IPv6 */
			struct sockaddr_in6 *ipv6;

			ipv6 = (struct sockaddr_in6 *)this->ai_addr;
			addr = &(ipv6->sin6_addr);
			ipver = "IPv6";
		}

		/* convert the IP to a string and print it */
		inet_ntop(this->ai_family, addr, ipaddr, sizeof(ipaddr));
		printf("%s:\t%s\n", ipver, ipaddr);
	}

	return 0;
}
{% endhighlight %}


## IP 地址

IP 有兩種表達方式，分別為點分十進制(Numbers and Dots Notation) 以及整形 (Binary Data) 。

對於 IPv4 實際上是一個 32bit 的整形，假設地址為 `A.B.C.D` ，那麼對應的整形是 `A<<24 + B<<16 + C<<8 + D` 。

在 MySQL 中，可以通過 `SELECT INET_ATON('192.168.1.38');` 函數將點分格式轉化為整形，然後再通過 `SELECT INET_NTOA(3232235814);` 執行反向轉換。

### IPv6

IPv6 採用的是 128 位，通常以 16 位為一組，每組之間以冒號 ':' 分割，總共分為 8 組，例如 `2001:0db8:85a3:08d3:1319:8a2e:0370:7344` 。

### C 語言使用

對於 C 語言中的地址轉換函數，也就是 BSD 網絡軟件包，可通過 `inet_addr()`、`inet_aton()` 和 `inet_ntoa()` 三個函數用於二進制地址格式與點分十進制之間的相互轉換，但是僅僅適用於 IPv4。

另外，兩個新函數 `inet_ntop()` 和 `inet_pton()` 具有相似的功能，字母 p 代表 presentation，字母 n 代表 numeric，並且同時支持 IPv4 和 IPv6 。

{% highlight c %}
#include <sys/socket.h>

//----- 將點分地址轉換成網絡字節序的IP地址
in_addr_t inet_addr(const char *strptr); /* INADDR_NONE: ERROR */
int inet_aton(const char *strptr, struct in_addr *addrptr); /* 1:OK, 0:ERROR */
int inet_pton(int family, const char *strptr, void *addrptr); /* 1:OK, 0:INVALID, -1:FAILED */

//----- 將點分地址轉換成主機字節序的IP地址
in_addr_t inet_network(const char *cp);

//----- 網絡字節序IP轉化點分十進制
char* inet_ntoa(struct in_addr inaddr);
const char* inet_ntop(int family, const void *addrptr, char *strptr, size_t len); /* NULL: ERROR */
{% endhighlight %}

`inet_addr()` 與 `inet_aton()` 不同在於其返回值為轉換後的 32 位網絡字節序二進制值，不過這樣會存在一個問題，返回的有效 IP 地址應該為 `0.0.0.0` 到 `255.255.255.255`，如果函數出錯，返回常量值 `INADDR_NONE` (一般為 `0xFFFFFFFF`)，也就是 `255.255.255.255` (IPv4 的有限廣播地址) 。

對於 `inet_ntop()` 和 `inet_pton()` 兩個函數，family 參數可以是 `AF_INET` 或者 `AF_INET6`，如果不是這兩個會返回錯誤，且將 `errno` 置為 `EAFNOSUPPORT` 。

緩衝區的大小在 `<netinet/in.h>` 中定義：

{% highlight c %}
#define INET_ADDRSTRLEN  16    /* for IPv4 dotted-decimal */
#define INET6_ADDRSTRLEN 46    /* for IPv6 hex string */
{% endhighlight %}

如果緩衝區無法容納表達格式結果 (包括空字符)，則返回一個空指針，並置 errno 為 `ENOSPC` 。

{% highlight c %}
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(void)
{
	char ip[] = "255.0.0.1";

	in_addr_t rc;  /* 一般通過 typedef uint32_t in_addr_t; 定義 */
	int status;
	struct in_addr addr;

	rc = inet_addr(ip); /* 返回網絡字節序 */
	if (rc == 0xffffffff) {
		fprintf(stderr, "Format error '%s'\n", ip);
		return -1;
	}
	fprintf(stdout, "inet_addr() ==> 0x%x\n", rc);

	rc = inet_network(ip); /* 返回主機字節序 */
	if (rc == 0xffffffff) {
		fprintf(stderr, "Format error '%s'\n", ip);
		return -1;
	}
	fprintf(stdout, "inet_network() ==> 0x%x\n", rc);

	status = inet_aton(ip, &addr);
	if (status == 0) {
		fprintf(stderr, "Format error '%s'\n", ip);
		return -1;
	}
	fprintf(stdout, "inet_aton() rc(%d) ==> 0x%x\n", status, addr.s_addr);

	/* Support IPv4(AF_INET) and IPv6(AF_INET6) */
	status = inet_pton(AF_INET, ip, &addr.s_addr);
	if (status == 0) {
		fprintf(stderr, "Format error '%s'\n", ip);
		return -1;
	}
	fprintf(stdout, "inet_aton() rc(%d) ==> 0x%x\n", status, addr.s_addr);

	char str[INET_ADDRSTRLEN];
	if(inet_ntop(AF_INET, &addr.s_addr, str, sizeof(str)) == NULL) {
		fprintf(stderr, "Format error 0x%x\n", addr.s_addr);
		return -1;
	}
	fprintf(stdout, "inet_network() ==> %s\n", str);

	return 0;
}
{% endhighlight %}

{% highlight text %}
{% endhighlight %}
