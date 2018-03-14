---
title: 50 行 Python 實現 Bitcoin
layout: post
comments: true
language: chinese
category: [misc,python]
keywords: bitcoin,python,區塊鏈,blockchain
description: 區塊鏈近年來被越炒越熱，是一串使用密碼學方法相關聯產生的數據塊，每一個數據塊中包含了一次比特幣網絡交易的信息，用於驗證其信息的有效性和生成下一個區塊。而比特幣的交易過程中，就使用了區塊鏈技術。
---

區塊鏈近年來被越炒越熱，是一串使用密碼學方法相關聯產生的數據塊，每一個數據塊中包含了一次比特幣網絡交易的信息，用於驗證其信息的有效性和生成下一個區塊。

而比特幣的交易過程中，就使用了區塊鏈技術。

<!-- more -->

![bitcoin logo]({{ site.url }}/images/misc/bitcoin-logo.jpg "bitcoin logo"){: .pull-center width="70%" }

## 簡介

簡單來說，區塊鏈是一個公共數據庫，數據被存儲在被稱作 "塊" 的容器中，完成這一過程中，塊將被添加到鏈上，該鏈包含了過去產生的數據。

這些數據可以是任何類型，對於比特幣或者其他一些數字貨幣來說，該數據就是一組交易。區塊鏈技術帶來了新的，完全數字化的貨幣，它們不是由中央機構發行或管理的。<!--比特幣也誕生了以太坊這種新型分佈式計算技術，它引入了有趣的概念，如智能合同。-->

下面將用不到 50 行的 Python 代碼來編寫一個小型的區塊鏈，暫且叫它為 SnakeCoin 。


## 塊結構

首先，需要定義塊的結構。

在塊鏈中，每個塊都存儲有時間戳和可選的索引，這裡會同時存儲兩者；同時為了確保整個塊鏈的完整性，每個塊會計算一個 Hash 值。

每個塊的 Hash 值是塊的索引、時間戳、數據以及前一個塊的哈希散列的加密散列，著跟比特幣相似，其中的數據可以是任何類型。

塊的代碼如下：

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

## 創建區塊鏈

有了塊結構後，接著是創建區塊鏈，也就是需要向鏈中添加塊。如前所述，每個塊都需要上一個塊的信息，那麼區塊鏈的第一個區塊是如何到達那裡的呢？

實際上，第一個塊，或者說起源塊，是一個特殊的塊，一般都是通過手動添加，或者有獨特的邏輯允許添加。如下，將創建一個函數返回一個起源塊，這個塊的索引是 0 。

{% highlight python %}
import datetime as date

def create_genesis_block():
	# Manually construct a block with
	# index zero and arbitrary previous hash
	return Block(0, date.datetime.now(), "Genesis Block", "0")
{% endhighlight %}


## 生成函數

現在已經創建好了起源塊，接下來創建一個函數，用於在區塊鏈中生成後續的塊。

該函數把鏈中的前一個塊作為參數，添加一些適當的數據，然後創建新的塊數據。當新的塊哈希信息來自前面的塊時，區塊鏈的完整性會隨著每個新塊而增加，通過哈希值確保整個鏈條不被修改。

這一系列的散列可以作為加密的證據，確保一旦將塊添加到區塊鏈，它就不能被替換或刪除。

{% highlight python %}
def next_block(last_block):
	this_index = last_block.index + 1
	this_timestamp = date.datetime.now()
	this_data = "Hey! I'm block " + str(this_index)
	this_hash = last_block.hash
	return Block(this_index, this_timestamp, this_data, this_hash)
{% endhighlight %}

## 創建區塊鏈

到此為止，大部分工作已經完成，現在可以創建區塊鏈了。

在這裡的示例中，區塊鏈本身是一個簡單的 Python 列表，第一個元素是起源塊，然後通過 for 循環生成 20 個新塊。

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

接下來，直接運行上述的程序即可，可以直接參考 [github bitcoin.py]( {{ site.example_repository }}/python/bitcoin.py) 。

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




OK，這就是區塊鏈。

## 其它

這就是 SnakeCoin 要提供的所有東西，為了使 SnakeCoin 規模達到今天生產區塊鏈的規模，必須添加更多的功能，比如服務器層，以跟蹤多臺機器上的鏈變化，以及在給定的時間段內限制添加的塊數量的工作算法。


## 參考

如果想了解更多的技術信息，可以在這裡查看原始的 [比特幣白皮書](https://bitcoin.org/bitcoin.pdf) 。

<!--
https://en.bitcoin.it/wiki/Proof_of_work
-->

{% highlight text %}
{% endhighlight %}
