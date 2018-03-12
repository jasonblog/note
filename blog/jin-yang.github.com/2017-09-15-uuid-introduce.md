---
title: UUID 简介
layout: post
comments: true
language: chinese
category: [linux,misc]
keywords: uuid,introduce,rfc4122,libuuid
description: UUID 的目的是让分布式系统中的所有元素，都能有唯一的辨识信息，而不需要通过中心节点指定，无需考虑数据库创建时的名称重复问题。目前最广泛应用的 UUID 是 RFC4122 协议规范的，有时 GUID 也特指是微软对标准 UUID 的实现，其实 RFC4122 的作者之一也是微软员工。

---

UUID 的目的是让分布式系统中的所有元素，都能有唯一的辨识信息，而不需要通过中心节点指定，无需考虑数据库创建时的名称重复问题。

目前最广泛应用的 UUID 是 RFC4122 协议规范的，有时 GUID 也特指是微软对标准 UUID 的实现，其实 RFC4122 的作者之一也是微软员工。

<!-- more -->

## 简介

[Universally Unique IDentifier, UUID](https://en.wikipedia.org/wiki/Universally_unique_identifier) 是一个 128 位的数字，一般通过 32 个十六进制表示，一般类似 `00d460f0-ec1a-4a0f-a452-1afb4b5d1686` ，详细标准可以参考 [RFC4122](https://tools.ietf.org/html/rfc4122) 。

在 Linux 中，可以通过 `uudigen` 生成一个 UUID ，该命令实际上调用的是 `libuuid` 库，可以生成基于时间 (`--time`) 或者随机 (`--random`) 的 UUID 字符串。

另外，也可以通过 `/proc/sys/kernel/random/uuid` 文件获取，两种方式类似。

现在很多的磁盘也是通过 UUID 区分，可以通过如下的方式查看。

{% highlight text %}
ls -l /dev/disk/by-uuid
blkid /dev/sda1
{% endhighlight %}

### 系统UUID

对于系统的 UUID 不同的平台提供了不同的方法，例如 Linux 可以通过 `dmidecode -t system` 命令查看，也可以查看 `/sys/class/dmi/id/product_uuid`、`/sys/hypervisor/uuid` 。

## 版本演化

如上，1 个 UUID 被连字符分为五段，形式为 `8-4-4-4-12` 的 32 个字符，其中的字母是 16 进制表示，而且大小写无关。

UUID 本身也经过了多个版本的演化，每个版本的算法都不同，其标准格式如下。

{% highlight text %}
xxxxxxxx-xxxx-Mxxx-Nxxx-xxxxxxxxxxxx
    M: 表示版本号，只会是1 2 3 4 5
    N: 只会8 9 a b
{% endhighlight %}

如下，简单介绍各个版本的实现方法。

### V1 基于时间

通过当前时间戳、机器 MAC 地址生成，因为 MAC 地址是全球唯一的，从而间接的可以保证 UUID 全球唯一，不过它暴露了电脑的 MAC 地址和生成这个 UUID 的时间，从而一直被诟病。

可以通过 `python -c "import uuid; print uuid.uuid1()"` 生成，其中最后一部分的内容就是 MAC 地址(可通过 `ifconfig` 查看)。

### V2 DCE安全

<!-- Distributed Computing Environment, DCE --> 和基于时间的 UUID 算法相同，但会把时间戳的前 4 位置换为 POSIX 的 UID 或 GID，不过这个版本在 UUID 规范中没有明确指定，基本不会实现。

### V3 基于命名空间

由用户指定一个命名空间和一个具体的字符串，然后通过 MD5 散列来生成 UUID 。不过这个版本按照规范描述，主要是是为了向后兼容，所以也很少用到。

可以通过 `python -c "import uuid; print uuid.uuid3(uuid.NAMESPACE_DNS, 'foobar')"` 生成。

### V4 基于随机数

根据随机数或者伪随机数生成 UUID ，这个版本也是有意或者无意之中使用最多的。

可以通过 `python -c "import uuid; print uuid.uuid4()"` 生成。

### V5 基于名字空间

其实和版本 3 类似，不过散列函数换成了 SHA1 。

可以通过 `python -c "import uuid; print uuid.uuid5(uuid.NAMESPACE_DNS, 'foobar')"` 生成。

## C语言实现

C 中一般使用的是 libuuid 这个库，一个跨平台开源的 uuid 操作库，一般动态库都存在，需要安装下头文件。

在 uuid.h 这个头文件中，定义了 `uuid_t` 类型，实际上也就是 `typedef unsigned char uuid_t[16];` ，常见的函数声明如下：

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

int uuid_parse(char *in, uuid_t uu);             /* 将in指向的字符串解析到uu */
void uuid_unparse(uuid_t uu, char *out);         /* 将uu内容解析为字符串，保存到out数组 */
void uuid_unparse_upper(uuid_t uu, char *out);
void uuid_unparse_lower(uuid_t uu, char *out);
time_t uuid_time(uuid_t uu, struct timeval *ret_tv); /* 将基于时间创建的uu在的时间部分解析出来 */
{% endhighlight %}

示例代码如下。

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
