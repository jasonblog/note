---
title: Hash 函数简介
layout: post
comments: true
language: chinese
category: [program,misc]
keywords: 
description:
---


<!-- more -->

## MurmurHash

实际上常见的 Hash 函数包括了 MD5、SHA1、SHA256 这类的，这些算法通常讲究安全性，而实际上很多场景下对安全性不敏感，例如这里的 Murmur ，一般要快几十倍。

这一算法在 Redis、Memcached、Cassandra、HBase、Lucene 上都有使用，默默无名却基本已经一统江湖。其中 `mur` 这个名字来源于其核心算法的处理方式，全称是 MUltiply and Rotate 的意思，因为算法的核心就是不断的 `x *= m; x = rotate_left(x,r);` 。

<!--
那Java自己的String的hashCode()呢？ 用的是Horner法则， s[0]*31^(n-1) + s[1]*31^(n-2) + ... + s[n-1]

实现上是循环原哈希值×31再加下一个Char的值，算法及其复杂度简单一目了然，连我都能看懂。

for (int i = 0; i < str.length(); i++) { hash = 31*hash + str.charAt[i]; } 

注意，int会溢出成负数，再溢成正数，比如"abcde"是 92599395， abcdef"是 -1424385949， "abcdefgh" 是 1259673732

Eclipse的自动代码生成的hashCode()函数也是类似的，循环原哈希值×31，再加下一个属性的hashCode()值。

31有个很好的特性，就是用移位和减法来代替乘法，可以得到更好的性能：31*i==(i<<5)-i。现在的VM可以自动完成这种优化。 Java自己的HashMap，会在调用Key的hashCode()得到一个数值后，用以下算法再hash一次，免得Key自己实现的hashCode()质量太差。

static int hash(int h) {
    h ^= (h >>> 20) ^ (h >>> 12);
    return h ^ (h >>> 7) ^ (h >>> 4);
}

那为什么Cassandra们不选简单直白的Horner法则呢？

我猜原因之一是它的性能，有个评测用js来跑的，还是murmur好些。

再猜原因之二它的变化不够激烈，比如"abc"是96354， "abd"就比它多1。而用 murmur"abc"是1118836419，"abd"是413429783。像在Jedis里，它的几个Shard节点的名字就叫做shard-1,shard-2，shard-3而已，hash这几个字符串，murmur的结果还能均匀的落在一致性哈希环上，用Honer法则就不行了。
-->























<!--
DJBHash()
 - hash += (hash << 5) + (*str++);
 + hash = ((hash << 5) + hash)  + (*str++);

https://www.byvoid.com/zhs/blog/string-hash-compare
http://www.partow.net/programming/hashfunctions/index.html

https://github.com/davidar/c-hashtable
https://github.com/larsendt/hashtable
-->

## 参考

[Murmur3 C](https://github.com/PeterScott/murmur3) C 语言实现的 Murmur3 哈希算法。

{% highlight text %}
{% endhighlight %}
