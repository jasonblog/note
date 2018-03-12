---
title: 金融学笔记
layout: post
comments: true
language: chinese
category: [misc]
keywords: 金融学
description: 可汗学院的公开课《金融学》笔记。
---

可汗学院的公开课《金融学》笔记。

<!-- more -->

### 1. 利息介绍

分为了单利、复利。

{% highlight text %}
100(1+10%*N)   Simple Interest
100(1+10%)^N   Compound Interest
{% endhighlight %}

### 2. 现值 (Present Value)

对折现率 (Discount Rate) 的假设，决定了如何选择策略，例如，有如下三种情况 I) 现在给100元；II) 两年后给110元 III) 现在给20，明年给50，后年给35：

<!--
Net Present Value  净现金量
Discounted Cash Value  净现值
-->

{% highlight text %}
I) 100  II) 110/1.05^2=99.77  III) 20+50/1.05+35/1.05^2=99.36     A) 一直为5%：          
I) 100  II) 110/1.02^2=105.73 III) 20+50/1.02+35/1.02^2=102.66    B) 一直为2%：          
I) 100  II) 110/1.05^2=99.77  III) 20+50/1.01+35/1.05^2=101.25    C) 第一年1%，第二年5%：
{% endhighlight %}

### 3. 资产负债表

也就是 Balance Sheet ，常见的有如下的三种概念：

* Assets 资产，可以在将来带来经济利益的东西，如钱可以用来买东西、雇佣工人、度假等，再如房子可以居住；
* Liability 负债，对别人的经济负担或者义务，需要在将来偿还的；
* Equity 权益，所有的资产在偿清债务之后剩余的。

通常也就是 `L + E = A`，假设现在有$250k，房价为1000k。

{% highlight text %}
    L(0) + E($250k) = A($250k)    A）贷款买房前
L($750k) + E($250k) = A($1m)      B）贷款后，银行说，你的信用等级不错，那就借给你吧；我就保留了一个IOU借据在银行。
L($750k) + E($250k) = A(a house)  C）买房后，此时有了一个法律保证的房契，银行的借据，以及虚无的权益。
{% endhighlight %}

{% highlight text %}
{% endhighlight %}
