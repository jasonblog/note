---
title: 50 行 Python 实现 Bitcoin
layout: post
comments: true
language: chinese
category: [misc,python]
keywords: bitcoin,python,区块链,blockchain
description: 区块链近年来被越炒越热，是一串使用密码学方法相关联产生的数据块，每一个数据块中包含了一次比特币网络交易的信息，用于验证其信息的有效性和生成下一个区块。而比特币的交易过程中，就使用了区块链技术。
---

区块链近年来被越炒越热，是一串使用密码学方法相关联产生的数据块，每一个数据块中包含了一次比特币网络交易的信息，用于验证其信息的有效性和生成下一个区块。

而比特币的交易过程中，就使用了区块链技术。

<!-- more -->

![bitcoin logo]({{ site.url }}/images/misc/bitcoin-logo.jpg "bitcoin logo"){: .pull-center width="70%" }

## 简介

简单来说，区块链是一个公共数据库，数据被存储在被称作 "块" 的容器中，完成这一过程中，块将被添加到链上，该链包含了过去产生的数据。

这些数据可以是任何类型，对于比特币或者其他一些数字货币来说，该数据就是一组交易。区块链技术带来了新的，完全数字化的货币，它们不是由中央机构发行或管理的。<!--比特币也诞生了以太坊这种新型分布式计算技术，它引入了有趣的概念，如智能合同。-->

下面将用不到 50 行的 Python 代码来编写一个小型的区块链，暂且叫它为 SnakeCoin 。


## 块结构

首先，需要定义块的结构。

在块链中，每个块都存储有时间戳和可选的索引，这里会同时存储两者；同时为了确保整个块链的完整性，每个块会计算一个 Hash 值。

每个块的 Hash 值是块的索引、时间戳、数据以及前一个块的哈希散列的加密散列，着跟比特币相似，其中的数据可以是任何类型。

块的代码如下：

{% highlight python %}
import hashlib as hasher

class Block:
	def __init__(self, index, timestamp, data, previous_hash):
		self.index = index
		self.timestamp = timestamp
		self.data = data
		self.previous_hash = previous_hash
		self.hash = self.hash_block()

	def hash_block(self):
		sha = hasher.sha256()
		sha.update(str(self.index) +
			   str(self.timestamp) +
			   str(self.data) +
			   str(self.previous_hash))
		return sha.hexdigest()
{% endhighlight %}

## 创建区块链

有了块结构后，接着是创建区块链，也就是需要向链中添加块。如前所述，每个块都需要上一个块的信息，那么区块链的第一个区块是如何到达那里的呢？

实际上，第一个块，或者说起源块，是一个特殊的块，一般都是通过手动添加，或者有独特的逻辑允许添加。如下，将创建一个函数返回一个起源块，这个块的索引是 0 。

{% highlight python %}
import datetime as date

def create_genesis_block():
	# Manually construct a block with
	# index zero and arbitrary previous hash
	return Block(0, date.datetime.now(), "Genesis Block", "0")
{% endhighlight %}


## 生成函数

现在已经创建好了起源块，接下来创建一个函数，用于在区块链中生成后续的块。

该函数把链中的前一个块作为参数，添加一些适当的数据，然后创建新的块数据。当新的块哈希信息来自前面的块时，区块链的完整性会随着每个新块而增加，通过哈希值确保整个链条不被修改。

这一系列的散列可以作为加密的证据，确保一旦将块添加到区块链，它就不能被替换或删除。

{% highlight python %}
def next_block(last_block):
	this_index = last_block.index + 1
	this_timestamp = date.datetime.now()
	this_data = "Hey! I'm block " + str(this_index)
	this_hash = last_block.hash
	return Block(this_index, this_timestamp, this_data, this_hash)
{% endhighlight %}

## 创建区块链

到此为止，大部分工作已经完成，现在可以创建区块链了。

在这里的示例中，区块链本身是一个简单的 Python 列表，第一个元素是起源块，然后通过 for 循环生成 20 个新块。

{% highlight python %}
# Create the blockchain and add the genesis block
blockchain = [create_genesis_block()]
previous_block = blockchain[0]

# How many blocks should we add to the chain
# after the genesis block
num_of_blocks_to_add = 20

# Add blocks to the chain
for i in range(0, num_of_blocks_to_add):
	block_to_add = next_block(previous_block)
	blockchain.append(block_to_add)
	previous_block = block_to_add
	# Tell everyone about it!
	print "Block #{} has been added to the blockchain!".format(block_to_add.index)
	print "Hash: {}\n".format(block_to_add.hash)
{% endhighlight %}

接下来，直接运行上述的程序即可，可以直接参考 [github bitcoin.py]( {{ site.example_repository }}/python/bitcoin.py) 。

{% highlight text %}
Block #1 has been added to the blockchain!
Hash: 97580eeedd90f7d687646f91e340f9896caf48737a0942d9e87daaff05920d7e

Block #2 has been added to the blockchain!
Hash: a7719fda704a959faf2c42e303086607878eccfbb3429cdb2078cf674d6567da

Block #3 has been added to the blockchain!
Hash: 2a0cbd3f1df1ec24407c6eb214356544c7bff9dcddab4da5da0897eb6292dc3d

Block #4 has been added to the blockchain!
Hash: 0d9508be3c57755d0bc940852633fe7ea4f412393201f69b295fbd824cbfbd19

Block #5 has been added to the blockchain!
Hash: da99f8294b6fd78e33112233d0603662f982cae539bac61dce89b2d7cb67d2a1

Block #6 has been added to the blockchain!
Hash: db3c50bd0a36227331d3da123ef1c4c6e4737c79d41efb472d4876129ac14b56

Block #7 has been added to the blockchain!
Hash: c97f2874b5df5e96ef5abff9e32c3af011fd0ae070782c202b3da1708e0efbee

Block #8 has been added to the blockchain!
Hash: 730e97249c798b571f06167188a6b0bbb8f8e66bf4d74c9c433f55e8a3b5914b

Block #9 has been added to the blockchain!
Hash: 13f591846b06dc16d46871baec39e9e31db73617460f61efa69101bc76629ddb

Block #10 has been added to the blockchain!
Hash: 54044d982904d423c05355c194dafd3c3d085f2b8cde0d0d465ad174bfa68a70

Block #11 has been added to the blockchain!
Hash: 5670a2cc01cce7f7861efcd871209f079aa5c46e31fd2b156a6d4ead64111ad8

Block #12 has been added to the blockchain!
Hash: 4dd31f0a50785a4b47bf7fef74b65dfb13a2c33d667443c689f9349b57a840f9

Block #13 has been added to the blockchain!
Hash: 97cc6ea22ff03497751649f3d3acb86403515f8a9577ae12d17171de2cf1f49d

Block #14 has been added to the blockchain!
Hash: 4bb7f16691d9c1549a0c0f66e26b6a31e097cca108359e54a529941264ca393e

Block #15 has been added to the blockchain!
Hash: 816f82145a9de3210b95c396542399745842f2067d0308b731c88763167265b0

Block #16 has been added to the blockchain!
Hash: 599dd2fddec9d33cfe720d641315dbd17bc0f407fa8aaed470a837823a0baf5b

Block #17 has been added to the blockchain!
Hash: ea148ae80b26baaee7cb7d0676595decad66827b8d4f351a284f3715757dc88d

Block #18 has been added to the blockchain!
Hash: 91f0217a7dd327a4a3275f28e0b5fd45db045056cfd7ad90c27ed9422d462397

Block #19 has been added to the blockchain!
Hash: 346b2b19462ac367a67d1634bdccae3b45898ed97a02fa71d24b72afa7b341cc

Block #20 has been added to the blockchain!
Hash: 2336ac92b02df637fa85513593ce25ff8aa94dddbea296d84e8fc322412bd995
{% endhighlight %}




OK，这就是区块链。

## 其它

这就是 SnakeCoin 要提供的所有东西，为了使 SnakeCoin 规模达到今天生产区块链的规模，必须添加更多的功能，比如服务器层，以跟踪多台机器上的链变化，以及在给定的时间段内限制添加的块数量的工作算法。


## 参考

如果想了解更多的技术信息，可以在这里查看原始的 [比特币白皮书](https://bitcoin.org/bitcoin.pdf) 。

<!--
https://en.bitcoin.it/wiki/Proof_of_work
-->

{% highlight text %}
{% endhighlight %}
