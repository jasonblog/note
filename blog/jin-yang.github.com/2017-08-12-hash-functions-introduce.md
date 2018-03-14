---
title: Hash 函數簡介
layout: post
comments: true
language: chinese
category: [program,misc]
keywords: 
description:
---


<!-- more -->

## MurmurHash

實際上常見的 Hash 函數包括了 MD5、SHA1、SHA256 這類的，這些算法通常講究安全性，而實際上很多場景下對安全性不敏感，例如這裡的 Murmur ，一般要快幾十倍。

這一算法在 Redis、Memcached、Cassandra、HBase、Lucene 上都有使用，默默無名卻基本已經一統江湖。其中 `mur` 這個名字來源於其核心算法的處理方式，全稱是 MUltiply and Rotate 的意思，因為算法的核心就是不斷的 `x *= m; x = rotate_left(x,r);` 。

<!--
那Java自己的String的hashCode()呢？ 用的是Horner法則， s[0]*31^(n-1) + s[1]*31^(n-2) + ... + s[n-1]

實現上是循環原哈希值×31再加下一個Char的值，算法及其複雜度簡單一目瞭然，連我都能看懂。

for (int i = 0; i < str.length(); i++) { hash = 31*hash + str.charAt[i]; } 

注意，int會溢出成負數，再溢成正數，比如"abcde"是 92599395， abcdef"是 -1424385949， "abcdefgh" 是 1259673732

Eclipse的自動代碼生成的hashCode()函數也是類似的，循環原哈希值×31，再加下一個屬性的hashCode()值。

31有個很好的特性，就是用移位和減法來代替乘法，可以得到更好的性能：31*i==(i<<5)-i。現在的VM可以自動完成這種優化。 Java自己的HashMap，會在調用Key的hashCode()得到一個數值後，用以下算法再hash一次，免得Key自己實現的hashCode()質量太差。

static int hash(int h) {
    h ^= (h >>> 20) ^ (h >>> 12);
    return h ^ (h >>> 7) ^ (h >>> 4);
}

那為什麼Cassandra們不選簡單直白的Horner法則呢？

我猜原因之一是它的性能，有個評測用js來跑的，還是murmur好些。

再猜原因之二它的變化不夠激烈，比如"abc"是96354， "abd"就比它多1。而用 murmur"abc"是1118836419，"abd"是413429783。像在Jedis裡，它的幾個Shard節點的名字就叫做shard-1,shard-2，shard-3而已，hash這幾個字符串，murmur的結果還能均勻的落在一致性哈希環上，用Honer法則就不行了。
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

## 參考

[Murmur3 C](https://github.com/PeterScott/murmur3) C 語言實現的 Murmur3 哈希算法。

{% highlight text %}
{% endhighlight %}
