---
title: 金融學筆記
layout: post
comments: true
language: chinese
category: [misc]
keywords: 金融學
description: 可汗學院的公開課《金融學》筆記。
---

可汗學院的公開課《金融學》筆記。

<!-- more -->

### 1. 利息介紹

分為了單利、複利。

{% highlight text %}
100(1+10%*N)   Simple Interest
100(1+10%)^N   Compound Interest
{% endhighlight %}

### 2. 現值 (Present Value)

對摺現率 (Discount Rate) 的假設，決定了如何選擇策略，例如，有如下三種情況 I) 現在給100元；II) 兩年後給110元 III) 現在給20，明年給50，後年給35：

<!--
Net Present Value  淨現金量
Discounted Cash Value  淨現值
-->

{% highlight text %}
I) 100  II) 110/1.05^2=99.77  III) 20+50/1.05+35/1.05^2=99.36     A) 一直為5%：          
I) 100  II) 110/1.02^2=105.73 III) 20+50/1.02+35/1.02^2=102.66    B) 一直為2%：          
I) 100  II) 110/1.05^2=99.77  III) 20+50/1.01+35/1.05^2=101.25    C) 第一年1%，第二年5%：
{% endhighlight %}

### 3. 資產負債表

也就是 Balance Sheet ，常見的有如下的三種概念：

* Assets 資產，可以在將來帶來經濟利益的東西，如錢可以用來買東西、僱傭工人、度假等，再如房子可以居住；
* Liability 負債，對別人的經濟負擔或者義務，需要在將來償還的；
* Equity 權益，所有的資產在償清債務之後剩餘的。

通常也就是 `L + E = A`，假設現在有$250k，房價為1000k。

{% highlight text %}
    L(0) + E($250k) = A($250k)    A）貸款買房前
L($750k) + E($250k) = A($1m)      B）貸款後，銀行說，你的信用等級不錯，那就借給你吧；我就保留了一個IOU借據在銀行。
L($750k) + E($250k) = A(a house)  C）買房後，此時有了一個法律保證的房契，銀行的借據，以及虛無的權益。
{% endhighlight %}

{% highlight text %}
{% endhighlight %}
