---
title: UUID 簡介
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: uuid,introduce,rfc4122,libuuid
description: UUID 的目的是讓分佈式系統中的所有元素，都能有唯一的辨識信息，而不需要通過中心節點指定，無需考慮數據庫創建時的名稱重複問題。目前最廣泛應用的 UUID 是 RFC4122 協議規範的，有時 GUID 也特指是微軟對標準 UUID 的實現，其實 RFC4122 的作者之一也是微軟員工。

---

UUID 的目的是讓分佈式系統中的所有元素，都能有唯一的辨識信息，而不需要通過中心節點指定，無需考慮數據庫創建時的名稱重複問題。

目前最廣泛應用的 UUID 是 RFC4122 協議規範的，有時 GUID 也特指是微軟對標準 UUID 的實現，其實 RFC4122 的作者之一也是微軟員工。

<!-- more -->

## 簡介

[Universally Unique IDentifier, UUID](https://en.wikipedia.org/wiki/Universally_unique_identifier) 是一個 128 位的數字，一般通過 32 個十六進制表示，一般類似 `00d460f0-ec1a-4a0f-a452-1afb4b5d1686` ，詳細標準可以參考 [RFC4122](https://tools.ietf.org/html/rfc4122) 。

在 Linux 中，可以通過 `uudigen` 生成一個 UUID ，該命令實際上調用的是 `libuuid` 庫，可以生成基於時間 (`--time`) 或者隨機 (`--random`) 的 UUID 字符串。

另外，也可以通過 `/proc/sys/kernel/random/uuid` 文件獲取，兩種方式類似。

現在很多的磁盤也是通過 UUID 區分，可以通過如下的方式查看。

{% highlight text %}
ls -l /dev/disk/by-uuid
blkid /dev/sda1
{% endhighlight %}

### 系統UUID

對於系統的 UUID 不同的平臺提供了不同的方法，例如 Linux 可以通過 `dmidecode -t system` 命令查看，也可以查看 `/sys/class/dmi/id/product_uuid`、`/sys/hypervisor/uuid` 。

## 版本演化

如上，1 個 UUID 被連字符分為五段，形式為 `8-4-4-4-12` 的 32 個字符，其中的字母是 16 進製表示，而且大小寫無關。

UUID 本身也經過了多個版本的演化，每個版本的算法都不同，其標準格式如下。

{% highlight text %}
xxxxxxxx-xxxx-Mxxx-Nxxx-xxxxxxxxxxxx
    M: 表示版本號，只會是1 2 3 4 5
    N: 只會8 9 a b
{% endhighlight %}

如下，簡單介紹各個版本的實現方法。

### V1 基於時間

通過當前時間戳、機器 MAC 地址生成，因為 MAC 地址是全球唯一的，從而間接的可以保證 UUID 全球唯一，不過它暴露了電腦的 MAC 地址和生成這個 UUID 的時間，從而一直被詬病。

可以通過 `python -c "import uuid; print uuid.uuid1()"` 生成，其中最後一部分的內容就是 MAC 地址(可通過 `ifconfig` 查看)。

### V2 DCE安全

<!-- Distributed Computing Environment, DCE --> 和基於時間的 UUID 算法相同，但會把時間戳的前 4 位置換為 POSIX 的 UID 或 GID，不過這個版本在 UUID 規範中沒有明確指定，基本不會實現。

### V3 基於命名空間

由用戶指定一個命名空間和一個具體的字符串，然後通過 MD5 散列來生成 UUID 。不過這個版本按照規範描述，主要是是為了向後兼容，所以也很少用到。

可以通過 `python -c "import uuid; print uuid.uuid3(uuid.NAMESPACE_DNS, 'foobar')"` 生成。

### V4 基於隨機數

根據隨機數或者偽隨機數生成 UUID ，這個版本也是有意或者無意之中使用最多的。

可以通過 `python -c "import uuid; print uuid.uuid4()"` 生成。

### V5 基於名字空間

其實和版本 3 類似，不過散列函數換成了 SHA1 。

可以通過 `python -c "import uuid; print uuid.uuid5(uuid.NAMESPACE_DNS, 'foobar')"` 生成。

## C語言實現

C 中一般使用的是 libuuid 這個庫，一個跨平臺開源的 uuid 操作庫，一般動態庫都存在，需要安裝下頭文件。

在 uuid.h 這個頭文件中，定義了 `uuid_t` 類型，實際上也就是 `typedef unsigned char uuid_t[16];` ，常見的函數聲明如下：

{% highlight c %}
#include <uuid.h>

void uuid_generate(uuid_t out);
void uuid_generate_random(uuid_t out);
void uuid_generate_time(uuid_t out);
int uuid_generate_time_safe(uuid_t out);

int uuid_compare(uuid_t uu1, uuid_t uu2);
void uuid_copy(uuid_t dst, uuid_t src);
void uuid_clear(uuid_t uu);
int uuid_is_null(uuid_t uu);

int uuid_parse(char *in, uuid_t uu);             /* 將in指向的字符串解析到uu */
void uuid_unparse(uuid_t uu, char *out);         /* 將uu內容解析為字符串，保存到out數組 */
void uuid_unparse_upper(uuid_t uu, char *out);
void uuid_unparse_lower(uuid_t uu, char *out);
time_t uuid_time(uuid_t uu, struct timeval *ret_tv); /* 將基於時間創建的uu在的時間部分解析出來 */
{% endhighlight %}

示例代碼如下。

{% highlight c %}
#include <stdio.h>
#include <uuid/uuid.h>

/* gcc uuid.c -luuid -o uuid */

#define UUID_STR_LEN    37

int main()
{
	int i, rc;
	uuid_t uuids[4];
	char buffer[UUID_STR_LEN];
	struct timeval tv;

	uuid_generate(uuids[0]);
	uuid_generate_random(uuids[1]);
	uuid_generate_time(uuids[2]);
	rc = uuid_generate_time_safe(uuids[3]);
	printf("uuid_generate_time_safe() rc = %d\n", rc);

	for(i = 0; i < (int)sizeof(uuids)/(int)sizeof(uuids[0]); ++i) {
		uuid_unparse(uuids[i], buffer);
		printf("uuids[%d] \t\t%s\n", i, buffer);
	}

	uuid_time(uuids[2],&tv);
	printf("Get tv sec:%lx  usec:%lx\n", tv.tv_sec, tv.tv_usec);

	return 0;
}
{% endhighlight %}


{% highlight text %}
{% endhighlight %}
